package com.example.liufangyuan.cameraandroid;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import org.json.JSONObject;

import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

enum CameraOperations {
    OPEN_FRONT, CLOSE_FRONT, OPEN_REAR, CLOSE_REAR, OPEN_LEFT, CLOSE_LEFT, OPEN_RIGHT, CLOSE_RIGHT,
    OPEN_DRIVER_MONITOR, CLOSE_DRIVER_MONITOR,
    OPEN_AVM, CLOSE_AVM
}

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "cameraService";
    private String cameraName;
    private String host = "172.20.1.1";
    private int port = 8554;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button left_open = (Button) findViewById(R.id.left_open);
        Button left_close = (Button) findViewById(R.id.left_close);

        Button right_open = (Button) findViewById(R.id.right_open);
        Button right_close = (Button) findViewById(R.id.right_close);

        Button front_open = (Button) findViewById(R.id.front_open);
        Button front_close = (Button) findViewById(R.id.front_close);

        Button rear_open = (Button) findViewById(R.id.rear_open);
        Button rear_close = (Button) findViewById(R.id.rear_close);

        Button driver_monitor_open = (Button) findViewById(R.id.driver_monitor_open);
        Button driver_monitor_close = (Button) findViewById(R.id.driver_monitor_close);

        Button avm_open = (Button) findViewById(R.id.avm_open);
        Button avm_close = (Button) findViewById(R.id.avm_close);

        front_open.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.front_open:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.OPEN_FRONT);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        front_close.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.front_close:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.CLOSE_FRONT);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        rear_open.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.rear_open:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.OPEN_REAR);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        rear_close.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.rear_close:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.CLOSE_REAR);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        left_open.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.left_open:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.OPEN_LEFT);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        left_close.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.left_close:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.CLOSE_LEFT);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        right_open.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.right_open:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.OPEN_RIGHT);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        right_close.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.right_close:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.CLOSE_RIGHT);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        driver_monitor_open.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.driver_monitor_open:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.OPEN_DRIVER_MONITOR);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        driver_monitor_close.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.driver_monitor_close:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.CLOSE_DRIVER_MONITOR);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        avm_open.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.avm_open:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.OPEN_AVM);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });

        avm_close.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.avm_close:
                        new Thread(new Runnable() {
                            @Override
                            public void run() {
                                operate_camera(CameraOperations.CLOSE_AVM);
                            }
                        }).start();
                        break;
                    default:
                        break;
                }
            }
        });
    }

    private void send_msg(final String msg) {
        try {
            Log.i(TAG, "send_msg: 1");
            Socket socket = new Socket("172.20.1.11", 8888);
            OutputStream os = socket.getOutputStream();
            os.write(msg.getBytes());
            os.flush();
            socket.shutdownOutput();
            socket.close();
            Log.i(TAG, "send_msg: 2");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void select_cam(String cam_name) {
        JSONObject jsonCmd = new JSONObject();
        JSONObject param = new JSONObject();

        this.cameraName = cam_name;

        try {
            param.put("evt-version", "evt-version-2.0");
        } catch (Exception e) {
            Log.e(TAG, "select_cam: evt-version");
            return;
        }

        try {
            param.put("camera-name", cam_name);
        } catch (Exception e) {
            Log.e(TAG, "select_cam: camera-name");
            return;
        }

        try {
            jsonCmd.put("cmd-set-parameters", param);
        } catch (Exception e) {
            Log.e(TAG, "select_cam: cmd-set-parameters");
            return;
        }

        final String result = jsonCmd.toString();
        Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
        send_msg(result);
    }

    private int get_param() {
        return 0;
    }

    private int set_cam() {
        JSONObject jsonCmd = new JSONObject();
        JSONObject param = new JSONObject();

        try {
            param.put("preview-format", "preview-format-h264");
        } catch (Exception e) {
            Log.e(TAG, "operate_camera: preview-format");
            return -1;
        }

        try {
            param.put("preview-size", "1280x800");
        } catch (Exception e) {
            Log.e(TAG, "operate_camera:preview-size");
            return -1;
        }

        try {
            param.put("preview-fps-values", "30");
        } catch (Exception e) {
            Log.e(TAG, "operate_camera:preview-fps-values");
            return -1;
        }

        try {
            param.put("receive-ip-address", host);
        } catch (Exception e) {
            Log.e(TAG, "operate_camera:ip-address");
            return -1;
        }

        try {
            param.put("receive-port", port);
        } catch (Exception e) {
            Log.e(TAG, "operate_camera:receive-port");
            return -1;
        }

        try {
            jsonCmd.put("cmd-set-parameters", param);
        } catch (Exception e) {
            Log.e(TAG, "operate_camera: cmd-set-parameters");
            return -1;
        }
        final String result = jsonCmd.toString();

        send_msg(result);
        Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);

        return 0;
    }

    private int init_cam() {
        JSONObject jsonCmd = new JSONObject();

        try {
            jsonCmd.put("cmd-init-device", "");
        } catch (Exception e) {
            Log.e(TAG, "init_cam");
            return -1;
        }

        final String result = jsonCmd.toString();
        Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
        send_msg(result);

        return 0;
    }

    private int start_cam() {
        JSONObject jsonCmd = new JSONObject();

        try {
            jsonCmd.put("cmd-start-stream", "");
        } catch (Exception e) {
            Log.e(TAG, "start_cam");
            return -1;
        }

        final String result = jsonCmd.toString();
        Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
        send_msg(result);

        return 0;
    }

    private void release_cam() {
        JSONObject jsonCmd = new JSONObject();

        try {
            jsonCmd.put("cmd-release-device", "");
        } catch (Exception e) {
            Log.e(TAG, "release_cam");
            return;
        }

        final String result = jsonCmd.toString();
        Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
        send_msg(result);
    }

    private void operate_camera(CameraOperations operator) {
        switch (operator) {
            case OPEN_FRONT:
                select_cam("remote-camera-avm-front");
                get_param();
                set_cam();
                init_cam();
                break;

            case CLOSE_FRONT:
                release_cam();
                break;

            case OPEN_REAR:
                select_cam("remote-camera-avm-back");
                get_param();
                set_cam();
                init_cam();
                break;

            case CLOSE_REAR:
                release_cam();
                break;

            case OPEN_LEFT:
                select_cam("remote-camera-avm-left");
                get_param();
                set_cam();
                init_cam();
                break;

            case CLOSE_LEFT:
                release_cam();
                break;

            case OPEN_RIGHT:
                select_cam("remote-camera-avm-right");
                get_param();
                set_cam();
                init_cam();
                break;

            case CLOSE_RIGHT:
                release_cam();
                break;

            case OPEN_DRIVER_MONITOR:
                select_cam("remote-camera-driver-monitor");
                get_param();
                set_cam();
                init_cam();
                break;

            case CLOSE_DRIVER_MONITOR:
                release_cam();
                break;

            case OPEN_AVM:
                select_cam("remote-camera-avm-synthesis");
                get_param();
                set_cam();
                init_cam();
                start_cam();
                break;

            case CLOSE_AVM:
                release_cam();
                break;

            default:
                break;
        }
    }
}



