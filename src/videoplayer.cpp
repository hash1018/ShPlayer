
#include "videoplayer.h"
#include <qdebug.h>
#include "src/shplayer-ffmpeg/videofile.h"
#include "src/ui/videorenderwidget.h"
#include "src/shplayer-ffmpeg/videodecoder.h"
#include <qtimer.h>
#include "src/shplayer-ffmpeg/videofileutil.h"

VideoPlayer::VideoPlayer(const QString &filePath)
    :filePath(filePath), videoRenderWidget(nullptr), videoFile(nullptr), videoDecoder(nullptr) {


    this->videoFile = new VideoFileRead;

    if(this->videoFile->open(filePath.toUtf8(), PacketType::VideoPacket) == false){

        qDebug() << "VideoPlayer::VideoPlayer failed to open demuxer";
    }
    else{

        qDebug() << this->videoFile->getDuration();
        qDebug () << "VideoPlayer::VideoPlayer succeeded to open demuxer";
    }
}


VideoPlayer::~VideoPlayer() {

    if(this->videoFile != nullptr)
        delete this->videoFile;

}

void VideoPlayer::play() {

    qDebug() << "VideoPlayer::play";

    if(this->videoRenderWidget == nullptr){
        qDebug("VideoPlayer::play() videoRenderWidget is not set");
        return;
    }

    this->videoRenderWidget->setVideoSize(this->videoFile->getWidth(), this->videoFile->getHeight());


    if(this->videoFile->isOpened() == true) {

        CodecInfo codecInfo;
        if(this->videoFile->getVideoCodecInfo(codecInfo) ==false){

            qDebug() <<" failed to get VideoCodecInfo";
            return;
        }

        if(this->videoDecoder == nullptr){

            this->videoDecoder = new VideoDecoder;
            videoDecoder->setCodec(codecInfo);
        }

        Frame frame;
        VideoFileUtil::seek(50, *this->videoFile, *this->videoDecoder, frame);

        QTimer::singleShot(this->videoFile->getVideoFps(), this, &VideoPlayer::timePassed);
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

#include <qelapsedtimer.h>
void VideoPlayer::timePassed() {

    qDebug() << "VideoPlayer::timePassed";
    Packet packet;
    Frame frame;

    QElapsedTimer timer;
    timer.start();

    if(this->videoFile->readPacket(packet) == true) {

        if(packet.getPacketType() == PacketType::VideoPacket) {

            if(this->videoDecoder->decode(packet,frame)==true) {

                qDebug () << "frame Index  "  << frame.getFrameIndex();
                qDebug() << "frame pts " << frame.getPts();
                qDebug() << "frame realtime  "  << frame.getPtsRealTime();
                 qDebug() << " timer elapsed   " << timer.elapsed();

                this->videoRenderWidget->update(frame.getBuffer(),frame.getBufferSize());



            }
            QTimer::singleShot(this->videoFile->getVideoFps(),this,&VideoPlayer::timePassed);
        }
    }
    else{

        qDebug() << "Dsadsadsad";
    }




}
