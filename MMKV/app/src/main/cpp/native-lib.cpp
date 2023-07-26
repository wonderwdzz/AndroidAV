#include <jni.h>
#include <string>

#include <jni.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <android/log.h>
extern "C" JNIEXPORT jstring JNICALL
Java_com_wd_mmkv_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_wd_mmkv_ManiuBinder_write(JNIEnv *env, jobject thiz) {
    // TODO: implement write()
    std::string file = "/sdcard/maniubinder.txt";
    int m_fd = open(file.c_str(), O_RDWR | O_CREAT, S_IRWXU);
//    设置文件大小
    ftruncate(m_fd, 4096);

//如果  随机分配内存区域  虚拟机地址
    int8_t *m_ptr= static_cast<int8_t *>(mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd,
                                              0));
    std::string data("码牛 用代码成就你成为大牛的梦想");

//    发生 1   用户空间 切换成  内核空间  没有  2
//    write(m_fd, data.c_str());

    memcpy(m_ptr, data.data(), data.size());
    __android_log_print(ANDROID_LOG_ERROR, "david", "写入数据:%s", data.c_str());
    munmap(m_ptr, 4096);
    //关闭文件
    close(m_fd);
}



extern "C"
JNIEXPORT void JNICALL
Java_com_wd_mmkv_ManiuBinder_readTest(JNIEnv *env, jobject thiz) {
    // TODO: implement write()
    std::string file = "/sdcard/maniubinder.txt";
    int m_fd = open(file.c_str(), O_RDWR | O_CREAT, S_IRWXU);
//    设置文件大小
    ftruncate(m_fd, 4096);
    __android_log_print(ANDROID_LOG_ERROR, "david", "開始读取数据");
//如果  随机分配内存区域  虚拟机地址  磁盘没关系
    int8_t *m_ptr= static_cast<int8_t *>(mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd,
                                              0));

//m_ptr   虚拟地址     mmu  翻译成物理地址

    char *buf = static_cast<char *>(malloc(100));
    memcpy(buf, m_ptr, 100);
    std::string result(buf);
    __android_log_print(ANDROID_LOG_ERROR, "david", "读取数据:%s", result.c_str());
    //取消映射
    munmap(m_ptr, 4096);
    //关闭文件
    close(m_fd);


}