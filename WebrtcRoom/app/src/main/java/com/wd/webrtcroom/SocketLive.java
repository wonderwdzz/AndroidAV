package com.wd.webrtcroom;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

import org.java_websocket.WebSocket;
import org.java_websocket.client.WebSocketClient;
import org.java_websocket.handshake.ClientHandshake;
import org.java_websocket.handshake.ServerHandshake;
import org.java_websocket.server.WebSocketServer;

import java.net.InetSocketAddress;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

//音视频通话客户端
public class SocketLive {
    private  ManiuScoketServer maniuScoketServer;
    private int port=7033;
    //    为了模拟 后台通过推送   将这个房间的用户地址 发送过来，此时就直接写死了
//    视频   内容
    private String[] urls = {"ws://192.168.3.248:","ws://192.168.3.2:","ws://192.168.18.53:"};
    private static final String TAG = "David";
    List<MyWebSocketClient> socketClientList = new ArrayList<>();
    private IPeerConnection peerConnection;
    public SocketLive(IPeerConnection peerConnection) {
        //启动服务器端
        this.peerConnection = peerConnection;
        maniuScoketServer = new ManiuScoketServer(peerConnection);
        maniuScoketServer.start();
    }
    Timer timer;
    public void start(final Context context) {
        for (String value : urls) {
            if (value.contains(getLocalIpAddress(context))) {//排除本机IP
                continue;
            }
            URI url = null;
            try {
                //遍历IP进行链接
                url = new URI(value+port);
                MyWebSocketClient myWebSocketClient = new MyWebSocketClient(value,url);
                myWebSocketClient.connect();

            } catch (URISyntaxException e) {
                e.printStackTrace();
            }
        }
//模拟进入房间时，其他人主动需要 与新加入的人产生连接
        timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                for (String value : urls) {
                    if (value.contains(getLocalIpAddress(context))) {
                        continue;
                    }
                    boolean isSame = false;
                    for (MyWebSocketClient myWebSocketClient : socketClientList) {
                        if (value.contains(myWebSocketClient.getUrl())) {
                            isSame = true;
                            break;
                        }
                    }
                    if (isSame) {
                        continue;
                    }
                    //编译IP进行连接
                    URI url = null;
                    try {
                        url = new URI(value+port);
                        MyWebSocketClient myWebSocketClient = new MyWebSocketClient(value,url);
                        myWebSocketClient.connect();
                    } catch (URISyntaxException e) {
                        e.printStackTrace();
                    }
                }

                Log.i(TAG, "run: ------------延迟--------");
            }
        }, 3*1000, 5*1000);
    }

    public static String getLocalIpAddress(Context context) {
        try {

            WifiManager wifiManager = (WifiManager) context
                    .getSystemService(Context.WIFI_SERVICE);
            WifiInfo wifiInfo = wifiManager.getConnectionInfo();
            int i = wifiInfo.getIpAddress();
            String ip= int2ip(i);
            Log.i(TAG, "本机ip: " + ip);
            return ip;
        } catch (Exception ex) {
            return null;
        }
        // return null;
    }
    public static String int2ip(int ipInt) {
        StringBuilder sb = new StringBuilder();
        sb.append(ipInt & 0xFF).append(".");
        sb.append((ipInt >> 8) & 0xFF).append(".");
        sb.append((ipInt >> 16) & 0xFF).append(".");
        sb.append((ipInt >> 24) & 0xFF);
        return sb.toString();
    }
    public void sendData(byte[] bytes) {
        for (MyWebSocketClient myWebSocketClient : socketClientList) {
            if (myWebSocketClient.isOpen()) {
                myWebSocketClient.send(bytes);
            }
        }

    }
    private class MyWebSocketClient extends WebSocketClient {
        String url;
        public MyWebSocketClient(String url,URI serverURI) {
            super(serverURI);
            this.url = url;
        }

        public String getUrl() {
            return url;
        }
        //连接其他人成功进入这里
        @Override
        public void onOpen(ServerHandshake serverHandshake) {
            socketClientList.add(this);
        }
        @Override
        public void onMessage(String s) {
        }
        @Override
        public void onMessage(ByteBuffer bytes) {

        }
//        音视频的
        @Override
        public void onClose(int i, String s, boolean b) {
            Log.i(TAG, "onClose: ");
        }

        @Override
        public void onError(Exception e) {
            Log.i(TAG, "onError: ");
            if (socketClientList.contains(this)) {
                socketClientList.remove(this);
            }
        }
    }

    //Server端 当client连接会进入这里
    class ManiuScoketServer extends WebSocketServer  {
        private static final String TAG = "david";
        private volatile   int i = 0;
        private IPeerConnection peerConnection;
        public ManiuScoketServer(IPeerConnection peerConnection) {
            super(new InetSocketAddress(port));
            this.peerConnection = peerConnection;
        }
        @Override//被其他链接成功进入这里
        public void onOpen(WebSocket webSocket, ClientHandshake clientHandshake) {
            this.peerConnection.newConnection
                    (webSocket.getRemoteSocketAddress().getAddress().getHostName());
        }
        @Override
        public void onClose(WebSocket webSocket, int i, String s, boolean b) {
            Log.i(TAG, "onClose: 关闭 socket ");
        }
        @Override
        public void onMessage(WebSocket webSocket, String s) {

        }
        @Override//客户端有消息发送进入这里
        public void onMessage(WebSocket conn, ByteBuffer bytes) {
            byte[] buf = new byte[bytes.remaining()];
            bytes.get(buf);
            peerConnection.remoteReceiveData(conn.getRemoteSocketAddress().getAddress().
                    getHostName(),buf);
        }

        @Override
        public void onError(WebSocket webSocket, Exception e) {
        }

        @Override
        public void onStart() {

        }
    };
}
