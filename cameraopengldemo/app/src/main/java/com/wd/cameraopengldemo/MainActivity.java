package com.wd.cameraopengldemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.opengl.GLSurfaceView;
public class MainActivity extends AppCompatActivity {

    private String TAG = MainActivity.class.getSimpleName();
    private GLSurfaceView glSurfaceView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
    }
    private void initView() {
        glSurfaceView = (GLSurfaceView) findViewById(R.id.glSurfaceView);
        //GLContext设置OpenGLES2.0
        glSurfaceView.setEGLContextClientVersion(2);
        glSurfaceView.setRenderer(new TriangleRender());
        /*渲染方式，RENDERMODE_WHEN_DIRTY表示被动渲染，只有在调用requestRender或者onResume等方法时才会进行渲染。RENDERMODE_CONTINUOUSLY表示持续渲染*/
        glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    @Override
    protected void onResume() {
        super.onResume();
        glSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        glSurfaceView.onPause();
    }
}
