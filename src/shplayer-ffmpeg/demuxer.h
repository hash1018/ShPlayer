
#ifndef DEMUXER_H
#define DEMUXER_H

extern "C"{

#include <libavformat/avformat.h>

}

#include "packet.h"
#include "codecinfo.h"

class CodecInfo;

class Demuxer {

private:
    AVFormatContext *avFormatContext;
    int width;
    int height;

    int videoStreamIndex;
    int audioStreamIndex;
    int subtitleStreamIndex;

private:
    bool opened;
    bool closed;

private:
    PacketType packetTypeToRead;

public:
    Demuxer();
    ~Demuxer();

    bool open(const char *filePath, PacketType packetTypeToRead);
    bool readPacket(Packet &outPacket);
    bool close();

public:
    inline int getWidth() const { return this->width; }
    inline int getHeight() const { return this->height; }
    inline bool isOpened() const { return this->opened; }
    inline bool isClosed() const { return this->closed; }

    bool getVideoCodecInfo(CodecInfo &codecInfo);
    bool getAudioCodecInfo(CodecInfo &codecInfo);
    bool getSubtitleCodecInfo(CodecInfo &codecInfo);

    int getVideoFps();

};

#endif // DEMUXER_H
