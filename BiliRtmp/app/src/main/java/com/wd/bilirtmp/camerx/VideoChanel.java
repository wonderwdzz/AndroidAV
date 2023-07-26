package com.wd.bilirtmp.camerx;

import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.TextureView;
import android.view.ViewGroup;

import androidx.camera.core.CameraX;
import androidx.camera.core.ImageAnalysis;
import androidx.camera.core.ImageAnalysisConfig;
import androidx.camera.core.ImageProxy;
import androidx.camera.core.Preview;
import androidx.camera.core.PreviewConfig;
import androidx.lifecycle.LifecycleOwner;

import com.wd.bilirtmp.live.channel.ImageUtil;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.locks.ReentrantLock;

import static com.wd.bilirtmp.FileUtils.writeBytes;
import static com.wd.bilirtmp.FileUtils.writeContent;

public class VideoChanel implements Preview.OnPreviewOutputUpdateListener, ImageAnalysis.Analyzer {
    private static final String TAG = "rtmp";
    int width = 480;
    int height = 640;
    private HandlerThread handlerThread;
    //    直播中  480 640
    private CameraX.LensFacing currentFacing = CameraX.LensFacing.BACK;
    private TextureView textureView;
    public VideoChanel(LifecycleOwner lifecycleOwner, TextureView textureView) {
        this.textureView = textureView;
        //子线程中回调
        handlerThread = new HandlerThread("Analyze-thread");
        handlerThread.start();
        CameraX.bindToLifecycle(lifecycleOwner, getPreView(), getAnalysis());
    }

    private Preview getPreView() {
//        480 *640  是 1 不是  2 Cmaera
        PreviewConfig previewConfig = new PreviewConfig.Builder().setTargetResolution(new Size(width, height)).setLensFacing(currentFacing).build();
        Preview preview = new Preview(previewConfig);
        preview.setOnPreviewOutputUpdateListener(this);
        return preview;
    }

    private ImageAnalysis getAnalysis() {
        ImageAnalysisConfig imageAnalysisConfig = new ImageAnalysisConfig.Builder()
                .setCallbackHandler(new Handler(handlerThread.getLooper()))
                .setLensFacing(currentFacing)
                .setImageReaderMode(ImageAnalysis.ImageReaderMode.ACQUIRE_LATEST_IMAGE)
                .setTargetResolution(new Size(width, height))
                .build();

        ImageAnalysis imageAnalysis = new ImageAnalysis(imageAnalysisConfig);
        imageAnalysis.setAnalyzer(this);
        return imageAnalysis;
    }
//摄像
    @Override
    public void onUpdated(Preview.PreviewOutput output) {

        SurfaceTexture surfaceTexture = output.getSurfaceTexture();
        if (textureView.getSurfaceTexture() != surfaceTexture) {
            if (textureView.isAvailable()) {
                // 当切换摄像头时，会报错
                ViewGroup parent = (ViewGroup) textureView.getParent();
                parent.removeView(textureView);
                parent.addView(textureView, 0);
                parent.requestLayout();
            }
            textureView.setSurfaceTexture(surfaceTexture);
        }

    }
    private ReentrantLock lock = new ReentrantLock();
    private byte[] y;
    private byte[] u;
    private byte[] v;
    private MediaCodec mediaCodec;
    private byte[] nv21;
    byte[] nv21_rotated;
    byte[] nv12;
    @Override
    public void analyze(ImageProxy image, int rotationDegrees) {
        Log.i("rtmp", "analyze: " + image.getWidth() + "  height " + image.getHeight());
        lock.lock();
//          能够 播放  H264码流   x264     摸索   x264
//3 ge
        ImageProxy.PlaneProxy[] planes =  image.getPlanes();
        // 重复使用同一批byte数组，减少gc频率
        if (y == null) {
//            初始化y v  u
            y = new byte[planes[0].getBuffer().limit() - planes[0].getBuffer().position()];
            u = new byte[planes[1].getBuffer().limit() - planes[1].getBuffer().position()];
            v = new byte[planes[2].getBuffer().limit() - planes[2].getBuffer().position()];
        }

        if (image.getPlanes()[0].getBuffer().remaining() == y.length) {
            planes[0].getBuffer().get(y);
            planes[1].getBuffer().get(u);
            planes[2].getBuffer().get(v);
            int stride = planes[0].getRowStride();
            Size size = new Size(image.getWidth(), image.getHeight());
            int width = size.getHeight();
            int heigth = image.getWidth();
            Log.i(TAG, "analyze: "+width+"  heigth "+heigth);
            if (nv21 == null) {
                nv21 = new byte[heigth * width * 3 / 2];
                nv21_rotated = new byte[heigth * width * 3 / 2];
            }

            if (mediaCodec == null) {
                initCodec(size);
            }
            ImageUtil.yuvToNv21(y, u, v, nv21, heigth, width);
            ImageUtil.nv21_rotate_to_90(nv21, nv21_rotated, heigth, width);
            byte[] temp = ImageUtil.nv21toNV12(nv21_rotated, nv12);
            MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
            int inIndex = mediaCodec.dequeueInputBuffer(100000);
            if (inIndex >= 0) {
                ByteBuffer byteBuffer = mediaCodec.getInputBuffer(inIndex);
                byteBuffer.clear();
                byteBuffer.put(temp, 0, temp.length);
                mediaCodec.queueInputBuffer(inIndex, 0, temp.length,
                        0, 0);
            }
            int outIndex = mediaCodec.dequeueOutputBuffer(info, 100000);
            if (outIndex >= 0) {
                ByteBuffer byteBuffer = mediaCodec.getOutputBuffer(outIndex);
                byte[] ba = new byte[byteBuffer.remaining()];
                byteBuffer.get(ba);
                Log.e("rtmp", "ba = " + ba.length + "");
                writeContent(ba);
                writeBytes(ba);
                mediaCodec.releaseOutputBuffer(outIndex, false);
            }
        }

        lock.unlock();

    }

    private void initCodec(Size size) {
        try {
            mediaCodec = MediaCodec.createEncoderByType("video/avc");

            final MediaFormat format = MediaFormat.createVideoFormat("video/avc",
                    size.getHeight(), size.getWidth());
            //设置帧率
            format.setInteger(MediaFormat.KEY_COLOR_FORMAT,
                    MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible);
            format.setInteger(MediaFormat.KEY_FRAME_RATE, 15);
            format.setInteger(MediaFormat.KEY_BIT_RATE, 8000_000);
            format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 2);//2s一个I帧
            mediaCodec.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mediaCodec.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}