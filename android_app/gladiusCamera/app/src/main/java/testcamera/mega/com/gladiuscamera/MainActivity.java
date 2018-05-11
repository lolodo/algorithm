package testcamera.mega.com.gladiuscamera;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.RadioGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback, View.OnClickListener, VirtualCamera.CameraDeviceCallback {

    private static final String TAG = "MegaCamera";
    private static final boolean VERBOSE = true;

    private static final int UPDATE_RECORD_TIME = 100;

    private VirtualCamera mVirtualCamera = VirtualCamera.getInstance(this);
    private String mWorkingCamName = VirtualCamera.CAMERA_AVM_FRONT;

    private boolean mSurfaceChanged = false;
    private Button mLeftBtn;
    private Button mRightBtn;
    private RadioGroup mRGLeft;
    private RadioGroup mRGRight;
    private RelativeLayout mPhotoBar;
    private RelativeLayout mVideoBar;
    private TextView mRecordTimeView;
    private View mPhotoBtn;
    private View mVideoBtn;
    private boolean mVideoBtnEnabled = false;

    private long mRecordingStartTime;

    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case UPDATE_RECORD_TIME:
                    updateRecordingTime();
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if (VERBOSE) {
            Log.e(TAG, "--onCreate--");
        }
        initView();
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, 1);
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 1);
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, 1);
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.INTERNET) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.INTERNET}, 1);
        }

        SurfaceView surface = (SurfaceView) findViewById(R.id.idPreview);
        RelativeLayout.LayoutParams lp =
                (RelativeLayout.LayoutParams) surface.getLayoutParams();
        lp.height = 800;
        lp.width = 1280;
        surface.setLayoutParams(lp);

        SurfaceHolder holder = surface.getHolder();
//        holder.setFixedSize(1280, 800);
        holder.addCallback(this);
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        mVirtualCamera.registerContext(this);
        mVirtualCamera.registerContentResolver(getContentResolver());
    }

    private void initView() {
        mLeftBtn = (Button) findViewById(R.id.btLeft);
        mRightBtn = (Button) findViewById(R.id.btRight);

        mRGLeft = (RadioGroup) findViewById(R.id.rgLeft);
        mRGRight = (RadioGroup) findViewById(R.id.rgRight);

        mRecordTimeView = (TextView) findViewById(R.id.idRecordTime);

        mRGLeft.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                switch (checkedId) {
                    case R.id.item_picture:
                        Log.e(TAG, "----Picture---");
                        if (mVideoBtnEnabled) {
                            stopRecord(mWorkingCamName);
                            mVideoBtnEnabled = false;
                        }
                        mPhotoBar.setVisibility(View.VISIBLE);
                        mVideoBar.setVisibility(View.GONE);
                        mRecordTimeView.setVisibility(View.GONE);
                        break;
                    case R.id.item_video:
                        Log.e(TAG, "----Video----");
                        mPhotoBar.setVisibility(View.GONE);
                        mVideoBar.setVisibility(View.VISIBLE);
                        mVideoBtn.setBackgroundResource(R.drawable.video_icon);
                        mVideoBtnEnabled = false;
                        break;
                }
                mLeftBtn.setBackgroundResource(R.mipmap.item_list);
                mRGLeft.setVisibility(View.GONE);
            }
        });

        mRGRight.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                int ret = -1;
                Log.e(TAG, "onCheckedChanged: " + checkedId);
                switch (checkedId) {
                    case R.id.item_DVR:
                        mWorkingCamName = VirtualCamera.CAMERA_DVR;
                        break;
                    case R.id.item_ROOF:
                        mWorkingCamName = VirtualCamera.CAMERA_ROOF;
                        break;
                    case R.id.item_DVR_ROOF:
                        mWorkingCamName = VirtualCamera.CAMERA_DVR_ROOF;
                        break;
                    case R.id.item_AVM_SYS:
                        mWorkingCamName = VirtualCamera.CAMERA_AVM_SYNTHESIS;
                        break;
                    case R.id.item_AVM_FRONT:
                        mWorkingCamName = VirtualCamera.CAMERA_AVM_FRONT;
                        break;
                    case R.id.item_AVM_BACK:
                        mWorkingCamName = VirtualCamera.CAMERA_AVM_BACK;
                        break;
                    case R.id.item_AVM_LEFT:
                        mWorkingCamName = VirtualCamera.CAMERA_AVM_LEFT;
                        break;
                    case R.id.item_AVM_RIGHT:
                        mWorkingCamName = VirtualCamera.CAMERA_AVM_RIGHT;
                        break;
                    case R.id.item_DRIVER:
                        mWorkingCamName = VirtualCamera.CAMERA_DRIVER;
                        break;
                }
                Log.e(TAG, "Current work camera name: " + mWorkingCamName);
                ret = mVirtualCamera.initDevice(mWorkingCamName);
                if (ret != 0) {
                    Log.e(TAG, "Error: init device " + mWorkingCamName + " failed.");
                }
                if (!mSurfaceChanged) {
                    Log.e(TAG, "Error: Surface has not changed.");
                    return;
                }
                Log.d(TAG, "----1-----startStream----: " + mWorkingCamName);
                ret = mVirtualCamera.startStream(mWorkingCamName);
                if (ret != 0) {
                    Log.e(TAG, "Error: start stream " + mWorkingCamName + " failed.");
                }
                mRGRight.setVisibility(View.GONE);
                mRightBtn.setBackgroundResource(R.mipmap.item_list);
            }
        });

        mPhotoBar = (RelativeLayout) findViewById(R.id.idPhoto);
        mVideoBar = (RelativeLayout) findViewById(R.id.idVideo);

        mPhotoBtn = findViewById(R.id.photoButton);
        mVideoBtn = findViewById(R.id.videoButton);

        mLeftBtn.setOnClickListener(this);
        mRightBtn.setOnClickListener(this);

        mRGLeft.setOnClickListener(this);
        mRGRight.setOnClickListener(this);

        mPhotoBtn.setOnClickListener(this);
        mVideoBtn.setOnClickListener(this);
    }

    @Override
    protected void onStart() {
        super.onStart();

        if (VERBOSE) {
            Log.e(TAG, "--onStart--");
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (VERBOSE) {
            Log.e(TAG, "--onResume--");
        }

        mVirtualCamera.initDevice(mWorkingCamName);
        if (!mSurfaceChanged) {
            Log.e(TAG, "Error: Surface has not changed.");
            return;
        }
        Log.d(TAG, "----2-----startStream----: " + mWorkingCamName);
        mVirtualCamera.startStream(mWorkingCamName);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (VERBOSE) {
            Log.e(TAG, "--onPause--");
        }
        mVirtualCamera.stopStream(mWorkingCamName);
        mVirtualCamera.releaseDevice(mWorkingCamName);
    }

    @Override
    protected void onStop() {
        super.onStop();
        if (VERBOSE) {
            Log.e(TAG, "--onStop--");
        }
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        if (VERBOSE) {
            Log.e(TAG, "--onRestart--");
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (VERBOSE) {
            Log.e(TAG, "--onDestroy--");
        }
    }


    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btLeft:
                if (mRGLeft.getVisibility() != View.VISIBLE) {
                    mRGLeft.setVisibility(View.VISIBLE);
                    mLeftBtn.setBackgroundResource(R.mipmap.left_arrow);
                } else {
                    mRGLeft.setVisibility(View.GONE);
                    mLeftBtn.setBackgroundResource(R.mipmap.item_list);
                }
                break;
            case R.id.btRight:
                if (mRGRight.getVisibility() != View.VISIBLE) {
                    mRGRight.setVisibility(View.VISIBLE);
                    mRightBtn.setBackgroundResource(R.mipmap.right_arrow);
                } else {
                    mRGRight.setVisibility(View.GONE);
                    mRightBtn.setBackgroundResource(R.mipmap.item_list);
                }
                break;
            case R.id.photoButton:
                if (mPhotoBtn.isPressed()) {
                    Log.e(TAG, "---photo button is pressed.---");
                    mVirtualCamera.takePicture(mWorkingCamName);
                } else {
                    Log.e(TAG, "---photo button is not pressed.---");
                }
                break;
            case R.id.videoButton:
                if (!mVideoBtnEnabled) {
                    int ret = startRecord(mWorkingCamName);
                    if (ret != 0) {
                        mVideoBtnEnabled = false;
                    }
                    mVideoBtnEnabled = true;
                    updateRecordingTime();
                } else {
                    stopRecord(mWorkingCamName);
                    mVideoBtnEnabled = false;
                }

                break;
        }
    }

    @Override
    public void onCameraDeviceStatusChanged(String camName, int event) {
        if (event == VirtualCamera.EVENT_STOP_RECORD) {
            if (mVideoBtnEnabled) {
                stopRecord(camName);
                mVideoBtnEnabled = false;
            }
        } else if (event == VirtualCamera.EVENT_STOP_STREAM) {
            mVirtualCamera.stopStream(camName);
        } else if (event == VirtualCamera.EVENT_CLOSE_DEVICE) {
            mVirtualCamera.releaseDevice(camName);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        Log.e(TAG, "---surfaceCreated");
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
        Log.e(TAG, "---surfaceChanged");
        if (surfaceHolder.getSurface() == null) {
            Log.e(TAG, "surfaceHolder.getSurface() == null");
            return;
        }
        mSurfaceChanged = true;
        try {
            mVirtualCamera.registerSurface(surfaceHolder);
        } catch (IOException e) {
            e.printStackTrace();
        }
        Log.d(TAG, "----3-----startStream----: " + mWorkingCamName);
        mVirtualCamera.startStream(mWorkingCamName);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        Log.e(TAG, "---surfaceDestroyed");
        mSurfaceChanged = false;
    }

    private int startRecord(String camName) {
        int ret = mVirtualCamera.startRecord(camName);
        if (ret != 0) {
            Log.e(TAG, "Error: start record failed.");
            Toast.makeText(this, "录像失败！", Toast.LENGTH_LONG).show();
            return -1;
        }
        mVideoBtn.setBackgroundResource(R.drawable.video_icon_pressed);
        mRecordTimeView.setVisibility(View.VISIBLE);
        mRecordingStartTime = SystemClock.uptimeMillis();
        return 0;
    }

    private void stopRecord(String camName) {
        mVideoBtn.setBackgroundResource(R.drawable.video_icon);
        mRecordTimeView.setVisibility(View.GONE);
        mVirtualCamera.stopRecord(camName);
        Toast.makeText(this, "录像文件保存成功，请在相册中查看。", Toast.LENGTH_LONG).show();
    }

    private void updateRecordingTime() {
        if (!mVideoBtnEnabled) {
            return;
        }
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                long now = SystemClock.uptimeMillis();
                long delta = now - mRecordingStartTime;
                long deltaAdjusted = delta;
                long targetNextUpdateDelay = 1000;
                String text = millisecondToTimeString(deltaAdjusted, false);
                Log.e(TAG, "RecordingTime: " + text);
                mRecordTimeView.setText(text);

                long actualNextUpdateDelay = targetNextUpdateDelay
                        - (delta % targetNextUpdateDelay);
                mHandler.sendEmptyMessageDelayed(UPDATE_RECORD_TIME,
                        actualNextUpdateDelay);
            }
        });
    }

    private String millisecondToTimeString(long milliSeconds, boolean displayCentiSeconds) {
        long seconds = milliSeconds / 1000; // round down to compute seconds
        long minutes = seconds / 60;
        long hours = minutes / 60;
        long remainderMinutes = minutes - (hours * 60);
        long remainderSeconds = seconds - (minutes * 60);

        StringBuilder timeStringBuilder = new StringBuilder();

        // Hours
        if (hours < 10) {
            timeStringBuilder.append('0');
        }
        timeStringBuilder.append(hours);
        timeStringBuilder.append(':');

        // Minutes
        if (remainderMinutes < 10) {
            timeStringBuilder.append('0');
        }
        timeStringBuilder.append(remainderMinutes);
        timeStringBuilder.append(':');

        // Seconds
        if (remainderSeconds < 10) {
            timeStringBuilder.append('0');
        }
        timeStringBuilder.append(remainderSeconds);

        // Centi seconds
        if (displayCentiSeconds) {
            timeStringBuilder.append('.');
            long remainderCentiSeconds = (milliSeconds - seconds * 1000) / 10;
            if (remainderCentiSeconds < 10) {
                timeStringBuilder.append('0');
            }
            timeStringBuilder.append(remainderCentiSeconds);
        }

        return timeStringBuilder.toString();
    }
}
