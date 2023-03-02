package com.Source.S1_BDO.BDO.Trans;

import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.PowerManager;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresApi;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;

import static com.Source.S1_BDO.BDO.Main.MultiAP_SubAP.TAG;

// sidumili:
public class EditListView extends DemoAppActivity {

    private Context mContext;
    final Context c = this;
    public static String final_string;

    private int inTimeOut = 30;
    private boolean isTitle = false;

    Button btn_can;
    Button btn_update;
    TextView tv_title;
    Button btn_back;

    RecyclerView recyclerView;
    LinearLayout linearLayout;

    String s1[], s2[], s3[];
    Integer[] s4;
    String s5[];
    Boolean s6[];
    Boolean s7[];

    Intent intent = new Intent();
    Bundle bundle = new Bundle();

    public static String sHeaderTitle;

    public static boolean isResume = false;

    protected PowerManager.WakeLock mWakeLock;

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
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

        setContentView(R.layout.activity_editdetail);

        /* This code together with the one in onDestroy()
         * will make the screen be always on until this Activity gets destroyed. */
        final PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "S1_BDO:AAA-EditListView>>WAKE_LOCK");
        this.mWakeLock.acquire();

        Intent intent = getIntent();
        String dispmsg = intent.getStringExtra("pass_in_string");
        Log.i("dispmsg", dispmsg);

        btn_can = (Button) findViewById(R.id.btn_can);
        btn_update = (Button) findViewById(R.id.btn_update);
        tv_title = (TextView) findViewById(R.id.textView_txn);
        btn_back = (Button) findViewById(R.id.btn_back);

        // Display Trxn Title
        String[] dispmsginfotitle = dispmsg.split("\\|");
        tv_title.setText(dispmsginfotitle[0]);

        // Set to be called un EditListSelected
        sHeaderTitle = tv_title.getText().toString();

        ArrayList<String> lists1 = new ArrayList<>();
        ArrayList<String> lists2 = new ArrayList<>();
        ArrayList<String> lists3 = new ArrayList<>();
        ArrayList<Integer> lists4 = new ArrayList<>();
        ArrayList<String> lists5 = new ArrayList<>(); // FieldName
        ArrayList<Boolean> lists6 = new ArrayList<>(); // Flag Check Length
        ArrayList<Boolean> lists7 = new ArrayList<>(); // isHide

        // Read file
        try {
            Scanner sFile = new Scanner(new File("/data/data/pub/editfile.dat"));
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
                    int msgcnt = dispmsginfo.length;

                    //System.out.println("===========================================================");
                    //System.out.println("EditListView::record [" + record + "]" + "|msgcnt [" + msgcnt + "]");
                    //System.out.println("EditListView::dispmsginfo [" + dispmsginfo[0] + "," + dispmsginfo[1] + "," + dispmsginfo[2] + "," + dispmsginfo[3] + "," + dispmsginfo[4] + "," + dispmsginfo[5] + "]");

                    String sFldType = dispmsginfo[0];
                    String sFldName = dispmsginfo[1];
                    String sFldDesc = dispmsginfo[2];
                    String sFldValue = dispmsginfo[3];
                    Integer iFldLength = Integer.parseInt(dispmsginfo[4]);
                    Boolean iFlagCheckLength = (dispmsginfo[5].compareTo("1") == 0 ? true : false);
                    Boolean isHide = (dispmsginfo[6].compareTo("1") == 0 ? true : false);

                    //System.out.println("EditListView::x [" + x + "]");
                    //System.out.println("EditListView::sFldType [" + sFldType + "]");
                    //System.out.println("EditListView::sFldName [" + sFldName + "]");
                    //System.out.println("EditListView::sFldDesc [" + sFldDesc + "]");
                    //System.out.println("EditListView::sFldValue [" + sFldValue + "]");
                    //System.out.println("EditListView::iFldLength [" + iFldLength + "]");
                    //System.out.println("EditListView::iFlagCheckLength [" + iFlagCheckLength + "]");
                    //System.out.println("EditListView::isHide [" + isHide + "]");
                    //System.out.println("===========================================================");

                    if (sFldName.length() > 0 && sFldType.length() > 0)
                    {
                        lists1.add(sFldType);
                        lists2.add(sFldDesc);
                        lists3.add(sFldValue);
                        lists4.add(iFldLength);
                        lists5.add(sFldName);
                        lists6.add(iFlagCheckLength);
                        lists7.add(isHide);
                    }
                }

                x++;

            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        recyclerView = (RecyclerView) findViewById(R.id.recyclerView);

        s1 = lists1.toArray(new String[lists1.size()]); // Field Type
        s2 = lists2.toArray(new String[lists2.size()]); // Field Description
        s3 = lists3.toArray(new String[lists3.size()]); // Field Value
        s4 = lists4.toArray(new Integer[lists4.size()]); // Field Length
        s5 = lists5.toArray(new String[lists2.size()]); // Field Name
        s6 = lists6.toArray(new Boolean[lists6.size()]); // Flag Check Length
        s7 = lists7.toArray(new Boolean[lists7.size()]); // isHide

        /*
        System.out.println(Arrays.toString(s1));
        System.out.println(Arrays.toString(s2));
        System.out.println(Arrays.toString(s3));
        System.out.println(Arrays.toString(s4));
        System.out.println(Arrays.toString(s5));
        System.out.println(Arrays.toString(s6));
         */

        System.out.println("Field Description:"+ Arrays.toString(s2));
        System.out.println("Field Value:"+Arrays.toString(s3));

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

        EditAdapter EditAdapter = new EditAdapter(this, s1, s2, s3, s4, s6, s7);
        recyclerView.setAdapter(EditAdapter);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));

        /*Start timer*/
        //getTimerRestart();

        btn_update.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                Boolean isValid = checkAdapterFieldLength();
                if (!isValid)
                {
                    // Do nothing...
                }
                else
                {
                    String sAdapterData = getAdapterData();

                    /*cancel timer first*/
                    getTimerCancel();

                    final_string = "CONFIRM" + "|" + sAdapterData;
                    EditListView.this.finish();

                    // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                    synchronized (MainActivity.LOCK) {
                        MainActivity.LOCK.setCondition(true);
                        MainActivity.LOCK.notifyAll();

                    }
                }
            }
        });

        btn_can.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                //final_string = "CANCEL";
                final_string = "CANCEL" + "|" + "";
                EditListView.this.finish();

                Log.i("TINE", "EXIT BUTTON");

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

                //final_string = "CANCEL";
                final_string = "CANCEL" + "|" + "";
                EditListView.this.finish();

                Log.i("TINE", "BACK BUTTON");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

    }

    // Fetch All Value from recyclerview
    public String getAdapterData()
    {
        String sField = "";
        String sValue = "";
        String sTemp = "";
        String sFinalString = "";

        Log.i("sidumili", "getAdapterData: ");
        for(int i=0;i<s5.length;i++)
        {
            sField = s5[i];
            sValue = EditAdapter.data3[i];

            sTemp = "";
            //sTemp = sField + "^" + sValue;
            sTemp = sValue;
            sFinalString = sFinalString + sTemp + "|";

            Log.d(TAG, "Edited::sField=" + sField + "|sValue="+sValue);
        }

        Log.d(TAG, "Edited sFinalString=" + sFinalString);

        return sFinalString;
    }

    public Boolean checkAdapterFieldLength()
    {
        Boolean isValid = true;
        String sValue = "";
        String sDescription = "";
        Integer iLength = 0;
        Boolean isCheck = false;

        Log.i(TAG, "checkAdapterFieldLength: ");
        for(int i=0;i<s6.length;i++)
        {
            isCheck = s6[i];
            Log.d(TAG, "Edited isCheck=" + isCheck);
            if (isCheck)
            {
                sDescription = s2[i];
                sValue = EditAdapter.data3[i];
                iLength = s4[i];
                Log.d(TAG, "Check Length::sDescription=" + sDescription + "|sValue="+sValue+ "|iLength="+iLength+ "|sValue Length="+sValue.length());
                if (sValue.length() != iLength)
                {
                    isValid = false;
                    break;
                }
            }
        }

        if (!isValid)
            Toast.makeText(this, sDescription + " length must be equal to " + iLength, Toast.LENGTH_SHORT).show();

        return  isValid;
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
            Log.i("EditListView Timer", "Timer onTick");
        }

        @Override
        public void onFinish() {
            Log.i("Timer", "EditListView Timer onFinish");

            //etInputStr.clear();
            final_string = "TIME OUT";

            //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));

            Log.i("PATRICK", "Timeout EditListView");
            EditListView.this.finish();

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
    protected void onResume() {
        Log.i(TAG, "onResume: EditListView");
        super.onResume();

        Log.i(TAG, "onResume: isResume="+isResume);
        if (!isResume) return;

        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);    //Tine:  Show Status Bar

        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);

        int iIndex = EditListSelected.localposition;
        s3[iIndex] = EditListSelected.sSelectedNewValue;

        Log.i(TAG, "setListViewData: iIndex="+iIndex);
        Log.i(TAG, "setListViewData: s3="+Arrays.toString(s3));

        EditAdapter editAdapter = new EditAdapter(this, s1, s2, s3, s4, s6, s7);
        editAdapter.notifyDataSetChanged();
        recyclerView.setAdapter(editAdapter);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
    }

    @Override
    protected void onDestroy() {
        this.mWakeLock.release();
        super.onDestroy();
        final_string = null;
        mContext = null;
        recyclerView = null;
        s1 = null;
        s2 = null;
        s3 = null;
        s4 = null;
        s6 = null;
        // SysApplication.getInstance().removeActivity(this);
    }

}
