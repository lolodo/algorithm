package com.example.liufangyuan.notificationtest;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.net.Uri;
import android.support.v4.app.NotificationCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button sendNotice = (Button)findViewById(R.id.send_notice);
        sendNotice.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.send_notice:
                Intent intent = new Intent(this, NotificationActivity.class);
                PendingIntent pi = PendingIntent.getActivity(this, 0, intent, 0);
                NotificationManager manager = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
                Notification notification = new NotificationCompat.Builder(this)
                        .setContentTitle("This is content title")
                        .setContentText("hello")
//                        .setContentText("但是您至少应向通知添加一个操作。 操作允许用户直接从通知转到应用中的 Activity，他们可在其中查看一个或多个事件或执行进一步的操作。\n" +
//                                "\n" +
//                                "一个通知可以提供多个操作。您应该始终定义一个当用户点击通知时会触发的操作；通常，此操作会在应用中打开 Activity。 您也可以向通知添加按钮来执行其他操作，例如，暂停闹铃或立即答复短信；此功能自 Android 4.1 起可用。")
                        .setWhen(System.currentTimeMillis())
                        .setSmallIcon(R.mipmap.ic_launcher)
                        .setLargeIcon(BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher))
                        .setContentIntent(pi)
                        .setAutoCancel(true)
//                        .setSound(Uri.fromFile(new File("/system/media/audio/ringtones/Free.ogg")))
//                        .setVibrate(new long[]{0, 1000, 1000, 1000})
//                        .setLights(Color.GREEN, 1000, 1000)
                        .setDefaults(NotificationCompat.DEFAULT_ALL)
//                        .setStyle(new NotificationCompat.BigTextStyle().bigText("但是您至少应向通知添加一个操作。 操作允许用户直接从通知转到应用中的 Activity，他们可在其中查看一个或多个事件或执行进一步的操作。\n" +
//                                "\n" +
//                                "一个通知可以提供多个操作。您应该始终定义一个当用户点击通知时会触发的操作；通常，此操作会在应用中打开 Activity。 您也可以向通知添加按钮来执行其他操作，例如，暂停闹铃或立即答复短信；此功能自 Android 4.1 起可用。如果使用其他操作按钮，"))
                        .setPriority(NotificationCompat.PRIORITY_MAX)
                        .build();
                manager.notify(1, notification);
                break;
            default:
                break;
        }
    }
}
