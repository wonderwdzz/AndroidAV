package com.wd.wdface;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity  implements SurfaceHolder.Callback, Camera
        .PreviewCallback  {

    static {
        System.loadLibrary("native-lib");
    }


    private CameraHelper cameraHelper;
    int cameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SurfaceView surfaceView = findViewById(R.id.surfaceView);
        checkPermission();
        surfaceView.getHolder().addCallback(this);
        cameraHelper = new CameraHelper(cameraId);
        cameraHelper.setPreviewCallback(this);
        Utils.copyAssets(this, "lbpcascade_frontalface.xml");
//        Utils.copyAssets(this, "cascade.xml");

    }
    public boolean checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && checkSelfPermission(
                Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.CAMERA
            }, 1);

        }
        return false;
    }
    @Override
    protected void onResume() {
        super.onResume();
//        初始化模型的意思 cascade
        init("/sdcard/lbpcascade_frontalface.xml");
//        init("/sdcard/cascade.xml");
        cameraHelper.startPreview();
    }
    @Override
    protected void onStop() {
        super.onStop();
        cameraHelper.stopPreview();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

//        surface --->native 层进行渲染
//        gif图    ----》 bitmap 内存  =赋值
//        surface

    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        setSurface(holder.getSurface());
    }
    //    native 画布
    native void setSurface(Surface surface);
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
//        Log.i("David", "onPreviewFrame: -------------->");
        postData(data, CameraHelper.WIDTH, CameraHelper.HEIGHT, cameraId);

    }

    native void init(String model);

    native void postData(byte[] data, int w, int h, int cameraId);

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_UP) {
            cameraHelper.switchCamera();
            cameraId = cameraHelper.getCameraId();
        }
        return super.onTouchEvent(event);
    }

}

