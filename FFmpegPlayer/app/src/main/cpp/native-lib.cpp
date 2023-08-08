#include <jni.h>
#include <string>
//C语言 编译器

//C++ 编译
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include <libavutil/time.h>
}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_maniu_ffmpegpalyer_MainActivity_stringFromJNI(
//        JNIEnv* env,
//        jobject /* this */) {
////    返回ffempg的配置信息
////兼容性  不存 cpu
////ffmepg 命令 在
//    std::string hello = avcodec_configuration();
//    return env->NewStringUTF(hello.c_str());
//}
#include <android/log.h>

#include <android/native_window_jni.h>


#define LOGD(...) __android_log_print(ANDROID_LOG_INFO,"David",__VA_ARGS__)

static AVFormatContext *avFormatContext;
static AVCodecContext *avCodecContext;
AVCodec *vCodec;
ANativeWindow* nativeWindow;
ANativeWindow_Buffer windowBuffer;

static AVPacket *avPacket;
static AVFrame *avFrame, *rgbFrame;
struct SwsContext *swsContext;
uint8_t *outbuffer;


extern "C"
JNIEXPORT void JNICALL
Java_com_wd_ffmpegplayer_MainActivity_string(JNIEnv *env, jobject thiz){
    std::string hello = avcodec_configuration();
    LOGD("%s",hello.c_str());
}extern "C"
JNIEXPORT jint JNICALL
Java_com_wd_ffmpegplayer_MainActivity_play(JNIEnv *env, jobject thiz, jstring url_,
                                           jobject surface) {
    const char *url = env->GetStringUTFChars(url_, 0);
//    注册所有的组件
    avcodec_register_all();
    //实例化了上下文
    avFormatContext=avformat_alloc_context();
//打开视频文件    视频流
    //打开文件
    if(avformat_open_input(&avFormatContext,url,NULL,NULL)!=0){
        LOGD("Couldn't open input stream.\n");
        return -1;
    }
    LOGD("打开视频成功.\n");
    //查找文件的流信息  mp4  失败  的视频
    if(avformat_find_stream_info(avFormatContext,NULL)<0){
        LOGD("Couldn't find stream information.\n");
        return -1;
    }
//
    int videoindex = -1;
//    2   个
    for(int i=0; i<avFormatContext->nb_streams; i++) {
        if (avFormatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    }
//视频流索引
    if(videoindex == -1){
        LOGD("Couldn't find a video stream.\n");
        return -1;
    }
    LOGD("找到了视频流\n");


    //video/avc
//解码器上下文
    avCodecContext=avFormatContext->streams[videoindex]->codec;
    vCodec=avcodec_find_decoder(avCodecContext->codec_id);
//   视频流  h264   h265
//打开器
    //打开解码器
    if(avcodec_open2(avCodecContext, vCodec,NULL)<0){
        LOGD("Couldn't open codec.\n");
        return -1;
    }
    LOGD("打开了解码成功\n");
    //获取界面传下来的surface
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (0 == nativeWindow){
        LOGD("Couldn't get native window from surface.\n");
        return -1;
    }
//    三个容器
//ffmpeg实例化  avFrame   实例化他的大小
    avFrame = av_frame_alloc();
//
//ffmpeg实例化  avFrame
    avPacket = av_packet_alloc();

//实例化    rgb
    rgbFrame = av_frame_alloc();
    int width = avCodecContext->width;
    int height = avCodecContext->height;
    //
//    输入有关系avFrame    跟输出有关系 surface
//输入有关系avFrame
    int numBytes =   av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height,1 );
    LOGD("计算解码后的rgb %d\n",numBytes);
//实例化一个输入缓冲区
    outbuffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
//缓冲区 设置给 rgbferame
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, outbuffer, AV_PIX_FMT_RGBA, width,
                         height, 1);

//   转换器
    swsContext = sws_getContext(width, height, avCodecContext->pix_fmt,
                                width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);


    if (0 > ANativeWindow_setBuffersGeometry(nativeWindow,width,height,WINDOW_FORMAT_RGBA_8888)){
        LOGD("Couldn't set buffers geometry.\n");
        ANativeWindow_release(nativeWindow);
        return -1;
    }
    LOGD("ANativeWindow_setBuffersGeometry成功\n");
//

    while (av_read_frame(avFormatContext, avPacket)>=0) {

//        读出来的数据是什么数据 视频   音频数据不管
        if (avPacket->stream_index == videoindex) {
            int ret = avcodec_send_packet(avCodecContext, avPacket);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF){
                LOGD("解码出错");
                return -1;
            }

            ret = avcodec_receive_frame(avCodecContext, avFrame);
            if (ret == AVERROR(EAGAIN)) { //我还要
                continue;
            } else if (ret < 0) {
                break;
            }
//            未压缩的数据
            sws_scale(swsContext, avFrame->data, avFrame->linesize, 0, avCodecContext->height,
                      rgbFrame->data, rgbFrame->linesize);
            if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
                LOGD("cannot lock window");
            } else {

                //将图像绘制到界面上，注意这里pFrameRGBA一行的像素和windowBuffer一行的像素长度可能不一致
                //需要转换好，否则可能花屏
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                for (int h = 0; h < height; h++)
                {
                    memcpy(dst + h * windowBuffer.stride * 4,
                           outbuffer + h * rgbFrame->linesize[0],
                           rgbFrame->linesize[0]);
                }
                switch(avFrame->pict_type){
                    case AV_PICTURE_TYPE_I:
                        LOGD("I");
                        break;
                    case AV_PICTURE_TYPE_P:
                        LOGD("P");
                        break;
                    case AV_PICTURE_TYPE_B:
                        LOGD("B");
                        break;
                    default:
                        ;break;
                }
            }
//            音频解码     健壮    opengl
//window
            av_usleep(1000 * 33);
            ANativeWindow_unlockAndPost(nativeWindow);

//                解码
//avcodec_send_packet()
// avcodec_receive_frame();


//编码
//            avcodec_send_frame();
//            avcodec_receive_packet();



        }

    }


//    输出
    avformat_free_context(avFormatContext);
    env->ReleaseStringUTFChars(url_, url);

    return -1;


}