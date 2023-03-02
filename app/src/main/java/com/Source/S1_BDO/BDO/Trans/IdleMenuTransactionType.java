package com.Source.S1_BDO.BDO.Trans;

import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.PowerManager;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.OperatorHolder;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;
import com.Source.S1_BDO.BDO.model.DPopupMenu;

public class IdleMenuTransactionType extends DemoAppActivity {

    private boolean isFirstTrigger=true;
    private int flag=1, iHeader=0;
    private Button btn_title=null;
    private String TAG="MCC_Menu";

    private static Handler mHandler;

    public static String select_item;
    public String TrxList, AppHeaderBuff;
    public String[] transItems = new String[20];
    public int inCnt=0;


    private int inTimeOut = 20;

    private DPopupMenu mccMenu;
    private String MenuTitle, FullMenuItems;
    private String AppDetail1, AppDetail2, AppDetail, Networknm;
    private String[] MenuItems = new String[100];
    private String[] AppHeader = new String[100];
    private String[] TrxMenuItems = new String[100];
    private String[] TrxTitle = new String[100];

    private Button btn_nav_main;

    private TextView textView_appversion;
    private TextView textView_merchant;
    private TextView textView_tprofile;
    private TextView textView_carriername;

    protected PowerManager.WakeLock mWakeLock;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i("MenuTransactionType", "Popup Menu onCreate");

        overridePendingTransition(0, 0); // disable the animation, faster

        OperatorHolder operatorHolder = new OperatorHolder(this);
        WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        WifiInfo info = wifiManager.getConnectionInfo ();
        String ssid  = info.getSSID();

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

        setContentView(R.layout.activity_idle_popup);

        /* This code together with the one in onDestroy()
         * will make the screen be always on until this Activity gets destroyed. */
        final PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "S1_MCC:AAA-IdleMenuTransactionType>>WAKE_LOCK");
        this.mWakeLock.acquire();

        btn_nav_main = (Button) findViewById(R.id.btn_nav_main);

        textView_appversion = (TextView) findViewById(R.id.app_version);
        textView_merchant = (TextView) findViewById(R.id.merchant);
        textView_tprofile = (TextView) findViewById(R.id.tprofile);
        textView_carriername = (TextView) findViewById(R.id.NetworkCarrier);

        //btn_can.setText("hello");
        Intent intent = getIntent();
        String MenuBuff = intent.getStringExtra("pass_in_string");
        Log.i("main MenuBuff", MenuBuff);
        AppHeaderBuff = intent.getStringExtra("AppHeaderBuff");
        Log.i("main AppHeaderBuff", AppHeaderBuff);


        MenuTitle = MenuBuff.substring(0, MenuBuff.indexOf("|"));
        FullMenuItems = MenuBuff.substring(MenuBuff.indexOf("|") + 1);
        Log.i("MenuTitle", MenuTitle);
        Log.i("FullMenuItems", FullMenuItems);

        /*split out the menu items*/
        //MenuItems = FullMenuItems.split(" \n");
        TrxMenuItems = FullMenuItems.split(" \n");
        AppHeader = AppHeaderBuff.split(" \n");
        iHeader = AppHeader.length;

        for(int indx=0; indx<iHeader; indx++)
        {
            switch (indx) {
                case 0:
                    textView_appversion.setText(AppHeader[indx]);
                    break;
                case 1:
                    textView_merchant.setText(AppHeader[indx]);
                    break;
                case 2:
                    AppDetail1 = "MID: "+AppHeader[indx];
                    break;
                case 3:
                    AppDetail2 = "TID: "+AppHeader[indx];
                    break;

            }
        }
        AppDetail = AppDetail1 + "  |  " + AppDetail2;
        textView_tprofile.setText(AppDetail);

        if (ssid.equals("<unknown ssid>") || (ssid.length() <=0))
            Networknm = "Network: " + operatorHolder.getOperatorName();
        else
            Networknm = "Network: " + ssid;

        if (Networknm.equals("Network: "))
            Networknm = "Network: NOT CONNECTED";

        Log.i("AAA Networkname", "Networknm: " + Networknm);
        textView_carriername.setText(Networknm);

        btn_title = (Button) findViewById(R.id.mcc_menu_title);
        btn_title.setText(MenuTitle);

        /*Start timer*/
        //getTimerRestart();

        FuncKeyCancel();

/*
        btn_can.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i("CANCEL", "CANCEL DPopupMenuActivity");
                getTimerCancel();
                popMenu.dismiss();
                select_item = "d_KBD_CANCEL";

                Log.i("TINE", "CANCEL DPopupMenuActivity");
                //startActivity(new Intent(DPopupMenuActivity.this, MainActivity.class));
                DPopupMenuActivity.this.finish();

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();
                }
            }
        });
*/

/*
        button1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog.Builder builder = new AlertDialog.Builder(DPopupMenuActivity.this);
                builder.setTitle("Choose an animal");

                builder.setNegativeButton("cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        //finish();
                        select_item = "TO";


                        DPopupMenuActivity.this.finish();

                        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                        synchronized (MainActivity.LOCK) {
                            MainActivity.LOCK.setCondition(true);
                            MainActivity.LOCK.notifyAll();
                        }
                    }
                });

                // add a list
                String[] animals = {"horse", "cow", "camel", "sheep", "goat"};
                builder.setItems(animals, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        switch (which) {
                            case 0: // horse
                            case 1: // cow
                            case 2: // camel
                            case 3: // sheep
                            case 4: // goat
                        }
                    }
                });

                AlertDialog dialog = builder.create();
                dialog.show();
            }
    });
*/


        btn_title.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                flag = 1;

                mccMenu = new DPopupMenu(getApplicationContext());


                //popMenu.addItem(new DPopupMenu.Item("é€‰é¡¹ä¸€",0));
                //popMenu.addItem(new DPopupMenu.Item("é€‰é¡¹äºŒ",0));
                //popMenu.addItem(new DPopupMenu.Item("é€‰é¡¹ä¸‰",0));
                //popMenu.addItem(new DPopupMenu.Item("é€‰é¡¹å››",0));


                for (int i = 0; i < TrxMenuItems.length; i++) {
                    System.out.println(TrxMenuItems[i]);
                    TrxTitle[i] = TrxMenuItems[i].substring(0,TrxMenuItems[i].indexOf("|"));
                    MenuItems[i] = TrxMenuItems[i].substring(TrxMenuItems[i].indexOf("|") + 1);
                    Log.i("Transaction Title", TrxTitle[i]);
                    Log.i("Transaction Image ID", MenuItems[i]);

                    mccMenu.addItem(new DPopupMenu.Item(MenuItems[i], 0));

                    /*
                    switch (TrxTitle[i]) {
                        case "MCC":
                            mccMenu.addItem(new TxnPopupMenu.Item(MenuItems[i], 0));
                            //transItems[inCnt] = TrxTitle[i];
                            transItems[inCnt] = MenuItems[i];
                            inCnt++;
                            break;

                        case "BANCNET":
                            mccMenu.addItem(new TxnPopupMenu.Item(MenuItems[i], 0));
                            //transItems[inCnt] = TrxTitle[i];
                            transItems[inCnt] = MenuItems[i];
                            inCnt++;
                            break;

                        case "WECHAT":
                            mccMenu.addItem(new TxnPopupMenu.Item(MenuItems[i], 0));
                            //transItems[inCnt] = TrxTitle[i];
                            transItems[inCnt] = MenuItems[i];
                            inCnt++;
                            break;
                    }
                    */
                }


                mccMenu.setOnItemClickListener(popmenuItemClickListener);
                mccMenu.showAsDropDown(btn_title);
                //FuncKeyCancel();
                //popMenu.dismiss();
            }
        });

        //don't want change this sample code, so set auto click.
        new Handler().postDelayed(new Runnable() {

            public void run() {
                //button1.dispatchTouchEvent(MotionEvent.obtain(SystemClock.uptimeMillis(), SystemClock.uptimeMillis(), MotionEvent.ACTION_BUTTON_PRESS , 0, 0, 0));
                //button1.dispatchTouchEvent(MotionEvent.obtain(SystemClock.uptimeMillis(), SystemClock.uptimeMillis(), MotionEvent.ACTION_BUTTON_RELEASE , 0, 0, 0));
                Log.i("TINE", "Runnable DPopupMenuActivity");
                btn_title.callOnClick();
            }
        }, 100);

        //for testing only

    }


    private AdapterView.OnItemClickListener popmenuItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

            /*
            switch (position) {
                case 0:
                    popMenu.dismiss();
                    break;
                case 1:
                    popMenu.dismiss();
                    break;
                case 2:
                    popMenu.dismiss();
                    break;
                case 3:
                    popMenu.dismiss();
                    break;
            }
            */
            getTimerCancel();
            mccMenu.dismiss();
            select_item = String.valueOf(position+1);
            //select_item = transItems[position];

            Log.i("MenuTransactionType", select_item);
            //startActivity(new Intent(DPopupMenuActivity.this,MainActivity.class));

            IdleMenuTransactionType.this.finish();

            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }


        }
    };

    public void FuncKeyCancel() {
        btn_nav_main.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerCancel();
                mccMenu.dismiss();
                //Toast.makeText(getApplicationContext(), "Cancelling...", Toast.LENGTH_SHORT).show();

                select_item = "CANCEL";
                //startActivity(new Intent(AmountEntryActivity.this, MainActivity.class));
                IdleMenuTransactionType.this.finish();

                Log.i("PATRICK", "DPopupMenuActivity KeyBoard btn_can");
                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();
                }

            }
        });
    }



    /**
     * å–æ¶ˆå€’è®¡æ—¶
     */
    public void getTimerCancel() {
        timer.cancel();
    }

    /**
     * å¼€å§‹å€’è®¡æ—¶
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

            mccMenu.dismiss();
            select_item = "TO";

            //startActivity(new Intent(DPopupMenuActivity.this,MainActivity.class));


            Log.i("TINE", "Timeout DPopupMenuActivity");
            IdleMenuTransactionType.this.finish();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        getTimerCancel();
        mccMenu.dismiss();
        select_item = "CANCEL";
        //startActivity(new Intent(DPopupMenuActivity.this,MainActivity.class));
        IdleMenuTransactionType.this.finish();
        Log.i("PATRICK", "BackPressed IdleMenuTransactionType");

        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();
        }
    }

    @Override
    protected void onDestroy() {
        this.mWakeLock.release();
        super.onDestroy();
        select_item = null;
        AppHeaderBuff = null;
        // SysApplication.getInstance().removeActivity(this);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */


}
