LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= BufferQueue.cpp RTPH264Depay.cpp RTPReceiver.cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:=RTPReceiver
LOCAL_SHARED_LIBRARIES += libjrtp libjthread
LOCAL_CFLAGS += -g
LOCAL_C_INCLUDES := vendor/nio/external
include $(BUILD_EXECUTABLE)

