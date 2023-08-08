#ifndef MYMUSIC_WLFFMPEG_H
#define MYMUSIC_WLFFMPEG_H

#include "MNCallJava.h"
#include "pthread.h"
#include "MNAudio.h"
#include "MNPlaystatus.h"

extern "C"
{
#include "libavformat/avformat.h"
};


class MNFFmpeg {

public:
    MNCallJava *callJava = NULL;
    const char* url = NULL;
    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL;
    MNAudio *audio = NULL;
    MNPlaystatus *playstatus = NULL;



public:
    MNFFmpeg(MNPlaystatus *playstatus, MNCallJava *callJava, const char *url);
    ~MNFFmpeg();

    void parpared();
    void decodeFFmpegThread();
    void start();

};


#endif //MYMUSIC_WLFFMPEG_H
