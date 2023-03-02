package com.Source.S1_BDO.BDO.Trans;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
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
import com.Source.S1_BDO.BDO.wub_lib;

import static com.Source.S1_BDO.BDO.Main.MultiAP_SubAP.TAG;

public class TipEntryActivity extends DemoAppActivity {

    private Context mContext;

    private Button mButton;
    final Context c = this;

    private Handler aHandler;

    Button btnKey1, btnKey2, btnKey3,btnKey4,btnKey5,btnKey6,btnKey7,btnKey8,btnKey9,btnKey0,btnKeyEnter,btnKeyCancel;
    Button btn_back;

    LinearLayout llCancel;
    LinearLayout llError;
    TextView tvError;

    TextView txtViewAmount, txtViewTitle, textView_txn, textView1, textView_header;
    int amountminlength=0, amountmaxlength=0;
    long iminAmt=0, imaxAmt=0,iAmount=0;
    Double minAmount = 0.00, maxAmount = 0.00, enteredAmount = 0.00;
    String szMinAmount = "", szMaxAmount = "";

    String Amount="";
    public static String final_amt_string;
    private  int amountlength = 0;

    private int inTimeOut = 30;

    private String temp = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);
        super.onCreate(savedInstanceState);

        aHandler = new Handler(Looper.getMainLooper());
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

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.activity_tip_entry);

        Intent intent=getIntent();
        String maxlength=intent.getStringExtra("maxlength");
        String minlength=intent.getStringExtra("minlength");
        String type=intent.getStringExtra("type");


        String dispmsg = intent.getStringExtra("pass_in_string");
        Log.i("dispmsg", dispmsg);



        btnKeyCancel=(Button) findViewById(R.id.ibtnCancel);
        btnKeyEnter=(Button) findViewById(R.id.ibtnEnter);
        btn_back=(Button) findViewById(R.id.btn_back);

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
        btnKeyEnter=(Button) findViewById(R.id.ibtnEnter);

        textView_txn=(TextView) findViewById(R.id.textView_txn) ;
        textView1=(TextView) findViewById(R.id.textView_cur);

        txtViewAmount=(TextView) findViewById(R.id.textViewAmount);
        txtViewAmount.setText("0.00");

        amountmaxlength = Integer.valueOf(maxlength.toString());
        amountminlength = Integer.valueOf(minlength.toString());

        textView_header = (TextView) findViewById(R.id.textView_header);

        llCancel=(LinearLayout) findViewById(R.id.llCancel);

        llError=(LinearLayout) findViewById(R.id.llError);
        tvError=(TextView) findViewById(R.id.tvError);

        String[] dispmsginfo = dispmsg.split("\\|");
        int msgcnt = dispmsginfo.length;

        System.out.println("msgcnt [" + msgcnt + "]");

        for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx) {
                case 0:
                    textView_txn.setText(dispmsginfo[inIdx]);
                    break;
                case 1:
                    textView1.setText(dispmsginfo[inIdx]);
                    break;
                case 2:
                    szMinAmount = dispmsginfo[inIdx];
                    iminAmt = Integer.valueOf(szMinAmount);
                    //test
                    Log.i("saturn","saturn szMinAmount: "+ szMinAmount );
                    if (iminAmt > 99){
                        temp = (szMinAmount.substring(0,(szMinAmount.length()-2))) + "." + (szMinAmount.substring(szMinAmount.length()-2));
                        szMinAmount = temp;
                    }else{

                    }
                    minAmount = Double.valueOf(szMinAmount);
                    //minAmount = Double.parseDouble(temp);
                    Log.i("saturn","saturn minAmount: " + minAmount + " temp: " + szMinAmount );
                    temp = "";
                    break;
                case 3:
                    szMaxAmount = dispmsginfo[inIdx];
                    imaxAmt = Long.valueOf(szMaxAmount);
                    temp = szMaxAmount.substring(0,(szMaxAmount.length()-2)) + "." + szMaxAmount.substring(szMaxAmount.length()-2);
                    szMaxAmount = temp;
                    maxAmount = Double.valueOf(szMaxAmount);
                    //maxAmount = Double.parseDouble(szMaxAmount);
                    Log.i("tine","maxAmount: " + maxAmount + " temp" + szMaxAmount);
                    temp = "";
                    break;
            }
        }

        llError.setVisibility(View.INVISIBLE);

        // Set textView_header
        if (textView_txn.getText().toString().toUpperCase().equals("TIP ADJUST"))
        {
            textView_header.setText("Enter adjustment amount");
        }
        else
        {
            textView_header.setText("Please enter amount");
        }

        /*Start timer*/
        getTimerRestart();

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
        FuncKeyEnter();
        FuncKeyBack();
    }

    public void FuncKey1() {
        btnKey1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }
                Amount=Amount+"1";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();
            }
        });
    }

    public void FuncKey2() {
        btnKey2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }
                Amount=Amount+"2";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();
            }
        });
    }

    public void FuncKey3() {
        btnKey3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }
                Amount=Amount+"3";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();
            }
        });
    }

    public void FuncKey4() {
        btnKey4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }
                Amount=Amount+"4";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();
            }
        });
    }

    public void FuncKey5() {
        btnKey5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }
                Amount=Amount+"5";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();

            }
        });
    }

    public void FuncKey6() {
        btnKey6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }
                Amount=Amount+"6";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();
            }
        });
    }

    public void FuncKey7() {
        btnKey7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }
                Amount=Amount+"7";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();
            }
        });
    }

    public void FuncKey8() {
        btnKey8.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }
                Amount=Amount+"8";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();
            }
        });
    }

    public void FuncKey9() {
        btnKey9.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                Log.i(TAG, "FuncKey9: amountmaxlength="+amountmaxlength+",Amount.length()="+Amount.length()+",Amount="+Amount);
                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }
                Amount=Amount+"9";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();
            }
        });
    }

    public void FuncKey0() {
        btnKey0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                Log.i(TAG, "FuncKey0: amountmaxlength="+amountmaxlength+",Amount.length()="+Amount.length()+",Amount="+Amount+",txtViewAmount.getText()="+txtViewAmount.getText().toString());
                if(Amount.length() <= 0)
                    return;

                if(Amount.length() >= amountmaxlength) {
                    //Toast.makeText(getApplicationContext(), "Maximum of " + amountmaxlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Maximum of "+amountmaxlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }

                Amount=Amount+"0";
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                setAmount();
            }
        });
    }

    public void FuncKeyCancel() {
        btnKeyCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() > 0) {
                    Amount = Amount.substring(0, Amount.length()-1);
                }
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                Amount = txtViewAmount.getText().toString().replace(",", "").replace(".","");
                setAmount();

            }
        });

        llCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                getTimerRestart();

                if(Amount.length() > 0) {
                    Amount = Amount.substring(0, Amount.length()-1);
                }
                //txtViewAmount.setText(tools.getFormatAmount(Amount).toString());
                //txtViewAmount.setText(Amount);
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
                Amount = txtViewAmount.getText().toString().replace(",", "").replace(".","");
                setAmount();
            }
        });
    }

    public void FuncKeyEnter() {
        btnKeyEnter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                /*cancel timer first*/
                getTimerCancel();

                //dAmount = Double.parseDouble(etAmount.getText().toString());
                //final_amt_string = etAmount.getText().toString();
                //final_amt_string="12345678";
                if(Amount.length() <= 0) {
                    //Toast.makeText(getApplicationContext(), "Please Enter Amount", Toast.LENGTH_SHORT).show();
                    ShowError(true, "Invalid Amount");
                    return;
                }
                else if(Amount.length() < amountminlength) {
                    //Toast.makeText(getApplicationContext(), "Minimum length is " + amountminlength, Toast.LENGTH_SHORT).show();
                    //ShowError(true, "Minimum length is "+amountminlength);
                    ShowError(true, "Invalid Amount");
                    return;
                }

                //temp = (Amount.substring(0,(Amount.length()-2))) + "." + (Amount.substring(Amount.length()-2));
                //Log.i("temp", temp);
                //enteredAmount = Double.valueOf(temp);
                iAmount = Long.valueOf(Amount);
                if(iAmount < iminAmt || iAmount > imaxAmt)
                {
                    getTimerRestart();
                    CheckAmount();
                }
                else
                {
                    final_amt_string=Amount;

                    //final_amt_string = final_amt_string.replaceAll("[$,.]", "");

                    //startActivity(new Intent(AmountEntryActivity.this,MainActivity.class));
                    TipEntryActivity.this.finish();

                    Log.i("Tine", "Get Amount KeyBoard buOK");

                    // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                    synchronized (MainActivity.LOCK) {
                        MainActivity.LOCK.setCondition(true);
                        MainActivity.LOCK.notifyAll();

                    }
                }

                if(imaxAmt == 0)
                {
                    final_amt_string=Amount;

                    //final_amt_string = final_amt_string.replaceAll("[$,.]", "");

                    //startActivity(new Intent(AmountEntryActivity.this,MainActivity.class));
                    TipEntryActivity.this.finish();

                    Log.i("Tine", "Get Amount KeyBoard buOK");

                    // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                    synchronized (MainActivity.LOCK) {
                        MainActivity.LOCK.setCondition(true);
                        MainActivity.LOCK.notifyAll();

                    }
                }


            }

        });
    }

    public void FuncKeyBack() {
        btn_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if(Amount.length() > 0){
                    getTimerRestart();
                    Amount = "";
                    txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());

                } else {
                    getTimerCancel();
                    //Toast.makeText(getApplicationContext(), "Cancelling...", Toast.LENGTH_SHORT).show();

                    final_amt_string = "CANCEL";
                    //startActivity(new Intent(AmountEntryActivity.this, MainActivity.class));
                    TipEntryActivity.this.finish();

                    Log.i("PATRICK", "Get Amount KeyBoard buCancel");
                    // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                    synchronized (MainActivity.LOCK) {
                        MainActivity.LOCK.setCondition(true);
                        MainActivity.LOCK.notifyAll();
                    }


                }

            }
        });
    }

    private void CheckAmount()
    {
        if(iAmount < iminAmt)
        {
            //Toast.makeText(getApplicationContext(), "MIN AMOUNT " + szMinAmount, Toast.LENGTH_SHORT).show();
            //ShowError(true, "Minimum amount of "+szMinAmount);
            if (textView_txn.getText().toString().toUpperCase().equals("INSTALLMENT"))
                ShowError(true, "Minimum amount of "+szMinAmount);
            else
                ShowError(true, "Invalid Amount");

            Amount = "";
            txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
        }
        else if(iAmount > imaxAmt)
        {
            String tranType = textView_txn.getText().toString();

            if(iAmount < iminAmt)
            {
                //Toast.makeText(getApplicationContext(), "MIN AMOUNT " + szMinAmount, Toast.LENGTH_SHORT).show();
                ShowError(true, "Minimum amount of "+szMinAmount);
                Amount = "";
                txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
            }
            else if(iAmount > imaxAmt)
            {
                if (tranType.equals("TIP ADJUST"))
                    //Toast.makeText(getApplicationContext(), "TOO MUCH TIP", Toast.LENGTH_SHORT).show();
                    ShowError(true, "Too much tip");
                else
                    //Toast.makeText(getApplicationContext(), "OUT OF RANGE", Toast.LENGTH_SHORT).show();
                    ShowError(true, "Out of range");
            }
            Amount = "";

            txtViewAmount.setText(vdCTOSS_FormatAmountUI("NN,NNN,NNN,NNn.nn", Amount).toString());
        }
    }

    private void ShowError(boolean isError, String pMessage)
    {
        llError.setVisibility(View.INVISIBLE);
        tvError.setText(pMessage);

        if (isError)
        {
            llError.setVisibility(View.VISIBLE);
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
        ShowError(false, "");
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
            final_amt_string = "TIME OUT";

            //startActivity(new Intent(AmountEntryActivity.this, MainActivity.class));

            Log.i("PATRICK", "Timeout AmountEntryActivity");
            TipEntryActivity.this.finish();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };

    void setAmount()
    {
        Log.i(TAG, "setAmount: run");
        Amount = txtViewAmount.getText().toString().replace(",", "").replace(".","");
        amountlength = Amount.length();

        Log.i(TAG, "amountlength="+amountlength+",Amount="+Amount);
    }

    boolean isValidAmount(String pAmount, String pMaxLength)
    {
        boolean isValid = false;
        Log.i(TAG, "isValidAmount: pAmount="+pAmount+",len="+pAmount.length()+",pMaxLength="+pMaxLength);


        return isValid;
    }

    @Override
    public void onBackPressed() {
        getTimerCancel();

        final_amt_string = "CANCEL";
        //startActivity(new Intent(AmountEntryActivity.this, MainActivity.class));
        TipEntryActivity.this.finish();

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
        textView_txn.setText("");
        final_amt_string = null;
        mContext = null;
        aHandler = null;
        // // SysApplication.getInstance().removeActivity(this);

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String vdCTOSS_FormatAmountUI(String szFmt, String szInAmt);
}
