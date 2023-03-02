package com.Source.S1_BDO.BDO.model;

import android.content.Context;
import android.content.Intent;
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
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;

public class DPopupMenuActivity extends DemoAppActivity {

    private boolean isFirstTrigger=true;
    private int flag=1;
    private Button button1=null;
    private String TAG="Menu";

    private static Handler mHandler;

    public static String select_item;

    private int inTimeOut = 60;

    private DPopupMenu popMenu;
    private String MenuTitle;
    private String FullMenuTimes;
    private String[] MenuItmes = new String[100];

    private Button btn_can;

    private Button btn_back;
    private TextView tvheader1;
    private TextView tvheader2;

    protected PowerManager.WakeLock mWakeLock;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "Popup Menu onCreate");

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

        setContentView(R.layout.activity_popup_menu);

        /* This code together with the one in onDestroy()
         * will make the screen be always on until this Activity gets destroyed. */
        final PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "S1_BDO:DPopupMenuActivity>>WAKE_LOCK");
        this.mWakeLock.acquire();

        btn_can = (Button) findViewById(R.id.a_popup_menu_can);

        btn_back = (Button) findViewById(R.id.btn_back);
        tvheader1 = (TextView) findViewById(R.id.tvheader1);
        tvheader2 = (TextView) findViewById(R.id.tvheader2);

        //btn_can.setText("hello");
        Intent intent = getIntent();
        String pass_in_value = intent.getStringExtra("pass_in_string");
        Log.i("main pass in value", pass_in_value);


        MenuTitle = pass_in_value.substring(0, pass_in_value.indexOf("|"));
        FullMenuTimes = pass_in_value.substring(pass_in_value.indexOf("|") + 1);
        Log.i("MenuTitle", MenuTitle);
        Log.i("FullMenuTimes", FullMenuTimes);

        /*split out the menu items*/
        MenuItmes = FullMenuTimes.split(" \n");

        button1 = (Button) findViewById(R.id.popup_menu_button11);
        button1.setText(MenuTitle);

        tvheader1.setText(MenuTitle);

        /*Start timer*/
        getTimerRestart();

        FuncKeyCancel();
        FuncKeyBack();

        button1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                flag = 1;

                popMenu = new DPopupMenu(getApplicationContext());


                //popMenu.addItem(new DPopupMenu.Item("选项一",0));
                //popMenu.addItem(new DPopupMenu.Item("选项二",0));
                //popMenu.addItem(new DPopupMenu.Item("选项三",0));
                //popMenu.addItem(new DPopupMenu.Item("选项四",0));


                for (int i = 0; i < MenuItmes.length; i++) {
                    System.out.println(MenuItmes[i]);
                    popMenu.addItem(new DPopupMenu.Item(MenuItmes[i], 0));
                }

                // 菜单项点击监听器
                popMenu.setOnItemClickListener(popmenuItemClickListener);
                popMenu.showAsDropDown(button1);
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
                button1.callOnClick();
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
            popMenu.dismiss();
            select_item = String.valueOf(position+1);
            Log.i("PATRICK", select_item);
            //startActivity(new Intent(DPopupMenuActivity.this,MainActivity.class));

            Log.i("PATRICK", "FINISH DMenuList");

            new Handler().postDelayed(new Runnable() {

                public void run() {
                    DPopupMenuActivity.this.finish();
                }
            }, 10);

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };

    public void FuncKeyCancel() {
        btn_can.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                    getTimerCancel();
                    popMenu.dismiss();
                    //Toast.makeText(getApplicationContext(), "Cancelling...", Toast.LENGTH_SHORT).show();

                    select_item = "CANCEL";
                    //startActivity(new Intent(AmountEntryActivity.this, MainActivity.class));
                    DPopupMenuActivity.this.finish();

                    Log.i("PATRICK", "DPopupMenuActivity KeyBoard btn_can");
                    // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                    synchronized (MainActivity.LOCK) {
                        MainActivity.LOCK.setCondition(true);
                        MainActivity.LOCK.notifyAll();
                    }

            }
        });
    }

    public void FuncKeyBack() {
        btn_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerCancel();
                popMenu.dismiss();
                //Toast.makeText(getApplicationContext(), "Cancelling...", Toast.LENGTH_SHORT).show();

                select_item = "CANCEL";
                //startActivity(new Intent(AmountEntryActivity.this, MainActivity.class));
                DPopupMenuActivity.this.finish();

                Log.i("PATRICK", "DPopupMenuActivity KeyBoard btn_back");
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

            popMenu.dismiss();
            select_item = "TO";

            //startActivity(new Intent(DPopupMenuActivity.this,MainActivity.class));


            Log.i("PATRICK", "Timeout DPopupMenuActivity");
            DPopupMenuActivity.this.finish();

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
        popMenu.dismiss();
        select_item = "CANCEL";
        //startActivity(new Intent(DPopupMenuActivity.this,MainActivity.class));
        DPopupMenuActivity.this.finish();
        Log.i("PATRICK", "BackPressed DPopupMenuActivity");

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
        mHandler = null;
        // SysApplication.getInstance().removeActivity(this);
    }

}