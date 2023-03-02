package com.Source.S1_BDO.BDO.model;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;

public class ConfirmMenu extends DemoAppActivity {

    private Context mContext;

    private Button mButton;
    final Context c = this;

    EditText etInputStr;
    Button buOK;
    Button buCancel;

    TextView textView01;
    TextView textView02;
    TextView textView03;
    TextView textView04;
    TextView textView05;
    TextView textView06;
    TextView textView07;
    TextView textView08;
    TextView textView_txn;

    Double dAmount;
    int inResult;
    String stResult;

    boolean androidThinking;
    public static String final_string;
    public static String input_type;

    private int inTimeOut = 30;


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

        setContentView(R.layout.confirm_menu);

        Intent intent=getIntent();
        String dispmsg=intent.getStringExtra("pass_in_string");
        Log.i("dispmsg", dispmsg);

        buOK = (Button) findViewById(R.id.IPT_OKButton);
        buCancel = (Button) findViewById(R.id.IPT_CancelButton);

        textView_txn = (TextView) findViewById(R.id.textView_txn);
        textView01 = (TextView) findViewById(R.id.msg_text_01);
        textView02 = (TextView) findViewById(R.id.msg_text_02);
        textView03 = (TextView) findViewById(R.id.msg_text_03);
        textView04 = (TextView) findViewById(R.id.msg_text_04);
        textView05 = (TextView) findViewById(R.id.msg_text_05);
        textView06 = (TextView) findViewById(R.id.msg_text_06);
        textView07 = (TextView) findViewById(R.id.msg_text_07);
        textView08 = (TextView) findViewById(R.id.msg_text_08);

        String[] dispmsginfo = dispmsg.split("\\|");
		int msgcnt = dispmsginfo.length;

		System.out.println("msgcnt [" + msgcnt + "]");

        for (int inIdx = 0 ; inIdx < msgcnt; inIdx++)
        {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx)
            {
                case 0:
                    textView_txn.setText(dispmsginfo[inIdx]);
                    break;
                case 1:
                    textView01.setText(dispmsginfo[inIdx]);
                    break;
                case 2:
                    textView02.setText(dispmsginfo[inIdx]);
                    break;
                case 3:
                    textView03.setText(dispmsginfo[inIdx]);
                    break;
                case 4:
                    textView04.setText(dispmsginfo[inIdx]);
                    break;
                case 5:
                    textView05.setText(dispmsginfo[inIdx]);
                    break;
                case 6:
                    textView06.setText(dispmsginfo[inIdx]);
                    break;
                case 7:
                    textView07.setText(dispmsginfo[inIdx]);
                    break;
                case 8:
                    textView08.setText(dispmsginfo[inIdx]);
                    break;
            }
        }



//        textView02.setText(dispmsginfo[1]);
//        textView03.setText(dispmsginfo[2]);
//        textView04.setText(dispmsginfo[3]);
//        textView05.setText(dispmsginfo[4]);
//        textView06.setText(dispmsginfo[5]);
//        textView07.setText(dispmsginfo[6]);
//        textView08.setText(dispmsginfo[7]);

//        etInputStr = (EditText) findViewById(R.id.UserInput);
//
//        input_type = value.substring(0, 1);
//        if (input_type.equals("1")) {
//            Log.i("setInputType", "TYPE_CLASS_NUMBER");
//            etInputStr.setInputType(InputType.TYPE_CLASS_NUMBER);
//        }
//
//        if (input_type.equals("2")) {
//            etInputStr.setSingleLine(true);
//            Log.i("setInputType", "TYPE_NUMBER_VARIATION_PASSWORD");
//            etInputStr.setInputType(InputType.TYPE_NUMBER_VARIATION_PASSWORD);
//            //etInputStr.setInputType(InputType.TYPE_TEXT_VARIATION_WEB_PASSWORD);
//
//        }
//
//        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
//        imm.showSoftInput(etInputStr, InputMethodManager.SHOW_IMPLICIT);
//
//        if (input_type.equals("2"))
//        {
//            Log.i("setTransformationMethod", "PasswordTransformationMethod");
//            etInputStr.setTypeface(Typeface.DEFAULT);
//            etInputStr.setTransformationMethod(new PasswordTransformationMethod());
//            //明文显示密码
//            //edittext.setTransformationMethod(HideReturnsTransformationMethod.getInstance());
//            //隐藏密码
//            etInputStr.setTransformationMethod(PasswordTransformationMethod.getInstance());
//
//            etInputStr.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_VARIATION_PASSWORD);
//        }
//        else
//            etInputStr.setTransformationMethod(null); //set android:inputType="numberPassword" in xml and set this to display numeric only
//
//        etInputStr.setOnEditorActionListener(new DoneOnEditorActionListener());
//        etInputStr.setSelection(etInputStr.getText().length());



        /*Start timer*/
        getTimerRestart();

//        etInputStr.addTextChangedListener(new TextWatcher() {
//            @Override
//            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
//
//            }
//
//            @Override
//            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
//
//                Log.i("onTextChanged", "onTextChanged reset timer");
//                /*reset timer*/
//                getTimerCancel(); getTimerRestart();
//            }
//
//            @Override
//            public void afterTextChanged(Editable editable) {
//                if (etInputStr == null) return;
//
//
//                String inputString = editable.toString();
//                etInputStr.removeTextChangedListener(this);
//                String cleanString = inputString.toString().replaceAll("[$,.]", "");
//
//                //BigDecimal bigDecimal = new BigDecimal(cleanString).setScale(2, BigDecimal.ROUND_FLOOR).divide(new BigDecimal(100), BigDecimal.ROUND_FLOOR);
//                //String  converted = NumberFormat.getCurrencyInstance().format(bigDecimal);
//
//                etInputStr.setText(cleanString);
//                etInputStr.setSelection(cleanString.length());
//                etInputStr.addTextChangedListener(this);
//
//            }
//        });

        buOK.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                //dAmount = Double.parseDouble(etAmount.getText().toString());
                final_string = "CONFIRM";
                //final_string = final_string.replaceAll("[$,.]", "");

                //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));
                ConfirmMenu.this.finish();

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

                final_string = "CANCEL";

                //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));
                ConfirmMenu.this.finish();

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
            final_string = "TIME OUT";

            //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));

            Log.i("PATRICK", "Timeout UserInputString");
            ConfirmMenu.this.finish();

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
        final_string = null;
        input_type = null;
		mContext = null;
        // SysApplication.getInstance().removeActivity(this);
    }

}

