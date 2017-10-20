package com.nextev.screensharing.activity;

import android.app.Activity;
import android.app.Presentation;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.graphics.drawable.GradientDrawable;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.Image;
import android.media.ImageReader;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.SparseArray;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import com.nextev.screensharing.R;

import org.freedesktop.gstreamer.GStreamer;

import java.io.File;
import java.nio.ByteBuffer;

public class VirtualDisplayDemoImageReader extends Activity implements View.OnClickListener {

    private static final String TAG = "VDImageReader";

    private static final int mVirtualScreenWidth = 1280;
    private static final int mVirtualScreenHeight = 720;
    final private HandlerThread handlerThread=new HandlerThread(getClass().getSimpleName(),
            android.os.Process.THREAD_PRIORITY_BACKGROUND);
    private Handler handler;
    private boolean startPush = false;


    /*********************VirtualDisplay************************/
    private static final String PRESENTATION_KEY = "presentation";

    private String mVirtualDisplayName = "myVD";
    private int[] mVirtualContentsBackground = new int[]{
            ((int) (Math.random() * Integer.MAX_VALUE)) | 0xFF000000,
            ((int) (Math.random() * Integer.MAX_VALUE)) | 0xFF000000};

    // List of presentation contents indexed by displayId.
    // This state persists so that we can restore the old presentation
    // contents when the activity is paused or resumed.
    private SparseArray<PresentationContents> mSavedPresentationContents;

    // List of all currently visible presentations indexed by display id.
    private final SparseArray<VirtualDisplayPresentation> mActivePresentations =
            new SparseArray<VirtualDisplayPresentation>();

    private DisplayManager mDisplayManager;
    private Display[] mDisplays;
    private VirtualDisplay mVirtualDisplay;
    private Surface mVirtualDisplaySurface;

    /**************************************************************/
    /**************************************************************/
    private Button mButton;
    private boolean mIsRecording = false;

    /**************************************************************/

    private native void nativeInit();     // Initialize native code, build pipeline, etc
    private native void nativeFinalize(); // Destroy pipeline and shutdown native code
    private native void nativeSetUri(String uri); // Set the URI of the media to play
    private native void nativePlay();     // Set pipeline to PLAYING
    private native void nativeSetPosition(int milliseconds); // Seek to the indicated position, in milliseconds
    private native void nativePause();    // Set pipeline to PAUSED
    private static native boolean nativeClassInit(); // Initialize native class: cache Method IDs for callbacks
    private native void nativeSurfaceInit(Object surface); // A new surface is available
    private native void nativeSurfaceFinalize(); // Surface about to be destroyed
    public static native void nativePushBuff(ByteBuffer data); // A new surface is available
    private long native_custom_data;      // Native code will use this to keep private data

    private static native boolean nativejpegClassInit(); // Initialize native class: cache Method IDs for callbacks
    private native void nativejpegInit();     // Initialize native code, build pipeline, etc
    private native void nativejpegFinalize(); // Destroy pipeline and shutdown native code
    public static native void nativejpegPushBuff(ByteBuffer data); // A new surface is available


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        try {
            GStreamer.init(this);
        } catch (Exception e) {
            Toast.makeText(this, e.getMessage(), Toast.LENGTH_LONG).show();
        }

        if (savedInstanceState != null) {
            mSavedPresentationContents = savedInstanceState.getSparseParcelableArray(PRESENTATION_KEY);
        } else {
            mSavedPresentationContents = new SparseArray<PresentationContents>();
        }

        /* config video */
        /*
        mRecorder.configVideoSize(mVideoWidth, mVideoHeight);
        mRecorder.configVideoRate(mVideoFrameRate, mVideoBitrate);
        mRecorder.configCodec(mVideoMIMEType, mVideoIFrameInterval);
        boolean isEncoderInitialSuccess = false;
        try {
            isEncoderInitialSuccess = mRecorder.initEncoder();
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (!isEncoderInitialSuccess) {
            Log.e(TAG, "Initial video recorder failed...");
            return;
        }
        */

        ImageTransmogrifier it;
        it = new ImageTransmogrifier();

        mDisplayManager = (DisplayManager) getSystemService(Context.DISPLAY_SERVICE);

        //mVirtualDisplaySurface = mRecorder.getCodecSurface();
        mVirtualDisplaySurface = it.getSurface();
        Log.d(TAG, "Surface:    " + mVirtualDisplaySurface);

        mVirtualDisplay = mDisplayManager.createVirtualDisplay(mVirtualDisplayName, mVirtualScreenWidth, mVirtualScreenHeight,
                DisplayMetrics.DENSITY_DEFAULT, mVirtualDisplaySurface, DisplayManager.VIRTUAL_DISPLAY_FLAG_PRESENTATION);
        Surface mVirtualSurface = mVirtualDisplay.getSurface();

        Log.d(TAG, "Surface:    " + mVirtualSurface);

        mDisplays = mDisplayManager.getDisplays();
        for (Display display : mDisplays) {
            Log.d(TAG, "    " + display);
        }

        setContentView(R.layout.activity_virtual_display);
        mButton = findViewById(R.id.button);
        mButton.setOnClickListener(this);

        // check camera permission
        /*
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                == PackageManager.PERMISSION_GRANTED) {
            Log.d(TAG, "--2--onCreate");
        } else {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.CAMERA}, 1);//1 can be another integer
        }
        */
        nativeInit();
        nativejpegInit();

        handlerThread.start();
        handler=new Handler(handlerThread.getLooper());

    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "--1--onReseume");

        // Register to receive events from the display manager.
        mDisplayManager.registerDisplayListener(mDisplayListener, null);

        Display useDisplay = null;
        for (Display display : mDisplays) {
            Log.d(TAG, "[1] " + display.getName() + "; [2] " + mVirtualDisplayName);
            if (display.getName().equals(mVirtualDisplayName)) {
                useDisplay = display;
                Log.d(TAG, "My use virtual display: " + useDisplay);
            }
        }

        if (useDisplay == null) {
            Log.e(TAG, "Virtual display get failed...");
            return;
        }
        PresentationContents contents = new PresentationContents(mVirtualContentsBackground);
        showPresentation(useDisplay, contents);
    }

    @Override
    protected void onPause() {
        super.onPause();

        // Unregister from the display manager.
        mDisplayManager.unregisterDisplayListener(mDisplayListener);

        // Dismiss all of our presentations but remember their contents.
        Log.d(TAG, "Activity is being paused. Dismissing all active presentation.");
        for (int i = 0; i < mActivePresentations.size(); i++) {
            VirtualDisplayPresentation presentation = mActivePresentations.valueAt(i);
            int displayId = mActivePresentations.keyAt(i);
            mSavedPresentationContents.put(displayId, presentation.mContents);
            presentation.dismiss();
        }
        mActivePresentations.clear();

        if (mIsRecording) {
            //stopRecord();
            mButton.setText("Restart Record");
            mIsRecording = false;
        }
    }

    @Override
    public void onClick(View view) {
        if (mIsRecording) {
            startPush = false;
            //stopRecord();
            mButton.setText("Restart Record");
            mIsRecording = false;
        } else {
            startPush = true;
            //startRecord();
            mIsRecording = true;
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        // Be sure to call the super class.
        super.onSaveInstanceState(outState);
        Log.d(TAG, "--1--onSaveInstanceState");
        outState.putSparseParcelableArray(PRESENTATION_KEY, mSavedPresentationContents);
    }

    private void startRecord() {
        // video size
        File file = new File(Environment.getExternalStorageDirectory(),
                "record-" + mVirtualScreenWidth + "x" + mVirtualScreenHeight + "-" + System.currentTimeMillis() + ".mp4");
        mButton.setText("Stop Recorder");
        Toast.makeText(this, "Screen recorder is running..." + file.getAbsolutePath(), Toast.LENGTH_SHORT).show();
    }

    private void stopRecord() {
    }

    /**
     * Shows a {@link Presentation} on the specified display.
     */
    private void showPresentation(Display display, PresentationContents contents) {
        final int displayId = display.getDisplayId();
        if (mActivePresentations.get(displayId) != null) {
            return;
        }

        VirtualDisplayPresentation presentation = new VirtualDisplayPresentation(this, display, contents);
        presentation.show();
        presentation.setOnDismissListener(mOnDismissListener);
        mActivePresentations.put(displayId, presentation);
    }

    /**
     * Hides a {@link Presentation} on the specified display.
     */
    private void hidePresentation(Display display) {
        final int displayId = display.getDisplayId();
        VirtualDisplayPresentation presentation = mActivePresentations.get(displayId);
        if (presentation == null) {
            return;
        }

        Log.d(TAG, "Dismissing presentation on display #" + displayId + ".");

        presentation.dismiss();
        mActivePresentations.delete(displayId);
    }

    /**
     * Sets the display mode of the {@link Presentation} on the specified display
     * if it is already shown.
     */
    private void setPresentationDisplayMode(Display display, int displayModeId) {
        final int displayId = display.getDisplayId();
        VirtualDisplayPresentation presentation = mActivePresentations.get(displayId);
        if (presentation == null) {
            return;
        }

        presentation.setPreferredDisplayMode(displayModeId);
    }


    /**
     * Listens for when presentations are dismissed.
     */
    private final DialogInterface.OnDismissListener mOnDismissListener =
            new DialogInterface.OnDismissListener() {
                @Override
                public void onDismiss(DialogInterface dialog) {
                    VirtualDisplayPresentation presentation = (VirtualDisplayPresentation) dialog;
                    int displayId = presentation.getDisplay().getDisplayId();
                    Log.d(TAG, "Presentation on display #" + displayId + " was dismissed.");
                    mActivePresentations.delete(displayId);
                }
            };


    /**
     * Listens for displays to be added, changed or removed.
     * We use it to update the list and show a new {@link Presentation} when a
     * display is connected.
     * <p>
     * Note that we don't bother dismissing the {@link Presentation} when a
     * display is removed, although we could.  The presentation API takes care
     * of doing that automatically for us.
     */
    private final DisplayManager.DisplayListener mDisplayListener =
            new DisplayManager.DisplayListener() {
                @Override
                public void onDisplayAdded(int displayId) {
                    Log.d(TAG, "Display #" + displayId + " added.");
                }

                @Override
                public void onDisplayChanged(int displayId) {
                    Log.d(TAG, "Display #" + displayId + " changed.");
                }

                @Override
                public void onDisplayRemoved(int displayId) {
                    Log.d(TAG, "Display #" + displayId + " removed.");
                }
            };



    private void setMessage(final String message) {
        Log.i(TAG, "setMessageNew:" + message);
        if (message.equals("State changed to PLAYING")) {
            startPush = true;

        }else if (message.equals("State changed to PAUSED")) {
            startPush = false;

        }
    }

    private void setCurrentPosition(final int position, final int duration) {

    }

    private void onMediaSizeChanged (int width, int height) {

    }

    private void onGStreamerInitialized () {

    }


    static {
        System.loadLibrary("gstreamer_android");
        System.loadLibrary("screensharing");
        System.loadLibrary("minicap-common");
        nativeClassInit();
        nativejpegClassInit();
    }

    public class ImageTransmogrifier implements ImageReader.OnImageAvailableListener {
        private static final String TAG = "ImageTransmogrifier";
        private final int width;
        private final int height;
        private final ImageReader imageReader;
        private Bitmap latestBitmap = null;

        ImageTransmogrifier() {


            this.width = mVirtualScreenWidth;
            this.height = mVirtualScreenHeight;

            imageReader = ImageReader.newInstance(width, height,
                    PixelFormat.RGBA_8888, 2);
            imageReader.setOnImageAvailableListener(this, handler);
            Log.d(TAG, "ImageTransmogrifier :" + imageReader.getSurface().toString());
        }

        @Override
        public void onImageAvailable(ImageReader reader) {
            final Image image = reader.acquireLatestImage();
            //Image image = reader.acquireNextImage();
            Log.d(TAG, "onImageAvailable :" + imageReader.getSurface().toString());


            if (image != null) {
                Image.Plane[] planes = image.getPlanes();
                ByteBuffer buffer = planes[0].getBuffer();
                if (startPush) {
                    nativePushBuff(buffer);
                    nativejpegPushBuff(buffer);
                }

                if (image != null) {
                    image.close();
                }
                /*
                int pixelStride = planes[0].getPixelStride();
                int rowStride = planes[0].getRowStride();
                int rowPadding = rowStride - pixelStride * width;
                int bitmapWidth = width + rowPadding / pixelStride;

                if (latestBitmap == null ||
                        latestBitmap.getWidth() != bitmapWidth ||
                        latestBitmap.getHeight() != height) {
                    if (latestBitmap != null) {
                        latestBitmap.recycle();
                    }

                    latestBitmap = Bitmap.createBitmap(bitmapWidth,
                            height, Bitmap.Config.ARGB_8888);
                }

                latestBitmap.copyPixelsFromBuffer(buffer);

                if (image != null) {
                    image.close();
                }

                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                Bitmap cropped = Bitmap.createBitmap(latestBitmap, 0, 0,
                        width, height);

                cropped.compress(Bitmap.CompressFormat.JPEG, 100, baos);

                byte[] newPng = baos.toByteArray();
                //nativePushBuff(newPng);
                */

            }
        }

        Surface getSurface() {
            return (imageReader.getSurface());
        }

        int getWidth() {
            return (width);
        }

        int getHeight() {
            return (height);
        }

        void close() {
            imageReader.close();
        }
    }


}
