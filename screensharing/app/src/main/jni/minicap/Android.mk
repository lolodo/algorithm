LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := minicap-common

LOCAL_SRC_FILES := \
	JpgEncoder.cpp \
	SimpleServer.cpp \
	jpegjni.cpp \

LOCAL_STATIC_LIBRARIES := \
	libjpeg-turbo \

LOCAL_LDLIBS := -llog -landroid
include $(BUILD_SHARED_LIBRARY)
