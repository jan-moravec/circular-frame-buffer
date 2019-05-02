#include "cameraframe.h"
#include <chrono>
#include <iostream>

CameraFrame::CameraFrame(unsigned id): id(id)
{
    std::cout << __PRETTY_FUNCTION__ << ": " << id << std::endl;
}

CameraFrame::~CameraFrame()
{
    std::cout << __PRETTY_FUNCTION__ << ": " << id << std::endl;
}

CameraFrame *CameraFrame::getNext() const
{
    return next;
}

CameraFrame *CameraFrame::getLast() const
{
    return last;
}

void CameraFrame::setNext(CameraFrame *frame)
{
    next = frame;
}

void CameraFrame::setLast(CameraFrame *frame)
{
    last = frame;
}

unsigned CameraFrame::getId() const
{
    return id;
}

void CameraFrame::setId(unsigned id)
{
    this->id = id;
}

void CameraFrame::updateTimestamp()
{
    timestamp = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count();
}

int64_t CameraFrame::getTimestamp() const
{
    return timestamp;
}

bool CameraFrame::isValid() const
{
    return valid;
}

void CameraFrame::setValid(bool valid)
{
    this->valid = valid;
}

void CameraFrame::hold()
{
    semaphore++;
}

void CameraFrame::release()
{
    if (semaphore == 0) {
        return;
    }

    semaphore--;
}

bool CameraFrame::isUsed()
{
    return (semaphore != 0);
}

void CameraFrame::print()
{
    std::cout << last->getId() << " <- " << getId() << " -> " << next->getId() << std::endl;
}
