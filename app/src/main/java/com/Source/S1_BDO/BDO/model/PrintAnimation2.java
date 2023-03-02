package com.Source.S1_BDO.BDO.model;

import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;

import static android.content.ContentValues.TAG;

public class PrintAnimation2 extends DemoAppActivity {

    Animation fadein;

    LinearLayout llsuccess;
    ImageView imgsuccess;
    TextView tvsuccess;

    public static String final_string;
    private int inTimeOut = 1;

    private CountDownTimer timer = null; // sidumili - init timer, set private variable
    private long inDelay = 500;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate: PrintAnimation2");
        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);
        super.onCreate(savedInstanceState);
        Bundle bundle = getIntent().getExtras();

        overridePendingTransition(0, 0); // disable the animation, faster

        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);    //Tine:  Show Status Bar

        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        // Set the content to appear under the system bars so that the
                        // content doesn't resize when the system bars hide and show.
                        //| View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        //| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        //| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        // Hide the nav bar and status bar
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        //| View.SYSTEM_UI_FLAG_FULLSCREEN);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.confirm_printing2);
        Intent intent = getIntent();
        String dispmsg = intent.getStringExtra("pass_in_string");
        Log.i("PrintAnimation->dispmsg", dispmsg);

        llsuccess = (LinearLayout) findViewById(R.id.llsuccess);
        imgsuccess = (ImageView) findViewById(R.id.imgsuccess);
        tvsuccess = (TextView) findViewById(R.id.tvsuccess);

        showSuccess();

        getTimerRestart();

    }

    public void getTimerCancel() {
        if (timer != null)
            timer.cancel();
    }
    public void getTimerRestart()
    {
        timer = new CountDownTimer(inTimeOut*1000, 1000) {
            public void onTick(long millisUntilFinished) {
                Log.i("Timer", "Timer onTick");
                Log.i("Timer", "seconds remaining: " + millisUntilFinished / 1000);
            }
            public void onFinish() {
                Log.i("Timer", "Timer onFinish");

                //getTimerCancel();
                final_string = "TIMEOUT";

                Log.i("PATRICK", "Timeout S1InputString");

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        PrintAnimation2.this.finish();
                    }
                }, inDelay);

                try
                {
                    // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                    synchronized (MainActivity.LOCK) {
                        MainActivity.LOCK.setCondition(true);
                        MainActivity.LOCK.notifyAll();

                    }
                }
                catch (Exception ex)
                {
                    Log.i(TAG, "onItemClick: Exception error " + ex.getMessage());
                }
            }
        };
        timer.start();
    }

    public void showSuccess()
    {
        Log.i(TAG, "showSuccess: run");
        llsuccess.setVisibility(View.VISIBLE);

        fadein = AnimationUtils.loadAnimation(this, R.anim.fadein);
        imgsuccess.setBackgroundResource(R.drawable.icon_success);
        llsuccess.startAnimation(fadein);
        tvsuccess.setText("Transaction successful.");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        final_string = null;
        timer = null;
    }

}
