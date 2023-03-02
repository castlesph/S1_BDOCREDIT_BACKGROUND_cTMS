package com.Source.S1_BDO.BDO.Main;

import android.app.Activity;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.widget.Toast;

import com.Source.S1_BDO.BDO.SysApplication;
import com.Source.S1_BDO.BDO.model.DemoAppActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;

public class MultiAP_SubAP extends DemoAppActivity {

	boolean isInitialize = false;

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
//    private JNICB_implements_CTOS_SUB jni_cb_ctos = new JNICB_implements_CTOS_SUB(this);

    public int chmod(File path, int mode) throws Exception {
        Class fileUtils = Class.forName("android.os.FileUtils");
        Method setPermissions =
                fileUtils.getMethod("setPermissions", String.class, int.class, int.class, int.class);
        return (Integer) setPermissions.invoke(null, path.getAbsolutePath(), mode, -1, -1);
    }

    private void copyFile(String filename) {
        AssetManager assetManager = this.getAssets();

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
            else if (filename.toLowerCase().startsWith("55"))
            {
                newFileName = "/data/data/pub/" + strFilename;
            }
            else if (filename.toLowerCase().startsWith("56"))
            {
                newFileName = "/data/data/pub/" + strFilename;
            }
            else if (filename.toLowerCase().startsWith("11"))
            {
                newFileName = "/data/data/pub/" + strFilename;
            }
            else
            {
                newFileName = "/data/data/" + this.getPackageName() + "/" + strFilename;
            }

            Log.i("Castles", "copyFile() Path " + newFileName);

            File dbFile = new File(newFileName);
            if (!dbFile.exists())
            {
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

    public void copyFileOrDir(String path) {
        AssetManager assetManager = this.getAssets();
        String assets[] = null;
        try {
            Log.i("tag", "copyFileOrDir() " + path);
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
            Log.e("tag", "I/O Exception", ex);
        }
    }

    private EMVMainAsyncTask mEMVTask;

	private static class EMVMainAsyncTask extends AsyncTask<Boolean, Void, Boolean> {

//	 private WeakReference<MainActivity> mmainActivity;

//		  public ECRMainAsyncTask(MainActivity mainActivity) {
//			  this.mmainActivity = new WeakReference<MainActivity>(mainActivity);
//		  }

        Activity mContext = null;
        static AsyncTask<Boolean, Void, Boolean> myAsyncTaskInstance = null;

        // Private Constructor: can't be called from outside this class
        private EMVMainAsyncTask() {
        //    mContext = iContext;
        }

        @Override
        protected Boolean doInBackground(Boolean... booleans) {

            // do what you need and if you decide to stop this activity and wait for the sub-activity, do this
            inCTOSS_SubAPMain();
            Log.d("inCTOSS_SubAPMain", "End");
            return null;
        }

        public static AsyncTask<Boolean, Void, Boolean> getInstance() {
            // if the current async task is already running, return null: no new async task
            // shall be created if an instance is already running
            if (myAsyncTaskInstance != null && myAsyncTaskInstance.getStatus() == Status.RUNNING) {
                // it can be running but cancelled, in that case, return a new instance
                if (myAsyncTaskInstance.isCancelled()) {
                    myAsyncTaskInstance = new EMVMainAsyncTask();
                } else {
                    // display a toast to say "try later"
                    //Toast.makeText(iContext, "A task is already running, try later", Toast.LENGTH_SHORT).show();

                    return null;
                }
            }

            //if the current async task is pending, it can be executed return this instance
            if (myAsyncTaskInstance != null && myAsyncTaskInstance.getStatus() == Status.PENDING) {
                return myAsyncTaskInstance;
            }

            //if the current async task is finished, it can't be executed another time, so return a new instance
            if (myAsyncTaskInstance != null && myAsyncTaskInstance.getStatus() == Status.FINISHED) {
                myAsyncTaskInstance = new EMVMainAsyncTask();
            }

            // if the current async task is null, create a new instance
            if (myAsyncTaskInstance == null) {
                myAsyncTaskInstance = new EMVMainAsyncTask();
            }
            // return the current instance
            return myAsyncTaskInstance;
        }
	}


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        overridePendingTransition(0, 0);

//        int inTimes = FileCopyTime();
        SharedPreferences wmbPreference = PreferenceManager.getDefaultSharedPreferences(this);

        boolean isFirstRun = wmbPreference.getBoolean("FIRSTRUN", true);

        if (isFirstRun)
        {
            // copy file or Dir
            copyFileOrDir("");

            SharedPreferences.Editor editor = wmbPreference.edit();
            editor.putBoolean("FIRSTRUN", false);
            editor.commit();
        }else{
            //what you do everytime goes here
        }

        Toast.makeText(getApplicationContext(),"SHARLS COM MultiAP_SubAP" , Toast.LENGTH_SHORT).show();
		Log.d("inCTOSS_SubAPMain", "Begin");
//		if(isInitialize == false)
//		{
//			inEMVFirstInit();
//			isInitialize = true;
//		}
//	        jni_api.REG_CB_CTOS(jni_cb_ctos);

        mEMVTask = new EMVMainAsyncTask();
        mEMVTask.execute(true);

//		inCTOSS_SubAPMain();
//		Log.d("inCTOSS_SubAPMain", "End");
//		moveTaskToBack(true);
//	        super.onBackPressed();
		finish();

		Log.d("inCTOSS_SubAPMain", "End");
    }

    public static final String TAG = MainActivity.class.getSimpleName();

    @Override
    protected void onRestart() {
        Log.d(TAG, "onRestart()");
        super.onRestart();
    }

    @Override
    protected void onStart() {
        Log.d(TAG, "onStart()");
        super.onStart();
    }

    @Override
    protected void onResume() {
        Log.d(TAG, "onResume()");
        super.onResume();
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "onPause()");
        super.onPause();
    }

    @Override
    protected void onStop() {
        Log.d(TAG, "onStop()");
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy()");
        super.onDestroy();
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native int inCTOSS_SubAPMain();

}


