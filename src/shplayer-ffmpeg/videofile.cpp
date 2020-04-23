
#include "videofile.h"


VideoFileRead::VideoFileRead()
    :avFormatContext(NULL), width(0), height(0),
      videoStreamIndex(-1), audioStreamIndex(-1), subtitleStreamIndex(-1),
      opened(false), closed(false), packetTypeToRead(PacketType::Nothing) {

}

VideoFileRead::~VideoFileRead() {

    if(this->opened==true && this->closed==false)
        this->close();

}

bool VideoFileRead::open(const char *filePath, PacketType packetTypeToRead){

    if(this->opened == true) {

        printf("already opened\n");
        return false;
    }

    if(this->closed == true) {

        printf("already closed\n");
        return false;
    }

    this->avFormatContext = avformat_alloc_context();
    if(this->avFormatContext == NULL) {

        printf("failed to alloc avFormatContext\n");
        return false;
    }

    if(avformat_open_input(&this->avFormatContext, filePath, NULL, NULL) !=0) {

        printf("failed to open avFormatContext\n");
        return false;
    }


    AVCodecParameters *avCodecParameters;
    AVCodec *avCodec;
    this->videoStreamIndex = -1;
    this->audioStreamIndex = -1;
    this->subtitleStreamIndex = -1;

    for(unsigned int i=0; i < this->avFormatContext->nb_streams; i++) {

        avCodecParameters=this->avFormatContext->streams[i]->codecpar;
        avCodec=avcodec_find_decoder(avCodecParameters->codec_id);

        if(!avCodec){
            continue;
        }

        if(avCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {

            //No matter packetTypeToRead is videopacket or not, size should be saved.
            this->width = avCodecParameters->width;
            this->height = avCodecParameters->height;

            if( (packetTypeToRead & PacketType::VideoPacket) == PacketType::VideoPacket ) {

                this->videoStreamIndex = i;    
            }
        }
        else if(avCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {

            if((packetTypeToRead & PacketType::AudioPacket) == PacketType::AudioPacket) {

                this->audioStreamIndex = i;
            }
        }
        else if(avCodecParameters->codec_type == AVMEDIA_TYPE_SUBTITLE) {

            if((packetTypeToRead & PacketType::SubtitlePacket) == PacketType::SubtitlePacket) {

                this->subtitleStreamIndex = i;
            }
        }
    }

    if((packetTypeToRead & PacketType::VideoPacket) == PacketType::VideoPacket) {

        if(this->videoStreamIndex == -1){

            printf("failed to find videoStream\n");
            return false;
        }
    }

    if((packetTypeToRead & PacketType::AudioPacket) == PacketType::AudioPacket) {

        if(this->audioStreamIndex == -1){

            printf("failed to find audioStream\n");
            return false;
        }
    }

    if((packetTypeToRead & PacketType::SubtitlePacket) == PacketType::SubtitlePacket) {

        if(this->subtitleStreamIndex == -1){

            printf("failed to find subtitleStream\n");
            return false;
        }
    }



    this->opened = true;
    this->packetTypeToRead = packetTypeToRead;

    return true;
}


bool VideoFileRead::readPacket(Packet &outPacket) {

    if(this->opened == false) {

        printf("demuxer is not opened\n");
        return false;
    }

    if(this->closed == true) {

        printf("demuxer is already closed\n");
        return false;
    }

    while( av_read_frame(this->avFormatContext, outPacket.avPacket) >=0) {

        if(outPacket.avPacket->stream_index == this->videoStreamIndex ) {

            if( (this->packetTypeToRead & PacketType::VideoPacket) == PacketType::VideoPacket ) {

                outPacket.packetType = PacketType::VideoPacket;
                return true;
            }
        }
        else if(outPacket.avPacket->stream_index == this->audioStreamIndex ) {

            if( (this->packetTypeToRead & PacketType::AudioPacket) == PacketType::AudioPacket ) {

                outPacket.packetType = PacketType::AudioPacket;
                return true;
            }
        }
        else if(outPacket.avPacket->stream_index == this->subtitleStreamIndex ) {

            if( (this->packetTypeToRead & PacketType::SubtitlePacket) == PacketType::SubtitlePacket ) {

                outPacket.packetType = PacketType::SubtitlePacket;
                return true;
            }
        }

        av_packet_unref(outPacket.avPacket);
    }

    return false;
}

bool VideoFileRead::close() {

    if(this->opened == false)
        return false;

    if(this->closed == true)
        return false;

    avformat_close_input(&this->avFormatContext);
    avformat_free_context(this->avFormatContext);

    this->closed = true;

    return true;

}

bool VideoFileRead::getVideoCodecInfo(CodecInfo &codecInfo){

    if(this->closed==true) {

        printf(" failed to get CodecInfo\n");
        return false;
    }

    if(this->opened == false) {

        printf(" failed to get CodecInfo\n");
        return false;
    }

    if( ( this->packetTypeToRead & PacketType::VideoPacket) == PacketType::VideoPacket){


        codecInfo.avCodecParameters = this->avFormatContext->streams[this->videoStreamIndex]->codecpar;
        codecInfo.avCodec = avcodec_find_decoder(codecInfo.avCodecParameters->codec_id);
        codecInfo.codecType = CodecInfo::CodecType::VideoCodec;
        codecInfo.avStream = this->avFormatContext->streams[this->videoStreamIndex];

        return true;
    }

    return false;
}

bool VideoFileRead::getAudioCodecInfo(CodecInfo &codecInfo){

    return false;
}

bool VideoFileRead::getSubtitleCodecInfo(CodecInfo &codecInfo){

    return false;
}

int VideoFileRead::getVideoFps() {

    if(this->opened == false)
        return -1;

    if(this->closed == true)
        return -1;

    if((this->packetTypeToRead & PacketType::VideoPacket) == PacketType::VideoPacket){
        return this->avFormatContext->streams[this->videoStreamIndex]->avg_frame_rate.num /
                this->avFormatContext->streams[this->videoStreamIndex]->avg_frame_rate.den;
    }

    return -1;
}

uint64_t VideoFileRead::getDuration() const {

    if(this->opened == false)
        return 0;

    if(this->closed == true)
        return 0;

    return this->avFormatContext->duration / AV_TIME_BASE;

}
