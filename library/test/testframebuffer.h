#ifndef TESTFRAMEBUFFER_H
#define TESTFRAMEBUFFER_H

#include "library/frames/cameraframebuffer.h"
#include "testframe.h"
#include <functional>

class TestFrameBuffer: public CameraFrameBuffer
{
public:
    TestFrameBuffer();
    ~TestFrameBuffer();

    void initialize();
    void generateNewData();

    using frameReleaser = std::function<void(TestFrame *)>;
    using Frame = std::unique_ptr< TestFrame, frameReleaser >;

    Frame getCurrent() { return castFrame(CameraFrameBuffer::getCurrent()); }
    Frame getNth(unsigned nth) { return castFrame(CameraFrameBuffer::getNth(nth)); }
    Frame getFinal() { return castFrame(CameraFrameBuffer::getFinal()); }
    Frame getNext(const Frame &frame) { return castFrame(CameraFrameBuffer::getNext(frame.get())); }
    Frame getNextWait(const Frame &frame) { return castFrame(CameraFrameBuffer::getNextWait(frame.get())); }
    std::vector<Frame> getNextWaitN(unsigned n) { return castFrameVector(CameraFrameBuffer::getNextWaitN(n)); }
    Frame getLast(const Frame &frame) { return castFrame(CameraFrameBuffer::getLast(frame.get())); }
    std::vector<Frame> getCurrentN(unsigned n) { return castFrameVector(CameraFrameBuffer::getCurrentN(n)); }

protected:
    CameraFrame *newFrame(unsigned id) override { return new TestFrame(id); }

private:
    const frameReleaser releaser = [this](TestFrame *frame) { release(frame); };
    Frame castFrame(CameraFrame *frame);
    std::vector<Frame> castFrameVector(const std::vector<CameraFrame *> &frames);
};

#endif // TESTFRAMEBUFFER_H
