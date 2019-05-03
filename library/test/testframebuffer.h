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

    void initialize(unsigned n);
    void generateNewData();

    using frameReleaser = std::function<void(TestFrame *)>;
    using Frame = std::unique_ptr< TestFrame, frameReleaser >;

    Frame getCurrent();
    Frame getNth(unsigned nth);
    Frame getFinal();
    Frame getNext(const Frame &frame);
    Frame getNextWait();
    Frame getNextWait(const Frame &frame);
    std::vector<Frame> getNextWaitN(unsigned n);
    Frame getLast(const Frame &frame);
    std::vector<Frame> getCurrentN(unsigned n);
    std::vector<Frame> getFinalN(unsigned n);

protected:
    CameraFrame *newFrame(unsigned id) override { return new TestFrame(800, 600, id); }
    unsigned counter = 0;

private:
    const frameReleaser releaser = [this](TestFrame *frame) { release(frame); };
    inline Frame castFrame(CameraFrame *frame);
    inline std::vector<Frame> castFrameVector(const std::vector<CameraFrame *> &frames);
};

#endif // TESTFRAMEBUFFER_H
