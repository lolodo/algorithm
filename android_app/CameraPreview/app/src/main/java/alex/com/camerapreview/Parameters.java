package alex.com.camerapreview;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;

/**
 * Created by fangyuan on 5/11/18.
 */

public class Parameters {
    private static final String KEY_EVT_VERSION = "evt-version";
    private static final String KEY_CAMERA_NAME = "camera-name";

    private static final String KEY_PREVIEW_SIZE = "preview-size";
    private static final String KEY_PREVIEW_FORMAT = "preview-format";
    private static final String KEY_PREVIEW_FRAMERATE = "preview-fps";

    private static final String KEY_PICTURE_SIZE = "picture-size";
    private static final String KEY_PICTURE_FORMAT = "picture-format";

    private static final String KEY_CLIENT_ADDRESS = "receive-ip-address";
    private static final String KEY_CLINET_PORT = "receive-port";
    private static final String TAG = "CameraPreview";

    // Parameter key suffix for supported values.
    private static final String SUPPORTED_VALUES_SUFFIX = "-values";

    private final LinkedHashMap<String, String> mMap;

    public Parameters() {
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
        flattened.deleteCharAt(flattened.length() - 1);
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