
#ifndef VIDEOFILEUTIL_H
#define VIDEOFILEUTIL_H


class VideoFileRead;
class VideoDecoder;
class Frame;

class VideoFileUtil {

public:
    VideoFileUtil();
    ~VideoFileUtil();


public:
    static bool seek(int index, VideoFileRead &videoFileRead, VideoDecoder &videoDecoder, Frame &frame);


};

#endif // VIDEOFILEUTIL_H
