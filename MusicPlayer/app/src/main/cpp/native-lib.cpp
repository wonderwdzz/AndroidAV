#include <jni.h>
#include <string>
#include <android/log.h>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
};
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, "David", __VA_ARGS__)
extern "C" JNIEXPORT jstring JNICALL
Java_com_wd_musicplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_wd_musicplayer_MainActivity_playSound(JNIEnv *env, jobject instance, jstring input_) {
    const char *input = env->GetStringUTFChars(input_, 0);

    av_register_all();
//    总上下文
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, input, NULL, NULL) != 0) {
        LOGE("%s","打开输入视频文件失败");
        return;
    }

    if(avformat_find_stream_info(pFormatCtx,NULL) < 0){
        LOGE("%s","获取视频信息失败");
        return;
    }
    int audio_stream_idx=-1;
    int i=0;
    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            LOGE("  找到音频id %d", pFormatCtx->streams[i]->codec->codec_type);
            audio_stream_idx=i;
            break;
        }
    }
//    找到了音频索引

//找到解码器上下文
    AVCodecContext *pCodecCtx= pFormatCtx->streams[audio_stream_idx]->codec;
    //获取解码器  视频 1   音频2
    AVCodec *pCodex = avcodec_find_decoder(pCodecCtx->codec_id);
    //打开解码器
    if (avcodec_open2(pCodecCtx, pCodex, NULL)<0) {
        return;
    }
    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    //申请avframe，装解码后的数据
    AVFrame *frame = av_frame_alloc();
    int out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
//转换器上下文
    SwrContext *swrContext = swr_alloc();
    uint64_t  out_ch_layout=AV_CH_LAYOUT_STEREO;
    enum AVSampleFormat out_formart=AV_SAMPLE_FMT_S16;
    int out_sample_rate = pCodecCtx->sample_rate;
//    转换器的代码
    swr_alloc_set_opts(swrContext, out_ch_layout, out_formart, out_sample_rate,
//            输出的
                       pCodecCtx->channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0,NULL
    );


//    初始化转化上下文
    swr_init(swrContext);
//    1s的pcm个数
    uint8_t *out_buffer = (uint8_t *) av_malloc(44100 * 2);
//    反射的放射的方式
    jclass david_player = env->GetObjectClass(instance);
    jmethodID createAudio = env->GetMethodID(david_player, "createTrack", "(II)V");
    env->CallVoidMethod(instance, createAudio, 44100, out_channer_nb);
    jmethodID audio_write = env->GetMethodID(david_player, "playTrack", "([BI)V");
    while (av_read_frame(pFormatCtx, packet) >= 0) {

        if (packet->stream_index == audio_stream_idx) {
//            音频的数据
            int ret = avcodec_send_packet(pCodecCtx, packet);
            LOGE("解码成功%d",ret);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF){
                LOGE("解码出错");
                break;
            }
            ret = avcodec_receive_frame(pCodecCtx, frame);
            if (ret < 0 && ret != AVERROR_EOF){
                LOGE("读取出错");
                break;
            }
            if (ret>=0) {

//输出的我们写完了    我们再写输入数据
                swr_convert(swrContext, &out_buffer, 44100 * 2,
                            (const uint8_t **)(frame->data), frame->nb_samples);

//                解码了
                int size = av_samples_get_buffer_size(NULL, out_channer_nb, frame->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);
//java的字节数组
                jbyteArray audio_sample_array = env->NewByteArray(size);
                env->SetByteArrayRegion(audio_sample_array, 0, size,
                                        reinterpret_cast<const jbyte *>(out_buffer));
                env->CallVoidMethod(instance, audio_write, audio_sample_array, size);
                env->DeleteLocalRef(audio_sample_array);
            }
        }
    }

    env->ReleaseStringUTFChars(input_, input);
}