package com.wd.mmkv;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
//cpu
    ManiuBinder maniuBinder;
    int a;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        checkPermission();
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
//for  1000次   mmkv
        maniuBinder = new ManiuBinder();

        long start = System.currentTimeMillis();
//        44ms   这  mmkv  就好  物理
//        缺点比较耗
//        1个字节
//        127   实际上 4k
//        多虑     Class ---》    写
        for (int i = 0; i < 1000; i++) {
            maniuBinder.write();
        }

        long time = (System.currentTimeMillis() - start);
        Log.i("David", "mmkv  putString: 时间花销  "+time);


    }

    public void jump(View view) {
        Intent intent = new Intent(this, SecondActivity.class);
        startActivity(intent);
    }
}
