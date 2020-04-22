
#ifndef VIDEOFILEUTIL_H
#define VIDEOFILEUTIL_H


class VideoFileRead;
class VideoDecoder;

class VideoFileUtil {

public:
    VideoFileUtil();
    ~VideoFileUtil();


public:
    static bool seek(int index, VideoFileRead *videoFileRead, VideoDecoder *videoDecoder);


};

#endif // VIDEOFILEUTIL_H
