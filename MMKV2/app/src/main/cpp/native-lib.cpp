#include <jni.h>
#include <string>
#include <jni.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <android/log.h>
#include "MMKV.h"
extern "C"
JNIEXPORT void JNICALL
Java_com_wd_mmkv2_MMKV_jniInitialize(JNIEnv *env, jclass clazz, jstring root_dir) {
    const char *rootDir = env->GetStringUTFChars(root_dir, 0);

    MMKV::initializeMMKV(rootDir);

    env->ReleaseStringUTFChars(root_dir, rootDir);

}extern "C"
JNIEXPORT jlong JNICALL
Java_com_wd_mmkv2_MMKV_getDefaultMMKV(JNIEnv *env, jclass clazz) {
    MMKV *kv = MMKV::defaultMMKV();
    return reinterpret_cast<jlong>(kv);
}extern "C"
JNIEXPORT void JNICALL
Java_com_wd_mmkv2_MMKV_putInt(JNIEnv *env, jobject thiz, jlong handle, jstring key_, jint value) {
    const char *key = env->GetStringUTFChars(key_, 0);
    MMKV *kv = reinterpret_cast<MMKV *>(handle);
    kv->putInt(key, value);
    env->ReleaseStringUTFChars(key_, key);
}extern "C"
JNIEXPORT jint JNICALL
Java_com_wd_mmkv2_MMKV_getInt(JNIEnv *env, jobject thiz, jlong handle, jstring key_,
                              jint defaultValue) {
    const char *key = env->GetStringUTFChars(key_, 0);
    MMKV *kv = reinterpret_cast<MMKV *>(handle);
    int returnValue = kv->getInt(key, defaultValue);

    env->ReleaseStringUTFChars(key_, key);
    return returnValue;
}