
#ifndef VIDEODECODER_H
#define VIDEODECODER_H

extern "C"{

#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

}

#include "frame.h"
#include "codecinfo.h"
#include "packet.h"

struct AVStream;

class VideoDecoder {

    friend class VideoFileUtil;

private:
    AVCodecContext *avCodecContext;
    SwsContext *swsContext;
    bool initializedCodec;

    const AVStream *stream;
    AVFrame *avFrame;

public:
    VideoDecoder();
    ~VideoDecoder();

    bool setCodec(CodecInfo &codecInfo);
    bool decode(const Packet &packet, Frame &frame);

};

#endif // VIDEODECODER_H
