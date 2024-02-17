package com.autofarmbot;


import static com.autofarmbot.App.CHANNEL_1_ID;
import static com.autofarmbot.ui.plantstatus.PlantstatusFragment.ECMAXBOUND;
import static com.autofarmbot.ui.plantstatus.PlantstatusFragment.ECMINBOUND;
import static com.autofarmbot.ui.plantstatus.PlantstatusFragment.PHMAXBOUND;
import static com.autofarmbot.ui.plantstatus.PlantstatusFragment.PHMINBOUND;

import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;

import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.autofarmbot.server.Server;
import com.autofarmbot.ui.plantstatus.PlantstatusViewModel;

import java.util.Timer;
import java.util.TimerTask;


public class NotificationService extends Service {

    Timer timer;
    TimerTask timerTask;
    String TAG = "Timers";
    int Your_X_SECS = 10;


    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.e(TAG, "onStartCommand");
        super.onStartCommand(intent, flags, startId);

        startTimer();

        return START_STICKY;
    }


    @Override
    public void onCreate() {
        Log.e(TAG, "onCreate");


    }

    @Override
    public void onDestroy() {
        Log.e(TAG, "onDestroy");
        stoptimertask();
        super.onDestroy();


    }

    //we are going to use a handler to be able to run in our TimerTask
    final Handler handler = new Handler();


    public void startTimer() {
        //set a new Timer
        timer = new Timer();

        //initialize the TimerTask's job
        initializeTimerTask();

        //schedule the timer, after the first 5000ms the TimerTask will run every 10000ms
        timer.schedule(timerTask, 5000, Your_X_SECS * 1000); //
        //timer.schedule(timerTask, 5000,1000); //
    }

    public void stoptimertask() {
        //stop the timer, if it's not already null
        if (timer != null) {
            timer.cancel();
            timer = null;
        }
    }

    public void initializeTimerTask() {

        timerTask = new TimerTask() {
            public void run() {

                //use a handler to run a toast that shows the current timestamp
                handler.post(new Runnable() {
                    public void run() {

                        //TODO CALL NOTIFICATION FUNC
                        showNotification();

                    }
                });
            }
        };
    }
    private void showNotification() {
        Double ph = Server.getPH();
        Double ec = Server.getEC();

        if(ph < PHMINBOUND || ph > PHMAXBOUND || ec < ECMINBOUND || ec > ECMAXBOUND) {
            Intent activityIntent = new Intent(this, MainActivity.class);
            activityIntent.putExtra("FRAGMENT_TO_LOAD", "PlantStatusFragment");
            PendingIntent  contentIntent = PendingIntent.getActivity(this, 0, activityIntent, PendingIntent.FLAG_IMMUTABLE);

            NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_1_ID)
                    .setSmallIcon(R.drawable.baseline_notifications_active_24)
                    .setContentTitle("Hydroponic solution unstable")
                    .setContentText("Click for more details")
                    .setPriority(NotificationCompat.PRIORITY_HIGH)
                    .setCategory(NotificationCompat.CATEGORY_MESSAGE)
                    .setContentIntent(contentIntent)
                    .setAutoCancel(true);  // Dismiss the notification when clicked

            NotificationManagerCompat notificationManager = NotificationManagerCompat.from(this);
            notificationManager.notify(1, builder.build());
        }

    }
}


