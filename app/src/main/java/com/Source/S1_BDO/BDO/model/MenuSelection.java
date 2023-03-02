package com.Source.S1_BDO.BDO.model;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.support.v4.view.ViewPager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.DMenu.GridViewAdapter;
import com.Source.S1_BDO.BDO.DMenu.Model;
import com.Source.S1_BDO.BDO.DMenu.ViewPagerAdapter;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.Trans.AmountEntryActivity;
import com.Source.S1_BDO.BDO.wub_lib;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ExecutionException;

import static android.content.ContentValues.TAG;

public class MenuSelection extends DemoAppActivity {

    private Context mContext;

    private Button mButton;
    final Context c = this;

    ViewPager mPager;
    private List<View> mPagerList;
    private List<Model> mDatas;
    private LinearLayout mLlDot;
    LayoutInflater inflater;

    public static String final_string;
    private int inTimeOut = 30;
    private int pageCount;
    private int pageSize = 150;
    private int curIndex = 0;

    private int menu_View_Mode = 0;
    private int menu_Col_Count = 0;

    private String[] arr_menu_list;
    private int menu_count = 0;

    TextView tvheader1;
    TextView tvheader2;
    Button btn_back;

    private String[] titles = null;
    private String[] images = null;
    private String gMenuImageList;

    private  int inImageCount = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
//        // //CloseActivityClass.activityList.add(this);
//        // //SysApplication.getInstance().addActivity(this);
        super.onCreate(savedInstanceState);
        Bundle bundle = getIntent().getExtras();

        overridePendingTransition(0, 0); // disable the animation, faster

        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);    //Tine:  Show Status Bar

        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);

        setContentView(R.layout.v2_prompt_menu);

        Intent intent=getIntent();
        String dispmsg=intent.getStringExtra("pass_in_string");
        Log.i("dispmsg", dispmsg);

        String[] dispmsginfo = dispmsg.split("\\^");
        int length = dispmsginfo.length;
        int msgcnt = 0;
        Log.i(TAG, "length="+length);

        arr_menu_list = new String[3000];

        for (int inIdx = 0; inIdx < length; inIdx++) {
            System.out.println("split msg1 [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            String msgToParse = dispmsginfo[inIdx];
            String[] arrmsginfo = msgToParse.split("\\|");
            msgcnt = inImageCount = arrmsginfo.length;
            Log.i(TAG, "msgcnt="+msgcnt);

            switch (inIdx) {
                case 0:
                    Log.i(TAG, "Here At Case 0.....");
                    Log.i(TAG, "Case 0 Data-> [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                    for (int x = 0; x < msgcnt; x++)
                    {
                        System.out.println("case 0::split msg2 [" + x + "][" + arrmsginfo[x] + "]");
                        switch (x)
                        {
                            case 0:
                                menu_View_Mode = Integer.parseInt(arrmsginfo[x]);
                                break;
                            case 1:
                                menu_Col_Count = Integer.parseInt(arrmsginfo[x]);
                                break;
                            case 2:
                                tvheader1 = (TextView) findViewById(R.id.tvheader1);
                                tvheader1.setText(arrmsginfo[x]);
                                break;
                            case 3:
                                tvheader2 = (TextView) findViewById(R.id.tvheader2);
                                tvheader2.setText(arrmsginfo[x]);
                                break;
                        }
                    }
                    break;
                case 1:
                    Log.i(TAG, "Here At Case 1.....");
                    Log.i(TAG, "Case 1 Data-> [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                    if (menu_View_Mode == 3)
                    {
                        gMenuImageList = dispmsginfo[inIdx].replaceAll("#", "^"); // for transactions menu  --sidumili
                    }
                    else
                    {
                        for (int x = 0; x < msgcnt; x++)
                        {
                            System.out.println("case 1::split msg2 [" + x + "][" + arrmsginfo[x] + "]");
                            String msg = arrmsginfo[x];
                            Log.i(TAG,  "len="+msg.length() +",msg="+msg);
                            if (msg.length() > 0)
                                arr_menu_list[x] = msg;
                        }
                        menu_count = msgcnt;
                    }

                    break;
            }
        }

        btn_back = (Button) findViewById(R.id.btn_back);
        btn_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                /*cancel timer first*/
                getTimerCancel();

                final_string = "CANCEL";
                //MenuSelection.this.finish();

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        MenuSelection.this.finish();
                    }
                }, 180);

                Log.i("xxxx", "btn_back MenuSelection");

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

        Log.i(TAG, "menu_View_Mode="+menu_View_Mode);
        Log.i(TAG, "menu_Col_Count="+menu_Col_Count);
        Log.i(TAG, "gMenuImageList="+gMenuImageList);

        switch (menu_View_Mode)
        {
            case 1:
            case 2:
                initImageDatas(menu_View_Mode);
                break;
            case 3:
                initDatas(gMenuImageList);
                break;
        }

        mPager = (ViewPager) findViewById(R.id.viewpager);
        inflater = LayoutInflater.from(this);
        pageCount = (int) Math.ceil(mDatas.size() * 1.0 / pageSize);
        mPagerList = new ArrayList<View>();
        for (int i = 0; i < pageCount; i++) {
            GridView gridView = (GridView) inflater.inflate(R.layout.v2_gridview, mPager, false);
            gridView.setAdapter(new GridViewAdapter(getApplicationContext(), mDatas, i, pageSize, menu_View_Mode));
            gridView.setNumColumns(menu_Col_Count); // set number of gridview column -- sidumili
            mPagerList.add(gridView);

            gridView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    int pos = position + curIndex * pageSize;
                    Log.i(TAG, "onItemClick: Icon Name=" + mDatas.get(pos).getName());
                    //setContentView(R.layout.activity_disp);

                    /*cancel timer first*/
                    getTimerCancel();

                    final_string = mDatas.get(pos).getName(); // return selected -- sidumili
                    Log.i(TAG, "onItemClick: final_string="+final_string);

                    //MenuSelection.this.finish();
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            MenuSelection.this.finish();
                        }
                    }, 500);

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
                        Log.i(TAG, "onItemClick: Exception error " + ex.getMessage());
                    }
                }
            });
        }

        mPager.setAdapter(new ViewPagerAdapter(mPagerList));

        /*Start timer*/
        getTimerRestart();

    }

    private void initImageDatas(int view_mode) {
        int imageId = 0;
        String sMenu = "";
        Log.i(TAG, "initImageDatas: ");
        mDatas = new ArrayList<Model>();

        Log.i(TAG, "view_mode="+view_mode);
        Log.i(TAG, "inImageCount="+inImageCount);

        switch (view_mode)
        {
            case 1:
                for (int i = 0; i < menu_count; i++) {

                    if (view_mode == 2) // (1 - for text mode, 2 - for image mode) -- sidumili
                        imageId = getResources().getIdentifier("ic_wallet_" + i, "mipmap", getPackageName());

                    Log.i(TAG, "arr_menu_list[i]="+arr_menu_list[i]+",imageId="+imageId);
                    mDatas.add(new Model(arr_menu_list[i], imageId));
                }
                break;
            case 2:
                for (int i = 0; i < inImageCount; i++) {

                    String msgToParse = arr_menu_list[i];
                    Log.i(TAG, "msgToParse="+msgToParse);
                    String[] arrmsginfo = msgToParse.split("\\#");

                    sMenu = arrmsginfo[0];
                    String sFileName = arrmsginfo[1].replace(".png", "");
                    Log.i(TAG, "i="+i+"->sMenu="+sMenu +",sFileName="+sFileName);

                    imageId = getResources().getIdentifier(sFileName, "mipmap", getPackageName());

                    if (imageId <= 0)
                        imageId = getResources().getIdentifier("ic_blank", "mipmap", getPackageName());

                    Log.i(TAG, "sMenu[i]="+sMenu+",imageId="+imageId);

                    mDatas.add(new Model(sMenu, imageId));
                }
                break;
        }
    }

    private void InitMenuImageList(String pMenuImageList)
    {
        Log.i(TAG, "InitMenuImageList: run");
        Log.i(TAG, "pMenuImageList="+pMenuImageList);
        Log.i(TAG, "menu_View_Mode="+menu_View_Mode);

        ArrayList<String> listsTitle = new ArrayList<String>();
        ArrayList<String> listsImage = new ArrayList<String>();

        String[] pArrList = pMenuImageList.split("\\|");
        int pLen = pArrList.length;
        System.out.println("pLen [" + pLen + "]");

        titles = null;
        images = null;
        for (int i = 0; i < pLen; i++)
        {
            if (pArrList[i].length() > 0)
            {
                System.out.println("split msg [" + i + "][" + pArrList[i] + "]");
                String[] pArrParse = pArrList[i].split("\\^");
                String pTitle = pArrParse[0];
                String pImage = pArrParse[1];

                Log.i(TAG, "i="+i+",pTitle="+pTitle+",pImage="+pImage);

                listsTitle.add(pTitle);
                listsImage.add(pImage);
            }
        }

        titles = listsTitle.toArray(new String[listsTitle.size()]);
        images = listsImage.toArray(new String[listsImage.size()]);

        System.out.println("titles="+ Arrays.toString(titles));
        System.out.println("images="+ Arrays.toString(images));
    }

    private void initDatas(String pMenuImageList) {
        Log.i(TAG, "initDatas: run");
        Log.i(TAG, "pMenuImageList="+pMenuImageList);

        InitMenuImageList(pMenuImageList); // call to get title and images from TERMINAL.S3DB, IMG table -- sidumili

        mDatas = new ArrayList<Model>();
        for (int i = 0; i < titles.length; i++) {
            Log.i(TAG, "i="+i+",titles="+titles[i]+",images="+images[i]);

            int imageId = getResources().getIdentifier(images[i].replace(".png", ""), "mipmap", getPackageName());
            Log.i(TAG, "imageId="+imageId);

            if (imageId <= 0)
                imageId = getResources().getIdentifier("ic_blank", "mipmap", getPackageName());

            mDatas.add(new Model(titles[i], imageId));
        }
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

            getTimerCancel();

            final_string = "TIME OUT";

            //MenuSelection.this.finish();
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    MenuSelection.this.finish();
                }
            }, 300);

            Log.i("PATRICK", "Timeout MenuSelection");

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
                Log.i(TAG, "onFinish: Exception error " + ex.getMessage());
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
        Log.i(TAG, "onDestroy: ");
        super.onDestroy();
        final_string = null;
        mContext = null;

        if(mPagerList != null) {
            mPagerList.clear();
            mPagerList = null;
        }

       // SysApplication.getInstance().removeActivity(this);
    }

}
