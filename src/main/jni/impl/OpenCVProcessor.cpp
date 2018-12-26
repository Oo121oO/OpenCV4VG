//
// Created by OuyangShaocong on 2018/12/13.
//

#include "OpenCVProcessor.h"
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <iostream>
#include <opencv2/core/types_c.h>

using namespace cv;
using namespace std;

JNIEXPORT jobject JNICALL Java_com_lulu_lulubox_opencv_OpenCVProcessor_detectHoughCircles
        (JNIEnv *env, jobject, jobject bitmap,
        jfloat factor, jint x, jint y, jint w, jint h) {
    //获得ArrayList类引用
    jclass list_cls = env->FindClass("java/util/ArrayList");
    if (list_cls == NULL) {
        fprintf(stderr, "class java/util/ArrayList not found.");
        return NULL;
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
    Mat roi(gray, Rect(x, y, w, h));
    HoughCircles(roi, circles, HOUGH_GRADIENT, 1, 60, 300, 80, (jint)factor * 60, (jint)factor * 120);
    AndroidBitmap_unlockPixels(env, bitmap);

    int size = circles.size();
    if (size <= 0) {
        fprintf(stderr, "hough circles not found.");
        return list_obj;
    }
    jclass hc_cls = env->FindClass("com/lulu/lulubox/opencv/HoughCircles");
    if (hc_cls == NULL) {
        fprintf(stderr, "class com/lulu/lulubox/opencv/HoughCircles not found.");
        return list_obj;
    }
    jmethodID hc_init = env->GetMethodID(hc_cls, "<init>", "(FFF)V");

    //返回数据
    for (size_t i = 0; i < size; i++) {
        Vec3f vec = circles[i];
        jobject hc_obj = env->NewObject(hc_cls, hc_init, vec[0] + x, vec[1] + y, vec[2]);
        env->CallBooleanMethod(list_obj, list_add, hc_obj);
    }

    return list_obj;
}

JNIEXPORT jobject JNICALL Java_com_lulu_lulubox_opencv_OpenCVProcessor_matchTemplate
        (JNIEnv *env, jobject, jobject bitmap, jobject temp,
        jfloat factor, jint x, jint y, jint w, jint h, jint scale) {
    //获取image
    AndroidBitmapInfo info;
    void *pixels;
    int width, height;
    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
        fprintf(stderr, "getInfo error.");
        return NULL;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        fprintf(stderr, "wrong format.");
        return NULL;
    }
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
        fprintf(stderr, "get pixels error.");
        return NULL;
    }
    width = info.width;
    height = info.height;
    Mat _image(height, width, CV_8UC4, pixels);
    Mat _roi(_image, Rect(x, y, w, h));
    resize(_roi, _roi, Size(), 1.0 / scale, 1.0 / scale);
    //获取template
    if (AndroidBitmap_getInfo(env, temp, &info) < 0) {
        fprintf(stderr, "getInfo error.");
        return NULL;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        fprintf(stderr, "wrong format.");
        return NULL;
    }
    if (AndroidBitmap_lockPixels(env, temp, &pixels) < 0) {
        fprintf(stderr, "get pixels error.");
        return NULL;
    }
    width = info.width;
    height = info.height;
    Mat _temp(height, width, CV_8UC4, pixels);
    resize(_temp, _temp, Size(), factor / scale, factor / scale);
    //开始matchTemplate
    Mat result;
    matchTemplate(_roi, _temp, result, CV_TM_SQDIFF_NORMED);
    double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    AndroidBitmap_unlockPixels(env, temp);
    AndroidBitmap_unlockPixels(env, bitmap);
    //返回
    //获得Rect类引用
    jclass r_cls = env->FindClass("android/graphics/Rect");
    if (r_cls == NULL) {
        fprintf(stderr, "class android/graphics/Rect not found.");
        return NULL;
    }
    jmethodID r_init = env->GetMethodID(r_cls, "<init>", "(IIII)V");
    int left = scale * minLoc.x + x;
    int top = scale * minLoc.y + y;
    int right = left + scale * _temp.cols;
    int bottom = top + scale * _temp.rows;
    jobject r_obj = env->NewObject(r_cls, r_init, left, top, right, bottom);
    //获得RectWithConfidence类引用
    jclass rc_cls = env->FindClass("com/lulu/lulubox/opencv/RectWithConfidence");
    if (rc_cls == NULL) {
        fprintf(stderr, "class com/lulu/lulubox/opencv/RectWithConfidence not found.");
        return NULL;
    }
    jmethodID rc_init = env->GetMethodID(rc_cls, "<init>", "(Landroid/graphics/Rect;F)V");
    jobject rc_obj = env->NewObject(rc_cls, rc_init, r_obj, minVal);
    return rc_obj;
}