package com.Source.S1_BDO.BDO.Main;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.BitmapDrawable;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.Process;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresApi;
import android.support.constraint.ConstraintLayout;
import android.support.v4.app.ActivityCompat;
import android.support.v4.view.ViewPager;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.Source.S1_BDO.BDO.BdoApplication;
import com.Source.S1_BDO.BDO.DMenu.Model;
import com.Source.S1_BDO.BDO.DMenu.GridViewAdapter;
import com.Source.S1_BDO.BDO.DMenu.ViewPagerAdapter;
import com.Source.S1_BDO.BDO.EcrCommandDefintion;
import com.Source.S1_BDO.BDO.EcrCommandReceiver;
import com.Source.S1_BDO.BDO.EditDatabaseActivity;
import com.Source.S1_BDO.BDO.Global.Global;
import com.Source.S1_BDO.BDO.Kms.KMSUI;
import com.Source.S1_BDO.BDO.OperatorHolder;
import com.Source.S1_BDO.BDO.SignaturePad.SignaturePadCastles;
import com.Source.S1_BDO.BDO.Trans.AmountEntryActivity;
import com.Source.S1_BDO.BDO.Trans.AmountEntryWithMenu;
import com.Source.S1_BDO.BDO.Trans.BatchReview;
import com.Source.S1_BDO.BDO.Trans.CardEntry;
import com.Source.S1_BDO.BDO.Trans.CopyMenu;
import com.Source.S1_BDO.BDO.Trans.EditListView;
import com.Source.S1_BDO.BDO.Trans.GetCardOptionsActivity;
import com.Source.S1_BDO.BDO.Trans.GetExpiryDateUI;
import com.Source.S1_BDO.BDO.Trans.InputAlpha;

import com.Source.S1_BDO.BDO.R;
import com.Source.S1_BDO.BDO.Trans.LockScreen;
import com.Source.S1_BDO.BDO.Trans.S1InputString;
import com.Source.S1_BDO.BDO.Trans.SingleDataRecordListView;
import com.Source.S1_BDO.BDO.Trans.TipEntryActivity;
import com.Source.S1_BDO.BDO.Trans.TransDetailListView;
import com.Source.S1_BDO.BDO.Trans.UserCancelUI;
import com.Source.S1_BDO.BDO.Trans.UserConfirmCard;
import com.Source.S1_BDO.BDO.Trans.UserConfirmDCC;
import com.Source.S1_BDO.BDO.Trans.UserConfirmDetails;
import com.Source.S1_BDO.BDO.Trans.UserConfirmTipAdjust;
import com.Source.S1_BDO.BDO.Trans.UserConfirmVoid;
import com.Source.S1_BDO.BDO.model.ConfirmInput;
import com.Source.S1_BDO.BDO.model.ConfirmMenu;
import com.Source.S1_BDO.BDO.model.ConfirmMenu2;
import com.Source.S1_BDO.BDO.model.ConfirmOKCancelMenu;
import com.Source.S1_BDO.BDO.model.ConfirmOKMenu;
import com.Source.S1_BDO.BDO.model.ConfirmTotal;
import com.Source.S1_BDO.BDO.model.ConfirmYesNoUI;
import com.Source.S1_BDO.BDO.model.DOptionMenu;
import com.Source.S1_BDO.BDO.model.DPopupMenuActivity;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;
import com.Source.S1_BDO.BDO.model.EliteReceiptonScreen;
import com.Source.S1_BDO.BDO.model.MenuSelection;
import com.Source.S1_BDO.BDO.model.NormalKeyBoard;
import com.Source.S1_BDO.BDO.model.DPopupMenu;
import com.Source.S1_BDO.BDO.model.PrintAnimation;
import com.Source.S1_BDO.BDO.model.PrintAnimation2;
import com.Source.S1_BDO.BDO.model.PrintAnimation3;
import com.Source.S1_BDO.BDO.model.PrintFirstReceiptonScreen;
import com.Source.S1_BDO.BDO.model.PrintReceiptonScreen;
import com.Source.S1_BDO.BDO.model.QwertyUI;
import com.Source.S1_BDO.BDO.model.UserInputString;
import com.Source.S1_BDO.BDO.utility.GetMemoryInfor;
import com.Source.S1_BDO.BDO.utility.TemporaryData;
import com.Source.S1_BDO.BDO.utility.GetAppInfor;
import com.Source.S1_BDO.BDO.utility.GetNetworkInfor;
import com.Source.S1_BDO.BDO.wub_lib;

import CTOS.CtCtms;
import CTOS.CtSystem;
import CTOS.CtSystemException;
//import splashapp.android.nttd.cas.com.mylibrary.CtLed;


import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.List;
import static android.content.ContentValues.TAG;

import static java.lang.Short.MAX_VALUE;

import pl.droidsonroids.gif.GifDrawable;
import pl.droidsonroids.gif.GifImageView;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;



public class MainActivity extends DemoAppActivity implements View.OnClickListener {
    public static boolean Chinese = false;
    public static int Finish_Printf = 0;

    private int idlecardentry_status = 0;
    public static boolean fSTART = false, fBlockTrans = false;
    public static boolean idlecardentry_emvfallback = false;
    String szMSR_status = "MSR_NO";

    TextView app_menu;

    public static int Enter_Press;
    public static String sPinBlock = "";
    public static String sKSN = "";
    public static int inRet;
    int inKeySet = 0;
    int inKeyIndex = 0;
    int inKeyType = 0;
    int inPinBypassAllow = 0;
    String StrInData = "";
    int inLockScreen = 0;
    public static String OutputStr = "";

    final KMSUI kmsui = new KMSUI();


    private static Handler mHandler;
    private static Context mContext;

    private TextView title, result;
    private ImageView refresh, check;
    private ImageView[] food = new ImageView[6];
    private TextView[] food_count = new TextView[6];

    private int[] foodPickMount = new int[6];
    private int[] foodPrice = {45, 35, 160, 30, 40, 25};
    private int[] inPaddingLeft = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    private int total_price = 0;
    int ctlsdemo = 0;

    private String in_string = "";

    TextView tv;
    EditText edtLog;
    Thread test;
    String list[] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    ListView lsv_LCDTPrint;
    ArrayAdapter adapter;
    String displayMode[] = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    int addPadding[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    boolean isInitialize = false;

    Double dAmount;
    EditText etAmount;
    EditText etAmount1;
    private String amt_string = "";

    private int uiFlag = 0;

    private String pass_in_string = "";
    private DPopupMenu popMenu;
    AdapterView mAdapter[];

    /* Database */
//    private Cursor employees;
//    private MyDatabase db;

    /*
    SD or flash card write
     */
    private EditText editname;
    private EditText editdetail;
    private Button btnsave;
    private Button btnclean;
    private Button btnread;
    private Button btnauto;

    int inGravity;


    TextView tvMsgLine01;
    TextView tvMsgLine02;
    TextView tvMsgLine03;
    TextView tvMsgLine04;
    TextView tvMsgLine05;
    TextView tvMsgLine06;
    TextView tvMsgLine07;
    TextView tvMsgLine08;
    TextView tvMsgLine09;
    TextView tvMsgLine10;
    TextView tvMsgLine11;
    TextView tvMsgLine12;
    TextView tvMsgLine13;
    TextView tvMsgLine14;
    TextView tvMsgLine15;
    TextView tvMsgLine16;

    ImageView imageView;

    // Header
    String AppHeaderBuff;
    TextView app_version;
    TextView merchant;
    TextView tprofile;
    TextView carrier_name;

    /* Dmenu */
    //private String[] titles = {"INIT WAVE", "NETS Flash Pay", "NETS", "Credit Card/UPI", "NETS QR", "BCA Card", "NETS PP/Loyalty", "CEPAS", "NETS Cash Card", "Admin", "Setting",
    //"demo", "SALE", "VOID", "Comm Test", "EditDB", "DispMsg", "DispImg", "SETTLE"};
    //private String[] titles = {"SALE", "VOID", "SETTLE", "CARD_VER", "OFFLINE", "EditDB", "REPORTS"};
    //private String[] titles = {"SALE", "VOID", "INSTALLMENT", "SETTLEMENT", "BALANCE", "BATCH TOTAL", "BATCH REVIEW", "REPRINT", "REPORTS", "SETUP"}; //, "Edit Table"};
    //private String[] titles = {"START"};
    //private String[] titles = {"CARDS", "INSTALLMENT", "MOBILE WALLET", "REPRINT", "VOID", "BIN CHECK", "BALANCE INQUIRY", "SETTLEMENT", "BATCH TOTAL", "BATCH REVIEW", "REPORT", "LOGON", "RETRIEVE", "BDO PAY", "SETUP", "MANAGEMENT"};
    //private String[] images = {"ic_card.png", "ic_installment.png", "ic_wallet.png", "ic_reprint.png", "ic_void.png", "ic_bincheck.png", "ic_balinq.png", "ic_settlement.png", "ic_batchtotal.png", "ic_batchreview.png", "ic_report.png", "ic_logon.png", "ic_retrieve.png", "ic_bdopay.png", "ic_setup.png", "ic_management.png"};
    private String[] titles = null;
    private String[] images = null;
    private String gMenuImageList;

    private ViewPager mPager;
    private List<View> mPagerList;
    private List<Model> mDatas;
    private LinearLayout mLlDot;
    private LayoutInflater inflater;
    Button button;
    //private TextView textView_amtentry;
    int intransCount = 0;
    /**
     * 总的页数
     */
    private int pageCount;
    /**
     * 每一页显示的个数
     */
    private int pageSize = 35;
    /**
     * 当前显示的是第几页
     */
    private int curIndex = 0;

    private boolean isExit = false;
    private boolean isWaitEvent = false;

    private int inTimeOut = 30;

    private int inMessageDelay = 10; // 100 is default

    public static ImageView img_signalstrength;

    static {
        System.loadLibrary("crypto");
    }

    static {
        System.loadLibrary("ssl");
    }

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private JNI_Lib jni_api = new JNI_Lib();
    private JNICB_implements_CTOS jni_cb_ctos = new JNICB_implements_CTOS(this);
    private JNICB_implements_IPT jni_cb_ipt = new JNICB_implements_IPT(this);

    private JNI_offlinepin jni_api_offline_pin = new JNI_offlinepin();
    private JNICB_implements_EMV jni_cb_emv = new JNICB_implements_EMV(this);

    public void vdClearFile(String fileName) {

        Log.i(TAG, "del file" + fileName);
        //			  String fileName = "/data/data/pub/TERMINAL.S3DB";
        try {
            File file = new File(fileName);
            if (!file.exists()) {
                Log.i(TAG, "[" + fileName + "]" + "  Not exist");
            } else {
                if (file.isFile()) {
                    Log.i(TAG, "file exist: " + fileName + "  Deleting...");
                    boolean ret = false;
                    ret = file.delete();
                    Log.i(TAG, "del result:: " + ret);
                    //											  file.deleteOnExit();
                    File file1 = new File(fileName);
                    if (!file1.exists()) {
                        Log.i(TAG, "Del file:" + "Not exist" + "  expected!!!");
                    }
                    //											  ret = deleteFile(fileName);
                    Log.i(TAG, "del result: " + ret);
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "exception: " + e.getMessage());
            e.printStackTrace();
        }

    }


    private void copyFileOrDir(String path) {
        AssetManager assetManager = this.getAssets();
        String assets[] = null;
        try {
            Log.i("Castles", "copyFileOrDir() " + path);
            assets = assetManager.list(path);
            if (assets.length == 0) {
                copyFile(path);
            } else {
                for (int i = 0; i < assets.length; ++i) {
                    String p;
                    if (path.equals(""))
                        p = "";
                    else
                        p = path + "/";

                    if (!path.startsWith("images") && !path.startsWith("sounds") && !path.startsWith("webkit"))
                        copyFileOrDir(p + assets[i]);
                }
            }
        } catch (IOException ex) {
            Log.e("Castles", "I/O Exception", ex);
        }
    }

    public int chmod(File path, int mode) throws Exception {
        Class fileUtils = Class.forName("android.os.FileUtils");
        Method setPermissions =
                fileUtils.getMethod("setPermissions", String.class, int.class, int.class, int.class);
        return (Integer) setPermissions.invoke(null, path.getAbsolutePath(), mode, -1, -1);
    }

    private void copyFile(String filename) {
        AssetManager assetManager = this.getAssets();
        int inFlagFs_dataSkip = 0;

        InputStream in = null;
        OutputStream out = null;
        String newFileName = null;

        String strFilename = filename.substring(filename.lastIndexOf("/") + 1);
        Log.i("Castles", "strFilename() " + strFilename);

        try {
            Log.i("Castles", "copyFile() " + filename);

			if (filename.toLowerCase().startsWith("21"))
			{
				newFileName = "/data/data/pub/" + strFilename;
			}
			else if (filename.toLowerCase().startsWith("11"))
			{
				newFileName = "/data/data/font/" + strFilename;
			}
			else if (filename.toLowerCase().startsWith("55"))
			{
				newFileName = "/data/data/pub/" + strFilename;
			}
			else if (filename.toLowerCase().startsWith("56"))
			{
				newFileName = "/data/data/pub/" + strFilename;		
			}
            else if (filename.toLowerCase().startsWith("04"))
            {
                inCTOSS_CreatefsdataFolder();

                newFileName = "/data/data/" + this.getPackageName() + "/fs_data/" + strFilename;
                // Log.i(TAG, "copyFile: strFilename " + strFilename  +  " newFileName: " + newFileName);

                //if (inCTOSS_fGetECRFlg() != 1)
                //{
                    if(strFilename.equals("castleslogo.bmp"))
                    {
                        newFileName = "/data/data/" + this.getPackageName() + "/fs_data/" + "logo.bmp";
                        Log.i(TAG, "copyFile: strFilename " + strFilename  +  " newFileName: " + newFileName);
                    }
                    else if(strFilename.equals("logo.bmp"))
                    {
                        Log.i(TAG, "copyFile: return strFilename " + strFilename  +  " newFileName: " + newFileName);
                        return;
                    }
                //}

                inFlagFs_dataSkip = 1;
            }
            else
			{
	       	     newFileName = "/data/data/" + this.getPackageName() + "/" + strFilename;
			}
			
            Log.i("Castles", "copyFile() Path " + newFileName);
			
            Log.i("Castles", "copyFile() Path " + newFileName);

            File dbFile = new File(newFileName);

            // patrick testing code start 20190703
            if (dbFile.exists())
            {
                if (inFlagFs_dataSkip == 0)
                    dbFile.delete();
                else
                    inFlagFs_dataSkip = 0;
            }
            // patrick testing code end 20190703

            if (!dbFile.exists()) {
                Log.i("Castles", "copyFile() !exists");
                in = assetManager.open(filename);
                out = new FileOutputStream(newFileName);

                byte[] buffer = new byte[1024];
                int read;
                while ((read = in.read(buffer)) != -1) {
                    out.write(buffer, 0, read);
                }
                in.close();
                in = null;
                out.flush();
                out.close();
                out = null;
                chmod(new File(newFileName), 0777);
            }
        } catch (Exception e) {
            Log.e("Castles", "Exception in copyFile() of " + newFileName);
            Log.e("Castles", "Exception in copyFile() " + e.toString());
        }
    }

    public static class Lock {
        private boolean condition;

        public boolean conditionMet() {
            return condition;
        }

        public void setCondition(boolean condition) {
            this.condition = condition;
        }
    }


    public static Lock LOCK = new Lock();

    final Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            Log.i("handleMessage", "Process");
            Log.i(TAG, "handleMessage: msg.what="+msg.what);
            if (msg.what == 1) {
                try {
                    if(inLockScreen == 1)
                        executeLockScreen(); // lock screen -- sidumili
                }
                catch (Exception ex)
                {
                    Log.i(TAG, "handleMessage: message="+ex.getMessage());
                }
                GetMenu();
            }
        }
    };

    private void CheckInstallFlagAndRunCTMS(SharedPreferences wmbPreference)
    {
        int ret = 0;
        boolean isCheckInstallFlag = wmbPreference.getBoolean("CHECKINSTALL", false);
        Log.i(TAG, "isCheckInstallFlag=: " + isCheckInstallFlag);
        if (isCheckInstallFlag )
        {
            CtCtms ctCtms = BdoApplication.getCtCtmsObj();
            if(ctCtms != null) {
                ret = ctCtms.ResetFolder();
                Log.i(TAG, "ResetFolder=: " + ret);
            }
            wmbPreference.edit().remove("CHECKINSTALL").commit();
            Log.i(TAG, "call ctms udpate: ");
            Intent intent = new Intent();
            intent.putExtra("UPDATE_CONFIG_FLAG", false);
            intent.setClass(MainActivity.this, Activity_ctms_background.class);
            startActivity(intent);

        }
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate MainActivity");
        super.onCreate(savedInstanceState);
        TemporaryData.init(this);
        overridePendingTransition(0, 0);

        mHandler = new Handler(getMainLooper());
        mContext = this;

        SharedPreferences.Editor editorctls = TemporaryData.getSharedPreferences().edit();
        editorctls.putInt(TemporaryData.CTLSDEMO, 0);
        editorctls.apply();


        SharedPreferences sharedPreferences2 = PreferenceManager.getDefaultSharedPreferences(this);

        SharedPreferences.Editor editor2 = sharedPreferences2.edit();
        editor2.putInt("UIFLAG", 0);
        editor2.apply();

        SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        uiFlag = sharedPreferences.getInt("UIFLAG", 0);

        Log.d("onCreate", uiFlag + "");

        // Show status bar
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.dmenu_activity_main);

        mPager = (ViewPager) findViewById(R.id.viewpager);
        mLlDot = (LinearLayout) findViewById(R.id.ll_dot);
        tvMsgLine01 = (TextView) findViewById(R.id.msg_text_01);
//        btnauto = (Button) findViewById(R.id.btn_auto);
        /*make it no sleep*/
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        SharedPreferences wmbPreference = PreferenceManager.getDefaultSharedPreferences(this);

        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_WIFI_STATE,
                                                             Manifest.permission.ACCESS_COARSE_LOCATION,
                                                             Manifest.permission.ACCESS_FINE_LOCATION},
                                                             PackageManager.PERMISSION_GRANTED);

        //boolean isFirstRun = wmbPreference.getBoolean("FIRSTRUN", true);

/*
        CtSystem system = new CtSystem();
        String setTimeZone = "Asia/Taipei";
        String server = "216.239.35.0";
        String port = "123";
        // setTimeZone
        Log.d(TAG, " return = " + Integer.toString(system.setTimeZone(setTimeZone)));
        // updataNTPSetting
        Log.d(TAG, " return = " + Integer.toString(system.updateNTPSetting(server, port, true, 1)));
*/

//        btnauto.setOnClickListener(this);

        //ctms prm file exist

        if (isCTMSParameterFileExist()) {
            Log.i(TAG, "reset FIRSTRUN: ");
            wmbPreference.edit().remove("FIRSTRUN").commit();
        }

        int firstInit = inCTOSS_Get1stInitFlag();
        int DLNotFinished = inCTOSS_GetDLNotFinishedFlag();
        Log.i(TAG, "-inCTOSS_Get1stInitFlag: " + firstInit + " DLNotFinished:" + DLNotFinished);
        boolean isFirstRun = wmbPreference.getBoolean("FIRSTRUN", true);
        Log.i(TAG, "CREDIT:isFirstRun =" + isFirstRun);

        CheckInstallFlagAndRunCTMS(wmbPreference);

        DLNotFinished = 0;
        if (DLNotFinished == 1) {
            //terminal need to restart after call ctms in this case
            ScheduledExecutorService scheduledExecutorService = new ScheduledThreadPoolExecutor(1);
            scheduledExecutorService.schedule(new Runnable() {
                @Override
                public void run() {
                    Log.i(TAG, "run: ctms call");
                    Intent intent = new Intent();
                    intent.putExtra("REBOOT", true);
                    intent.setClass(MainActivity.this, Activity_ctms_update.class);
                    startActivity(intent);
                    Log.i(TAG, "run: ctms done");
                }
            }, 1000 / 2, TimeUnit.MILLISECONDS);
            Log.i(TAG, "return oncreate: ");
            return;
        }

        if (firstInit == 1) {
            isFirstRun = true;
            Log.i(TAG, "set isFirstRun true,after init set it to false: ");
        }

        if (isFirstRun) {

            Log.i(TAG, "onCreate: if running here.....");
            jni_api.REG_CB_CTOS(jni_cb_ctos);
            jni_api_offline_pin.REG_CB_EMV(jni_cb_emv);

            View decorView = getWindow().getDecorView();
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);

            setContentView(R.layout.activity_disp);
            lsv_LCDTPrint = (ListView) findViewById(R.id.lsv_LCDTPrint);
            edtLog = (EditText) findViewById(R.id.edtLog);
            adapter = new ArrayAdapter(MainActivity.this, R.layout.listitem, list) {

                @NonNull
                @Override
                public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
                    View view = super.getView(position, convertView, parent);

                    TextView textView = (TextView) view;

                    if (addPadding[position] != 0)
                        textView.setPadding(36 * addPadding[position], 0, 0, 0);


                    if (displayMode[position].equals("0")) {
                        textView.setGravity(Gravity.LEFT);
                    } else if (displayMode[position].equals("1")) {
                        textView.setGravity(Gravity.CENTER);
                    } else if (displayMode[position].equals("2")) {
                        textView.setGravity(Gravity.RIGHT);
                    }


                    return view;

                }
            };


            {
                //setContentView(R.layout.activity_keypad);
                Log.i("Castles", "INIT");
                new Thread() {
                    @Override
                    public void run() {
                        try {
                            vdClearFile("/data/data/pub/TERMINAL.S3DB");  //for testing use only
                            vdClearFile("/data/data/pub/EMV.S3DB");
                            vdClearFile("/data/data/pub/ENV.S3DB");
                            vdClearFile("/data/data/com.Source.S1_BDO.BDO/S1_BDO.S3DB");

                            for (int x = 1; x <= 109; ++x) {
                                if (x <= 9) {
                                    String s = "/data/data/com.Source.S1_BDO.BDO/DYNAMICMENU0" + x + ".S3DB";
                                    vdClearFile(s);
                                } else {
                                    String s = "/data/data/com.Source.S1_BDO.BDO/DYNAMICMENU" + x + ".S3DB";
                                    vdClearFile(s);
                                }
                            }

                            // for quick menu, crash after ctms dl fix -- sidumili
                            String s = "/data/data/com.Source.S1_BDO.BDO/QUICKMENU" + ".S3DB";
                            vdClearFile(s);

                            //copy file or Dir
                            //LCDDisplay(" |Initializing...");
                            DisplayBox("Initializing||Please wait...|wait");
                            copyFileOrDir("");
                            Log.i("saturn", "saturn call inCTOSS_InitWaveData");
                            inCTOSS_InitWaveData();
                            inCTOSS_TerminalStartUp();

                            final Thread thread = new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    Message message = new Message();
                                    message.what = 1;
                                    Log.d("handler.sendMessage", "test");
                                    handler.sendMessageDelayed(message, 100);
                                }

                            });

                            thread.start();
                        } catch (Exception e) {

                        }
                    }
                }.start();
            }


            SharedPreferences.Editor editor = wmbPreference.edit();
            editor.putBoolean("FIRSTRUN", false);
            editor.commit();

        } else {
            Log.i(TAG, "onCreate: else running here.....");
            Log.i(TAG, "run: firstboot else");

            Boolean isECR = false;
            Intent intent1=getIntent();
            Log.i(TAG, "ECR->>intent1: " + intent1);
            if(intent1 != null) {
//                String strEcrCmd = null;
                final String strEcrCmd = intent1.getStringExtra(EcrCommandDefintion.ECR_REQ_CMD_NAME);
                //String strEcrCmd = intent1.getStringExtra(EcrCommandDefintion.ECR_REQ_CMD_NAME);
                Log.i(TAG, "strEcrCmd: " + strEcrCmd);

                if(strEcrCmd != null && !strEcrCmd.isEmpty()) {
                    isECR = true;
                    fBlockTrans = true;

                    try {
                        Log.i(TAG, "Start ECR Transaction.....");
                        DisplayBox("ECR Transaction||Please wait...|wait");
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    ScheduledExecutorService executorService = new ScheduledThreadPoolExecutor(1);
                    executorService.schedule(new Runnable() {
                        @Override
                        public void run() {
                            Log.i(TAG, "run: startEcrTxn");
                            Log.i(TAG, "strEcrCmd="+strEcrCmd);

                            startEcrTxn(strEcrCmd);
                            Log.i(TAG, "run: startEcrTxn done");
                        }
                    }, 1000/2, TimeUnit.MILLISECONDS);
                }
            }

            Log.i(TAG, "onCreate: isECR="+isECR);

            if (!isECR) {
                Log.i("Castles", "TerminalStartUp");
                //GetMenu();
                jni_api.REG_CB_CTOS(jni_cb_ctos);
                jni_api_offline_pin.REG_CB_EMV(jni_cb_emv);

                View decorView = getWindow().getDecorView();
                decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
                decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);

                setContentView(R.layout.activity_disp);
                lsv_LCDTPrint = (ListView) findViewById(R.id.lsv_LCDTPrint);
                edtLog = (EditText) findViewById(R.id.edtLog);
                adapter = new ArrayAdapter(MainActivity.this, R.layout.listitem, list) {

                    @NonNull
                    @Override
                    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
                        View view = super.getView(position, convertView, parent);

                        TextView textView = (TextView) view;

                        if (addPadding[position] != 0)
                            textView.setPadding(36 * addPadding[position], 0, 0, 0);

                        if (displayMode[position].equals("0")) {
                            textView.setGravity(Gravity.LEFT);
                        } else if (displayMode[position].equals("1")) {
                            textView.setGravity(Gravity.CENTER);
                        } else if (displayMode[position].equals("2")) {
                            textView.setGravity(Gravity.RIGHT);
                        }


                        return view;

                    }
                };

                // -----------------------------------------------------------------
                // run startup
                // -----------------------------------------------------------------
                new Thread() {
                    @Override
                    public void run() {
                        try {

                            Log.i(TAG, "Loading application.....");
                            DisplayBox("Loading application||Please wait...|wait");
                            inCTOSS_TerminalStartUp();

                            final Thread thread = new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    Message message = new Message();
                                    message.what = 1;
                                    Log.d("handler.sendMessage", "test");
                                    handler.sendMessageDelayed(message, inMessageDelay);
                                    //handler.sendMessage(message);
                                }

                            });

                            thread.start();

                        } catch (Exception e) {

                        }
                    }
                }.start();
                //what you do everytime goes here
            }
        }
        inLockScreen = inCTOSS_GetEnvInt("LOCKPROMPT");
    }

    /**
     * 初始化数据源
     */
    private void initDatas() {
        Log.i(TAG, "initDatas: run");

        InitMenuImageList(); // call to get title and images from TERMINAL.S3DB, IMG table -- sidumili

        mDatas = new ArrayList<Model>();
        for (int i = 0; i < titles.length; i++) {
            Log.i(TAG, "i="+i+",titles="+titles[i]+",images="+images[i]);

            //int imageId = getResources().getIdentifier("ic_category_" + i, "mipmap", getPackageName());
            int imageId = getResources().getIdentifier(images[i].replace(".png", ""), "mipmap", getPackageName());
            Log.i(TAG, "imageId="+imageId);

            if (imageId <= 0)
                imageId = getResources().getIdentifier("ic_blank", "mipmap", getPackageName());

            mDatas.add(new Model(titles[i], imageId));
        }
    }

    // dmenu start code

    /**
     * 设置圆点
     */
    public void setOvalLayout() {
        for (int i = 0; i < pageCount; i++) {
            mLlDot.addView(inflater.inflate(R.layout.dot, null));
        }
        // 默认显示第一页
        mLlDot.getChildAt(0).findViewById(R.id.v_dot)
                .setBackgroundResource(R.drawable.dot_selected);
        mPager.setOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            public void onPageSelected(int position) {
                // 取消圆点选中
                mLlDot.getChildAt(curIndex)
                        .findViewById(R.id.v_dot)
                        .setBackgroundResource(R.drawable.dot_normal);
                // 圆点选中
                mLlDot.getChildAt(position)
                        .findViewById(R.id.v_dot)
                        .setBackgroundResource(R.drawable.dot_selected);
                curIndex = position;
            }

            public void onPageScrolled(int arg0, float arg1, int arg2) {
            }

            public void onPageScrollStateChanged(int arg0) {
            }
        });
    }

    // dmenu end code
/*
    @Override
    protected void onDestroy() {
        super.onDestroy();
        employees.close();
        db.close();
    }
*/
    private void bindViews() {
        editdetail = (EditText) findViewById(R.id.editdetail);
        editname = (EditText) findViewById(R.id.editname);
        btnclean = (Button) findViewById(R.id.btnclean);
        btnsave = (Button) findViewById(R.id.btnsave);
        btnread = (Button) findViewById(R.id.btnread);

        btnclean.setOnClickListener(this);
        btnsave.setOnClickListener(this);
        btnread.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnclean:
                editdetail.setText("");
                editname.setText("");
                break;
            case R.id.btnsave:
                break;
            case R.id.btnread:
                break;
            case R.id.viewpager:
                if (ctlsdemo == 1) {
                    intransCount++;
                    //Toast.makeText(mContext, "Transaction Count "+intransCount, Toast.LENGTH_SHORT).show();
                    jni_api.REG_CB_CTOS(jni_cb_ctos);
                    setContentView(R.layout.activity_disp);
                    lsv_LCDTPrint = (ListView) findViewById(R.id.lsv_LCDTPrint);
                    edtLog = (EditText) findViewById(R.id.edtLog);
                    adapter = new ArrayAdapter(MainActivity.this, R.layout.listitem, list) {

                        @NonNull
                        @Override
                        public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
                            View view = super.getView(position, convertView, parent);


                            TextView textView = (TextView) view;

//                            Log.d("Position", String.valueOf(position));
//                            Log.d("Padding", String.valueOf(addPadding[position]));

                            if (addPadding[position] != 0)
                                textView.setPadding(36 * addPadding[position], 0, 0, 0);

//                            Log.d("DisplayMode", displayMode[position]);

                            if (displayMode[position].equals("0")) {
                                textView.setGravity(Gravity.LEFT);
                            } else if (displayMode[position].equals("1")) {
                                textView.setGravity(Gravity.CENTER);
                            } else if (displayMode[position].equals("2")) {
                                textView.setGravity(Gravity.RIGHT);
                            }


                            return view;

                        }
                    };
                    {
                        Log.i("Castles", "SALE AUTO");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_Sale(false);

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                }
        }
    }

    public void showMsg(final String msg) {
        runOnUiThread(new Runnable() {
                          @Override
                          public void run() {
                              edtLog.append(msg + '\n');
                          }
                      }
        );
    }

    public String checkAppRunning(String msg)
    {
        if(isApplicationStarted(this, msg))
            return "1";
        return "0";
    }

    public String ForkSubAP(String msg)
    {
        int inMsgCnt = 0;
        int inIdx = 0;
        String szSubAP = ".Main.MainActivity";

        Log.i("Castles", msg);

        try {
            if (isRun(this, msg)) {
                Log.i(TAG, msg + " app is running: ");

                Log.i(TAG, msg + " app is running END: ");
                return "OK";
            } else {
                Log.d(TAG, msg + " app is not running");
            }

            ComponentName apk2Component = new ComponentName(msg, msg+szSubAP);
            Log.i("Castles", msg+szSubAP);

            Intent intent = new Intent();
            Bundle bundle = new Bundle();

            bundle.putString("subAP", "1");
            bundle.putString("subAP1", "0");

            intent.putExtras(bundle);
            intent.setComponent(apk2Component);

            startActivity(intent);
        }catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }
        return "OK";
    }


   /* public String displayMultipleMsg(final String disp_msg)
    {
        //setContentView(R.layout.displaytextmessage);
       mHandler.post(new Runnable() {
           @Override
           public void run() {
               Toast.makeText(MainActivity.this, "test", Toast.LENGTH_SHORT).show();
               //tvMsgLine01.setTag(new Date().getTime());

               System.out.println("start displayMultipleMsg");
               System.out.println(disp_msg);


               String[] parsemsg = disp_msg.split("\\|");

               System.out.println("line:"+parsemsg[1]+"msg:"+parsemsg[2]+"mode:"+parsemsg[3]);

               int inmsgcnt = parsemsg.length;
               int line = Integer.parseInt(parsemsg[1]);
               String msg = parsemsg[2];
               int dispmode = Integer.parseInt(parsemsg[3]);

               System.out.println("Original Msg");
               System.out.println("line:"+parsemsg[1]+parsemsg[2]+parsemsg[3]);
               System.out.println("Convert Msg");
               System.out.println("line:"+line+msg+dispmode);

               System.out.println("line:"+line+msg+dispmode);

               System.out.printf("dispmode[%d]\n", dispmode);

               if(dispmode == 0)
                   inGravity = Gravity.LEFT;
               else if(dispmode == 1)
                   inGravity = Gravity.CENTER;
               else if(dispmode == 2)
                   inGravity = Gravity.RIGHT;

               System.out.printf("inGravity[%d]\n", inGravity);


               tvMsgLine01.setGravity(inGravity);
               tvMsgLine01.setText(parsemsg[2]);



               System.out.println("end displayMultipleMsg");
           }
       });




        return "OK";
    }*/


    public String displayMultipleMsg(final String disp_msg) {
        mHandler.post(new Runnable() {
            @Override
            public void run() {

                setContentView(R.layout.displaytextmessage);

                tvMsgLine01 = (TextView) findViewById(R.id.msg_text_01);
                tvMsgLine02 = (TextView) findViewById(R.id.msg_text_02);
                tvMsgLine03 = (TextView) findViewById(R.id.msg_text_03);
                tvMsgLine04 = (TextView) findViewById(R.id.msg_text_04);
                tvMsgLine05 = (TextView) findViewById(R.id.msg_text_05);
                tvMsgLine06 = (TextView) findViewById(R.id.msg_text_06);
                tvMsgLine07 = (TextView) findViewById(R.id.msg_text_07);
                tvMsgLine08 = (TextView) findViewById(R.id.msg_text_08);
                tvMsgLine09 = (TextView) findViewById(R.id.msg_text_09);
                tvMsgLine10 = (TextView) findViewById(R.id.msg_text_10);
                tvMsgLine11 = (TextView) findViewById(R.id.msg_text_11);
                tvMsgLine12 = (TextView) findViewById(R.id.msg_text_12);
                tvMsgLine13 = (TextView) findViewById(R.id.msg_text_13);
                tvMsgLine14 = (TextView) findViewById(R.id.msg_text_14);
                tvMsgLine15 = (TextView) findViewById(R.id.msg_text_15);
                tvMsgLine16 = (TextView) findViewById(R.id.msg_text_16);

                tvMsgLine01.setTag(new Date().getTime());

                System.out.println("start displayMultipleMsg");
                System.out.println(disp_msg);

                {
                    getMsg();

                    String[] parsemsg = disp_msg.split("\\|");

                    System.out.println("line:" + parsemsg[1] + "msg:" + parsemsg[2] + "mode:" + parsemsg[3]);

                    int inmsgcnt = parsemsg.length;
                    int line = Integer.parseInt(parsemsg[1]);
                    String msg = parsemsg[2];
                    int dispmode = Integer.parseInt(parsemsg[3]);

                    System.out.println("line:" + line + msg + dispmode);

                    if (dispmode == 0)
                        inGravity = Gravity.LEFT;
                    else if (dispmode == 1)
                        inGravity = Gravity.CENTER;
                    else if (dispmode == 2)
                        inGravity = Gravity.RIGHT;

                    System.out.printf("inGravity[%d]\n", inGravity);

                    switch (line) {
                        case 1:
                            tvMsgLine01.setGravity(inGravity);
                            tvMsgLine01.setText(parsemsg[2]);

                            break;
                        case 2:
                            tvMsgLine02.setGravity(inGravity);
                            tvMsgLine02.setText(parsemsg[2]);

                            break;
                        case 3:
                            tvMsgLine03.setGravity(inGravity);
                            tvMsgLine03.setText(parsemsg[2]);
                            break;
                        case 4:
                            tvMsgLine04.setGravity(inGravity);
                            tvMsgLine04.setText(parsemsg[2]);
                            break;
                        case 5:
                            tvMsgLine05.setGravity(inGravity);
                            tvMsgLine05.setText(parsemsg[2]);
                            break;
                        case 6:
                            tvMsgLine06.setGravity(inGravity);
                            tvMsgLine06.setText(parsemsg[2]);
                            break;
                        case 7:
                            tvMsgLine07.setGravity(inGravity);
                            tvMsgLine07.setText(parsemsg[2]);
                            break;
                        case 8:
                            tvMsgLine08.setGravity(inGravity);
                            tvMsgLine08.setText(parsemsg[2]);
                            break;
                        case 9:
                            tvMsgLine09.setGravity(inGravity);
                            tvMsgLine09.setText(parsemsg[2]);
                            break;
                        case 10:
                            tvMsgLine10.setGravity(inGravity);
                            tvMsgLine10.setText(parsemsg[2]);
                            break;
                        case 11:
                            tvMsgLine11.setGravity(inGravity);
                            tvMsgLine11.setText(parsemsg[2]);
                            break;
                        case 12:
                            tvMsgLine12.setGravity(inGravity);
                            tvMsgLine12.setText(parsemsg[2]);
                            break;
                        case 13:
                            tvMsgLine13.setGravity(inGravity);
                            tvMsgLine13.setText(parsemsg[2]);
                            break;
                        case 14:
                            tvMsgLine14.setGravity(inGravity);
                            tvMsgLine14.setText(parsemsg[2]);
                            break;
                        case 15:
                            tvMsgLine15.setGravity(inGravity);
                            tvMsgLine15.setText(parsemsg[2]);
                            break;
                        case 16:
                            tvMsgLine16.setGravity(inGravity);
                            tvMsgLine16.setText(parsemsg[2]);
                            break;
                    }
                }

                setMsg();

                System.out.println("end displayMultipleMsg");
            }
        });


        return "OK";
    }


    public String printbyXY(int x, int y, String disp_msg) {
        setContentView(R.layout.displaytextmessage);

        tvMsgLine01 = (TextView) findViewById(R.id.msg_text_01);
        tvMsgLine02 = (TextView) findViewById(R.id.msg_text_02);
        tvMsgLine03 = (TextView) findViewById(R.id.msg_text_03);
        tvMsgLine04 = (TextView) findViewById(R.id.msg_text_04);
        tvMsgLine05 = (TextView) findViewById(R.id.msg_text_05);
        tvMsgLine06 = (TextView) findViewById(R.id.msg_text_06);
        tvMsgLine07 = (TextView) findViewById(R.id.msg_text_07);
        tvMsgLine08 = (TextView) findViewById(R.id.msg_text_08);
        tvMsgLine09 = (TextView) findViewById(R.id.msg_text_09);
        tvMsgLine10 = (TextView) findViewById(R.id.msg_text_10);
        tvMsgLine11 = (TextView) findViewById(R.id.msg_text_11);
        tvMsgLine12 = (TextView) findViewById(R.id.msg_text_12);
        tvMsgLine13 = (TextView) findViewById(R.id.msg_text_13);
        tvMsgLine14 = (TextView) findViewById(R.id.msg_text_14);
        tvMsgLine15 = (TextView) findViewById(R.id.msg_text_15);
        tvMsgLine16 = (TextView) findViewById(R.id.msg_text_16);

        System.out.println("start printbyXY");
        System.out.println(disp_msg);

        inGravity = Gravity.NO_GRAVITY;
        x = x - 1;


        getMsg();
        System.out.printf("inGravity[%d]\n", inGravity);

        switch (y) {
            case 1:
                tvMsgLine01.setGravity(inGravity);
                tvMsgLine01.setPadding(36 * x, 0, 0, 0);
                tvMsgLine01.setText(disp_msg);
                inPaddingLeft[1] = 36 * x;

                break;
            case 2:
                tvMsgLine02.setGravity(inGravity);
                tvMsgLine02.setPadding(36 * x, 0, 0, 0);
                tvMsgLine02.setText(disp_msg);
                inPaddingLeft[2] = 36 * x;
                break;
            case 3:
                tvMsgLine03.setGravity(inGravity);
                tvMsgLine03.setPadding(36 * x, 0, 0, 0);
                tvMsgLine03.setText(disp_msg);
                inPaddingLeft[3] = 36 * x;
                break;
            case 4:
                tvMsgLine04.setGravity(inGravity);
                tvMsgLine04.setPadding(36 * x, 0, 0, 0);
                tvMsgLine04.setText(disp_msg);
                inPaddingLeft[4] = 36 * x;
                break;
            case 5:
                tvMsgLine05.setGravity(inGravity);
                tvMsgLine05.setPadding(36 * x, 0, 0, 0);
                tvMsgLine05.setText(disp_msg);
                inPaddingLeft[5] = 36 * x;
                break;
            case 6:
                tvMsgLine06.setGravity(inGravity);
                tvMsgLine06.setPadding(36 * x, 0, 0, 0);
                tvMsgLine06.setText(disp_msg);
                inPaddingLeft[6] = 36 * x;
                break;
            case 7:
                tvMsgLine07.setGravity(inGravity);
                tvMsgLine07.setPadding(36 * x, 0, 0, 0);
                tvMsgLine07.setText(disp_msg);
                inPaddingLeft[7] = 36 * x;
                break;
            case 8:
                tvMsgLine08.setGravity(inGravity);
                tvMsgLine08.setPadding(36 * x, 0, 0, 0);
                tvMsgLine08.setText(disp_msg);
                inPaddingLeft[8] = 36 * x;
                break;
            case 9:
                tvMsgLine09.setGravity(inGravity);
                tvMsgLine09.setPadding(36 * x, 0, 0, 0);
                tvMsgLine09.setText(disp_msg);
                inPaddingLeft[9] = 36 * x;
                break;
            case 10:
                tvMsgLine10.setGravity(inGravity);
                tvMsgLine10.setPadding(36 * x, 0, 0, 0);
                tvMsgLine10.setText(disp_msg);
                inPaddingLeft[10] = 36 * x;
                break;
            case 11:
                tvMsgLine11.setGravity(inGravity);
                tvMsgLine11.setPadding(36 * x, 0, 0, 0);
                tvMsgLine11.setText(disp_msg);
                inPaddingLeft[11] = 36 * x;
                break;
            case 12:
                tvMsgLine12.setGravity(inGravity);
                tvMsgLine12.setPadding(36 * x, 0, 0, 0);
                tvMsgLine12.setText(disp_msg);
                inPaddingLeft[12] = 36 * x;
                break;
            case 13:
                tvMsgLine13.setGravity(inGravity);
                tvMsgLine13.setPadding(36 * x, 0, 0, 0);
                tvMsgLine13.setText(disp_msg);
                inPaddingLeft[13] = 36 * x;
                break;
            case 14:
                tvMsgLine14.setGravity(inGravity);
                tvMsgLine14.setPadding(36 * x, 0, 0, 0);
                tvMsgLine14.setText(disp_msg);
                inPaddingLeft[14] = 36 * x;
                break;
            case 15:
                tvMsgLine15.setGravity(inGravity);
                tvMsgLine15.setPadding(36 * x, 0, 0, 0);
                tvMsgLine15.setText(disp_msg);
                inPaddingLeft[15] = 36 * x;
                break;
            case 16:
                tvMsgLine16.setGravity(inGravity);
                tvMsgLine16.setPadding(36 * x, 0, 0, 0);
                tvMsgLine16.setText(disp_msg);
                inPaddingLeft[16] = 36 * x;
                break;
        }

        setMsg();

        System.out.println("end displayMultipleMsg");
        return "DISP_MSG_OK";
    }

    public void GetMenu() {
        Log.i(TAG, "GetMenu: run");
        String AppDetail1 = "", AppDetail2 = "", TrxImageId, AppDetail, Networknm;
        String MainHeader;
        String[] AppHeader = new String[100];
        String rstatus = "0";
        String idleMode = "0";
        String sCommMode = "";
        String ssid = "";

        final PowerManager pm2 = (PowerManager) getSystemService(Context.POWER_SERVICE);

        Log.i(TAG, "GetMenu: run");
        setContentView(R.layout.dmenu_activity_main_2);

        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        // -----------------------------------------------------------------------
        // Set Header
        // -----------------------------------------------------------------------
        TextView tv_main_app_version;
        TextView tv_main_merchant;
        TextView tv_main_tprofile;
        TextView tv_main_NetworkCarrier;

        // Header Image
        ImageView img_transaction;
        ImageView img_history;
        ImageView img_setting;

        // Init Object
        tv_main_app_version = (TextView) findViewById(R.id.main_app_version);
        tv_main_merchant = (TextView) findViewById(R.id.main_merchant);
        tv_main_tprofile = (TextView) findViewById(R.id.main_tprofile);
        tv_main_NetworkCarrier = (TextView) findViewById(R.id.main_NetworkCarrier);

        // Init Header Image
        img_transaction = (ImageView) findViewById(R.id.img_transaction);
        img_history = (ImageView) findViewById(R.id.img_history);
        img_setting = (ImageView) findViewById(R.id.img_setting);
        img_signalstrength = (ImageView) findViewById(R.id.img_signalstrength);

        AppHeaderBuff = GetAppHeaderDetails(3); // Quick Menu -- sidumili
        Log.i(TAG, "GetMenu: GetAppHeaderDetails, AppHeaderBuff=" + AppHeaderBuff);
        MainHeader = AppHeaderBuff;
        Log.i("sidumili", AppHeaderBuff);
        AppHeader = MainHeader.split(" \n");
        int msgcnt = AppHeader.length;

        for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
            System.out.println("GetMenu::split msg [" + inIdx + "][" + AppHeader[inIdx] + "]");
            switch (inIdx) {
                case 0:
                    tv_main_app_version.setText(AppHeader[inIdx]);
                    break;
                case 1:
                    tv_main_merchant.setText(AppHeader[inIdx]);
                    break;
                case 2:
                    AppDetail1 = "MID: " + AppHeader[inIdx];
                    Log.i("TINE", AppDetail1);
                    break;
                case 3:
                    AppDetail2 = "TID: " + AppHeader[inIdx];
                    Log.i("TINE", AppDetail2);
                    break;
                case 4:
                    rstatus = AppHeader[inIdx];
                    break;
                case 5:
                    idleMode = AppHeader[inIdx];
                    break;
                case 6:
                    sCommMode = AppHeader[inIdx];
                    break;
                case 7: // Menu/Images
                    gMenuImageList = AppHeader[inIdx];
                    break;
            }
        }

        img_signalstrength.setBackgroundResource(R.drawable.bar0);

        Log.i(TAG, "GetMenu: gMenuImageList="+gMenuImageList);
        Log.i(TAG, "sCommMode=" + sCommMode);
        // TID / MID
        AppDetail = AppDetail1 + "  |  " + AppDetail2;
        tv_main_tprofile.setText(AppDetail);

        Networknm = "";
        boolean isValid = false;
        int signal = 0;
        //TelephonyManager telephonyManager;
        //OperatorHolder psListener;

        OperatorHolder operatorHolder = new OperatorHolder(this);
        Log.i(TAG, "getOperatorName=" + operatorHolder.getOperatorName());
        if (sCommMode.equals("GPRS")) {
            Networknm = operatorHolder.getOperatorName();
            //psListener = new OperatorHolder(this);
            //telephonyManager = (TelephonyManager)this.getSystemService(Context.TELEPHONY_SERVICE);
            //telephonyManager.listen(psListener, PhoneStateListener.LISTEN_SIGNAL_STRENGTHS);
            //signal = psListener.signalStrengthValue;
            //Log.i(TAG, "GetMenu,signal="+signal);

        } else if (sCommMode.equals("WIFI")) {
            WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            WifiInfo info = wifiManager.getConnectionInfo();
            if (info.getSupplicantState() == SupplicantState.COMPLETED) {
                ssid = info.getSSID().replace("\"", "");
            }
            Log.i(TAG, "wifi ssid=" + ssid);

            if (ssid.equals("<unknown ssid>") || (ssid.length() <= 0))
                Networknm = "Not connected";
            else
                Networknm = ssid;
        }

        Log.i(TAG, "GetMenu: Networknm="+Networknm);
        Log.i(TAG, "signal="+signal);
        tv_main_NetworkCarrier.setText(Networknm);

        // -----------------------------------------------------------------------
        // Set Header
        // -----------------------------------------------------------------------

        mPager = (ViewPager) findViewById(R.id.viewpager);

        //??????
        initDatas();
        inflater = LayoutInflater.from(MainActivity.this);
        //????=??/????,???
        pageCount = (int) Math.ceil(mDatas.size() * 1.0 / pageSize);
        mPagerList = new ArrayList<View>();
        for (int i = 0; i < pageCount; i++) {
            //??????inflate??????"
            GridView gridView = (GridView) inflater.inflate(R.layout.gridview, mPager, false);
            gridView.setAdapter(new GridViewAdapter(this, mDatas, i, pageSize, 0));
            mPagerList.add(gridView);

            gridView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    int pos = position + curIndex * pageSize;
                    Log.i(TAG, "onItemClick: Icon Name=" + mDatas.get(pos).getName());

                    jni_api.REG_CB_CTOS(jni_cb_ctos);
                    //setContentView(R.layout.activity_disp);
                    //lsv_LCDTPrint = (ListView) findViewById(R.id.lsv_LCDTPrint);
                    edtLog = (EditText) findViewById(R.id.edtLog);
                    adapter = new ArrayAdapter(MainActivity.this, R.layout.listitem, list) {

                        @NonNull
                        @Override
                        public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
                            View view = super.getView(position, convertView, parent);


                            TextView textView = (TextView) view;

                            if (addPadding[position] != 0)
                                textView.setPadding(36 * addPadding[position], 0, 0, 0);

                            if (displayMode[position].equals("0")) {
                                textView.setGravity(Gravity.LEFT);
                            } else if (displayMode[position].equals("1")) {
                                textView.setGravity(Gravity.CENTER);
                            } else if (displayMode[position].equals("2")) {
                                textView.setGravity(Gravity.RIGHT);
                            }


                            return view;

                        }
                    };

                    Log.i(TAG, "onItemClick: mDatas.get(pos).getName()="+mDatas.get(pos).getName());
                    if (mDatas.get(pos).getName().equals("SALE") || mDatas.get(pos).getName().equals("CARDS") || mDatas.get(pos).getName().equals("CARD SALE")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "CARD SALE");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_Sale(false);

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("INSTALLMENT")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "INSTALLMENT");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOS_INSTALLMENT();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("VOID")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "VOID");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_Void();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            // handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("BIN CHECK")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "BIN CHECK");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOS_BINCHECK();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("SETTLE")) {
                        Log.i("Castles", "SETTLE");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_Settle();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("LOGON")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "LOGON");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_LogOn();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("BALANCE INQUIRY") || mDatas.get(pos).getName().equals("BAL INQUIRY") || mDatas.get(pos).getName().equals("BAL INQ")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "BALANCE INQUIRY");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_Balance();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("SETTLEMENT")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "SETTLEMENT");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_Settle_Selection();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("BATCH TOTAL")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "BATCH TOTAL");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_Batch_Total();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("BATCH REVIEW") || mDatas.get(pos).getName().equals("REVIEW")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "BATCH REVIEW");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_Batch_Review();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("MOBILE WALLET")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "MOBILE WALLET");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOS_QRPay();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("RETRIEVE")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "RETRIEVE");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOS_Retrieve();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("BDO PAY")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "BDO PAY");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOS_BDOPayMenu();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("REPRINT")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "REPRINT");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_REPRINT();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("REPORTS") || mDatas.get(pos).getName().equals("REPORT")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "REPORTS");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_REPORTS();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("SETUP")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "SETUP");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_SETUP();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("MANAGEMENT")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "MANAGEMENT");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_MANAGEMENT();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("TIP ADJUST")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "TIP ADJUST");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_TIPADJUST();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                    if (mDatas.get(pos).getName().equals("CTMS UPDATE")) {
                        //setContentView(R.layout.activity_keypad);
                        Log.i("Castles", "CTMS UPDATE");
                        new Thread() {
                            @Override
                            public void run() {
                                try {
                                    inCTOSS_CTMSDownloadRequest();

                                    final Thread thread = new Thread(new Runnable() {

                                        @Override
                                        public void run() {
                                            Message message = new Message();
                                            message.what = 1;
                                            Log.d("handler.sendMessage", "test");
                                            handler.sendMessageDelayed(message, inMessageDelay);
                                            //handler.sendMessage(message);
                                        }

                                    });

                                    thread.start();
                                } catch (Exception e) {

                                }
                            }
                        }.start();
                    }

                }
            });

        }

        //?????
        mPager.setAdapter(new ViewPagerAdapter(mPagerList));
        //????
        setOvalLayout();

        img_transaction.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i(TAG, "onClick: TRANSACTIONS Image pressed");
                Log.i("Castles", "TRANSACTIONS");
                new Thread() {
                    @Override
                    public void run() {
                        try {

                            inCTOS_TransactionsMenu(1);

                            final Thread thread = new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    Message message = new Message();
                                    message.what = 1;
                                    Log.d("handler.sendMessage", "test");
                                    handler.sendMessageDelayed(message, inMessageDelay);
                                    //handler.sendMessage(message);
                                }

                            });

                            thread.start();
                        } catch (Exception e) {
                            Log.i(TAG, "Image transaction exception error " + e.getMessage());
                        }
                    }
                }.start();
            }
        });

        img_history.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i(TAG, "onClick: HISTORY Image pressed");
                Log.i("Castles", "HISTORY");
                new Thread() {
                    @Override
                    public void run() {

                        try {

                            inCTOS_HistoryMenu();

                            final Thread thread = new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    Message message = new Message();
                                    message.what = 1;
                                    Log.d("handler.sendMessage", "test");
                                    handler.sendMessageDelayed(message, inMessageDelay);
                                    //handler.sendMessage(message);
                                }

                            });

                            thread.start();
                        } catch (Exception e) {
                            Log.i(TAG, "Image history exception error " + e.getMessage());
                        }
                    }
                }.start();
            }
        });

        img_setting.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i(TAG, "onClick: SETUP Image pressed");
                Log.i("Castles", "SETUP");
                new Thread() {
                    @Override
                    public void run() {
                        try {
                            inCTOSS_SETUP();

                            final Thread thread = new Thread(new Runnable() {

                                @Override
                                public void run() {
                                    Message message = new Message();
                                    message.what = 1;
                                    Log.d("handler.sendMessage", "test");
                                    handler.sendMessageDelayed(message, inMessageDelay);
                                    //handler.sendMessage(message);
                                }

                            });

                            thread.start();
                        } catch (Exception e) {
                            Log.i(TAG, "Image setting exception error " + e.getMessage());
                        }
                    }
                }.start();
            }
        });

    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    public String displayImage(int x, int y, String msg_img) {
        setContentView(R.layout.displayimage);
        Log.i("Castles", "Start displayImage");
        Log.i("Castles", "x:" + x + "y:" + y + "name:" + msg_img);
        // System.out.println("Start displayImage");
        // System.out.println("x:"+x+"y:"+y+"name:"+msg_img);


        int resid = getResources().getIdentifier(msg_img, "drawable", getApplicationContext().getPackageName());

        imageView = (ImageView) findViewById(R.id.imageView);
        imageView.setImageResource(resid);

/*        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(400, 400);
        imageView.setLayoutParams(params);
        imageView.setBackgroundColor(getColor(R.color.colorAccent));*/
        ViewGroup.MarginLayoutParams margin = new ViewGroup.MarginLayoutParams(imageView.getLayoutParams());
        margin.leftMargin = x;
        margin.topMargin = y;
        RelativeLayout.LayoutParams layoutParams = new RelativeLayout.LayoutParams(margin);
//        layoutParams.height = 400;//set the height
//        layoutParams.width = 400; //set the width
        imageView.setLayoutParams(layoutParams);
        //imageView.setScaleType(ImageView.ScaleType.FIT_XY);

        Log.i("Castles", "End displayImage");

        //System.out.println("End displayImage");
        return "OK";
    }


    public void getMsg() {
        tvMsgLine01.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine01, null));
        tvMsgLine02.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine02, null));
        tvMsgLine03.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine03, null));
        tvMsgLine04.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine04, null));
        tvMsgLine05.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine05, null));
        tvMsgLine06.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine06, null));
        tvMsgLine07.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine07, null));
        tvMsgLine08.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine08, null));
        tvMsgLine09.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine09, null));
        tvMsgLine10.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine10, null));
        tvMsgLine11.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine11, null));
        tvMsgLine12.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine12, null));
        tvMsgLine13.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine13, null));
        tvMsgLine14.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine14, null));
        tvMsgLine15.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine15, null));
        tvMsgLine16.setText(TemporaryData.getSharedPreferences().getString(TemporaryData.DispLine16, null));

        tvMsgLine01.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity01, 0));
        tvMsgLine02.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity02, 0));
        tvMsgLine03.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity03, 0));
        tvMsgLine04.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity04, 0));
        tvMsgLine05.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity05, 0));
        tvMsgLine06.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity06, 0));
        tvMsgLine07.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity07, 0));
        tvMsgLine08.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity08, 0));
        tvMsgLine09.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity09, 0));
        tvMsgLine10.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity10, 0));
        tvMsgLine11.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity11, 0));
        tvMsgLine12.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity12, 0));
        tvMsgLine13.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity13, 0));
        tvMsgLine14.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity14, 0));
        tvMsgLine15.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity15, 0));
        tvMsgLine16.setGravity(TemporaryData.getSharedPreferences().getInt(TemporaryData.Gravity16, 0));

        tvMsgLine01.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft01, 0), 0, 0, 0);
        tvMsgLine02.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft02, 0), 0, 0, 0);
        tvMsgLine03.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft03, 0), 0, 0, 0);
        tvMsgLine04.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft04, 0), 0, 0, 0);
        tvMsgLine05.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft05, 0), 0, 0, 0);
        tvMsgLine06.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft06, 0), 0, 0, 0);
        tvMsgLine07.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft07, 0), 0, 0, 0);
        tvMsgLine08.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft08, 0), 0, 0, 0);
        tvMsgLine09.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft09, 0), 0, 0, 0);
        tvMsgLine10.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft10, 0), 0, 0, 0);
        tvMsgLine11.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft11, 0), 0, 0, 0);
        tvMsgLine12.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft12, 0), 0, 0, 0);
        tvMsgLine13.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft13, 0), 0, 0, 0);
        tvMsgLine14.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft14, 0), 0, 0, 0);
        tvMsgLine15.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft15, 0), 0, 0, 0);
        tvMsgLine16.setPadding(TemporaryData.getSharedPreferences().getInt(TemporaryData.PaddingLeft16, 0), 0, 0, 0);

    }

    public void setMsg() {

        SharedPreferences.Editor editor = TemporaryData.getSharedPreferences().edit();
        editor.putString(TemporaryData.DispLine01, tvMsgLine01.getText().toString());
        editor.putString(TemporaryData.DispLine02, tvMsgLine02.getText().toString());
        editor.putString(TemporaryData.DispLine03, tvMsgLine03.getText().toString());
        editor.putString(TemporaryData.DispLine04, tvMsgLine04.getText().toString());
        editor.putString(TemporaryData.DispLine05, tvMsgLine05.getText().toString());
        editor.putString(TemporaryData.DispLine06, tvMsgLine06.getText().toString());
        editor.putString(TemporaryData.DispLine07, tvMsgLine07.getText().toString());
        editor.putString(TemporaryData.DispLine08, tvMsgLine08.getText().toString());
        editor.putString(TemporaryData.DispLine09, tvMsgLine09.getText().toString());
        editor.putString(TemporaryData.DispLine10, tvMsgLine10.getText().toString());
        editor.putString(TemporaryData.DispLine11, tvMsgLine11.getText().toString());
        editor.putString(TemporaryData.DispLine12, tvMsgLine12.getText().toString());
        editor.putString(TemporaryData.DispLine13, tvMsgLine13.getText().toString());
        editor.putString(TemporaryData.DispLine14, tvMsgLine14.getText().toString());
        editor.putString(TemporaryData.DispLine15, tvMsgLine15.getText().toString());
        editor.putString(TemporaryData.DispLine16, tvMsgLine16.getText().toString());

        editor.putInt(TemporaryData.Gravity01, tvMsgLine01.getGravity());
        editor.putInt(TemporaryData.Gravity02, tvMsgLine02.getGravity());
        editor.putInt(TemporaryData.Gravity03, tvMsgLine03.getGravity());
        editor.putInt(TemporaryData.Gravity04, tvMsgLine04.getGravity());
        editor.putInt(TemporaryData.Gravity05, tvMsgLine05.getGravity());
        editor.putInt(TemporaryData.Gravity06, tvMsgLine06.getGravity());
        editor.putInt(TemporaryData.Gravity07, tvMsgLine07.getGravity());
        editor.putInt(TemporaryData.Gravity08, tvMsgLine08.getGravity());
        editor.putInt(TemporaryData.Gravity09, tvMsgLine09.getGravity());
        editor.putInt(TemporaryData.Gravity10, tvMsgLine10.getGravity());
        editor.putInt(TemporaryData.Gravity11, tvMsgLine11.getGravity());
        editor.putInt(TemporaryData.Gravity12, tvMsgLine12.getGravity());
        editor.putInt(TemporaryData.Gravity13, tvMsgLine13.getGravity());
        editor.putInt(TemporaryData.Gravity14, tvMsgLine14.getGravity());
        editor.putInt(TemporaryData.Gravity15, tvMsgLine15.getGravity());
        editor.putInt(TemporaryData.Gravity16, tvMsgLine16.getGravity());

        editor.putInt(TemporaryData.PaddingLeft01, inPaddingLeft[1]);
        editor.putInt(TemporaryData.PaddingLeft02, inPaddingLeft[2]);
        editor.putInt(TemporaryData.PaddingLeft03, inPaddingLeft[3]);
        editor.putInt(TemporaryData.PaddingLeft04, inPaddingLeft[4]);
        editor.putInt(TemporaryData.PaddingLeft05, inPaddingLeft[5]);
        editor.putInt(TemporaryData.PaddingLeft06, inPaddingLeft[6]);
        editor.putInt(TemporaryData.PaddingLeft07, inPaddingLeft[7]);
        editor.putInt(TemporaryData.PaddingLeft08, inPaddingLeft[8]);
        editor.putInt(TemporaryData.PaddingLeft09, inPaddingLeft[9]);
        editor.putInt(TemporaryData.PaddingLeft10, inPaddingLeft[10]);
        editor.putInt(TemporaryData.PaddingLeft11, inPaddingLeft[11]);
        editor.putInt(TemporaryData.PaddingLeft12, inPaddingLeft[12]);
        editor.putInt(TemporaryData.PaddingLeft13, inPaddingLeft[13]);
        editor.putInt(TemporaryData.PaddingLeft14, inPaddingLeft[14]);
        editor.putInt(TemporaryData.PaddingLeft15, inPaddingLeft[15]);
        editor.putInt(TemporaryData.PaddingLeft16, inPaddingLeft[16]);
        editor.apply();

    }


    /**
     * 为了得到传回的数据，必须在前面的Activity中（指MainActivity类）重写onActivityResult方法
     * <p>
     * requestCode 请求码，即调用startActivityForResult()传递过去的值
     * resultCode 结果码，结果码用于标识返回数据来自哪个新Activity
     */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        String result = data.getExtras().getString("result");//得到新Activity 关闭后返回的数据
        Log.i("onActivityResult", result);
    }

    public String GetAmountString(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    //intent.setClass(MainActivity.this, GetAmount.class);
                    intent.setClass(MainActivity.this, AmountEntryActivity.class);

                    //bundle.putString("amt_string", amt_string);
                    bundle.putString("pass_in_string", pass_in_string);
                    //bundle.putString("display", "AMOUNT  (PHP)");
                    bundle.putString("minlength", "1");
                    bundle.putString("maxlength", "11");
                    bundle.putString("type", "1");
                    //bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();


        /*
        Intent intent = new Intent();
        Bundle bundle = new Bundle();
        intent.setClass(MainActivity.this, GetAmount.class);

        bundle.putString("amt_string", amt_string);
        bundle.putString("amt_disp_msg", text);
        Log.i("amt_disp_msg", text);

        intent.putExtras(bundle);
        //startActivity(intent);
        startActivityForResult(intent, 0);

        Log.i("PATRICK", "startActivity");
        //finish();
        */


        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        //user_str = GetAmount.final_amt_string;
        user_str = AmountEntryActivity.final_amt_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH GetAmountString");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }


    public String GetAmountStringWithMenu(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    //intent.setClass(MainActivity.this, GetAmount.class);
                    intent.setClass(MainActivity.this, AmountEntryWithMenu.class);

                    //bundle.putString("amt_string", amt_string);
                    bundle.putString("pass_in_string", pass_in_string);
                    //bundle.putString("display", "AMOUNT  (PHP)");
                    bundle.putString("minlength", "1");
                    bundle.putString("maxlength", "11");
                    bundle.putString("type", "1");
                    //bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();


        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        //user_str = GetAmount.final_amt_string;
        user_str = AmountEntryWithMenu.final_amt_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH GetAmountStringWithMenu");
        //pin_num = pin_num + "XXX";

        //		  Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }


    public String GetPanString(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    //intent.setClass(MainActivity.this, GetAmount.class);
                    intent.setClass(MainActivity.this, GetCardOptionsActivity.class);

                    //bundle.putString("amt_string", amt_string);
                    bundle.putString("pass_in_string", pass_in_string);
                    //bundle.putString("display", "AMOUNT  (PHP)");
                    //bundle.putString("minlength", "1");
                    //bundle.putString("maxlength", "10");
                    //bundle.putString("type", "1");
                    //bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        //user_str = GetAmount.final_amt_string;
        user_str = GetCardOptionsActivity.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 456");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String UserInputString(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;


        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    //intent.setClass(MainActivity.this, InputString.class);
                    intent.setClass(MainActivity.this, UserInputString.class);


                    //bundle.putString("PARAM_DISPLAY", "PASSWORD");
                    //bundle.putString("PARAM_MINLENGTH", "1");
                    //bundle.putString("PARAM_MAXLENGTH", "6");
                    //bundle.putString("PARAM_TYPE", "1");
                    //bundle.putString("PARAM_PASSWORD", "123456");

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = UserInputString.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String InputStringUI(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;


        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, S1InputString.class);
                    //intent.setClass(MainActivity.this, UserInputString.class);


                    //bundle.putString("PARAM_DISPLAY", "PASSWORD");
                    //bundle.putString("PARAM_MINLENGTH", "1");
                    //bundle.putString("PARAM_MAXLENGTH", "6");
                    //bundle.putString("PARAM_TYPE", "1");
                    //bundle.putString("PARAM_PASSWORD", "123456");

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        Log.i(TAG, "InputStringUI, LOCK="+LOCK);
        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = S1InputString.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH S1InputString");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String InputStringAlpha(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;


        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, InputAlpha.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("saturn pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = InputAlpha.final_string;

        Log.i("TINE", user_str);
        Log.i("TINE", "FINISH InputStringAlpha");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }


    public String UserConfirmMenu(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, ConfirmMenu.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = ConfirmMenu.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String ConfirmYesNo(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, ConfirmYesNoUI.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = ConfirmYesNoUI.final_string;

        Log.i("TINE", user_str);
        Log.i("TINE", "FINISH ConfirmYesNo");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String BatchReviewUI(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, BatchReview.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = BatchReview.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String UserConfirmOKMenu(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, ConfirmOKMenu.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = ConfirmOKMenu.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String UserConfirmCard(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;
        cardentrytimer.cancel();

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, UserConfirmCard.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = UserConfirmCard.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH UserConfirmCard");
        return user_str;
    }

    public String ReceiptOnScreen(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, PrintReceiptonScreen.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = PrintReceiptonScreen.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH PrintReceiptonScreen");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }


    public String FirstReceiptOnScreen(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, PrintFirstReceiptonScreen.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = PrintFirstReceiptonScreen.final_string;

        Log.i("TINE", user_str);
        Log.i("TINE", "FINISH PrintFirstReceiptonScreen");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String EliteReceiptOnScreen(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, EliteReceiptonScreen.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = EliteReceiptonScreen.final_string;

        Log.i("TINE", user_str);
        Log.i("TINE", "FINISH EliteReceiptonScreen");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String UserCancelUI(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, UserCancelUI.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = UserCancelUI.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String PrintCopy(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, CopyMenu.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        //user_str = CopyMenu.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String getAnyNumStr(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, NormalKeyBoard.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();


        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "IPT_ERR";
                }
            }
        }

        user_str = NormalKeyBoard.user_num_str;
        //Log.i("PATRICK123456", pin_num);
        Log.i("PATRICK", "FINISH 777");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String DOptionMenuDisplay(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, DOptionMenu.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();


        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "IPT_ERR";
                }
            }
        }

        user_str = DOptionMenu.select_item;
        //Log.i("PATRICK123456", pin_num);
        Log.i("PATRICK", "FINISH 777");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String DPopupMenuDisplay(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, DPopupMenuActivity.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();


        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "IPT_ERR";
                }
            }
        }

        user_str = DPopupMenuActivity.select_item;
        //Log.i("PATRICK123456", pin_num);
        Log.i("PATRICK", "FINISH DPopupMenuActivity");
        //pin_num = pin_num + "XXX";
        //Toast.makeText(this, user_str, Toast.LENGTH_LONG).show();

        return user_str;

    }

    public String CTMSUPDATE(String text) throws InterruptedException {

        String user_str = "000";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    //intent.setClass(MainActivity.this, BatchReview.class);
                    intent.setClass(MainActivity.this, Activity_ctms_update.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        //user_str = "";

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "CTMS 789");
        //pin_num = pin_num + "XXX";
        //	Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String CTMSUPDATEBackground(String text) throws InterruptedException {

        String user_str = "000";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    //intent.setClass(MainActivity.this, BatchReview.class);
//                    intent.setClass(MainActivity.this, Activity_ctms_update.class);
                    intent.setClass(MainActivity.this, Activity_ctms_background.class);

                    //remark this line if no need to do check install result after boot up install done
                    bundle.putBoolean("CHECKINSTALL_FLAG", true);
                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        //user_str = "";

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "CTMS 789");
        //pin_num = pin_num + "XXX";
        //	Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String UserConfirmMenuInvandAmt(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    //intent.setClass(MainActivity.this, ConfirmMenu.class);
                    intent.setClass(MainActivity.this, UserConfirmVoid.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = UserConfirmVoid.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public static Bitmap drawTextToLeftTop(Context context, Bitmap bitmap, String text,

                                           int size, int color, int paddingLeft, int paddingTop) {

        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);

        paint.setColor(color);

        paint.setTextSize(dp2px(context, size));

        Rect bounds = new Rect();

        paint.getTextBounds(text, 0, text.length(), bounds);

        return drawTextToBitmap(context, bitmap, text, paint, bounds,

                dp2px(context, paddingLeft),

                dp2px(context, paddingTop) + bounds.height());

    }

    public static Bitmap drawTextToRightBottom(Context context, Bitmap bitmap, String text,

                                               int size, int color, int paddingRight, int paddingBottom) {

        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);

        paint.setColor(color);

        paint.setTextSize(dp2px(context, size));

        Rect bounds = new Rect();

        paint.getTextBounds(text, 0, text.length(), bounds);

        return drawTextToBitmap(context, bitmap, text, paint, bounds,

                bitmap.getWidth() - bounds.width() - dp2px(context, paddingRight),

                bitmap.getHeight() - dp2px(context, paddingBottom));

    }

    public static Bitmap drawTextToRightTop(Context context, Bitmap bitmap, String text,

                                            int size, int color, int paddingRight, int paddingTop) {

        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);

        paint.setColor(color);

        paint.setTextSize(dp2px(context, size));

        Rect bounds = new Rect();

        paint.getTextBounds(text, 0, text.length(), bounds);

        return drawTextToBitmap(context, bitmap, text, paint, bounds,

                bitmap.getWidth() - bounds.width() - dp2px(context, paddingRight),

                dp2px(context, paddingTop) + bounds.height());

    }

    public static Bitmap drawTextToLeftBottom(Context context, Bitmap bitmap, String text,

                                              int size, int color, int paddingLeft, int paddingBottom) {

        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);

        paint.setColor(color);

        paint.setTextSize(dp2px(context, size));

        Rect bounds = new Rect();

        paint.getTextBounds(text, 0, text.length(), bounds);

        return drawTextToBitmap(context, bitmap, text, paint, bounds,

                dp2px(context, paddingLeft),

                bitmap.getHeight() - dp2px(context, paddingBottom));

    }

    public static Bitmap drawTextToCenter(Context context, Bitmap bitmap, String text,

                                          int size, int color) {

        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);

        paint.setColor(color);

        paint.setTextSize(dp2px(context, size));

        Rect bounds = new Rect();

        paint.getTextBounds(text, 0, text.length(), bounds);

        return drawTextToBitmap(context, bitmap, text, paint, bounds,

                (bitmap.getWidth() - bounds.width()) / 2,

                (bitmap.getHeight() + bounds.height()) / 2);

    }

    private static Bitmap drawTextToBitmap(Context context, Bitmap bitmap, String text,

                                           Paint paint, Rect bounds, int paddingLeft, int paddingTop) {

        android.graphics.Bitmap.Config bitmapConfig = bitmap.getConfig();


        paint.setDither(true); // 获取跟清晰的图像采样

        paint.setFilterBitmap(true);// 过滤一些

        if (bitmapConfig == null) {

            bitmapConfig = android.graphics.Bitmap.Config.ARGB_8888;

        }

        bitmap = bitmap.copy(bitmapConfig, true);

        Canvas canvas = new Canvas(bitmap);


        canvas.drawText(text, paddingLeft, paddingTop, paint);

        return bitmap;

    }

    public static Bitmap scaleWithWH(Bitmap src, double w, double h) {

        if (w == 0 || h == 0 || src == null) {

            return src;

        } else {

            // 记录src的宽高

            int width = src.getWidth();

            int height = src.getHeight();

            // 创建一个matrix容器

            Matrix matrix = new Matrix();

            // 计算缩放比例

            float scaleWidth = (float) (w / width);

            float scaleHeight = (float) (h / height);

            // 开始缩放

            matrix.postScale(scaleWidth, scaleHeight);

            // 创建缩放后的图片

            return Bitmap.createBitmap(src, 0, 0, width, height, matrix, true);

        }

    }

    public static int dp2px(Context context, float dp) {

        final float scale = context.getResources().getDisplayMetrics().density;

        return (int) (dp * scale + 0.5f);

    }

    public ArrayList<Bitmap> getBitmapArrayByGif(int inResId) {
        try {
            ArrayList<Bitmap> BitmapList = new ArrayList<>();
            int i = 0;
            //GifDrawable gifDrawable = new GifDrawable(getResources(), R.drawable.cardidle);//加载一个动态图gif
            GifDrawable gifDrawable = new GifDrawable(getResources(), inResId);//加载一个动态图gif
            int totalCount = gifDrawable.getNumberOfFrames();
            for (i = 0; i < totalCount; i++) {
                BitmapList.add(gifDrawable.seekToFrameAndGet(i));
            }
            return BitmapList;
        } catch (Exception e) {
            return null;
        }
    }

    public int getResource(String imageName) {
        Context ctx = getBaseContext();
        int resId = getResources().getIdentifier(imageName, "drawable", ctx.getPackageName());
        return resId;
    }

    public String DisplayUI(String text) throws InterruptedException {

        String user_str = "";
        in_string = text;

        runOnUiThread(new Runnable() {
            @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN)
            @Override
            public void run() {
                setContentView(R.layout.display_cardidle);

                //Intent intent=getIntent();
                //String dispmsg=intent.getStringExtra("pass_in_string");
                //Log.i("dispmsg", dispmsg);

                String dispmsg = in_string;
                Log.i(TAG, dispmsg);

                // Show status bar
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);


                String[] dispmsginfo = dispmsg.split("\\|");
                int msgcnt = dispmsginfo.length;


                GifImageView gifImageView1 = (GifImageView) findViewById(R.id.gif1);
                try {// 如果加载的是gif动图，第一步需要先将gif动图资源转化为GifDrawable
                    // 将gif图资源转化为GifDrawable
                    ArrayList<Bitmap> BitmapList = new ArrayList<>();
                    ArrayList<Bitmap> BitmapListNew = new ArrayList<>();
                    ArrayList<Integer> Fontcolor = new ArrayList<>();
                    BitmapDrawable bd = null;

                    Fontcolor.add(Color.RED);
                    Fontcolor.add(Color.BLUE);
                    Fontcolor.add(Color.GREEN);
                    Fontcolor.add(Color.YELLOW);
                    Fontcolor.add(Color.WHITE);
                    Fontcolor.add(Color.BLACK);

                    Bitmap newbp = null;
                    Bitmap tempbp;
                    byte[] gifbyte = null;

                    AnimationDrawable animation = new AnimationDrawable();

                    int i = 0;

                    int ResId = getResource(dispmsginfo[0]);
                    BitmapList = getBitmapArrayByGif(ResId);

                    Log.i("BitmapList.size()", BitmapList.size() + "");

                    //String input = "TEST第一次";

                    for (i = 0; i < BitmapList.size(); i++) {
                        Log.i("BitmapList.size()", i + "");
                        //Thread.sleep(5000);
                        if (msgcnt == 2) {
                            newbp = BitmapList.get(i);
                        } else if (msgcnt == 3) {
                            newbp = drawTextToRightTop(getApplicationContext(), BitmapList.get(i), dispmsginfo[2], 16, Color.BLACK, 6, 6);
                        } else if (msgcnt == 4) {
                            tempbp = drawTextToRightTop(getApplicationContext(), BitmapList.get(i), dispmsginfo[2], 16, Color.BLACK, 6, 6);
                            newbp = drawTextToRightTop(getApplicationContext(), tempbp, dispmsginfo[3], 28, Color.BLACK, 10, 100);
                        } else if (msgcnt == 5) {
                            tempbp = drawTextToRightTop(getApplicationContext(), BitmapList.get(i), dispmsginfo[2], 16, Color.BLACK, 6, 6);
                            tempbp = drawTextToRightTop(getApplicationContext(), tempbp, dispmsginfo[3], 28, Color.BLACK, 10, 100);
                            newbp = drawTextToLeftTop(getApplicationContext(), tempbp, dispmsginfo[4], 28, Color.BLACK, 10, 100);
                        }
                        bd = new BitmapDrawable(getResources(), newbp);
                        animation.addFrame(bd, 200);

                    }
                    animation.setOneShot(false);


                    if (dispmsginfo[1].equals("front")) {
                        gifImageView1.setImageDrawable(animation);
                    } else {
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                            gifImageView1.setBackground(animation);
                        }
                    }
                    // start the animation!
                    animation.start();

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
        return user_str;
    }

    Bitmap myBitmap = null;
    ImageView imageView_receipt_bmp;
    File receipt_bmp_file = null;
    boolean isHide = false;

    public String LCDDisplay(String text) throws InterruptedException {

        Log.i(TAG, "LCDDisplay: run");

        String user_str = "";
        in_string = text;
        cardentrytimer.cancel();

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                overridePendingTransition(0, 0); // disable the animation, faster

                // Show status bar
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

                setContentView(R.layout.lcddisplay_message);

                //Intent intent=getIntent();
                //String dispmsg=intent.getStringExtra("pass_in_string");
                //Log.i("dispmsg", dispmsg);

                String dispmsg = in_string;
                Log.i(TAG, dispmsg);

                TextView textViewmsg;
                TextView textView_dtxn;
                TextView textView_cur;
                TextView textView_opt1;
                TextView textView_opt2;

                // For BMP Display
                ConstraintLayout linearLayout;
                LinearLayout linearLayout_bmp;

                // For Status Display
                LinearLayout linearLayout_status;
                TextView textView_status1;
                TextView textView_status2;
                ImageView imageView_status;

                LinearLayout lcd_header;
                LinearLayout lcd_txntitle;
                LinearLayout lcd_txn_cur;

                // Show status bar
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);


                String[] dispmsginfo = dispmsg.split("\\|");
                int msgcnt = dispmsginfo.length;

                textViewmsg = (TextView) findViewById(R.id.textView_msg);
                textView_dtxn = (TextView) findViewById(R.id.textView_dtxn);
                textView_cur = (TextView) findViewById(R.id.textView_cur);
                textView_opt1 = (TextView) findViewById(R.id.textView6);
                textView_opt2 = (TextView) findViewById(R.id.textView7);

                linearLayout = (ConstraintLayout) findViewById(R.id.linearLayout);
                linearLayout_bmp = (LinearLayout) findViewById(R.id.linearLayout_bmp);

                linearLayout_status = (LinearLayout) findViewById(R.id.linearLayout_status);

                lcd_header = (LinearLayout) findViewById(R.id.lcd_header);
                lcd_txntitle = (LinearLayout) findViewById(R.id.lcd_txntitle);
                lcd_txn_cur = (LinearLayout) findViewById(R.id.lcd_txn_cur);

                try {
                    //to do

                    for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
                        System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                        switch (inIdx) {
                            case 0:
                                textView_dtxn.setText(dispmsginfo[inIdx]);
                                break;
                            case 1:
                                textViewmsg.setText(dispmsginfo[inIdx]);
                                if (dispmsginfo[1].toString().equals("PRINTING...")) // Printing animation
                                {
                                    linearLayout.setVisibility(View.GONE);
                                    linearLayout_bmp.setVisibility(View.VISIBLE);

                                    receipt_bmp_file = new File("/data/data/pub/Print_BMP.bmp");

                                    myBitmap = BitmapFactory.decodeFile(receipt_bmp_file.getAbsolutePath());
                                    imageView_receipt_bmp = (ImageView) findViewById(R.id.receipt_bmp);
                                    imageView_receipt_bmp.setImageBitmap(myBitmap);

                                    Animation animation = AnimationUtils.loadAnimation(getApplicationContext(), R.anim.move_up);
                                    animation.setDuration(10000);
                                    imageView_receipt_bmp.startAnimation(animation);

                                }
                                //----------------------------------------------------------------------------------------------------------------------------
                                break;
                            case 2:
                                textView_cur.setText(dispmsginfo[inIdx]);
                                break;
                            case 3:
                                textView_opt1.setText(dispmsginfo[inIdx]);
                                break;
                            case 4:
                                textView_opt2.setText(dispmsginfo[inIdx]);
                                break;
                        }
                    }

                    // sidumili: Set visibility to GONE when passing no value. Fix on press cancel during printing preview
                    if (dispmsginfo[0].trim().length() <= 0 || dispmsginfo[1].trim().length() <= 0)
                        isHide = true;
                    else
                        isHide = false;

                    Log.i("sidumili", "run: isHide=" + isHide + ",msgcnt=" + msgcnt);
                    if (isHide) {
                        Log.i("sidumili", "run: Hide me now");
                        lcd_header.setVisibility(View.GONE);
                        lcd_txntitle.setVisibility(View.GONE);
                        lcd_txn_cur.setVisibility(View.GONE);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }

        });
        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();

        }
        user_str = "LCDDisplay_DONE";

        Log.i(TAG, "LCDDisplay: end");

        return user_str;
    }

    public void getTimerCancel() {cardentrytimer.cancel();}
    public void getTimerRestart() {cardentrytimer.start();}

    private CountDownTimer cardentrytimer = new CountDownTimer(inTimeOut*1000, 1000) {

        @Override
        public void onTick(long millisUntilFinished) {
            Log.i("Timer MainActivity", "Timer onTick");
        }

        @Override
        public void onFinish() {
            Log.i("Timer", "Timer onFinish");

            /*cancel timer first*/
            getTimerCancel();

            //etInputStr.clear();
            final_string = "TIME OUT";

            Log.i("PATRICK", "Timeout MainActivity");
            inCTOSS_CARDENTRYTIMEOUT();

            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }
    };


    public String usCARDENTRY(String text) throws InterruptedException {
        Log.i(TAG, "usCARDENTRY: run");

        String user_str = "";
        in_string = text;

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
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

                    setContentView(R.layout.cardentry);

                    //Intent intent=getIntent();
                    //String dispmsg=intent.getStringExtra("pass_in_string");
                    //Log.i("dispmsg", dispmsg);
                    //int inTimeOut = 30;

                    String man_entry_flag = "";
                    String dispmsg = in_string;
                    Log.i(TAG, dispmsg);

                    final TextView textViewmsg;
                    TextView textView_dtxn;
                    TextView textView_cur;
                    TextView textViewAmount;
                    TextView textView_header;
                    Button btn_can;
                    LinearLayout man_entry;
                    ConstraintLayout cardSale;
                    Button btn_back;

                    ImageView img_entry1;
                    ImageView img_entry2;
                    ImageView img_entry3;
                    ImageView img_entry4;

                    LinearLayout img_icon;

                    // Show status bar
                    getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);


                    String[] dispmsginfo = dispmsg.split("\\|");
                    int msgcnt = dispmsginfo.length;

                    textViewmsg = (TextView) findViewById(R.id.textView_msg);
                    textView_dtxn = (TextView) findViewById(R.id.textView_txn);
                    textView_cur = (TextView) findViewById(R.id.textView_cur);
                    textViewAmount = (TextView) findViewById(R.id.textViewAmount);
                    textView_header = (TextView) findViewById(R.id.textView_header);

                    btn_can = (Button) findViewById(R.id.btn_can);
                    man_entry = (LinearLayout) findViewById(R.id.manentry_layout);
                    btn_back = (Button) findViewById(R.id.btn_back);
                    img_icon = (LinearLayout) findViewById(R.id.ImageLinear);

                    img_entry1 = (ImageView) findViewById(R.id.img_entry1);
                    img_entry2 = (ImageView) findViewById(R.id.img_entry2);
                    img_entry3 = (ImageView) findViewById(R.id.img_entry3);
                    img_entry4 = (ImageView) findViewById(R.id.imageView4);

                    cardSale = (ConstraintLayout) findViewById(R.id.linearLayout);
                    ViewGroup.MarginLayoutParams marginlayout = (ViewGroup.MarginLayoutParams) cardSale.getLayoutParams();

                    //to do
                    for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
                        System.out.println("usCARDENTRY->split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                        switch (inIdx) {
                            case 0:
                                textView_dtxn.setText(dispmsginfo[inIdx]);
                                break;
                            case 1:
                                //textViewmsg.setText(dispmsginfo[inIdx]);
                                textView_header.setText(dispmsginfo[inIdx]);
                                break;
                            case 2:
                                textView_cur.setText(dispmsginfo[inIdx]);
                                break;
                            case 3:
                                textViewAmount.setText(dispmsginfo[inIdx]);
                                break;
                            case 4:
                                man_entry_flag = dispmsginfo[inIdx];
                                if (man_entry_flag.equals("0"))
                                    man_entry.setVisibility(View.GONE);
                                if (man_entry_flag.equals("1"))
                                    man_entry.setVisibility(View.VISIBLE);
                        }
                    }

                    // set image
                    img_entry1.setBackgroundResource(R.drawable.ic_insert);
                    img_entry2.setBackgroundResource(R.drawable.ic_swipe);
                    img_entry3.setBackgroundResource(R.drawable.supported_4);
                    img_entry4.setBackgroundResource(R.drawable.supported_5);

                    if (textView_dtxn.getText().toString().toUpperCase().equals("BALANCE INQUIRY") ||
                            textView_dtxn.getText().toString().toUpperCase().equals("CARD VERIFY") ||
                            textView_dtxn.getText().toString().toUpperCase().equals("COMPLETION"))
                    {

                        if (textView_dtxn.getText().toString().toUpperCase().equals("CARD VERIFY") ||
                                textView_dtxn.getText().toString().toUpperCase().equals("COMPLETION")) {
                            man_entry.setVisibility(View.VISIBLE);
                            img_entry4.setVisibility(View.VISIBLE);
                            img_entry1.setBackgroundResource(R.drawable.ic_insert);
                            img_entry2.setBackgroundResource(R.drawable.ic_swipe);
                            img_entry3.setBackgroundResource(R.drawable.supported_4b);
                            img_icon.getLayoutParams().height = 100;
                            img_icon.getLayoutParams().width = 600;
                        }
                        else {
                            img_entry1.setBackgroundResource(R.drawable.ic_tap);
                            img_entry2.setBackgroundResource(R.drawable.ic_insert);
                            man_entry.setVisibility(View.INVISIBLE);
                            img_entry4.setVisibility(View.INVISIBLE);
                            marginlayout.topMargin = 150;
                        }
                        textView_cur.setVisibility(View.INVISIBLE);
                        textViewAmount.setVisibility(View.INVISIBLE);
                    }
                    else
                    {
                        if (man_entry_flag.equals("0"))
                        {
                            man_entry.setVisibility(View.INVISIBLE);
                            img_entry4.setVisibility(View.INVISIBLE);

                            img_entry1.setBackgroundResource(R.drawable.ic_tap);
                            img_entry2.setBackgroundResource(R.drawable.ic_insert);
                            img_entry3.setBackgroundResource(R.drawable.supported_4);
                            marginlayout.topMargin = 150;
                            //img_entry4.setVisibility(View.GONE);
                            if (textView_dtxn.getText().toString().toUpperCase().equals("INSTALLMENT"))
                            {
                                img_entry1.setBackgroundResource(R.drawable.ic_insert);
                                img_entry2.setBackgroundResource(R.drawable.ic_swipe);
                                img_entry3.setBackgroundResource(R.drawable.supported_4b);
                                img_icon.getLayoutParams().height = 100;
                                img_icon.getLayoutParams().width = 650;
                            }

                        }
                        else
                        {
                            man_entry.setVisibility(View.VISIBLE);
                            img_entry4.setVisibility(View.VISIBLE);
                            if (textView_dtxn.getText().toString().toUpperCase().equals("INSTALLMENT"))
                            {
                                img_entry1.setBackgroundResource(R.drawable.ic_insert);
                                img_entry2.setBackgroundResource(R.drawable.ic_swipe);
                                img_entry3.setBackgroundResource(R.drawable.supported_4b);
                                img_icon.getLayoutParams().height = 100;
                                img_icon.getLayoutParams().width = 650;
                            }
                            else
                            {
                                img_entry1.setBackgroundResource(R.drawable.ic_tap);
                                img_entry2.setBackgroundResource(R.drawable.ic_insert);
                                img_entry3.setBackgroundResource(R.drawable.supported_4);
                            }
                           // img_entry4.setVisibility(View.GONE);

                        }
                    }

                    cardentrytimer.start();

                    btn_can.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            //startActivity(new Intent(CardEntry.this, MainActivity.class));
                            Log.i("PATRICK", "Cancel buOK");
                            cardentrytimer.cancel();
                            inCTOSS_BUTTONCANCEL();
                        }
                    });

                    man_entry.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            //startActivity(new Intent(CardEntry.this, MainActivity.class));
                            Log.i("saturn", "saturn Manual Entry");
                            cardentrytimer.cancel();
                            inCTOSS_MANUALENTRY();
                        }
                    });

                    btn_back.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            //startActivity(new Intent(CardEntry.this, MainActivity.class));
                            Log.i("PATRICK", "Back Button");
                            cardentrytimer.cancel();
                            inCTOSS_BUTTONCANCEL();
                        }
                    });

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

        });

        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();
        }

        return user_str;
    }


    public String GetSerialNumber(String text) throws InterruptedException {
        CtSystem system = new CtSystem();


        Log.i("saturn GetSerialNumber", OutputStr);
        // getFactorySN
        byte buf[] = new byte[17];

        try {
            buf = system.getFactorySN();
            Log.d(TAG, String.format("saturn FactorySN = %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", buf[0], buf[1], buf[2],
                    buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9],
                    buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]));

            OutputStr = String.format("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", buf[0], buf[1], buf[2],
                    buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9],
                    buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);

        } catch (CtSystemException e) {
            e.showStatus();
        }

        return OutputStr;

    }

    public String ePad_SignatureCaptureLibEex_Java(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, SignaturePadCastles.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "IPT_ERR";
                }
            }
        }

        Log.i("PATRICK", "FINISH 888");

        return user_str;
    }

    public String usCARDENTRY2_Java(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, CardEntry.class);

                    bundle.putString("pass_in_string", pass_in_string);

                    //String result = bundle.getString("result");
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                } catch (Exception e) {
                    //e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        Log.i("TINE", "FINISH usCARDENTRY_Java");
        user_str = CardEntry.cardentry_final_string;
        return user_str;

    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Log.i("TINE", "BackPressed MainActivity");

        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();
        }

    }

/*
	public String GetPinUI(String text) throws InterruptedException {
		
	in_string = text;
	final CtLed ctLed = new CtLed();
	ctLed.init(MainActivity.this);
		
		
	new Thread(new Runnable() {
	@Override
	public void run() {
	try {
		for (; ; ) {
			byte[] bytes = inCTOS_GetBuffer();
			ctLed.draw(bytes);
		}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	}).start();
		
		return in_string;
	}
*/

    //TINE: android
    public String GetExpiryDate(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, GetExpiryDateUI.class);
                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = GetExpiryDateUI.final_string;

        Log.i("TINE", user_str);
        Log.i("TINE", "FINISH GetExpiryDateUI");
        return user_str;

    }

    public String GetIPPPinEx(int KeySet, int KeyIndex, String pInData, int pinBypassAllow) throws InterruptedException {


        inKeySet = KeySet;
        inKeyIndex = KeyIndex;
        StrInData = pInData;
        inPinBypassAllow = pinBypassAllow;
        Enter_Press = 0;
        inKeyType = 3;


        Log.i("GetIPPPinEx", inKeySet + "");
        Log.i("GetIPPPinEx", inKeyIndex + "");
        Log.i("GetIPPPinEx", StrInData + "");
        Log.i("GetIPPPinEx", inPinBypassAllow + "");

        kmsui.init(MainActivity.this, inKeySet, inKeyIndex, StrInData, inKeyType, inPinBypassAllow);


        while (MainActivity.Enter_Press == 0) {
            //Log.d(TAG_A, MainActivity.Enter_Press+"");

            Thread.sleep(500); // a small delay to fix 2nd txn online PIN terminal hang issue. happen on S1F2.

        }

        Log.d("GetIPPPinEx", MainActivity.Enter_Press + "");


        Log.i("GetIPPPinEx", MainActivity.Enter_Press + "");
        OutputStr = String.valueOf(MainActivity.inRet) + '*' + sPinBlock + '|' + sKSN;

        Log.i("GetIPPPinEx", OutputStr);

        return OutputStr;

    }


    public String GetPIN_With_3DESDUKPTEx(int KeySet, int KeyIndex, String pInData, int pinBypassAllow) throws InterruptedException {


        inKeySet = KeySet;
        inKeyIndex = KeyIndex;
        StrInData = pInData;
        inPinBypassAllow = pinBypassAllow;
        Enter_Press = 0;
        inKeyType = 3;


        Log.i("GetPIN_With_3DESDUKPTEx", inKeySet + "");
        Log.i("GetPIN_With_3DESDUKPTEx", inKeyIndex + "");
        Log.i("GetPIN_With_3DESDUKPTEx", StrInData + "");

        kmsui.init(MainActivity.this, inKeySet, inKeyIndex, StrInData, inKeyType, inPinBypassAllow);


        while (MainActivity.Enter_Press == 0) {
            //Log.d(TAG_A, MainActivity.Enter_Press+"");

            Log.i("saturn", "saturn before delay");

            Thread.sleep(500); // a small delay to fix 2nd txn online PIN terminal hang issue. happen on S1F2.
        }

        Log.d("GetPIN_With_3DESDUKPTEx", MainActivity.Enter_Press + "");

        Log.i("GetPIN_With_3DESDUKPTEx", "sPinBlock:"+sPinBlock);
        Log.i("GetPIN_With_3DESDUKPTEx", "sKSN:"+sKSN);

        OutputStr = String.valueOf(MainActivity.inRet) + '*' + sPinBlock + '|' + sKSN;

        if(sKSN.equals("2911") && sPinBlock.isEmpty())
        {
            runOnUiThread(new Runnable() {
                public void run() {

                    Toast.makeText(getApplicationContext(), "PIN BYPASSED", Toast.LENGTH_LONG).show();
                    Log.i("GetPIN_With_3DESDUKPTEx", "PIN BYPASSED");
                }
            });
            //Toast.makeText(getApplicationContext(), "PIN BYPASSED", Toast.LENGTH_LONG).show();
        }

        Log.i("GetPIN_With_3DESDUKPTEx", OutputStr);

        return OutputStr;

    }

    public String usEditDatabase(String text) throws InterruptedException {

        Log.i("Castles", "Edit Table");
        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, EditDatabaseActivity.class);
                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = EditDatabaseActivity.final_string;

        Log.i("TINE", "FINISH EditDatabaseActivity " + user_str);

        //Toast.makeText(this, user_str, Toast.LENGTH_LONG).show();

        return user_str;

    }

    public String BackToProgress(String text) throws InterruptedException {

        in_string = text;

        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                jni_api.REG_CB_CTOS(jni_cb_ctos);
                setContentView(R.layout.activity_disp);
                lsv_LCDTPrint = (ListView) findViewById(R.id.lsv_LCDTPrint);
                edtLog = (EditText) findViewById(R.id.edtLog);
                adapter = new ArrayAdapter(MainActivity.this, R.layout.listitem, list) {

                    @NonNull
                    @Override
                    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
                        View view = super.getView(position, convertView, parent);

                        TextView textView = (TextView) view;

                        if (addPadding[position] != 0)
                            textView.setPadding(36 * addPadding[position], 0, 0, 0);


                        if (displayMode[position].equals("0")) {
                            textView.setGravity(Gravity.LEFT);
                        } else if (displayMode[position].equals("1")) {
                            textView.setGravity(Gravity.CENTER);
                        } else if (displayMode[position].equals("2")) {
                            textView.setGravity(Gravity.RIGHT);
                        }


                        return view;

                    }
                };
            }
        });

        return in_string;
    }

    public String GetWIFISettings(String text) {

        String user_str = "";
        GetNetworkInfor getNetworkInfor = new GetNetworkInfor(mContext);


        Log.i("saturn", "saturn GetWIFISettings");

        user_str = getNetworkInfor.GetIPSettings();

        Log.i("saturn", "saturn GetWIFISettings: " + user_str);

        return user_str;

    }


    public String fGetConnectStatus(String text) {

        String user_str = "";
        //in_string = text;
        ConnectivityManager cm = (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
        if (activeNetwork != null) {
            Log.i("saturn", "AAA activeNetwork is active");
            Log.i("saturn", "AAA text.equals " + text);
            // connected to the internet
            if (text.equals("4")) {
                if (activeNetwork.getType() == ConnectivityManager.TYPE_WIFI) {
                    // connected to wifi
                    Log.i("saturn", "AAA active getType is WIFI");
                    user_str = "YES";
                } else if (activeNetwork.getType() == ConnectivityManager.TYPE_MOBILE) {
                    // connected to mobile data
                    Log.i("saturn", "AAA active getType is mobile");
                    user_str = "FALLBACK";
                }
            }
            if (text.equals("2")) {
                if (activeNetwork.getType() == ConnectivityManager.TYPE_MOBILE)
                    user_str = "YES";
                else
                    user_str = "NO";
            }
        } else {
            // not connected to the internet
            Log.i("saturn", "AAA active getType is no connectivity");
            user_str = "NONETWORK";
        }

        return user_str;

    }

    public String UserConfirmDetails(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;
        cardentrytimer.cancel();

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, UserConfirmDetails.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = UserConfirmDetails.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH UserConfirmDetails");
        return user_str;
    }

    public String DisplayStatusBox(String text) throws InterruptedException {
        Log.i(TAG, "DisplayStatusBox: run");
        String user_str = "";
        in_string = text;
        if (cardentrytimer != null)
            cardentrytimer.cancel();

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                overridePendingTransition(0, 0); // disable the animation, faster

                //Intent intent=getIntent();
                //String dispmsg=intent.getStringExtra("pass_in_string");
                //Log.i("dispmsg", dispmsg);

                String dispmsg = in_string;
                Log.i(TAG, dispmsg);

                TextView box_msg;
                TextView box_msg2;
                TextView box_msg3;
                ImageView imageView;

                String[] dispmsginfo = dispmsg.split("\\|");
                int msgcnt = dispmsginfo.length;
                String image_str = "";

                // Show status bar
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
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
                setContentView(R.layout.displaystatusbox_message);

                imageView = (ImageView) findViewById(R.id.imageView);
                box_msg = (TextView) findViewById(R.id.box_msg);
                box_msg2 = (TextView) findViewById(R.id.box_msg2);
                box_msg3 = (TextView) findViewById(R.id.box_msg3);

                for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
                    System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                    switch (inIdx) {
                        case 0: // Message 1
                            box_msg = (TextView) findViewById(R.id.box_msg);
                            box_msg.setText(dispmsginfo[inIdx]);
                            break;
                        case 1: // Message 2
                            box_msg2 = (TextView) findViewById(R.id.box_msg2);
                            box_msg2.setText(dispmsginfo[inIdx]);
                            break;
                        case 2: // Image Icon
                            image_str = dispmsginfo[inIdx];
                            System.out.println("image_str [" + image_str + "]");
                            imageView = (ImageView) findViewById(R.id.imageView);
                            wub_lib.ViewImageResourcesByType(image_str, imageView); // sidumili: added to call function in any java, code optimization

                            if (!image_str.equals("none"))
                                wub_lib.ViewImageResourcesByType(image_str, imageView);
                            break;
                    }
                }

                if (image_str.equals("process") || image_str.equals("wait") || image_str.equals("init"))
                    AnimateProcessing();
                else
                    AnimationMessageStatusBox();
            }
        });

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        Log.i(TAG, "DisplayStatusBox: exit");
        return user_str;
    }

    public String DisplayBox(String text) throws InterruptedException {
        Log.i(TAG, "DisplayBox: run");
        final String[] user_str = {"CANCEL"};
        in_string = text;

        Log.i(TAG, "DisplayBox: run");
        Log.i(TAG, "cardentrytimer="+cardentrytimer);

        if (cardentrytimer != null)
            cardentrytimer.cancel();

        isExit = false;
        isWaitEvent = false;
        final_string = "CANCEL";
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                overridePendingTransition(0, 0); // disable the animation, faster

                //Intent intent=getIntent();
                //String dispmsg=intent.getStringExtra("pass_in_string");
                //Log.i("dispmsg", dispmsg);

                String dispmsg = in_string;
                Log.i(TAG, dispmsg);

                TextView box_msg = null;
                TextView box_msg2 = null;
                TextView box_msg3 = null;
                ImageView imageView;

                ConstraintLayout constraintLayout_main;
                LinearLayout linearLayout1;
                LinearLayout linearLayout2;

                String[] dispmsginfo = dispmsg.split("\\|");
                int msgcnt = dispmsginfo.length;
                String image_str = "";
                String rebootRCF = "";

                // Show status bar
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
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
                setContentView(R.layout.displaybox_message);

                // set onject
                constraintLayout_main = (ConstraintLayout) findViewById(R.id.constraintLayout_main);
                linearLayout1 = (LinearLayout) findViewById(R.id.linearLayout1);
                linearLayout2 = (LinearLayout) findViewById(R.id.linearLayout2);

                for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
                    System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                    switch (inIdx) {
                        case 0: // Message 1
                            box_msg = (TextView) findViewById(R.id.box_msg);
                            box_msg.setText(dispmsginfo[inIdx]);
                            break;
                        case 1: // Message 2
                            box_msg2 = (TextView) findViewById(R.id.box_msg2);
                            box_msg2.setText(dispmsginfo[inIdx]);
                            break;
                        case 2: // Message 3
                            box_msg3 = (TextView) findViewById(R.id.box_msg3);
                            box_msg3.setText(dispmsginfo[inIdx]);
                            break;
                        case 3: // Image Icon
                            image_str = dispmsginfo[inIdx];
                            System.out.println("image_str [" + image_str + "]");
                            imageView = (ImageView) findViewById(R.id.imageView);

                            if (!image_str.equals("none"))
                                wub_lib.ViewImageResourcesByType(image_str, imageView);

                            break;
                    }
                }

                if (image_str.equals("process") || image_str.equals("wait") || image_str.equals("init"))
                    AnimateProcessing();
                else
                    AnimationMessageStatusBox();

                // check if need an event - sidumili
                Log.i(TAG, "box_msg3.getText().toString()="+box_msg3.getText().toString());
                if (box_msg3.getText().toString().equals("Tap to continue"))
                {
                    Log.i(TAG, "pasok ka ba dini??");
                    isWaitEvent = true;
                }

                constraintLayout_main.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Log.i(TAG, "onClick: constraintLayout_main");
                        cardentrytimer.cancel();
                        final_string = "CONFIRM";
                        isExit = true;
                        WaitForEventLoop();
                    }
                });

                linearLayout1.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Log.i(TAG, "onClick: linearLayout1");
                        cardentrytimer.cancel();
                        final_string = "CONFIRM";
                        isExit = true;
                        WaitForEventLoop();
                    }
                });

                linearLayout2.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Log.i(TAG, "onClick: linearLayout2");
                        cardentrytimer.cancel();
                        final_string = "CONFIRM";
                        isExit = true;
                        WaitForEventLoop();
                    }
                });
            }
        });

        Log.i(TAG, "DisplayBox: end,final_string=" + final_string);

        return final_string;
    }

    public void WaitForEventLoop()
    {
        Log.i(TAG, "MainActivityLoop: run");
        Log.i(TAG, "isWaitEvent="+isWaitEvent);
        Log.i(TAG, "isExit="+isExit);
        Log.i(TAG, "final_string="+final_string);

        if (isWaitEvent)
        {
            do {
                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();
                }
            }while (!isExit);
        }
    }

    public void AnimationMessageBox() throws InterruptedException {
        TextView box_msg;
        TextView box_msg2;
        TextView box_msg3;
        ImageView imageView;

        // declare animation
        final Animation stb = AnimationUtils.loadAnimation(this, R.anim.stb);
        final Animation ttb = AnimationUtils.loadAnimation(this, R.anim.ttb);

        imageView = (ImageView) findViewById(R.id.imageView);
        box_msg = (TextView) findViewById(R.id.box_msg);
        box_msg2 = (TextView) findViewById(R.id.box_msg2);
        box_msg3 = (TextView) findViewById(R.id.box_msg3);

        // set animaiton
        imageView.startAnimation(stb);

        box_msg.startAnimation(ttb);
        box_msg2.startAnimation(ttb);
        box_msg3.startAnimation(ttb);
        //Thread.sleep(50);

        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();

        }

    }

    public void AnimationMessageStatusBox() {
        Log.i(TAG, "AnimationMessageStatusBox: run");
        TextView box_msg;
        TextView box_msg2;
        ImageView imageView;

        // declare animation
        final Animation stb = AnimationUtils.loadAnimation(this, R.anim.stb);
        final Animation ttb = AnimationUtils.loadAnimation(this, R.anim.ttb);

        imageView = (ImageView) findViewById(R.id.imageView);
        box_msg = (TextView) findViewById(R.id.box_msg);
        box_msg2 = (TextView) findViewById(R.id.box_msg2);

        // set animaiton
        imageView.startAnimation(stb);

        box_msg.startAnimation(ttb);
        box_msg2.startAnimation(ttb);

        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();

        }

    }

    public void AnimateProcessing() {
        Log.i(TAG, "AnimateProcessing: run");
        TextView box_msg;
        TextView box_msg2;
        ImageView imageView;

        // declare animation
        final Animation stb = AnimationUtils.loadAnimation(this, R.anim.clockwise);
        final Animation ttb = AnimationUtils.loadAnimation(this, R.anim.ttb);

        imageView = (ImageView) findViewById(R.id.imageView);
        box_msg = (TextView) findViewById(R.id.box_msg);
        box_msg2 = (TextView) findViewById(R.id.box_msg2);

        // set animaiton
        imageView.startAnimation(stb);

        box_msg.startAnimation(ttb);
        box_msg2.startAnimation(ttb);

        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();

        }
    }

    public void AnimateTimeout() throws InterruptedException {
        TextView box_msg;
        TextView box_msg2;
        TextView box_msg3;
        ImageView imageView;

        // declare animation
        final Animation stb = AnimationUtils.loadAnimation(this, R.anim.halfturn);
        final Animation ttb = AnimationUtils.loadAnimation(this, R.anim.ttb);

        imageView = (ImageView) findViewById(R.id.imageView);
        box_msg = (TextView) findViewById(R.id.box_msg);
        box_msg2 = (TextView) findViewById(R.id.box_msg2);
        box_msg3 = (TextView) findViewById(R.id.box_msg3);

        // set animaiton
        imageView.startAnimation(stb);

        box_msg.startAnimation(ttb);
        box_msg2.startAnimation(ttb);
        box_msg3.startAnimation(ttb);

        //Thread.sleep(50);

        synchronized (MainActivity.LOCK) {
            MainActivity.LOCK.setCondition(true);
            MainActivity.LOCK.notifyAll();

        }


    }


    private boolean isCTMSParameterFileExist() {
        String packageName = getPackageName();
        String packagePath = "/data/data/" + getPackageName() + "/files/" + packageName.toUpperCase() + ".prm";
        Log.i(TAG, "packagePath: " + packagePath);
        File file = new File(packagePath);
        if (file.exists()) {
            Log.i(TAG, "reset FIRSTRUN: ");
            return true;
        }
        return false;
    }

    // sidumili: added for edit host
    public String EditInfoListViewUI(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;
        Log.i("EditInfoListViewUI:", "1");
        new Thread() {
            public void run() {
                try {
                    EditListView.isResume = false;
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    Log.i("EditInfoListViewUI:", "2");
                    intent.setClass(MainActivity.this, EditListView.class);
                    Log.i("EditInfoListViewUI:", "3");
                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = EditListView.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH EditInfoListViewUI");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String UserConfirmMenu2(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, ConfirmMenu2.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = ConfirmMenu2.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String UserConfirmOKCancelMenu(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, ConfirmOKCancelMenu.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = ConfirmOKCancelMenu.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH UserConfirmOKCancelMenu");

        return user_str;
    }

    public String GetMOBILESettings(String text) {

        String user_str = "";
        String sInfoPart1 = "";
        String sInfoPart2 = "";
        String sMobileInfo = "";

        GetNetworkInfor getNetworkInfor = new GetNetworkInfor(mContext);
        Log.i("saturn", "saturn GetMOBILESettings");

        //sidumili: Get Terminal IP, Gateway, SubNetMask
        sInfoPart1 = getNetworkInfor.GetDeviceipMobileData();
        Log.i("saturn", "saturn GetDeviceipMobileData: sInfoPart1=" + sInfoPart1);

        //sidumili: Get DNS 1, DNS 2
        sInfoPart2 = getNetworkInfor.GetDNSServer(this);
        Log.i("saturn", "saturn GetDNSServer: sInfoPart2=" + sInfoPart2);

        sMobileInfo = sInfoPart1 + sInfoPart2;
        Log.i("saturn", "saturn GetMOBILESettings: sMobileInfo=" + sMobileInfo);

        user_str = sMobileInfo;

        return user_str;

    }

    public String ConfirmTotal(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, ConfirmTotal.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = ConfirmTotal.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String AppPackageInfo(String text) {

        int x = 0;
        String szCRC = "";
        String szAppCRC = "";
        szAppCRC = text;
        Log.i("TINE", "getAppInfor - START");
        Log.i("szAppCRC", "" + szAppCRC);
        GetAppInfor getAppInfor = new GetAppInfor(mContext);
        //String currentSignature = getAppInfor.getInstalledAppHashCode();
        String currentSignature = getAppInfor.getAppHashCode(szAppCRC);
        Log.i("currentSignature", "" + currentSignature);
        //Toast.makeText(getApplicationContext(), "currentSignature " + currentSignature, Toast.LENGTH_SHORT).show();
        x = 0;
        for (int i = 0; i < 9; i++) {
            szCRC = szCRC + currentSignature.substring(x, x + 1);
            x = x + 4;
        }

        Log.i("szCRC", "" + szCRC);
        return szCRC;
    }

    public static String final_string;

    public String DisplayApprovedUI(String text) throws InterruptedException {
        final String[] user_str = {"CANCEL"};
        in_string = text;

        if (cardentrytimer != null)
            cardentrytimer.cancel();

        final_string = "CANCEL";
        isExit = false;
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    overridePendingTransition(0, 0); // disable the animation, faster

                    String dispmsg = in_string;
                    Log.i(TAG, dispmsg);

                    TextView box_msg;
                    TextView box_msg1;
                    TextView box_msg2;
                    TextView box_msg3;
                    TextView box_msg4;
                    TextView box_msg5;
                    TextView box_msg6;
                    TextView box_msg7;
                    TextView box_msg8;

                    ImageView imageView;
                    TextView tvheader1;
                    TextView tvheader2;

                    LinearLayout linearLayout1;
                    LinearLayout linearLayout2;
                    LinearLayout linearLayout3;

                    TextView lbl_msg8;

                    String[] dispmsginfo = dispmsg.split("\\|");
                    int msgcnt = dispmsginfo.length;
                    String image_str = "";
                    String rebootRCF = "";

                    // Show status bar
                    getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
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
                    setContentView(R.layout.confirm_approved);

                    box_msg = (TextView) findViewById(R.id.box_msg);
                    box_msg1 = (TextView) findViewById(R.id.box_msg1);
                    box_msg2 = (TextView) findViewById(R.id.box_msg2);
                    box_msg3 = (TextView) findViewById(R.id.box_msg3);
                    box_msg4 = (TextView) findViewById(R.id.box_msg4);
                    box_msg5 = (TextView) findViewById(R.id.box_msg5);
                    box_msg6 = (TextView) findViewById(R.id.box_msg6);
                    box_msg7 = (TextView) findViewById(R.id.box_msg7);
                    box_msg8 = (TextView) findViewById(R.id.box_msg8);

                    imageView = (ImageView) findViewById(R.id.imageView);
                    tvheader1 = (TextView) findViewById(R.id.tvheader1);
                    tvheader2 = (TextView) findViewById(R.id.tvheader2);

                    linearLayout1 = (LinearLayout) findViewById(R.id.linearLayout1);
                    linearLayout2 = (LinearLayout) findViewById(R.id.linearLayout2);
                    linearLayout3 = (LinearLayout) findViewById(R.id.linearLayout3);

                    lbl_msg8 = (TextView) findViewById(R.id.lbl_msg8);

                    for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
                        System.out.println("DisplayApproved->split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                        switch (inIdx) {
                            case 0: // Header 1
                                tvheader1.setText(dispmsginfo[inIdx]);
                                break;
                            case 1: // Header 2
                                tvheader2.setText(dispmsginfo[inIdx]);
                                break;
                            case 2: // Type
                                wub_lib.ViewImageResourcesByType("success", imageView); // sidumili: added to call function in any java, code optimization
                                break;
                            case 3: // Message 1
                                box_msg.setText(dispmsginfo[inIdx]);
                                break;
                            case 4: // Message 2
                                box_msg1.setText(dispmsginfo[inIdx]);
                                break;
                            case 5: // Message 3
                                box_msg2.setText(dispmsginfo[inIdx]);
                                break;
                            case 6: // Message 4
                                box_msg3.setText(dispmsginfo[inIdx]);
                                break;
                            case 7: // Message 5
                                box_msg4.setText(dispmsginfo[inIdx]);
                                break;
                            case 8: // Message 6
                                box_msg5.setText(dispmsginfo[inIdx]);
                                break;
                            case 9: // Message 7
                                box_msg6.setText(dispmsginfo[inIdx]);
                                break;
                            case 10: // Message 8
                                if (dispmsginfo[inIdx].equals("Void") || dispmsginfo[inIdx].equals("VOID"))
                                {
                                    lbl_msg8.setText("Amount Voided");
                                    box_msg8.setTextColor(Color.parseColor("#FF0000"));
                                }
                                else
                                {
                                    lbl_msg8.setText("Amount");
                                    box_msg8.setTextColor(Color.parseColor("#00A995"));
                                }

                                box_msg7.setText(dispmsginfo[inIdx]);
                                break;
                            case 11: // Message 9
                                box_msg8.setText(dispmsginfo[inIdx]);
                                break;

                        }
                    }

                    linearLayout1.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            Log.i(TAG, "onClick: linearLayout1");
                            cardentrytimer.cancel();
                            final_string = "CONFIRM";
                            isExit = true;
                        }
                    });

                    linearLayout2.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            Log.i(TAG, "onClick: linearLayout2");
                            cardentrytimer.cancel();
                            final_string = "CONFIRM";
                            isExit = true;
                        }
                    });

                    linearLayout3.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            Log.i(TAG, "onClick: linearLayout3");
                            cardentrytimer.cancel();
                            final_string = "CONFIRM";
                            isExit = true;
                        }
                    });
                }
            });

            // wait for tap event -- sidumili
            do {
                // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
                synchronized (MainActivity.LOCK) {
                    MainActivity.LOCK.setCondition(true);
                    MainActivity.LOCK.notifyAll();
                }
            }while (!isExit);

        Log.i(TAG, "DisplayApprovedUI: final_string=" + final_string);

        return final_string;
    }

    public String PromptMessageUI(String text) throws InterruptedException {

        String user_str = "";
        in_string = text;
        if (cardentrytimer != null)
            cardentrytimer.cancel();

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                overridePendingTransition(0, 0); // disable the animation, faster

                String dispmsg = in_string;
                Log.i(TAG, dispmsg);

                ImageView imageView;
                TextView tvmessage;
                TextView tvmessage1;
                TextView tvmessage2;

                String[] dispmsginfo = dispmsg.split("\\|");
                int msgcnt = dispmsginfo.length;
                String image_str = "";
                String rebootRCF = "";

                // Show status bar
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
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
                setContentView(R.layout.prompt_message);

                for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
                    System.out.println("split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                    switch (inIdx) {
                        case 0: // Message 1
                            tvmessage = (TextView) findViewById(R.id.tvmessage);
                            tvmessage.setText(dispmsginfo[inIdx]);
                            break;
                        case 1: // Message 2
                            tvmessage1 = (TextView) findViewById(R.id.tvmessage1);
                            tvmessage1.setText(dispmsginfo[inIdx]);
                            break;
                        case 2: // Message 3
                            tvmessage2 = (TextView) findViewById(R.id.tvmessage2);
                            tvmessage2.setText(dispmsginfo[inIdx]);
                            break;
                        case 3: // Image Icon
                            image_str = dispmsginfo[inIdx];
                            System.out.println("image_str [" + image_str + "]");
                            imageView = (ImageView) findViewById(R.id.imageView);
                            wub_lib.ViewImageResourcesByType(image_str, imageView); // sidumili: added to call function in any java, code optimization
                            break;
                    }
                }
            }
        });

        return user_str;
    }

    public String MenuSelection(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {

                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, MenuSelection.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = MenuSelection.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH MenuSelection");

        return user_str;
    }

    public String TransDetailListViewUI(String text) throws InterruptedException {

        String user_str="";
        pass_in_string = text;
        Log.i("TransDetailListViewUI:", "1");
        new Thread()
        {
            public void run()
            {
                try
                {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    Log.i("TransDetailListViewUI:", "2");
                    intent.setClass(MainActivity.this, TransDetailListView.class);
                    Log.i("TransDetailListViewUI:", "3");
                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                }
                catch(Exception e)
                {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str =  TransDetailListView.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH TransDetailListViewUI");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    public String ConfirmInputUI(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();

                    intent.setClass(MainActivity.this, ConfirmInput.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = ConfirmInput.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH ConfirmInputUI");

        return user_str;
    }

    public String SingleRecordDataListViewUI(String text) throws InterruptedException {

        String user_str="";
        pass_in_string = text;
        new Thread()
        {
            public void run()
            {
                try
                {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, SingleDataRecordListView.class);
                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                }
                catch(Exception e)
                {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str =  SingleDataRecordListView.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH SingleRecordDataListViewUI");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;
    }

    // init array for titles/images -- sidumili
    private void InitMenuImageList()
    {
        Log.i(TAG, "InitMenuImageList: run");
        Log.i(TAG, "gMenuImageList="+gMenuImageList);

        ArrayList<String> listsTitle = new ArrayList<String>();
        ArrayList<String> listsImage = new ArrayList<String>();
        
        String[] pArrList = gMenuImageList.split("\\|");
        int pLen = pArrList.length;
        System.out.println("pLen [" + pLen + "]");

        titles = null;
        images = null;
        for (int i = 0; i < pLen; i++)
        {
            Log.i(TAG, "For Loop->i="+i+",Len="+pArrList[i].length());
            if (pArrList[i].length() > 0)
            {
                System.out.println("split msg [" + i + "][" + pArrList[i] + "]");
                String[] pArrParse = pArrList[i].split("\\^");

                if (pArrParse[0].length() > 0 && pArrParse[1].length() > 0)
                {
                    String pTitle = pArrParse[0];
                    String pImage = pArrParse[1];

                    Log.i(TAG, "i="+i+",pTitle="+pTitle+",pImage="+pImage);

                    listsTitle.add(pTitle);
                    listsImage.add(pImage);
                }
            }
        }

        titles = listsTitle.toArray(new String[listsTitle.size()]);
        images = listsImage.toArray(new String[listsImage.size()]);

        System.out.println("titles="+ Arrays.toString(titles));
        System.out.println("images="+ Arrays.toString(images));
    }

    public void startEcrTxn(String txnType) {
        Log.i(TAG, "****startEcrTxn: ****" + txnType);
        //scheduleTaskStop();
        //onIdlePause();
        //To test memory leak, no logic in C
        //fBlockTrans = true;

        if(false) {
            Log.i(TAG, "ignore ecr command test: ");
            wakeUpScreen();
            //setupMyViews("Credit Sale");
            Log.i("Castles", "SALE done");
            final Thread thread = new Thread(new Runnable() {
                @Override
                public void run() {
                    Log.i(TAG, "run txn: ");
                    jni_api.REG_CB_CTOS(jni_cb_ctos);
                    jni_api_offline_pin.REG_CB_EMV(jni_cb_emv);
                    inCTOSS_Sale(true);
                    Message message = new Message();
                    message.what = 1;
                    Log.d("handler.sendMessage", "sale");
                    handler.sendMessageDelayed(message, inMessageDelay);
                }

            });

            thread.start();
            return;
        }

        if(txnType.equals(EcrCommandDefintion.ECR_REQ_CMD_SALE))
        {
            Log.i(TAG, "process ecr command: ");
            wakeUpScreen();
            //setupMyViews("Credit Sale");

            Log.i(TAG, "****Credit Sale: ****");
            new Thread() {
                @Override
                public void run() {
                    try {
                        fSTART = true;
                        GetMemoryInfor.displayBriefMemory(mContext);
                        jni_api.REG_CB_CTOS(jni_cb_ctos);
                        jni_api_offline_pin.REG_CB_EMV(jni_cb_emv);
                        inCTOSS_ECR_Menu(1);
                        Log.i("Castles", "ECR SALE done");
                        fSTART = false;

                        EcrCommandReceiver.isEcrProcessing=false;
                    } catch (Exception e) {

                    }
                }
            }.start();
        }
        else if (txnType.equals(EcrCommandDefintion.ECR_REQ_CMD_VOID)){
            Log.i("Castles", "Void");
            wakeUpScreen();
            //setupMyViews("Void");
            new Thread() {
                @Override
                public void run() {
                    try {
                        fSTART = true;
                        jni_api.REG_CB_CTOS(jni_cb_ctos);
                        // patrick fix offline pin crash issue 20210123
                        jni_api_offline_pin.REG_CB_EMV(jni_cb_emv);
                        //inCTOSS_Void(true);
                        inCTOSS_ECR_Menu(2);
                        fSTART = false;

                        Log.i("Castles", "ECR Void done");

                        EcrCommandReceiver.isEcrProcessing=false;
                      /* final Thread thread = new Thread(new Runnable(){

                            @Override
                            public void run() {
                                Message message = new Message();
                                message.what = 1;
                                Log.d("handler.sendMessage",  "test");
                                handler.sendMessageDelayed(message, 100);
                            }

                        });

                        thread.start(); */
                    } catch (Exception e) {

                    }
                }
            }.start();

        }
        Log.i(TAG, "startEcrTxn: exit");
    }

    private void wakeUpScreen() {
        PowerManager pm = (PowerManager) this.getSystemService(Context.POWER_SERVICE);
        boolean screenOn = pm.isScreenOn();
        Log.i(TAG, "screenOn: " + screenOn);
        if(!screenOn) {
            // 获取PowerManager.WakeLock对象,后面的参数|表示同时传入两个值,最后的是LogCat里用的Tag
            @SuppressLint("InvalidWakeLockTag") PowerManager.WakeLock wl = pm.newWakeLock(PowerManager.ACQUIRE_CAUSES_WAKEUP | PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "bright");
            wl.acquire(); // 点亮屏幕
            wl.release(); // 释放

        }
    }

    public void JNIGetMenu(){
        Log.i(TAG, "JNIGetMenu: run");
        final Thread thread = new Thread(new Runnable() {

            @Override
            public void run() {

                idlecardentry_status= 100;
                if(false)//To handle ecr indenpendly as sub app
                {
                    Log.i(TAG, "not main app, so run ecr MainActivity finish: ");
                    MainActivity.this.finish();
                    int pid = Process.myPid();
                    Log.i(TAG, "Process pid: " + pid);
//                                    Process.killProcess(pid);
                    System.exit(0);
                }
                else
                {
                    //executeLockScreen(); // lock screen -- sidumili

                    Message message = new Message();
                    message.what = 1;
                    Log.d("handler.sendMessage", "sale");
                    handler.sendMessageDelayed(message, inMessageDelay);
                }

            }

        });

        thread.start();

        // Message message = new Message();
        //  message.what = 1;
        // Log.d("handler.sendMessage", "sale");
        // handler.sendMessageDelayed(message, 100);
    }

    /**
     * To check whether app is running or not
     * @param context
     * @param PackageName, eg:com.persistent.app
     * @return
     */
    public boolean isApplicationStarted(Context context,String PackageName) {
        //<uses-permission android:name="android.permission.GET_TASKS"/>
        boolean isStarted =false;
        Log.i(TAG, "isApplicationStarted: " + PackageName);
        try {
            ActivityManager mActivityManager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);
            int intGetTastCounter = 1000;
            List<ActivityManager.RunningServiceInfo> mRunningService = mActivityManager.getRunningServices(intGetTastCounter );
            for (ActivityManager.RunningServiceInfo amService : mRunningService) {
                if(0 == amService.service.getPackageName().compareTo(PackageName)) {
                    isStarted = true;
                    break;
                }
            }
        }
        catch(SecurityException e) {
            e.printStackTrace();
        }
        Log.i(TAG, "isStarted: " + isStarted);
        return isStarted;
    }

    public boolean isRun(Context context,String PackageName) {
        ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningTaskInfo> list = am.getRunningTasks(MAX_VALUE);
        boolean isAppRunning = false;
        String MY_PKG_NAME = PackageName;
        Log.i(TAG, "isRun: " + MY_PKG_NAME);

        final List<ActivityManager.RunningServiceInfo> services = am.getRunningServices(Integer.MAX_VALUE);
        for (ActivityManager.RunningServiceInfo runningServiceInfo : services) {
            // Log.d(TAG, String.format("Service:%s", runningServiceInfo.service.getClassName()));
            if (runningServiceInfo.service.getClassName().contains(MY_PKG_NAME)){
                isAppRunning = true;
                Log.i("ActivityService isRun()", runningServiceInfo.service.getClassName());
                break;
            }
        }

        Log.i("ActivityService isRun()", "com.ad ...isAppRunning..... " + isAppRunning);
        if (isAppRunning == true)
            return isAppRunning;

        MY_PKG_NAME = PackageName;
        Log.i(TAG, "isRun: " + MY_PKG_NAME);

        for (ActivityManager.RunningTaskInfo info : list) {
            // Log.d(TAG, String.format("Activity:%s", info.topActivity.getPackageName()));
            // Log.d(TAG, String.format("Activity:%s", info.baseActivity.getPackageName()));
            if (info.topActivity.getPackageName().equals(MY_PKG_NAME) || info.baseActivity.getPackageName().equals(MY_PKG_NAME)) {
                isAppRunning = true;
                Log.i("Activity isRun()", info.topActivity.getPackageName() + " info.baseActivity.getPackageName()=" + info.baseActivity.getPackageName());
                break;
            }
        }

        return isAppRunning;
    }

    public String UserConfirmTipAdjust(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    //intent.setClass(MainActivity.this, ConfirmMenu.class);
                    intent.setClass(MainActivity.this, UserConfirmTipAdjust.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = UserConfirmTipAdjust.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    // display for tip adjust approved ui -- sidumili
    public String DisplayTipAdjustApprovedUI(String text) throws InterruptedException {
        final String[] user_str = {"CANCEL"};
        in_string = text;

        if (cardentrytimer != null)
            cardentrytimer.cancel();

        final_string = "CANCEL";
        isExit = false;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                overridePendingTransition(0, 0); // disable the animation, faster

                String dispmsg = in_string;
                Log.i(TAG, dispmsg);

                TextView box_msg;
                TextView box_msg1;
                TextView box_msg2;

                ImageView imageView;
                TextView tvheader1;
                TextView tvheader2;

                LinearLayout linearLayout1;
                LinearLayout linearLayout2;
                LinearLayout linearLayout3;

                String[] dispmsginfo = dispmsg.split("\\|");
                int msgcnt = dispmsginfo.length;
                String image_str = "";
                String rebootRCF = "";

                // Show status bar
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
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
                setContentView(R.layout.confirm_tip_adjust_approved);

                box_msg = (TextView) findViewById(R.id.box_msg);
                box_msg1 = (TextView) findViewById(R.id.box_msg1);
                box_msg2 = (TextView) findViewById(R.id.box_msg2);

                imageView = (ImageView) findViewById(R.id.imageView);
                tvheader1 = (TextView) findViewById(R.id.tvheader1);
                tvheader2 = (TextView) findViewById(R.id.tvheader2);

                linearLayout1 = (LinearLayout) findViewById(R.id.linearLayout1);
                linearLayout2 = (LinearLayout) findViewById(R.id.linearLayout2);
                linearLayout3 = (LinearLayout) findViewById(R.id.linearLayout3);

                wub_lib.ViewImageResourcesByType("success", imageView); // sidumili: added to call function in any java, code optimization
                tvheader1.setText("Tip adjusted successfully.");
                tvheader2.setText("Tap to continue");

                for (int inIdx = 0; inIdx < msgcnt; inIdx++) {
                    System.out.println("DisplayTipAdjustApprovedUI->split msg [" + inIdx + "][" + dispmsginfo[inIdx] + "]");
                    switch (inIdx) {
                        case 0: // Message 1
                            box_msg.setText(dispmsginfo[inIdx]);
                            break;
                        case 1: // Message 2
                            box_msg1.setText(dispmsginfo[inIdx]);
                            break;
                        case 2: // Message 3
                            box_msg2.setText(dispmsginfo[inIdx]);
                            break;
                    }
                }

                linearLayout1.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Log.i(TAG, "onClick: linearLayout1");
                        cardentrytimer.cancel();
                        final_string = "CONFIRM";
                        isExit = true;
                    }
                });

                linearLayout2.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Log.i(TAG, "onClick: linearLayout2");
                        cardentrytimer.cancel();
                        final_string = "CONFIRM";
                        isExit = true;
                    }
                });

                linearLayout3.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Log.i(TAG, "onClick: linearLayout3");
                        cardentrytimer.cancel();
                        final_string = "CONFIRM";
                        isExit = true;
                    }
                });
            }
        });

        // wait for tap event -- sidumili
        do {
            // do whatever you need in child activity, but once you want to finish, do this and continue in parent activity
            synchronized (MainActivity.LOCK) {
                MainActivity.LOCK.setCondition(true);
                MainActivity.LOCK.notifyAll();
            }
        }while (!isExit);

        Log.i(TAG, "DisplayTipAdjustApprovedUI: final_string=" + final_string);

        return final_string;
    }

    public String UserConfirmDCC(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    //intent.setClass(MainActivity.this, ConfirmMenu.class);
                    intent.setClass(MainActivity.this, UserConfirmDCC.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = UserConfirmDCC.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String InputQWERTYUI(String text) throws InterruptedException {

        String user_str="";
        pass_in_string = text;


        new Thread()
        {
            public void run()
            {
                try
                {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, QwertyUI.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                }
                catch(Exception e)
                {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = QwertyUI.final_string;

        Log.i("TINE, user_str", user_str);
        Log.i("TINE, user_str", "FINISH InputQWERTYUI");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String Ping(String text) {

        String user_str = "";
        String bat_level = "";
        String success = "NOT_OK";

        String ipAddress = text;

        Log.i("saturn", "saturn Ping: "+ipAddress);

        try {
            InetAddress inet = InetAddress.getByName(ipAddress);

            System.out.println("Sending Ping Request to " + ipAddress);
            if(inet.isReachable(5000))
                success="OK";

            if(success.equals("OK"))
                Log.i("saturn Ping", "Host is reachable");
            else
                Log.i("saturn Ping", "Host is NOT reachable");
        }
        catch (Exception e) {

        }

        Log.i("saturn", "saturn Ping: " + success);

        user_str = success;

        Log.i("saturn", "saturn Ping user_str: " + user_str);

        return user_str;
    }

    public String GetBatteryLevel(String text) {

        String user_str="";
        String bat_level="";
        String charging="";

        BatteryManager bm = (BatteryManager)getSystemService(BATTERY_SERVICE);



        IntentFilter ifilter = new IntentFilter(Intent.ACTION_BATTERY_CHANGED);
        Intent batteryStatus = mContext.registerReceiver(null, ifilter);

        int status = batteryStatus.getIntExtra(BatteryManager.EXTRA_STATUS, -1);
        boolean isCharging = status == BatteryManager.BATTERY_STATUS_CHARGING;

        if (isCharging == true)
            charging = "1";
        else
            charging = "0";

        Log.i("saturn", "saturn isCharging: " + isCharging);

        //if (android.os.Build.VERSION.SDK_INT > = android.os.Build.VERSION_CODES.LOLLIPOP) {
        int percentage = 0;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
            percentage = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY);
        }
        bat_level = String.valueOf(percentage);
        //}

        user_str = charging+bat_level;


        Log.i("saturn", "saturn GetBatteryLevel: " + user_str);

        return user_str;

    }

    public String GetTipString(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    //intent.setClass(MainActivity.this, GetAmount.class);
                    intent.setClass(MainActivity.this, TipEntryActivity.class);

                    //bundle.putString("amt_string", amt_string);
                    bundle.putString("pass_in_string", pass_in_string);
                    //bundle.putString("display", "AMOUNT  (PHP)");
                    bundle.putString("minlength", "1");
                    bundle.putString("maxlength", "11");
                    bundle.putString("type", "1");
                    //bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                    //startActivityForResult(intent, 0);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = TipEntryActivity.final_amt_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH GetTipString");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public static void displaySignalStrength(int pStrength)
    {
        Log.i(TAG, "displaySignalStrength: ");
        Log.i(TAG, "pStrength="+pStrength);

        img_signalstrength.setBackgroundResource(R.drawable.bar0);

        if (pStrength <= 25)
            img_signalstrength.setBackgroundResource(R.drawable.bar1);
        else if (pStrength <= 50)
            img_signalstrength.setBackgroundResource(R.drawable.bar2);
        else if (pStrength <= 75)
            img_signalstrength.setBackgroundResource(R.drawable.bar3);
        else if (pStrength <= 100)
            img_signalstrength.setBackgroundResource(R.drawable.bar4);
    }

    private void executeLockScreen()
    {
        Log.i(TAG, "executeLockScreen: run");

        Intent intent = new Intent();
        Bundle bundle = new Bundle();
        intent.setClass(MainActivity.this, LockScreen.class);
        bundle.putString("pass_in_string", pass_in_string);
        Log.i("pass_in_string", pass_in_string);
        intent.putExtras(bundle);
        startActivity(intent);

    }

    public String PrintAnimation(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, PrintAnimation.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = PrintAnimation.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String PrintAnimation2(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, PrintAnimation2.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = PrintAnimation2.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    public String PrintAnimation3(String text) throws InterruptedException {

        String user_str = "";
        pass_in_string = text;

        new Thread() {
            public void run() {
                try {
                    Intent intent = new Intent();
                    Bundle bundle = new Bundle();
                    intent.setClass(MainActivity.this, PrintAnimation3.class);

                    bundle.putString("pass_in_string", pass_in_string);
                    Log.i("pass_in_string", pass_in_string);

                    intent.putExtras(bundle);
                    startActivity(intent);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();

        MainActivity.LOCK.setCondition(false);

        synchronized (LOCK) {
            while (!LOCK.conditionMet()) {
                try {
                    LOCK.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, "Exception when waiting for condition", e);
                    return "Exception";
                }
            }
        }

        user_str = PrintAnimation3.final_string;

        Log.i("PATRICK", user_str);
        Log.i("PATRICK", "FINISH 789");
        //pin_num = pin_num + "XXX";
//        Toast.makeText(this, pin_num, Toast.LENGTH_SHORT).show();
        return user_str;

    }

    @Override
    protected void onResume() {
        Log.i(TAG, "onResume: run");
        super.onResume();

        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        releaseAll();
        Log.i(TAG, "****S1_BDO onDestroy: ****");
    }

    private void releaseAll() {
        Log.i(TAG, "S1_BDO releaseAll: ");

        //final_string = null;;
//        Enter_Press = 0;
//        inRet = 0;
        //PinBlock = null;
        //KSN = null;

        AppHeaderBuff = null;
        final_string = null;

        sPinBlock = null;
        sKSN = null;
        OutputStr = null;

        mContext = null;
        mHandler = null;

        LOCK = null;
        jni_api = null;
        jni_api_offline_pin = null;
        jni_cb_ctos = null;
        jni_cb_emv = null;

        lsv_LCDTPrint = null;
        adapter = null;
        mPager = null;
        mDatas.clear();
        mDatas = null;
        mPagerList.clear();
        mPagerList = null;
        mLlDot = null;
        inflater = null;

/*      jni_api.REG_CB_CTOS(jni_cb_ctos);
        jni_api_offline_pin.REG_CB_EMV(jni_cb_emv);*/
//        SysApplication.getInstance().exit();
        Log.i(TAG, "releaseAll done.: ");
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int inCTOSS_TerminalStartUp();
    public native int inCTOSS_InitWaveData();
	public native int inCTOSS_Main();
	//public native int inCTOSS_Sale();
    public native int inCTOSS_Sale(boolean isECR);
    public native int inCTOSS_Void();
    public native int inCTOS_BINCHECK();
    public native int inCTOSS_Settle();
	public native int inDisplayMsg();
	public native int inDisplayImage();
    public native int inCTOSS_REPORTS();
    public native int inCTOSS_PREAUTH();
    public native int inCTOSS_INSTALLMENT();
    public native int inCTOSS_SALE_OFFLINE();
    public native int inCTOSS_TIP_ADJUST();
    public native int inCTOSS_REPRINT();
    public native int inCTOSS_SETTINGS();
    public native int inCTOSS_BUTTONCANCEL();
    public native int inCTOSS_CARDENTRYTIMEOUT();
    public native int inCTOSS_LogOn();
    public native int inCTOSS_Balance();
	public native byte[] inCTOS_GetBuffer();
    public native int inCTOSS_SETUP();
    public native int inCTOSS_Batch_Total();
    public native int inCTOSS_Batch_Review();
    public native int inCTOSS_Settle_Selection();
    public native int inCTOSS_MANUALENTRY();
    public native int AppStart();
    public native String CTOSS_MSRRead_inJava();
    public native int inCTOSS_Idle_Sale();
    public native int idleAmountEntry();
    public native int posMain_init();
    public native String idleEMVFallback();
	public native int inCTOSS_Get1stInitFlag();
	public native int inCTOSS_GetDLNotFinishedFlag();
    public native String GetAppHeaderDetails(int inMenu);
    public native int inCTOS_INSTALLMENT();
    public native int inCTOS_QRPay();
    public native int inCTOS_Retrieve();
    public native int inCTOS_BDOPayMenu();
    public native int inCTOS_HistoryMenu();
    public native int inCTOSS_MANAGEMENT();
    public native int inCTOSS_TIPADJUST();
    public native int inCTOSS_CTMSDownloadRequest();

    public native int inCTOSS_ECR_Menu(int inECRTxnType);

    public native int inCTOS_TransactionsMenu(int inMenu);
    public native int inCTOSS_CreatefsdataFolder();
    public native int inCTOSS_GetEnvInt(String sztag);


}

