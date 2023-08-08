#ifndef MYMUSIC_WLAUDIO_H
#define MYMUSIC_WLAUDIO_H

#include "MNQueue.h"
#include "MNPlaystatus.h"
#include "MNCallJava.h"

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
    SLVolumeItf pcmVolumePlay = NULL;
    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

//    -------------新加的----------------
    int duration = 0;
//时间单位         总时间/帧数   单位时间     *   时间戳= pts  * 总时间/帧数
    AVRational time_base;
//当前时间
    double now_time;//当前frame时间

    double clock;//当前播放的时间    准确时间

    MNCallJava *callJava = NULL;
    double last_tiem; //上一次调用时间
//立体声
    int mute = 2;
    SLMuteSoloItf  pcmMutePlay = NULL;
    int volumePercent = 100;

public:
    MNAudio(MNPlaystatus *playstatus, int sample_rate,MNCallJava *callJava);
    ~MNAudio();

    void play();
    int resampleAudio();

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sample_rate);
    void onCallTimeInfo(int type, int curr, int total);
    void pause();

    void resume();
    void setMute(int mute);

    void setVolume(int percent);
};


#endif //MYMUSIC_WLAUDIO_H
