package com.nextev.screensharing.activity;

import android.app.Presentation;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Point;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;

import com.nextev.screensharing.R;

/**
 * Created by walimis on 17-10-19.
 */

public class VirtualDisplayPresentation extends Presentation implements SurfaceHolder.Callback {
    private static final String TAG = "VDPresentation";
    final PresentationContents mContents;

    /*************************Camera****************************/
    private android.hardware.Camera mCameraDevice;
    private android.hardware.Camera.Parameters mCameraParameters;
    private SurfaceHolder mSurfaceHolder;
    private boolean mIsPreviewing = false;

    /***********************************************************/

    public VirtualDisplayPresentation(Context context, Display display, PresentationContents contents) {
        super(context, display);
        Log.d(TAG, "init");
        mContents = contents;
    }

    /**
     * Sets the preferred display mode id for the presentation.
     */
    public void setPreferredDisplayMode(int modeId) {
        Log.d(TAG, "VirtualDisplayPresentation");
        mContents.displayModeId = modeId;

        WindowManager.LayoutParams params = getWindow().getAttributes();
        params.preferredDisplayModeId = modeId;
        getWindow().setAttributes(params);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Be sure to call the super class.
        super.onCreate(savedInstanceState);

        Log.d(TAG, "create");
        // Get the resources for the context of the presentation.
        // Notice that we are getting the resources from the context of the presentation.
        Resources r = getContext().getResources();

        // Inflate the layout.
        setContentView(R.layout.presentation_content);

        final Display display = getDisplay();
        final int displayId = display.getDisplayId();

        // Get SurfaceView for CameraPreview.
        SurfaceView sv = (SurfaceView) findViewById(R.id.id_surface);
        SurfaceHolder holder = sv.getHolder();
        holder.addCallback(this);
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        GradientDrawable drawable = new GradientDrawable();
        drawable.setShape(GradientDrawable.RECTANGLE);
        drawable.setGradientType(GradientDrawable.RADIAL_GRADIENT);

        // Set the background to a random gradient.
        Point p = new Point();
        getDisplay().getSize(p);
        drawable.setGradientRadius(Math.max(p.x, p.y) / 2);
        drawable.setColors(mContents.colors);
        findViewById(android.R.id.content).setBackground(drawable);

        /*****************************Camera*********************/
        mCameraDevice = android.hardware.Camera.open(0);
        mCameraParameters = mCameraDevice.getParameters();
        mCameraParameters.setPreviewSize(1280, 720);
        mCameraDevice.setParameters(mCameraParameters);
//            mCameraDevice.setDisplayOrientation((90 + 360) % 360);
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.d(TAG, "----onStart");
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d(TAG, "onStop");
        stopPreview();
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
        Log.d(TAG, "Surface Size: " + width + " X " + height);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        Log.d(TAG, "surfaceCreated");

        if (surfaceHolder.getSurface() == null) {
            Log.e(TAG, "surfaceHolder.getSurface() == null");
            return;
        }
        mSurfaceHolder = surfaceHolder;

        try {
            mCameraDevice.setPreviewDisplay(surfaceHolder);
        } catch (Throwable ex) {
            relaseCamera();
            throw new RuntimeException("setPreviewDisplay failed", ex);
        }
        startPreview();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        Log.d(TAG, "surfaceDestroyed");
        mSurfaceHolder = null;
    }

    private void startPreview() {
        if (!mIsPreviewing) {
            mCameraDevice.startPreview();
            mIsPreviewing = true;
        }
    }

    private void stopPreview() {
        if (mIsPreviewing) {
            mCameraDevice.stopPreview();
            mIsPreviewing = false;
        }
    }

    private void relaseCamera() {
        stopPreview();
        mCameraDevice.release();
    }
}
