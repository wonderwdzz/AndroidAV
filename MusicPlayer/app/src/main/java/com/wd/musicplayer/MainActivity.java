package com.wd.musicplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    private AudioTrack audioTrack;
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button btn = (Button)findViewById(R.id.btn);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                File file = new File(Environment.getExternalStorageDirectory(), "input.mp3");
                playSound(file.getAbsolutePath());
            }
        });

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 5);
        }

    }
    public native void playSound(String input);

    //    native层回调  调用 sampleRateInHz  采样评率  44100  通道数   2
    public void createTrack(int sampleRateInHz,int nb_channals) {
//AudioTrack
//OpenSl Es
        Toast.makeText(this, "初始化播放器", Toast.LENGTH_SHORT).show();
        int channaleConfig;//通道数

        if (nb_channals == 1) {
            channaleConfig = AudioFormat.CHANNEL_OUT_MONO;
        } else if (nb_channals == 2) {
            channaleConfig = AudioFormat.CHANNEL_OUT_STEREO;
        }else {
            channaleConfig = AudioFormat.CHANNEL_OUT_MONO;
        }
        int buffersize=AudioTrack.getMinBufferSize(sampleRateInHz,
                channaleConfig, AudioFormat.ENCODING_PCM_16BIT);
        audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,sampleRateInHz,channaleConfig, AudioFormat.ENCODING_PCM_16BIT
                ,buffersize,
                AudioTrack.MODE_STREAM);
        audioTrack.play();
    }
//  native层回调  pcm数据内容  pcm实际长度 喇叭就能够播放  ([BI)V   主线程解码   ---主线程播放  所以有问题   anr      线程

    public void playTrack(byte[] buffer, int lenth) {

        if (audioTrack != null && audioTrack.getPlayState() == AudioTrack.PLAYSTATE_PLAYING) {
            audioTrack.write(buffer, 0, lenth);
        }
    }

}
