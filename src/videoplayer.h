
#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H


#include <qstring.h>

class VideoRenderWidget;


class VideoPlayer {

private:
    QString filePath;
    VideoRenderWidget *videoRenderWidget;

public:
    VideoPlayer(const QString &filePath);
    ~VideoPlayer();

    void setVideoRenderWidget(VideoRenderWidget *videoRenderWidget) { this->videoRenderWidget = videoRenderWidget; }

    void play();
    void stop();
    void pause();
    void resume();

};

#endif // VIDEOPLAYER_H
