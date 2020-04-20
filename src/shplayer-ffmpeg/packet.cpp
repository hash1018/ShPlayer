#include "packet.h"

Packet::Packet()
    :packetType(Nothing) {


    this->avPacket = av_packet_alloc();


}

Packet::~Packet() {

    av_packet_free(&this->avPacket);

}
