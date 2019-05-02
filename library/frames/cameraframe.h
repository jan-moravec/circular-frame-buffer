#ifndef CAMERAFRAME_H
#define CAMERAFRAME_H

#include <cstdint>

class CameraFrame
{
public:
    friend class CameraFrameBuffer;

    CameraFrame(unsigned id);
    virtual ~CameraFrame();

    unsigned getId() const;
    int64_t getTimestamp() const;
    void print();

protected:
    CameraFrame *getNext() const;
    CameraFrame *getLast() const;
    void setNext(CameraFrame *frame);
    void setLast(CameraFrame *frame);

    void setId(unsigned id);
    void updateTimestamp();

    bool isValid() const;
    void setValid(bool valid);

    void hold();
    void release();
    bool isUsed();

private:
    unsigned id = 0;
    bool valid = true; // false
    unsigned semaphore = 0;
    int64_t timestamp = 0;

    CameraFrame *next = nullptr;
    CameraFrame *last = nullptr;
};

#endif // CAMERAFRAME_H
