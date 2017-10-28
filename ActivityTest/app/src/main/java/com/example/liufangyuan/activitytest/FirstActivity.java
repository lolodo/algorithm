package com.example.liufangyuan.activitytest;

import android.content.Intent;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class FirstActivity extends BaseActivity {

    private static final String TAG = "FirstActivity";

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);

        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        super.onOptionsItemSelected(item);

        switch (item.getItemId()) {
            case R.id.add_item:
                Toast.makeText(this, "You clicked Add", Toast.LENGTH_SHORT).show();
                break;

            case R.id.remove_item:
                Toast.makeText(this, "You clicked Remove", Toast.LENGTH_SHORT).show();
                break;
            default:
        }

        return true;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case 1:
                if (resultCode == RESULT_OK) {
                    String returnedData = data.getStringExtra("data_return");
                    Log.d(TAG, "onActivityResult: returnedData is " + returnedData);
                }
                break;
            default:
                break;
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.d(TAG, "onStart");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        Log.d(TAG, "onCreate: " + this.toString());
        Log.d(TAG, "onCreate: task id is " + getTaskId());
        setContentView(R.layout.first_layout);

        Button button1 = (Button) findViewById(R.id.button_1);
        button1.setOnClickListener (new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SecondActivity.actionStart(FirstActivity.this, "data1", "data2");
//                Intent intent = new Intent(FirstActivity.this, SecondActivity.class);
//           //                Intent intent = new Intent(Intent.ACTION_VIEW);
//                intent.setData(Uri.parse("http:www.baidu.com"));
//                Intent intent = new Intent(Intent.ACTION_DIAL);
//                intent.setData(Uri.parse("tel:10086"));
//                String data = "Hello SecondActivity";
//                Intent intent = new Intent(FirstActivity.this, SecondActivity.class);
//                intent.putExtra("extra_data", data);
//                startActivity(intent);
//                Toast.makeText(FirstActivity.this, "You clicked Button 1", Toast.LENGTH_LONG).show();
//                startActivityForResult(intent, 1);


            }
        });
    }
}
