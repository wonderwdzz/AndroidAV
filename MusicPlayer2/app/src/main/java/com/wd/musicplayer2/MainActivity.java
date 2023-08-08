package com.wd.musicplayer2;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

import com.wd.musicplayer2.listener.WlOnParparedListener;
import com.wd.musicplayer2.log.MyLog;
import com.wd.musicplayer2.player.MNPlayer;

public class MainActivity extends AppCompatActivity {
    private MNPlayer wlPlayer;
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        wlPlayer = new MNPlayer();
        wlPlayer.setWlOnParparedListener(new WlOnParparedListener() {
            @Override
            public void onParpared() {
                MyLog.d("准备好了，可以开始播放声音了");
                wlPlayer.start();
            }
        });
    }

    public void begin(View view) {
        wlPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        wlPlayer.parpared();
    }
}
