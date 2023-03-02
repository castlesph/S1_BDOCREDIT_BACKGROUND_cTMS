package com.Source.S1_MCC.MCC;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Looper;
import android.util.Log;

public class BootUpReceiver extends BroadcastReceiver {
    private static final String TAG = "BootUpReceiver";
    public boolean isMainThread() {
        return Looper.getMainLooper() == Looper.myLooper();
    }
    @Override
    public void onReceive(Context context, Intent intent) {
        Log.i(TAG, "onReceive: " + isMainThread());

    }
}
