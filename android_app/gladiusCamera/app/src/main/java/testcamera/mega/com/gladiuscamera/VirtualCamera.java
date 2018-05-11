package testcamera.mega.com.gladiuscamera;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.hardware.Camera;
import android.media.CamcorderProfile;
import android.media.MediaRecorder;
import android.net.Uri;
import android.os.Environment;
import android.os.StatFs;
import android.os.SystemClock;
import android.provider.MediaStore;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Handler;

import mega.net.RemoteCamera;

public class VirtualCamera {
    private static final String TAG = "VirtualCamera";
    private static final boolean VERBOSE = true;

    public static final String CAMERA_DVR = "camera-dvr";
    public static final String CAMERA_ROOF = "camera-roof";
    public static final String CAMERA_DVR_ROOF = "camera-dvr-roof";
    public static final String CAMERA_AVM_SYNTHESIS = "camera-avm-synthesis";
    public static final String CAMERA_AVM_FRONT = "camera-avm-front";
    public static final String CAMERA_AVM_BACK = "camera-avm-back";
    public static final String CAMERA_AVM_LEFT = "camera-avm-left";
    public static final String CAMERA_AVM_RIGHT = "camera-avm-right";
    public static final String CAMERA_DRIVER = "camera-driver";

    public static final int EVENT_STOP_STREAM = 11;
    public static final int EVENT_STOP_RECORD = 12;
    public static final int EVENT_CLOSE_DEVICE = 13;

    /* Remote Camera */
    private static final String SERVER_IP   = "172.20.1.11";
    private static final int    SERVER_PORT = 37568;
    private static final String CLIENT_IP   = "172.20.1.10";
    private static final int    CLIENT_PORT = 8554;

    private RemoteCameraPlayer mRemoteCameraPlayer = null;
    private boolean mRemoteConnected = false;

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    private static final int CAMERA_DVR_INDEX = 0;
    private static final int CAMERA_ROOF_INDEX = 1;
    private static final int CAMERA_DVR_ROOF_INDEX = 2;
    private static final int CAMERA_AVM_SYNTHESIS_INDEX = 3;
    private static final int CAMERA_AVM_FRONT_INDEX = 4;
    private static final int CAMERA_AVM_BACK_INDEX = 5;
    private static final int CAMERA_AVM_LEFT_INDEX = 6;
    private static final int CAMERA_AVM_RIGHT_INDEX = 7;
    private static final int CAMERA_DRIVER_INDEX = 8;

    private static final int MAX_CAM_DEV_COUNT = 9;
    private static final int MAX_CAM_DEV_WORK_COUNT = 2;

    private static final int DEV_CLOSED = 100;
    private static final int DEV_OPENED = 101;
    private static final int DEV_STREAMING = 102;
    private static final int DEV_RECORDING = 103;

    private static final int PREVIEW_WIDTH = 1280;
    private static final int PREVIEW_HEIGHT_LOW = 800;
    private static final int PREVIEW_HEIGHT_HIGH = 1600;

    private static final String BASE_DIR = Environment
            .getExternalStorageDirectory().toString();
    private final String PIC_DIRECTORY = BASE_DIR + "/DCIM/Camera";
    private final String VID_DIRECTORY = BASE_DIR + "/DCIM/Camera";
    private final long RESERVE_EXTERNAL_STORAGE = 100000000; // 100M

    private Object mLock = new Object();
    private Map<String, Integer> mNameIndexMap = new HashMap<>();
    private int[] mDevStatus = new int[MAX_CAM_DEV_COUNT];
    private CameraDeviceCallback mCameraDeviceCallback;
    private Handler mHandler;
    private SurfaceHolder mSurfaceHolder;

    private Camera mCameraDevice;
    private Camera.Parameters mCameraParameters;
    private MediaRecorder mMediaRecorder;
    private long mRecordingStartTime;
    private CamcorderProfile mCamcorderProfile;
    private String mVideoFileName;
    private ContentValues mCurrentVideoValues;

    private Context mContext;
    private ContentResolver mContentResolver;

    private String[] mWorkingCamName = new String[MAX_CAM_DEV_WORK_COUNT];
    private boolean[] mRecordingHint = new boolean[MAX_CAM_DEV_WORK_COUNT];

    private static volatile VirtualCamera singleton = null;

    public static VirtualCamera getInstance(CameraDeviceCallback cb) {
        if (singleton == null) {
            synchronized (VirtualCamera.class) {
                singleton = new VirtualCamera(cb);
            }
        }
        return singleton;
    }

    private VirtualCamera(CameraDeviceCallback cb) {
        mNameIndexMap.clear();
        mNameIndexMap.put(CAMERA_DVR, CAMERA_DVR_INDEX);
        mNameIndexMap.put(CAMERA_ROOF, CAMERA_ROOF_INDEX);
        mNameIndexMap.put(CAMERA_DVR_ROOF, CAMERA_DVR_ROOF_INDEX);
        mNameIndexMap.put(CAMERA_AVM_SYNTHESIS, CAMERA_AVM_SYNTHESIS_INDEX);
        mNameIndexMap.put(CAMERA_AVM_FRONT, CAMERA_AVM_FRONT_INDEX);
        mNameIndexMap.put(CAMERA_AVM_BACK, CAMERA_AVM_BACK_INDEX);
        mNameIndexMap.put(CAMERA_AVM_LEFT, CAMERA_AVM_LEFT_INDEX);
        mNameIndexMap.put(CAMERA_AVM_RIGHT, CAMERA_AVM_RIGHT_INDEX);
        mNameIndexMap.put(CAMERA_DRIVER, CAMERA_DRIVER_INDEX);

        mCameraDeviceCallback = cb;

        for (int i = 0; i < mDevStatus.length; i++) {
            mDevStatus[i] = DEV_CLOSED;
        }
    }

    public interface CameraDeviceCallback {
        void onCameraDeviceStatusChanged(String camName, int event);
    }

    public void registerContext(Context context) {
        mContext = context;
    }

    public void registerContentResolver(ContentResolver solver) {
        mContentResolver = solver;
    }

    public void registerHandler(Handler handler) {
        mHandler = handler;
    }

    public void unregisterHandler() {
        mHandler = null;
    }

    public void registerSurface(SurfaceHolder holder) throws IOException {
        mSurfaceHolder = holder;
        if (mCameraDevice != null) {
            Log.d(TAG, "---1---setPreviewDisplay----");
            mCameraDevice.setPreviewDisplay(holder);
        }
    }

    public int initDevice(String camName) {

        int cameraIndex = getCamIndexFromCamName(camName);
        if (cameraIndex < 0) {
            Log.e(TAG, "Error: Unknow camera name " + camName);
            return -1;
        }
        if (getCamDevStatus(cameraIndex) == DEV_OPENED) {
            Log.w(TAG, "Warning: camera " + camName + " has opened.");
            return 0;
        }
        if (CAMERA_DVR_INDEX <= cameraIndex && cameraIndex <= CAMERA_DVR_ROOF_INDEX) {
            for (int i = CAMERA_DVR_INDEX; i <= CAMERA_DVR_ROOF_INDEX; i++) {
                if (mDevStatus[i] != DEV_CLOSED) {
                    if (mDevStatus[i] == DEV_RECORDING) {
                        mCameraDeviceCallback.onCameraDeviceStatusChanged(getCamNameFromCamIndex(i), EVENT_STOP_RECORD);
                    }
                    mCameraDeviceCallback.onCameraDeviceStatusChanged(getCamNameFromCamIndex(i), EVENT_STOP_STREAM);
                    mCameraDeviceCallback.onCameraDeviceStatusChanged(getCamNameFromCamIndex(i), EVENT_CLOSE_DEVICE);
                }
            }
            mCameraDevice = Camera.open(cameraIndex);

            mCameraDevice.setErrorCallback(new Camera.ErrorCallback() {
                @Override
                public void onError(int error, Camera camera) {

                }
            });
            mCameraParameters = mCameraDevice.getParameters();
            List<Camera.Size> supportedPreviewSizes = mCameraParameters.getSupportedPreviewSizes();
            for (Camera.Size s:supportedPreviewSizes) {
                Log.e(TAG, "SupportedPreviewSizes: " + s.width + "x" + s.height);
            }
            List<Camera.Size> supportedPictureSizes = mCameraParameters.getSupportedPictureSizes();
            for (Camera.Size s : supportedPictureSizes) {
                Log.e(TAG, "SupportPictureSizes: " + s.width + "x" + s.height);
            }
        } else {
            switch (camName) {
            case CAMERA_AVM_SYNTHESIS:
                mRemoteCameraPlayer =
                    RemoteCameraPlayer.getInstance(RemoteCamera.EVT_VERSION_2_0, RemoteCamera.REMOTE_CAMERA_AVM_SYNTHESIS);
                mRemoteCameraPlayer.setCameraName(RemoteCamera.REMOTE_CAMERA_AVM_SYNTHESIS);
                break;
            case CAMERA_AVM_FRONT:
                mRemoteCameraPlayer =
                    RemoteCameraPlayer.getInstance(RemoteCamera.EVT_VERSION_2_0, RemoteCamera.REMOTE_CAMERA_AVM_FRONT);
                mRemoteCameraPlayer.setCameraName(RemoteCamera.REMOTE_CAMERA_AVM_FRONT);
                break;

            case CAMERA_AVM_BACK:
                mRemoteCameraPlayer =
                    RemoteCameraPlayer.getInstance(RemoteCamera.EVT_VERSION_2_0, RemoteCamera.REMOTE_CAMERA_AVM_BACK);
                mRemoteCameraPlayer.setCameraName(RemoteCamera.REMOTE_CAMERA_AVM_BACK);
                break;

            case CAMERA_AVM_LEFT:
                mRemoteCameraPlayer =
                    RemoteCameraPlayer.getInstance(RemoteCamera.EVT_VERSION_2_0, RemoteCamera.REMOTE_CAMERA_AVM_LEFT);
                mRemoteCameraPlayer.setCameraName(RemoteCamera.REMOTE_CAMERA_AVM_LEFT);
                break;

            case CAMERA_AVM_RIGHT:
                mRemoteCameraPlayer =
                    RemoteCameraPlayer.getInstance(RemoteCamera.EVT_VERSION_2_0, RemoteCamera.REMOTE_CAMERA_AVM_RIGHT);
                mRemoteCameraPlayer.setCameraName(RemoteCamera.REMOTE_CAMERA_AVM_RIGHT);
                break;

            case CAMERA_DRIVER:
                mRemoteCameraPlayer =
                    RemoteCameraPlayer.getInstance(RemoteCamera.EVT_VERSION_2_0, RemoteCamera.REMOTE_CAMERA_DRIVER);
                mRemoteCameraPlayer.setCameraName(RemoteCamera.REMOTE_CAMERA_DRIVER);
                break;

            }
            if (mSurfaceHolder != null) {
                mRemoteCameraPlayer.registerSurface(mSurfaceHolder);
            } else {
                Log.e(TAG, "Error: mSurfaceHolder is null.");
            }
            Log.d(TAG, "---1---initDevice------");
            int ret = mRemoteCameraPlayer.connect(SERVER_IP, SERVER_PORT, CLIENT_PORT);
            Log.d(TAG, "---11---initDevice------");
            if (ret != 0) {
                Log.e(TAG, "Error: Connect remote camera failed.");
                mRemoteConnected = false;
            } else {
                mRemoteConnected = true;
                Log.d(TAG, "---2---initDevice------");
                mRemoteCameraPlayer.configure(RemoteCamera.KEY_PREVIEW_FORMAT_H264, 30,
                                              PREVIEW_WIDTH, PREVIEW_HEIGHT_LOW, CLIENT_IP, CLIENT_PORT);
                Log.d(TAG, "---22---initDevice------");
            }
        }

        setCamDevStatus(cameraIndex, DEV_OPENED);

        return 0;
    }

    public void releaseDevice(String camName) {

        int cameraIndex = getCamIndexFromCamName(camName);
        if (cameraIndex < 0) {
            Log.e(TAG, "Error: Unknow camera name " + camName);
            return;
        }

        if (getCamDevStatus(cameraIndex) == DEV_RECORDING) {
            mCameraDeviceCallback.onCameraDeviceStatusChanged(camName, EVENT_STOP_RECORD);
        }
        if (getCamDevStatus(cameraIndex) == DEV_STREAMING) {
            mCameraDeviceCallback.onCameraDeviceStatusChanged(camName, EVENT_STOP_STREAM);
        }
        if (CAMERA_DVR_INDEX <= cameraIndex && cameraIndex <= CAMERA_DVR_ROOF_INDEX) {
            if (mCameraDevice != null) {
                mCameraDevice.stopPreview();
                mCameraDevice.setErrorCallback(null);
                mCameraDevice.release();
                mCameraDevice = null;
            }
        } else {
            if (mRemoteConnected) {
                mRemoteCameraPlayer.disconnect();
            }
        }

        setCamDevStatus(cameraIndex, DEV_CLOSED);
    }

    public void setRecordingHing(String camName, boolean hint) {
        int cameraIndex = getCamIndexFromCamName(camName);
        if (cameraIndex < 0) {
            Log.e(TAG, "Error: Unknow camera name " + camName);
            return;
        }
        if (getCamDevStatus(cameraIndex) == DEV_RECORDING) {
            Log.w(TAG, "Warning: camera is recording, can't set recording hint.");
            return;
        }
        if (CAMERA_DVR_INDEX <= cameraIndex && cameraIndex <= CAMERA_DVR_ROOF_INDEX) {
            synchronized (mLock) {
                mRecordingHint[0] = hint;
            }
        } else {
            synchronized (mLock) {
                mRecordingHint[1] = hint;
            }
        }
    }

    public int startStream(String camName) {
        int cameraIndex = getCamIndexFromCamName(camName);
        if (cameraIndex < 0) {
            Log.e(TAG, "Error: Unknow camera name " + camName);
            return -1;
        }
        if (getCamDevStatus(cameraIndex) == DEV_STREAMING ||
                getCamDevStatus(cameraIndex) == DEV_RECORDING) {
            Log.w(TAG, "Warning: camera " + camName + " has streaming..");
            return 0;
        }
//        if (getCamDevStatus(cameraIndex) == DEV_CLOSED || mCameraDevice == null) {
//            Log.e(TAG, "Error: start camera " + " stream failed because of has not init.");
//            return -1;
//        }
        if (getCamDevStatus(cameraIndex) == DEV_OPENED) {
            if (CAMERA_DVR_INDEX <= cameraIndex && cameraIndex <= CAMERA_DVR_ROOF_INDEX) {
                try {
                    if (mSurfaceHolder != null) {
                        mCameraDevice.setPreviewDisplay(mSurfaceHolder);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
                if (cameraIndex != CAMERA_DVR_ROOF_INDEX) {
                    mCameraParameters.setPreviewSize(PREVIEW_WIDTH, PREVIEW_HEIGHT_LOW);
                    mCameraParameters.setPictureSize(PREVIEW_WIDTH, PREVIEW_HEIGHT_LOW);
                } else {
                    mCameraParameters.setPreviewSize(PREVIEW_WIDTH, PREVIEW_HEIGHT_HIGH);
                    mCameraParameters.setPictureSize(PREVIEW_WIDTH, PREVIEW_HEIGHT_HIGH);
                }
                mCameraDevice.setParameters(mCameraParameters);
                // mCameraDevice.setDisplayOrientation((90 + 360) % 360);
                Log.d(TAG, "-------startPreview-----");
                mCameraDevice.startPreview();
            } else {
                if (mRemoteConnected) {
                    if (mSurfaceHolder != null) {
                        mRemoteCameraPlayer.registerSurface(mSurfaceHolder);
                    } else {
                        Log.d(TAG, "Error: -2-mSurfaceHolder is null.");
                    }

                    mRemoteCameraPlayer.start();
                }
            }

            setCamDevStatus(cameraIndex, DEV_STREAMING);
        }

        return 0;
    }

    public void stopStream(String camName) {
        int cameraIndex = getCamIndexFromCamName(camName);
        if (cameraIndex < 0) {
            Log.e(TAG, "Error: Unknow camera name " + camName);
            return;
        }
        if (getCamDevStatus(cameraIndex) == DEV_RECORDING) {
            mCameraDeviceCallback.onCameraDeviceStatusChanged(camName, EVENT_STOP_RECORD);
        }
        if (getCamDevStatus(cameraIndex) == DEV_STREAMING) {
            if (CAMERA_DVR_INDEX <= cameraIndex && cameraIndex <= CAMERA_DVR_ROOF_INDEX) {
                mCameraDevice.stopPreview();
            } else {
                if (mRemoteConnected) {
                    mRemoteCameraPlayer.stop();
                }
            }
            setCamDevStatus(cameraIndex, DEV_OPENED);
        }

        return;
    }

    public void takePicture(String camName) {
        int cameraIndex = getCamIndexFromCamName(camName);
        if (cameraIndex < 0) {
            Log.e(TAG, "Error: Unknow camera name " + camName);
            return;
        }
        if (mDevStatus[cameraIndex] != DEV_STREAMING) {
            Log.w(TAG, "Warning: camera " + camName + " has not streaming, can't take picture.");
            return;
        }

        if (CAMERA_DVR_INDEX <= cameraIndex && cameraIndex <= CAMERA_DVR_ROOF_INDEX) {
            mCameraDevice.takePicture(null, null, null, new Camera.PictureCallback() {
                @Override
                public void onPictureTaken(byte[] data, Camera camera) {
                    Log.e(TAG, "------------onPictureTaken");
                    mCameraDevice.startPreview();
                    if (getSDCardAvailableSize() <= RESERVE_EXTERNAL_STORAGE) {
                        Log.e(TAG, "Error: onPictureTaken: reserve external storage " + getSDCardAvailableSize() + " is so low.");
                        if (mContext != null) {
                            Toast.makeText(mContext, "磁盘空间已满", Toast.LENGTH_LONG).show();
                        }
                        return;
                    }
                    if (mContext != null) {
                        Toast.makeText(mContext, "拍照成功，请在相册中查看。", Toast.LENGTH_LONG).show();
                    }
                    storeImage(data);
                }
            });
        } else {

        }
    }

    public int startRecord(String camName) {
        int cameraIndex = getCamIndexFromCamName(camName);
        if (cameraIndex < 0) {
            Log.e(TAG, "Error: Unknow camera name " + camName);
            return -1;
        }
        if (getCamDevStatus(cameraIndex) == DEV_RECORDING) {
            Log.w(TAG, "Warning: Recording has enabled.");
            return 0;
        }
        if (getCamDevStatus(cameraIndex) == DEV_CLOSED) {
            Log.e(TAG, "Error: Camera device has closed, can't start record.");
            return -1;
        }
        if (getCamDevStatus(cameraIndex) == DEV_OPENED) {
            Log.w(TAG, "Warning: Camera device has opened, but isn't streaming.");
            return -1;
        }
        if (CAMERA_DVR_INDEX <= cameraIndex && cameraIndex <= CAMERA_DVR_ROOF_INDEX) {
            int ret = initVideoRecorder(camName);
            if (ret != 0) {
                Log.e(TAG, "Error: init MediaRecorder failed.");
                return ret;
            }
            mMediaRecorder.start();
            mRecordingStartTime = SystemClock.uptimeMillis();
        } else {

        }

        setCamDevStatus(cameraIndex, DEV_RECORDING);
        return 0;
    }

    public void stopRecord(String camName) {
        int cameraIndex = getCamIndexFromCamName(camName);
        if (cameraIndex < 0) {
            Log.e(TAG, "Error: Unknow camera name " + camName);
            return;
        }
        if (getCamDevStatus(cameraIndex) == DEV_RECORDING) {
            if (CAMERA_DVR_INDEX <= cameraIndex && cameraIndex <= CAMERA_DVR_ROOF_INDEX) {
                mMediaRecorder.setOnErrorListener(null);
                mMediaRecorder.setOnInfoListener(null);
                mMediaRecorder.stop();
                mMediaRecorder.release();
                mMediaRecorder = null;

                addVideoToMediaStore();
            } else {

            }
            setCamDevStatus(cameraIndex, DEV_STREAMING);
        }
    }

    private int getCamIndexFromCamName(String camName) {
        int cameraIndex = -1;
        for (Map.Entry<String, Integer> entry : mNameIndexMap.entrySet()) {
            if (camName.equals(entry.getKey())) {
                cameraIndex = entry.getValue();
                break;
            }
        }
        return cameraIndex;
    }

    private String getCamNameFromCamIndex(int camIdx) {
        String key = null;
        for (String getKey : mNameIndexMap.keySet()) {
            if (mNameIndexMap.get(getKey) == camIdx) {
                key = getKey;
                break;
            }
        }
        return key;
    }

    private int getCamDevStatus(int camIdx) {
        synchronized (mLock) {
            return mDevStatus[camIdx];
        }
    }

    private void setCamDevStatus(int camIdx, int status) {
        synchronized (mLock) {
            mDevStatus[camIdx] = status;
        }
    }

    private void storeImage(final byte[] data) {
        long dateTaken = System.currentTimeMillis();
        String title = generatePictureName(dateTaken);
        Camera.Size s = mCameraDevice.getParameters().getPictureSize();
        Log.e(TAG, "Picture Size: " + s.width + ", " + s.height);
        addImage(title, dateTaken, data, s.width, s.height);
    }

    public void addImage(String title, long date,
                         byte[] jpeg, int width, int height) {

        File dir = new File(PIC_DIRECTORY);
        if (!dir.exists())
            dir.mkdirs();

        String path = PIC_DIRECTORY + '/' + title + ".jpg";
        FileOutputStream out = null;
        try {
            out = new FileOutputStream(path);
            out.write(jpeg);
        } catch (Exception e) {
            Log.e(TAG, "Failed to write image", e);
            return;
        } finally {
            try {
                out.close();
            } catch (Exception e) {
            }
        }

        // Insert into MediaStore.
        ContentValues values = new ContentValues(9);
        values.put(MediaStore.Images.ImageColumns.TITLE, title);
        values.put(MediaStore.Images.ImageColumns.DISPLAY_NAME, title + ".jpg");
        values.put(MediaStore.Images.ImageColumns.DATE_TAKEN, date);
        values.put(MediaStore.Images.ImageColumns.MIME_TYPE, "image/jpeg");
        values.put(MediaStore.Images.ImageColumns.DATA, path);
        values.put(MediaStore.Images.ImageColumns.SIZE, jpeg.length);
        values.put(MediaStore.Images.ImageColumns.WIDTH, width);
        values.put(MediaStore.Images.ImageColumns.HEIGHT, height);
        Uri uri = mContentResolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
        if (uri == null) {
            Log.e(TAG, "Failed to write MediaStore");
            return;
        }
    }

    public String generatePictureName(long dateTaken) {
        Date date = new Date(dateTaken);
        SimpleDateFormat format = new SimpleDateFormat("'IMG'_yyyyMMdd_HHmmss");
        String result = format.format(date);
        return result;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private int initVideoRecorder(String camName) {
        if (mCameraDevice == null) {
            Log.e(TAG, "Error: init video recorder failed of camera device is null.");
            return -1;
        }
        if (mSurfaceHolder == null) {
            Log.e(TAG, "Error: init video recorder failed of surfaceholder is null.");
            return -1;
        }
        mMediaRecorder = new MediaRecorder();
        if (camName.equals(CAMERA_DVR)) {
            mCamcorderProfile = CamcorderProfile.get(0, CamcorderProfile.QUALITY_720P);
        } else if (camName.equals(CAMERA_ROOF)) {
            mCamcorderProfile = CamcorderProfile.get(1, CamcorderProfile.QUALITY_720P);
        }
        mCameraDevice.unlock();
        mMediaRecorder.setCamera(mCameraDevice);
        mMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
        mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.CAMCORDER);
        mMediaRecorder.setProfile(mCamcorderProfile);
        mMediaRecorder.setMaxDuration(0);
        generateVideoFilename(mCamcorderProfile.fileFormat);
        createAndChangVideoFileMode();
        mMediaRecorder.setOutputFile(mVideoFileName);
        mMediaRecorder.setPreviewDisplay(mSurfaceHolder.getSurface());
        mMediaRecorder.setMaxFileSize(getSDCardAvailableSize() - RESERVE_EXTERNAL_STORAGE);
        try {
            mMediaRecorder.prepare();
        } catch (IOException e) {
            e.printStackTrace();
        }
        mMediaRecorder.setOnErrorListener(new MediaRecorder.OnErrorListener() {
            @Override
            public void onError(MediaRecorder mr, int what, int extra) {
                Log.e(TAG, "OnError: MediaRecorder has error: " + what);
            }
        });
        mMediaRecorder.setOnInfoListener(new MediaRecorder.OnInfoListener() {
            @Override
            public void onInfo(MediaRecorder mr, int what, int extra) {
                Log.e(TAG, "Info: MediaRecorder has info: " + what);
            }
        });
        return 0;
    }

    private String convertOutputFormatToMimeType(int outputFileFormat) {
        if (outputFileFormat == MediaRecorder.OutputFormat.MPEG_4) {
            return "video/mp4";
        }
        return "video/3gpp";
    }

    private String convertOutputFormatToFileExt(int outputFileFormat) {
        if (outputFileFormat == MediaRecorder.OutputFormat.MPEG_4) {
            return ".mp4";
        }
        return ".3gp";
    }

    private void generateVideoFilename(int outputFileFormat) {
        long dateTaken = System.currentTimeMillis();
        Date date = new Date(dateTaken);
        SimpleDateFormat dateFormat = new SimpleDateFormat("'VID'_yyyyMMdd_HHmmss");
        String title = dateFormat.format(date);
        // Used when emailing.
        String filename = title
                + convertOutputFormatToFileExt(outputFileFormat);
        String mime = convertOutputFormatToMimeType(outputFileFormat);

        File dir = new File(VID_DIRECTORY);
        if (!dir.exists())
            dir.mkdirs();

        mVideoFileName = VID_DIRECTORY + '/' + filename;
        mCurrentVideoValues = new ContentValues(7);
        mCurrentVideoValues.put(MediaStore.Video.Media.TITLE, title);
        mCurrentVideoValues.put(MediaStore.Video.Media.DISPLAY_NAME, filename);
        mCurrentVideoValues.put(MediaStore.Video.Media.DATE_TAKEN, dateTaken);
        mCurrentVideoValues.put(MediaStore.Video.Media.MIME_TYPE, mime);
        mCurrentVideoValues.put(MediaStore.Video.Media.DATA, mVideoFileName);
        mCurrentVideoValues.put(
                MediaStore.Video.Media.RESOLUTION,
                Integer.toString(mCamcorderProfile.videoFrameWidth) + "x"
                        + Integer.toString(mCamcorderProfile.videoFrameHeight));
        if (VERBOSE) {
            Log.d(TAG, "New video filename: " + mVideoFileName);
        }
    }

    private void createAndChangVideoFileMode() {
        try {
            File videoFile = new File(mVideoFileName);
            if (!videoFile.exists()) {
                videoFile.createNewFile();
                Runtime.getRuntime().exec(
                        "chmod 660 " + videoFile.getAbsolutePath());
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private long getSDCardAvailableSize() {
        File path = Environment.getExternalStorageDirectory();
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSizeLong();
        long availableBlocks = stat.getAvailableBlocksLong();

        Log.e(TAG, "blockSize = " + blockSize + ", availableBlocks = " + availableBlocks);
        Log.e(TAG, "availableSize = " + availableBlocks * blockSize);

        return blockSize * availableBlocks;
    }

    private void addVideoToMediaStore() {
        Uri videoTable = Uri.parse("content://media/external/video/media");
        mCurrentVideoValues.put(MediaStore.Video.Media.SIZE, new File(
                mVideoFileName).length());
        long duration = SystemClock.uptimeMillis() - mRecordingStartTime;
        if (duration > 0) {
            mCurrentVideoValues.put(MediaStore.Video.Media.DURATION, duration);
        } else {
            Log.w(TAG, "Video duration <= 0 : " + duration);
        }
        Uri currentVideoUri = null;
        try {
            currentVideoUri = mContentResolver.insert(videoTable,
                    mCurrentVideoValues);
            mContext.sendBroadcast(new Intent(
                    android.hardware.Camera.ACTION_NEW_VIDEO,
                    currentVideoUri));
        } catch (Exception e) {
            // We failed to insert into the database. This can happen if
            // the SD card is unmounted.
            currentVideoUri = null;
            mVideoFileName = null;
        } finally {
            if (VERBOSE)
                Log.d(TAG, "Current video URI: " + currentVideoUri);
        }
        mCurrentVideoValues = null;
    }

}
