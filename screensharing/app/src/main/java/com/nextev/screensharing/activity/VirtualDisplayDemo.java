package com.nextev.screensharing.activity;

import android.app.Activity;
import android.app.Presentation;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.graphics.Point;
import android.graphics.drawable.GradientDrawable;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.os.Bundle;
import android.os.Environment;
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
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicBoolean;

public class VirtualDisplayDemo extends Activity implements View.OnClickListener {

    private static final String TAG = "VirtualDisplayDemo";

    private static final int mVirtualScreenWidth = 1280;
    private static final int mVirtualScreenHeight = 720;

    /********************MediaCodecConfigure*******************/
    private static final int mVideoWidth = 1280;
    private static final int mVideoHeight = 720;
    private static final int mVideoBitrate = 6000000;
    private static final int mVideoFrameRate = 30;
    private static final int mVideoIFrameInterval = 2;         // 2 seconds between I-frames
    private static final String mVideoMIMEType = "video/avc";  // H.264 Advanced Video coding

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
    private HiddenRecorder mRecorder = new HiddenRecorder();
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

        mDisplayManager = (DisplayManager) getSystemService(Context.DISPLAY_SERVICE);
        mVirtualDisplaySurface = mRecorder.getCodecSurface();
        mVirtualDisplay = mDisplayManager.createVirtualDisplay(mVirtualDisplayName, mVirtualScreenWidth, mVirtualScreenHeight,
                DisplayMetrics.DENSITY_DEFAULT, mVirtualDisplaySurface, DisplayManager.VIRTUAL_DISPLAY_FLAG_PRESENTATION);
        Surface mVirtualSurface = mVirtualDisplay.getSurface();
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
            stopRecord();
            mButton.setText("Restart Record");
            mIsRecording = false;
        }
    }

    @Override
    public void onClick(View view) {
        if (mIsRecording) {
            stopRecord();
            mButton.setText("Restart Record");
            mIsRecording = false;
        } else {
            startRecord();
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
        mRecorder.setVideoFile(file.getAbsolutePath());
        mRecorder.start();
        mButton.setText("Stop Recorder");
        Toast.makeText(this, "Screen recorder is running..." + file.getAbsolutePath(), Toast.LENGTH_SHORT).show();
    }

    private void stopRecord() {
        mRecorder.quit();
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


    /**
     * The presentation to show on the secondary display.
     * <p>
     * Note that the presentation display may have different metrics from the display on which
     * the main activity is showing so we must be careful to use the presentation's
     * own {@link Context} whenever we load resources.
     */
    private final class VirtualDisplayPresentation extends Presentation implements SurfaceHolder.Callback {

        final PresentationContents mContents;

        /*************************Camera****************************/
        private android.hardware.Camera mCameraDevice;
        private android.hardware.Camera.Parameters mCameraParameters;
        private SurfaceHolder mSurfaceHolder;
        private boolean mIsPreviewing = false;

        /***********************************************************/

        public VirtualDisplayPresentation(Context context, Display display, PresentationContents contents) {
            super(context, display);
            Log.d(TAG, "--1--VirtualDisplayPresentation");
            mContents = contents;
        }

        /**
         * Sets the preferred display mode id for the presentation.
         */
        public void setPreferredDisplayMode(int modeId) {
            Log.d(TAG, "--2--VirtualDisplayPresentation");
            mContents.displayModeId = modeId;

            WindowManager.LayoutParams params = getWindow().getAttributes();
            params.preferredDisplayModeId = modeId;
            getWindow().setAttributes(params);
        }

        @Override
        protected void onCreate(Bundle savedInstanceState) {
            // Be sure to call the super class.
            super.onCreate(savedInstanceState);

            Log.d(TAG, "--3--VirtualDisplayPresentation");
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
            Log.d(TAG, "-----onStop");
            stopPreview();
        }

        @Override
        public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
            Log.d(TAG, "Surface Size: " + width + " X " + height);
        }

        @Override
        public void surfaceCreated(SurfaceHolder surfaceHolder) {
            Log.d(TAG, "---surfaceCreated");

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
            Log.d(TAG, "---surfaceDestroyed");
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

    /**
     * Information about the content we want to show in the presentation.
     */
    private final static class PresentationContents implements Parcelable {

        final int[] colors;
        int displayModeId;

        public static final Creator<PresentationContents> CREATOR =
                new Creator<PresentationContents>() {
                    @Override
                    public PresentationContents createFromParcel(Parcel in) {
                        return new PresentationContents(in);
                    }

                    @Override
                    public PresentationContents[] newArray(int size) {
                        return new PresentationContents[size];
                    }
                };


        public PresentationContents(int[] colors) {
            this.colors = colors;
        }

        private PresentationContents(Parcel in) {
            colors = new int[]{in.readInt(), in.readInt()};
            displayModeId = in.readInt();
        }

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeInt(colors[0]);
            dest.writeInt(colors[1]);
            dest.writeInt(displayModeId);
        }
    }

    private void setMessage(final String message) {
        Log.i(TAG, "setMessageNew:" + message);
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
        nativeClassInit();
    }
    public class HiddenRecorder extends Thread {

        private static final String TAG = "HiddenRecorder";

        private int mVideoWidth = 1280;
        private int mVideoHeight = 720;
        private int mVideoBitrate = 6000000;
        private int mVideoFrameRate = 30;
        private int mVideoIFrameInterval = 2;         // 2 seconds between I-frames
        private String mVideoMIMEType = "video/avc";  // H.264 Advanced Video coding
        private int mWaitOutBufferTimeUs = 10000;

        private MediaCodec mEncoder;
        private MediaMuxer mMuxer;
        private MediaCodec.BufferInfo mBufferInfo = new MediaCodec.BufferInfo();
        private Surface mSourceSurface;

        private boolean mMuxerStarted = false;
        private int mVideoTrackIndex = -1;
        private AtomicBoolean mQuit = new AtomicBoolean(false);
        private String mVideoFileName;

        public void configVideoSize(int width, int height) {
            mVideoWidth = width;
            mVideoHeight = height;
        }

        public void configVideoRate(int frameRate, int bitRate) {
            mVideoFrameRate = frameRate;
            mVideoBitrate = bitRate;
        }

        public void configCodec(String mimeType, int iFrameInterval) {
            mVideoMIMEType = mimeType;
            mVideoIFrameInterval = iFrameInterval;
        }

        public boolean initEncoder() throws IOException {
            MediaFormat format = MediaFormat.createVideoFormat(mVideoMIMEType, mVideoWidth, mVideoHeight);
            if (format == null) {
                Log.e(TAG, "Create video format failed...");
                return false;
            }
            format.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
            format.setInteger(MediaFormat.KEY_BIT_RATE, mVideoBitrate);
            format.setInteger(MediaFormat.KEY_FRAME_RATE, mVideoFrameRate);
            format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, mVideoIFrameInterval);

            Log.d(TAG, "Created video format: " + format);

            mEncoder = MediaCodec.createEncoderByType(mVideoMIMEType);
            if (mEncoder == null) {
                Log.e(TAG, "Create video encoder failed...");
                return false;
            }
            mEncoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            /*
            mEncoder.setCallback(new MediaCodec.Callback() {
                @Override
                public void onInputBufferAvailable(MediaCodec mediaCodec, int i) {
                    //ByteBuffer rawData = mEncoder.getInputBuffer(i);
                    //nativePushBuff(rawData);

                }

                @Override
                public void onOutputBufferAvailable(MediaCodec mediaCodec, int i, MediaCodec.BufferInfo bufferInfo) {

                }

                @Override
                public void onError(MediaCodec mediaCodec, MediaCodec.CodecException e) {

                }

                @Override
                public void onOutputFormatChanged(MediaCodec mediaCodec, MediaFormat mediaFormat) {

                }
            });
            */
            mSourceSurface = mEncoder.createInputSurface();
            if (mSourceSurface == null) {
                Log.e(TAG, "Create encode source surface failed...");
                mEncoder.release();
                mEncoder = null;
                return false;
            }
            return true;
        }

        public Surface getCodecSurface() {
            return mSourceSurface;
        }

        public void setVideoFile(String fileName) {
            mVideoFileName = fileName;
        }

        public void quit() {
            mQuit.set(true);
        }

        @Override
        public void run() {
            super.run();

            try {
                try {
                    mEncoder.start();
                    mMuxer = new MediaMuxer(mVideoFileName, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                recordRun();

            } finally {
                release();
            }
        }

        private  void recordRun() {
            while (!mQuit.get()) {
                int index = mEncoder.dequeueOutputBuffer(mBufferInfo, mWaitOutBufferTimeUs);
                if (index == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    Log.d(TAG, "INFO_OUTPUT_FORMAT_CHANGED");
                    resetOutputFormat();
                } else if (index == MediaCodec.INFO_TRY_AGAIN_LATER) {
                    Log.d(TAG, "Retrieving buffers time out!");
                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } else if (index >= 0) {
                    Log.d(TAG, "dequeue output buffer index: " + index);
                    if (!mMuxerStarted) {
                        throw new IllegalStateException("MediaMuxer des not call addTrack(format)");
                    }
                    encodeVideoTrack(index);

                    mEncoder.releaseOutputBuffer(index, false);
                }
                //index = mEncoder.dequeueInputBuffer(mWaitOutBufferTimeUs);
                if (index >= 0) {
                    //ByteBuffer rawData = mEncoder.getInputBuffer(index);
                    //nativePushBuff(rawData);
                }
            }
        }

        private void encodeVideoTrack(int index) {
            ByteBuffer encodedData = mEncoder.getOutputBuffer(index);
            MediaFormat bufferFormat = mEncoder.getOutputFormat(index); // option A
            bufferFormat.getByteBuffer("csd-0");
            bufferFormat.getByteBuffer("csd-1");
            /*
            Image imageEncoder = mEncoder.getOutputImage();
            ByteBuffer imageData = imageEncoder.getPlanes()[0].getBuffer();
            */

            if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                // The codec config data was pulled out and fed to the muxer when we got
                // the INFO_OUTPUT_FORMAT_CHANGED status.
                // Ignore it.
                Log.d(TAG, "Ignore BUFFER_FLAG_CODEC_CONFIG");
                mBufferInfo.size = 0;
                encodedData = null;
            }

            if (encodedData != null) {
                Log.d(TAG, "Video Buffer Info: size=" + mBufferInfo.size
                        + ", presentationTimeUs=" + mBufferInfo.presentationTimeUs
                        + ", offset=" + mBufferInfo.offset);
                encodedData.position(mBufferInfo.offset);
                encodedData.limit(mBufferInfo.offset + mBufferInfo.size);
                //mMuxer.writeSampleData(mVideoTrackIndex, encodedData, mBufferInfo);
                nativePushBuff(encodedData);
            }
        }

        private void resetOutputFormat() {
            // should happen before receiving buffers, and should only happen once
            if (mMuxerStarted) {
                throw new IllegalStateException("output format already changed!");
            }
            MediaFormat newFormat =  mEncoder.getOutputFormat();

            Log.d(TAG, "Output Format changed.\n new format: " + newFormat.toString());

            mVideoTrackIndex = mMuxer.addTrack(newFormat);
            mMuxer.start();
            mMuxerStarted = true;
            Log.d(TAG, "Start MediaMuxer, videoIndex = " + mVideoTrackIndex);
        }

        private void release() {
            if (mEncoder != null) {
                mEncoder.stop();
                mEncoder.release();
                mEncoder = null;
            }
            if (mMuxer != null) {
                mMuxer.stop();
                mMuxer.release();
                mMuxer = null;
            }
        }


    }
}