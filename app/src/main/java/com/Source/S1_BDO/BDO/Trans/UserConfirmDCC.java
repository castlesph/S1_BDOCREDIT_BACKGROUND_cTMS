package com.Source.S1_BDO.BDO.Trans;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Build;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

import java.io.File;

public class UserConfirmDCC extends DemoAppActivity {

    private Context mContext;

    private Button mButton;
    final Context c = this;

    TextView textView_txn;
    TextView tvheader1, tvheader2;
    Button btn_back;

    ImageView imglocal;
    ImageView imgforeign;

    LinearLayout lllocal;
    LinearLayout llforeign;

    TextView tvphpcurrency, tvlocalamount;
    TextView tvforeigncurrency, tvforeignamount, tvconversionrate, tvmarkup;

    Double dAmount;
    int inResult;
    String stResult;

    boolean androidThinking;
    public static String final_string;
    public static String input_type;

    private int inTimeOut = 30;
    private  String pFileName = "";
    private  String pPathFile = "";
    File image_file;
    Bitmap myimage;

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

        setContentView(R.layout.confirm_dcc2);

        Intent intent=getIntent();
        String dispmsg=intent.getStringExtra("pass_in_string");
        Log.i("dispmsg", dispmsg);

        btn_back = (Button) findViewById(R.id.btn_back);

        tvheader1 = (TextView) findViewById(R.id.tvheader1);
        tvheader2 = (TextView) findViewById(R.id.tvheader2);

        // -------------------------------------------------------
        // new object from layout
        // -------------------------------------------------------
        lllocal = (LinearLayout) findViewById(R.id.lllocal);
        llforeign = (LinearLayout) findViewById(R.id.llforeign);

        // local
        tvphpcurrency = (TextView) findViewById(R.id.tvphpcurrency);
        tvlocalamount = (TextView) findViewById(R.id.tvlocalamount);
        imglocal = (ImageView)findViewById(R.id.imglocal);

        // foreign
        tvforeigncurrency = (TextView) findViewById(R.id.tvforeigncurrency);
        tvforeignamount = (TextView) findViewById(R.id.tvforeignamount);
        imgforeign = (ImageView)findViewById(R.id.imgforeign);
        tvconversionrate = (TextView) findViewById(R.id.tvconversionrate);
        tvmarkup = (TextView) findViewById(R.id.tvmarkup);

        tvheader1.setText("Card Sale");
        tvheader2.setText("Select currency");

        String[] dispmsginfo = dispmsg.split("\\|");
        int msgcnt = dispmsginfo.length;

        System.out.println("msgcnt [" + msgcnt + "]");

        for (int inIdx = 0 ; inIdx < msgcnt; inIdx++)
        {
            System.out.println("UserConfirmDCC->split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx)
            {
                case 0:
                    tvheader1.setText(dispmsginfo[inIdx]);
                    break;
                case 1:
                    tvheader2.setText(dispmsginfo[inIdx]);
                    break;
                case 2:
                    tvconversionrate.setText(dispmsginfo[inIdx]);
                    break;
                case 3:
                    tvphpcurrency.setText(dispmsginfo[inIdx]);

                    // display local flag image
                    pFileName = tvphpcurrency.getText().toString().toLowerCase();
                    pPathFile =  "/data/data/pub/"+pFileName+".png";
                    System.out.println("pPathFile [" + pPathFile + "]");
                    image_file = new File(pPathFile);
                    if (!image_file.exists())
                    {
                        pPathFile =  "/data/data/pub/blank.png";
                        image_file = new File(pPathFile);
                    }

                    myimage = BitmapFactory.decodeFile(image_file.getAbsolutePath());
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                        imglocal.setBackground(new BitmapDrawable(getResources(), myimage));
                    }

                    break;
                case 4:
                    tvforeigncurrency.setText(dispmsginfo[inIdx]);

                    // display foreign flag image
                    pFileName = tvforeigncurrency.getText().toString().toLowerCase();
                    pPathFile =  "/data/data/pub/"+pFileName+".png";
                    System.out.println("pPathFile [" + pPathFile + "]");
                    image_file = new File(pPathFile);
                    if (!image_file.exists())
                    {
                        pPathFile =  "/data/data/pub/blank.png";
                        image_file = new File(pPathFile);
                    }

                    myimage = BitmapFactory.decodeFile(image_file.getAbsolutePath());
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                        imgforeign.setBackground(new BitmapDrawable(getResources(), myimage));
                    }

                    break;
                case 5:

                    break;
                case 6:
                    tvlocalamount.setText(dispmsginfo[inIdx]);
                    break;
                case 7:
                    tvforeignamount.setText(dispmsginfo[inIdx]);
                    break;
                case 8:
                    tvmarkup.setText(dispmsginfo[inIdx]);
                    break;
            }
        }

        /*Start timer*/
        getTimerRestart();

        btn_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                /*cancel timer first*/
                getTimerCancel();

                final_string = "CANCEL";

                //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));
                UserConfirmDCC.this.finish();

                Log.i("UserConfirmDCC", "Button buCancel");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

        // currency local
        lllocal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*cancel timer first*/
                getTimerCancel();

                final_string = tvphpcurrency.getText().toString();

                UserConfirmDCC.this.finish();

                Log.i("UserConfirmDCC", "Button buCancel");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

        // currency foreign
        llforeign.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /*cancel timer first*/
                getTimerCancel();

                final_string = tvforeigncurrency.getText().toString();

                UserConfirmDCC.this.finish();

                Log.i("UserConfirmDCC", "Button buCancel");

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });

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
            final_string = "TIME OUT";

            //startActivity(new Intent(ConfirmMenu.this,MainActivity.class));

            Log.i("UserConfirmDCC", "Timeout");
            UserConfirmDCC.this.finish();

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
        // SysApplication.getInstance().removeActivity(this);
    }

}

