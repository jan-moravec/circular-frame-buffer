#include "testframebuffer.h"

TestFrameBuffer::TestFrameBuffer()
{
}

TestFrameBuffer::~TestFrameBuffer()
{
}

void TestFrameBuffer::initialize()
{
    initializeFrames(8);
}

void TestFrameBuffer::generateNewData()
{
    TestFrame *frame = static_cast<TestFrame *>(getNewCurrent());

    if (frame) {
        //for (int i = 0; i < 0; ++i) {
        //    frame->data[i] = 0;
        //}
        setNewReady(frame);
    }
    check();
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
