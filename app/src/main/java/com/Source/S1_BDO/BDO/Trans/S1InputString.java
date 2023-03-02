package com.Source.S1_BDO.BDO.Trans;

import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.text.InputType;
import android.text.method.PasswordTransformationMethod;
import android.util.Log;
import android.view.Gravity;
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
import com.Source.S1_BDO.BDO.model.MenuSelection;
import com.Source.S1_BDO.BDO.wub_lib;

import static android.content.ContentValues.TAG;

public class S1InputString extends DemoAppActivity {

    private static final String TAG = "S1InputString";
    Button btnKey1, btnKey2, btnKey3,btnKey4,btnKey5,btnKey6,btnKey7,btnKey8,btnKey9,
            btnKey0,btnKeyEnter,btnKeyCancel,btnKeyClear;

    LinearLayout llCancel;
    TextView textView_txn;
    TextView textView_disp;
    TextView textView_entry;

    LinearLayout linearLayout_entry;
    Button btn_back;

    String userentry="", maskedentry="", password="", passwordchar="*";
    String sztxn="";


    public static String final_amt_string;
    public static String final_string;
    int invmaxlength=0, invminlength=0, typedef=0;
    private int inDelay = 0;


    private int inTimeOut = 30;
    String display, maxlength, minlength, type, password1="", password2="", password3="";

    private int pPadType = 0;

    private CountDownTimer timer = null; // sidumili - init timer, set private variable

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

        setContentView(R.layout.activity_user_entry);

        Intent intent=getIntent();
        String value=intent.getStringExtra("pass_in_string");
        Log.i("value", value);

        btnKey1=(Button) findViewById(R.id.ibtnOne);
        btnKey2=(Button) findViewById(R.id.ibtnTwo);
        btnKey3=(Button) findViewById(R.id.ibtnThree);
        btnKey4=(Button) findViewById(R.id.ibtnFour);
        btnKey5=(Button) findViewById(R.id.ibtnFive);
        btnKey6=(Button) findViewById(R.id.ibtnSix);
        btnKey7=(Button) findViewById(R.id.ibtnSeven);
        btnKey8=(Button) findViewById(R.id.ibtnEight);
        btnKey9=(Button) findViewById(R.id.ibtnNine);
        btnKey0=(Button) findViewById(R.id.ibtnZero);
        btnKeyCancel=(Button) findViewById(R.id.ibtnCancel);
        //btnKeyClear=(Button) findViewById(R.id.ibtnClear);
        btnKeyEnter=(Button) findViewById(R.id.ibtnEnter);


        textView_txn = (TextView) findViewById(R.id.textView_txn);
        textView_entry= (TextView) findViewById(R.id.textView_entry);
        textView_disp = (TextView) findViewById(R.id.textView_disp);

        linearLayout_entry = (LinearLayout) findViewById(R.id.linearLayout_entry);
        btn_back =(Button) findViewById(R.id.btn_back);
        llCancel=(LinearLayout) findViewById(R.id.llCancel);

        String[] dispmsginfo = value.split("\\|");
        int msgcnt = dispmsginfo.length;

        System.out.println("msgcnt [" + msgcnt + "]");
        for (int inIdx = 0 ; inIdx < msgcnt; inIdx++)
        {
            System.out.println("S1InputString->split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx)
            {
                case 0:
                    type=dispmsginfo[inIdx];
                    break;
                case 1:
                    minlength=dispmsginfo[inIdx];
                    break;
                case 2:
                    maxlength=dispmsginfo[inIdx];
                    break;
                case 3:
                    sztxn = dispmsginfo[inIdx];
                    textView_txn.setText(sztxn);
                    if (dispmsginfo[inIdx].equals("Void"))
                    {
                        Log.i(TAG, "Void,Delay 180");
                        inDelay = 180;
                    }
                    else {
                        Log.i(TAG, "Void,Delay 1");
                        inDelay = 1;
                    }
                    break;
                case 4:
                    textView_disp.setText(dispmsginfo[inIdx]);
                    break;
                case 5:
                    password1=dispmsginfo[inIdx];
                    break;
                case 6:
                    password2=dispmsginfo[inIdx];
                    break;
                case 7:
                    password3=dispmsginfo[inIdx];
                    break;
            }
        }

        Log.i(TAG, "sztxn="+sztxn);
        //if(!sztxn.equals("VOID"))
        getTimerRestart();

        typedef = Integer.valueOf(type);
        invminlength = Integer.valueOf(minlength);
        invmaxlength = Integer.valueOf(maxlength);

        Log.i(TAG, "typedef="+typedef);
        Log.i(TAG, "invminlength="+invminlength);
        Log.i(TAG, "invmaxlength="+invmaxlength);
        Log.i(TAG, "password1="+password1);
        Log.i(TAG, "password2="+password2);
        Log.i(TAG, "password3="+password3);

        Log.i("pass in value", value);

        textView_entry.setTextSize(30); // initial size;  // sidumili

        // Set InputType / background -- sidumili
        if (typedef == 1) // Numeric
        {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
                textView_entry.setTextAlignment(View.TEXT_ALIGNMENT_TEXT_START);
            }

            if (textView_disp.getText().toString().toUpperCase().equals("ENTER APPROVAL CODE"))
            {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
                    textView_entry.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
                }
                textView_entry.setTextColor(Color.BLACK);
                linearLayout_entry.setBackgroundResource(R.drawable.shape_white_bg);
            }
            else
            {
                textView_entry.setTextColor(Color.WHITE);
                linearLayout_entry.setBackgroundColor(Color.TRANSPARENT);
            }
        }
        if (typedef == 2) // Password
        {
            textView_entry.setTextSize(40); // initial size;

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
                textView_entry.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
            }
            textView_entry.setInputType(InputType.TYPE_TEXT_VARIATION_PASSWORD);
            textView_entry.setTextColor(Color.BLACK);
            linearLayout_entry.setBackgroundResource(R.drawable.shape_white_bg);

            textView_entry.setTransformationMethod(PasswordTransformationMethod.getInstance()); // show system password instead of * -- sidumili

        }

        // Set pad type for auto format of display
        if (textView_disp.getText().toString().toUpperCase().equals("PLEASE ENTER CARD NUMBER"))
            pPadType = 1; // for card number
        else if (textView_disp.getText().toString().toUpperCase().equals("PLEASE ENTER EXPIRY DATE"))
            pPadType = 2; // for expiry date

        FuncKey1();
        FuncKey2();
        FuncKey3();
        FuncKey4();
        FuncKey5();
        FuncKey6();
        FuncKey7();
        FuncKey8();
        FuncKey9();
        FuncKey0();
        FuncKeyCancel();
        //FuncKeyClear();
        FuncKeyEnter();
        FuncKeyBack();

    }

    public void FuncKey1() {
        btnKey1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                userentry = userentry + "1";

                //Type 1 - numeric
                if(typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }

                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }
                //Type 2 = password
                if(typedef == 2){
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }
            }
        });
    }

    public void FuncKey2() {
        btnKey2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "2";

                //Type 1 - numeric
                if(typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }

                //Type 2 = password
                if(typedef == 2){
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }

            }
        });
    }

    public void FuncKey3() {
        btnKey3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "3";

                //Type 1 - numeric
                if (typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }

                //Type 2 = password
                if (typedef == 2) {
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }

            }
        });
    }

    public void FuncKey4() {
        btnKey4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "4";
                //Type 1 - numeric
                if (typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }

                //Type 2 = password
                if (typedef == 2) {
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }

            }
        });
    }

    public void FuncKey5() {
        btnKey5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "5";
                //Type 1 - numeric
                if (typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }

                //Type 2 = password
                if (typedef == 2) {
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }


            }
        });
    }

    public void FuncKey6() {
        btnKey6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "6";
                //Type 1 - numeric
                if (typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }

                //Type 2 = password
                if (typedef == 2) {
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }

            }
        });
    }

    public void FuncKey7() {
        btnKey7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "7";
                //Type 1 - numeric
                if (typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }

                //Type 2 = password
                if (typedef == 2) {
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }

            }
        });
    }

    public void FuncKey8() {
        btnKey8.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "8";
                //Type 1 - numeric
                if (typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }

                //Type 2 = password
                if (typedef == 2) {
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }

            }
        });
    }

    public void FuncKey9() {
        btnKey9.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "9";
                //Type 1 - numeric
                if (typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }

                //Type 2 = password
                if (typedef == 2) {
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }

            }
        });
    }


    public void FuncKey0() {
        btnKey0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "0";
                //Type 1 - numeric
                if (typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        //Toast.makeText(getApplicationContext(), "Maximum of " +invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }

                //Type 2 = password
                if (typedef == 2) {
                    maskedentry = maskedentry + passwordchar;
                    textView_entry.setText(maskedentry);
                }

            }
        });
    }

    public void FuncKeyCancel(){
        btnKeyCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                /*cancel timer first*/
                getTimerCancel();

                if(userentry.length() > 0) {
                    userentry = maskedentry = userentry.substring(0, userentry.length()-1);
                }

                String pTemp = vdAutoPadding(userentry);
                textView_entry.setText(pTemp);

            }
        });

        llCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                /*cancel timer first*/
                getTimerCancel();

                if(userentry.length() > 0) {
                    userentry = maskedentry = userentry.substring(0, userentry.length()-1);
                }

                String pTemp = vdAutoPadding(userentry);
                textView_entry.setText(pTemp);

            }
        });
    }

    public void FuncKeyClear() {
        btnKeyClear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if(userentry.length() > 0) {
                    userentry = userentry.substring(0, userentry.length()-1);
                    if(typedef == 2)
                        maskedentry = maskedentry.substring(0, maskedentry.length()-1);
                }

                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                if(typedef == 1)
                {
                    String pTemp = vdAutoPadding(userentry);
                    textView_entry.setText(pTemp);
                }
                else if(typedef == 2){
                    textView_entry.setText(maskedentry);
                }

            }
        });
    }

    public void FuncKeyEnter() {
        btnKeyEnter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                Log.i(TAG, "onClick: FuncKeyEnter");
                Log.i(TAG, "onClick: password1="+password1);
                Log.i(TAG, "onClick: password2="+password2);
                Log.i(TAG, "onClick: userentry="+userentry);
                Log.i(TAG, "onClick: typedef="+typedef);

                /*cancel timer first*/
                getTimerCancel();

                //dAmount = Double.parseDouble(etAmount.getText().toString());
                //final_amt_string = etAmount.getText().toString();
                //final_amt_string="12345678";
                if(typedef == 1) {

                    if (userentry.length() < invminlength) {
                        //Toast.makeText(getApplicationContext(), "Minimum length must be " + String.valueOf(invminlength), Toast.LENGTH_LONG).show();
                        userentry="";
                        return;
                    }

                    final_string = userentry.replace(" ", "").replace("/", ""); // replace space and / to "" -- sidumili
                    //S1InputString.this.finish();
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            wub_lib.finishActivity(S1InputString.this);
                        }
                    }, 180);

                    Log.i("Tine", "Get Invoice KeyBoard buOK");
                }
                else if(typedef == 2){
                    if (userentry.length() < invminlength) {
                        //Toast.makeText(getApplicationContext(), "Minimum length must be " + String.valueOf(invminlength), Toast.LENGTH_SHORT).show();
                        return;
                    }

                    if(password1.length() > 0 && password1.equals(userentry) || password2.length() > 0 && password2.equals(userentry) ||
					password3.length() > 0 && password3.equals(userentry))
                    {
                        //Toast.makeText(getApplicationContext(), "PASSWORD CORRECT", Toast.LENGTH_SHORT).show();
                        //final_string = userentry;
                        //S1InputString.this.finish();
                    }
                    else
                    {
                        Toast.makeText(getApplicationContext(), "WRONG PASSWORD", Toast.LENGTH_SHORT).show();
                        userentry = "";
                        maskedentry = "";
                        textView_entry.setText(maskedentry);
                        return;
                    }
                    final_string = userentry.replace(" ", "").replace("/", ""); // replace space and / to "" -- sidumili
					//S1InputString.this.finish();
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                    wub_lib.finishActivity(S1InputString.this);
                        }
                    }, 180);

                }

                try {
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

        });
    }

    public void FuncKeyBack(){
        btn_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                /*cancel timer first*/
                getTimerCancel();

                if(userentry.length() > 0) {
                    userentry = "";
                    textView_entry.setText(userentry);

                    if(typedef == 2){
                        maskedentry = "";
                        textView_entry.setText(maskedentry);
                    }
                    return;
                }

                final_string = "CANCEL";
                //startActivity(new Intent(S1InputString.this, MainActivity.class));

                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                   S1InputString.this.finish();
                    }
                }, inDelay);

                Log.i("PATRICK", "Get Amount KeyBoard btn_back");

                try {
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
        });
    }

    // auto-padding of space for PAN -- sidumili
    // eg. 1234 5678 9012 3456 -- cardnumber
    // eg. 12/34 -- expiry date
    public String vdAutoPadding(String pString)
    {
        String pPaddString = "";
        int pLen = pString.length();
        int i = 0;
        String pTemp = "";
        String pAddChar = "";

        Log.i(TAG, "vdAutoPadding: ");
        Log.i(TAG, "len="+pString.length()+",pString="+ "["+pString+"]");
        Log.i(TAG, "invmaxlength="+invmaxlength);
        if (pLen > 0)
        {
            String[] str_arr = pString.split("");
            for (String ch : str_arr) {
                //System.out.println("i="+i);

                switch (pPadType)
                {
                    case 1: // card number
                        if (i % 4 == 0 && i > 0)
                        {
                            if (i >= invmaxlength)
                                pAddChar = "";
                            else
                                pAddChar = " ";
                        }
                        else
                        {
                            pAddChar = "";
                        }
                        break;
                    case 2: // expiry date
                        if (i % 2 == 0 && i > 0)
                        {
                            if (i >= invmaxlength)
                                pAddChar = "";
                            else
                                pAddChar = "/";
                        }
                        else
                        {
                            pAddChar = "";
                        }
                        break;
                }
                i++;

                pPaddString = pPaddString + ch + pAddChar;
            }

            Log.i(TAG, "pPaddString="+ "["+pPaddString+"]");

        }

        return  pPaddString;
    }

    /**
     * 取消倒计时
     */
    public void getTimerCancel() {
        if (timer != null)
            timer.cancel();
    }

    /**
     * 开始倒计时
     */
    public void getTimerRestart()
    {
        timer = new CountDownTimer(inTimeOut*1000, 1000) {
            public void onTick(long millisUntilFinished) {
                Log.i("Timer", "Timer onTick");
                Log.i("Timer", "seconds remaining: " + millisUntilFinished / 1000);
            }
            public void onFinish() {
                Log.i("Timer", "Timer onFinish");

                //etInputStr.clear();
                final_string = "TO";

                Log.i("PATRICK", "Timeout S1InputString");
                //S1InputString.this.finish();
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        S1InputString.this.finish();
                    }
                }, inDelay);

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();
                }
            }
        };
        timer.start();
    }

    /*
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

            //startActivity(new Intent(S1InputString.this,MainActivity.class));

            Log.i("PATRICK", "Timeout UserInputString");
            S1InputString.this.finish();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };
    */

    @Override
    public void onBackPressed() {
        Log.i(TAG, "onBackPressed: run");
        //startActivity(new Intent(S1InputString.this,MainActivity.class));
        getTimerCancel();
        final_string = "CANCEL";
        //S1InputString.this.finish();
        wub_lib.finishActivity(S1InputString.this);

        Log.i("PATRICK", "Back");

        // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        final_string = null;
        final_amt_string = null;
        timer = null;
        // SysApplication.getInstance().removeActivity(this);
    }

}
