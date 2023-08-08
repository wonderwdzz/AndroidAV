
#ifndef MYMUSIC_WLCALLJAVA_H
#define MYMUSIC_WLCALLJAVA_H

#include "jni.h"
#include <linux/stddef.h>
//#include <jni.h>
#include "AndroidLog.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1


class MNCallJava {

public:
    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;

    jmethodID jmid_parpared;

public:
    MNCallJava(_JavaVM *javaVM, JNIEnv *env, jobject *obj);
    ~MNCallJava();

    void onCallParpared(int type);

};


#endif //MYMUSIC_WLCALLJAVA_H
