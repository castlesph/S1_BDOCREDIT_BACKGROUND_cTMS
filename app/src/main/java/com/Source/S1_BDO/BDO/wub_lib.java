package com.Source.S1_BDO.BDO;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Handler;
import android.text.InputFilter;
import android.text.InputType;
import android.text.method.PasswordTransformationMethod;
import android.util.Log;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;

import com.Source.S1_BDO.BDO.R;

import java.io.File;
import java.util.Arrays;

import static android.content.ContentValues.TAG;

public class wub_lib {
    private final static byte[] hex = "0123456789ABCDEF".getBytes();  
    private static int parse(char c) {  
        if (c >= 'a')  
            return (c - 'a' + 10) & 0x0f;  
        if (c >= 'A')  
            return (c - 'A' + 10) & 0x0f;  
        return (c - '0') & 0x0f;  
    }  
 
    public static String Bytes2HexString(byte[] b) {
        byte[] buff = new byte[2 * b.length];  
        for (int i = 0; i < b.length; i++) {  
            buff[2 * i] = hex[(b[i] >> 4) & 0x0f];  
            buff[2 * i + 1] = hex[b[i] & 0x0f];  
        }  
        return new String(buff);
    }  
   
    public static byte[] HexString2Bytes(String hexstr) {
        byte[] b = new byte[hexstr.length() / 2];  
        int j = 0;  
        for (int i = 0; i < b.length; i++) {  
            char c0 = hexstr.charAt(j++);  
            char c1 = hexstr.charAt(j++);  
            b[i] = (byte) ((parse(c0) << 4) | parse(c1));  
        }  
        return b;  
    }  
    public static void main(String[] args) {
        byte[] bt = new byte[]{10, 2, 12, 14, 1, 0, 0, 1, 0, 31, 45, 1, 8, 0, 1, 0, -96, -45, 10, 3};  
        System.out.println(Bytes2HexString(bt));
        System.out.println(Arrays.toString(HexString2Bytes("0A020C0E01000001001F2D0108000100A0D30A03")));
    }

    // sidumili: added to call function in any java, code optimization
    public static void ViewImageResourcesByType(String sType, ImageView imageView)
    {
        Log.i("sidumili", "ViewImageResourcesByType: ");

        if(sType.equals("success"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_success));
        else if(sType.equals("error"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_error));
        else if(sType.equals("warning"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_warning));
        else if(sType.equals("info"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_info));
        else if(sType.equals("question"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_question));
        else if(sType.equals("print"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_print));
        else if(sType.equals("process"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_process));
        else if(sType.equals("send"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_send));
        else if(sType.equals("receive"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_receive));
        else if(sType.equals("wait"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_wait));
        else if(sType.equals("timeout"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_timeout));
        else if(sType.equals("init"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_init));
        else if(sType.equals("remove"))
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_remove));
        else
            imageView.setImageDrawable(imageView.getResources().getDrawable(R.drawable.icon_none));
    }
    // sidumili: add button press
    public static void ButtonPressed(Context context, Button btn)
    {
        // declare animation
        final Animation fade = AnimationUtils.loadAnimation(context, R.anim.fade);
        btn.startAnimation(fade);
    }

    // sidumili: check if file exist
    public static boolean isFileExist(String fileName)
    {
        boolean isExist = false;
        File file = new File(fileName);

        System.out.println("--isFileExist--");
        System.out.println("fileName="+fileName);

        if (!file.exists())
        {
            isExist = false;
            System.out.println("File " + fileName + " not exist.");
        }
        else
        {
            isExist = true;
            System.out.println("File " + fileName + " exist.");
        }

        return  isExist;
    }
    // sidumili: Set EditText object InputType
    public static void SetInputType(int inputType, EditText tv_value, boolean isInputPassword)
    {
        Log.i(TAG, "SetInputType: inputType="+inputType);
        Log.i(TAG, "SetInputType: isInputPassword="+isInputPassword);

        switch (inputType)
        {
            case 1: // Numeric Only
                if (!isInputPassword)
                {
                    tv_value.setRawInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
                }
                else
                {
                    tv_value.setRawInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_VARIATION_PASSWORD | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
                    tv_value.setTransformationMethod(PasswordTransformationMethod.getInstance());
                }

                break;
            case 2: // AlphaNumeric
                if (!isInputPassword)
                {
                    tv_value.setRawInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_NORMAL | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS | InputType.TYPE_TEXT_FLAG_CAP_CHARACTERS);
                }
                else
                {
                    tv_value.setRawInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD | InputType.TYPE_TEXT_VARIATION_NORMAL | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS | InputType.TYPE_TEXT_FLAG_CAP_CHARACTERS);
                    tv_value.setTransformationMethod(PasswordTransformationMethod.getInstance());
                }
                break;
            case 3: // Numeric / PhoneNo
                if (!isInputPassword)
                {
                    tv_value.setRawInputType(InputType.TYPE_CLASS_PHONE | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS | InputType.TYPE_TEXT_FLAG_CAP_CHARACTERS);
                }
                else
                {
                    tv_value.setRawInputType(InputType.TYPE_CLASS_PHONE | InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS | InputType.TYPE_TEXT_FLAG_CAP_CHARACTERS);
                    tv_value.setTransformationMethod(PasswordTransformationMethod.getInstance());
                }
                break;
        }
    }

    // sidumili: Set EditText object MaxLength
    public static void SetInputMaxLength(EditText tv_value, int iMaxLength)
    {
        Log.i(TAG, "SetInputMaxLength: iMaxLength="+iMaxLength);

        InputFilter[] filterArray = new InputFilter[1];
        filterArray[0] = new InputFilter.LengthFilter(iMaxLength);
        tv_value.setFilters(filterArray);
    }

    public static void finishActivity(Activity activity)
    {
        int inDelay = 10;

        Log.i(TAG, "finishActivity: run");
        Log.i(TAG, "activity="+activity);
        Log.i(TAG, "inDelay="+inDelay);

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                activity.finish();
            }
        }, inDelay);
    }
}  