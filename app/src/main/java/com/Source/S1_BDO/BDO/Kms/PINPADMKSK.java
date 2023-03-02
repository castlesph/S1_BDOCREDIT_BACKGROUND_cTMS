package com.Source.S1_BDO.BDO.Kms;

import android.app.Activity;
import android.binder.aidl.IKMS2Callback;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;

import com.Source.S1_BDO.BDO.Main.MainActivity;
import com.Source.S1_BDO.BDO.R;

import java.io.File;

import CTOS.CtKMS2CustomPINPadbyImage;
import CTOS.CtKMS2MKSK;
import CTOS.CtKMS2System;


public class PINPADMKSK extends Thread
{
    private static final String TAG = "PINPADMKSK";


    public Activity activity;
    public int inKeySet;
    public int inKeyIndex;
    public String StrInData;
    public int inPinBypassAllow;
    private TextView PinDigitTV;
    private EditText PinDigitET;
    private EditText FunctionKeyET;

    private Handler background_ON_Handler;

    private Handler background_OFF_Handler;

    private Runnable background_on = new Runnable () {
        public void run() {
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                	Log.d(TAG, "saturn background_on:start");
                    PinDigitTV.setEnabled(false);
                    PinDigitTV.setAlpha(0.0f);
                    PinDigitET.setEnabled(false);
                    PinDigitET.setAlpha(0.0f);
                    FunctionKeyET.setAlpha(0.0f);
                    FunctionKeyET.setEnabled(false);
                    FunctionKeyET.setAlpha(0.0f);
                    Log.d(TAG, "saturn background_on:end");
					
                }
            });
        }
    };

    private Runnable background_off = new Runnable () {
        public void run() {
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "saturn background_off:start");
                    PinDigitTV.setEnabled(true);
                    PinDigitTV.setAlpha(1.0f);
                    PinDigitET.setEnabled(true);
                    PinDigitET.setText("");
                    PinDigitET.setAlpha(1.0f);
                    FunctionKeyET.setEnabled(true);
                    FunctionKeyET.setText("");
                    FunctionKeyET.setAlpha(1.0f);
					Log.d(TAG, "saturn background_off:end");
 
                }
            });
        }
    };

    public PINPADMKSK(Activity activity, int inKeySet, int inKeyIndex, String StrInData, int inPinBypassAllow)
    {
        this.activity = activity;
        this.inKeySet = inKeySet;
        this.inKeyIndex = inKeyIndex;
        this.StrInData = StrInData;
        this.inPinBypassAllow = inPinBypassAllow;
        PinDigitTV = (TextView) activity.findViewById(R.id.PinDigitTextView);
        PinDigitET = (EditText) activity.findViewById(R.id.PinDigitEditText);
        FunctionKeyET = (EditText) activity.findViewById(R.id.FunctionkeyText);
		
		PinDigitTV.setText("ENTER ONLINE PIN");

		
		Log.d(TAG, "saturn PINPADMKSK");
		
        HandlerThread background_ON_Thread = new HandlerThread("background_on");
        background_ON_Thread.start();
        background_ON_Handler = new Handler (background_ON_Thread.getLooper());
        HandlerThread background_OFF_Thread = new HandlerThread("background_off");
        background_OFF_Thread.start();
        background_OFF_Handler = new Handler (background_OFF_Thread.getLooper());
    }
    @Override
    public void run()
    {
        IKMS2Callback.Stub callback = new IKMS2Callback.Stub(){
            StringBuffer sb = new StringBuffer();
            byte recv;
            @Override
            public int testCancel(){
                return 0;
            }
            @Override
            public void onGetDigit(byte Digit) {
                Log.d(TAG, String.format("saturn NoDigits = %d, OnGetPINDigit.", Digit));
                recv = Digit;
                sb.setLength(0);
                for(int i = 0;i < recv;i ++)
                    sb.append("*");
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        PinDigitET.setText(sb);
                    }
                });
            }

            @Override
            public void onGetFunctionKey(byte FunctionKey) {
                Log.d(TAG, "saturn OnGetPINOtherKeys");
                if(FunctionKey=='A')
                {
                    Log.d(TAG, "saturn Enter Key");
                    //if(sb.length()!=0) {
                    if((sb.length()!=0) && (sb.length()>=4))
                    {
						
                        MainActivity.Enter_Press = 1;
                        Log.d(TAG, MainActivity.Enter_Press + "");
                    }
                }
                Log.d(TAG, String.format("saturn FunctionKey = %d, OnGetPINOtherKeys.", FunctionKey));
                recv = FunctionKey;
/*
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    FunctionKeyET.setText(String.format("FunctionKey = %d",recv));
                }
            });
*/
            }};

        int xAxis = 0;
        int yAxis = 0;
        int iWidth = 240;
        int iHeight = 200;
        int iGap = 2;
        int iXOffSet = 0;
        int iYOffSet = 0;

        String rtn ="";
        CtKMS2CustomPINPadbyImage setvirtualpinpad_para = new CtKMS2CustomPINPadbyImage();
        //xAxis = iXOffSet;
        xAxis = 1;
        yAxis = 420;
        Log.i(TAG, "run: 0.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 0.bmp, yAxis="+yAxis);
        setvirtualpinpad_para.VKBD_0.x = xAxis;
        setvirtualpinpad_para.VKBD_0.y = yAxis;
        setvirtualpinpad_para.VKBD_0.width = iWidth;
        setvirtualpinpad_para.VKBD_0.height = iHeight;
        setvirtualpinpad_para.VKBD_0.filepath = new File("/data/data/pub/0.bmp");

        xAxis+=iWidth+iGap+iYOffSet;
        Log.i(TAG, "run: 1.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 1.bmp, yAxis="+yAxis);
        setvirtualpinpad_para.VKBD_1.x = xAxis;
        setvirtualpinpad_para.VKBD_1.y = yAxis;
        setvirtualpinpad_para.VKBD_1.width = iWidth;
        setvirtualpinpad_para.VKBD_1.height = iHeight;
        setvirtualpinpad_para.VKBD_1.filepath = new File("/data/data/pub/1.bmp");

        xAxis+=iWidth+iGap+iYOffSet;
        Log.i(TAG, "run: 2.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 0.bmp, yAxis="+yAxis);
        setvirtualpinpad_para.VKBD_2.x = xAxis;
        setvirtualpinpad_para.VKBD_2.y = yAxis;
        setvirtualpinpad_para.VKBD_2.width = iWidth;
        setvirtualpinpad_para.VKBD_2.height = iHeight;
        setvirtualpinpad_para.VKBD_2.filepath = new File("/data/data/pub/2.bmp");

        //xAxis = iXOffSet;
        //yAxis = 605;
        xAxis=1;
        yAxis=620;
        Log.i(TAG, "run: 3.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 3.bmp, yAxis="+yAxis);
        setvirtualpinpad_para.VKBD_3.x = xAxis;
        setvirtualpinpad_para.VKBD_3.y = yAxis;
        setvirtualpinpad_para.VKBD_3.width = iWidth;
        setvirtualpinpad_para.VKBD_3.height = iHeight;
        setvirtualpinpad_para.VKBD_3.filepath = new File("/data/data/pub/3.bmp");

        xAxis+=iWidth+iGap+iYOffSet;
        Log.i(TAG, "run: 4.bmp, xAxis="+xAxis);
        setvirtualpinpad_para.VKBD_4.x = xAxis;
        setvirtualpinpad_para.VKBD_4.y = yAxis;
        setvirtualpinpad_para.VKBD_4.width = iWidth;
        setvirtualpinpad_para.VKBD_4.height = iHeight;
        setvirtualpinpad_para.VKBD_4.filepath = new File("/data/data/pub/4.bmp");

        xAxis+=iWidth+iGap+iYOffSet;
        Log.i(TAG, "run: 5.bmp, xAxis="+xAxis);
        setvirtualpinpad_para.VKBD_5.x = xAxis;
        setvirtualpinpad_para.VKBD_5.y = yAxis;
        setvirtualpinpad_para.VKBD_5.width = iWidth;
        setvirtualpinpad_para.VKBD_5.height = iHeight;
        setvirtualpinpad_para.VKBD_5.filepath = new File("/data/data/pub/5.bmp");

        //xAxis = iXOffSet;
        //yAxis = 765;
        xAxis=1;
        yAxis=820;
        Log.i(TAG, "run: 6.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 6.bmp, yAxis="+yAxis);
        setvirtualpinpad_para.VKBD_6.x = xAxis;
        setvirtualpinpad_para.VKBD_6.y = yAxis;
        setvirtualpinpad_para.VKBD_6.width = iWidth;
        setvirtualpinpad_para.VKBD_6.height = iHeight;
        setvirtualpinpad_para.VKBD_6.filepath = new File("/data/data/pub/6.bmp");

        xAxis+=iWidth+iGap+iYOffSet;
        Log.i(TAG, "run: 7.bmp, xAxis="+xAxis);
        setvirtualpinpad_para.VKBD_7.x = xAxis;
        setvirtualpinpad_para.VKBD_7.y = yAxis;
        setvirtualpinpad_para.VKBD_7.width = iWidth;
        setvirtualpinpad_para.VKBD_7.height = iHeight;
        setvirtualpinpad_para.VKBD_7.filepath = new File("/data/data/pub/7.bmp");

        xAxis+=iWidth+iGap+iYOffSet;
        Log.i(TAG, "run: 8.bmp, xAxis="+xAxis);
        setvirtualpinpad_para.VKBD_8.x = xAxis;
        setvirtualpinpad_para.VKBD_8.y = yAxis;
        setvirtualpinpad_para.VKBD_8.width = iWidth;
        setvirtualpinpad_para.VKBD_8.height = iHeight;
        setvirtualpinpad_para.VKBD_8.filepath = new File("/data/data/pub/8.bmp");

        //xAxis = 202;
        //yAxis = 925;
        xAxis=1;
        yAxis=1020;
        //xAxis+=iHeight+iGap+iYOffSet;
        Log.i(TAG, "run: backspace.bmp, xAxis="+xAxis);
        setvirtualpinpad_para.VKBD_CLEAR.x = xAxis;
        setvirtualpinpad_para.VKBD_CLEAR.y = yAxis;
        setvirtualpinpad_para.VKBD_CLEAR.width = iWidth;
        setvirtualpinpad_para.VKBD_CLEAR.height = iHeight;
        setvirtualpinpad_para.VKBD_CLEAR.filepath = new File("/data/data/pub/backspace.bmp");


        xAxis+=iWidth+iGap+iYOffSet;
        Log.i(TAG, "run: 9.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 9.bmp, yAxis="+yAxis);
        setvirtualpinpad_para.VKBD_9.x = xAxis;
        setvirtualpinpad_para.VKBD_9.y = yAxis;
        setvirtualpinpad_para.VKBD_9.width = iWidth;
        setvirtualpinpad_para.VKBD_9.height = iHeight;
        setvirtualpinpad_para.VKBD_9.filepath = new File("/data/data/pub/9.bmp");


        xAxis+=iWidth+iGap+iYOffSet;
        Log.i(TAG, "run: enter.bmp, xAxis="+xAxis);
        setvirtualpinpad_para.VKBD_ENTER.x = xAxis;
        setvirtualpinpad_para.VKBD_ENTER.y = yAxis;
        setvirtualpinpad_para.VKBD_ENTER.width = iWidth;
        setvirtualpinpad_para.VKBD_ENTER.height = iHeight;
        setvirtualpinpad_para.VKBD_ENTER.filepath = new File("/data/data/pub/enter.bmp");

        xAxis=20;
        yAxis = 40;
        //xAxis+=iWidth+iGap+iYOffSet;
        Log.i(TAG, "run: cancel.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: cancel.bmp, yAxis="+yAxis);
        setvirtualpinpad_para.VKBD_CANCEL.x = xAxis;
        setvirtualpinpad_para.VKBD_CANCEL.y = yAxis;
        setvirtualpinpad_para.VKBD_CANCEL.width = 120;
        setvirtualpinpad_para.VKBD_CANCEL.height = 100;
        setvirtualpinpad_para.VKBD_CANCEL.filepath = new File("/data/data/pub/back1.bmp");



        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY1.x = 50;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY1.y = 995;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY1.width = 140;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY1.height = 90;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY1.value = 'S';
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY1.filepath = new File("/data/data/pub/f1.bmp");
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY2.x = 210;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY2.y = 995;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY2.width = 140;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY2.height = 90;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY2.value = 'S';
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY2.filepath = new File("/data/data/pub/f2.bmp");
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY3.x = 370;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY3.y = 995;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY3.width = 140;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY3.height = 90;
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY3.value = 'S';
        setvirtualpinpad_para.VKBD_EX_FUNCTIONKEY3.filepath = new File("/data/data/pub/f3.bmp");
        setvirtualpinpad_para.setMovingRange(0,0,720,1100,20,20);

		byte outblocklen = 8;
        int keySet =inKeySet;
        int keyIndex = inKeyIndex;
        int PinBypassAllow = inPinBypassAllow;
        //byte[] PAN_number = StrInData.getBytes();
        int PAN_offset = 0;
        byte Null_PIN = 0;
        int Timeout = 360;
        int First_timeout = 0;
        int MaxDigit = 6,MinDigit = 4;
		//byte[] sk = {0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31};




		String[] Pindata = StrInData.split("\\|");


		
		Log.d("saturn", "saturn pinpadmksk after split");

		byte[] PAN_number = Pindata[0].getBytes();	
		
		String buf1 = (Convert.ByteArrayTohexString(PAN_number));
		Log.d("saturn", "saturn pinpadmksk after copy pan number:" + buf1);

		
		//byte[] sk = Pindata[1].getBytes();

		byte[] sk = Convert.hexStringToByteArray(Pindata[1]);
	
		String buf2 = (Convert.ByteArrayTohexString(sk));
		Log.d("saturn", "saturn pinpadmksk after copy sk:"+buf2);



        //byte[] PAN_number = {0x35, 0x32, 0x31, 0x30, 0x36, 0x39, 0x30, 0x33, 0x31, 0x37, 0x39, 0x30, 0x34, 0x37, 0x36, 0x34};
        //byte[] sk = {0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31};
        //byte[] sk = {0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32};
        //int keySet =0xC000;
        //int keyIndex = 0x0001;
        //Byte outblocklen = 8;
        //byte Null_PIN = 0;
        //int Timeout = 60;
        //int First_timeout = 0;
        //int MaxDigit = 12,MinDigit = 4;

        if(PinBypassAllow==1)
            Null_PIN = 1;

        try{
		/*
            CtKMS2System system = new CtKMS2System();
            system.setPINPADMoving(false);
            system.setPINScrambling(true);
            system.setPINSound(true);
            Log.d(TAG, String.format("saturn keySet = %d keyinde = %d", keyIndex, keyIndex));
            Log.d(TAG, String.format("saturn PAN_number.length = %d", PAN_number.length));
            CtKMS2MKSK mksk = new CtKMS2MKSK(); 
			byte baSK[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; //hardcoded SK

			
			
			mksk.selectKey(keySet, keyIndex);  
			mksk.setCipherMethod(mksk.PIN_CIPHER_METHOD_ECB);  
			mksk.setInputData(PAN_number, PAN_offset, PAN_number.length); 
			mksk.setPinInfo(mksk.PIN_BLOCKTYPE_ANSI_X9_8_ISO_0, (byte)MaxDigit, (byte)MinDigit, outblocklen);
			mksk.setSK(baSK);
			
            mksk.setPinControl(Null_PIN, Timeout, First_timeout);
            mksk.setCallback(callback);
            background_OFF_Handler.post(background_off);
			mksk.startVirtualPin(activity, setvirtualpinpad_para);
            String out = (Convert.ByteArrayTohexString(mksk.getOutpuData()));
            Log.d(TAG, "saturn OutpuData:" + out);
            background_ON_Handler.post(background_on);
            */
            CtKMS2System system = new CtKMS2System();
            system.setPINPADMoving(false);
            system.setPINScrambling(true);
            system.setPINSound(true);
            CtKMS2MKSK mksk = new CtKMS2MKSK();
            mksk.selectKey(keySet,keyIndex);

            String sKeySet = String.format("%04X", mksk.getKeySet());
            String sKeyIndex = String.format("%04X", mksk.getKeyIndex());
            String sKeyVersion = Integer.toHexString(0x100 | mksk.getKeyVersion()).substring(1);
            String sKeyType = Integer.toHexString(0x100 | mksk.getKeyType()).substring(1);
            String sKeyAttribute = String.format("%08X", mksk.getKeyAttribute());
            String sKeyLen = String.format("%08X", mksk.getKeyLength());
            String sCV = Convert.ByteArrayTohexString(mksk.getCV(3));
            Log.d("saturn",  "KeySet = " + sKeySet);
            Log.d("saturn",  "KeyIndex = " + sKeyIndex);
            Log.d("saturn",  "KeyVersion = " + sKeyVersion);
            Log.d("saturn", "KeyType = " + sKeyType);
            Log.d("saturn",  "KeyAttribute = " + sKeyAttribute);
            Log.d("saturn",  "KeyLen = " + sKeyLen);
            Log.d("saturn", "CV = " + sCV);

			byte output[] = new byte [16];


            mksk.setCipherMethod(mksk.PIN_CIPHER_METHOD_ECB);
            mksk.setPinInfo(mksk.PIN_BLOCKTYPE_ANSI_X9_8_ISO_0,(byte)MaxDigit,(byte)MinDigit,outblocklen);
            mksk.setCallback(callback);
           // mksk.setInputData(PAN_number,0,16);
		    mksk.setInputData(PAN_number,0,PAN_number.length);  
            mksk.setPinControl(Null_PIN, Timeout, First_timeout);
            mksk.setSK(sk);
            background_OFF_Handler.post(background_off);
           // mksk.startVirtualPin(activity, R.id.layout);
           mksk.startVirtualPin(activity, setvirtualpinpad_para);
            background_ON_Handler.post(background_on);
            output = mksk.getOutpuData();
            Log.d("saturn", "saturn test after mksk");
            

            String out = (Convert.ByteArrayTohexString(output));
			Log.d(TAG, "saturn PINBLOCK:" + out);
            MainActivity.sPinBlock=out;
            MainActivity.inRet = 0;
            MainActivity.Enter_Press = 1;


        } catch (CTOS.CtKMS2Exception e) {
            MainActivity.Enter_Press = 1;
            MainActivity.inRet = 1;
            rtn = Integer.toHexString(e.getError());
            MainActivity.sKSN = rtn;
            Log.d(TAG, "saturn rtn =" + rtn);
            background_ON_Handler.post(background_on);
        }
    }
}
