#include "testframe.h"
#include <iostream>

TestFrame::TestFrame(unsigned width, unsigned height, unsigned id): CameraFrame(id), width(width), height(height)
{
    if (debug) {
        std::cout << __PRETTY_FUNCTION__ << ": " << getId() << std::endl;
    }

    data = new unsigned[width * height];
}

TestFrame::~TestFrame()
{
    if (debug) {
        std::cout << __PRETTY_FUNCTION__ << ": " << getId() << std::endl;
    }

    delete[] data;
}
