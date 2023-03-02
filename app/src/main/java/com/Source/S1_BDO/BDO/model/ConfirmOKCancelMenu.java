package com.Source.S1_BDO.BDO.model;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.Trans.AmountEntryActivity;
import com.Source.S1_BDO.BDO.wub_lib;

import static android.content.ContentValues.TAG;

public class ConfirmOKCancelMenu extends DemoAppActivity {

    private Context mContext;

    private Button mButton;
    final Context c = this;

    EditText etInputStr;
    Button buOK;
    Button buCancel;

    public static String final_string;
    private int inTimeOut = 30;
    private long inDelay = 1000;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);
        super.onCreate(savedInstanceState);
        Bundle bundle = getIntent().getExtras();

        overridePendingTransition(0, 0); // disable the animation, faster

        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);    //Tine:  Show Status Bar

        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);

        setContentView(R.layout.confirm_ok_cancel_menu);

        Intent intent=getIntent();
        String dispmsg=intent.getStringExtra("pass_in_string");
        Log.i("dispmsg", dispmsg);

        TextView textView01;
        TextView textView02;
        TextView textView03;
        ImageView imageView;
        TextView tv_title_header;

        ///textView_txn = (TextView) findViewById(R.id.textView_txn);
        textView01 = (TextView) findViewById(R.id.msg_text_01);
        textView02 = (TextView) findViewById(R.id.msg_text_02);
        textView03 = (TextView) findViewById(R.id.msg_text_03);
        tv_title_header = (TextView) findViewById(R.id.tv_title_header);

        // Button
        buOK = (Button) findViewById(R.id.IPT_OKButton);
        buCancel = (Button) findViewById(R.id.IPT_CANCELButton);

        String[] dispmsginfo = dispmsg.split("\\|");
        int msgcnt = dispmsginfo.length;
        String image_str = "";

        System.out.println("msgcnt [" + msgcnt + "]");

        for (int inIdx = 0 ; inIdx < msgcnt; inIdx++)
        {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx)
            {
                case 0:
                    textView01.setText(dispmsginfo[inIdx]);
                    break;
                case 1:
                    textView02.setText(dispmsginfo[inIdx]);
                    break;
                case 2:
                    textView03.setText(dispmsginfo[inIdx]);
                    break;
                case 3:
                    tv_title_header.setText(dispmsginfo[inIdx]);
                    break;
                case 4: // Icon
                    image_str = dispmsginfo[inIdx];
                    System.out.println("image_str [" + image_str + "]");
                    imageView = (ImageView) findViewById(R.id.imageView);
                    wub_lib.ViewImageResourcesByType(image_str, imageView); // sidumili: added to call function in any java, code optimization
                    break;
                case 5: // IPT_CANCELButton Text
                    buCancel.setVisibility(View.VISIBLE);
                    if (dispmsginfo[inIdx].equals("NONE"))
                        buCancel.setVisibility(View.GONE);

                    buCancel.setText(dispmsginfo[inIdx]);
                    break;
                case 6: // IPT_OKButton Text

                    // auto adjust button width -- sidumili
                    if (buCancel.getVisibility() == View.GONE)
                    {
                        LinearLayout.LayoutParams btnParam = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT,1.0f);
                        btnParam.setMargins(10, 0, 10, 0);
                        buOK.setLayoutParams(btnParam);
                    }

                    buOK.setText(dispmsginfo[inIdx]);
                    break;
            }
        }

        /*Start timer*/
        getTimerRestart();

        buOK.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                final_string = "CONFIRM";

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        ConfirmOKCancelMenu.this.finish();
                    }
                }, inDelay);

                Log.i("xxxx", "buOK ConfirOKCancelMenu");

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
                    Log.i(TAG, "onClick: Exception error " + ex.getMessage());
                }


            }
        });

        buCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                final_string = "CANCEL";

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        ConfirmOKCancelMenu.this.finish();
                    }
                }, inDelay);

                Log.i("xxxx", "buCancel ConfirOKCancelMenu");

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
                    Log.i(TAG, "onClick: Exception error " + ex.getMessage());
                }

            }
        });

    }

    /**
     * 取消倒计时
     */
    public void getTimerCancel() {
        timer.cancel();
    }

    /**
     * 开始倒计时
     */
    public void getTimerRestart()
    {
        timer.start();
    }

    private CountDownTimer timer = new CountDownTimer(inTimeOut*1000, 1000) {

        @Override
        public void onTick(long millisUntilFinished) {
            Log.i("Timer", "Timer onTick");
        }

        @Override
        public void onFinish() {
            Log.i("Timer", "Timer onFinish");

            /*cancel timer first*/
            getTimerCancel();

            //etInputStr.clear();
            final_string = "TIME OUT";

            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    ConfirmOKCancelMenu.this.finish();
                }
            }, inDelay);

            Log.i("PATRICK", "Timeout ConfirmOKCancelMenu");

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
                Log.i(TAG, "onClick: Exception error " + ex.getMessage());
            }

        }
    };

    @Override
    protected void onResume() {
        super.onResume();

        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public void onBackPressed() {

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        final_string = null;
        mContext = null;

        //// SysApplication.getInstance().removeActivity(this);
    }

}
