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

public class PrintAnimation3 extends DemoAppActivity {

    Animation ttb2, bottom_animation, fadein, rotate;

    LinearLayout llapproved;
    TextView box_msg;
    TextView box_msg1;
    TextView box_msg2;
    TextView box_msg3;
    TextView box_msg4;
    TextView box_msg5;
    TextView box_msg6;
    TextView box_msg7;
    TextView box_msg8;

    LinearLayout llsuccess;
    ImageView imgsuccess;
    TextView tvsuccess;

    LinearLayout lloptionmerchant;
    LinearLayout lloptioncustomer;

    TextView tvoptionmerchant;
    TextView tvoptioncustomer;

    ImageView imgoptionmerchant;
    ImageView imgoptioncustomer;

    public static String final_string;
    private int inTimeOut = 2;

    private CountDownTimer timer = null; // sidumili - init timer, set private variable
    private long inDelay = 500;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate: PrintAnimation3");
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

        setContentView(R.layout.confirm_printing3);
        Intent intent = getIntent();
        String dispmsg = intent.getStringExtra("pass_in_string");
        Log.i("PrintAnimation->dispmsg", dispmsg);

        box_msg = (TextView) findViewById(R.id.box_msg);
        box_msg1 = (TextView) findViewById(R.id.box_msg1);
        box_msg2 = (TextView) findViewById(R.id.box_msg2);
        box_msg3 = (TextView) findViewById(R.id.box_msg3);
        box_msg4 = (TextView) findViewById(R.id.box_msg4);
        box_msg5 = (TextView) findViewById(R.id.box_msg5);
        box_msg6 = (TextView) findViewById(R.id.box_msg6);
        box_msg7 = (TextView) findViewById(R.id.box_msg7);
        box_msg8 = (TextView) findViewById(R.id.box_msg8);

        llapproved = (LinearLayout) findViewById(R.id.llapproved);
        llsuccess = (LinearLayout) findViewById(R.id.llsuccess);
        imgsuccess = (ImageView) findViewById(R.id.imgsuccess);
        tvsuccess = (TextView) findViewById(R.id.tvsuccess);

        // Merchant
        lloptionmerchant = (LinearLayout) findViewById(R.id.lloptionmerchant);
        imgoptionmerchant = (ImageView) findViewById(R.id.imgoptionmerchant);
        tvoptionmerchant = (TextView) findViewById(R.id.tvoptionmerchant);

        // Customer
        lloptioncustomer = (LinearLayout) findViewById(R.id.lloptioncustomer);
        imgoptioncustomer = (ImageView) findViewById(R.id.imgoptioncustomer);
        tvoptioncustomer = (TextView) findViewById(R.id.tvoptioncustomer);

        String[] dispmsginfo = dispmsg.split("\\|");
        int msgcnt = dispmsginfo.length;

        for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
            System.out.println("PrintAnimation->split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx) {
                case 0: // Header 1

                    break;
                case 1: // Header 2

                    break;
                case 2: // Type

                    break;
                case 3: // Message 1
                    box_msg.setText(dispmsginfo[inIdx]);
                    break;
                case 4: // Message 2
                    box_msg1.setText(dispmsginfo[inIdx]);
                    break;
                case 5: // Message 3
                    box_msg2.setText(dispmsginfo[inIdx]);
                    break;
                case 6: // Message 4
                    box_msg3.setText(dispmsginfo[inIdx]);
                    break;
                case 7: // Message 5
                    box_msg4.setText(dispmsginfo[inIdx]);
                    break;
                case 8: // Message 6
                    box_msg5.setText(dispmsginfo[inIdx]);
                    break;
                case 9: // Message 7
                    box_msg6.setText(dispmsginfo[inIdx]);
                    break;
                case 10: // Message 8
                    box_msg7.setText(dispmsginfo[inIdx]);
                    break;
                case 11: // Message 9
                    box_msg8.setText(dispmsginfo[inIdx]);
                    break;
            }
        }

        showApproved();
        showSuccess();

        //getTimerRestart();

        lloptionmerchant.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerCancel();
                final_string = "CONFIRM PRINT MERCHANT COPY";

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        PrintAnimation3.this.finish();
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
        });

        lloptioncustomer.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerCancel();
                final_string = "CONFIRM PRINT CUSTOMER COPY";

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        PrintAnimation3.this.finish();
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
        });
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

                getTimerCancel();
                final_string = "TIMEOUT";

                Log.i("PATRICK", "Timeout S1InputString");

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        PrintAnimation3.this.finish();
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

    public void showApproved()
    {
        Log.i(TAG, "showApproved: run");
        llapproved.setVisibility(View.VISIBLE);
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
