package com.Source.S1_BDO.BDO;

import android.content.Context;
import android.telephony.PhoneStateListener;
import android.telephony.SignalStrength;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.Source.S1_BDO.BDO.Global.Global;
import com.Source.S1_BDO.BDO.Main.MainActivity;

import static com.Source.S1_BDO.BDO.Main.MultiAP_SubAP.TAG;


public class OperatorHolder extends PhoneStateListener {
    private TelephonyManager manager;
    public int signalStrengthValue;

    public OperatorHolder(Context context){
        manager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    }
    public String getOperatorName() {
        return manager.getNetworkOperatorName();
    }

    /*
    @Override
    public void onSignalStrengthsChanged(SignalStrength signalStrength) {

        Log.i(TAG, "onSignalStrengthsChanged: run");
        Log.i(TAG, "signalStrength.isGsm()="+signalStrength.isGsm());
        if (signalStrength.isGsm()) {
            signalStrengthValue = signalStrength.getGsmSignalStrength();
        } else {
            signalStrengthValue = signalStrength.getCdmaDbm();
        }

        Global.signalStrength = signalStrengthValue;
        Log.i(TAG, "signalStrengthValue="+signalStrengthValue);
        Log.i(TAG, "Global.signalStrength="+Global.signalStrength);
        Log.i(TAG, "Global.isSplash="+Global.isSplash);

        if (Global.isSplash)
            Splash.displaySignalStrength(signalStrengthValue);
        else
            MainActivity.displaySignalStrength(signalStrengthValue);
    }
     */
}
