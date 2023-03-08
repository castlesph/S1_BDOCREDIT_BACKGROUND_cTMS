package com.Source.S1_BDO.BDO.Main;

import android.app.Activity;
import android.support.annotation.WorkerThread;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;


import com.Source.S1_BDO.BDO.Kms.Convert;
import com.Source.S1_BDO.BDO.Kms.KMSUI;
import com.Source.S1_BDO.BDO.R;

import java.io.File;

import CTOS.CtEMVCusPINPadbyImg;

public class JNICB_offlinepin
{
    public JNICB_offlinepin() {
    }

    public interface  cb_EMV
	{
        long ShowVirtualPIN(byte[] pVirtualPinPadPara);
        long ShowPINDigit(byte digitsNum, final byte pinType, final byte remainingCounter);
        long GetPINDone();
    }
}


class JNICB_implements_EMV implements JNICB_offlinepin.cb_EMV
{
	private static final String TAG = "JNICB_offlinepin";

	EditText PinDigitET;
    MainActivity MainContext;
    CtEMVCusPINPadbyImg vpinpad = new CtEMVCusPINPadbyImg();

    public JNICB_implements_EMV(MainActivity InContext)
    {
        MainContext = InContext;

        // (full screen resolution height: 1280 and width: 720)
		// top status bar height: 48 , bottom navigation bar height: 96
		// if both bar exist (not hide or layout_hide)
		// then the main layout will be from left top (0,48) to right bottom (720,1184) ,
		// sub layouts function or value below will set the margin value form (0,48) if top status bar exist,so margin(0,0) will be the(0,48) on screen
		// Show Pinpad by default :
        //f1f2f3
        // 0 1 2 back('R')
        // 3 4 5 enter('A')
        // 6 7 8 cancel('C')
        //   9

        int xAxis = 0;
        int yAxis = 0;
        int iWidth = 310;
        int iHeight = 150;
        int iGap = 2;
        int iXOffSet = 20;
        int iYOffSet = 30;

		xAxis = 0;
		yAxis = 0;
		iWidth = 310;
		iHeight = 150;
		iGap = 2;
		iXOffSet = 20;
		iYOffSet = 30;

		xAxis = iXOffSet;
        yAxis = 450;
        Log.i(TAG, "run: 0.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 0.bmp, yAxis="+yAxis);
        vpinpad.VKBD_0.x = xAxis;
		vpinpad.VKBD_0.y = yAxis;
		vpinpad.VKBD_0.width = iWidth;
		vpinpad.VKBD_0.height = iHeight;		
//      vpinpad.VKBD_0.x = 50;
//		vpinpad.VKBD_0.y = 545;
//		vpinpad.VKBD_0.width = 140;
//		vpinpad.VKBD_0.height = 140;
		vpinpad.VKBD_0.filepath = new File("/data/data/pub/0.bmp");
        xAxis+=iHeight+iGap+iYOffSet;
        Log.i(TAG, "run: 1.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 1.bmp, yAxis="+yAxis);
		vpinpad.VKBD_1.x = xAxis;
		vpinpad.VKBD_1.y = yAxis;
		vpinpad.VKBD_1.width = iWidth;
		vpinpad.VKBD_1.height = iHeight;
//		vpinpad.VKBD_1.x = 210;
//		vpinpad.VKBD_1.y = 545;
//		vpinpad.VKBD_1.width = 140;
//		vpinpad.VKBD_1.height = 140
		vpinpad.VKBD_1.filepath = new File("/data/data/pub/1.bmp");
        xAxis+=iHeight+iGap+iYOffSet;
        Log.i(TAG, "run: 2.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 0.bmp, yAxis="+yAxis);
		vpinpad.VKBD_2.x = xAxis;
		vpinpad.VKBD_2.y = yAxis;
		vpinpad.VKBD_2.width = iWidth;
		vpinpad.VKBD_2.height = iHeight;
//		vpinpad.VKBD_2.x = 370;
//		vpinpad.VKBD_2.y = 545;
//		vpinpad.VKBD_2.width = 140;
//		vpinpad.VKBD_2.height = 140;
		vpinpad.VKBD_2.filepath = new File("/data/data/pub/2.bmp");

        xAxis = iXOffSet;
        yAxis = 605;
        Log.i(TAG, "run: 3.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 3.bmp, yAxis="+yAxis);
		vpinpad.VKBD_3.x = xAxis;
		vpinpad.VKBD_3.y = yAxis;
		vpinpad.VKBD_3.width = iWidth;
		vpinpad.VKBD_3.height = iHeight;
//		vpinpad.VKBD_3.x = 50;
//		vpinpad.VKBD_3.y = 695;
//		vpinpad.VKBD_3.width = 140;
//		vpinpad.VKBD_3.height = 140;
		vpinpad.VKBD_3.filepath = new File("/data/data/pub/3.bmp");

        xAxis+=iHeight+iGap+iYOffSet;
        Log.i(TAG, "run: 4.bmp, xAxis="+xAxis);
		vpinpad.VKBD_4.x = xAxis;
		vpinpad.VKBD_4.y = yAxis;
		vpinpad.VKBD_4.width = iWidth;
		vpinpad.VKBD_4.height = iHeight;
//		vpinpad.VKBD_4.x = 210;
//		vpinpad.VKBD_4.y = 695;
//		vpinpad.VKBD_4.width = 140;
//		vpinpad.VKBD_4.height = 140;		
		vpinpad.VKBD_4.filepath = new File("/data/data/pub/4.bmp");
        xAxis+=iHeight+iGap+iYOffSet;
        Log.i(TAG, "run: 5.bmp, xAxis="+xAxis);
		vpinpad.VKBD_5.x = xAxis;
		vpinpad.VKBD_5.y = yAxis;
		vpinpad.VKBD_5.width = iWidth;
		vpinpad.VKBD_5.height = iHeight;
//		vpinpad.VKBD_5.x = 370;
//		vpinpad.VKBD_5.y = 695;
//		vpinpad.VKBD_5.width = 140;
//		vpinpad.VKBD_5.height = 140;		
		vpinpad.VKBD_5.filepath = new File("/data/data/pub/5.bmp");

        xAxis = iXOffSet;
        yAxis = 765;
        Log.i(TAG, "run: 6.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 6.bmp, yAxis="+yAxis);
		vpinpad.VKBD_6.x = xAxis;
		vpinpad.VKBD_6.y = yAxis;
		vpinpad.VKBD_6.width = iWidth;
		vpinpad.VKBD_6.height = iHeight;
//		vpinpad.VKBD_6.x = 50;
//		vpinpad.VKBD_6.y = 845;
//		vpinpad.VKBD_6.width = 140;
//		vpinpad.VKBD_6.height = 140;
		vpinpad.VKBD_6.filepath = new File("/data/data/pub/6.bmp");

        xAxis+=iHeight+iGap+iYOffSet;
        Log.i(TAG, "run: 7.bmp, xAxis="+xAxis);
		vpinpad.VKBD_7.x = xAxis;
		vpinpad.VKBD_7.y = yAxis;
		vpinpad.VKBD_7.width = iWidth;
		vpinpad.VKBD_7.height = iHeight;
//		vpinpad.VKBD_7.x = 210;
//		vpinpad.VKBD_7.y = 845;
//		vpinpad.VKBD_7.width = 140;
//		vpinpad.VKBD_7.height = 140;		
		vpinpad.VKBD_7.filepath = new File("/data/data/pub/7.bmp");

        xAxis+=iHeight+iGap+iYOffSet;
        Log.i(TAG, "run: 8.bmp, xAxis="+xAxis);
		vpinpad.VKBD_8.x = xAxis;
		vpinpad.VKBD_8.y = yAxis;
		vpinpad.VKBD_8.width = iWidth;
		vpinpad.VKBD_8.height = iHeight;
//		vpinpad.VKBD_8.x = 370;
//		vpinpad.VKBD_8.y = 845;
//		vpinpad.VKBD_8.width = 140;
//		vpinpad.VKBD_8.height = 140;
		vpinpad.VKBD_8.filepath = new File("/data/data/pub/8.bmp");

		xAxis = iXOffSet;
		yAxis = 925;

		Log.i(TAG, "run: backspace.bmp, xAxis="+xAxis);
		vpinpad.VKBD_CLEAR.x = xAxis;
		vpinpad.VKBD_CLEAR.y = yAxis;
		vpinpad.VKBD_CLEAR.width = iWidth;
		vpinpad.VKBD_CLEAR.height = iHeight;
//		vpinpad.VKBD_CLEAR.x = 530;
//		vpinpad.VKBD_CLEAR.y = 695;
//		vpinpad.VKBD_CLEAR.width = 140;
//		vpinpad.VKBD_CLEAR.height = 140;
		vpinpad.VKBD_CLEAR.filepath = new File("/data/data/pub/backspace.bmp");

		xAxis+=iHeight+iGap+iYOffSet;
		// xAxis = 202;
        // yAxis = 925;
        Log.i(TAG, "run: 9.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: 9.bmp, yAxis="+yAxis);
		vpinpad.VKBD_9.x = xAxis;
		vpinpad.VKBD_9.y = yAxis;
		vpinpad.VKBD_9.width = iWidth;
		vpinpad.VKBD_9.height = iHeight;
//		vpinpad.VKBD_9.x = 210;
//		vpinpad.VKBD_9.y = 995;
//		vpinpad.VKBD_9.width = 140;
//		vpinpad.VKBD_9.height = 140;
		vpinpad.VKBD_9.filepath = new File("/data/data/pub/9.bmp");

		xAxis+=iHeight+iGap+iYOffSet;
		Log.i(TAG, "run: enter.bmp, xAxis="+xAxis);
		vpinpad.VKBD_ENTER.x = xAxis;
		vpinpad.VKBD_ENTER.y = yAxis;
		vpinpad.VKBD_ENTER.width = iWidth;
		vpinpad.VKBD_ENTER.height = iHeight;
//		vpinpad.VKBD_ENTER.x = 530;
//		vpinpad.VKBD_ENTER.y = 545;
//		vpinpad.VKBD_ENTER.width = 140;
//		vpinpad.VKBD_ENTER.height = 140;
		vpinpad.VKBD_ENTER.filepath = new File("/data/data/pub/enter.bmp");

        xAxis=iXOffSet;
        yAxis = 1085;
        Log.i(TAG, "run: cancel.bmp, xAxis="+xAxis);
        Log.i(TAG, "run: cancel.bmp, yAxis="+yAxis);
		vpinpad.VKBD_CANCEL.x = xAxis;
		vpinpad.VKBD_CANCEL.y = yAxis;
		vpinpad.VKBD_CANCEL.width = iWidth;
		vpinpad.VKBD_CANCEL.height = iHeight;
//		vpinpad.VKBD_CANCEL.x = 530;
//		vpinpad.VKBD_CANCEL.y = 845;
//		vpinpad.VKBD_CANCEL.width = 140;
//		vpinpad.VKBD_CANCEL.height = 140;
		// vpinpad.VKBD_CANCEL.filepath = new File("/data/data/pub/cancel.bmp");

		vpinpad.VKBD_EX_FUNCTIONKEY1.x = 50;
		vpinpad.VKBD_EX_FUNCTIONKEY1.y = 995;
		vpinpad.VKBD_EX_FUNCTIONKEY1.width = 140;
		vpinpad.VKBD_EX_FUNCTIONKEY1.height = 90;
		vpinpad.VKBD_EX_FUNCTIONKEY1.value = 'S';// set 'S' ->f1 won't show on screen
		vpinpad.VKBD_EX_FUNCTIONKEY1.filepath = new File("/data/data/pub/f1.bmp");
		vpinpad.VKBD_EX_FUNCTIONKEY2.x = 210;
		vpinpad.VKBD_EX_FUNCTIONKEY2.y = 995;
		vpinpad.VKBD_EX_FUNCTIONKEY2.width = 140;
		vpinpad.VKBD_EX_FUNCTIONKEY2.height = 90;
		vpinpad.VKBD_EX_FUNCTIONKEY2.value = 'S';// set 'S' ->f2 won't show on screen
		vpinpad.VKBD_EX_FUNCTIONKEY2.filepath = new File("/data/data/pub/f2.bmp");
		vpinpad.VKBD_EX_FUNCTIONKEY3.x = 370;
		vpinpad.VKBD_EX_FUNCTIONKEY3.y = 995;
		vpinpad.VKBD_EX_FUNCTIONKEY3.width = 140;
		vpinpad.VKBD_EX_FUNCTIONKEY3.height = 90;
		vpinpad.VKBD_EX_FUNCTIONKEY3.value = 'S';// set 'S' ->f3 won't show on screen
		vpinpad.VKBD_EX_FUNCTIONKEY3.filepath = new File("/data/data/pub/f3.bmp");
        vpinpad.setMovingRange(false,0,0,720,1100,20,20);

		vpinpad.BtnRandom(true); // random pinpad (pin with random order) if true
		vpinpad.SetPinDigit_Color_Size("#000000",20);//PIN Digits color and size
		vpinpad.SetPINDigits("*"); //set pin digit which show on screen
		vpinpad.SetPinDigit_leftMargin_topMargin(160,60);

		 vpinpad.SetScreenBackground("#00000000"); //main layout background, if you want layout transparent, set #00000000.
		 vpinpad.SetPinDigitEnable(false); //EMV layout show PinDigit TextView
		 vpinpad.SetPinTextEnable(false); //EMV layout show PinText TextView

		 //vpinpad.SetPINSoundEnable(true);
		//vpinpad.InitAllVKBD_WidthHeight_and_Padding(130,130,20,360,765);//(width_of_img,heigh_of_img,padding between img,center X,center Y)
		//call InitAllVKBD_WidthHeight_and_Padding first when needed(after setting values of VKBD)
		//be ware of the size of the layout group,last two parameter are center(x,y) of the layout group,and (x,y)should not near edge

        /*
		  vpinpad.BtnRandom(true); // random pinpad (pin with random order) if true
		  
        vpinpad.setMovingRange(true,0,150,720,1100,20,20);
		// set whole pinpad moving randomly in range of top left (x,y) to bottom right(x,y) with (x,y)distance unit
		
        int[] VKBD_position = { 1,2,3,
                                4,5,6,
                                7,8,9,
								   0,  'R', //back('R')
                                       'A', //enter('A')
									   'C'};//cancel('C')
        vpinpad.SetVKBD_Order(VKBD_position);
            //VKBD_position[x]
            //VKBD_position[0] = 1 ; -> put VKBD_1 in position 0
            // (position map below)
            //x= 0 1 2 10
            //   3 4 5 11
            //   6 7 8 12
            //     9

       	vpinpad.SetAllVKBD_WidthHeight(140,140); //Width Height of PIN
        vpinpad.SetAllVKBD_EX_WidthHeight(140,90); //Width Height of F1 F2 F3
        vpinpad.SetAllVKBD_and_EX_Offset(-5,50);  // move all PIN by +- its (x,y)
        vpinpad.SetPINDigits("*"); //set pin digit which show on screen
		
        //*/

		 /*
        vpinpad.SetScreenBackground("#FFFFFF");//main layout background

        vpinpad.SetTextViewBackground("#FFFFFF");//title text view background
		vpinpad.SetPinDigitText_Color_Size("#000000",20);//title text color and size

        vpinpad.SetPINDigitsBackground("#EEEEEE");//PIN Digits view background
        vpinpad.SetPinDigit_Color_Size("#000000",20);//PIN Digits color and size


        vpinpad.SetPinDigitTextView_width_height(ViewGroup.LayoutParams.MATCH_PARENT,ViewGroup.LayoutParams.WRAP_CONTENT);
        //set width and height parameter of title text view
        vpinpad.SetPinDigitTextView_leftMargin_topMargin(0,0);
        //if top status bar exist and not hide or layout_hide then set (0,0) will be (0,48) from screen most top left


        vpinpad.SetPinDigit_width_height(200,30);
        //set width and height parameter of Pin Digit view
        vpinpad.SetPinDigit_leftMargin_topMargin(160,60);
        //if top status bar exist and not hide or layout_hide then set (160,60) will be (160,108) from screen most top left
		// */


		//vpinpad.Set_hideSystembar(true); //don't use if it cause system bar change unexpected after removeAllView function
		//set true to hide system bar by using the flag that Set_SystemUiVisibility had set

		 /*
		vpinpad.Set_SystemUiVisibility( // hide system bar by value below
				// Set the content to appear under the system bars so that the
				// content doesn't resize when the system bars hide and show.
				View.SYSTEM_UI_FLAG_LAYOUT_STABLE
				| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
				| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
				// Hide the nav bar and status bar
				| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
				| View.SYSTEM_UI_FLAG_FULLSCREEN
		);
		//*/
		//vpinpad.Set_Yoffset_to_kernel(48);//pass (position of img + Yoffset)  to kernel //this is a back up
		// if the actual press zone is higher(lower), set a positive(negative) number to make its y position lower(higher),

	}


    boolean isViewCreated = false;
    public long ShowVirtualPIN(byte[] pVirtualPinPadPara)
    {
    	MainContext.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				MainContext.setContentView(R.layout.activity_virtual_pin);

				PinDigitET = (EditText) MainContext.findViewById(R.id.PinDigitEditText);
			}
		});
			
        if(GlobalPara.isPINPadAlone == true)
        {
            Log.d("pVirtualPinPadPara IN", Convert.ByteArrayTohexString(pVirtualPinPadPara));

            vpinpad.showPINPAD(MainContext,pVirtualPinPadPara);

			Log.d("pVirtualPinPadPara : ", Convert.ByteArrayTohexString(pVirtualPinPadPara));
        }
       
        return 0;
    }


    public long ShowPINDigit(byte digitsNum, final byte pinType, final byte remainingCounter)
    {
		Log.i(TAG, "ShowPINDigit: run");
		Log.i(TAG, "digitsNum="+digitsNum);
		Log.i(TAG, "pinType="+pinType);
		Log.i(TAG, "remainingCounter="+remainingCounter);

        String title;

        if (pinType == 0)
            title="Enter Online Pin :";
        else {
        	if (remainingCounter == 1)
				title = "ReEnter Offline Pin :";
        	else
			title = "Enter Offline Pin :";
		}

        vpinpad.showPINDigits(digitsNum,title);

		PinDigitET.setText(digitsNum);

		Log.i(TAG, "title="+title);

        return 0;
    }

    public long GetPINDone()
    {

		vpinpad.removeAllView(MainContext);//already in ui thread
		
		try
        {
          //  Thread.sleep(300);//if ui tread take too long
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        return 0;
    }
}
