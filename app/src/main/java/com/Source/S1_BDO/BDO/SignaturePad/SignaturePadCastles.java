package com.Source.S1_BDO.BDO.SignaturePad;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.media.ThumbnailUtils;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.CloseActivityClass;
import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;
import com.github.gcacace.signaturepad.monochrome.BitmapConvertor;
import com.github.gcacace.signaturepad.views.SignaturePad;

import static android.content.ContentValues.TAG;

import java.io.FileOutputStream;

public class SignaturePadCastles extends DemoAppActivity {

    private SignaturePad mSignaturePad;
    private Button mClearButton;
    private Button mSaveButton;

    protected PowerManager.WakeLock mWakeLock;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        //CloseActivityClass.activityList.add(this);
        //SysApplication.getInstance().addActivity(this);

        super.onCreate(savedInstanceState);
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        setContentView(R.layout.activity_signaturepad);

        /* This code together with the one in onDestroy()
         * will make the screen be always on until this Activity gets destroyed. */
        final PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "S1_BDO:AAA-SignaturePadCastles>>WAKE_LOCK");
        this.mWakeLock.acquire();

		Intent intent = getIntent();
		String dispmsg = intent.getStringExtra("pass_in_string");

        Log.i(TAG, dispmsg);

        // Display Amount in Signature
        TextView text_msg;
        TextView detail_msg_01;

        String[] dispmsginfo = dispmsg.split("\\|");
        int msgcnt = dispmsginfo.length;

        // sidumili: For display amount in signature
        for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
            System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
            switch (inIdx) {
                case 0: // TransType
                    // Do nothing...
                    break;
                case 1: // Message1
                    text_msg = (TextView) findViewById(R.id.msg_text_01);
                    text_msg.setText(dispmsginfo[inIdx]);
                    break;
                case 2: // Message2
                    detail_msg_01 = (TextView) findViewById(R.id.msg_detail_01);
                    detail_msg_01.setText(dispmsginfo[inIdx]);
                    break;
            }
        }

        mSignaturePad = (SignaturePad) findViewById(R.id.signature_pad);
        mSignaturePad.setOnSignedListener(new SignaturePad.OnSignedListener() {
            @Override
            public void onStartSigning() {
                //Toast.makeText(getApplicationContext(), "OnStartSigning", Toast.LENGTH_SHORT).show();
                Log.i("TINE", "OnStartSigning");
            }

            @Override
            public void onSigned() {
                mSaveButton.setEnabled(true);
                mClearButton.setEnabled(true);
            }

            @Override
            public void onClear() {
                mSaveButton.setEnabled(false);
                mClearButton.setEnabled(false);
            }
        });

        mClearButton = (Button) findViewById(R.id.clear_button);
        mSaveButton = (Button) findViewById(R.id.save_button);

        mClearButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mSignaturePad.clear();
            }
        });

        mSaveButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //Bitmap signatureBitmap = mSignaturePad.getSignatureBitmap();


                Bitmap originalBitmap =mSignaturePad.getTransparentSignatureBitmap(true);
                //TrimBitmap(originalBitmap);
                Bitmap signatureBitmap = Bitmap.createBitmap(originalBitmap.getWidth(), originalBitmap.getHeight(), Bitmap.Config.ARGB_8888);
                Canvas canvas = new Canvas(signatureBitmap);
                canvas.drawColor(-1);
                canvas.drawBitmap(originalBitmap, 0.0F, 0.0F, (Paint)null);
                signatureBitmap = Bitmap.createScaledBitmap(signatureBitmap, 300, 120, false);
                //Bitmap resizeBmp = ThumbnailUtils.extractThumbnail(signatureBitmap, 380, 460);
                //Bitmap resizeBmp = ThumbnailUtils.extractThumbnail(signatureBitmap, 380, 200);
                //Bitmap resizeBmp = ThumbnailUtils.extractThumbnail(signatureBitmap, 320, 140);
                Bitmap resizeBmp = ThumbnailUtils.extractThumbnail(signatureBitmap, 300, 120);

                BitmapConvertor convertor = new BitmapConvertor(SignaturePadCastles.this);
                //convertor.convertBitmap(signatureBitmap, "/data/data/pub/signtest.bmp");

                convertor.convertBitmap(resizeBmp, "/data/data/com.Source.S1_BDO.BDO/signtest.bmp");

                //startActivity(new Intent(SignaturePadCastles.this, MainActivity.class));
                finish();

                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();

                }
            }
        });
    }

    @Override
    protected void onDestroy() {
        this.mWakeLock.release();
        super.onDestroy();
        // SysApplication.getInstance().removeActivity(this);
    }
    public static Bitmap TrimBitmap(Bitmap bmp) {
        int imgHeight = bmp.getHeight();
        int imgWidth = bmp.getWidth();
        int backgroundColor = -1;
        int xMin = 2147483647;
        int xMax = -2147483648;
        int yMin = 2147483647;
        int yMax = -2147483648;
        boolean foundPixel = false;

        int y;
        boolean stop;
        int x;
        for(y = 0; y < imgWidth; ++y) {
            stop = false;

            for(x = 0; x < imgHeight; ++x) {
                if (bmp.getPixel(y, x) != backgroundColor) {
                    xMin = y;
                    stop = true;
                    foundPixel = true;
                    break;
                }
            }

            if (stop) {
                break;
            }
        }

        if (!foundPixel) {
            return null;
        } else {
            for(y = 0; y < imgHeight; ++y) {
                stop = false;

                for(x = xMin; x < imgWidth; ++x) {
                    if (bmp.getPixel(x, y) != backgroundColor) {
                        yMin = y;
                        stop = true;
                        break;
                    }
                }

                if (stop) {
                    break;
                }
            }

            for(y = imgWidth - 1; y >= xMin; --y) {
                stop = false;

                for(x = yMin; x < imgHeight; ++x) {
                    if (bmp.getPixel(y, x) != backgroundColor) {
                        xMax = y;
                        stop = true;
                        break;
                    }
                }

                if (stop) {
                    break;
                }
            }

            for(y = imgHeight - 1; y >= yMin; --y) {
                stop = false;

                for(x = xMin; x <= xMax; ++x) {
                    if (bmp.getPixel(x, y) != backgroundColor) {
                        yMax = y;
                        stop = true;
                        break;
                    }
                }

                if (stop) {
                    break;
                }
            }

            return Bitmap.createBitmap(bmp, xMin, yMin, xMax - xMin, yMax - yMin);
        }
    }
}
