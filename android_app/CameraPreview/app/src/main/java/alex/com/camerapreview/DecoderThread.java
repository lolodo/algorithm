package alex.com.camerapreview;

import android.hardware.Camera;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.ArrayList;

/**
 * Created by fangyuan on 5/11/18.
 */

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

public class DecoderThread extends Thread {
    private static final String TAG = "CameraPreview";
    private boolean mIsCacheOK = false;
    private final static String MIME_TYPE = "video/avc";
    private MediaCodec mDecoder = null;
    private boolean mDecoderInited = false;
    private SurfaceHolder mSurfaceHolder = null;
    private int mGetFailedCount = 0;
    private ArrayList<VideoBuffer> mVideoQueue = new ArrayList<VideoBuffer>();
    private static final int CACHE_BUF_COUNT = 5;
    private static final int GET_BUF_TIMEOUT = 30;
    private Object mLock = new Object();
    private boolean mThreadExited = false;
    private boolean mThreadSelfExit = false;
    private EventHandler mEventHandler;

    private class EventHandler extends Handler {
        public static final int CAMERA_STREAM_EOF = -99;

        private static final int CAMERA_MSG_ERROR = 0x001;
        private static final int CAMERA_MSG_PREVIEW_FRAME = 0x002;
        private static final int CAMERA_MSG_RAW_IMAGE = 0x003;
        private static final int CAMERA_MSG_COMPRESSED_IMAGE = 0x004;
        private static final int CAMERA_MSG_PREVIEW_FRAME_EOF = 0x010;

        public EventHandler(RemoteCamera c, Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case CAMERA_MSG_ERROR:
                    Log.e(TAG, "Error " + msg.arg1);
                    onError(msg.arg1);
                    return;

                case CAMERA_MSG_PREVIEW_FRAME: {
                    onPreviewFrame((byte[]) msg.obj, msg.arg1, msg.arg2);
                    return;
                }

                case CAMERA_MSG_PREVIEW_FRAME_EOF: {
                    onPreviewFrame(null, 0, CAMERA_STREAM_EOF);
                    return;
                }

                case CAMERA_MSG_RAW_IMAGE:
                    onPictureTaken((byte[]) msg.obj);
                    return;

                case CAMERA_MSG_COMPRESSED_IMAGE:
                    onPictureTaken((byte[]) msg.obj);
                    break;

                default:
                    Log.e(TAG, "Unknown message type " + msg.what);
                    return;
            }
        }

        private void onPictureTaken(byte[] obj) {
            Log.d(TAG, "onPictureTaken: take a pic");
        }

        private void onError(int arg1) {
            Log.e(TAG, "onError: arg:" + arg1);
        }

        private void onPreviewFrame(byte[] data, int timestamp, int isEOF) {
            Log.d(TAG, "onPreviewFrame: enter");
            if (isEOF == CAMERA_STREAM_EOF) {
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
            mVideoQueue.add(buf);
        }
    }

    public DecoderThread(SurfaceHolder holder) {
        mSurfaceHolder = holder;
    }

    private void postEventFromNative(Object camera_ref, int what, int arg1, int arg2, Object obj) {
        Message m = mEventHandler.obtainMessage(what, arg1, arg2, obj);
        mEventHandler.sendMessage(m);
    }

    public boolean initDecoder(int width, int height) {

        if (mDecoder != null) {
            releaseDecoder();
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

        return true;
    }

    public void releaseDecoder() {
        mDecoder.stop();
        mDecoder.release();
        mDecoder = null;
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
                }
            }

            if (mGetFailedCount > 10000) {
                synchronized (mLock) {
                    mThreadSelfExit = true;
                }

                Log.e(TAG, "Has not received preview frame data.");
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