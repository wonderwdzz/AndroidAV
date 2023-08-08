#include <jni.h>
#include <string>
#include "MNFFmpeg.h"
#include "MNPlaystatus.h"

extern "C"
{
#include <libavformat/avformat.h>
}
_JavaVM *javaVM = NULL;
MNCallJava *callJava = NULL;
MNFFmpeg *fFmpeg = NULL;
MNPlaystatus *playstatus = NULL;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if(vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
    {

        return result;
    }
    return JNI_VERSION_1_4;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_wd_musicplayer2_player_MNPlayer_n_1parpared(JNIEnv *env, jobject instance, jstring source_) {
    const char *source = env->GetStringUTFChars(source_, 0);

    if(fFmpeg == NULL)
    {
        if(callJava == NULL)
        {
            callJava = new MNCallJava(javaVM, env, &instance);
        }
        playstatus = new MNPlaystatus();
        fFmpeg = new MNFFmpeg(playstatus, callJava, source);
        fFmpeg->parpared();
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_wd_musicplayer2_player_MNPlayer_n_1start(JNIEnv *env, jobject thiz) {
    // TODO
    if(fFmpeg != NULL)
    {
        fFmpeg->start();
    }

}extern "C"
JNIEXPORT void JNICALL
Java_com_wd_musicplayer2_player_MNPlayer_n_1resume(JNIEnv *env, jobject thiz) {
    if(fFmpeg != NULL)
    {
        fFmpeg->resume();
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_wd_musicplayer2_player_MNPlayer_n_1seek(JNIEnv *env, jobject thiz, jint secds) {
    LOGE("最开始%d  ", secds);
    if(fFmpeg != NULL)
    {
        fFmpeg->seek(secds);
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_wd_musicplayer2_player_MNPlayer_n_1pause(JNIEnv *env, jobject thiz) {
    if(fFmpeg != NULL)
    {
        fFmpeg->pause();
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_wd_musicplayer2_player_MNPlayer_n_1mute(JNIEnv *env, jobject thiz, jint mute) {
    if(fFmpeg != NULL)
    {
        fFmpeg->setMute(mute);
    }

}extern "C"
JNIEXPORT void JNICALL
Java_com_wd_musicplayer2_player_MNPlayer_n_1volume(JNIEnv *env, jobject thiz, jint percent) {
    if(fFmpeg != NULL)
    {
        fFmpeg->setVolume(percent);
    }
}