package com.wd.webrtcroom;

public interface IPeerConnection {
//房间里面有人加入
    public void newConnection(String remoteIp);
//渲染本地
    public void remoteReceiveData(String remoteIp, byte[] data);


}
