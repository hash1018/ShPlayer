
#include "frame.h"

Frame::Frame()
    :width(0), height(0), buffer(nullptr), bufferSize(0), pts(0), frameIndex(0),
      ptsRealTime(0), finalFrame(false) {

}

Frame::~Frame() {

    if(this->buffer !=nullptr )
        delete [] this->buffer;

}
