#ifndef CAMERAFRAME_H
#define CAMERAFRAME_H

#include <cstdint>
#include <string>

class CameraFrame
{
public:
    friend class CameraFrameBuffer;

    CameraFrame(unsigned id);
    virtual ~CameraFrame();

    unsigned getId() const;
    int64_t getTimestamp() const;
    std::string getTimeString() const;
    void print() const;

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

    bool debug = false;

private:
    unsigned id = 0;
    bool valid = false;
    unsigned semaphore = 0;
    int64_t timestamp = 0;

    CameraFrame *next = nullptr;
    CameraFrame *last = nullptr;
};

#endif // CAMERAFRAME_H
