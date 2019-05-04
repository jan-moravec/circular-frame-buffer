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

    void waitForNew(); /// Will block the thread until new frame is ready
    void setDebug(bool on);
    void print();
    void check();

protected:
    void initializeFrames(unsigned n); /// Create buffer with N frames

    /// Functions for accessing the frames
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

    void release(CameraFrame *frame); /// Do not forget to release taken frames

protected:
    virtual CameraFrame *newFrame(unsigned id) = 0; /// Create new frame of child class
    void connectFrames();

    CameraFrame *getNewCurrent(); /// Get frame for updating with new data
    void setNewReady(CameraFrame *frame); /// Once updated, set as ready
    void newFrameReady();

    CameraFrame *getNextValid(CameraFrame *frame);
    CameraFrame *getLastValid(CameraFrame *frame);

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
