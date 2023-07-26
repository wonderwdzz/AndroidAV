package com.wd.bilirtmp.camerx;

import android.Manifest;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.os.Build;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.wd.bilirtmp.R;
import com.wd.bilirtmp.live.LivePusher;


public class MainActivity extends AppCompatActivity {

//H264码流
    private LivePusher livePusher;
    private TextureView textureView;
    private String url = "rtmp://live-push.bilivideo.com/live-bvc/?streamname=live_345162489_81809986&key=03693092c85bd15a1d3fbbc227da0ad1&schedule=rtmp";
    VideoChanel videoChanel;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camerax);
        checkPermission();
        textureView = findViewById(R.id.textureView);
        checkPermission();
        videoChanel = new VideoChanel(this,textureView);

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

    public void switchCamera(View view) {
        livePusher.switchCamera();
    }

    public void startLive(View view) {
        livePusher.startLive(url);
    }

    public void stopLive(View view) {
        livePusher.stopLive();
    }

}
