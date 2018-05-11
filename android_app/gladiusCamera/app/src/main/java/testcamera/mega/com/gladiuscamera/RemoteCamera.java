/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package testcamera.mega.com.gladiuscamera;

import java.util.LinkedHashMap;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.text.TextUtils;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;

public class RemoteCamera {

    static {
        System.loadLibrary("remote_camera_jni");
        native_init();
    }

    private static final String TAG = "RemoteCamera";

    public static final String REMOTE_CAMERA_DVR           = "remote-camera-dvr";
    public static final String REMOTE_CAMERA_ROOF          = "remote-camera-roof";
    public static final String REMOTE_CAMERA_AVM_SYNTHESIS = "remote-camera-avm-synthesis";
    public static final String REMOTE_CAMERA_AVM_FRONT     = "remote-camera-avm-front";
    public static final String REMOTE_CAMERA_AVM_BACK      = "remote-camera-avm-back";
    public static final String REMOTE_CAMERA_AVM_LEFT      = "remote-camera-avm-left";
    public static final String REMOTE_CAMERA_AVM_RIGHT     = "remote-camera-avm-right";
    public static final String REMOTE_CAMERA_DRIVER        = "remote-camera-driver";

    public static final String EVT_VERSION_1_0 = "evt-version-1.0";
    public static final String EVT_VERSION_2_0 = "evt-version-2.0";

    public static final String KEY_PREVIEW_FORMAT_H264 = "preview-format-h264";
    public static final String KEY_PREVIEW_FORMAT_JPEG = "preview-format-jpeg";

    public static final String PIXEL_FORMAT_YUV422SP = "yuv422sp";
    public static final String PIXEL_FORMAT_YUV420SP = "yuv420sp";
    public static final String PIXEL_FORMAT_YUV422I = "yuv422i-yuyv";
    public static final String PIXEL_FORMAT_YUV420P = "yuv420p";
    public static final String PIXEL_FORMAT_RGB565 = "rgb565";
    public static final String PIXEL_FORMAT_JPEG = "jpeg";
    public static final String PIXEL_FORMAT_BAYER_RGGB = "bayer-rggb";

    public static final int CAMERA_STREAM_EOF = -99;

    private static final int CAMERA_MSG_ERROR             = 0x001;
    private static final int CAMERA_MSG_PREVIEW_FRAME     = 0x002;
    private static final int CAMERA_MSG_RAW_IMAGE         = 0x003;
    private static final int CAMERA_MSG_COMPRESSED_IMAGE  = 0x004;
    private static final int CAMERA_MSG_PREVIEW_FRAME_EOF = 0x010;

    private long mNativeContext = 0;
    private EventHandler mEventHandler;
    private ErrorCallback mErrorCallback;
    private PreviewCallback mPreviewCallback;
    private PictureCallback mRawImageCallback;
    private PictureCallback mJpegCallback;

    private static volatile RemoteCamera singleton = null;

    public static RemoteCamera getInstance() {
        if (singleton == null) {
            synchronized (RemoteCamera.class) {
                if (singleton == null) {
                    singleton = new RemoteCamera();
                }
            }
        }
        return singleton;
    }

    private RemoteCamera() {
        Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else {
            mEventHandler = null;
        }

        int err = native_setup(new WeakReference<RemoteCamera>(this));
        if (err != 0) {
            throw new RuntimeException("Remote camera setup error.");
        }
    }

    public int connectRemote(String serverIP, int serverPort, int clientPort) {
        if (serverIP != null && serverPort > 0) {
            return native_init_client(serverIP, serverPort, clientPort);
        }
        return -1;
    }

    public void disconnectRemote() {
        native_release_client();
    }

    public int startStream() {
        return native_start_stream();
    }

    public void stopStream() {
        native_stop_stream();
    }

    public void setParameters(Parameters params) {
        native_setParameters(params.flatten());
    }

    public Parameters getEmptyParameters() {
        Parameters p = new Parameters();
        return p;
    }

    public Parameters getParameters() {
        Parameters p = new Parameters();
        String s = native_getParameters();
        p.unflatten(s);
        return p;
    }

    public void release() {
        native_release();
    }

    private static native final void native_init();
    private native final int native_setup(Object camera_this);
    private native final void native_release();
    private native final int native_init_client(String serverIP, int sererPort, int clientPort);
    private native final void native_release_client();
    private native final void native_setParameters(String params);
    private native final String native_getParameters();
    private native final int native_start_stream();
    private native final void native_stop_stream();
    private native final void native_takePicture(int msgType);

    /**
     * Callback interface for camera error notification.
     *
     * @see #setErrorCallback(ErrorCallback)
     *
     * @deprecated We recommend using the new {@link android.hardware.camera2} API for new
     *             applications.
     */
    @Deprecated
    public interface ErrorCallback
    {
        /**
         * Callback for camera errors.
         * @param error   error code:
         * <ul>
         * <li>{@link #CAMERA_ERROR_UNKNOWN}
         * <li>{@link #CAMERA_ERROR_SERVER_DIED}
         * </ul>
         * @param camera  the Camera service object
         */
        void onError(int error);
    };

    /**
     * Registers a callback to be invoked when an error occurs.
     * @param cb The callback to run
     */
    public final void setErrorCallback(ErrorCallback cb)
    {
        mErrorCallback = cb;
    }

    public interface PreviewCallback
    {
        void onPreviewFrame(byte[] data, int timestamp, int isEOF);
    };

    public final void setPreviewCallback(PreviewCallback cb) {
        mPreviewCallback = cb;
    }

    public interface PictureCallback {
        void onPictureTaken(byte[] data);
    };

    public final void takePicture(PictureCallback raw, PictureCallback jpeg) {
        mRawImageCallback = raw;
        mJpegCallback = jpeg;

        // If callback is not set, do not send me callbacks.
        int msgType = 0;
        if (mRawImageCallback != null) {
            msgType |= CAMERA_MSG_RAW_IMAGE;
        }
        if (mJpegCallback != null) {
            msgType |= CAMERA_MSG_COMPRESSED_IMAGE;
        }

        native_takePicture(msgType);
    }

    private class EventHandler extends Handler
    {
        private final RemoteCamera mCamera;

        public EventHandler(RemoteCamera c, Looper looper) {
            super(looper);
            mCamera = c;
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case CAMERA_MSG_ERROR:
                Log.e(TAG, "Error " + msg.arg1);
                if (mErrorCallback != null) {
                    mErrorCallback.onError(msg.arg1);
                }
                return;

            case CAMERA_MSG_PREVIEW_FRAME: {
                PreviewCallback pCb = mPreviewCallback;
                if (pCb != null) {
                    pCb.onPreviewFrame((byte[])msg.obj, msg.arg1, msg.arg2);
                }
                return;
            }

            case CAMERA_MSG_PREVIEW_FRAME_EOF: {
                PreviewCallback pCb = mPreviewCallback;
                if (pCb != null) {
                    pCb.onPreviewFrame(null, 0, CAMERA_STREAM_EOF);
                }
                return;
            }

            case CAMERA_MSG_RAW_IMAGE:
                if (mRawImageCallback != null) {
                    mRawImageCallback.onPictureTaken((byte[])msg.obj);
                }
                return;

            case CAMERA_MSG_COMPRESSED_IMAGE:
                if (mJpegCallback != null) {
                    mJpegCallback.onPictureTaken((byte[])msg.obj);
                }
                break;

            default:
                Log.e(TAG, "Unknown message type " + msg.what);
                return;
            }
        }
    }

    private static void postEventFromNative(Object camera_ref,
                                            int what, int arg1, int arg2, Object obj)
    {
        RemoteCamera c = (RemoteCamera)((WeakReference)camera_ref).get();
        if (c == null)
            return;

        if (c.mEventHandler != null) {
            Message m = c.mEventHandler.obtainMessage(what, arg1, arg2, obj);
            c.mEventHandler.sendMessage(m);
        }
    }

    public class Size {
        /**
         * Sets the dimensions for pictures.
         *
         * @param w the photo width (pixels)
         * @param h the photo height (pixels)
         */
        public Size(int w, int h) {
            width = w;
            height = h;
        }
        /**
         * Compares {@code obj} to this size.
         *
         * @param obj the object to compare this size with.
         * @return {@code true} if the width and height of {@code obj} is the
         *         same as those of this size. {@code false} otherwise.
         */
        @Override
        public boolean equals(Object obj) {
            if (!(obj instanceof Size)) {
                return false;
            }
            Size s = (Size) obj;
            return width == s.width && height == s.height;
        }
        @Override
        public int hashCode() {
            return width * 32713 + height;
        }
        /** width of the picture */
        public int width;
        /** height of the picture */
        public int height;
    };

    public class Parameters {
        private static final String KEY_EVT_VERSION = "evt-version";
        private static final String KEY_CAMERA_NAME = "camera-name";

        private static final String KEY_PREVIEW_SIZE = "preview-size";
        private static final String KEY_PREVIEW_FORMAT = "preview-format";
        private static final String KEY_PREVIEW_FRAMERATE = "preview-fps";

        private static final String KEY_PICTURE_SIZE = "picture-size";
        private static final String KEY_PICTURE_FORMAT = "picture-format";

        private static final String KEY_CLIENT_ADDRESS = "receive-ip-address";
        private static final String KEY_CLINET_PORT    = "receive-port";

        // Parameter key suffix for supported values.
        private static final String SUPPORTED_VALUES_SUFFIX = "-values";

        private final LinkedHashMap<String, String> mMap;

        private Parameters() {
            mMap = new LinkedHashMap<String, String>(/*initialCapacity*/64);
        }

        public String flatten() {
            StringBuilder flattened = new StringBuilder(128);
            for (String k : mMap.keySet()) {
                flattened.append(k);
                flattened.append("=");
                flattened.append(mMap.get(k));
                flattened.append(";");
            }
            // chop off the extra semicolon at the end
            flattened.deleteCharAt(flattened.length()-1);
            return flattened.toString();
        }

        public void unflatten(String flattened) {
            mMap.clear();

            TextUtils.StringSplitter splitter = new TextUtils.SimpleStringSplitter(';');
            splitter.setString(flattened);
            for (String kv : splitter) {
                int pos = kv.indexOf('=');
                if (pos == -1) {
                    continue;
                }
                String k = kv.substring(0, pos);
                String v = kv.substring(pos + 1);
                mMap.put(k, v);
            }
        }

        public void remove(String key) {
            mMap.remove(key);
        }

        public void set(String key, String value) {
            if (key.indexOf('=') != -1 || key.indexOf(';') != -1 || key.indexOf(0) != -1) {
                Log.e(TAG, "Key \"" + key + "\" contains invalid character (= or ; or \\0)");
                return;
            }
            if (value.indexOf('=') != -1 || value.indexOf(';') != -1 || value.indexOf(0) != -1) {
                Log.e(TAG, "Value \"" + value + "\" contains invalid character (= or ; or \\0)");
                return;
            }

            put(key, value);
        }

        public void set(String key, int value) {
            put(key, Integer.toString(value));
        }

        private void put(String key, String value) {
            /*
             * Remove the key if it already exists.
             *
             * This way setting a new value for an already existing key will always move
             * that key to be ordered the latest in the map.
             */
            mMap.remove(key);
            mMap.put(key, value);
        }

        public String get(String key) {
            return mMap.get(key);
        }

        public int getInt(String key) {
            return Integer.parseInt(mMap.get(key));
        }

        public void setEvtVersion(String version) {
            set(KEY_EVT_VERSION, version);
        }

        public String getEvtVersion() {
            return get(KEY_EVT_VERSION);
        }

        public void setRemoteCamera(String remoteCamera) {
            set(KEY_CAMERA_NAME, remoteCamera);
        }

        public String getRemoteCamera() {
            return get(KEY_CAMERA_NAME);
        }

        public void setClientInfo(String ip, int port) {
            set(KEY_CLIENT_ADDRESS, ip);
            set(KEY_CLINET_PORT, port);
        }

        public void setPreviewSize(int width, int height) {
            String v = Integer.toString(width) + "x" + Integer.toString(height);
            set(KEY_PREVIEW_SIZE, v);
        }

        public Size getPreviewSize() {
            String pair = get(KEY_PREVIEW_SIZE);
            return strToSize(pair);
        }

        public List<Size> getSupportedPreviewSizes() {
            String str = get(KEY_PREVIEW_SIZE + SUPPORTED_VALUES_SUFFIX);
            return splitSize(str);
        }

        public void setPreviewFormat(String pixel_format) {
            set(KEY_PREVIEW_FORMAT, pixel_format);
        }

        public String getPreviewFormat() {
            return get(KEY_PREVIEW_FORMAT);
        }

        public List<String> getSupportedPreviewFormats() {
            String str = get(KEY_PREVIEW_FORMAT + SUPPORTED_VALUES_SUFFIX);
            ArrayList<String> formats = new ArrayList<String>();
            for (String s : split(str)) {
                formats.add(s);
            }
            return formats;
        }

        public void setPreviewFrameRate(int fps) {
            set(KEY_PREVIEW_FRAMERATE, fps);
        }

        public int getPreviewFrameRate() {
            return getInt(KEY_PREVIEW_FRAMERATE);
        }

        public List<Integer> getSupportedPreviewFrameRates() {
            String str = get(KEY_PREVIEW_FRAMERATE + SUPPORTED_VALUES_SUFFIX);
            return splitInt(str);
        }

        public void setPictureSize(int width, int height) {
            String v = Integer.toString(width) + "x" + Integer.toString(height);
            set(KEY_PICTURE_SIZE, v);
        }

        public Size getPictureSize() {
            String pair = get(KEY_PICTURE_SIZE);
            return strToSize(pair);
        }

        public List<Size> getSupportedPictureSizes() {
            String str = get(KEY_PICTURE_SIZE + SUPPORTED_VALUES_SUFFIX);
            return splitSize(str);
        }

        public void setPictureFormat(String pixel_format) {
            set(KEY_PICTURE_FORMAT, pixel_format);
        }

        public String getPictureFormat() {
            return get(KEY_PICTURE_FORMAT);
        }

        public List<String> getSupportedPictureFormats() {
            String str = get(KEY_PICTURE_FORMAT + SUPPORTED_VALUES_SUFFIX);
            ArrayList<String> formats = new ArrayList<String>();
            for (String s : split(str)) {
                formats.add(s);
            }
            return formats;
        }

        private ArrayList<String> split(String str) {
            if (str == null) return null;

            TextUtils.StringSplitter splitter = new TextUtils.SimpleStringSplitter(',');
            splitter.setString(str);
            ArrayList<String> substrings = new ArrayList<String>();
            for (String s : splitter) {
                substrings.add(s);
            }
            return substrings;
        }

        // Splits a comma delimited string to an ArrayList of Integer.
        // Return null if the passing string is null or the size is 0.
        private ArrayList<Integer> splitInt(String str) {
            if (str == null) return null;

            TextUtils.StringSplitter splitter = new TextUtils.SimpleStringSplitter(',');
            splitter.setString(str);
            ArrayList<Integer> substrings = new ArrayList<Integer>();
            for (String s : splitter) {
                substrings.add(Integer.parseInt(s));
            }
            if (substrings.size() == 0) return null;
            return substrings;
        }

        private void splitInt(String str, int[] output) {
            if (str == null) return;

            TextUtils.StringSplitter splitter = new TextUtils.SimpleStringSplitter(',');
            splitter.setString(str);
            int index = 0;
            for (String s : splitter) {
                output[index++] = Integer.parseInt(s);
            }
        }

        // Splits a comma delimited string to an ArrayList of Size.
        // Return null if the passing string is null or the size is 0.
        private ArrayList<Size> splitSize(String str) {
            if (str == null) return null;

            TextUtils.StringSplitter splitter = new TextUtils.SimpleStringSplitter(',');
            splitter.setString(str);
            ArrayList<Size> sizeList = new ArrayList<Size>();
            for (String s : splitter) {
                Size size = strToSize(s);
                if (size != null) sizeList.add(size);
            }
            if (sizeList.size() == 0) return null;
            return sizeList;
        }

        // Parses a string (ex: "480x320") to Size object.
        // Return null if the passing string is null.
        private Size strToSize(String str) {
            if (str == null) return null;

            int pos = str.indexOf('x');
            if (pos != -1) {
                String width = str.substring(0, pos);
                String height = str.substring(pos + 1);
                return new Size(Integer.parseInt(width),
                                Integer.parseInt(height));
            }
            Log.e(TAG, "Invalid size parameter string=" + str);
            return null;
        }
    };
};
