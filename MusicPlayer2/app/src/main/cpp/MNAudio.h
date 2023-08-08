#ifndef MYMUSIC_WLAUDIO_H
#define MYMUSIC_WLAUDIO_H

#include "MNQueue.h"
#include "MNPlaystatus.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include <libswresample/swresample.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

class MNAudio {

public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecpar = NULL;
    MNQueue *queue = NULL;
    MNPlaystatus *playstatus = NULL;

    pthread_t thread_play;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = 0;
    uint8_t *buffer = NULL;
    int data_size = 0;
    int sample_rate = 0;

    // 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //pcm
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

public:
    MNAudio(MNPlaystatus *playstatus, int sample_rate);
    ~MNAudio();

    void play();
    int resampleAudio();

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sample_rate);



};


#endif //MYMUSIC_WLAUDIO_H
