package com.yy.opencv;

import android.graphics.Point;

import java.util.ArrayList;

public class OpenCVProcessor {
    static {
        System.loadLibrary("opencv");
    }

    public static native ArrayList<HoughCircles> detectHoughCircles(Object bitmap, int minRadius, int maxRadius,
                                                                    float factor, int x, int y, int w, int h);

    public static native Point matchTemplate(Object bitmap, Object template,
                                             float factor, int x, int y, int w, int h);
}
