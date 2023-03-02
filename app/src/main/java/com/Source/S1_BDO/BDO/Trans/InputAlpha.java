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

public class InputAlpha extends DemoAppActivity {


    Button btnKey1, btnKey2, btnKey3,btnKey4,btnKey5,btnKey6,btnKey7,btnKey8,btnKey9,
            btnKey0,btnKeyEnter,btnKeyCancel,btnKeyClear;
    TextView textView_msg, textView_name, textView_alphaString;
    String userentry="", base="", inputchar="";

    boolean fNewChar=false,fNextSlot=false;
    String szLastChar="", szNewChar=""; //1,2,3,..0

    public static String final_string;
    int inputlength=0, minlength=0, typedef=0, count=0;



    private int inTimeOut = 30;
    String display, szmaxlength, szminlength, type;

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

        setContentView(R.layout.activity_key_alpha);

        Intent intent=getIntent();
        String value=intent.getStringExtra("pass_in_string");

        textView_msg=(TextView) findViewById(R.id.textView_msg);
        textView_name = (TextView) findViewById(R.id.textView_name);
        textView_alphaString = (TextView) findViewById(R.id.textView_alphaString);


        String[] dispmsginfo = value.split("\\|");
        int msgcnt = dispmsginfo.length;

        System.out.println("msgcnt [" + msgcnt + "]");

        for (int inIdx = 0 ; inIdx < msgcnt; inIdx++)
        {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx)
            {
                case 0:
                    minlength=Integer.valueOf(dispmsginfo[inIdx]);
                    break;
                case 1:
                    inputlength=Integer.valueOf(dispmsginfo[inIdx]);
                    break;
                case 2:
                    textView_name.setText(dispmsginfo[inIdx]);
                    break;
                case 3:
                    textView_msg.setText(dispmsginfo[inIdx]);
                    break;
            }
        }
        //typedef = Integer.valueOf(type.toString());

        //Toast.makeText(getApplicationContext(), "display: "+display, Toast.LENGTH_SHORT).show();
        //Toast.makeText(getApplicationContext(), "minlength: "+minlength, Toast.LENGTH_SHORT).show();
        //Toast.makeText(getApplicationContext(), "maxlength: "+maxlength, Toast.LENGTH_SHORT).show();
        //Toast.makeText(getApplicationContext(), "type: "+type, Toast.LENGTH_SHORT).show();

        btnKey1=(Button) findViewById(R.id.btn_qz);
        btnKey2=(Button) findViewById(R.id.btn_abc);
        btnKey3=(Button) findViewById(R.id.btn_def);
        btnKey4=(Button) findViewById(R.id.btn_ghi);
        btnKey5=(Button) findViewById(R.id.btn_jkl);
        btnKey6=(Button) findViewById(R.id.btn_mno);
        btnKey7=(Button) findViewById(R.id.btn_prs);
        btnKey8=(Button) findViewById(R.id.btn_tuv);
        btnKey9=(Button) findViewById(R.id.btn_wxy);
        btnKey0=(Button) findViewById(R.id.btn_00);
        btnKeyCancel=(Button) findViewById(R.id.btn_can);
        btnKeyClear=(Button) findViewById(R.id.btn_clr);
        btnKeyEnter=(Button) findViewById(R.id.btn_ok);


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

        activity_timer.start();

    }

    public void FuncKey1() {
        btnKey1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "1QZ";
                inputchar="";
                szNewChar="1";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }

                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                //Toast.makeText(getApplicationContext(), "inputchar: "+inputchar, Toast.LENGTH_SHORT).show();

                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }

    public void FuncKey2() {
        btnKey2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "2ABC";
                inputchar="";
                szNewChar="2";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }
                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }

    public void FuncKey3() {
        btnKey3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "3DEF";
                inputchar="";
                szNewChar="3";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }
                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }

    public void FuncKey4() {
        btnKey4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "4GHI";
                inputchar="";
                szNewChar="4";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }
                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }

    public void FuncKey5() {
        btnKey5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "5JKL";
                inputchar="";
                szNewChar="5";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }
                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }

    public void FuncKey6() {
        btnKey6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "6MNO";
                inputchar="";
                szNewChar="6";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }
                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }

    public void FuncKey7() {
        btnKey7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "7PRS";
                inputchar="";
                szNewChar="7";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }
                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }

    public void FuncKey8() {
        btnKey8.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "8TUV";
                inputchar="";
                szNewChar="8";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }
                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }

    public void FuncKey9() {
        btnKey9.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "9WXY";
                inputchar="";
                szNewChar="9";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }
                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }


    public void FuncKey0() {
        btnKey0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                activity_timer.start();
                timer.start();
                fNextSlot=false;
                base = "0";
                inputchar="";
                szNewChar="0";
                if(szLastChar.equals(szNewChar)) {
                    count++;
                    if(count>=base.length())
                        count=0;
                    userentry = userentry.substring(0, userentry.length()-1);
                }
                else {
                    count = 0;
                    //go to next array
                    fNextSlot=true;
                }
                inputchar = base.substring(count,count+1);
                userentry = userentry + inputchar;
                textView_alphaString.setText(userentry);
                szLastChar=szNewChar;
            }
        });
    }

    public void FuncKeyCancel(){
        btnKeyCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                /*cancel timer first*/
                getTimerCancel();
                activity_timer.cancel();

                if(userentry.length() > 0) {
                    userentry = "";
                    textView_alphaString.setText(userentry);
                    count=0;
                    szLastChar="";
                    szNewChar="";
                    return;
                }

                final_string = "CANCEL";
                //startActivity(new Intent(InputAlpha.this, MainActivity.class));
                InputAlpha.this.finish();

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

                activity_timer.start();
                if(userentry.length() > 0) {
                    userentry = userentry.substring(0, userentry.length()-1);
                    textView_alphaString.setText(userentry);
                }
                count=0;
                szLastChar="";
                szNewChar="";
            }
        });
    }

    public void FuncKeyEnter() {
        btnKeyEnter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*cancel timer first*/
                activity_timer.cancel();
                getTimerCancel();

				Log.i("saturn", "min input"+minlength);
				Log.i("saturn", "max input"+inputlength);
				Log.i("saturn", "input len"+userentry.length());

                //if (userentry.length() < inputlength) {
                if ((userentry.length() < minlength) || (userentry.length() > inputlength+1)) {
                    //Toast.makeText(getApplicationContext(), "Minimum length must be " + String.valueOf(minlength), Toast.LENGTH_SHORT).show();
                    //return;
                    invalidinput();
                } 
				else
				{
					final_string = userentry;
                	InputAlpha.this.finish();

                	Log.i("Tine", "InputAlpha buOK");

                	// do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                	synchronized (MainActivity.LOCK) {
                    	MainActivity.LOCK.setCondition(true);
                    	MainActivity.LOCK.notifyAll();
                	}
				}

                
            }

        });
    }

    private void invalidinput() {
        AlertDialog.Builder dialog=new AlertDialog.Builder(this);
        dialog.setMessage("Invalid input");
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
        */
        AlertDialog alertDialog=dialog.create();
        alertDialog.show();
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

    private CountDownTimer timer = new CountDownTimer(1000, 1000) {

        @Override
        public void onTick(long millisUntilFinished) {
            Log.i("InputAlpha Timer", "Timer onTick");
        }

        @Override
        public void onFinish() {
            Log.i("Timer", "InputAlpha Timer onFinish");
            //final_string = "TIME OUT";
            //InputAlpha.this.finish();
            count=0;
            szLastChar="";
            szNewChar="";
        }
    };

    private CountDownTimer activity_timer = new CountDownTimer(inTimeOut*1000, 1000) {

        @Override
        public void onTick(long millisUntilFinished) {
            Log.i("InputAlpha activity_timer", "Timer onTick");
        }

        @Override
        public void onFinish() {
            Log.i("Timer", "InputAlpha activity_timer onFinish");
            final_string = "TIME OUT";
            InputAlpha.this.finish();

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
        // SysApplication.getInstance().removeActivity(this);
    }

}
