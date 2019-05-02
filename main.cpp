#include <iostream>

#include "library/test/testframebuffer.h"

int main()
{
    TestFrameBuffer buffer;
    buffer.initialize();

    for (int i = 0; i < 8; ++i) {
        buffer.generateNewData();
    }

    for (int i = 0; i < 2; ++i) {
        TestFrameBuffer::Frame frame0 = buffer.getFinal();
        TestFrameBuffer::Frame frame1 = buffer.getNext(frame0);
        TestFrameBuffer::Frame frame2 = buffer.getNext(frame1);
        buffer.generateNewData();
    }

    for (int i = 0; i < 2; ++i) {
        buffer.generateNewData();
    }

    return 0;
}
