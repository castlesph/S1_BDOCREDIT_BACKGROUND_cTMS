package com.Source.S1_BDO.BDO.Trans;

import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

public class GetCardDetailsActivity extends DemoAppActivity {


    Button btnKey1, btnKey2, btnKey3,btnKey4,btnKey5,btnKey6,btnKey7,btnKey8,btnKey9,
            btnKey0,btnKeyEnter,btnKeyCancel,btnKeyClear;
    TextView textView_txn;
    TextView textView_disp;
    TextView textView_entry;
    String userentry="", maskedentry="", password="", passwordchar="*";


    public static String final_amt_string;
    public static String final_string;
    int invmaxlength=0, invminlength=0, typedef=0;


    private int inTimeOut = 20;
    String display, maxlength, minlength, type, password1="", password2="", password3="";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);
        super.onCreate(savedInstanceState);
        Bundle bundle = getIntent().getExtras();

        overridePendingTransition(0, 0); // disable the animation, faster

        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);    //Tine:  Show Status Bar
        setContentView(R.layout.activity_card_entry);

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
        btnKeyClear=(Button) findViewById(R.id.ibtnClear);
        btnKeyEnter=(Button) findViewById(R.id.ibtnEnter);


        textView_txn = (TextView) findViewById(R.id.textView_txn);
        textView_entry= (TextView) findViewById(R.id.textView_entry);
        textView_disp = (TextView) findViewById(R.id.textView_disp);

        String[] dispmsginfo = value.split("\\|");
        int msgcnt = dispmsginfo.length;

        System.out.println("msgcnt [" + msgcnt + "]");
        for (int inIdx = 0 ; inIdx < msgcnt; inIdx++)
        {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
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
                    textView_txn.setText(dispmsginfo[inIdx]);
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

        getTimerRestart();

        //type=dispmsginfo[0];
        //min
        //max
        //display=dispmsginfo[3];

        //if(dispmsginfo[4].length() > 0)
        //password1=dispmsginfo[4];
        //if(dispmsginfo[5].length() > 0)
        //password2=dispmsginfo[5];
        //if(dispmsginfo[6].length() > 0)
        //spassword3=dispmsginfo[6];

        //Toast.makeText(getApplicationContext(), "password1: "+password1, Toast.LENGTH_SHORT).show();
        //Toast.makeText(getApplicationContext(), "password2: "+password2, Toast.LENGTH_SHORT).show();
        //Toast.makeText(getApplicationContext(), "password3: "+password3, Toast.LENGTH_SHORT).show();

        //display = "ENTER PASSWORD";
        //minlength=intent.getStringExtra("PARAM_MINLENGTH");
        //maxlength=intent.getStringExtra("PARAM_MAXLENGTH");

        //password = intent.getStringExtra("PARAM_PASSWORD");


        //invmaxlength=Integer.valueOf(maxlength.toString());
        //invminlength=Integer.valueOf(minlength.toString());
        typedef = Integer.valueOf(type);
        invminlength = Integer.valueOf(minlength);
        invmaxlength = Integer.valueOf(maxlength);

        //Toast.makeText(getApplicationContext(), "display: "+display, Toast.LENGTH_SHORT).show();
        //Toast.makeText(getApplicationContext(), "minlength: "+minlength, Toast.LENGTH_SHORT).show();
        //Toast.makeText(getApplicationContext(), "maxlength: "+maxlength, Toast.LENGTH_SHORT).show();
        //Toast.makeText(getApplicationContext(), "type: "+type, Toast.LENGTH_SHORT).show();

        Log.i("pass in value", value);



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
        FuncKeyClear();
        FuncKeyEnter();

    }

    public void FuncKey1() {
        btnKey1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                userentry = userentry + "1";

                //Type 1 - numeric
                if(typedef == 1) {
                    if (userentry.length() > invmaxlength) {
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                        Toast.makeText(getApplicationContext(), "Maximum of 6 digits only", Toast.LENGTH_SHORT).show();
                        userentry = userentry.substring(0, userentry.length() - 1);
                        return;
                    }
                    textView_entry.setText(userentry);
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
                GetCardDetailsActivity.this.finish();

                Log.i("PATRICK", "Get Amount KeyBoard buCancel");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
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
                    textView_entry.setText(userentry);
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
                /*cancel timer first*/
                getTimerCancel();

                //dAmount = Double.parseDouble(etAmount.getText().toString());
                //final_amt_string = etAmount.getText().toString();
                //final_amt_string="12345678";
                if(typedef == 1) {

                    if (userentry.length() < invminlength) {
                        Toast.makeText(getApplicationContext(), "Minimum length must be " + String.valueOf(invminlength), Toast.LENGTH_LONG).show();
                        userentry="";
                        return;
                    }

                    final_string = userentry;
                    GetCardDetailsActivity.this.finish();

                    Log.i("Tine", "Get Invoice KeyBoard buOK");
                }
                else if(typedef == 2){
                    if (userentry.length() < invminlength) {
                        Toast.makeText(getApplicationContext(), "Minimum length must be " + String.valueOf(invminlength), Toast.LENGTH_SHORT).show();
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
                    final_string = userentry;
                    GetCardDetailsActivity.this.finish();
                }


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
            final_string = "TO";

            //startActivity(new Intent(S1InputString.this,MainActivity.class));

            Log.i("PATRICK", "Timeout UserInputString");
            GetCardDetailsActivity.this.finish();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };

    @Override
    public void onBackPressed() {
        //startActivity(new Intent(S1InputString.this,MainActivity.class));
        getTimerCancel();
        final_string = "CANCEL";
        GetCardDetailsActivity.this.finish();

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
        // SysApplication.getInstance().removeActivity(this);
    }

}
