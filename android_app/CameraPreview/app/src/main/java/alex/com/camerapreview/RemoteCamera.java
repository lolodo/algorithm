package alex.com.camerapreview;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;

import java.lang.ref.WeakReference;
import java.util.List;

/**
 * Created by fangyuan on 5/10/18.
 */

public class RemoteCamera {
    private static final String TAG = "CameraPreview";
    private Parameters mRemoteCameraParams = null;
    private List<Size> mSupportedPreviewSizes;
    private List<Integer> mSupportedPreviewFrameRates;
    private List<String> mSupportedPreiewFormats;
    private DecoderThread mDecoderThread = null;

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

    private String mCameraName;

    private SurfaceHolder mSurfaceHolder = null;

    public Parameters getEmptyParameters() {
        Parameters p = new Parameters();
        return p;
    }

    private static boolean IPCheck(String IPAddr) {
        if (IPAddr != null && !IPAddr.isEmpty()) {
            // define regex
            String regex = "^(((\\d{1,2})|(1\\d{2})|(2[2-4]\\d)|(25[0-5]))\\.){3}((\\d{1,2})|(1\\d{2})|(2[2-4]\\d)|(25[0-5]))$";
            //check if ip matched
            if (IPAddr.matches(regex)) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    private static boolean check_port(int port) {
        if (port < 0 || port > 65535) {
            Log.e(TAG, "check_port: port should in [0, 65535]");
            return false;
        }

        return true;
    }

    private boolean checkPreviewSize(int width, int height) {
        if (mSupportedPreviewSizes != null) {
            for (Size s : mSupportedPreviewSizes) {
                Log.d(TAG, "framesize s: " + s.width + "x" + s.height);
                if (s.width == width && s.height == height) {
                    return true;
                }
            }
        }
        return false;
    }

    private boolean checkFrameRate(int fps) {
        if (mSupportedPreviewFrameRates != null) {
            for (Integer f : mSupportedPreviewFrameRates) {
                Log.d(TAG, "framerate f: " + f);
                if (f.intValue() == fps) {
                    return true;
                }
            }
        }
        return false;
    }

    public static Parameters getParameters() {
        Parameters p = new Parameters();
        String s = native_getParameters();
        p.unflatten(s);
        return p;
    }

    public void registerSurface(SurfaceHolder holder) {
        mSurfaceHolder = holder;
    }

    public int start() {
        int ret;

        if (mDecoderThread != null) {
            mDecoderThread.releaseDecoder();
            mDecoderThread.destroy();
            mDecoderThread = null;
        }

        mDecoderThread = new DecoderThread(mSurfaceHolder);
        if (mRemoteCameraParams == null) {
            mRemoteCameraParams = getParameters();
        }

        Log.d(TAG, "start: init decoder");
        Size previewSize = mRemoteCameraParams.getPreviewSize();

        if (!mDecoderThread.initDecoder(previewSize.width, previewSize.height)) {
            Log.e(TAG, "start: init decoder failed");
            return -1;
        }

        ret = native_start_stream();
        if (ret != 0) {
            Log.e(TAG, "start: start remote camera stream failed");
            mDecoderThread.releaseDecoder();
            mDecoderThread = null;
            return -1;
        }

        if (mDecoderThread.isAlive() == false) {
            mDecoderThread.start();
        }

        return 0;
    }

    public void setCameraName(String camName) {
        mCameraName = camName;
    }

    public int connect(String serverIP, int serverPort, int clientPort) {
        int ret = -1;

        if (false == IPCheck(serverIP)) {
            Log.e(TAG, "connect: server ip is wrong:" + serverIP);
            return -1;
        }

        if (false == check_port(serverPort)) {
            Log.e(TAG, "connect: server port is wrong");
            return -1;
        }

        if (false == check_port(clientPort)) {
            Log.e(TAG, "connect: client port is wrong");
            return -1;
        }

        return native_init_client(serverIP, serverPort, clientPort);
    }

    public int configure(String format, int fps, int width, int height, String ip, int port) {
        mRemoteCameraParams = getParameters();
        mSupportedPreviewSizes = mRemoteCameraParams.getSupportedPreviewSizes();
        mSupportedPreviewFrameRates = mRemoteCameraParams.getSupportedPreviewFrameRates();
        mSupportedPreiewFormats = mRemoteCameraParams.getSupportedPreviewFormats();

        Log.d(TAG, "format: " + format + ", fps: " + fps + ", width: " + width + ", height: " + height);

        if (format == null) {
            Log.e(TAG, "configure: format is null");
            return -1;
        }

        if (checkPreviewSize(width, height)) {
            mRemoteCameraParams.setPreviewSize(width, height);
        } else {
            Log.e(TAG, "configure: unsupport preview size");
            return -1;
        }

        if (fps > 0) {
            if (checkFrameRate(fps)) {
                mRemoteCameraParams.setPreviewFrameRate(fps);
            } else {
                Log.e(TAG, "configure: unsupport framerate");
                return -1;
            }
        }

        mRemoteCameraParams.setClientInfo(ip, port);
        native_setParameters(mRemoteCameraParams.flatten());
        return 0;
    }
}
