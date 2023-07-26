package com.wd.bilirtmp.live;

import android.app.Activity;
import android.util.Log;
import android.view.SurfaceHolder;

import com.wd.bilirtmp.live.channel.AudioChannel;
import com.wd.bilirtmp.live.channel.VideoChannel;
import com.wd.bilirtmp.FileUtils;
public class LivePusher {


    static {
        System.loadLibrary("native-lib");
    }

    private AudioChannel audioChannel;
    private VideoChannel videoChannel;

    public LivePusher(Activity activity, int width, int height, int bitrate,
                      int fps, int cameraId) {
        native_init();
        if (videoChannel == null) {
            return;
        }
        videoChannel = new VideoChannel(this,activity, width, height, bitrate, fps, cameraId);
        audioChannel = new AudioChannel();
    }

    public void setPreviewDisplay(SurfaceHolder surfaceHolder) {
        videoChannel.setPreviewDisplay(surfaceHolder);
    }

    public void switchCamera() {
        videoChannel.switchCamera();
    }
    private void onPrepare(boolean isConnect) {
        //通知UI
    }
    public void startLive(String path) {
        native_start(path);
        if (videoChannel == null) {
            return;
        }
        videoChannel.startLive();
        audioChannel.startLive();
    }

    public void stopLive(){
        native_stop();
        if (videoChannel == null) {
            return;
        }
        videoChannel.stopLive();
        audioChannel.stopLive();
    }

//    jni回调java层的方法  byte[] data    char *data
    private void postData(byte[] data) {
        if (videoChannel == null) {
            return;
        }
        Log.i("rtmp", "postData: "+data.length);
        FileUtils.writeBytes(data);
        FileUtils.writeContent(data);
    }

    public void sendAudio(byte[] buffer, int len) {
        nativeSendAudio(buffer, len);
    }
    public native void native_init();

    public native void native_setVideoEncInfo(int width, int height, int fps, int bitrate);

    public native void native_start(String path);

    public native int initAudioEnc(int sampleRate, int channels);

    public native void native_pushVideo(byte[] data);

    public native void native_stop();

    public native void native_release();

    private native void nativeSendAudio(byte[] buffer, int len);
}
