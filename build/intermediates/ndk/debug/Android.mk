LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := location_test
LOCAL_SRC_FILES := \
	/Users/droga5_639/AndroidStuff/of_v0.8.4_androidStudio/apps/myApps/location_test/jni/Android.mk \
	/Users/droga5_639/AndroidStuff/of_v0.8.4_androidStudio/apps/myApps/location_test/jni/Application.mk \

LOCAL_C_INCLUDES += /Users/droga5_639/AndroidStuff/of_v0.8.4_androidStudio/apps/myApps/location_test/jni
LOCAL_C_INCLUDES += /Users/droga5_639/AndroidStuff/of_v0.8.4_androidStudio/apps/myApps/location_test/src/debug/jni

include $(BUILD_SHARED_LIBRARY)
