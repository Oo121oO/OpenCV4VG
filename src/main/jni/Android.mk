LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
# ###########################################################
#这里改成你的路径，分割线内其余内容不变
OPENCV_ANDROID_SDK := $(LOCAL_PATH)

OPENCV_BULID_TYPE := NDK #默认NDK环境,不会自动导入openCV_java3.so,故不支持OpenCV Java库
#OPENCV_BULID_TYPE := JAVA_AND_NDK #将自动导入openCV_java3.so,来支持Java库(无需安装OpenCV Manager)

ifeq ($(OPENCV_BULID_TYPE), JAVA_AND_NDK)
    OPENCV_LIB_TYPE := SHARED
    OPENCV_INSTALL_MODULES := on
else
    OPENCV_LIB_TYPE := STATIC
endif

ifdef OPENCV_ANDROID_SDK
  ifneq ("","$(wildcard $(OPENCV_ANDROID_SDK)/OpenCV.mk)")
    include ${OPENCV_ANDROID_SDK}/OpenCV.mk
  else
    include ${OPENCV_ANDROID_SDK}/sdk/jni/OpenCV.mk
  endif
else
  include ./sdk/jni/OpenCV.mk
endif
# ###########################################################

#动态链接日志库
LOCAL_LDLIBS += -llog -ljnigraphics

LOCAL_MODULE    := opencv
LOCAL_SRC_FILES := $(LOCAL_PATH)/impl/OpenCVProcessor.cpp

include $(BUILD_SHARED_LIBRARY)