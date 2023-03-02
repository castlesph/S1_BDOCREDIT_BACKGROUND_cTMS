package com.Source.S1_BDO.BDO.Trans;


import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.Global.Global;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.OperatorHolder;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

import static com.Source.S1_BDO.BDO.Main.MultiAP_SubAP.TAG;


public class LockScreen extends DemoAppActivity {

    RelativeLayout llmain;
    TextView tvheader1;
    TextView tvheader2;
    TextView tvheader3;
    TextView tvheader4;

    public static String final_string;

    public static TextView tv_main_NetworkCarrier;
    public static ImageView img_signalstrength;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate: LockScreen");
        // TODO 自动生成的方法存根
        super.onCreate(savedInstanceState);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN); // full screen
        setContentView(R.layout.lockscreen);

        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        //Intent intent=getIntent();
        //String dispmsg=intent.getStringExtra("pass_in_string");
        //Log.i("dispmsg", dispmsg);

        llmain= (RelativeLayout) findViewById(R.id.llmain);
        tvheader1 = (TextView) findViewById(R.id.tvheader1);
        tvheader2 = (TextView) findViewById(R.id.tvheader2);
        tvheader3 = (TextView) findViewById(R.id.tvheader3);
        tvheader4 = (TextView) findViewById(R.id.tvheader4);

        tv_main_NetworkCarrier = (TextView) findViewById(R.id.tv_main_NetworkCarrier);
        img_signalstrength = (ImageView) findViewById(R.id.img_signalstrength);

        String sCommMode = "";
        String[] pArrInfo = new String[100];
        String pInfo = getMerchantInfo();
        Log.i(TAG, "getMerchantInfo, pInfo="+pInfo);

        pArrInfo = pInfo.split(" \n");
        int msgcnt = pArrInfo.length;
        for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
            System.out.println("LockScreen::split msg [" + inIdx + "][" + pArrInfo[inIdx] + "]");
            switch (inIdx) {
                case 0:

                    break;
                case 1:
                    tvheader1.setText(pArrInfo[inIdx]);
                    break;
                case 2:
                    tvheader2.setText("MID: "+ (pArrInfo[inIdx].length() > 0 ? pArrInfo[inIdx] : "0000000000000000"));
                    break;
                case 3:
                    tvheader3.setText("TID: "+(pArrInfo[inIdx].length() > 0 ? pArrInfo[inIdx] : "00000000"));
                    break;
                case 4:
                    tvheader4.setText("<"+(pArrInfo[inIdx].length() > 0 ? pArrInfo[inIdx] : "Terminal App Version")+">");
                    break;
                case 5:
                    sCommMode = pArrInfo[inIdx];
                    break;
            }
        }

        img_signalstrength.setBackgroundResource(R.drawable.bar0);
        Log.i(TAG, "LockScreen: sCommMode="+sCommMode);

        int signal = 0;
        String Networknm = "";
        String ssid = "";
        //TelephonyManager telephonyManager;
        //OperatorHolder psListener;
        //OperatorHolder operatorHolder = new OperatorHolder(this);

        switch (sCommMode)
        {
            case "GPRS":
                OperatorHolder operatorHolder = new OperatorHolder(this);
                Networknm = operatorHolder.getOperatorName();
                //psListener = new OperatorHolder(this);
                //telephonyManager = (TelephonyManager)this.getSystemService(Context.TELEPHONY_SERVICE);
                //telephonyManager.listen(psListener, PhoneStateListener.LISTEN_SIGNAL_STRENGTHS);
                //signal = psListener.signalStrengthValue;
                //Log.i(ContentValues.TAG, "GetMenu,signal="+signal);
                //displaySignalStrength(signal);
                break;
            case "WIFI":
                WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
                WifiInfo info = wifiManager.getConnectionInfo();
                if (info.getSupplicantState() == SupplicantState.COMPLETED) {
                    ssid = info.getSSID().replace("\"", "");
                }
                Log.i(ContentValues.TAG, "wifi ssid=" + ssid);

                if (ssid.equals("<unknown ssid>") || (ssid.length() <= 0))
                    Networknm = "Not connected";
                else
                    Networknm = ssid;
                break;
        }

        Log.i(ContentValues.TAG, "LockScreen: Networknm="+Networknm);
        Log.i(ContentValues.TAG, "signal="+signal);
        tv_main_NetworkCarrier.setText(Networknm);

        llmain.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i(TAG, "onClick: llmain");
                final_string = "CONFIRM";
                Global.isLock = false;
                LockScreen.this.finish();

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

    }

    public static void displaySignalStrength(int pStrength)
    {
        Log.i(ContentValues.TAG, "displaySignalStrength: ");
        Log.i(ContentValues.TAG, "pStrength="+pStrength);

        img_signalstrength.setBackgroundResource(R.drawable.bar0);

        if (pStrength <= 25)
            img_signalstrength.setBackgroundResource(R.drawable.bar1);
        else if (pStrength <= 50)
            img_signalstrength.setBackgroundResource(R.drawable.bar2);
        else if (pStrength <= 75)
            img_signalstrength.setBackgroundResource(R.drawable.bar3);
        else if (pStrength <= 100)
            img_signalstrength.setBackgroundResource(R.drawable.bar4);
    }

    @Override
    protected void onResume() {
        super.onResume();

        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy: run");
        super.onDestroy();
        final_string = null;
        SysApplication.getInstance().removeActivity(this);
    }

    public native String getMerchantInfo();
}