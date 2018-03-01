LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= BufferQueue.cpp RtpH264Depay.cpp rtpreceiver.cpp  
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:=RTPReceiver
LOCAL_SHARED_LIBRARIES += libjrtp libjthread
include $(BUILD_EXECUTABLE)

