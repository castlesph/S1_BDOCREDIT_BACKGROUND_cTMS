package com.Source.S1_BDO.BDO.Trans;

import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.content.Context;
import android.icu.math.BigDecimal;
import android.icu.text.NumberFormat;
import android.os.CountDownTimer;
import android.support.annotation.RequiresApi;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

import static android.content.ContentValues.TAG;

public class GetAmount extends DemoAppActivity {

    private Context mContext;

    private Button mButton;
    final Context c = this;

    EditText etAmount;
    Button buOK;
    Button buCancel;
    TextView tvMessage;

    Double dAmount;
    int inResult;
    String stResult;

    boolean androidThinking;
    public static String final_amt_string;

    private int inTimeOut = 30;

    private String any_amt = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate: GetAmount");
        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);
        super.onCreate(savedInstanceState);
        Bundle bundle = getIntent().getExtras();

        overridePendingTransition(0, 0); // disable the animation, faster

        setContentView(R.layout.prompt_amount_main);

        Intent intent=getIntent();
        String value=intent.getStringExtra("pass_in_string");
        Log.i("main pass in value", value);

        tvMessage = (TextView) findViewById(R.id.MessageLabel);
        etAmount = (EditText) findViewById(R.id.AmountInput);
        //etAmount.setSelection(etAmount.getText().length()); //move bottom can work

        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.showSoftInput(etAmount, InputMethodManager.SHOW_IMPLICIT);

        etAmount.setTransformationMethod(null); //set android:inputType="numberPassword" in xml and set this to display numeric only
        etAmount.setOnEditorActionListener(new DoneOnEditorActionListener());
        etAmount.setSelection(etAmount.getText().length());

        buOK = (Button) findViewById(R.id.OKButton);
        buCancel = (Button) findViewById(R.id.CancelButton);

        /*Start timer*/
        getTimerRestart();

		etAmount.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

                Log.i("onTextChanged", "onTextChanged reset timer");
                /*reset timer*/
                getTimerCancel(); getTimerRestart();
            }

            @RequiresApi(api = Build.VERSION_CODES.N)
            @Override
            public void afterTextChanged(Editable editable) {
                if (etAmount == null) return;


                String inputString = editable.toString();
                etAmount.removeTextChangedListener(this);
                String cleanString = inputString.toString().replaceAll("[$,.]", "");
                //String cleanString = inputString.toString();
                BigDecimal bigDecimal = new BigDecimal(cleanString).setScale(2, BigDecimal.ROUND_FLOOR).divide(new BigDecimal(100), BigDecimal.ROUND_FLOOR);
                String  converted = NumberFormat.getCurrencyInstance().format(bigDecimal);
                //String  converted = NumberFormat.getNumberInstance().format(bigDecimal);

                etAmount.setText(converted);
                etAmount.setSelection(converted.length());
                etAmount.addTextChangedListener(this);

            }
        });

        buOK.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                //dAmount = Double.parseDouble(etAmount.getText().toString());
                final_amt_string = etAmount.getText().toString();
                final_amt_string = final_amt_string.replaceAll("[$,.]", "");

                //startActivity(new Intent(GetAmount.this,MainActivity.class));
                GetAmount.this.finish();

                Log.i("PATRICK", "Get Amount KeyBoard buOK");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }

            }
        });

        buCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                final_amt_string = "CANCEL";

                //startActivity(new Intent(GetAmount.this,MainActivity.class));
                GetAmount.this.finish();

                Log.i("PATRICK", "Get Amount KeyBoard buCancel");

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

            //etInputStr.clear();
            final_amt_string = "TO";

            //startActivity(new Intent(GetAmount.this,MainActivity.class));

            Log.i("PATRICK", "Timeout UserInputString");
            GetAmount.this.finish();

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
        super.onDestroy();
        final_amt_string = null;
		mContext = null;
    }

}

