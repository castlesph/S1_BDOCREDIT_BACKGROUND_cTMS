package com.Source.S1_BDO.BDO.Trans;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

import java.io.File;

public class UserConfirmDetails extends DemoAppActivity {

    String issuerlogo_path;

    TextView textView_txn;
    TextView textView_issuer;
    TextView textView_cardnum;
    TextView textView_item1;
    ImageView imageView;
    Button btn_can;
    Button btn_proceed;
	boolean fBlankLogo = false;

    public static String final_string;

    private int inTimeOut = 30;
    Bitmap txnlogo = null;
    File issuerlogo_bmp_file = null;

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

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.confirm_details);
        Intent intent = getIntent();
        String dispmsg = intent.getStringExtra("pass_in_string");
        Log.i("dispmsg", dispmsg);

        textView_cardnum = (TextView) findViewById(R.id.msg_text_04);
        textView_item1 = (TextView) findViewById(R.id.msg_text_03);
        textView_txn = (TextView) findViewById(R.id.textView_txn);
        textView_issuer = (TextView) findViewById(R.id.textView_issuer);
        imageView = (ImageView) findViewById(R.id.issuer_logo);

        btn_can = (Button) findViewById(R.id.btn_can);
        btn_proceed = (Button) findViewById(R.id.btn_proceed);

        String[] dispmsginfo = dispmsg.split("\\|");
        int msgcnt = dispmsginfo.length;

        //System.out.println("msgcnt [" + msgcnt + "]");

        for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
            //System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx) {
                case 0:
                    textView_txn.setText(dispmsginfo[inIdx]);
                    break;
                case 1:
                    textView_item1.setText(dispmsginfo[inIdx]);
                    break;
                case 2:
                    textView_cardnum.setText(dispmsginfo[inIdx]);
                    break;
                case 3:

					Log.i("saturn", "saturn "+dispmsginfo[inIdx]);

					if (dispmsginfo[inIdx].equals("flt.bmp")){
						Log.i("saturn", "saturn 2"+dispmsginfo[inIdx]);
						fBlankLogo = true;
						break;

				    }
                     
                    issuerlogo_path = "/data/data/pub/"+dispmsginfo[inIdx];
                    issuerlogo_bmp_file = new File(issuerlogo_path);
                    txnlogo = BitmapFactory.decodeFile(issuerlogo_bmp_file.getAbsolutePath());

                    imageView.setImageBitmap(txnlogo);
                    break;
				  
                case 4:
                    textView_issuer.setText(dispmsginfo[inIdx]);
                    break;
            }
        }

        /*Start timer*/
        getTimerRestart();

        btn_proceed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                /*cancel timer first*/
                getTimerCancel();

                //dAmount = Double.parseDouble(etAmount.getText().toString());
                final_string = "CONFIRM";
                //final_string = final_string.replaceAll("[$,.]", "");

                //startActivity(new Intent(GetAmount.this,MainActivity.class));
                UserConfirmDetails.this.finish();

                //Log.i("TINE", "UserConfirmCard Proceed");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }

            }
        });

        btn_can.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                getTimerCancel();

                final_string = "CANCEL";
                //Tine:  try to disable going back to MainActivity
                //startActivity(new Intent(UserConfirmCard.this,MainActivity.class));
                UserConfirmDetails.this.finish();

                //Log.i("PATRICK", "Get Amount KeyBoard btn_can");

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
            //Log.i("Timer", "Timer onFinish");

            //etInputStr.clear();
            final_string = "TIME OUT";

            //startActivity(new Intent(UserConfirmCard.this, MainActivity.class));

            //Log.i("PATRICK", "Timeout UserInputString");
            UserConfirmDetails.this.finish();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };



    @Override
    protected void onDestroy() {
        super.onDestroy();
        final_string = null;
		if (fBlankLogo == false){
			Log.i("saturn", "saturn ecycle logo");
        	txnlogo.recycle();
		}
        txnlogo = null;
        issuerlogo_bmp_file = null;
        // SysApplication.getInstance().removeActivity(this);
    }

}
