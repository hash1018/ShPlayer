
#include "videodecoder.h"

extern "C" {

#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}

VideoDecoder::VideoDecoder()
    :avCodecContext(NULL), swsContext(NULL),initializedCodec(false) {


}

VideoDecoder::~VideoDecoder() {

    if(this->avCodecContext != NULL)
        avcodec_free_context(&this->avCodecContext);

    if(this->swsContext != NULL)
        sws_freeContext(this->swsContext);

}

bool VideoDecoder::setCodec(CodecInfo &codecInfo){

    if(codecInfo.codecType != CodecInfo::CodecType::VideoCodec){

        printf("codecInfo is not videoCodecType\n");
        return false;
    }


    this->initializedCodec = false;

    if(this->avCodecContext != NULL){
        avcodec_free_context(&this->avCodecContext);
        this->avCodecContext = NULL;
    }

    this->avCodecContext = avcodec_alloc_context3(codecInfo.avCodec);

    if(this->avCodecContext == NULL){

        printf("failed to alloc codecContext\n");
        return false;
    }

    if(avcodec_parameters_to_context(this->avCodecContext, codecInfo.avCodecParameters) < 0) {

        printf("failed to initialize codecContext\n");
        return false;
    }

    if(avcodec_open2(this->avCodecContext,codecInfo.avCodec,NULL) <0 ) {

        printf("failed to open codec\n");
        return false;
    }

    this->initializedCodec = true;

    this->stream = codecInfo.avStream;

    return true;
}

bool VideoDecoder::decode(const Packet &packet, Frame &frame){

    if(this->initializedCodec == false) {

        printf("codec is not initialized\n");
        return false;
    }

    int response = avcodec_send_packet(this->avCodecContext, packet.avPacket);

    if(response < 0){

        printf("failed to decode Packet \n");

        av_packet_unref(packet.avPacket);
        return false;
    }

    AVFrame *avFrame;
    avFrame = av_frame_alloc();

    response = avcodec_receive_frame(this->avCodecContext, avFrame);

    if(response == AVERROR(EAGAIN)){

        av_packet_unref(packet.avPacket);
        av_frame_unref(avFrame);
        av_frame_free(&avFrame);

        printf("error eagain\n");
        return false;
    }
    else if(response == AVERROR_EOF){

        av_packet_unref(packet.avPacket);
        av_frame_unref(avFrame);
        av_frame_free(&avFrame);
        printf("error end of file\n");
        return false;
    }
    else if( response < 0 ) {

        av_packet_unref(packet.avPacket);
        av_frame_unref(avFrame);
        av_frame_free(&avFrame);

        printf("failed to decode packet\n");
        return false;
    }


    //Success from here.    ///////////////////////////////////////////////


    av_packet_unref(packet.avPacket);

    int fps = this->stream->avg_frame_rate.num /
            this->stream->avg_frame_rate.den;

    frame.pts = avFrame->pts;
    frame.ptsRealTime = avFrame->pts *
            (double) this->stream->time_base.num /
            (double) this->stream->time_base.den;
    frame.frameIndex = (double(fps * avFrame->pts) /
                        (double)this->stream->time_base.den) *
          (double)this->stream->time_base.num;

    frame.width = avFrame->width;
    frame.height= avFrame->height;


    int size = av_image_get_buffer_size(AVPixelFormat::AV_PIX_FMT_YUV420P, avFrame->width,
                                        avFrame->height,1);


    if(this->swsContext == NULL) {
        this->swsContext=sws_getContext(avFrame->width, avFrame->height, this->avCodecContext->pix_fmt,
                                        avFrame->width, avFrame->height, AV_PIX_FMT_YUV420P,
                                        SWS_FAST_BILINEAR, NULL, NULL, NULL);
    }

    if(this->swsContext == NULL){

        av_frame_unref(avFrame);
        av_frame_free(&avFrame);

        printf("failed to initialized swsContext\n");
        return false;
    }

    AVPicture picture;
    avpicture_alloc(&picture,AVPixelFormat::AV_PIX_FMT_YUV420P, avFrame->width, avFrame->height);

    sws_scale(this->swsContext, avFrame->data, avFrame->linesize, 0, avFrame->height, picture.data, picture.linesize);

    if(frame.bufferSize != size){

        if(frame.buffer !=nullptr){
            delete [] frame.buffer;
            frame.buffer = nullptr;
        }

        frame.bufferSize = size;
        frame.buffer = new uint8_t [size];
    }

    memcpy(frame.buffer, picture.data[0], avFrame->width * avFrame->height);

    frame.buffer += avFrame->width * avFrame->height;
    memcpy(frame.buffer, picture.data[1], avFrame->width * avFrame->height / 4 );

    frame.buffer += avFrame->width * avFrame->height / 4;
    memcpy(frame.buffer, picture.data[2], avFrame->width * avFrame->height / 4 );

    frame.buffer -= avFrame->width * avFrame->height;
    frame.buffer -= avFrame->width * avFrame->height / 4;

    avpicture_free(&picture);
    av_frame_unref(avFrame);
    av_frame_free(&avFrame);

    return true;
}
