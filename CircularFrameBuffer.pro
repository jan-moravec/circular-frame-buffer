TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        library/frames/cameraframe.cpp \
        library/frames/cameraframebuffer.cpp \
        library/test/testframe.cpp \
        library/test/testframebuffer.cpp \
        main.cpp

HEADERS += \
    library/frames/cameraframe.h \
    library/frames/cameraframebuffer.h \
    library/test/testframe.h \
    library/test/testframebuffer.h
