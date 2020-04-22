
#ifndef CODECINFO_H
#define CODECINFO_H

extern "C"{

#include <libavcodec/avcodec.h>

}

struct AVStream;

class CodecInfo {

    friend class VideoFileRead;
    friend class VideoDecoder;

public:
    enum CodecType{

        VideoCodec,
        AudioCodec,
    };

private:
    AVCodecParameters *avCodecParameters;
    AVCodec *avCodec;

    CodecType codecType;

    const AVStream *avStream;

public:
    CodecInfo();
    ~CodecInfo();

    inline CodecType getCodecType() const { return this->codecType; }

};

#endif // CODECINFO_H
