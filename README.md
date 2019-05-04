# Circular Frame Buffer

I need to work with frames from thermocamera in emebedded device, where performance is the main issue. The data must be accessible from different threads. So I created these classes for representing the camera data and for accesing the data itself safely.

Assignment:
- Have N frames or measurements (any piece of data).
- Acces them from multiple threads.
- Generate the frames from multiple threads.
- The data itself must be shared memory due to performance.

## Example application

For example, there is one thread generating the data. It is saving the data to circular buffer - in frames. The buffer must know, which frame is the newest and which is the oldest.

There are multiple threads accesing the data. There is a thread that waits for the new frame and once its ready, it displays its content.
Then there is a thread that does a measurement on N frames on command, so it takes current frame and N-1 previous frames and does its measurements.

All these actions must be thread safe and fast as possible. Once the thread takes a frame, it can access its data until the frame is released again.

## Getting Started

The project has two classes and main.cpp, in which the classes are tested.

### Prerequisites

I am using C++11, so newer toolchain is needed. Also I am using the __PRETTY_FUNCTION__ macro, which I think is gcc extension. If this is an issue, just use __func__ instead.

## Classes Description

### CameraFrame [library/frames/cameraframe.h](CameraFrame)

Basic class design to contain the data. It's got the semaphore - a counter, which increases once the frame is taken and decreases once its released.

It also has a pointer to next and previous frame.

There are no mutexes, all is controlled from CameraFrameBuffer.

### TestFrame [library/test/testframe.h](TestFrame)

Example of CameraFrame usage - just inherit from it and add your data and other information.

### CameraFrameBuffer [library/frames/cameraframebuffer.h](CameraFrameBuffer)

This class does all the heavy lifting - it controlles the shared memory (CameraFrame frames) access.

There is one pure virtual function:
```
virtual CameraFrame *newFrame(unsigned id) = 0;
```
This function must be overriden. It contains the new frame creation and returns a pointer to it. The reason is that you want to create a TestFrame, for example, but the CameraFrameBuffer needs it as CameraFrame object to work with it.

Since the newFrame() is virtual function, it SHOULD NOT be called from constructor - thus the function initializeFrames() is needed. This function prepares desired number of images and linkes them together.

Then there are multiple functions for accessing the CameraFrame frames. They are protected - the child class must decide which of them can be used as public.

There is no function for generating new data - this must be in its derived child class.

### TestFrameBuffer [library/test/testframebuffer.h](TestFrameBuffer)

This class works with the TestFrame frames. It hast a function for generating the data, function for creating a new frame:
```
CameraFrame *newFrame(unsigned id) override { return new TestFrame(800, 600, id); }
```

It has also overshadowed the functions for accessing the CameraFrame frames. It added new type 
```
using frameReleaser = std::function<void(TestFrame *)>;
const frameReleaser releaser = [this](TestFrame *frame) { release(frame); };
using Frame = std::unique_ptr< TestFrame, frameReleaser >;
```

which is a unique pointer to the frame, that does the releasing once destroyed. So there is no need to call the release() class function.

## Internal Work Description

The frames are saved in vector. There is a reference to current frame and final frame. 
- Current frame: next is always Final.
- Final frame: last is always Current.

The frames thus are represented as circular buffer. The circular buffer size can vary due to the data generating and data accessing.

Each frame itself also has a pointer to next and precious.

Each frame can be taken (semaphore is not a zero), or invalid (it is being updated).

The functions:
```
CameraFrame *getNewCurrent();
void setNewReady(CameraFrame *frame);
```
are used for getting an unused and valid frame, that will be updated (populate with new data), and then set as ready.

Since none or all frames can be taken or invalid, user must check if the frame is not a nullptr.

If some frames are taken, they will not be updated until they are available. That means that they will be skipped in the getNewCurrent() function and will be "frozen", until updated again. 

So, for example, if you create buffer with 6 frames and 4 of them are taken for long period of time, only 2 frames are available for updating. Also the user can not get more than the 2 new frames, since the 4 taken frames are temporarily deleted from the circular buffer.

## Recommendation

I tried to test all the function to guarantee it to be thread safe. But for the est performance, you should always create enough frames in the buffer. If you know you application will take 32 frames at a time, make the buffer like 64 frames wide. 

Once all frames are taken, it can not save new data.

## Author

* **Jan Moravec**


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
