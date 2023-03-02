package com.Source.S1_BDO.BDO.model;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.PowerManager;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.Trans.UserConfirmDCC;

public class ConfirmTotal extends DemoAppActivity {

    private Context mContext;

    private Button mButton;
    final Context c = this;

    EditText etInputStr;
    Button buOK;
    Button buCancel;

    TextView textView00;
    TextView textViewCount00;
    TextView textViewDetail00;

    TextView textView01;
    TextView textViewCount01;
    TextView textViewDetail01;

    TextView textView02;
    TextView textViewCount02;
    TextView textViewDetail02;

    TextView textView03;
    TextView textViewCount03;
    TextView textViewDetail03;

    TextView textView04;
    TextView textViewCount04;
    TextView textViewDetail04;

    TextView textView_txn;
	
    TextView textView_cur;

    TextView tv_tap;
    LinearLayout linearLayout_tap;

    Button btn_back;

    Double dAmount;
    int inResult;
    String stResult;

    boolean androidThinking;
    public static String final_string;
    public static String input_type;

    private int inTimeOut = 30;

    protected PowerManager.WakeLock mWakeLock;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
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

        setContentView(R.layout.confirm_total);

        /* This code together with the one in onDestroy()
         * will make the screen be always on until this Activity gets destroyed. */
        final PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "S1_QRPAY:AAA-ConfirmTotal>>WAKE_LOCK");
        this.mWakeLock.acquire();

        Intent intent=getIntent();
        String dispmsg=intent.getStringExtra("pass_in_string");
        Log.i("dispmsg", dispmsg);

        textView_txn = (TextView) findViewById(R.id.textView_txn);	
        textView_cur = (TextView) findViewById(R.id.textView_cur);

        textView00 = (TextView) findViewById(R.id.msg_text_00);
        textViewCount00 = (TextView) findViewById(R.id.msg_count_00);
        textViewDetail00 = (TextView) findViewById(R.id.msg_detail_00);

        textView01 = (TextView) findViewById(R.id.msg_text_01);
        textViewCount01 = (TextView) findViewById(R.id.msg_count_01);
        textViewDetail01 = (TextView) findViewById(R.id.msg_detail_01);

        textView02 = (TextView) findViewById(R.id.msg_text_02);
        textViewCount02 = (TextView) findViewById(R.id.msg_count_02);
        textViewDetail02 = (TextView) findViewById(R.id.msg_detail_02);

        textView03 = (TextView) findViewById(R.id.msg_text_03);
        textViewCount03 = (TextView) findViewById(R.id.msg_count_03);
        textViewDetail03 = (TextView) findViewById(R.id.msg_detail_03);

        textView04 = (TextView) findViewById(R.id.msg_text_04);
        textViewCount04 = (TextView) findViewById(R.id.msg_count_04);
        textViewDetail04 = (TextView) findViewById(R.id.msg_detail_04);

        btn_back = (Button) findViewById(R.id.btn_back);

        String[] dispmsginfo = dispmsg.split("\\|");
		int msgcnt = dispmsginfo.length;

		System.out.println("msgcnt [" + msgcnt + "]");

        for (int inIdx = 0 ; inIdx < msgcnt; inIdx++)
        {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx)
            {
                case 0:
                    textView_txn.setText(dispmsginfo[inIdx]);
                    break;
                case 1:
                    //textView00.setText(dispmsginfo[inIdx]);
                    break;
				case 2:
                    textView01.setText(dispmsginfo[inIdx]);
					break;
                case 3:
                    textViewCount01.setText(dispmsginfo[inIdx]);
                    break;
                case 4:
                    textViewDetail01.setText(dispmsginfo[inIdx]);
                    break;
                case 5:
                    textView02.setText(dispmsginfo[inIdx]);
                    break;
                case 6:
                    textViewCount02.setText(dispmsginfo[inIdx]);
                    break;
                case 7:
                    textViewDetail02.setText(dispmsginfo[inIdx]);
                    break;
                case 8:
                    textView03.setText(dispmsginfo[inIdx]);
                    break;
                case 9:
                    textViewCount03.setText(dispmsginfo[inIdx]);
                    break;
                case 10:
                    textViewDetail03.setText(dispmsginfo[inIdx]);
                    break;
                case 11:
                    textView04.setText(dispmsginfo[inIdx]);
                    break;
                case 12:
                    textViewCount04.setText(dispmsginfo[inIdx]);
                    break;
                case 13:
                    textViewDetail04.setText(dispmsginfo[inIdx]);
                    break;

            }
        }

        buOK = (Button) findViewById(R.id.IPT_OKButton);
        buCancel = (Button) findViewById(R.id.IPT_CancelButton);

        /*Start timer*/
        getTimerRestart();

        buOK.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                //dAmount = Double.parseDouble(etAmount.getText().toString());
                final_string = "CONFIRM";
                //final_string = final_string.replaceAll("[$,.]", "");

                //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));
                ConfirmTotal.this.finish();

                Log.i("PATRICK", "Get Amount KeyBoard buOK");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }

            }
        });

        buCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                final_string = "CANCEL";

                //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));
                ConfirmTotal.this.finish();

                Log.i("PATRICK", "Get Amount KeyBoard buCancel");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

        btn_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                final_string = "CANCEL";

                //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));
                ConfirmTotal.this.finish();

                Log.i("PATRICK", "KeyBoard btn_back");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

        // tap TextView -- sidumili
        tv_tap = (TextView) findViewById(R.id.tv_tap);
        tv_tap.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*cancel timer first*/
                getTimerCancel();

                final_string = "CONFIRM";

                ConfirmTotal.this.finish();

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

        // tap LinearLayout -- sidumili
        linearLayout_tap = (LinearLayout) findViewById(R.id.linearLayout_tap);
        linearLayout_tap.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*cancel timer first*/
                getTimerCancel();

                final_string = "CONFIRM";

                ConfirmTotal.this.finish();

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

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

            //etInputStr.clear();
            final_string = "TIME OUT";

            //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));

            Log.i("PATRICK", "Timeout UserInputString");
            ConfirmTotal.this.finish();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };

    @Override
    public void onBackPressed() {

    }

    @Override
    protected void onDestroy() {
        this.mWakeLock.release();
        super.onDestroy();
        final_string = null;
        mContext = null;
        // SysApplication.getInstance().removeActivity(this);
    }

}

