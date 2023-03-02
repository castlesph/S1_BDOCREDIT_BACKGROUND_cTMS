package com.Source.S1_BDO.BDO;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.db.AndroidDatabaseManager;


public class EditDatabaseActivity extends AppCompatActivity implements View.OnClickListener{
    Button btnterminal;
    Button btnwave;
    Button btnemv;
    Button btnexit;
    Button btnenv;
    Button btnqrpay;
    Button btneftsec;
    Button btnermtct;
    Button btnmenu;
    Button btnquickmenu;

    private static final String TAG = EditDatabaseActivity.class.getSimpleName();
    private String TerminalDb = "/data/data/pub/TERMINAL.S3DB";
    private String WaveDb = "/data/data/pub/WAVE.S3DB";
    private String EmvDb = "/data/data/pub/EMV.S3DB";
    private String EnvDb = "/data/data/pub/ENV.S3DB";
    private String QRPayDb = "/data/data/pub/BDOPAY.S3DB";
    private String EFTSECDb = "/data/data/pub/EFT.S3DB";
    private String ERMTCTDb = "/data/data/pub/ERMTCT.S3DB";
    private String MenuDb = "/data/data/com.Source.S1_BDO.BDO/DYNAMICMENU25.S3DB";
    private String QuickMenuDb = "/data/data/com.Source.S1_BDO.BDO/QUICKMENU.S3DB";

    public static String final_string;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_edit_database);

        MainActivity.fSTART = true;

        btnterminal = (Button) findViewById(R.id.btn_edit_terminal);
        btnwave = (Button) findViewById(R.id.btn_edit_wave);
        btnemv = (Button) findViewById(R.id.btn_edit_emv);
        btnexit = (Button) findViewById(R.id.btn_exit);
        btnenv = (Button) findViewById(R.id.btn_edit_env);
        btnqrpay = (Button) findViewById(R.id.btn_edit_qrpay);
        btneftsec = (Button) findViewById(R.id.btn_edit_eftsec);
        btnermtct = (Button) findViewById(R.id.btn_edit_ermtct);
        btnmenu = (Button) findViewById(R.id.btn_edit_menu);
        btnquickmenu = (Button) findViewById(R.id.btn_edit_quickmenu);

        btnterminal.setOnClickListener(this);
        btnwave.setOnClickListener(this);
        btnemv.setOnClickListener(this);
        btnexit.setOnClickListener(this);
        btnenv.setOnClickListener(this);
        btnqrpay.setOnClickListener(this);
        btneftsec.setOnClickListener(this);
        btnermtct.setOnClickListener(this);
        btnmenu.setOnClickListener(this);
        btnquickmenu.setOnClickListener(this);

    }

    @Override
    public void onClick(View view) {
        Intent intent = new Intent();
        Bundle bundle = new Bundle();

        switch (view.getId())
        {
            case R.id.btn_edit_terminal:
                Log.d(TAG, "btn_edit_terminal");
                intent.setClass(EditDatabaseActivity.this, AndroidDatabaseManager.class);
                bundle.putString("dbname", TerminalDb);
                intent.putExtras(bundle);
                startActivity(intent);

                final_string = "EDITDB_OK";
                EditDatabaseActivity.this.finish();
                break;
            case R.id.btn_edit_wave:
                Log.d(TAG, "btn_edit_wave");
                intent.setClass(EditDatabaseActivity.this, AndroidDatabaseManager.class);
                bundle.putString("dbname", WaveDb);
                intent.putExtras(bundle);
                startActivity(intent);

                final_string = "EDITDB_OK";
                EditDatabaseActivity.this.finish();
                break;
            case R.id.btn_edit_emv:
                Log.d(TAG, "btn_edit_emv");
                intent.setClass(EditDatabaseActivity.this, AndroidDatabaseManager.class);
                bundle.putString("dbname", EmvDb);
                intent.putExtras(bundle);
                startActivity(intent);

                final_string = "EDITDB_OK";
                EditDatabaseActivity.this.finish();
                break;
            case R.id.btn_edit_env:
                Log.d(TAG, "btn_edit_env");
                intent.setClass(EditDatabaseActivity.this, AndroidDatabaseManager.class);
                bundle.putString("dbname", EnvDb);
                intent.putExtras(bundle);
                startActivity(intent);

                final_string = "EDITDB_OK";
                EditDatabaseActivity.this.finish();
                break;
            case R.id.btn_edit_qrpay:
                Log.d(TAG, "btn_edit_qrpay");
                intent.setClass(EditDatabaseActivity.this, AndroidDatabaseManager.class);
                bundle.putString("dbname", QRPayDb);
                intent.putExtras(bundle);
                startActivity(intent);

                final_string = "EDITDB_OK";
                EditDatabaseActivity.this.finish();
                break;
            case R.id.btn_edit_eftsec:
                Log.d(TAG, "btn_edit_eftsec");
                intent.setClass(EditDatabaseActivity.this, AndroidDatabaseManager.class);
                bundle.putString("dbname", EFTSECDb);
                intent.putExtras(bundle);
                startActivity(intent);

                final_string = "EDITDB_OK";
                EditDatabaseActivity.this.finish();
                break;
            case R.id.btn_edit_ermtct:
                Log.d(TAG, "btn_edit_ermtct");
                intent.setClass(EditDatabaseActivity.this, AndroidDatabaseManager.class);
                bundle.putString("dbname", ERMTCTDb);
                intent.putExtras(bundle);
                startActivity(intent);

                final_string = "EDITDB_OK";
                EditDatabaseActivity.this.finish();
                break;
            case R.id.btn_edit_menu:
                Log.d(TAG, "btn_edit_menu");
                intent.setClass(EditDatabaseActivity.this, AndroidDatabaseManager.class);
                bundle.putString("dbname", MenuDb);
                intent.putExtras(bundle);
                startActivity(intent);

                final_string = "EDITDB_OK";
                EditDatabaseActivity.this.finish();
                break;
            case R.id.btn_edit_quickmenu:
                Log.d(TAG, "btn_edit_quickmenu");
                intent.setClass(EditDatabaseActivity.this, AndroidDatabaseManager.class);
                bundle.putString("dbname", QuickMenuDb);
                intent.putExtras(bundle);
                startActivity(intent);

                final_string = "EDITDB_OK";
                EditDatabaseActivity.this.finish();
                break;
            case R.id.btn_exit:
                Log.d(TAG, "btn_exit");

                final_string = "EDITDB_OK";
                //intent.setClass(EditDatabaseActivity.this, MainActivity.class);
                //startActivity(intent);
                //startActivity(new Intent(EditDatabaseActivity.this,MainActivity.class));
                EditDatabaseActivity.this.finish();

                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }

                break;

        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

    }
}
