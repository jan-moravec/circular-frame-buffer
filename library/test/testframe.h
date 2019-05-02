#ifndef TESTFRAME_H
#define TESTFRAME_H

#include "library/frames/cameraframe.h"

class TestFrame: public CameraFrame
{
public:
    friend class TestFrameBuffer;

    TestFrame(unsigned id);
    ~TestFrame();

    const unsigned *getData() { return data; }

protected:
    unsigned *data = nullptr;
};

#endif // TESTFRAME_H
