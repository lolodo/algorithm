#include <fcntl.h>
#include <getopt.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>

#include <cmath>
#include <condition_variable>
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>


#include "util/debug.h"
#include "JpgEncoder.hpp"
#include "SimpleServer.hpp"

#include <jni.h>
#include <android/log.h>
#include <pthread.h>

#define BANNER_VERSION 1
#define BANNER_SIZE 24

#define DEFAULT_SOCKET_NAME "minicap"
#define DEFAULT_DISPLAY_ID 0
#define DEFAULT_JPG_QUALITY 80

enum {
  QUIRK_DUMB            = 1,
  QUIRK_ALWAYS_UPRIGHT  = 2,
  QUIRK_TEAR            = 4,
};

static int
pumps(int fd, unsigned char* data, size_t length) {
  do {
    // Make sure that we don't generate a SIGPIPE even if the socket doesn't
    // exist anymore. We'll still get an EPIPE which is perfect.
    int wrote = send(fd, data, length, MSG_NOSIGNAL);

    if (wrote < 0) {
      return wrote;
    }

    data += wrote;
    length -= wrote;
  }
  while (length > 0);

  return 0;
}

static int
pumpf(int fd, unsigned char* data, size_t length) {
  do {
    int wrote = write(fd, data, length);

    if (wrote < 0) {
      return wrote;
    }

    data += wrote;
    length -= wrote;
  }
  while (length > 0);

  return 0;
}

static int
putUInt32LE(unsigned char* data, int value) {
  data[0] = (value & 0x000000FF) >> 0;
  data[1] = (value & 0x0000FF00) >> 8;
  data[2] = (value & 0x00FF0000) >> 16;
  data[3] = (value & 0xFF000000) >> 24;
}

static void
signal_handler(int signum) {
  switch (signum) {
  case SIGINT:
    MCINFO("Received SIGINT, stopping");
    break;
  case SIGTERM:
    MCINFO("Received SIGTERM, stopping");
    break;
  default:
    abort();
    break;
  }
}


static pthread_t gst_app_thread;
static pthread_key_t current_jni_env;
static JavaVM *java_vm;
static jfieldID custom_data_field_id;
static jmethodID set_message_method_id;
static jmethodID set_current_position_method_id;
static jmethodID on_gstreamer_initialized_method_id;
static jmethodID on_media_size_changed_method_id;

int accept_fd;
JpgEncoder encoder(4, 0);
static int print_tid = 0;
JpgEncoder::Frame frame;
unsigned int quality = DEFAULT_JPG_QUALITY;
int width = 1280;
int height = 720;

static void detach_current_thread(void *env) {
    //MCERROR("Detaching thread %p", g_thread_self());
    java_vm->DetachCurrentThread();
}

void native_jpeg_push_buff(JNIEnv *env, jobject thiz, jobject obj_buff) {

    //JNIEnv *env_new = get_jni_env();
    JNIEnv *env_new;

    if (print_tid <= 1) {
        pid_t tid = gettid();
        __android_log_print(ANDROID_LOG_ERROR, "minicap",
                            "%s:%d tid:%d env:%p env_new:%p", __func__, __LINE__, tid, env, env_new);
        __android_log_print(ANDROID_LOG_ERROR, "minicap",
                            "%s:%d enter", __func__, __LINE__);
        __android_log_print(ANDROID_LOG_ERROR, "minicap",
                            "send to fd:%d",
                            accept_fd);
    }
    if (accept_fd <= 0)
        return;

    char *java_buffer = (char*)env->GetDirectBufferAddress(obj_buff);
    uint_t size = env->GetDirectBufferCapacity(obj_buff);

    frame.data = java_buffer;
    frame.format = JpgEncoder::FORMAT_RGBA_8888;
    frame.width = width;
    frame.stride = width;
    frame.bpp = 4;
    frame.height = height;

    if (print_tid <= 1) {
        __android_log_print(ANDROID_LOG_ERROR, "minicap",
                            "push buffer size:%u thiz:%x obj_buff:%x java_buffer:%x",
                            size, thiz, obj_buff, java_buffer);
    }

    if (print_tid <= 1) {
        print_tid++;
    }

    // Encode the frame.
    if (!encoder.encode(&frame, quality)) {
        __android_log_print(ANDROID_LOG_ERROR, "minicap",
        "Unable to encode frame");
        return;
    }

    // Push it out synchronously because it's fast and we don't care
    // about other clients.
    unsigned char* data = encoder.getEncodedData() - 4;
    size = encoder.getEncodedSize();

    putUInt32LE(data, size);

    int ret;
    if ((ret = pumps(accept_fd, data, size + 4)) < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "minicap",
                    "Unable to send frame");
        return;
    }

}

static void *app_function(void *userdata) {
  const char* sockname = DEFAULT_SOCKET_NAME;

  // Set up signal handler.
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);

  // Leave a 4-byte padding to the encoder so that we can inject the size
  // to the same buffer.

  // Server config.
  SimpleServer server;

  if (!encoder.reserveData(width, height)) {
    MCERROR("Unable to reserve data for JPG encoder");
    return NULL;
  }


  if (!server.start(sockname)) {
    MCERROR("Unable to start server on namespace '%s'", sockname);
    return NULL;
  }

  while ((accept_fd = server.accept()) > 0) {
        __android_log_print(ANDROID_LOG_ERROR, "minicap",
                            "new client connection fd:%d",
                            accept_fd);
  }

}

static void native_jpeg_init(JNIEnv *env, jobject thiz) {
    pid_t tid = gettid();
    __android_log_print(ANDROID_LOG_ERROR, "minicap",  "%s:%d thiz:%x tid:%d", __func__, __LINE__, thiz, tid);
    pthread_setname_np(gst_app_thread, "gst_app_thread");
    pthread_create(&gst_app_thread, NULL, &app_function, NULL);
}

static void native_jpeg_finalize(JNIEnv *env, jobject thiz) {

    pthread_join(gst_app_thread, NULL);
    //SET_CUSTOM_DATA (env, thiz, custom_data_field_id, NULL);

}

static jboolean native_jpeg_class_init(JNIEnv *env, jclass klass) {
    custom_data_field_id = env->GetFieldID(klass, "native_custom_data", "J");
    set_message_method_id = env->GetMethodID(klass, "setMessage", "(Ljava/lang/String;)V");
    set_current_position_method_id = env->GetMethodID(klass, "setCurrentPosition", "(II)V");
    on_gstreamer_initialized_method_id = env->GetMethodID(klass, "onGStreamerInitialized",
                                                             "()V");
    on_media_size_changed_method_id = env->GetMethodID(klass, "onMediaSizeChanged",
                                                          "(II)V");

    if (!custom_data_field_id || !set_message_method_id || !on_gstreamer_initialized_method_id ||
        !on_media_size_changed_method_id || !set_current_position_method_id) {
        /* We emit this message through the Android log instead of the GStreamer log because the later
         * has not been initialized yet.
         */
        __android_log_print(ANDROID_LOG_ERROR, "minicap",
                            "The calling class does not implement all necessary interface methods");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


/* List of implemented native methods */
static JNINativeMethod native_methods[] = {
        {"nativejpegInit",            "()V",                   (void *) native_jpeg_init},
        {"nativejpegFinalize",        "()V",                   (void *) native_jpeg_finalize},
        {"nativejpegPushBuff",        "(Ljava/nio/ByteBuffer;)V", (void *) native_jpeg_push_buff},
        {"nativejpegClassInit",       "()Z",                   (void *) native_jpeg_class_init}
};

/* Library initializer */
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;

    java_vm = vm;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "minicap", "Could not retrieve JNIEnv");
        return 0;
    }

    jclass klass = env->FindClass("com/nextev/screensharing/activity/VirtualDisplayDemoImageReader");
    env->RegisterNatives(klass, native_methods, sizeof(native_methods)/sizeof((native_methods)[0]));

/*
    klass = env->FindClass("com/nextev/screensharing/activity/VirtualDisplayDemo");
    env->RegisterNatives(klass, native_methods, sizeof(native_methods)/sizeof((native_methods)[0]));

    klass = env->FindClass("com/nextev/screensharing/activity/VirtualDisplayDemoNew");
    env->RegisterNatives(klass, native_methods, sizeof(native_methods)/sizeof((native_methods)[0]));
    */

    pthread_key_create(&current_jni_env, detach_current_thread);

    return JNI_VERSION_1_4;
}
