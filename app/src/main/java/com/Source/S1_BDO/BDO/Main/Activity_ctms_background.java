package com.Source.S1_BDO.BDO.Main;

import android.app.ActivityManager;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.BdoApplication;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

import org.json.JSONObject;

import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.List;
import java.util.Locale;
import java.util.concurrent.TimeUnit;

import CTOS.CtCtms;
import CTOS.CtSystem;
import castles.ctms.module.commonbusiness.DownloadInfo;
import castles.ctms.module.commonbusiness.IStatusCallback;
import castles.ctms.module.commonbusiness.PackageInfo;

public class Activity_ctms_background extends DemoAppActivity {
    private static final String TAG = "Activity_ctms_background";
    private CtCtms ctCtms;
    private int installNum = 0;
    private int totalNum = 0;
    private TextView tvStatus;
    private ProgressBar progressBar;
    private boolean downloaded = false;
    private String currentDownloadFileName;
    public String strPreDownloadfileName = "";
    CtSystem ctSystem = new CtSystem();
    private String strSn = null;
    final Handler handler = new Handler();
    private final boolean CreatedCtmsObj = true;
    byte bootupConnectFlag = 0;
    private boolean bootflag = false;//This flag is used for resume download, eg: if got 2 app, one app downloaded, terminal restart, after restart terminal should auto do ctms call
    private boolean isRegisteredCallback = false;
    private boolean backUpDbFlag = false;

    private String HostIP = null;
    private int HostPort = 0;
    private String strTlsHostIP = null;
    private int TlsHostPort = 0;
    private int strMode = 0;
    private boolean fGotFileDownloadFailed = false;//20210127 for CTMS
    private int ConnectFailedTimes = 0;
    private int inTimeOut = 10;
    boolean isCounterRunning  = false;
    private int inDLFullSize = 0;
    private boolean isReboot = false;
    private int strCommMode = 0;
    private long inSleep = 100;
    private long inPostDelay = 2000;
    private boolean checkInstallFlag = false;
    private boolean isLowBattery = false;

    // connection information
    TextView tv_ctms_sn;
    TextView tv_ctms_tcp;
    TextView tv_ctms_tls;
    TextView tv_ctms_mode;
    TextView tv_comm_mode;

    static {
        System.loadLibrary("crypto");
    }

    static {
        System.loadLibrary("ssl");
    }

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    //for bind service?
/*
    private ServiceConnection connection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.i(TAG, "onServiceConnected: " + name);
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            Log.i(TAG, "onServiceDisconnected: " + name);
        }
    };
*/

    private void vdServiceList(final String className) {
        ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningServiceInfo> info = activityManager.getRunningServices(Integer.MAX_VALUE);
        if (info == null || info.size() == 0) {
            Log.i(TAG, "no service: ");
            return ;
        }
        for (ActivityManager.RunningServiceInfo aInfo : info) {
            Log.i(TAG, "aInfo.service" + aInfo.service);
            if (className.equals(aInfo.service.getClassName())) {
                Log.i(TAG, "found: found");
//                return true;
            }
        }
//        return false;
    }
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        Log.i(TAG, "onCreate: ");
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        super.onCreate(savedInstanceState);
        Bundle bundle = getIntent().getExtras();
        setContentView(R.layout.activity_ctms_update);
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);

        tvStatus = (TextView) findViewById(R.id.tv_ctms_msg);
        progressBar = (ProgressBar) findViewById(R.id.downloadStatusPb);
        progressBar.setVisibility(View.GONE);

        // connection information
        tv_ctms_sn = (TextView) findViewById(R.id.tv_ctms_sn);
        tv_ctms_tcp = (TextView) findViewById(R.id.tv_ctms_tcp);
        tv_ctms_tls = (TextView) findViewById(R.id.tv_ctms_tls);
        tv_ctms_mode = (TextView) findViewById(R.id.tv_ctms_mode);
        tv_comm_mode = (TextView) findViewById(R.id.tv_label);

        Intent intent = getIntent();
        if(intent != null) {
            //20210127 for CTMS, can remove this flag in the 2nd phase, because we don't need to do Resume download by app level, can run it background
//            bootflag = intent.getBooleanExtra("REBOOT", false);
            Log.i(TAG, "bootflag: " + bootflag);

            checkInstallFlag = intent.getBooleanExtra("CHECKINSTALL_FLAG", false);
            Log.i(TAG, "checkInstallFlag: " + checkInstallFlag);
        }

        String dispmsg = intent.getStringExtra("pass_in_string");
        System.out.println("dispmsg="+dispmsg);
        String[] dispmsginfo = dispmsg.split("\\|");
        int msgcnt = dispmsginfo.length;

        for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx) {
                case 0:
                    strSn = dispmsginfo[inIdx];
                    break;
                case 1:
                    //TCP
                    HostIP = dispmsginfo[inIdx];
                    break;
                case 2:
                    //TCP
                    HostPort = Integer.parseInt(dispmsginfo[inIdx]);
                    break;
                case 3:
                    //TLS
                    strTlsHostIP = dispmsginfo[inIdx];
                    break;
                case 4:
                    //TLS
                    TlsHostPort = Integer.parseInt(dispmsginfo[inIdx]);
                    break;
                case 5:
                    strMode = Integer.parseInt(dispmsginfo[inIdx]);
                    break;
                case 6:
                    strCommMode = Integer.parseInt(dispmsginfo[inIdx]);
                    break;

            }
        }

        System.out.println("strSn="+strSn);
        System.out.println("HostIP="+HostIP);
        System.out.println("HostPort="+HostPort);
        System.out.println("strTlsHostIP="+strTlsHostIP);
        System.out.println("TlsHostPort="+TlsHostPort);
        System.out.println("strMode="+strMode);
        System.out.println("strCommMode="+strCommMode);

        connectionInfo();
/*
        if(CreatedCtmsObj) {
            ctCtms = BdoApplication.getCtCtmsObj();
            Log.i(TAG, "CreatedCtmsObj: " + (ctCtms == null));
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    Log.i(TAG, "run: registerCallback");
                    ctCtms.registerCallback(statusCalback);
                    isRegisteredCallback = true;
                }
            }, 1000);
        }
        else {

            ctCtms = new CtCtms(this);
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    Log.i(TAG, "run: registerCallback");
                    ctCtms.registerCallback(statusCalback);
                    isRegisteredCallback = true;
                }
            }, 3000);
        }
*/
        if(CreatedCtmsObj) {
            ctCtms = BdoApplication.getCtCtmsObj();
            Log.i(TAG, "CreatedCtmsObj: " + (ctCtms == null));

            try {
                Thread.sleep(inSleep);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            Log.i(TAG, "run: registerCallback");
            ctCtms.registerCallback(statusCalback);
            isRegisteredCallback = true;
        }
        else {

            ctCtms = new CtCtms(this);

            try {
                Thread.sleep(inSleep);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            Log.i(TAG, "run: registerCallback");
            ctCtms.registerCallback(statusCalback);
            isRegisteredCallback = true;
        }

/*
        try {
            strSn = ctSystem.getFactorySNEx();
            Log.i(TAG, "getFactorySNEx: " + strSn);
        } catch (CtSystemException e) {
            e.printStackTrace();
            Log.e(TAG, "CtSystemException: " + e );
        }

 */
        //TCP
//        String HostIP = "staging-pos.castechcentre.com";
        //String HostIP = "pos.uat.castechcentre.com";
//        int HostPort = 80;
//        String strTlsHostIP = "staging.castechcentre.com";
//        String strTlsHostIP = "ctms.uat.castechcentre.com";
//        int TlsHostPort = 443;
        byte ctmsFlag = 1;
        Log.i(TAG, "getCTMSStatus: " + ctCtms.getCTMSStatus());
        Log.i(TAG, "setCTMSEnable = "+ ctmsFlag);
        ctCtms.setCTMSEnable(ctmsFlag);

        Log.i(TAG, "setCM_Mode = "+ strMode);
        ctCtms.setCM_Mode((byte) strMode);//1 tcp, 2 tls

        //20210127 for CTMS
        //install mode can set it here or can set it from CTMS server with CTMS agent CSV file(rebootInstallMode), must use flag install mode
        int RebootInstallMode = ctCtms.getRebootInstallMode();//0 - reboot install if got file(MUST not use this setting), 1- only getRebootInstallFlag is true, then install file,after install done, agent set setRebootInstallFlag to false
        boolean RebootInstallAllow = ctCtms.getRebootInstallFlag();
        Log.i(TAG, "getRebootInstallMode: " + RebootInstallMode);
        Log.i(TAG, "getRebootInstallFlag: " + RebootInstallAllow);
        ctCtms.setRebootInstallMode(1);//0 force reboot install, means very reboot just install if any, 1 flag to control reboot install, only getRebootInstallFlag to true, then install during boot up
        //set all config with Json format
        {
            String strOriJsonConfig;
            JSONObject jsonObject;
            try {
//                                String strTCPConfig = ctCtms.getConfig(CtCtms.CONFIG_COMMUNICATE_TCP);
//                                Log.i(TAG, "strTCPConfig: " + strTCPConfig);

                String str = ctCtms.getAllConfig();
                Log.i(TAG, "getAllConfig: " + str);
                jsonObject = new JSONObject(str);
                Log.i(TAG, "get all config: " + jsonObject.toString());
                jsonObject.put("Serial_Number", strSn);

                //20210127 for CTMS, use setBootConnectEnable set boot connect, with new Fw, now we can turn on boot up connect if u need, because app can control the install part
                ctCtms.setBootConnectEnable(bootupConnectFlag);
                Log.i(TAG, "ctCtms.getBootConnectStatus(): " + ctCtms.getBootConnectStatus());

//                Log.i(TAG, "ctCtms.getBootConnectStatus(): " + ctCtms.getBootConnectStatus());//setBootConnectEnable

                if (strMode == 2)
                {
                    Log.i(TAG, "TLS configuration.....");
                    //TLS config
                    JSONObject jsonObjTLS = jsonObject.getJSONObject("TLS");
                    Log.i(TAG, "get jsonObjTLS config: " + jsonObjTLS.toString());
                    jsonObjTLS.put("Host_IP", strTlsHostIP);
                    jsonObjTLS.put("Host_Port", TlsHostPort);
                }
                else
                {
                    Log.i(TAG, "TCP configuration.....");
                    //TCP config
                    JSONObject jsonObjTCP = jsonObject.getJSONObject("TCP");
                    Log.i(TAG, "get jsonObjTCP config: " + jsonObjTCP.toString());
                    jsonObjTCP.put("Host_IP", HostIP);
                    jsonObjTCP.put("Host_Port", HostPort);
                }

                Log.i(TAG, "set all config: " + jsonObject.toString());
                ctCtms.setAllConfig(jsonObject.toString());

            } catch (Exception e) {
                Log.e(TAG, "Exception: " + e);
                e.printStackTrace();
            }

            //        UpdateNow(ctCtms);

            new Thread(new Runnable() {
                @Override
                public void run() {
                    Log.i(TAG, "run TMS update thread: ");
                    /*Start timer*/
                    getTimerRestart();
                    UpdateNow(ctCtms);
                }
            }).start();

        }

		/*
		// startActivity(new Intent(Activity_ctms_update.this, MainActivity.class));	
		// Activity_ctms_update.this.finish();

		
        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();
        }	
		*/			
        Log.i(TAG, "onCreate exit: ");
    }

    /**
     * ?????
     */

    public void getTimerCancel()
    {
        timer.cancel();
    }

    /**
     * ?????
     */

    public void getTimerRestart()
    {
        timer.start();
    }

    private CountDownTimer timer = new CountDownTimer(inTimeOut*1000, 1000) {

        @Override
        public void onTick(long millisUntilFinished) {
            Log.i(TAG, "Timer onTick " + millisUntilFinished);
        }

        @Override
        public void onFinish() {
            Log.i("Timer", "Activity_ctms_update Timer onFinish");

            vdRebootNow();
			//Activity_ctms_update.this.finish();

            /*
	        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
	        synchronized (MainActivity.LOCK) {
	            MainActivity.LOCK.setCondition(true);
	            MainActivity.LOCK.notifyAll();
	        }
             */
        }
    };


    @Override
    public void onBackPressed() {
        Log.i(TAG, "onBackPressed: ");
        super.onBackPressed();
        finish();
    }

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy:downloaded=" + downloaded);
        Log.i(TAG, "onDestroy:isCounterRunning=" + isCounterRunning);
        Log.i(TAG, "onDestroy:isRegisteredCallback=" + isRegisteredCallback);

        if(isCounterRunning ){
            isCounterRunning = false;
            //getTimerCancel();
        }

        Log.i(TAG, "onDestroy:1");

        if(isRegisteredCallback) {
//            ctCtms.unbindService(Activity_ctms_update.this);
            ctCtms.unregisterCallback(statusCalback);//if not added, show ServiceConnectionLeaked
            statusCalback = null;
            ctCtms = null;

        }
        Log.i(TAG, "onDestroy:2");
        if (!downloaded) {
            Log.i(TAG, "NO UPDATES: ");
            vdUpdateDownloadStatus("NO UPDATES");//no use, other activity already in front
        }

        Log.i(TAG, "onDestroy:3");

        super.onDestroy();
        Intent intent = new Intent();
        Bundle bundle = new Bundle();
        intent.setClass(Activity_ctms_background.this, MainActivity.class);
        startActivity(intent);
        this.finish();
        Log.i(TAG, "onDestroy:4");
    }

    private String UpdateNow(CtCtms ctCtms) {
        Log.i(TAG, "UpdateNow: ");
        String strConnectTime = null;
        int status = 0;
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd-HH:mm");
        Calendar connectTime = ctCtms.getConnectTime();
        Calendar triggerTime = ctCtms.getTrigger();
        Log.i(TAG, " when the terminal connects to the server, getConnectTime: " + connectTime.getTime());
        Log.i(TAG, "Getting the trigger time setting, triggerTime: " + triggerTime.getTime());
        Calendar activeTime = ctCtms.getActiveTime();
        Log.i(TAG, "Getting the active time setting, activeTime: " + activeTime.getTime());

        if(isCounterRunning ){
            isCounterRunning = false;
            //getTimerCancel();
        }

		/*Start timer*/
        if( !isCounterRunning ){
            isCounterRunning = true;
            //getTimerRestart();
        }

/*        ctCtms.setActiveMode(CtCtms.OPTION_MANUAL_CONTROL);
        if(status == 0){
            Log.d(TAG, "setActiveMode ok: ");
        }
        Log.i(TAG, "getDebugStatus: " + ctCtms.getDebugStatus() + " getBootConnectStatus:" + ctCtms.getBootConnectStatus());

        status = ctCtms.getTriggerStatus();
        if(status == 0){
            Log.d(TAG, "disabled ");
        } else if(status == 1) {
            Log.d(TAG, "Enable ");
        } else {
            Log.w(TAG, "failure: ");
        }

        ctCtms.setTriggerEnable(CtCtms.OPTION_DISABLE);//CTMS will follow trigger time to connect to CTMS serve*/

        Log.i(TAG, "call UpdateImmediately: ");
        status = ctCtms.UpdateImmediately();
        Log.i(TAG, "after UpdateImmediately status: " + status);
        if(status == 0) {
            strConnectTime = dateFormat.format(connectTime.getTime());
        } else {
            strConnectTime = "Connect failed";
        }
        Log.i(TAG, "strConnectTime: " + strConnectTime);
        return strConnectTime;
    }

    private void vdUpdateDownloadStatus (final String msg) {
        Log.i(TAG, "vdUpdateDownloadStatus delay 1 seconds: " + msg);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                tvStatus.setText(msg);
            }
        });

/*        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }*/
    }
    private void vdUpdateDownloadProcessBar (final int progress) {
        Log.i(TAG, "vdUpdateDownloadProcessBar: " + progress);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                progressBar.setVisibility(View.VISIBLE);
                progressBar.setProgress(progress);
            }
        });
    }

    public void vdFinishBackgroundActivity() {
        Log.i(TAG, "vdFinishBackgroundActivity: ");
        Activity_ctms_background.this.finish();
        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();
        }
    }

    public void DisplayDiaglocBox() {
        Log.i(TAG, "DisplayDiaglocBox: ");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog dialog = new AlertDialog.Builder(Activity_ctms_background.this)
                        .setTitle("CTMS UPDATE")
                        .setMessage("Pls charge your temrinal to update POS app")
//                .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
//                    @Override
//                    public void onClick(DialogInterface dialog, int which) {
//                        Log.i(TAG, "onClick: " + which);
//                    }
//                })
                        .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                Log.i(TAG, "onClick: " + which);
//                                vdRebootNow();
                                vdFinishBackgroundActivity();
                            }
                        })
                        .create();

                dialog.setOnShowListener(new DialogInterface.OnShowListener() {
                    private static final int AUTO_DISMISS_MILLIS = 15000;//15 seconds
                    @Override
                    public void onShow(final DialogInterface dialog) {
                        final Button defaultButton = ((AlertDialog) dialog).getButton(AlertDialog.BUTTON_NEGATIVE);
                        final CharSequence negativeButtonText = defaultButton.getText();
                        Log.i(TAG, "onShow negativeButtonText: " + negativeButtonText.toString());
                        new CountDownTimer(AUTO_DISMISS_MILLIS, 100) {
                            @Override
                            public void onTick(long millisUntilFinished) {
                                defaultButton.setText(String.format(
                                        Locale.getDefault(), "%s (%d)",
                                        negativeButtonText,
                                        TimeUnit.MILLISECONDS.toSeconds(millisUntilFinished) + 1 //add one so it never displays zero
                                ));
                            }
                            @Override
                            public void onFinish() {
                                Log.i(TAG, "onFinish dismiss: ");
                                if (((AlertDialog) dialog).isShowing()) {
                                    dialog.dismiss();
//                                    vdRebootNow();
                                    vdFinishBackgroundActivity();
                                }
                            }
                        }.start();
                    }
                });
                dialog.show();

            }
        });

    }

    private void vdRebootNow() {
        Log.i(TAG, "vdRebootNow: ");
        Log.i(TAG, "downloaded="+downloaded);
        Log.i(TAG, "isReboot="+isReboot);

        if (!isReboot)
        {
            vdUpdateDownloadStatus("No update");
            progressBar.setVisibility(View.INVISIBLE);

            handler.postDelayed(new Runnable() {
                @Override
                public void run() {

                    Activity_ctms_background.this.finish();

                    // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                    synchronized (MainActivity.LOCK) {
                        MainActivity.LOCK.setCondition(true);
                        MainActivity.LOCK.notifyAll();
                    }
                }
            }, inPostDelay);

            return;
        }
        else // Reboot terminal
        {
            CtSystem system = new CtSystem();
            byte bmode = 0;// shutdown
            vdUpdateDownloadStatus("Rebooting...");
            try {
                Log.i(TAG, "do vdRebootNow..: ");
                Log.d("Rebooting-TAG", String.format("return = %X", system.shutdown(bmode)));
            } catch (Exception e) {
                Log.d(TAG, "SHUTDOWN encountered ERROR");
                e.printStackTrace();
            }
        }

    }

    private void setInstallFlag() {
        if (checkInstallFlag) {
            SharedPreferences wmbPreference = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());

            SharedPreferences.Editor editor = wmbPreference.edit();
            editor.putBoolean("CHECKINSTALL", true);
            editor.commit();
            Log.i(TAG, "wmbPreference setInstallFlag: ");
        }

        Log.i(TAG, "-setInstallFlag: ");
    }
    IStatusCallback statusCalback = new IStatusCallback() {
//    IAgentCallback statusCalback = new IAgentCallback.Stub() {

        @Override
        public void readyCallback()  {
            Log.i(TAG, "readyCallback: run");
            /*Start timer*/
            if(isCounterRunning ){
                isCounterRunning = true;
                //getTimerCancel();
                //getTimerRestart();
            }
            Log.i("CTMS-cb", "readyCallback: Callback when CTMS process is finish, got download:" + downloaded);
            Log.i(TAG, "totalNum downloaded file: " + totalNum + "  bootflag=" + bootflag);
//            inCTOSS_SetDownloadedNotFinishedFlag(0);//20210127 for CTMS
            if(downloaded || bootflag)
            {
                Log.i(TAG, "readyCallback: if Dl multi app, then this will be called multi times");
                Log.i(TAG, ".myPid(): " + android.os.Process.myPid());
                //20210127 for CTMS start
                Log.i(TAG, "fGotFileDownloadFailed: " + fGotFileDownloadFailed);
                BatteryManager batteryManager=(BatteryManager)getSystemService(BATTERY_SERVICE);
                int batteryRemain = 0;
                Boolean isCounterTopTerminal = false;
                String modelName = "";
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    batteryRemain = batteryManager.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY);
                    Log.i(TAG, "BATTERY_PROPERTY_CAPACITY: " + batteryRemain ) ;
                    try {
                        modelName = ctSystem.getModelName();
                        Log.i(TAG,  " getModelName:" + ctSystem.getModelName() + " getDeviceModel="+ ctSystem.getDeviceModel()) ;

                    } catch (Exception e) {
                        Log.e(TAG, "get infor erro: " + e.getMessage() );
                    }
                    if(modelName.equals("Saturn1000KC")) {
                        Log.i(TAG, "Saturn1000KC CounterTop terminal: ");
                        isCounterTopTerminal = true;
                    }

                    if((batteryRemain > 35) || isCounterTopTerminal)//S1KC terminal cap batteryRemain is 0
                    {
                        //Do not allow to do installation
                        //Fw + software
                        Log.i(TAG, "devtest getRebootInstallFlag: " + ctCtms.getRebootInstallFlag());
						Log.i(TAG, "devtest fGotFileDownloadFailed: " + fGotFileDownloadFailed);
                        if(!fGotFileDownloadFailed)
                        {
                            ////new background dl
                            Log.i(TAG, "devtest before setInstallFlag and ctCtms.setRebootInstallFlag");
                            setInstallFlag();
                            ctCtms.setRebootInstallFlag(true);
                        }
                    } else {
                        //TODO: we will not set allow to install flag here, and pls show some msg based on your project
                        //then only next CTMS call will try to install the new software and when battery is higher than 35%
                        Log.w(TAG, "Please charge your terminal, will not set install flag to true");
//                        vdUpdateDownloadStatus("Please charge your terminal above 30%, and install new software");

                        DisplayDiaglocBox();
                        isLowBattery = true;

                        // Activity_ctms_update.this.finish();
				        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
/*				        synchronized (MainActivity.LOCK) {
				            MainActivity.LOCK.setCondition(true);
				            MainActivity.LOCK.notifyAll();
				        }
						
                        return;*/
                    }
                }
                ////new background dl, if isLowBattery true, will finish current UI inside  DisplayDiaglocBox fun
                if(!isLowBattery) {
                    //20210127 for CTMS end
                    handler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            Log.i(TAG, "run: vdRebootNow();");
                            vdRebootNow();
                        }
                    }, inPostDelay);
                    Log.i(TAG, "reboot after delay: ");
                }

            }
            else
            {
                Log.i(TAG, "readyCallback: no file DL");
                //20210127 for CTMS end
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        Log.i(TAG, "run: vdRebootNow();");
                        vdRebootNow();
                    }
                }, inPostDelay);
            }

            // Activity_ctms_update.this.finish();
	        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
	        /*
            synchronized (MainActivity.LOCK) {
	            MainActivity.LOCK.setCondition(true);
	            MainActivity.LOCK.notifyAll();
	        }
	         */
/*            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    Log.i(TAG, "run: finish");
                    Activity_ctms_update.this.finish();
                }
            }, 150);*/

        }

        @Override
        public void connectCallback(int i) {
            Log.i(TAG, "connectCallback: run");
            /*Start timer*/
            if(isCounterRunning ){
                isCounterRunning = true;
                /*cancel timer first*/
                getTimerCancel();
                //getTimerRestart();
            }

            Log.i("CTMS-cb " + i,  "connectCallback: callback when connect to server:");
            vdUpdateDownloadStatus("Connecting...");
            //20210127 for CTMS
            if(i != 0) {
                ConnectFailedTimes ++ ;
                Log.e(TAG, "connectCallback failed: " + ConnectFailedTimes);
                vdUpdateDownloadStatus("Connect attempt count " + ConnectFailedTimes);
                if(ConnectFailedTimes >= 3) {
                    // Activity_ctms_update.this.finish();

			        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
			        synchronized (MainActivity.LOCK) {
			            MainActivity.LOCK.setCondition(true);
			            MainActivity.LOCK.notifyAll();
			        }


                    return;
                }
            }

            vdUpdateDownloadStatus("Connected");
        }

        @Override
        public void getTerminalInfoCallback(int i) {
            Log.i(TAG, "getTerminalInfoCallback: run");
            /*Start timer*/
            if(isCounterRunning ){
                isCounterRunning = true;
                //getTimerCancel();
                //getTimerRestart();
            }
            Log.i("CTMS-cb " + i, "getTerminalInfoCallback: callback when upload app and FW infor:");
            vdUpdateDownloadStatus("Upload app and FW infor...");
        }

        @Override
        public void downloadCallback(int i, DownloadInfo downloadInfo) {
            Log.i(TAG, "downloadCallback: run");

            // initialize private variable -- sidumili
            inDLFullSize = 0;

            /*Start timer*/
            if(isCounterRunning ){
                isCounterRunning = true;
                //getTimerCancel();
                //getTimerRestart();
            }
            Log.i("CTMS-cb-DL " + i, "Download fileName: " + downloadInfo.fileName + "  fullSize:" + downloadInfo.fullSize + "   downloadSize=" + downloadInfo.downloadSize + "  totalDownloadNumber:" + downloadInfo.totalDownloadNumber+ " downloadIndex=" + downloadInfo.downloadIndex);
            Log.i(TAG, "downloadCallback: " + ctCtms.getUpdateList());

            //20210127 for CTMS
            if((i != 0) && (i != 4096)) {
                //Download not succ, cannot set install flag to true when goes readyCallback
                Log.i(TAG, "devtest set fGotFileDownloadFailed to true");
                fGotFileDownloadFailed = true;
                Log.e("CTMS-cb-DL ", "File DL failed: " + downloadInfo.fileName);
            }

            if (downloadInfo.downloadSize > 0) {
                Log.i(TAG, "downloadSize >0:" + " Pre-DownloadfileName:" + strPreDownloadfileName);

                if(!downloadInfo.fileName.equals("UpdateList")) {
                    if(downloadInfo.downloadSize >= downloadInfo.fullSize)
                    {
                        if(!strPreDownloadfileName.equals(downloadInfo.fileName))
                        {
                            strPreDownloadfileName = "";
                            strPreDownloadfileName = downloadInfo.fileName;
                            totalNum++;
                        }

                        Log.i("CTMS-cb-DL ", "finish download one file, total file num increase to: " + totalNum);
                    }

                    downloaded = true;
                    //back up data based on the .tbd file
                    if(!backUpDbFlag) {
                        Log.i(TAG, "back db file: ");
                        inCTOSS_BackupDataScript();
                        backUpDbFlag = true;
                    }

                    //TODO: need to set back later
                    //set downloaded flag to true, so when boot up will do auto update again if download not finished yet
//                    inCTOSS_SetDownloadedNotFinishedFlag(1);//20210127 for CTMS

                    int progress = (int) ((((float) downloadInfo.downloadSize) / ((float) downloadInfo.fullSize)) * 100);

                    Log.i(TAG, "progress: " + progress);
                    String s=String.valueOf(progress) + "%";

                    String filesize;
                    double size;
                    StringBuffer bytes = new StringBuffer();
                    DecimalFormat format = new DecimalFormat("###.0");
                    if(downloadInfo.fullSize > (1024*1024)) {
                        size = (downloadInfo.fullSize / (1024.0 * 1024.0));
                        bytes.append(format.format(size)).append("MB");
                    } else if(downloadInfo.fullSize > (1024))
                    {
                        size = (downloadInfo.fullSize / (1024.0));
                        bytes.append(format.format(size)).append("KB");

                    } else {
                        bytes.append((int) downloadInfo.fullSize).append("B");
                    }

                    // terminal reboot when has an update -- sidumili
                    // totalNum -> file count, if > 1 then included application during download
                    inDLFullSize = (int)downloadInfo.fullSize;
                    int inCTMSPRMSize = inCTOSS_GetEnvInt("CTMSPRMSIZE");
                    int inCTMSForceReboot = inCTOSS_GetEnvInt("CTMSFORCEREBOOT");
                    Log.i(TAG, "downloadCallback: inDLFullSize="+inDLFullSize+",inCTMSPRMSize="+inCTMSPRMSize + ",totalNum="+totalNum + ",inCTMSForceReboot="+inCTMSForceReboot);
                    if (inCTMSPRMSize != inDLFullSize || totalNum > 1 || inCTMSForceReboot > 0)
                    {
                        inCTOSS_SetEnvInt("CTMSPRMSIZE", inDLFullSize); // set prm size to env -- sidumili
                        isReboot = true;
                    }

                    filesize = bytes.toString();
                    Log.i(TAG, "downloadCallback: filesize="+filesize+",downloadInfo.fullSize="+downloadInfo.fullSize);
                    vdUpdateDownloadStatus("Downloading...\n" + downloadInfo.fileName + "\n"+ filesize + "\n" + s);
                    vdUpdateDownloadProcessBar(progress);
                    Log.d(TAG, "got update: ");
                }

            } else {
                Log.i(TAG, "No update from CTMS server: ");
            }
        }

        @Override
        public void diagnosticCallback(int i) {
            /*Start timer*/
            if(isCounterRunning ){
                isCounterRunning = true;
                //getTimerCancel();
                //getTimerRestart();
            }
            Log.i("CTMS-cb " + i, "diagnosticCallback: callback when run diag");
            vdUpdateDownloadStatus("Diagnostic...");
        }

        @Override
        public void freeTypeDownloadCallback(int i, DownloadInfo downloadInfo) {
            /*Start timer*/
            if(isCounterRunning ){
                isCounterRunning = true;
                //getTimerCancel();
                //getTimerRestart();
            }
            Log.i("uob-CTMS-cb ", "freeTypeDownloadCallback: ");
        }

        @Override
        public void freeTypeInstallCallback(int i, PackageInfo packageInfo) {
            /*Start timer*/
            if(isCounterRunning ){
                isCounterRunning = true;
                //getTimerCancel();
                //getTimerRestart();
            }
            Log.i("uob-CTMS-cb ", "freeTypeInstallCallback: ");
        }

        //Install Callback is not used for Reboot mode
        @Override
        public void installCallback(int i, PackageInfo packageInfo) {
            /*Start timer*/
            if(isCounterRunning ){
                isCounterRunning = true;
                //getTimerCancel();
                //getTimerRestart();
            }
            Log.i("CTMS-cb " + i, "installCallback: " + packageInfo.fileName);
            Log.i(TAG, "install getUpdateList: " + ctCtms.getUpdateList());
            if(i == 4096)//4096 start, 0 end
                installNum++;
            String strInstallNum = "[install " + installNum + "/" + totalNum + "]\n";
            vdUpdateDownloadStatus("Installing...\n" + strInstallNum +  packageInfo.fileName);
        }
    };

    private void connectionInfo()
    {
        String sTemp = "";

        tv_ctms_sn.setText("Serial No.: "+strSn);
        tv_ctms_tcp.setText("TCP Host IP/Port: "+HostIP+":"+HostPort);
        tv_ctms_tls.setText("TLS Host IP/Port: "+strTlsHostIP+":"+TlsHostPort);

        switch (strMode)
        {
            case 1:
                sTemp = "TCP";
                break;
            case 2:
                sTemp = "TLS";
                break;
            default:
                sTemp = "NONE";
                break;
        }

        tv_ctms_mode.setText("TMS Mode: "+sTemp);

        switch (strCommMode)
        {
            case 0:
                sTemp = "DIALUP";
                break;
            case 1:
                sTemp = "ETHERNET";
                break;
            case 2:
                sTemp = "GPRS";
                break;
            case 4:
                sTemp = "WIFI";
                break;
            default:
                sTemp = "NONE";
                break;
        }

        tv_comm_mode.setText("Comm Mode: "+sTemp);

    }

    public native int inCTOSS_GetEnvInt(String sztag);
    public native int inCTOSS_SetEnvInt(String sztag, int invalue);

    public native int inCTOSS_BackupDataScript();
    //20210127 for CTMS, can remove this function in the 2nd release, CTMS can auto do resume download, and app only control when need to install process
//    public native int inCTOSS_SetDownloadedNotFinishedFlag(int value);
}
