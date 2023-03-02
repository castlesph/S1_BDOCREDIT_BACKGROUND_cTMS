package com.Source.S1_BDO.BDO.Trans;

import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

public class UserCancelUI extends DemoAppActivity {

    TextView textView1;
    Button btn_userCan;

    public static String final_string;

    private int inTimeOut = 20;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);
        super.onCreate(savedInstanceState);
        Bundle bundle = getIntent().getExtras();

        overridePendingTransition(0, 0); // disable the animation, faster

        setContentView(R.layout.activity_cancel);
        Intent intent = getIntent();
        String dispmsg = intent.getStringExtra("pass_in_string");
        Log.i("dispmsg", dispmsg);

        String[] dispmsginfo = dispmsg.split("\\|");
        int msgcnt = dispmsginfo.length;

        System.out.println("msgcnt [" + msgcnt + "]");

        for (int inIdx = 0 ; inIdx < msgcnt; inIdx++)
        {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx)
            {
//                case 0:
//                    textView01.setText(dispmsginfo[inIdx]);
//                    break;
//                case 1:
//                    textView02.setText(dispmsginfo[inIdx]);
//                    break;
//                case 2:
//                    textView03.setText(dispmsginfo[inIdx]);
//                    break;
                default:
                    break;
            }
        }

        textView1 = (TextView) findViewById(R.id.textView_cur);
        btn_userCan = (Button) findViewById(R.id.btn_userCan);

        textView1.setText(dispmsg);

        btn_userCan.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                getTimerCancel();

                final_string = "CANCEL";
                //Tine:  try to disable going back to MainActivity
                UserCancelUI.this.finish();

                Log.i("PATRICK", "UserCancelUI btn_userCan");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }

        });


    }

    public void getTimerCancel(){
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

            //etInputStr.clear();
            final_string = "TO";

            //startActivity(new Intent(UserCancelUI.this, MainActivity.class));

            Log.i("PATRICK", "Timeout UserInputString");
            UserCancelUI.this.finish();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };

    @Override
    public void onBackPressed() {
        UserCancelUI.this.finish();

        Log.i("PATRICK", "Back");

        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        final_string = null;
        // SysApplication.getInstance().removeActivity(this);
    }

}
