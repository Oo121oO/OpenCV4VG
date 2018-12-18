//
// Created by OuyangShaocong on 2018/12/13.
//

#include "com_yy_opencv_OpenCVProcessor.h"
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <iostream>
#include <opencv2/core/types_c.h>

using namespace cv;
using namespace std;

JNIEXPORT jobject JNICALL Java_com_yy_opencv_OpenCVProcessor_detectHoughCircles
        (JNIEnv *env, jobject, jobject bitmap, jint offsetX, jint offsetY,
         jint minRadius, jint maxRadius) {
    //获得ArrayList类引用
    jclass list_cls = env->FindClass("java/util/ArrayList");
    if (list_cls == NULL) {
        fprintf(stderr, "class arraylist not found.");
    }
    //获得得构造函数Id
    jmethodID list_init = env->GetMethodID(list_cls, "<init>", "()V");
    jobject list_obj = env->NewObject(list_cls, list_init);
    jmethodID list_add = env->GetMethodID(list_cls, "add", "(Ljava/lang/Object;)Z");

    AndroidBitmapInfo info;
    void *pixels;
    int width, height;
    //解析bitmap
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        fprintf(stderr, "getInfo error.");
        return list_obj;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        fprintf(stderr, "wrong format.");
        return list_obj;
    }
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        fprintf(stderr, "get pixels error.");
        return list_obj;
    }
    width = info.width;
    height = info.height;
    Mat image(height, width, CV_8UC4, pixels);
    Mat gray;
    cvtColor(image, gray, COLOR_RGBA2GRAY);
    vector<Vec3f> circles;
    Mat roi(gray, Rect(offsetX, offsetY, width - offsetX, height - offsetY));
    HoughCircles(roi, circles, HOUGH_GRADIENT, 1, 60, 300, 80, minRadius, maxRadius);
    AndroidBitmap_unlockPixels(env, bitmap);

    int size = circles.size();
    if (size <= 0) {
        fprintf(stderr, "hough circles not found.");
        return list_obj;
    }
    jclass hc_cls = env->FindClass("com/yy/opencv/HoughCircles");
    if (hc_cls == NULL) {
        fprintf(stderr, "class HoughCircles not found.");
        return list_obj;
    }
    jmethodID hc_init = env->GetMethodID(hc_cls, "<init>", "(FFF)V");

    //返回数据
    for (size_t i = 0; i < size; i++) {
        Vec3f vec = circles[i];
        jobject hc_obj = env->NewObject(hc_cls, hc_init, vec[0] + offsetX, vec[1] + offsetY, vec[2]);
        env->CallBooleanMethod(list_obj, list_add, hc_obj);
    }

    return list_obj;
}