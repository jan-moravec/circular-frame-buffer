#ifndef CAMERAFRAMEBUFFER_H
#define CAMERAFRAMEBUFFER_H

#include <vector>
#include <set>
#include <mutex>
#include <condition_variable>
#include "cameraframe.h"

class CameraFrameBuffer
{
public:
    CameraFrameBuffer();
    virtual ~CameraFrameBuffer();

    void waitForNew();

protected:
    void initializeFrames(unsigned n);

    CameraFrame *getCurrent();
    CameraFrame *getNth(unsigned nth);
    std::vector<CameraFrame *> getCurrentN(unsigned n);
    CameraFrame *getFinal();
    CameraFrame *getNext(CameraFrame *frame);
    CameraFrame *getNextWait(CameraFrame *frame);
    std::vector<CameraFrame *> getNextWaitN(unsigned n);
    CameraFrame *getLast(CameraFrame *frame);

    void release(CameraFrame *frame);

protected:
    virtual CameraFrame *newFrame(unsigned id) = 0;
    void connectFrames();

    void newFrameReady();
    CameraFrame *getNextValid(CameraFrame *frame);
    CameraFrame *getLastValid(CameraFrame *frame);

    CameraFrame *getNewCurrent();
    void setNewReady(CameraFrame *frame);
    CameraFrame *getNextIndex(unsigned &index);

    void print();

protected:
    std::mutex mutex;
    std::condition_variable new_frame_ready;
    std::vector<CameraFrame *> frames;

    CameraFrame *current = nullptr;
    CameraFrame *final = nullptr;
    std::set<CameraFrame *> skipped;
};

#endif // CAMERAFRAMEBUFFER_H
