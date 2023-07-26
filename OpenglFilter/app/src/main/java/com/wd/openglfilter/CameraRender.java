package com.wd.openglfilter;

import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.Log;

import androidx.camera.core.Preview;
import androidx.lifecycle.LifecycleOwner;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class CameraRender implements GLSurfaceView.Renderer, Preview.OnPreviewOutputUpdateListener, SurfaceTexture.OnFrameAvailableListener {
    private static final String TAG = "david";
    private CameraHelper cameraHelper;
    private CameraView cameraView;
    private SurfaceTexture mCameraTexure;

//    int
    private ScreenFilter screenFilter;
    private  int[] textures;
    float[] mtx = new float[16];
    public CameraRender(CameraView cameraView) {
        this.cameraView = cameraView;
        LifecycleOwner lifecycleOwner = (LifecycleOwner) cameraView.getContext();
//        打开摄像头
        cameraHelper = new CameraHelper(lifecycleOwner, this);

    }

//textures onSurfaceCreated、onSurfaceChanged、onDrawFrame这三个方法都是GLSurfaceView渲染的回调
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
//surface SurfaceView创建时调用
        textures = new int[1];
//        1
//        让 SurfaceTexture   与 Gpu  共享一个数据源  0-31
        mCameraTexure.attachToGLContext(textures[0]);
//监听摄像头数据回调，mCameraTexure发生变化回调onFrameAvailable
        mCameraTexure.setOnFrameAvailableListener(this);
        screenFilter = new ScreenFilter(cameraView.getContext());
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
//SurfaceView改变时调用

        screenFilter.setSize(width,height);
    }
//SurfaceView绘制时调用
    @Override
    public void onDrawFrame(GL10 gl) {
        Log.i(TAG, "onDrawFrame 线程: " + Thread.currentThread().getName());
//        拿最新摄像头的数据  ---》
//        更新摄像头的数据  给了  gpu
        mCameraTexure.updateTexImage();
//        从CameraTexure拿到SurfaceTexture的变换矩阵给GPU
//      这个矩阵帮助GPU从纹理坐标正确渲染
        mCameraTexure.getTransformMatrix(mtx);
        screenFilter.setTransformMatrix(mtx);
//int   数据   byte[]  往GPU0图层绘制
        screenFilter.onDraw(textures[0]);
    }
//
    @Override
    public void onUpdated(Preview.PreviewOutput output) {
//        摄像头预览到的数据 在这里原始的camera数据给到gpu
        Log.i(TAG, "onUpdated 线程: " + Thread.currentThread().getName());
        mCameraTexure=output.getSurfaceTexture();


    }
//当有数据 过来的时候
    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
//一帧 一帧回调时  当onUpdated调用后mCameraTexure发生变化 于是这里会被调用 请求渲染onDrawFrame被调用
        Log.i(TAG, "onFrameAvailable线程: " + Thread.currentThread().getName());
        cameraView.requestRender();


    }
}
