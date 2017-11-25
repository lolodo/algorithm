package com.example.liufangyuan.sharedpreferencestest;

import android.content.SharedPreferences;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.sql.BatchUpdateException;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "sharedpreferencestest";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.e(TAG, "onCreate: ");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button saveData = (Button)findViewById(R.id.save_data);
        saveData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.e(TAG, "save: ");
                SharedPreferences.Editor editor = getSharedPreferences("data", MODE_PRIVATE).edit();
                editor.putString("name", "Tom");
                editor.putInt("age", 28);
                editor.putBoolean("married", false);
                editor.apply();
            }
        });

        Button restoreData = (Button)findViewById(R.id.restore_data);
        restoreData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d(TAG, "onClick: enter");
                SharedPreferences pref = getSharedPreferences("data", MODE_PRIVATE);
                String name = pref.getString("name", "");
                int age = pref.getInt("age", 0);
                boolean married = pref.getBoolean("married", false);
                Log.d(TAG, "onClick: name is " + name);
                Log.d(TAG, "onClick: age is " + age);
                Log.d(TAG, "onClick: married is " + married);
            }
        });
    }
}
