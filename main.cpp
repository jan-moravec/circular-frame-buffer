#include <iostream>
#include <thread>
#include <random>

#include "library/test/testframebuffer.h"

int main()
{
    {
        // Basic function check
        TestFrameBuffer buffer;
        buffer.setDebug(true);
        buffer.initialize(8);

        for (int i = 0; i < 8; ++i) {
            buffer.generateNewData();
        }

        buffer.print();
        for (int i = 0; i < 4; ++i) {
            std::vector<TestFrameBuffer::Frame> frames = buffer.getFinalN(2);
            buffer.generateNewData();
            buffer.print();
            buffer.check();
        }

        for (int i = 0; i < 4; ++i) {
            std::vector<TestFrameBuffer::Frame> frames = buffer.getCurrentN(2);
            buffer.generateNewData();
            buffer.print();
            buffer.check();
        }

        std::thread t_generate([&](){
            std::this_thread::sleep_for(std::chrono::seconds(5));
            buffer.generateNewData();
        });

        std::cout << "Waiting for new frame..." << std::endl;
        buffer.waitForNew();
        TestFrameBuffer::Frame frame = buffer.getCurrent();
        std::cout << "Frame " << frame->getId() << " ready." << std::endl;

        t_generate.join();
    }

    std::cout << std::endl << "Generating data, please wait..." << std::endl;
    // Generating huge data and checking sanity
    bool run = true;
    TestFrameBuffer buffer;
    buffer.initialize(128);

    std::thread t_generate([&](){
        while (run) {
            buffer.generateNewData();
            buffer.check();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::thread t_take_new([&](){
        while (run) {
            std::vector<TestFrameBuffer::Frame> frames = buffer.getNextWaitN(32);
            buffer.check();
        }
    });

    std::thread t_take_final([&](){
        while (run) {
            std::vector<TestFrameBuffer::Frame> frames = buffer.getFinalN(32);
            buffer.check();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<unsigned> dis(0, 1000);
    std::vector<std::thread> take_new_random;
    for (int i = 0; i < 128; ++i) {
        std::thread t([&](){
            while (run) {
                buffer.waitForNew();
                TestFrameBuffer::Frame frame = buffer.getCurrent();
                std::this_thread::sleep_for(std::chrono::milliseconds(dis(mt)));
                buffer.check();
            }
        });

        take_new_random.push_back(std::move(t));
    }

    std::this_thread::sleep_for(std::chrono::seconds(60));
    run = false;
    t_generate.join();
    for (int i = 0; i < 128; ++i) {
        buffer.generateNewData();
        buffer.check();
    }

    t_take_final.join();
    t_take_new.join();
    for (std::thread &t: take_new_random) {
        t.join();
    }

    std::cout << "Finished." << std::endl;
    return 0;
}
