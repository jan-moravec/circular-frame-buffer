#include "testframe.h"
#include <iostream>

TestFrame::TestFrame(unsigned id): CameraFrame(id)
{
    std::cout << __PRETTY_FUNCTION__ << ": " << getId() << std::endl;
}

TestFrame::~TestFrame()
{
    std::cout << __PRETTY_FUNCTION__ << ": " << getId() << std::endl;
    delete[] data;
}
