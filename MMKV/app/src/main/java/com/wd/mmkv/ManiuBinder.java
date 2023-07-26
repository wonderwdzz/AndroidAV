package com.wd.mmkv;

public class ManiuBinder {
    static {
        System.loadLibrary("native-lib");
    }
    public native void write();

    public native void readTest();
}
