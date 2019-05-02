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
    std::cout << __PRETTY_FUNCTION__ << ": " << frame->getId() << std::endl;
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

    // Find next available frame by index, so we can include skipped ones
    std::vector<CameraFrame *> skipped_temp;
    unsigned index = current->getId();
    CameraFrame *frame = current;
    do {
        frame = getNextIndex(index);

        if (frame == current) {
            return nullptr;
        }

        if (!frame->isValid() || frame->isUsed()) {
            skipped_temp.push_back(frame);
        }

    } while (!frame->isValid() || frame->isUsed()); // We need to skip taken and invalid frames

    for (int i = 0; i < skipped_temp.size(); ++i) {
        skipped.insert(skipped_temp.at(i));
    }

    std::cout << __PRETTY_FUNCTION__ << "A: " << frame->getId() << " "  << current->getId() << " " << final->getId() << std::endl;
    // Update final. If available frame is skipped one, do not move final. TODO how to check if frame is skipped
    bool isskipped = skipped.find(frame) != skipped.end();
    if (!isskipped) {
        final = frame->getNext();
    }// else if (frame != current->getNext()) { // TODO && (current->getNext()->isValid() && !current->getNext()->isUsed()
    //    final = frame->getNext();
    //}

    /*CameraFrame *next = frame->getNext();
    CameraFrame *last = frame->getLast();
    last->setNext(next);
    next->setLast(last);*/

    std::cout << __PRETTY_FUNCTION__ << "B: " << frame->getId() << " "  << current->getId() << " " << final->getId() << std::endl;

    current->setNext(frame);
    frame->setLast(current);

    // Set new current frame
    current = frame;

    // Keep the circle
    current->setNext(final);
    final->setLast(current);

    std::cout << __PRETTY_FUNCTION__ << "C: " << frame->getId() << " "  << current->getId() << " " << final->getId() << std::endl;
    frame->setValid(false);
    return frame;
}

void CameraFrameBuffer::setNewReady(CameraFrame *frame)
{
    std::lock_guard<std::mutex> lock(mutex);
    frame->setValid(true);
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
    CameraFrame *frame = final;
    std::cout << frame->getId();
    do {
        frame = frame->getNext();
        std::cout << " -> " << frame->getId();
        counter++;
    } while (frame != final);
    std::cout << std::endl;

    std::cout << __PRETTY_FUNCTION__ << ": Total frames = " << counter << std::endl;

    for (CameraFrame *frame: frames) {
        frame->print();
    }
}
