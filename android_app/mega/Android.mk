LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

src_dirs := src
res_dirs := res

LOCAL_STATIC_JAVA_LIBRARIES := \
        android-support-v4 \
        android-support-v7-recyclerview \
        android-support-v7-appcompat \
        remotecamera

LOCAL_JNI_SHARED_LIBRARIES := \
        libremote_camera_jni

LOCAL_SRC_FILES := $(call all-java-files-under, $(src_dirs))

LOCAL_RESOURCE_DIR := $(addprefix $(LOCAL_PATH)/, $(res_dirs))
LOCAL_RESOURCE_DIR += frameworks/support/v7/appcompat/res
LOCAL_RESOURCE_DIR += frameworks/support/v7/recyclerview/res

LOCAL_AAPT_FLAGS := --auto-add-overlay
LOCAL_AAPT_FLAGS += --extra-packages android.support.v7.appcompat
LOCAL_AAPT_FLAGS += --extra-packages android.support.v7.recyclerview

LOCAL_PACKAGE_NAME := MegaCamera
LOCAL_PRIVILEGED_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)

