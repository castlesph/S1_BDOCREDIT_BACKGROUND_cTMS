package com.Source.S1_BDO.BDO.Trans;

import android.content.Intent;
import android.graphics.Bitmap;
import android.media.ThumbnailUtils;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.HelloJni;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

import static android.content.ContentValues.TAG;

public class CardEntry extends DemoAppActivity {

    Button mCancelButton;
    Button man_entry;
    TextView textViewmsg;
    TextView textView_dtxn;
    TextView textView_cur;
    TextView textViewAmount;

    public static String cardentry_final_string;
    private int inTimeOut = 30;
    private int inRet=1;

    String szTotalAmount = "";
    String szOtherAmt = "";
    String szTransType = "";
    String szCatgCode = "";
    String szCurrCode = "";
    String szWAVE_status="";
    String szMSR_status="";
    String szICC_status="";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate: CardEntry");

        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);

        super.onCreate(savedInstanceState);
        Bundle bundle = getIntent().getExtras();

        overridePendingTransition(0, 0); // disable the animation, faster
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);    //Tine:  Show Status Bar

        setContentView(R.layout.activity_cardentry);
        Intent intent = getIntent();
        String dispmsg = intent.getStringExtra("pass_in_string");

        //mClearButton = (Button) findViewById(R.id.clear_button);
        mCancelButton = (Button) findViewById(R.id.btn_can);
        man_entry = (Button) findViewById(R.id.man_entry);

        String[] dispmsginfo = dispmsg.split("\\|");
        int msgcnt = dispmsginfo.length;

        textViewmsg = (TextView) findViewById(R.id.textView_msg);
        textView_dtxn = (TextView) findViewById(R.id.textView_dtxn);
        textView_cur = (TextView) findViewById(R.id.textView_cur);
        textViewAmount = (TextView) findViewById(R.id.textViewAmount);

        //to do
        for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
            System.out.println("CardEntry->split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx) {
                case 0:
                    textView_dtxn.setText(dispmsginfo[inIdx]);
                    break;
                case 1:
                    textViewmsg.setText(dispmsginfo[inIdx]);
                    break;
                case 2:
                    textView_cur.setText(dispmsginfo[inIdx]);
                    break;
                case 3:
                    szTotalAmount = dispmsginfo[inIdx];
                    textViewAmount.setText(dispmsginfo[inIdx]);
                    break;
                case 4:
                    szOtherAmt = dispmsginfo[inIdx];
                    break;
                case 5:
                    szTransType = dispmsginfo[inIdx];
                    break;
                case 6:
                    szCatgCode = dispmsginfo[inIdx];
                    break;
                case 7:
                    szCurrCode = dispmsginfo[inIdx];
                    break;
            }
        }


        getTimerRestart();

        FuncKeyCan();
        FuncKeyManualEntry();

        szWAVE_status = usCTOSS_CtlsV3Trans_inJava(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);

    }
        public void FuncKeyCan() {
            mCancelButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    getTimerCancel();
                    //startActivity(new Intent(CardEntry.this, MainActivity.class));
                    cardentry_final_string = "CANCEL";
                    CardEntry.this.finish();

                    // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                    synchronized (MainActivity.LOCK) {
                        MainActivity.LOCK.setCondition(true);
                        MainActivity.LOCK.notifyAll();

                    }
                }
            });
        }

        public void FuncKeyManualEntry() {
            man_entry.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {

                    //startActivity(new Intent(CardEntry.this, MainActivity.class));
                    cardentry_final_string = "TO";
                    CardEntry.this.finish();

                    // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                    synchronized (MainActivity.LOCK) {
                        MainActivity.LOCK.setCondition(true);
                        MainActivity.LOCK.notifyAll();

                    }
                }
            });
        }

    public void getTimerCancel() {
        timer.cancel();
    }

    public void getTimerRestart()
    {
        timer.start();
    }

    //private CountDownTimer timer = new CountDownTimer(inTimeOut*1000, 1000) {
    private CountDownTimer timer = new CountDownTimer(inTimeOut*1000, 1000) {
        @Override
        public void onTick(long millisUntilFinished) {
            Log.i("Timer", "Timer onTick");
            //String szMSR_status="";
            //String szICC_status="";
            //String szWAVE_status="";
            setContentView(R.layout.activity_cardentry);
            szMSR_status = CTOSS_MSRRead_inJava();
            szICC_status = CTOSS_SCStatus_inJava();
            //szWAVE_status = usCTOSS_CtlsV3Trans_inJava(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
            Log.d("TINE", szMSR_status);
            if(szMSR_status.equals("MSR_OK"))
            {
                getTimerCancel();
                cardentry_final_string = "CARD_ENTRY_MSR";
                CardEntry.this.finish();

                Log.i("Tine", "CardEntry MSRRead OK");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
            else if(szICC_status.equals("ICC_OK"))
            {
                getTimerCancel();
                cardentry_final_string = "CARD_ENTRY_ICC";
                CardEntry.this.finish();

                Log.i("Tine", "CardEntry ICCRead OK");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
            else if(szWAVE_status.equals("WAVE_OK"))
            {
                getTimerCancel();
                cardentry_final_string = "CARD_ENTRY_WAVE";
                CardEntry.this.finish();

                Log.i("Tine", "CardEntry CTLSRead OK");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
            else {
                cardentry_final_string = "CARD_ENTRY_NO";
            }
        }

        @Override
        public void onFinish() {
            Log.i("Timer", "Timer onFinish");

            //etInputStr.clear();
            cardentry_final_string = "TIME OUT";

            //startActivity(new Intent(AmountEntryActivity.this, MainActivity.class));

            Log.i("PATRICK", "Timeout CardEntry");
            CardEntry.this.finish();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };

    @Override
    public void onBackPressed() {
        //getTimerCancel();

        cardentry_final_string = "CANCEL";
        //startActivity(new Intent(AmountEntryActivity.this, MainActivity.class));
        CardEntry.this.finish();

        Log.i("PATRICK", "Get Amount KeyBoard buCancel");
        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        cardentry_final_string = null;
        // SysApplication.getInstance().removeActivity(this);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    //USHORT CTOS_MSRRead(BYTE*baTk1Buf, USHORT*pusTk1Len, BYTE*baTk2Buf, USHORT*pusTk2Len, BYTE*baTk3Buf, USHORT*pusTk3Len);
    public native String CTOSS_MSRRead_inJava();
    public native String CTOSS_SCStatus_inJava();
    public native String usCTOSS_CtlsV3Trans_inJava(String szTotalAmount,String szOtherAmt,String szTransType,String szCatgCode,String szCurrCode);


}
