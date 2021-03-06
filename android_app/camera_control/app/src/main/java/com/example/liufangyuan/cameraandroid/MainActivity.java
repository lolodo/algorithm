package com.example.liufangyuan.cameraandroid;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import org.json.JSONObject;

import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

enum CameraOperations {
	OPEN_FRONT, CLOSE_FRONT, OPEN_REAR, CLOSE_REAR, OPEN_LEFT, CLOSE_LEFT, OPEN_RIGHT, CLOSE_RIGHT,
	OPEN_DRIVER_MONITOR, CLOSE_DRIVER_MONITOR,
	OPEN_AVM, CLOSE_AVM, AVM_H_ROTATE, AVM_VD_ROTATE, AVM_VH_ROTATE,
	AVM_H_ROTATE_DEGREE, AVM_VD_ROTATE_VALUE, AVM_VH_ROTATE_VALUE
}

public class MainActivity extends AppCompatActivity {

	private static final String TAG = "cameraService";
	private String cameraName;

	public static final String CMD_SET = "cmd-set-parameters";
	public static final String CMD_GET = "cmd-get-parameters";
	public static final String CMD_CHECK = "cmd-check-device";
	public static final String CMD_INIT = "cmd-init-device";
	public static final String CMD_START = "cmd-start-stream";
	public static final String CMD_STOP = "cmd-start-stream";
	public static final String CMD_RELEASE = "cmd-release-device";
	public static final String CMD_CONTROL = "cmd-control";

	public static final String EVT_VERSION = "evt-version";
	public static final String CAMERA_NAME = "camera-name";
	public static final String SUPPORTED_PREVIEW_FORMAT = "preview-format-values";
	public static final String SUPPORTED_PREVIEW_SIZE = "preview-size-values";
	public static final String SUPPORTED_PREVIEW_FPS = "preview-fps-values";
	public static final String RECEIVE_IP_ADDRESS = "receive-ip-address";
	public static final String RECEIVE_PORT = "receive-port";

	public static final String EVT_VER_ONE = "evt-version-1.0";
	public static final String EVT_VER_TWO = "evt-version-2.0";
	public static final String PIXEL_FORMAT_JPEG = "jpeg";
	public static final String PIXEL_FORMAT_BAYER_RGGB = "bayer-rggb";

	public static final String FORMAT_H_264 = "preview-format-h264";
	public static final String DEFAULT_SIZE = "1280x800";
	public static final String DEFAULT_FPS = "30";
	public static final String DEFAULT_IP = "127.0.0.1";
	public static final String DEFAULT_PORT = "8554";

	public static final String AVM_VIEW_HORIZONTAL_ROTATION_KEY = "view_horizontal_rotation";
	public static final String AVM_VIEW_HEIGHT_ROTATION_KEY = "view_height_rotation";
	public static final String AVM_VIEW_DISTANCE_ROTATION_KEY = "view_distance_rotation";

	private int h_rotate;
	private int vh_rotate;
	private int vd_rotate;

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

		Button avm_hori_rotation = (Button) findViewById(R.id.avm_hori_rotate);
		Button avm_vd_rotation = (Button) findViewById(R.id.avm_vd_rotate);
		Button avm_vh_rotation = (Button) findViewById(R.id.avm_vh_rotate);

		SeekBar seekBar = (SeekBar) findViewById(R.id.h_progress);
		final TextView textView = (TextView) findViewById(R.id.h_text);
		seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				// 当拖动条的滑块位置发生改变时触发该方法,在这里直接使用参数progress，即当前滑块代表的进度值
				textView.setText("horizontal degree:" + Integer.toString(progress + 90));
//                if ((progress % 5) == 0) {
				h_rotate = progress + 90;
				new Thread((new Runnable() {
					@Override
					public void run() {
						operate_camera(CameraOperations.AVM_H_ROTATE_DEGREE);
					}
				})).start();
//                }
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				Log.e("------------", "开始滑动！");
			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				Log.e("------------", "停止滑动！");
			}
		});

		SeekBar vh_seekBar = (SeekBar) findViewById(R.id.vh_progress);
		final TextView vh_textView = (TextView) findViewById(R.id.vh_text);
		vh_seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				// 当拖动条的滑块位置发生改变时触发该方法,在这里直接使用参数progress，即当前滑块代表的进度值
				vh_textView.setText("height:" + Integer.toString(progress + 800) + "mm");
				vh_rotate = progress + 800;
				new Thread((new Runnable() {
					@Override
					public void run() {
						operate_camera(CameraOperations.AVM_VH_ROTATE_VALUE);
					}
				})).start();
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				Log.e("------------", "开始滑动！");
			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				Log.e("------------", "停止滑动！");
			}
		});

		SeekBar vd_seekBar = (SeekBar) findViewById(R.id.vd_progress);
		final TextView vd_textView = (TextView) findViewById(R.id.vd_text);
		vd_seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				vd_textView.setText("distance:" + Integer.toString(progress + 1000) + "mm");
				vd_rotate = progress + 1000;
					new Thread((new Runnable() {
						@Override
						public void run() {
							operate_camera(CameraOperations.AVM_VD_ROTATE_VALUE);
						}
					})).start();
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				Log.e("------------", "开始滑动！");
			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				Log.e("------------", "停止滑动！");
			}
		});

		avm_hori_rotation.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				switch (v.getId()) {
					case R.id.avm_hori_rotate:
						new Thread(new Runnable() {
							@Override
							public void run() {
								operate_camera(CameraOperations.AVM_H_ROTATE);
							}
						}).start();
						break;
					default:
						break;
				}
			}
		});

		avm_vd_rotation.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				switch (v.getId()) {
					case R.id.avm_vd_rotate:
						new Thread(new Runnable() {
							@Override
							public void run() {
								operate_camera(CameraOperations.AVM_VD_ROTATE);
							}
						}).start();
						break;
					default:
						break;
				}
			}
		});

		avm_vh_rotation.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				switch (v.getId()) {
					case R.id.avm_vh_rotate:
						new Thread(new Runnable() {
							@Override
							public void run() {
								operate_camera(CameraOperations.AVM_VH_ROTATE);
							}
						}).start();
						break;
					default:
						break;
				}
			}
		});

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
			Socket socket = new Socket("172.20.1.11", 37568);
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
		StringBuilder sel_cmd = new StringBuilder();

		this.cameraName = cam_name;
		sel_cmd.append(EVT_VERSION);
		sel_cmd.append("=");
		sel_cmd.append(EVT_VER_TWO);
		sel_cmd.append(";");

		sel_cmd.append(CAMERA_NAME);
		sel_cmd.append("=");
		sel_cmd.append(cam_name);

		try {
			jsonCmd.put(CMD_SET, sel_cmd.toString());
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
		StringBuilder set_cmd = new StringBuilder();

		set_cmd.append(SUPPORTED_PREVIEW_FORMAT);
		set_cmd.append("=");
		set_cmd.append(FORMAT_H_264);
		set_cmd.append(";");

		set_cmd.append(SUPPORTED_PREVIEW_SIZE);
		set_cmd.append("=");
		set_cmd.append(DEFAULT_SIZE);
		set_cmd.append(";");

		set_cmd.append(SUPPORTED_PREVIEW_FPS);
		set_cmd.append("=");
		set_cmd.append(DEFAULT_FPS);
		set_cmd.append(";");

		set_cmd.append(RECEIVE_IP_ADDRESS);
		set_cmd.append("=");
		set_cmd.append(DEFAULT_IP);
		set_cmd.append(";");

		set_cmd.append(RECEIVE_PORT);
		set_cmd.append("=");
		set_cmd.append(DEFAULT_PORT);

		try {
			jsonCmd.put(CMD_SET, set_cmd.toString());
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
			jsonCmd.put(CMD_INIT, "NULL");
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
			jsonCmd.put(CMD_START, "NULL");
		} catch (Exception e) {
			Log.e(TAG, "start_cam");
			return -1;
		}

		final String result = jsonCmd.toString();
		Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
		send_msg(result);

		return 0;
	}

	private int control_h_rotate() {
		JSONObject jsonCmd = new JSONObject();
		StringBuilder h_cmd = new StringBuilder();

		h_cmd.append(CAMERA_NAME);
		h_cmd.append("=");
		h_cmd.append("remote-camera-avm-synthesis;");

		h_cmd.append(AVM_VIEW_HORIZONTAL_ROTATION_KEY);
		h_cmd.append("=");
		h_cmd.append("yes");
		try {
			jsonCmd.put(CMD_CONTROL, h_cmd.toString());
		} catch (Exception e) {
			Log.e(TAG, "control_h_rotate");
			return -1;
		}

		final String result = jsonCmd.toString();
		Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
		send_msg(result);

		return 0;
	}

	private int control_vh_rotate() {
		JSONObject jsonCmd = new JSONObject();
		StringBuilder h_cmd = new StringBuilder();

		h_cmd.append(CAMERA_NAME);
		h_cmd.append("=");
		h_cmd.append("remote-camera-avm-synthesis;");

		h_cmd.append(AVM_VIEW_HEIGHT_ROTATION_KEY);
		h_cmd.append("=");
		h_cmd.append("yes");

		try {
			jsonCmd.put(CMD_CONTROL, h_cmd.toString());
		} catch (Exception e) {
			Log.e(TAG, "control_vh_rotate");
			return -1;
		}

		final String result = jsonCmd.toString();
		Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
		send_msg(result);

		return 0;
	}

	private int control_vd_rotate() {
		JSONObject jsonCmd = new JSONObject();
		StringBuilder h_cmd = new StringBuilder();

		h_cmd.append(CAMERA_NAME);
		h_cmd.append("=");
		h_cmd.append("remote-camera-avm-synthesis;");

		h_cmd.append(AVM_VIEW_DISTANCE_ROTATION_KEY);
		h_cmd.append("=");
		h_cmd.append("yes");

		try {
			jsonCmd.put(CMD_CONTROL, h_cmd.toString());
		} catch (Exception e) {
			Log.e(TAG, "control_vh_rotate");
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
			jsonCmd.put(CMD_RELEASE, "NULL");
		} catch (Exception e) {
			Log.e(TAG, "release_cam");
			return;
		}

		final String result = jsonCmd.toString();
		Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
		send_msg(result);
	}

	private void horizontal_degree() {
		JSONObject jsonCmd = new JSONObject();
		StringBuilder h_cmd = new StringBuilder();

		h_cmd.append(CAMERA_NAME);
		h_cmd.append("=");
		h_cmd.append("remote-camera-avm-synthesis;");

		h_cmd.append("zrot");
		h_cmd.append("=");
		h_cmd.append(h_rotate);

		try {
			jsonCmd.put(CMD_CONTROL, h_cmd.toString());
		} catch (Exception e) {
			Log.e(TAG, "horizontal_degree");
			return;
		}

		final String result = jsonCmd.toString();
		Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
		send_msg(result);
	}

	private void vh_control() {
		JSONObject jsonCmd = new JSONObject();
		StringBuilder h_cmd = new StringBuilder();

		h_cmd.append(CAMERA_NAME);
		h_cmd.append("=");
		h_cmd.append("remote-camera-avm-synthesis;");

		h_cmd.append("viewh");
		h_cmd.append("=");
		h_cmd.append(vh_rotate);

		try {
			jsonCmd.put(CMD_CONTROL, h_cmd.toString());
		} catch (Exception e) {
			Log.e(TAG, "horizontal_degree");
			return;
		}

		final String result = jsonCmd.toString();
		Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
		send_msg(result);
	}


	private void vd_control() {
		JSONObject jsonCmd = new JSONObject();
		StringBuilder h_cmd = new StringBuilder();

		h_cmd.append(CAMERA_NAME);
		h_cmd.append("=");
		h_cmd.append("remote-camera-avm-synthesis;");

		h_cmd.append("viewd");
		h_cmd.append("=");
		h_cmd.append(vd_rotate);

		try {
			jsonCmd.put(CMD_CONTROL, h_cmd.toString());
		} catch (Exception e) {
			Log.e(TAG, "horizontal_degree");
			return;
		}

		final String result = jsonCmd.toString();
		Log.i(TAG, "cmd>>>>>>>>>>>>: " + result);
		send_msg(result);
	}

	private void operate_camera(CameraOperations operator) {
		switch (operator) {
			case AVM_VD_ROTATE_VALUE:
				vd_control();
				break;

			case AVM_VH_ROTATE_VALUE:
				vh_control();
				break;

			case AVM_H_ROTATE_DEGREE:
				horizontal_degree();
				break;

			case AVM_H_ROTATE:
				control_h_rotate();
				break;

			case AVM_VD_ROTATE:
				control_vd_rotate();
				break;

			case AVM_VH_ROTATE:
				control_vh_rotate();
				break;

			case OPEN_FRONT:
				select_cam("remote-camera-avm-front");
				get_param();
				set_cam();
				init_cam();
				start_cam();
				break;

			case CLOSE_FRONT:
				release_cam();
				break;

			case OPEN_REAR:
				select_cam("remote-camera-avm-back");
				get_param();
				set_cam();
				init_cam();
				start_cam();
				break;

			case CLOSE_REAR:
				release_cam();
				break;

			case OPEN_LEFT:
				select_cam("remote-camera-avm-left");
				get_param();
				set_cam();
				init_cam();
				start_cam();
				break;

			case CLOSE_LEFT:
				release_cam();
				break;

			case OPEN_RIGHT:
				select_cam("remote-camera-avm-right");
				get_param();
				set_cam();
				init_cam();
				start_cam();
				break;

			case CLOSE_RIGHT:
				release_cam();
				break;

			case OPEN_DRIVER_MONITOR:
				select_cam("remote-camera-driver-monitor");
				get_param();
				set_cam();
				init_cam();
				start_cam();
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



