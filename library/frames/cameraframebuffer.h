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
    void setDebug(bool on);
    void print();
    void check();

protected:
    void initializeFrames(unsigned n);

    CameraFrame *getCurrent();
    CameraFrame *getNth(unsigned nth);
    std::vector<CameraFrame *> getCurrentN(unsigned n);
    CameraFrame *getFinal();
    std::vector<CameraFrame *> getFinalN(unsigned n);
    CameraFrame *getNext(CameraFrame *frame);
    CameraFrame *getNextWait();
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

protected:
    std::mutex mutex;
    std::condition_variable new_frame_ready;
    std::vector<CameraFrame *> frames;

    CameraFrame *current = nullptr;
    CameraFrame *final = nullptr;
    std::set<CameraFrame *> skipped;

    bool debug = false;
};

#endif // CAMERAFRAMEBUFFER_H
