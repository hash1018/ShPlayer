
#ifndef PACKET_H
#define PACKET_H

extern "C"{

#include <libavcodec/avcodec.h>
}


enum PacketType{

    Nothing = 0,
    VideoPacket = 1,
    AudioPacket = 2,
    SubtitlePacket = 4,

};

class Packet {

    friend class Demuxer;
    friend class VideoDecoder;

private:
    AVPacket *avPacket;
    PacketType packetType;

public:
    Packet();
    ~Packet();

public:
    inline PacketType getPacketType() const { return this->packetType; }
};

#endif // PACKET_H
