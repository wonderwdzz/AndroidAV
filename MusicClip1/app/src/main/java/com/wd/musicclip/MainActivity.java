package com.wd.musicclip;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.pm.PackageManager;
import android.content.res.AssetFileDescriptor;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;


public class MainActivity extends AppCompatActivity {
    MusicProcess musicProcess;
    private String TAG =  "musicclip";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        checkPermission();
        musicProcess = new MusicProcess();
    }
    public boolean checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && checkSelfPermission(
                Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE
            }, 1);

        }
        return false;
    }
    public void clip(View view) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                boolean success = true;
                final String aacPath = new File(Environment.getExternalStorageDirectory(), "music.mp3").getAbsolutePath();
                final String outPath = new File(Environment.getExternalStorageDirectory(), "out.mp3").getAbsolutePath();
                try {
                    copyAssets("music.mp3", aacPath);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                try {
                    Log.d(TAG, "run: aacPath="+aacPath+" outPath="+outPath);
                    musicProcess.clip(aacPath,outPath,10*1000*1000,15*1000*1000);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }
    private void copyAssets(String assetsName, String path) throws IOException {
        AssetFileDescriptor assetFileDescriptor = getAssets().openFd(assetsName);
        FileChannel from = new FileInputStream(assetFileDescriptor.getFileDescriptor()).getChannel();
        FileChannel to = new FileOutputStream(path).getChannel();
        from.transferTo(assetFileDescriptor.getStartOffset(), assetFileDescriptor.getLength(), to);
    }
}
