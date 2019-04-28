#ifndef CAMERAFRAMEBUFFER_H
#define CAMERAFRAMEBUFFER_H

#include <vector>
#include <mutex>
#include <condition_variable>
#include "cameraframe.h"

class CameraFrameBuffer
{
public:
    CameraFrameBuffer();
    virtual ~CameraFrameBuffer();

    void initializeFrames(unsigned n);
    virtual void addFrame() {}
    virtual void addFrame(void *) {}

    CameraFrame *getCurrent();
    CameraFrame *getNth(unsigned nth);
    CameraFrame *getFinal();
    CameraFrame *getNext(CameraFrame *frame);
    CameraFrame *getNextWait(CameraFrame *frame);
    std::vector<CameraFrame *> getNextWaitN(unsigned n);
    CameraFrame *getLast(CameraFrame *frame);
    std::vector<CameraFrame *> getLastN(unsigned n);

    void release(CameraFrame *frame);
    void waitForNew();

protected:
    virtual CameraFrame *newFrame(unsigned id) = 0;
    void connectFrames();

    void newFrameReady();
    CameraFrame *getNextValid(CameraFrame *frame);
    CameraFrame *getLastValid(CameraFrame *frame);
    CameraFrame *getNextAvailable();
    CameraFrame *getNextIndex(unsigned &index);

protected:
    std::mutex mutex;
    std::condition_variable new_frame_ready;
    std::vector<CameraFrame *> frames;

    unsigned current = 0;
    unsigned final = 0;
};

#endif // CAMERAFRAMEBUFFER_H
