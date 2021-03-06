
#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H


#include <qstring.h>
#include <qobject.h>

class VideoFileRead;

class VideoRenderWidget;
class VideoDecoder;


class VideoPlayer : public QObject {

private:
    QString filePath;
    VideoRenderWidget *videoRenderWidget;

private:
    VideoFileRead *videoFile;
    VideoDecoder *videoDecoder;

public:
    VideoPlayer(const QString &filePath);
    ~VideoPlayer();

    void setVideoRenderWidget(VideoRenderWidget *videoRenderWidget) { this->videoRenderWidget = videoRenderWidget; }

    void play();
    void stop();
    void pause();
    void resume();

private slots:
    void timePassed();

};

#endif // VIDEOPLAYER_H
