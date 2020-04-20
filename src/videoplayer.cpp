
#include "videoplayer.h"
#include <qdebug.h>
#include "src/shplayer-ffmpeg/demuxer.h"
#include "src/ui/videorenderwidget2.h"
#include "src/shplayer-ffmpeg/videodecoder.h"
#include <qtimer.h>

VideoPlayer::VideoPlayer(const QString &filePath)
    :filePath(filePath), videoRenderWidget(nullptr), demuxer(nullptr), videoDecoder(nullptr) {


    this->demuxer = new Demuxer;

    if(this->demuxer->open(filePath.toUtf8(), PacketType::VideoPacket) == false){

        qDebug() << "VideoPlayer::VideoPlayer failed to open demuxer";
    }
    else{

        qDebug () << "VideoPlayer::VideoPlayer succeeded to open demuxer";
    }
}


VideoPlayer::~VideoPlayer() {

    if(this->demuxer != nullptr)
        delete this->demuxer;

}

void VideoPlayer::play() {

    qDebug() << "VideoPlayer::play";

    if(this->videoRenderWidget == nullptr){
        qDebug("VideoPlayer::play() videoRenderWidget is not set");
        return;
    }

    this->videoRenderWidget->setVideoSize(this->demuxer->getWidth(),this->demuxer->getHeight());


    if(this->demuxer->isOpened() == true) {

        CodecInfo codecInfo;
        if(this->demuxer->getVideoCodecInfo(codecInfo) ==false){

            qDebug() <<" failed to get VideoCodecInfo";
            return;
        }

        if(this->videoDecoder == nullptr){

            this->videoDecoder = new VideoDecoder;
            videoDecoder->setCodec(codecInfo);
        }

        QTimer::singleShot(this->demuxer->getVideoFps(), this, &VideoPlayer::timePassed);
    }
}

void VideoPlayer::stop() {

    if(this->videoRenderWidget == nullptr){
        qDebug("VideoPlayer::stop() videoRenderWidget is not set");
        return;
    }

}

void VideoPlayer::pause() {

    if(this->videoRenderWidget == nullptr){
        qDebug("VideoPlayer::pause() videoRenderWidget is not set");
        return;
    }
}

void VideoPlayer::resume() {

    if(this->videoRenderWidget == nullptr){
        qDebug("VideoPlayer::resume() videoRenderWidget is not set");
        return;
    }
}

void VideoPlayer::timePassed() {

    qDebug() << "VideoPlayer::timePassed";
    Packet packet;
    Frame frame;

    if(this->demuxer->readPacket(packet) == true) {

        if(packet.getPacketType() == PacketType::VideoPacket) {

            if(this->videoDecoder->decode(packet,frame)==true) {

                qDebug () << "frame Index  "  << frame.getFrameIndex();
                qDebug() << "frame pts " << frame.getPts();
                qDebug() << "frame realtime  "  << frame.getPtsRealTime();

                this->videoRenderWidget->update(frame.getBuffer(),frame.getBufferSize());



            }
            QTimer::singleShot(this->demuxer->getVideoFps(),this,&VideoPlayer::timePassed);
        }
    }
    else{

        qDebug() << "Dsadsadsad";
    }




}
