LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hq_control
LOCAL_SRC_FILES := \
	/Users/droga5_639/AndroidStuff/of_v0.8.4_androidStudio/apps/myApps/hq_control/jni/Android.mk \
	/Users/droga5_639/AndroidStuff/of_v0.8.4_androidStudio/apps/myApps/hq_control/jni/Application.mk \

LOCAL_C_INCLUDES += /Users/droga5_639/AndroidStuff/of_v0.8.4_androidStudio/apps/myApps/hq_control/jni
LOCAL_C_INCLUDES += /Users/droga5_639/AndroidStuff/of_v0.8.4_androidStudio/apps/myApps/hq_control/src/debug/jni

include $(BUILD_SHARED_LIBRARY)
