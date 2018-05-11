package testcamera.mega.com.gladiuscamera;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Handler;
import android.util.Log;
import android.view.SurfaceHolder;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import mega.net.RemoteCamera;

public class RemoteCameraPlayer implements RemoteCamera.PreviewCallback {

    private static final boolean DEBUG = false;
    private final static boolean VERBOSE = true;
    private final static String TAG = "RemoteCameraPlayer";

    private static final int CACHE_BUF_COUNT = 5;
    private static final int VIDEO_BUF_SIZE = 1000000;
    private static final int VIDEO_BUF_SUM = 10;

    private static final int GET_BUF_TIMEOUT = 30;

    private Object mLock = new Object();
    // Remote Camera
    private RemoteCamera mRemoteCamera = RemoteCamera.getInstance();
    private RemoteCamera.Parameters mRemoteCameraParams = null;
    private boolean mIsRemoteConnected = false;

    private List<RemoteCamera.Size> mSupportedPreiewSizes;
    private List<String> mSupportedPreiewFormats;
    private List<Integer> mSupportedPreviewFrameRates;

    private String mEvtVersion = RemoteCamera.EVT_VERSION_2_0;
    private String mCameraName = RemoteCamera.REMOTE_CAMERA_AVM_SYNTHESIS;

    // thread.
    private Thread mPlayerMainThread = null;
    private DecoderThread mDecoderThread = null;
    private boolean decoder_init_flag = false;

    private boolean mIsStreaming = false;
    private boolean mThreadExited = false;
    private boolean mThreadSelfExit = false;
    private Object mDataLock = new Object();
    private ArrayList<VideoBuffer> mVideoQueue = new ArrayList<VideoBuffer>();
    private int mFrameCnt = 0;
    private Handler mHandler = null;
    private SurfaceHolder mSurfaceHolder = null;

    private static volatile RemoteCameraPlayer singleton = null;

    public static RemoteCameraPlayer getInstance(String evt, String camera) {
        if (singleton == null) {
            synchronized (RemoteCameraPlayer.class) {
                singleton = new RemoteCameraPlayer(evt, camera);
            }
        }
        return singleton;
    }

    private RemoteCameraPlayer(String evt, String camera) {
        mEvtVersion = evt;
        mCameraName = camera;
    }

    public void setCameraName(String camera) {
        mCameraName = camera;
    }

    public void registerHandler(Handler handler) {
        mHandler = handler;

        switch (mEvtVersion) {
            case RemoteCamera.EVT_VERSION_1_0:
                if (!mCameraName.equals(RemoteCamera.REMOTE_CAMERA_DVR) &&
                        !mCameraName.equals(RemoteCamera.REMOTE_CAMERA_ROOF)) {
                    Log.e(TAG, "Evt1.0 not support camera " + mCameraName);
                    // mHandler.sendMessage();
                }
                return;
            case RemoteCamera.EVT_VERSION_2_0:
                if (!mCameraName.equals(RemoteCamera.REMOTE_CAMERA_AVM_SYNTHESIS) &&
                        !mCameraName.equals(RemoteCamera.REMOTE_CAMERA_AVM_FRONT) &&
                        !mCameraName.equals(RemoteCamera.REMOTE_CAMERA_AVM_BACK) &&
                        !mCameraName.equals(RemoteCamera.REMOTE_CAMERA_AVM_LEFT) &&
                        !mCameraName.equals(RemoteCamera.REMOTE_CAMERA_AVM_RIGHT) &&
                        !mCameraName.equals(RemoteCamera.REMOTE_CAMERA_DRIVER)) {
                    Log.e(TAG, "Evt2.0 not support camera " + mCameraName);
                    // mHandler.sendMessage();
                }
                return;
            default:
                Log.e(TAG, "Evt verson is error.");
                // mHandler.sendMessage();
                return;
        }
    }

    public void unregisterHandler() {
        mHandler = null;
    }

    public void registerSurface(SurfaceHolder holder) {
        mSurfaceHolder = holder;
    }

    public int connect(String serverAddr, int serverPort, int clientPort) {
        int ret = 0;
        Log.d(TAG, "RemoteCameraPlayer: ---1----");
        mRemoteCamera.setPreviewCallback(this);
        Log.d(TAG, "RemoteCameraPlayer: ---2----");
        if (!mIsRemoteConnected) {
            Log.d(TAG, "RemoteCameraPlayer: ---3----");
            ret = mRemoteCamera.connectRemote(serverAddr, serverPort, clientPort);
            Log.d(TAG, "RemoteCameraPlayer: ---4----");
            if (ret == 0) {
                mIsRemoteConnected = true;
                ret = 0;
            } else {
                mIsRemoteConnected = false;
                ret = -1;
            }
        }
        Log.d(TAG, "--1--connect----");

        RemoteCamera.Parameters params = mRemoteCamera.getEmptyParameters();
        params.setEvtVersion(mEvtVersion);
        params.setRemoteCamera(mCameraName);
        mRemoteCamera.setParameters(params);

        Log.d(TAG, "--2--connect [ " + ret + " ]----");

        return ret;
    }

    public void disconnect() {
        stop();
        if (mIsRemoteConnected) {
            mIsRemoteConnected = false;
            mRemoteCamera.disconnectRemote();
        }
    }

    public void configure(String format, int fps, int width, int height, String ip, int port) {

        mRemoteCameraParams = mRemoteCamera.getParameters();
        mSupportedPreiewSizes = mRemoteCameraParams.getSupportedPreviewSizes();
        mSupportedPreiewFormats = mRemoteCameraParams.getSupportedPreviewFormats();
        mSupportedPreviewFrameRates = mRemoteCameraParams.getSupportedPreviewFrameRates();

        Log.d(TAG, "format: " + format + ", fps: " + fps + ", width: " + width + ", height: " + height);

        if (format != null) {
            if (checkFormat(format)) {
                mRemoteCameraParams.setPreviewFormat(format);
            } else {
                Log.w(TAG, "Warning: unsupport format " + format + ", use default format " + mRemoteCameraParams.getPreviewFormat());
                // mHandler.sendMessage();
            }
        }
        if (width > 0 && height > 0) {
            if (checkPreviewSize(width, height)) {
                mRemoteCameraParams.setPreviewSize(width, height);
            } else {
                Log.w(TAG, "Warning: unsupport preview size "
                        + width + "/" + height +
                        ", use default size "
                        + mRemoteCameraParams.getPreviewSize().width + "/" + mRemoteCameraParams.getPreviewSize().height);
                // mHandler.sendMessage();
            }
        }
        if (fps > 0) {
            if (checkFrameRate(fps)) {
                mRemoteCameraParams.setPreviewFrameRate(fps);
            } else {
                Log.w(TAG, "Warning: unsupport framerate " + fps + ", use default framerate " + mRemoteCameraParams.getPreviewFrameRate());
                // mHandler.sendMessage();
            }
        }
        mRemoteCameraParams.setClientInfo(ip, port);
        mRemoteCamera.setParameters(mRemoteCameraParams);
    }

    public int start() {
        Log.d(TAG, "-----start----");
        int ret = 0;
        synchronized (mLock) {
            mThreadExited = false;
            mThreadSelfExit = false;
        }
        if (mIsRemoteConnected) {
            mPlayerMainThread = null;

            mPlayerMainThread = new PlayerMainThread();
            mPlayerMainThread.start();
            ret = -1;
        }

        return ret;
    }

    public void stop() {
        Log.d(TAG, "-----stop----");
        synchronized (mLock) {
            mThreadExited = true;
            mThreadSelfExit = false;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private boolean checkFormat(String format) {
        if (mSupportedPreiewFormats != null) {
            for (String s : mSupportedPreiewFormats) {
                Log.d(TAG, "format s: " + s);
                if (s.equals(format)) {
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

    private boolean checkPreviewSize(int width, int height) {
        if (mSupportedPreiewSizes != null) {
            for (RemoteCamera.Size s : mSupportedPreiewSizes) {
                Log.d(TAG, "framesize s: " + s.width + "x" + s.height);
                if (s.width == width && s.height == height) {
                    return true;
                }
            }
        }
        return false;
    }

    public class VideoBuffer {
        public byte[] mData;
        public int mFrameSize;
        public long mTimeStamp;

        public VideoBuffer() {
            mData = null;
            mFrameSize = 0;
            mTimeStamp = 0;
        }
    }

    @Override
    public void onPreviewFrame(byte[] data, int timestamp, int isEOF) {
        Log.d(TAG, "----onPreviewFrame");
        synchronized (mLock) {
            if (!mIsStreaming) {
                Log.d(TAG, "----onPreviewframe--end---");
                return;
            }
        }
        if (isEOF == RemoteCamera.CAMERA_STREAM_EOF) {
            synchronized (mLock) {
                mThreadSelfExit = true;
            }
            return;
        }
        if (data == null) {
            Log.w(TAG, "Error: receive null frame.");
            return;
        }

        VideoBuffer buf = new VideoBuffer();
        buf.mData = new byte[data.length];
        System.arraycopy(data, 0, buf.mData, 0, data.length);
        buf.mFrameSize = data.length;
        buf.mTimeStamp = timestamp;

        Log.d(TAG, "videoBuffer info: size: " + buf.mFrameSize +
                ", timestap: " + buf.mTimeStamp);

        synchronized (mDataLock) {
            mVideoQueue.add(buf);
            mDataLock.notify();
        }
    }

    private class PlayerMainThread extends Thread {

        public PlayerMainThread() {

        }

        @Override
        public void run() {
            super.run();

            Log.d(TAG, "-----PlayerMainThread---start----");
            if (mDecoderThread == null) {
                mDecoderThread = new DecoderThread(mSurfaceHolder);
            }
//			mDecoderThread = new DecoderThread(mSurfaceHolder);

            if (mRemoteCameraParams == null) {
                mRemoteCameraParams = mRemoteCamera.getParameters();
            }

            Log.d(TAG, "run: initdecoder!");
            RemoteCamera.Size previewSize = mRemoteCameraParams.getPreviewSize();
            android.util.Log.d(TAG, "run: width:" + previewSize.width + " height:" + previewSize.height);
//			if (mDecoderThread == null) {
            if (!mDecoderThread.initDecoder(previewSize.width, previewSize.height)) {
                // send message.
                Log.e(TAG, "Error: init video decoder failed.");
                return;
            }
//			}

            synchronized (mLock) {
                mIsStreaming = true;
            }

            Log.d(TAG, "run: startstream!");
            int ret = mRemoteCamera.startStream();
            if (ret != 0) {
                Log.e(TAG, "Error: start remote camera stream failed.");
                synchronized (mLock) {
                    mIsStreaming = false;
                }
                mDecoderThread.releaseDecoder();
                mDecoderThread = null;
                return;
            }

            Log.d(TAG, "run: startdecoder!");
            if (mDecoderThread.isAlive() == false) {
                mDecoderThread.start();
            }

            try {
                mDecoderThread.join();
            } catch (Exception e) {
                e.printStackTrace();
            }
            synchronized (mLock) {
                mIsStreaming = false;
            }
            Log.d(TAG, "-----PlayerMainThread---stopstream----");
            mRemoteCamera.stopStream();
            mDecoderThread.releaseDecoder();
            mDecoderThread = null;
            Log.d(TAG, "-----PlayerMainThread---end----");
        }
    }

    private class DecoderThread extends Thread {

        private boolean mIsCacheOK = false;
        private final static String MIME_TYPE = "video/avc";
        private MediaCodec mDecoder = null;
        private boolean mDecoderInited = false;
        private SurfaceHolder mSurfaceHolder = null;
        private int mGetFailedCount = 0;

        public DecoderThread(SurfaceHolder holder) {
            mSurfaceHolder = holder;
        }

        public boolean initDecoder(int width, int height) {

            if (decoder_init_flag == true) {
                return true;
            }

            try {
                android.util.Log.d(TAG, "initDecoder: create decoder type");
                mDecoder = MediaCodec.createDecoderByType(MIME_TYPE);
            } catch (IOException e) {
                android.util.Log.d(TAG, "initDecoder: failed!");
                e.printStackTrace();
                return false;
            }

            android.util.Log.d(TAG, "initDecoder: createvideoformat");
            MediaFormat format = MediaFormat.createVideoFormat(MIME_TYPE, width, height);
            android.util.Log.d(TAG, "initDecoder: configure");
            mDecoder.configure(format, mSurfaceHolder.getSurface(), null, 0);
            if (mDecoder == null) {
                Log.e(TAG, "Error: Can't find video info!");
                return false;
            }
            android.util.Log.d(TAG, "initDecoder: start");
            mDecoder.start();

            decoder_init_flag = true;
            return true;
        }

        public void releaseDecoder() {
            mDecoder.stop();
            mDecoder.release();
        }

        @Override
        public void run() {
            super.run();

            int sleepTime = 1000 / 30 - 5;

            for (; ; ) {
                synchronized (mLock) {
                    if (mThreadExited || mThreadSelfExit) {
                        Log.d(TAG, "-1-mThreadExited " + mThreadExited + ", mThreadSelfExit " + mThreadSelfExit);
                        break;
                    }
                }
                if (!mIsCacheOK) {
                    if (mVideoQueue.size() < CACHE_BUF_COUNT) {
                        try {
                            Thread.sleep(60);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        continue;
                    } else {
                        mIsCacheOK = true;
                        // mHandler.sendEmptyMessage(IOTC_OK_GETDATA);
                    }
                }
                if (mGetFailedCount > 10000) {
                    synchronized (mLock) {
                        mThreadSelfExit = true;
                    }
                    Log.e(TAG, "Has not received preview frame data.");
                    // mHandler.sendEmptyMessage(IOTC_ERR_GETDATA_TIMEOUT);
                    break;
                }

                VideoBuffer buf;
                synchronized (mLock) {
                    if (mVideoQueue.isEmpty()) {
                        try {
                            mLock.wait(GET_BUF_TIMEOUT);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                    if (mVideoQueue.isEmpty()) {
                        mGetFailedCount++;
                        continue;
                    } else {
                        mGetFailedCount = 0;
                        buf = mVideoQueue.get(0);
                        mVideoQueue.remove(0);
                    }
                }
                // Log.d(TAG, "Java videoBuffer info: cap: " + mVideoQueue.getBufferCapacity() +
                //       ", size: " + buf.mFrameSize +
                //       ", timestap: " + buf.mTimeStamp);

                int inputBufferIdx = mDecoder.dequeueInputBuffer(100);
                Log.d(TAG, "input buffer idx: " + inputBufferIdx);
                if (inputBufferIdx >= 0) {
                    ByteBuffer inputBuffer = mDecoder.getInputBuffer(inputBufferIdx);
                    inputBuffer.put(buf.mData, 0, buf.mFrameSize);
                    mDecoder.queueInputBuffer(inputBufferIdx, 0, buf.mFrameSize, buf.mTimeStamp, 0);
                }

                android.util.Log.d(TAG, "run:mDecoder.dequeueOutputBuffer");
                MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
                int outputBufferId = mDecoder.dequeueOutputBuffer(bufferInfo, 100);
                android.util.Log.d(TAG, "run: outputbufferid is " + outputBufferId);
                if (outputBufferId >= 0) {
                    mDecoder.releaseOutputBuffer(outputBufferId, true);
                } else if (outputBufferId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    Log.d(TAG, "outputBufferId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED");
                }

                try {
                    Thread.sleep(sleepTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    ;
}
