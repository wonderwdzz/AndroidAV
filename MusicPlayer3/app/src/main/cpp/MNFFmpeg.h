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

    int duration = 0;
    pthread_mutex_t seek_mutex;

public:
    MNFFmpeg(MNPlaystatus *playstatus, MNCallJava *callJava, const char *url);
    ~MNFFmpeg();

    void parpared();
    void decodeFFmpegThread();
    void start();
    void pause();
    void seek(int64_t secds);
    void resume();
    void setMute(int mute);
    void setVolume(int percent);
};


#endif //MYMUSIC_WLFFMPEG_H
