package com.autofarmbot;

import android.app.AlarmManager;
import android.app.Application;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;

import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.autofarmbot.server.Server;

public class App extends Application {
    public static final String CHANNEL_1_ID = "channel1";
    private static NotificationManagerCompat notificationManager;

    @Override
    public void onCreate() {
        super.onCreate();
        createNotificationChannels();
        notificationManager = NotificationManagerCompat.from(this);
        Server.startConnection();

        // Set up the AlarmManager to trigger the NotificationService
        setAlarm();
    }

    private void createNotificationChannels() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel1 = new NotificationChannel(
                    CHANNEL_1_ID,
                    "Action required",
                    NotificationManager.IMPORTANCE_HIGH
            );
            channel1.enableVibration(true);
            channel1.setDescription("Alert");
            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(channel1);
        }
    }

    private void setAlarm() {
        AlarmManager alarmManager = (AlarmManager) getSystemService(Context.ALARM_SERVICE);

        Intent intent = new Intent(this, NotificationBroadcastReceiver.class);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(this, 0, intent, PendingIntent.FLAG_IMMUTABLE);

        // Set the alarm to repeat every 5 seconds for testing
        alarmManager.setRepeating(
                AlarmManager.RTC,
                System.currentTimeMillis(),
                5 * 1000,  // 5 seconds in milliseconds
                pendingIntent
        );
    }
}
