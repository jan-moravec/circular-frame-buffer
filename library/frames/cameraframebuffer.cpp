#include "cameraframebuffer.h"

CameraFrameBuffer::CameraFrameBuffer()
{
}

CameraFrameBuffer::~CameraFrameBuffer()
{
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
    for (unsigned i = 0; i < frames.size() - 1; ++i) {
        frames[i]->setNext(frames[i + 1]);
        frames[i + 1]->setLast(frames[i]);
    }

    frames.back()->setNext(frames.front());
    frames.front()->setLast(frames.back());

    current = 0;
    final = frames.size() - 1;
}

CameraFrame *CameraFrameBuffer::getCurrent()
{
    std::lock_guard<std::mutex> lock(mutex);
    return getLastValid(frames.at(current));
}

CameraFrame *CameraFrameBuffer::getNth(unsigned nth)
{
    std::lock_guard<std::mutex> lock(mutex);

    CameraFrame *frame = frames.at(current);
    for (unsigned i = 0; i < nth; ++i) {
        frame = frame->getLast();
    }

    return getLastValid(frame);
}

CameraFrame *CameraFrameBuffer::getFinal(void)
{
    std::lock_guard<std::mutex> lock(mutex);
    return getNextValid(frames.at(final));
}

CameraFrame *CameraFrameBuffer::getNext(CameraFrame *frame)
{
    std::lock_guard<std::mutex> lock(mutex);
    return getNextValid(frame);
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
    return getLastValid(frame);
}

std::vector<CameraFrame *> CameraFrameBuffer::getLastN(unsigned n)
{
    if (n >= frames.size()) {
        n = frames.size() - 1;
    }

    std::vector<CameraFrame *> taken_frames;
    CameraFrame *frame = getFinal();
    if (!frame) {
        return taken_frames;
    }
    taken_frames.push_back(frame);

    for (unsigned i = 1; i < n; ++i) {
        CameraFrame *frame = getNext(taken_frames.back());
        if (!frame) {
            return taken_frames;
        }
        taken_frames.push_back(frame);
    }

    return taken_frames;
}

void CameraFrameBuffer::release(CameraFrame *frame)
{
    std::lock_guard<std::mutex> lock(mutex);
    frame->release();
}

void CameraFrameBuffer::waitForNew()
{
    std::unique_lock<std::mutex> lck(mutex);
    unsigned index = current;

    new_frame_ready.wait(lck, [&]{ return index != current; });
}

void CameraFrameBuffer::newFrameReady()
{
    new_frame_ready.notify_all();
}

CameraFrame *CameraFrameBuffer::getNextValid(CameraFrame *frame)
{
    CameraFrame *next = frame;

    if (frames.at(current) == next) {
        return nullptr;
    }

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
    CameraFrame *last = frame;

    if (frames.at(final) == last) {
        return nullptr;
    }

    while (!last->isValid()) {
        last = last->getLast();
        if (last == frame) {
            return nullptr;
        }
    }

    last->hold();
    return last;
}

CameraFrame *CameraFrameBuffer::getNextAvailable()
{
    std::lock_guard<std::mutex> lock(mutex);

    unsigned index = current;
    CameraFrame *next = getNextIndex(index);
    while (!next->isValid() || next->isUsed()) {
        next = getNextIndex(index);

        if (index == current) {
            return nullptr;
        }
    }

    next->setValid(false);
    return next;
}

CameraFrame *CameraFrameBuffer::getNextIndex(unsigned &index)
{
    index++;

    if (index >= frames.size()) {
        index = 0;
    }

    return frames.at(index);
}
