package com.yy.opencv;

import java.util.ArrayList;

public class OpenCVProcessor {
    static {
        System.loadLibrary("opencv");
    }

    public static native ArrayList<HoughCircles> detectHoughCircles(Object bitmap, int offsetX, int offsetY,
                                                             int minRadius, int maxRadius);
}
