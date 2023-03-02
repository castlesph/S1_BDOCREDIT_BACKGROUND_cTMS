package com.Source.S1_BDO.BDO.Trans;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

public class GetCardOptionsActivity extends DemoAppActivity {

    Button mCancelButton;
    Button cardentry;
    Button man_entry;
    TextView textViewmsg;
    TextView textView_dtxn;
    TextView textView_cur;

    public static String final_string;
    private int inTimeOut = 30;
    int inRet=0;


    @Override
    protected void onCreate(Bundle savedInstanceState) {

        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);

        super.onCreate(savedInstanceState);

        new Thread() {
            @Override
            public void run() {
                try {
                    //inRet = inCTOSS_WaveGetCardFields();

                    Bundle bundle = getIntent().getExtras();

                    setContentView(R.layout.activity_cardentry_options);
                    Intent intent = getIntent();
                    String dispmsg = intent.getStringExtra("pass_in_string");

                    //mClearButton = (Button) findViewById(R.id.clear_button);
                    mCancelButton = (Button) findViewById(R.id.btn_can);
                    //cardentry = (Button) findViewById(R.id.cardentry);
                    man_entry = (Button) findViewById(R.id.man_entry);

                    String[] dispmsginfo = dispmsg.split("\\|");
                    int msgcnt = dispmsginfo.length;

                    textViewmsg = (TextView) findViewById(R.id.textView_msg);
                    textView_dtxn = (TextView) findViewById(R.id.textView_dtxn);
                    textView_cur = (TextView) findViewById(R.id.textView_cur);


                    //to do
                    for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
                        System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                        switch (inIdx) {
                            case 0:
                                textView_dtxn.setText(dispmsginfo[inIdx]);
                                break;
                            case 1:
                                textViewmsg.setText(dispmsginfo[inIdx]);
                                //cardentry.setText(dispmsginfo[inIdx]);
                                break;
                            case 2:
                                textView_cur.setText(dispmsginfo[inIdx]);
                                break;
                        }
                    }

                    mCancelButton.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {

                            //startActivity(new Intent(CardEntry.this, MainActivity.class));
                            final_string = "CANCEL";
                            GetCardOptionsActivity.this.finish();

                            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                            synchronized (MainActivity.LOCK) {
                                MainActivity.LOCK.setCondition(true);
                                MainActivity.LOCK.notifyAll();

                            }
                        }
                    });

                    man_entry.setOnClickListener(new View.OnClickListener(){
                        @Override
                        public void onClick(View view) {

                            //startActivity(new Intent(CardEntry.this, MainActivity.class));
                            final_string = "TO";
                            GetCardOptionsActivity.this.finish();

                            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                            synchronized (MainActivity.LOCK) {
                                MainActivity.LOCK.setCondition(true);
                                MainActivity.LOCK.notifyAll();

                            }
                        }
                    });


                } catch (Exception e) {

                }
            }
        }.start();
    }

    @Override
    public void onBackPressed() {
        //getTimerCancel();

        final_string = "TO";
        //startActivity(new Intent(AmountEntryActivity.this, MainActivity.class));
        GetCardOptionsActivity.this.finish();

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
        final_string = null;
        // SysApplication.getInstance().removeActivity(this);
    }

    public native int inCTOSS_WaveGetCardFields();

}