package alex.com.camerapreview;

import android.content.Context;
//import android.graphics.Camera;
//import android.hardware.Camera;
import android.hardware.Camera;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.WindowManager;

import java.io.IOException;

/**
 * Created by liufangyuan on 2018/5/9.
 */

public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {
    private SurfaceHolder mHolder;
    private static final String TAG = "CameraPreview";
    private Camera mCamera;

    public CameraPreview(Context context) {
        super(context);
        mHolder = getHolder();
        mHolder.addCallback(this);
    }

    private static Camera getCameraInstance() {
        Camera c = null;
        try {
            c = Camera.open(1);
        } catch (Exception e) {
            Log.d(TAG, "getCameraInstance: camera is not available");
        }
        return c;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mCamera = getCameraInstance();
        try {
            mCamera.setPreviewDisplay(holder);
            mCamera.startPreview();
        } catch (IOException e) {
            Log.d(TAG, "surfaceCreated: error setting camera preview:" + e.getMessage());
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mHolder.removeCallback(this);
        mCamera.setPreviewCallback(null);
        mCamera.stopPreview();
        mCamera.release();
        mCamera = null;
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        int rotation = getDisplayOrientation();
        mCamera.setDisplayOrientation(rotation);

    }

    public int getDisplayOrientation() {
        Display display = ((WindowManager) getContext().getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        int rotation = display.getRotation();
        int degress = 0;
        switch (rotation) {
            case Surface.ROTATION_0:
                degress = 0;
                break;
            case Surface.ROTATION_90:
                degress = 90;
                break;
            case Surface.ROTATION_180:
                degress = 180;
                break;
            case Surface.ROTATION_270:
                degress = 270;
                break;
        }

        android.hardware.Camera.CameraInfo camInfo = new android.hardware.Camera.CameraInfo();
        android.hardware.Camera.getCameraInfo(Camera.CameraInfo.CAMERA_FACING_FRONT, camInfo);
        int result = (360 + 180 + camInfo.orientation - degress) % 360;
        return result;
    }
}
