LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)


LOCAL_SRC_FILES:= BufferQueue.cpp PracticalSocket.cpp RtpH264Depay.cpp rtpreceiver.cpp  
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:=main
LOCAL_LDFLAGS += -L$(LOCAL_PATH)/lib
LOCAL_LDLIBS += -ljrtp -ljthread -lstdc++ -lgcc -lc

include $(BUILD_EXECUTABLE)

