package com.example.liufangyuan.uiwidgettest;

import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private static final String TAG = "MainActivity";
    private EditText editText;
    private ImageView imageView;
    private ProgressBar progressbar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button button = (Button)findViewById(R.id.button);
//        button.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Log.d(TAG, "onClick: get it!");
//            }
//        });
        editText = (EditText)findViewById(R.id.edit_text);
        imageView = (ImageView)findViewById(R.id.image_view);
        progressbar = (ProgressBar)findViewById(R.id.progress_bar);
        button.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.button:
//                if (progressbar.getVisibility() == View.GONE) {
//                    progressbar.setVisibility(View.VISIBLE);
//                } else {
//                    progressbar.setVisibility(View.GONE);
//                }
//                String inputText = editText.getText().toString();
//                Toast.makeText(MainActivity.this, inputText, Toast.LENGTH_SHORT).show();
//                Log.d(TAG, "onClick: button");
//                imageView.setImageResource(R.drawable.robots);
//                int progress = progressbar.getProgress();
//                progress += 10;
//                progressbar.setProgress(progress);
//                AlertDialog.Builder dialog = new AlertDialog.Builder(MainActivity.this);
//                dialog.setTitle("This is Dialog");
//                dialog.setMessage("Something important");
//                dialog.setCancelable(false);
//                dialog.setPositiveButton("OK", new DialogInterface.OnClickListener() {
//                    @Override
//                    public void onClick(DialogInterface dialog, int which) {
//
//                    }
//                });
//
//                dialog.setNegativeButton("Cancel", new DialogInterface.OnClickListener(){
//                    @Override
//                    public void onClick(DialogInterface dialog, int which) {
//
//                    }
//                });
//
//                dialog.show();
                ProgressDialog progressDialog = new ProgressDialog(MainActivity.this);
                progressDialog.setTitle("This is ProgressDialog");
                progressDialog.setMessage("Loading...");
                progressDialog.setCancelable(true);
                progressDialog.show();
                break;
            default:
                break;
        }
    }
}
