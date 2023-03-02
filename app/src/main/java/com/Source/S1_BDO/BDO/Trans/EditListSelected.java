package com.Source.S1_BDO.BDO.Trans;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.PowerManager;
import android.support.v7.widget.RecyclerView;
import android.text.Editable;
import android.text.InputFilter;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;
import com.Source.S1_BDO.BDO.wub_lib;

import static com.Source.S1_BDO.BDO.Main.MultiAP_SubAP.TAG;

// sidumili: Display selected item from recyclerview
public class EditListSelected extends DemoAppActivity {

    Context context;
    TextView textView1, textView2, textView3;
    EditText editText;
    Button btn_ok;
    Button btn_can;
    Button btn_clr;
    Button btn_back;
    ImageView myImageView;
    TextView textView_txn;

    TextView tv_maxlength, tv_fieldlength, tv_inputlength;


    public static String final_string = "";
    public static String sSelectedNewValue = "";

    private int inTimeOut = 20;

    String localdata1, localdata2, localdata3, localdata4, localdata5;
    public static int localposition = 0;

    private static String data1[], data2[], data3[];
    private  static Integer data4[];
    private static Boolean data5[];

    RecyclerView recyclerView;
    final Context c = this;

    private static String sMaxLength = "Max Len:";
    private static String sFieldLength = "Field Len:";
    private static String sInputLength = "Input Len:";

    protected PowerManager.WakeLock mWakeLock;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Log.i(TAG, "onCreate: EditListSelected");

        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);
        super.onCreate(savedInstanceState);
        Bundle bundle = getIntent().getExtras();

        overridePendingTransition(0, 0); // disable the animation, faster

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

        setContentView(R.layout.confirm_selected);

        /* This code together with the one in onDestroy()
         * will make the screen be always on until this Activity gets destroyed. */
        final PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "S1_BDO:AAA-EditListSelected>>WAKE_LOCK");
        this.mWakeLock.acquire();

        myImageView = (ImageView) findViewById(R.id.myImageView);
        textView1 = (TextView) findViewById(R.id.textView1);
        textView2 = (TextView) findViewById(R.id.textView2);
        textView3 = (TextView) findViewById(R.id.textView3);
        textView_txn = (TextView) findViewById(R.id.textView_txn);
        editText = (EditText) findViewById(R.id.tv_value);
        btn_can = (Button) findViewById(R.id.btn_can);
        btn_clr = (Button) findViewById(R.id.btn_clr);
        btn_ok = (Button) findViewById(R.id.btn_ok);
        btn_back = (Button) findViewById(R.id.btn_back);

        tv_maxlength = (TextView) findViewById(R.id.tv_maxlength);
        tv_fieldlength = (TextView) findViewById(R.id.tv_fieldlength);
        tv_inputlength = (TextView) findViewById(R.id.tv_inputlength);

        getData(); // Get data from intent
        setData(); // Set data from intent
        SetInputMaxLength(); // Set Max length entry on edit text
        /*Start timer*/
        //getTimerRestart();

        EditListView.isResume = false;
        btn_ok.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                wub_lib.ButtonPressed(c, btn_ok); // sidumili: Press animation

                sSelectedNewValue = editText.getText().toString();
                Log.i("sidumili", "OKButton Before localposition=" + localposition + ",sSelectedNewValue=" + sSelectedNewValue);
                if (localposition > 0)
                {
                    // sidumili: Check input length
                    if (!checkInputLength())
                        return;

                    getTimerCancel();

                    final_string = "OK";

                    EditListView.isResume = true;

                    Log.i("sidumili", "OKButton final_string=" + final_string);

                    EditListSelected.this.finish();
                }
                else
                {
                    Toast.makeText(getApplicationContext(), "No transaction selected", Toast.LENGTH_SHORT).show();
                }
            }
        });

        btn_can.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                wub_lib.ButtonPressed(c, btn_can); // sidumili: Press animation

                getTimerCancel();

                final_string = "CANCEL";

                //Tine:  try to disable going back to MainActivity
                EditListSelected.this.finish();

                Log.i("PATRICK", "UserCancelUI btn_userCan");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                //synchronized (EditListView.LOCK) {
                //    TransDetailListView.LOCK.setCondition(true);
                //    TransDetailListView.LOCK.notifyAll();
                //}
            }

        });

        btn_clr.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {

                wub_lib.ButtonPressed(c, btn_clr); // sidumili: Press animation

                setData();

                getTimerCancel();

                final_string = "CLEAR";

                editText.setText("");
            }
        });

        editText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                tv_inputlength.setText(sInputLength + editText.length());

            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        });

        btn_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                wub_lib.ButtonPressed(c, btn_back); // sidumili: Press animation

                getTimerCancel();

                final_string = "CANCEL";

                //Tine:  try to disable going back to MainActivity
                EditListSelected.this.finish();

                Log.i("PATRICK", "UserCancelUI btn_back");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                //synchronized (EditListView.LOCK) {
                //    TransDetailListView.LOCK.setCondition(true);
                //    TransDetailListView.LOCK.notifyAll();
                //}
            }

        });
    }

    private void getData()
    {
        Log.i(TAG, "getData: run");

        if (getIntent().hasExtra("data1") && getIntent().hasExtra("data2") && getIntent().hasExtra("data3")
                && getIntent().hasExtra("data4") && getIntent().hasExtra("data5"))
        {
            localdata1 = getIntent().getStringExtra("data1"); // Field Type
            localdata2 = getIntent().getStringExtra("data2"); // Field Description
            localdata3 = getIntent().getStringExtra("data3"); // Field Value
            localdata4 = getIntent().getStringExtra("data4"); // Field Length
            localdata5 = getIntent().getStringExtra("data5"); // Flag Check Length
            localposition = getIntent().getIntExtra("dataindex", 0);

            Log.i("EditListSelected", "getData: localdata1="+localdata1);
            Log.i("EditListSelected", "getData: localdata2="+localdata2);
            Log.i("EditListSelected", "getData: localdata3="+localdata3);
            Log.i("EditListSelected", "getData: localdata4="+localdata4);
            Log.i("EditListSelected", "getData: localdata5="+localdata5);
            Log.i("EditListSelected", "getData: localposition="+localposition);
        }
        else
        {
            Toast.makeText(this, "No data", Toast.LENGTH_SHORT).show();
        }
    }

    private void setData()
    {
        Log.i(TAG, "setData: run");

        textView1.setText(localdata2.toUpperCase()); // Field Description
        textView2.setText(localdata3); // Field Value
        textView_txn.setText(EditListView.sHeaderTitle); // Header Title

        // Set Length Description
        tv_maxlength.setText(sMaxLength + localdata4 + "/" + (localdata5.compareTo("1") == 0 ? "True" : "False"));
        tv_fieldlength.setText(sFieldLength + localdata3.length());
        tv_inputlength.setText(sInputLength + "0");
    }

    // sidumili: check input length in edittext object
    private Boolean checkInputLength()
    {
        Boolean isValid = true;
        Integer iLength = 0;
        Integer iInputLength = 0;

        Log.i(TAG, "checkInputLength: run");

        // Check field enable
        if (Integer.parseInt(localdata5) > 0)
        {
            iLength = Integer.parseInt(localdata4);
            iInputLength = editText.length();

            Log.i(TAG, "checkInputLength: iLength="+iLength + ",iInputLength="+iInputLength);

            if (iLength != iInputLength)
                isValid = false;

        }

        if (!isValid)
            Toast.makeText(this, localdata2.toUpperCase() + " length must be equal to " + iLength, Toast.LENGTH_SHORT).show();

        Log.i(TAG, "checkInputLength: isValid="+isValid);
        return  isValid;
    }

    // sidumili: Function to set max length in edittext object
    private void SetInputMaxLength()
    {
        int iMaxLength = Integer.parseInt(localdata4);

        Log.i(TAG, "SetInputMaxLength: iMaxLength="+iMaxLength);

        InputFilter[] filterArray = new InputFilter[1];
        filterArray[0] = new InputFilter.LengthFilter(iMaxLength);
        editText.setFilters(filterArray);
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
            Log.i("Timer", "Timer onFinish");

            //etInputStr.clear();
            final_string = "TO";

            //startActivity(new Intent(EditListSelected.this, EditListView.class));

            Log.i("PATRICK", "Timeout EditListSelected");
            EditListSelected.this.finish();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            //synchronized (TransDetailListView.LOCK) {
            //    TransDetailListView.LOCK.setCondition(true);
            //    TransDetailListView.LOCK.notifyAll();
            //}
        }
    };

    @Override
    public void onBackPressed() {
        EditListSelected.this.finish();

        Log.i("PATRICK", "Back");

        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
        //synchronized (TransDetailListView.LOCK) {
         //   TransDetailListView.LOCK.setCondition(true);
          //  TransDetailListView.LOCK.notifyAll();
        //}
    }

    @Override
    protected void onDestroy() {
        this.mWakeLock.release();
        super.onDestroy();
        final_string = null;
        // SysApplication.getInstance().removeActivity(this);
    }

}
