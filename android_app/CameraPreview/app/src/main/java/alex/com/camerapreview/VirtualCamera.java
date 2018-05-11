package alex.com.camerapreview;

import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;

import java.io.IOException;

/**
 * Created by fangyuan on 5/10/18.
 */

public class VirtualCamera {
    private static final String TAG = "CameraPreview";
    private static final int CAMERA_DVR_INDEX = 0;
    private static final int CAMERA_ROOF_INDEX = 1;

    public static final String CAMERA_DVR = "camera-dvr";
    public static final String CAMERA_ROOF = "camera-roof";
    public static final String CAMERA_AVM_SYNTHESIS = "camera-avm-synthesis";
    public static final String CAMERA_AVM_FRONT = "camera-avm-front";
    public static final String CAMERA_AVM_BACK = "camera-avm-back";
    public static final String CAMERA_AVM_LEFT = "camera-avm-left";
    public static final String CAMERA_AVM_RIGHT = "camera-avm-right";
    public static final String CAMERA_DRIVER = "camera-driver";

    public static final String KEY_PREVIEW_FORMAT_H264 = "preview-format-h264";
    private static final int PREVIEW_FPS = 30;

    /* Remote Camera */
    private static final String SERVER_IP = "172.20.1.11";
    private static final int SERVER_PORT = 37568;
    private static final String CLIENT_IP = "172.20.1.10";
    private static final int CLIENT_PORT = 8554;

    private RemoteCamera mRemoteCamera = new RemoteCamera();
    SurfaceHolder mSurfaceHolder;

    private static final int PREVIEW_WIDTH = 1280;
    private static final int PREVIEW_HEIGHT = 800;

    private Camera mCameraDevice;

    public int initDevice(String camName) {
        int ret;
        switch (camName) {
            case CAMERA_DVR:
                Log.d(TAG, "initDevice:CAMERA_DVR");
                ret = Camera.getNumberOfCameras();
                if (ret == 0) {
                    Log.d(TAG, "initDevice: no camera!");
                    return -1;
                }

                Log.d(TAG, "initDevice: camera number is " + ret);
                mCameraDevice = Camera.open(CAMERA_DVR_INDEX);
                break;

            case CAMERA_ROOF:
                ret = Camera.getNumberOfCameras();
                if (ret == 0) {
                    Log.d(TAG, "initDevice: no camera!");
                    return -1;
                }
                Log.d(TAG, "initDevice:CAMERA_ROOF");
                mCameraDevice = Camera.open(CAMERA_ROOF_INDEX);
                break;

            case CAMERA_AVM_SYNTHESIS:
            case CAMERA_AVM_FRONT:
            case CAMERA_AVM_BACK:
            case CAMERA_AVM_LEFT:
            case CAMERA_AVM_RIGHT:
            case CAMERA_DRIVER:
                mRemoteCamera.setCameraName(camName);
                break;

            default:
                Log.d(TAG, "initDevice: wrong name:" + camName);
                return -1;
        }

        if (mSurfaceHolder != null) {
            mRemoteCamera.registerSurface(mSurfaceHolder);
        } else {
            Log.e(TAG, "initDevice: surfaceHolder is null!");
            return -1;
        }

        ret = mRemoteCamera.connect(SERVER_IP, SERVER_PORT, CLIENT_PORT);
        if (ret != 0) {
            Log.e(TAG, "initDevice: connect remote camera failed!");
            return -1;
        } else {
            Log.d(TAG, "initDevice: connect succeed!");
            ret = mRemoteCamera.configure(KEY_PREVIEW_FORMAT_H264, PREVIEW_FPS, PREVIEW_WIDTH, PREVIEW_HEIGHT, CLIENT_IP, CLIENT_PORT);
            if (ret != 0) {
                Log.e(TAG, "initDevice: configure failed!");
                return -1;
            }

            Log.d(TAG, "initDevice: configure succeed!");
        }

        return 0;
    }

    public void registerSurface(SurfaceHolder holder) throws IOException {
        mSurfaceHolder = holder;
        Log.d(TAG, "registerSurface:enter");
        if (mCameraDevice != null) {
            Log.d(TAG, "registerSurface:setPreviewDisplay");
            mCameraDevice.setPreviewDisplay(holder);
        }
    }

    public int startStream(String camName) {
        Log.d(TAG, "startStream: " + camName);
        switch (camName) {
            case CAMERA_DVR:
            case CAMERA_ROOF:
                Camera.Parameters parameters = mCameraDevice.getParameters();
                parameters.setPreviewSize(PREVIEW_WIDTH, PREVIEW_HEIGHT);
                parameters.setPictureSize(PREVIEW_WIDTH, PREVIEW_HEIGHT);
                mCameraDevice.setParameters(parameters);
                mCameraDevice.startPreview();
                break;

            case CAMERA_AVM_SYNTHESIS:
            case CAMERA_AVM_FRONT:
            case CAMERA_AVM_BACK:
            case CAMERA_AVM_LEFT:
            case CAMERA_AVM_RIGHT:
            case CAMERA_DRIVER:
                mRemoteCamera.registerSurface(mSurfaceHolder);
                mRemoteCamera.start();
                break;

            default:
                Log.d(TAG, "initDevice: wrong name:" + camName);
                return -1;
        }
        return 0;
    }
}
