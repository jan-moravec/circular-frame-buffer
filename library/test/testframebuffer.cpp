#include "testframebuffer.h"
#include <iostream>

TestFrameBuffer::TestFrameBuffer()
{
}

TestFrameBuffer::~TestFrameBuffer()
{
}

void TestFrameBuffer::initialize(unsigned n)
{
    initializeFrames(n);
}

void TestFrameBuffer::generateNewData()
{
    TestFrame *frame = static_cast<TestFrame *>(getNewCurrent());

    if (frame) {
        if (debug) {
            std::cout << __PRETTY_FUNCTION__ << ": " << frame->getId() << std::endl;
        }

        for (unsigned i = 0; i < frame->getWidth() * frame->getHeight(); ++i) {
            frame->data[i] = counter;
        }

        setNewReady(frame);
        counter++;
    } else {
        if (debug) {
            std::cout << __PRETTY_FUNCTION__ << ": Error getting frame" << std::endl;
        }
    }
}

TestFrameBuffer::Frame TestFrameBuffer::getCurrent()
{
    return castFrame(CameraFrameBuffer::getCurrent());
}

TestFrameBuffer::Frame TestFrameBuffer::getNth(unsigned nth)
{
    return castFrame(CameraFrameBuffer::getNth(nth));
}

TestFrameBuffer::Frame TestFrameBuffer::getFinal()
{
    return castFrame(CameraFrameBuffer::getFinal());
}

TestFrameBuffer::Frame TestFrameBuffer::getNext(const Frame &frame)
{
    return castFrame(CameraFrameBuffer::getNext(frame.get()));
}

TestFrameBuffer::Frame TestFrameBuffer::getNextWait()
{
    return castFrame(CameraFrameBuffer::getNextWait());
}

TestFrameBuffer::Frame TestFrameBuffer::getNextWait(const Frame &frame)
{
    return castFrame(CameraFrameBuffer::getNextWait(frame.get()));
}

std::vector<TestFrameBuffer::Frame> TestFrameBuffer::getNextWaitN(unsigned n)
{
    return castFrameVector(CameraFrameBuffer::getNextWaitN(n));
}

TestFrameBuffer::Frame TestFrameBuffer::getLast(const Frame &frame)
{
    return castFrame(CameraFrameBuffer::getLast(frame.get()));
}

std::vector<TestFrameBuffer::Frame> TestFrameBuffer::getCurrentN(unsigned n)
{
    return castFrameVector(CameraFrameBuffer::getCurrentN(n));
}

std::vector<TestFrameBuffer::Frame> TestFrameBuffer::getFinalN(unsigned n)
{
    return castFrameVector(CameraFrameBuffer::getFinalN(n));
}

TestFrameBuffer::Frame TestFrameBuffer::castFrame(CameraFrame *frame)
{
    return {static_cast<TestFrame *>(frame), releaser};
}

std::vector<TestFrameBuffer::Frame> TestFrameBuffer::castFrameVector(const std::vector<CameraFrame *> &frames)
{
    std::vector<Frame> frames_vector;
    for (CameraFrame *frame: frames) {
        frames_vector.push_back(castFrame(frame));
    }

    return frames_vector;
}
