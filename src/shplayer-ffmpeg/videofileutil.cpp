
#include "videofileutil.h"
#include "videodecoder.h"
#include "videofile.h"
#include "frame.h"

VideoFileUtil::VideoFileUtil() {

}

VideoFileUtil::~VideoFileUtil() {


}

bool VideoFileUtil::seek(int index, VideoFileRead &videoFileRead, VideoDecoder &videoDecoder, Frame &frame){

    if(videoFileRead.opened == false) {

        printf("videoFile is not open\n");
        return false;
    }

    if(videoFileRead.closed == true) {

        printf("videoFile is already close\n");
        return false;
    }

    if(videoFileRead.videoStreamIndex == -1) {

        printf("videoFile is not allowed to read video stream\n");
        return false;
    }

    if(videoDecoder.initializedCodec == false){

        printf("decoder is not initialized\n");
        return false;
    }

    if(av_seek_frame(videoFileRead.avFormatContext, videoFileRead.videoStreamIndex, index, AVSEEK_FLAG_BACKWARD) < 0) {

        printf("failed to seek frame\n");
        return false;
    }

    avcodec_flush_buffers(videoDecoder.avCodecContext);

    Packet packet;


    do{


        if(videoFileRead.readPacket(packet) == true){

            if(packet.getPacketType() == PacketType::VideoPacket){

                videoDecoder.decode(packet,frame);
            }
        }


    }while (frame.getFrameIndex() < index);



    return true;
}
