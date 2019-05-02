#include "cameraframebuffer.h"
#include <iostream>

CameraFrameBuffer::CameraFrameBuffer()
{
}

CameraFrameBuffer::~CameraFrameBuffer()
{
    print();

    for (CameraFrame *frame: frames) {
        delete frame;
    }

    frames.clear();
}

void CameraFrameBuffer::initializeFrames(unsigned n)
{
    for (unsigned i = 0; i < n; ++i) {
        frames.push_back(newFrame(i));
    }

    connectFrames();
    print();
    check();
}

void CameraFrameBuffer::connectFrames()
{
    // Connect frames, except current and final
    for (unsigned i = 0; i < frames.size() - 1; ++i) {
        frames[i]->setNext(frames[i + 1]);
        frames[i + 1]->setLast(frames[i]);
    }

    // Current frame, next is always final
    current = frames.back();
    frames.back()->setNext(frames.front());

    // Final frame, last is always current
    final = frames.front();
    frames.front()->setLast(frames.back());
}

CameraFrame *CameraFrameBuffer::getCurrent()
{
    std::lock_guard<std::mutex> lock(mutex);
    return getLastValid(current);
}

CameraFrame *CameraFrameBuffer::getNth(unsigned nth)
{
    std::lock_guard<std::mutex> lock(mutex);

    CameraFrame *frame = current;
    for (unsigned i = 0; i < nth; ++i) {
        frame = frame->getLast();
    }

    return getLastValid(frame);
}

std::vector<CameraFrame *> CameraFrameBuffer::getCurrentN(unsigned n)
{
    if (n >= frames.size()) {
        n = frames.size() - 1;
    }

    std::vector<CameraFrame *> taken_frames;
    CameraFrame *frame = getCurrent();
    if (!frame) {
        return taken_frames;
    }
    taken_frames.push_back(frame);

    for (unsigned i = 1; i < n; ++i) {
        CameraFrame *frame = getLast(taken_frames.back());
        if (!frame) {
            return taken_frames;
        }
        taken_frames.push_back(frame);
    }

    return taken_frames;
}


CameraFrame *CameraFrameBuffer::getFinal(void)
{
    std::lock_guard<std::mutex> lock(mutex);
    return getNextValid(final);
}

CameraFrame *CameraFrameBuffer::getNext(CameraFrame *frame)
{
    std::lock_guard<std::mutex> lock(mutex);
    return getNextValid(frame->getNext());
}

std::vector<CameraFrame *> CameraFrameBuffer::getNextWaitN(unsigned n)
{
    if (n >= frames.size()) {
        n = frames.size() - 1;
    }

    std::vector<CameraFrame *> taken_frames;
    CameraFrame *frame = getCurrent();
    if (!frame) {
        return taken_frames;
    }
    taken_frames.push_back(frame);

    for (unsigned i = 1; i < n; ++i) {
        taken_frames.push_back(getNextWait(taken_frames.back()));
    }

    return taken_frames;
}

CameraFrame *CameraFrameBuffer::getNextWait(CameraFrame *frame)
{
    CameraFrame *frameNext;

    while (true) {
        frameNext = getNext(frame);
        if (frameNext) {
            break;
        } else {
            std::unique_lock<std::mutex> lck(mutex);
            new_frame_ready.wait(lck);
        }
    }

    return frameNext;
}

CameraFrame *CameraFrameBuffer::getLast(CameraFrame *frame)
{
    std::lock_guard<std::mutex> lock(mutex);
    return getLastValid(frame->getLast());
}

void CameraFrameBuffer::release(CameraFrame *frame)
{
    std::lock_guard<std::mutex> lock(mutex);
    frame->release();
}

void CameraFrameBuffer::waitForNew()
{
    std::unique_lock<std::mutex> lck(mutex);
    unsigned index = current->getId();

    new_frame_ready.wait(lck, [&]{ return index != current->getId(); });
}

void CameraFrameBuffer::newFrameReady()
{
    new_frame_ready.notify_all();
}

CameraFrame *CameraFrameBuffer::getNextValid(CameraFrame *frame)
{
    if (current == frame) {
        return nullptr;
    }

    CameraFrame *next = frame;
    while (!next->isValid()) {
        next = next->getNext();
        if (next == frame) {
            return nullptr;
        }
    }

    next->hold();
    return next;
}

CameraFrame *CameraFrameBuffer::getLastValid(CameraFrame *frame)
{
    if (final == frame) {
        return nullptr;
    }

    CameraFrame *last = frame;
    while (!last->isValid()) {
        last = last->getLast();
        if (last == frame) {
            return nullptr;
        }
    }

    last->hold();
    return last;
}

CameraFrame *CameraFrameBuffer::getNewCurrent()
{
    std::lock_guard<std::mutex> lock(mutex);

    CameraFrame *frame = nullptr;
    // Try if skipped frames are available
    for (CameraFrame *f: skipped) {
        if (f->isValid() && !f->isUsed()) {
            skipped.erase(f);
            frame = f;
            break;
        }
    }

    // If not, find next available frame
    if (frame == nullptr) {
        frame = current;
        while (true) {
            frame = frame->getNext();

            if (frame == current) {
                return nullptr;
            }

            if ((frame->isValid() && !frame->isUsed()) || frame->getTimestamp() == 0) {
                break;
            } else {
                skipped.insert(frame); // Save the skipped ones
            }
        }

        // Update final. If available frame is skipped one, do not move final.
        final = frame->getNext();
    }

    current->setNext(frame);
    frame->setLast(current);

    // Set new current frame
    current = frame;

    // Keep the circle
    current->setNext(final);
    final->setLast(current);

    print();
    frame->updateTimestamp();
    frame->setValid(false);
    return frame;
}

void CameraFrameBuffer::setNewReady(CameraFrame *frame)
{
    std::lock_guard<std::mutex> lock(mutex);
    frame->setValid(true);
    newFrameReady();
}

CameraFrame *CameraFrameBuffer::getNextIndex(unsigned &index)
{
    index++;

    if (index >= frames.size()) {
        index = 0;
    }

    return frames.at(index);
}

void CameraFrameBuffer::print()
{
    int counter = 0;
    CameraFrame *frame = current;
    do {
        frame = frame->getNext();
        if (frame == current) {
            std::cout << "{" << frame->getId() << "}" << std::endl;
        } else if (frame == final) {
        std::cout << "[" << frame->getId() << "]" << " -> ";
        } else {
            std::cout << frame->getId() << " -> ";
        }
        counter++;
    } while (frame != current);

    std::cout << __PRETTY_FUNCTION__ << ": Total frames = " << counter << std::endl;

    for (CameraFrame *frame: frames) {
        frame->print();
    }
}

void CameraFrameBuffer::check()
{
    std::lock_guard<std::mutex> lock(mutex);
    unsigned counter = 0;
    CameraFrame *frame = current;
    do {
        frame = frame->getNext();
        counter++;
    } while (frame != current);

    if (counter > frames.size()) {
        std::cerr << __PRETTY_FUNCTION__ << ": Error in frame pointers!" << std::endl;
    }

    if (current->next != final || final->last != current) {
        std::cerr << __PRETTY_FUNCTION__ << ": Error in current final!" << std::endl;
    }

    frame = final;
    do {
        std::cerr << __PRETTY_FUNCTION__ << ": " << frame->getId() << " - " << frame->getTimestamp() << std::endl;
        if (frame->getTimestamp() > frame->getNext()->getTimestamp()) {
            std::cerr << __PRETTY_FUNCTION__ << ": Error in timestamp!" << std::endl;
        }
        frame = frame->getNext();
    } while (frame != current);
    std::cerr << __PRETTY_FUNCTION__ << ": " << frame->getId() << " - " << frame->getTimestamp() << std::endl;

    for (CameraFrame *frame: frames) {
        if (frame->isUsed() && !frame->isValid()) {
            std::cerr << __PRETTY_FUNCTION__ << ": Error in usage!" << std::endl;
        }
    }
}
