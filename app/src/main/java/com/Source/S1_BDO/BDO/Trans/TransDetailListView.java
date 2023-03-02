package com.Source.S1_BDO.BDO.Trans;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.PowerManager;
import android.support.annotation.Nullable;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

// sidumili:
public class TransDetailListView extends DemoAppActivity {

    private Context mContext;
    final Context c = this;
    public static String final_string;

    public static int inTimeOut = 30;

    RecyclerView recyclerView;
    String s0[], s1[], s2[], s3[], s4[], s5[], s6[], s7[], s8[], s9[], s10[], s11[];
    TextView tv_title;
    Button btn_back;

    String final_selected_string;

    protected PowerManager.WakeLock mWakeLock;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {

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

        setContentView(R.layout.activity_transdetail);

        /* This code together with the one in onDestroy()
         * will make the screen be always on until this Activity gets destroyed. */
        final PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "S1_BDO:TransDetailListView>>WAKE_LOCK");
        this.mWakeLock.acquire();


        Intent intent=getIntent();
        String dispmsg=intent.getStringExtra("pass_in_string");
        Log.i("saturn dispmsg", dispmsg);

        tv_title = (TextView) findViewById(R.id.textView_txn);
        btn_back = (Button) findViewById(R.id.btn_back);

        // Display Trxn Title
        String[] dispmsginfotitle = dispmsg.split("\\|");
        tv_title.setText(dispmsginfotitle[0]);

        ArrayList<String> lists0 = new ArrayList<String>();
        ArrayList<String> lists1 = new ArrayList<String>();
        ArrayList<String> lists2 = new ArrayList<String>();
        ArrayList<String> lists3 = new ArrayList<String>();
        ArrayList<String> lists4 = new ArrayList<String>();
        ArrayList<String> lists5 = new ArrayList<String>();
        ArrayList<String> lists6 = new ArrayList<String>();
        ArrayList<String> lists7 = new ArrayList<String>();
        ArrayList<String> lists8 = new ArrayList<String>();

        // Read file
        try {
            Scanner sFile = new Scanner(new File("/data/data/pub/transdetailfile.dat"));
            int x = 0;

            while (sFile.hasNext())
            {
                //Log.i("Data=", sFile.nextLine());

                String record = sFile.nextLine();

                if (record == null)
                {
                    // do nothing....
                }
                else
                {
                    String[] dispmsginfo = record.split("\\|");
                    System.out.println("TransDetailListView::record [" + record + "]");

                    String sData0, sData1, sData2, sData3, sData4, sData5, sData6, sData7, sData8;
                    sData0 = dispmsginfo[0];
                    sData1 = dispmsginfo[1];
                    sData2 = dispmsginfo[2];
                    sData3 = dispmsginfo[3];
                    sData4 = dispmsginfo[4];
                    sData5 = dispmsginfo[5];
                    sData6 = dispmsginfo[6];
                    sData7  = dispmsginfo[7];
                    sData8  = dispmsginfo[8];

                    /*
                    System.out.println("TransDetailListView::x [" + x + "]");
                    System.out.println("TransDetailListView::sData0 [" + sData0 + "]");
                    System.out.println("TransDetailListView::sData1 [" + sData1 + "]");
                    System.out.println("TransDetailListView::sData2 [" + sData2 + "]");
                    System.out.println("TransDetailListView::sData3 [" + sData3 + "]");
                    System.out.println("TransDetailListView::sData4 [" + sData4 + "]");
                    System.out.println("TransDetailListView::sData5 [" + sData5 + "]");
                    System.out.println("TransDetailListView::sData6 [" + sData6 + "]");
                    System.out.println("TransDetailListView::sData7 [" + sData7 + "]");
                    System.out.println("TransDetailListView::sData8 [" + sData8 + "]");
                     */

                    lists0.add(sData0);
                    lists1.add(sData1);
                    lists2.add(sData2);
                    lists3.add(sData3);
                    lists4.add(sData4);
                    lists5.add(sData5);
                    lists6.add(sData6);
                    lists7.add(sData7);
                    lists8.add(sData8);

                }

                x++;

            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        recyclerView = (RecyclerView) findViewById(R.id.recyclerView);

        s0 = lists0.toArray(new String[lists0.size()]);
        s1 = lists1.toArray(new String[lists1.size()]);
        s2 = lists2.toArray(new String[lists2.size()]);
        s3 = lists3.toArray(new String[lists3.size()]);
        s4 = lists4.toArray(new String[lists4.size()]);
        s5 = lists5.toArray(new String[lists5.size()]);
        s6 = lists6.toArray(new String[lists6.size()]);
        s7 = lists7.toArray(new String[lists7.size()]);
        s8 = lists8.toArray(new String[lists8.size()]);

        /*
        System.out.println(Arrays.toString(s1));
        System.out.println(Arrays.toString(s2));
        System.out.println(Arrays.toString(s3));
        System.out.println(Arrays.toString(s4));
        System.out.println(Arrays.toString(s5));
        System.out.println(Arrays.toString(s6));
        System.out.println(Arrays.toString(s7));
        System.out.println(Arrays.toString(s8));
        */

        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);    //Tine:  Show Status Bar

        decorView = getWindow().getDecorView();
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

        // Design Horizontal Layout
        LinearLayoutManager layoutManager = new LinearLayoutManager(this, LinearLayoutManager.HORIZONTAL, false);
        recyclerView.setLayoutManager(layoutManager);
        recyclerView.setItemAnimator(new DefaultItemAnimator());

        // Initialize Adapter
        TransDetailAdapter transDetailAdapter = new TransDetailAdapter(this, s0, s1, s2, s3, s4, s5, s6, s7, s8);

        // Set Adapter to RecyclerView
        recyclerView.setAdapter(transDetailAdapter);

        /*Start timer*/
        //getTimerRestart();

        btn_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*cancel timer first*/
                getTimerCancel();

                final_string = "CANCEL";

                TransDetailListView.this.finish();

                Log.i("UserConfirmVoid", "Button buCancel");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

    }

    public static class Lock {
        private boolean condition;

        public boolean conditionMet() {
            return condition;
        }

        public void setCondition(boolean condition) {
            this.condition = condition;


        }

    }

    private void getData()
    {
        String sSelected =  TransDetailAdapter.final_rb_selected_string;
        Log.i("sidumili", "getData: sSelected="+sSelected);
        if (sSelected.length() > 0)
        {
            final_selected_string = sSelected;
        }
    }

    public static Lock LOCK = new Lock();

    public void getTimerCancel() {
        timer.cancel();
    }

    public void getTimerRestart()
    {
        timer.start();
    }

    private CountDownTimer timer = new CountDownTimer(inTimeOut*1000, 1000) {

        @Override
        public void onTick(long millisUntilFinished) {
            Log.i("EditListView Timer", "Timer onTick");
        }

        @Override
        public void onFinish() {
            Log.i("Timer", "TransDetailListView Timer onFinish");

            //etInputStr.clear();
            final_string = "TIME OUT";

            Log.i("PATRICK", "Timeout TransDetailListView");
            TransDetailListView.this.finish();

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
        final_selected_string = null;
        mContext = null;
        recyclerView = null;
        // SysApplication.getInstance().removeActivity(this);
    }
}
