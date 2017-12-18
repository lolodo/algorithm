package com.example.alex.networktest;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import org.w3c.dom.Text;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

import okhttp3.FormBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    TextView responseText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button sendRequest = (Button)findViewById(R.id.send_request);
        responseText = (TextView)findViewById(R.id.resonse_text);
        sendRequest.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.send_request) {
            sendRequestWithOkHttp();
        }
    }

    private void sendRequestWithOkHttp() {
        new Thread(new Runnable() {
            @Override
            public void run() {
//                HttpURLConnection connection = null;

//                BufferedReader reader = null;
                try {
                    OkHttpClient client = new OkHttpClient();
//                    URL url = new URL("https://www.baidu.com");
//                    connection = (HttpURLConnection)url.openConnection();
//                    connection.setRequestMethod("GET");
//                    connection.setConnectTimeout(8000);
//                    connection.setReadTimeout(8000);
                    Request request = new Request.Builder().url("http://www.baidu.com").build();
                    Response response = client.newCall(request).execute();
                    String responseData = response.body().string();
//                    RequestBody requestBody = new FormBody().Builder()
//                            .add("username", "admin")
//                            .add("passwd", "123456")
//                            .build();

//                    InputStream in = connection.getInputStream();

//                    reader = new BufferedReader(new InputStreamReader(in));
//                    StringBuilder response = new StringBuilder();
//                    String line;
//                    while ((line = reader.readLine()) != null) {
//                        response.append(line);
//                    }

                    showResponse(responseData);
                } catch (Exception e){
                    e.printStackTrace();
                }
            }
        }).start();
    }

    private void showResponse(final String s) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                responseText.setText(s);
            }
        });
    }
}
