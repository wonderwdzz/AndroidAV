package com.wd.mmkv2;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.util.HashMap;

public class MainActivity extends AppCompatActivity {

    private MMKV mmkv;
    int a;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        checkPermission();
//初始化文件夹 创建文件夹
        MMKV.initialize(this);
        mmkv = MMKV.defaultMMKV();

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

    public void write(View view) {
        long start = System.currentTimeMillis();
        for (int i = 0; i < 1000; i++) {
            mmkv.putInt("name"+i , i);
        }
        long time = (System.currentTimeMillis() - start);
        Log.i("David", "SharedPreferences  putString: 时间花销  "+time);

    }

    public void jump(View view) {
        Toast.makeText(this, "---------->" + mmkv.getInt("name2", -1), Toast.LENGTH_SHORT).show();
    }
}
