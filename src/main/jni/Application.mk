APP_ABI := armeabi-v7a
APP_PLATFORM := android-19
APP_STL := gnustl_static
APP_CPPFLAGS := -frtti -fexceptions
APP_OPTIM := release
VA_ROOT          := $(call my-dir)
NDK_MODULE_PATH  := $(NDK_MODULE_PATH):$(VA_ROOT)