
#include "videoplayer.h"
#include <qdebug.h>

VideoPlayer::VideoPlayer(const QString &filePath)
    :filePath(filePath), videoRenderWidget(nullptr) {

}


VideoPlayer::~VideoPlayer() {


}

void VideoPlayer::play() {


    if(this->videoRenderWidget == nullptr){
        qDebug("VideoPlayer::play() videoRenderWidget is not set");
        return;
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
