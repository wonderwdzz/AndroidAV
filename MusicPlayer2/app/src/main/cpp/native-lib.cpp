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

}