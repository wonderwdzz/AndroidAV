#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <android/log.h>
#include <android/native_window_jni.h>
using namespace cv;
//   检测器的  Adpater
DetectionBasedTracker *tracker = 0;
ANativeWindow *window = 0;
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"David",__VA_ARGS__)
//实例化适配器  方块  丢给 Adapter      图像 未知图像    关键点    提供
class CascadeDetectorAdapter:public  DetectionBasedTracker::IDetector{
public:
//    onBinderViewHodler     目的  View
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector): IDetector(),maniuDetector(detector){
    }
    void detect(const cv::Mat &image, std::vector<cv::Rect> &objects){
////
//        Detector->detectMultiScale(image, objects, scaleFactor, minNeighbours, 0, minObjSize,
//                                   maxObjSize);
        maniuDetector->detectMultiScale(image, objects, scaleFactor, minNeighbours, 0, minObjSize,
                                   maxObjSize);

    }
//     我传进来
private:
    CascadeDetectorAdapter();
//    分类器    作用    分类
    cv::Ptr<cv::CascadeClassifier> maniuDetector;
};

extern "C"
JNIEXPORT void JNICALL
Java_com_wd_wdface_MainActivity_init(JNIEnv *env, jobject thiz, jstring model_) {

    const char *model = env->GetStringUTFChars(model_, 0);
    LOGI("model:%s",model);
//    检测器 CascadeClassifier * cascadeClassifier=new CascadeClassifier(model)

//    CascadeClassifier *cascadeClassifier = new CascadeClassifier(model);
//智能指针   自己实现了析构函数  opencv    实例化 所有对象
    Ptr<CascadeClassifier> classifier = makePtr<CascadeClassifier>(model);

//Apdater
//opencv   jieguo
    //创建一个检测器
    Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(classifier);

    //创建一个跟踪器
    Ptr<CascadeClassifier> classifier1 = makePtr<CascadeClassifier>(model);
    Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(classifier1);


    DetectionBasedTracker::Parameters detectorParams;
    tracker=  new DetectionBasedTracker(mainDetector, trackingDetector, detectorParams);
    tracker->run();
//    CascadeDetectorAdapter *cascadeDetectorAdapter = new CascadeDetectorAdapter(classifier);
    env->ReleaseStringUTFChars(model_, model);
//    程序  IDetector
//DetectionBasedTracker    相当于RecyclerView

//IDetector       相当于适配器 Adapter
}
static int index = 0;
extern "C"
JNIEXPORT void JNICALL
Java_com_wd_wdface_MainActivity_postData(JNIEnv *env, jobject thiz, jbyteArray data_, jint w,
                                                jint h, jint cameraId) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
//    data  数据未知的数据
//图像的意思
//    Mat * mat = new Mat(h + h / 2, w, CV_8UC1, data);
//data   nv21
    Mat  src(h + h / 2, w, CV_8UC1, data);
    //颜色格式的转换 nv21->RGBA
    cvtColor(src, src, COLOR_YUV2RGBA_NV21);
    if (cameraId == 1) {

        rotate(src, src, ROTATE_90_COUNTERCLOCKWISE);
        flip(src, src, 1);
    } else {
//
        //顺时针旋转90度
        rotate(src, src, ROTATE_90_CLOCKWISE);
    }
    Mat gray;
    //灰色
    cvtColor(src, gray, COLOR_RGBA2GRAY);
    //增强对比度 (直方图均衡)
    equalizeHist(gray, gray);
    char p[100];
    mkdir("/sdcard/maniu/", 0777);

    tracker->process(gray);
//    faces   List<Rect>  = faces
    std::vector<Rect> faces;
//    检测到的结果   矩形框         人脸  键盘   被子
    tracker->getObjects(faces);

//    faces 有数据     意思是   识别 出来了   位置
    for (Rect face:faces) {
        sprintf(p, "/sdcard/maniu/%d.jpg", index++);
        Mat m;
        m=gray(face).clone();
        resize(m, m, Size(24, 24));
//        imwrite(p, m);

        LOGI(" 识别出David width: %d  height: %d",face.width, face.height);
//        原图
//        Scalar *scalar = new Scalar(0, 0, 255);
//        rectangle(src, face,*scalar);
//   Scalar(0, 0, 255)
        rectangle(src, face,Scalar(0, 0, 255));
    }
//     话一个框框  释放
//    数据画到SurfaceView
    if (window) {
//        初始化了
//        画面中   window  缓冲区 设置 大小


        do {
//            if (!window) {
//                break;
//            }
            ANativeWindow_setBuffersGeometry(window, src.cols, src.rows, WINDOW_FORMAT_RGBA_8888);
//            缓冲区    得到

            ANativeWindow_Buffer buffer;
            if(ANativeWindow_lock(window, &buffer, 0)) {
                ANativeWindow_release(window);
                window = 0;
            }
//            知道为什么*4   rgba
            int srclineSize = src.cols * 4;
//目的数据
            int dstlineSize = buffer.stride * 4;

//            待显示的缓冲区
            uint8_t *dstData  =   static_cast<uint8_t *>(buffer.bits);
//像素的数据源
//            dstData 目的 内存    src   数据源
//for循环行数   这个
            for (int i = 0; i < buffer.height; ++i) {
                memcpy(dstData+i*dstlineSize, src.data+i*srclineSize, srclineSize);
            }
            ANativeWindow_unlockAndPost(window);
        } while (0);
    }
    env->ReleaseByteArrayElements(data_, data, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wd_wdface_MainActivity_setSurface(JNIEnv *env, jobject thiz, jobject surface) {
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
//        渲染surface   --->window   --->windwo
    window= ANativeWindow_fromSurface(env, surface);

}