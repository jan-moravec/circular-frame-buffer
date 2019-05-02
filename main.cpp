#include <iostream>

#include "library/test/testframebuffer.h"

int main()
{
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


        //TestFrameBuffer::Frame frame0 = buffer.getCurrent();
        //TestFrameBuffer::Frame frame1 = buffer.getLast(frame0);
        //TestFrameBuffer::Frame frame10 = buffer.getNth(10);

        //std::vector<TestFrameBuffer::Frame> frames = buffer.getCurrentN(16);

        //for (TestFrameBuffer::Frame &frame: frames) {
        //    std::cout << frame->getId() << " - " << (int *)frame->getData() << std::endl;
        //}

        //if (frame0) {
        //    std::cout << (int *)frame0->getData() << std::endl;
        //}
    }

    std::cout << "AAAAAAAAAAAAAAAAA" << std::endl;
    return 0;
}
