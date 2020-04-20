
#ifndef FRAME_H
#define FRAME_H

#include <inttypes.h>

class Frame {

    friend class VideoDecoder;

private:
    int width;
    int height;
    uint8_t *buffer;
    int bufferSize;

    int64_t pts;

    int frameIndex;
    double ptsRealTime;
    bool finalFrame;

public:
    Frame();
    ~Frame();

public:
    inline int getWidth() const { return this->width; }
    inline int getHeight() const { return this->height; }
    inline int64_t getPts() const { return this->pts; }
    inline const uint8_t* getBuffer() const { return this->buffer; }
    inline int getFrameIndex() const { return this->frameIndex; }
    inline double getPtsRealTime() const { return this->ptsRealTime; }
    inline bool isFinalFrame() const { return this->finalFrame; }
    inline int getBufferSize() const { return this->bufferSize; }
};

#endif // FRAME_H
