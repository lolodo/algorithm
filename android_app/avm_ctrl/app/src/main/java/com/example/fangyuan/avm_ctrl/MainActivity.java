package com.example.fangyuan.avm_ctrl;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class MainActivity extends AppCompatActivity {

	private int option;
	private int angle;
	private EditText ipAddr;
	private Socket socket;
	private OutputStream outputStream;
	private CameraPara cameraPara;
	private byte[] buffer= new byte[32];

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		RadioGroup mOptions = (RadioGroup) findViewById(R.id.Options);
		RadioGroup mAngles = (RadioGroup) findViewById(R.id.Angles);
		Button send = (Button) findViewById(R.id.send);
		ipAddr = (EditText)findViewById(R.id.ip_address);

		option = -1;
		angle = -1;

		mOptions.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(RadioGroup group, int checkedId) {
				switch (checkedId) {
					case R.id.option_2:
						option = 2;
						break;
					case R.id.option_3:
						option = 3;
						break;
					case R.id.option_4:
						option = 4;
						break;
					case R.id.option_5:
						option = 5;
						break;
					case R.id.option_6:
						option = 6;
						break;
					case R.id.option_7:
						option = 7;
						break;
					default:
						option = 2;
						break;
				}
			}
		});

		mAngles.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(RadioGroup group, int checkedId) {
				switch (checkedId) {
					case R.id.angle_0:
						angle = 0;
						break;
					case R.id.angle_45:
						angle = 45;
						break;
					case R.id.angle_90:
						angle = 90;
						break;
					case R.id.angle_135:
						angle = 135;
						break;
					case R.id.angle_180:
						angle = 180;
						break;
					case R.id.angle_225:
						angle = 225;
						break;
					case R.id.angle_270:
						angle = 270;
						break;
					case R.id.angle_315:
						angle = 315;
						break;
					default:
						angle = 180;
						break;
				}
			}
		});

		send.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				if (ipAddr.getText().toString().isEmpty()) {
					Toast.makeText(getApplicationContext(), "Please set ip address!", Toast.LENGTH_SHORT).show();
					return;
				}

				switch (v.getId()) {
					case R.id.send:
						send_socket();
						break;
					default:
						break;
				}
			}
		});
	}

	private void send_socket() {
		if (option == -1 || angle == -1) {
			Toast.makeText(getApplicationContext(), "Please select option and angle!", Toast.LENGTH_SHORT).show();
			return;
		}

		try {
			InetAddress serAddr = InetAddress.getByName(ipAddr.getText().toString());
			socket = new Socket(serAddr, 8556);
		} catch (UnknownHostException e1) {
			e1.printStackTrace();
		} catch (IOException e2) {
			e2.printStackTrace();
		}

		if (socket.isConnected()) {
			Toast.makeText(getApplicationContext(), "server is connected!", Toast.LENGTH_SHORT).show();
		}

		try {
			cameraPara = new CameraPara();
			cameraPara.setHorizontalViewAngle(angle);
			cameraPara.setOption(option);

			outputStream = socket.getOutputStream();
			outputStream.write(buffer, 0, 32);
			outputStream.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}

		try {
			outputStream.close();
			socket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public class CameraPara {
		private  int option;
		private  float horizontalViewAngle;
		private float viewPointHeight;
		private float viewPointDist;
		private float resultHFOV;
		private int shiftX;
		private int shiftY;
		private float zoom;
		private boolean addCar;

		public CameraPara() {
			option = 2;
			horizontalViewAngle = 180;
			viewPointHeight = 500;
			viewPointDist = 1500;
			resultHFOV = 100;
			shiftX = 0;
			shiftY = 0;
			zoom = 1;
			addCar = true;
		}

		public void setOption(int option) {
			this.option = option;
		}

		public void setHorizontalViewAngle(float angle) {
			this.horizontalViewAngle = angle;
		}

		public void setViewPointHeight(float height) {
			this.viewPointHeight = height;
		}

		public void setViewPointDist(float dist) {
			this.viewPointDist = dist;
		}

		public void setResultHFOV(float hfov) {
			this.resultHFOV = hfov;
		}

		public void setShiftX(int shiftX) {
			this.shiftX = shiftX;
		}

		public void setShiftY(int shiftY) {
			this.shiftY = shiftY;
		}

		public void setZoom(int zoom) {
			this.zoom = zoom;
		}

		public void formatBuffer() {
			intToLH(0, this.option);
			floatToLH(4, this.horizontalViewAngle);
			floatToLH(8, this.viewPointHeight);
			floatToLH(12, this.viewPointDist);
			floatToLH(16, this.resultHFOV);
			intToLH(20, this.shiftX);
			intToLH(24, this.shiftY);
			floatToLH(28, this.zoom);
		}

		private void intToLH(int n, int val) {
			buffer[n] = (byte)(val & 0xff);
			buffer[n + 1] = (byte)(val >> 8 & 0xff);
			buffer[n + 2] = (byte)(val >> 16 & 0xff);
			buffer[n + 3] = (byte)(val >> 24 & 0xff);
		}

		private void floatToLH(int n, float val) {
			int v = Float.floatToRawIntBits(val);
			intToLH(n, v);
		}
	}
}
