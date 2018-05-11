package alex.com.camerapreview;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

import java.io.IOException;

public class MainActivity extends Activity implements SurfaceHolder.Callback, View.OnClickListener {
    private static final String TAG = "CameraPreview";
    private String mCameraName = VirtualCamera.CAMERA_AVM_BACK;
    private VirtualCamera mVirtualCamera = new VirtualCamera();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initView();
        if (PackageManager.PERMISSION_GRANTED != ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, 1);
        }

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
    }

    private void initView() {
        Log.d(TAG, "initView: enter");
        Button frontCamera = (Button) findViewById(R.id.frontCamera);
        Button rearCamera = (Button) findViewById(R.id.rearCamera);
        Button leftCamera = (Button) findViewById(R.id.leftCamera);
        Button rightCamera = (Button) findViewById(R.id.rightCamera);
        Button driverCamera = (Button) findViewById(R.id.dvrCamera);
        Button dvrCamera = (Button) findViewById(R.id.roofCamera);
        Button roofCamera = (Button) findViewById(R.id.avmCamera);
        Button avmCamera = (Button) findViewById(R.id.driverMonitor);
        Button avmRotation = (Button) findViewById(R.id.avm_rotation);
        Button avmDistance = (Button) findViewById(R.id.avm_distance);
        Button avmHeight = (Button) findViewById(R.id.avm_height);
        Button takePic = (Button) findViewById(R.id.takePhoto);
        Button recordVideo = (Button) findViewById(R.id.recorder);
        CheckBox localPreview = (CheckBox) findViewById(R.id.localPreview);

        frontCamera.setOnClickListener(this);
        rearCamera.setOnClickListener(this);
        leftCamera.setOnClickListener(this);
        rightCamera.setOnClickListener(this);
        driverCamera.setOnClickListener(this);
        dvrCamera.setOnClickListener(this);
        roofCamera.setOnClickListener(this);
        avmCamera.setOnClickListener(this);
        avmRotation.setOnClickListener(this);
        avmDistance.setOnClickListener(this);
        avmHeight.setOnClickListener(this);
        takePic.setOnClickListener(this);
        recordVideo.setOnClickListener(this);
        localPreview.setOnClickListener(this);

        SurfaceView surfaceView = (SurfaceView) findViewById(R.id.idPreview);
        SurfaceHolder holder = surfaceView.getHolder();
        holder.addCallback(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onRestart() {
        super.onRestart();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged");
        try {
            Log.d(TAG, "surfaceChanged: registerSurfaceHolder");
            mVirtualCamera.registerSurface(holder);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.frontCamera:
                mCameraName = VirtualCamera.CAMERA_AVM_FRONT;
                Log.d(TAG, "onClick: frontCamera");
                break;

            case R.id.rearCamera:
                mCameraName = VirtualCamera.CAMERA_AVM_BACK;
                Log.d(TAG, "onClick: rearCamera");
                break;

            case R.id.leftCamera:
                mCameraName = VirtualCamera.CAMERA_AVM_LEFT;
                Log.d(TAG, "onClick: leftCamera");
                break;

            case R.id.rightCamera:
                mCameraName = VirtualCamera.CAMERA_AVM_RIGHT;
                Log.d(TAG, "onClick: rightCamera");
                break;

            case R.id.driverMonitor:
                mCameraName = VirtualCamera.CAMERA_DRIVER;
                Log.d(TAG, "onClick: driverMonitor");
                break;

            case R.id.roofCamera:
                mCameraName = VirtualCamera.CAMERA_ROOF;
                Log.d(TAG, "onClick: roofCamera");
                break;

            case R.id.dvrCamera:
                mCameraName = VirtualCamera.CAMERA_DVR;
                Log.d(TAG, "onClick: dvrCamera");
                break;

            case R.id.avmCamera:
                mCameraName = VirtualCamera.CAMERA_AVM_SYNTHESIS;
                Log.d(TAG, "onClick: avmCamera");
                break;

            case R.id.takePhoto:
                Log.d(TAG, "onClick: takePhoto");
                break;

            case R.id.recorder:
                Log.d(TAG, "onClick: recorder");
                break;

            case R.id.avm_rotation:
                Log.d(TAG, "onClick: avm_rotation");
                break;

            case R.id.avm_distance:
                Log.d(TAG, "onClick: avm_distance");
                break;

            case R.id.avm_height:
                Log.d(TAG, "onClick: avm_height");
                break;

            case R.id.localPreview:
                Log.d(TAG, "onClick: localPreview");
                break;
        }

        if (mVirtualCamera.initDevice(mCameraName) != 0) {
            Log.e(TAG, "onClick: initDevice failed!");
            return;
        }

        if (mVirtualCamera.startStream(mCameraName) != 0) {
            Log.e(TAG, "onClick: start stream failed:" + mCameraName );
        }
    }
}
