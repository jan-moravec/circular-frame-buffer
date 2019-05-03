#ifndef TESTFRAME_H
#define TESTFRAME_H

#include "library/frames/cameraframe.h"

class TestFrame: public CameraFrame
{
public:
    friend class TestFrameBuffer;

    TestFrame(unsigned width, unsigned height, unsigned id);
    ~TestFrame();

    const unsigned *getData() { return data; }
    unsigned getWidth() { return width; }
    unsigned getHeight() { return height; }

protected:
    unsigned width;
    unsigned height;
    unsigned *data;
};

#endif // TESTFRAME_H
