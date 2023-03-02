package com.Source.S1_BDO.BDO.Kms;

import android.app.Activity;
import android.content.Intent;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.R;


public class KMSUI {
    private Activity mActivity;

    TextView PinDigitTV;
    EditText PinDigitET;
    EditText FunctionKeyET;
    TextView textView_txn;
    TextView textView_disp;

    private static final String TAG_A = "KMS";
    private static final String TAG_C = "KMSUI";

    private PINPADFIXED MovingEncrythread;
    private PINPADDUKPT ScramblingEncrythread;
	private PINPADMKSK  MkskEncrythread;
	
    private int inKeySet = 0;
    private int inKeyIndex= 0;
    private String StrInData= "";
    private int inKeyType = 0;
    public static String final_string;
    public static String PinBlock;
    public static String KSN;
    private int inPinBypassAllow = 0;


    private Handler background_ON_Handler;
    private HandlerThread background_ON_Thread;

    private static final String TAG = "KMSUI";


    private Runnable background_on = new Runnable () {
        public void run() {
            KMSUI.this.mActivity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    PinDigitTV.setEnabled(false);
                    PinDigitTV.setAlpha(0.0f);
                    PinDigitET.setEnabled(false);
                    PinDigitET.setAlpha(0.0f);
                    FunctionKeyET.setEnabled(false);
                }
            });
        }
    };


    public KMSUI() {

    }

    public void init(Activity activity, int KeySet, int KeyIndex, String pInData, int KeyType, int PinBypassAllow) {
        this.mActivity = activity;
        this.inKeySet = KeySet;
        this.inKeyIndex = KeyIndex;
        this.StrInData = pInData;
        this.inKeyType = KeyType;
        this.inPinBypassAllow = PinBypassAllow;
        this.mActivity.runOnUiThread(new Runnable() {
            public void run() {
                KMSUI.this.mActivity.setContentView(R.layout.activity_kms);

                PinDigitTV = (TextView) KMSUI.this.mActivity.findViewById(R.id.PinDigitTextView);
                PinDigitET = (EditText) KMSUI.this.mActivity.findViewById(R.id.PinDigitEditText);
                FunctionKeyET = (EditText) KMSUI.this.mActivity.findViewById(R.id.FunctionkeyText);

                textView_txn = (TextView) KMSUI.this.mActivity.findViewById(R.id.textView_txn);

                Intent intent=KMSUI.this.mActivity.getIntent();

                Log.i("saturn KeyType=", KeyType+"");
                Log.i("saturn KeySet=", KeySet+"");
                Log.i("saturn KeyIndex=", KeyIndex+"");
                Log.i("saturn pInData=", pInData);
                Log.i("saturn PinBypassAllow=", PinBypassAllow+"");

                Log.i("saturn inKeyType=", inKeyType+"");
                Log.i("saturn inKeySet=", inKeySet+"");
                Log.i("saturn inKeyIndex=", inKeyIndex+"");
                Log.i("saturn StrInData=", StrInData);


                PinDigitTV.setEnabled(false);
                PinDigitTV.setAlpha(0.0f);
                PinDigitET.setEnabled(false);
                PinDigitET.setAlpha(0.0f);
                FunctionKeyET.setEnabled(false);
                FunctionKeyET.setAlpha(0.0f);

                //HandlerThread background_ON_Thread = new HandlerThread("background_on");
                //background_ON_Thread.start();
                //background_ON_Handler = new Handler (background_ON_Thread.getLooper());

                String sTitle, sData;
                String[] dispmsginfo = StrInData.split("\\^");
                sTitle = dispmsginfo[0];
                sData = dispmsginfo[1];
                Log.i(TAG, "sTitle="+sTitle);
                Log.i(TAG, "sData="+sData);

                textView_txn.setText(sTitle);
                StrInData = sData;

                if(inKeyType == 1)
                {
                    
					Log.d(TAG_A, TAG_C + "saturn On Click startButton Listener. inKeyType = 1");
					MovingEncrythread = new PINPADFIXED(KMSUI.this.mActivity, inKeySet, inKeyIndex, StrInData, inPinBypassAllow);
					MovingEncrythread.start();
                }

                if(inKeyType == 2)
                {
                    
					Log.d(TAG_A, TAG_C + "saturn On Click startButton2 Listener. inKeyType = 2");
					ScramblingEncrythread = new PINPADDUKPT(KMSUI.this.mActivity, inKeySet, inKeyIndex, StrInData, inPinBypassAllow);
					ScramblingEncrythread.start();
                }

				if (inKeyType == 3)
				{				
					Log.d(TAG_A, TAG_C + "saturn On Click startButton3 Listener. inKeyType = 3");
			    		MkskEncrythread = new PINPADMKSK(KMSUI.this.mActivity, inKeySet, inKeyIndex, StrInData, inPinBypassAllow);
					MkskEncrythread.start();

				}
            }
        });






    }


    public void exit() {
        this.mActivity.runOnUiThread(new Runnable() {
            public void run() {
				
            }
        });
    }
}
