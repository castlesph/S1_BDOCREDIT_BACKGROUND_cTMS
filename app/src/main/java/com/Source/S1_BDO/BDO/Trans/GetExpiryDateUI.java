package com.Source.S1_BDO.BDO.Trans;

import android.app.AlertDialog;
import android.content.DialogInterface;
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

public class GetExpiryDateUI extends DemoAppActivity {


    Button btnKey1, btnKey2, btnKey3,btnKey4,btnKey5,btnKey6,btnKey7,btnKey8,btnKey9,
            btnKey0,btnKeyEnter,btnKeyCancel,btnKeyClear;
    TextView textView_txn;
    TextView textView_detail1;
    TextView textView_detail1value;
    TextView textView_detail2;
    TextView textView_detail2value;
    String userentry="", formatinputstr="";


    public static String final_string;
    int invmaxlength=0, invminlength=0;
    int month=0;


    private int inTimeOut = 60;
    String maxlength, minlength;

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

        setContentView(R.layout.activity_getexpirydate);

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
        textView_detail1= (TextView) findViewById(R.id.textView_detail1);
        textView_detail1value = (TextView) findViewById(R.id.textView_detail1value);
        textView_detail2= (TextView) findViewById(R.id.textView_detail2);
        textView_detail2value = (TextView) findViewById(R.id.textView_detail2value);

        String[] dispmsginfo = value.split("\\|");
        int msgcnt = dispmsginfo.length;

        System.out.println("msgcnt [" + msgcnt + "]");
        for (int inIdx = 0 ; inIdx < msgcnt; inIdx++)
        {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx)
            {
                case 0:
                    minlength=dispmsginfo[inIdx];
                    break;
                case 1:
                    maxlength=dispmsginfo[inIdx];
                    break;
                case 2:
                    textView_txn.setText(dispmsginfo[inIdx]);
                    break;
                case 3:
                    textView_detail1.setText(dispmsginfo[inIdx]);
                    break;
                case 4:
                    textView_detail1value.setText(dispmsginfo[inIdx]);
                    break;
                case 5:
                    textView_detail2.setText(dispmsginfo[inIdx]);
                    break;
            }
        }

        //getTimerRestart();

        invminlength = Integer.valueOf(minlength);
        invmaxlength = Integer.valueOf(maxlength);

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

    private void FuncKey1() {
        btnKey1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                userentry = userentry + "1";


                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());

            }
        });
    }

    private void FuncKey2() {
        btnKey2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "2";

                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());

            }
        });
    }

    private void FuncKey3() {
        btnKey3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "3";

                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());

            }
        });
    }

    private void FuncKey4() {
        btnKey4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "4";

                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());

            }
        });
    }

    private void FuncKey5() {
        btnKey5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "5";
                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());


            }
        });
    }

    private void FuncKey6() {
        btnKey6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "6";

                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());

            }
        });
    }

    private void FuncKey7() {
        btnKey7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "7";

                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());

            }
        });
    }

    private void FuncKey8() {
        btnKey8.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "8";

                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());

            }
        });
    }

    private void FuncKey9() {
        btnKey9.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "9";

                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());

            }
        });
    }


    private void FuncKey0() {
        btnKey0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                userentry = userentry + "0";

                if (userentry.length() > invmaxlength) {
                    Toast.makeText(getApplicationContext(), "Maximum of " + invmaxlength + " digits only", Toast.LENGTH_SHORT).show();
                    userentry = userentry.substring(0, userentry.length() - 1);
                    return;
                }

                formatinput();
                textView_detail2value.setText(formatinputstr);
                Log.i("Input Length", ""+userentry.length());

            }
        });
    }

    private void FuncKeyCancel(){
        btnKeyCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                /*cancel timer first*/
                getTimerCancel();

                if(userentry.length() > 0) {
                    userentry = "";
                    formatinputstr = "";
                    textView_detail2value.setText(formatinputstr);

                    return;
                }

                final_string = "CANCEL";
                //startActivity(new Intent(S1InputString.this, MainActivity.class));
                GetExpiryDateUI.this.finish();

                Log.i("PATRICK", "GetExpiryDateUI KeyBoard buCancel");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

    }

    private void FuncKeyClear() {
        btnKeyClear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(userentry.length() > 0) {
                    userentry = userentry.substring(0, userentry.length()-1);

                    formatinput();
                    textView_detail2value.setText(formatinputstr);
                    Log.i("Input Length", ""+userentry.length());
                }
            }
        });
    }

    private void FuncKeyEnter() {
        btnKeyEnter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*cancel timer first*/
                getTimerCancel();

                //dAmount = Double.parseDouble(etAmount.getText().toString());
                //final_amt_string = etAmount.getText().toString();
                //final_amt_string="12345678";

                if (userentry.length() < invminlength) {
                    Toast.makeText(getApplicationContext(), "Minimum length must be " + String.valueOf(invminlength), Toast.LENGTH_LONG).show();
                    userentry="";
                    return;
                }

                final_string = userentry;
                GetExpiryDateUI.this.finish();

                Log.i("Tine", "GetExpiryDateUI KeyBoard buOK");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();


                }
            }

        });
    }


    private void invalidMonth() {
		
		Toast.makeText(getApplicationContext(), "Invalid input for Month ", Toast.LENGTH_SHORT).show();
		return;
/*
        AlertDialog.Builder dialog=new AlertDialog.Builder(this);



        dialog.setMessage("Invalid input for Month");
        //dialog.setTitle("Dialog Box");
        dialog.setPositiveButton("OK",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog,
                                        int which) {
                        //Toast.makeText(getApplicationContext(),"Yes is clicked",Toast.LENGTH_LONG).show();

                    }
                });
        /*
        dialog.setNegativeButton("cancel",new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Toast.makeText(getApplicationContext(),"cancel is clicked",Toast.LENGTH_LONG).show();
            }
        });
        
        AlertDialog alertDialog=dialog.create();
        alertDialog.show();*/
    }

    private void formatinput()
    {
        if(userentry.length() == 2) {
            formatinputstr = userentry + " / ";
            textView_detail2value.setText(formatinputstr);
            //check month input if valid
            month = Integer.valueOf(userentry);
            if(month > 12 || month == 0)
            {
                invalidMonth();

                userentry = "";
                formatinputstr = "";
            }
        } else if(userentry.length() > 2) {
            Log.i("userentry:", ""+userentry);
            formatinputstr = userentry.substring(0, 2) + " / " + userentry.substring(2);
            Log.i("formatinputstr: ", ""+formatinputstr);
        } else {
            formatinputstr = userentry;
        }
    }

    /**
     * å–æ¶ˆå€’è®¡æ—¶
     */
    public void getTimerCancel() {
        timer.cancel();
    }

    /**
     * å¼€å§‹å€’è®¡æ—¶
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
            GetExpiryDateUI.this.finish();

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
        GetExpiryDateUI.this.finish();

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
        // SysApplication.getInstance().removeActivity(this);
    }

}
