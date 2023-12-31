#extension GL_OES_EGL_image_external : require
//必须 写的 固定的  意思   用采样器
//所有float类型数据的精度是lowp
precision mediump float;
varying vec2 aCoord;
//采样器  uniform static
uniform samplerExternalOES vTexture;
void main(){
//Opengl 自带函数
    vec4 rgba = texture2D(vTexture,aCoord);
//黄色滤镜
//    gl_FragColor=vec4(rgba.r+0.3,rgba.g+0.3,rgba.b,rgba.a);
    gl_FragColor=vec4(rgba.r,rgba.g,rgba.b,rgba.a);
}