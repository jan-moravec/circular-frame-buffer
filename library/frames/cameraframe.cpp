#include "cameraframe.h"
#include <chrono>
#include <iostream>

CameraFrame::CameraFrame(unsigned id): id(id)
{
    if (debug) {
        std::cout << __PRETTY_FUNCTION__ << ": " << id << std::endl;
    }
}

CameraFrame::~CameraFrame()
{
    if (debug) {
        std::cout << __PRETTY_FUNCTION__ << ": " << id << std::endl;
    }
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

std::string CameraFrame::getTimeString() const
{
    std::time_t time = static_cast<std::time_t>(timestamp / 1000);
    int64_t milli = timestamp - static_cast<int64_t>(time) * 1000;

    std::tm * ptm = std::localtime(&time);
    char bufferr[32];
    int ret = std::strftime(bufferr, 32, "%H:%M:%S", ptm);

    if (ret) {
        return std::string(bufferr) + "." + std::to_string(milli);
    }
    return "";
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
    if (debug) {
        std::cout << __PRETTY_FUNCTION__ << ": " << getId() << std::endl;
    }
    semaphore++;
}

void CameraFrame::release()
{
    if (debug) {
        std::cout << __PRETTY_FUNCTION__ << ": " << getId() << std::endl;
    }
    if (semaphore == 0) {
        return;
    }

    semaphore--;
}

bool CameraFrame::isUsed()
{
    return (semaphore != 0);
}

void CameraFrame::print() const
{
    std::cout << last->getId() << " <- " << getId() << " -> " << next->getId() << std::endl;
}
