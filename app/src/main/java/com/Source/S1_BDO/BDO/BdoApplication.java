package com.Source.S1_BDO.BDO;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.os.RemoteException;


//import com.squareup.leakcanary.LeakCanary;

import java.util.ArrayList;
import java.util.List;

import CTOS.CtCtms;
import castles.ctms.module.commonbusiness.DownloadInfo;
import castles.ctms.module.commonbusiness.IAgentCallback;


public class BdoApplication extends Application {
    private static final String TAG = "BdoApplication";
    private Context mContext;
    private static int activityNum = 0;
    private static List<String> activityList = new ArrayList<String>();

	private static BdoApplication instance;
    private int installNum = 0;
    private int totalNum = 0;
    private boolean downloaded = false;
    private static CtCtms ctCtms;

    public static List<String> getActivityList() {
        return activityList;
    }

    public static int getActivityNum() {
        Log.i(TAG, "getActivityNum: " + activityNum);
        return activityNum;
    }

	 public static CtCtms getCtCtmsObj() {
        Log.i(TAG, "getCtCtmsObj getCTMSStatus: " + ctCtms.getCTMSStatus());
        return ctCtms;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "onCreate: ");

		ctCtms = new CtCtms(this);
		
        //Check Memory Leak
/*
        if (LeakCanary.isInAnalyzerProcess(this)) {
            //此过程专用于LeakCanary进行堆分析。在此过程中不应初始化应用程序。
            Log.i(TAG, "isInAnalyzerProcess: ");
            return;
        }
        LeakCanary.install(this);
 */

        registerActivityLifecycleCallbacks(new ActivityLifecycleCallbacks() {
            @Override
            public void onActivityCreated(Activity activity, Bundle bundle) {
                Log.i(TAG, "onActivityCreated: " + activity.toString());
                activityNum++;
                activityList.add(activity.getLocalClassName());
            }

            @Override
            public void onActivityStarted(Activity activity) {
                Log.i(TAG, "onActivityStarted: " + activity.toString());
            }

            @Override
            public void onActivityResumed(Activity activity) {
                Log.i(TAG, "onActivityResumed: " + activity.toString());

            }

            @Override
            public void onActivityPaused(Activity activity) {
                Log.i(TAG, "onActivityPaused: " + activity.toString());
            }

            @Override
            public void onActivityStopped(Activity activity) {
                Log.i(TAG, "onActivityStopped: " + activity.toString());
            }

            @Override
            public void onActivitySaveInstanceState(Activity activity, Bundle bundle) {
                Log.i(TAG, "onActivitySaveInstanceState: ");
            }

            @Override
            public void onActivityDestroyed(Activity activity) {
                Log.i(TAG, "onActivityDestroyed: " + activity.toString() + " class name: " + activity.getLocalClassName());
                activityNum--;
                activityList.remove(activity.getLocalClassName());
                // CloseActivityClass.activityList.remove(activity);
            }
        });

    }
	 public static Context getMyApplication() {
        return instance;
    }

	private void vdUpdateDownloadStatus (final String msg) {
			 Log.i(TAG, "vdUpdateDownloadStatus: " + msg);
	 /* 	   runOnUiThread(new Runnable() {
				 @Override
				 public void run() {
					 tvStatus.setText(msg);
				 }
			 });*/
	}


   /* IAgentCallback statusCalback = new IAgentCallback.Stub() {

        @Override
        public void readyCallback() throws RemoteException {
            Log.i("CTMS-cb", "readyCallback: Callback when CTMS process is finish, got download:" + downloaded);
            Log.i(TAG, "totalNum downloaded file: " + totalNum);
            if(downloaded) {
//                inCTOSS_BackupDataScript();
                Log.i(TAG, "readyCallback: if Dl multi app, then this will be called multi times");
//                vdRebootNow();
            }



        }

        @Override
        public void connectCallback(int i) throws RemoteException {
            Log.i("CTMS-cb " + i,  "connectCallback: callback when connect to server:");

        }

        @Override
        public void getTerminalInfoCallback(int i) throws RemoteException {
            Log.i("CTMS-cb " + i, "getTerminalInfoCallback: callback when upload app and FW infor:");

        }

        @Override
        public void downloadCallback(int i, DownloadInfo downloadInfo) throws RemoteException {
            Log.i("CTMS-cb " + i, "Download fileName: " + downloadInfo.fileName + "  fullSize:" + downloadInfo.fullSize + "   downloadSize=" + downloadInfo.downloadSize + "  totalDownloadNumber:" + downloadInfo.totalDownloadNumber+ " downloadIndex=" + downloadInfo.downloadIndex);
            if (downloadInfo.downloadSize > 0) {
                Log.i(TAG, "downloadSize >0:");
                if(!downloadInfo.fileName.equals("UpdateList")) {
                    if(downloadInfo.downloadSize >= downloadInfo.fullSize)
                    {
                        totalNum++;
                        Log.i(TAG, "finish download one file, total file num increase to: " + totalNum);
                    }

                    downloaded = true;
                    int progress = (int) ((((float) downloadInfo.downloadSize) / ((float) downloadInfo.fullSize)) * 100);

                    Log.i(TAG, "progress: " + progress);
                    String s=String.valueOf(progress) + "%";

                    String filesize;
                    double size;
                    StringBuffer bytes = new StringBuffer();
                    DecimalFormat format = new DecimalFormat("###.0");
                    if(downloadInfo.fullSize > (1024*1024)) {
                        size = (downloadInfo.fullSize / (1024.0 * 1024.0));
                        bytes.append(format.format(size)).append("MB");
                    } else if(downloadInfo.fullSize > (1024))
                    {
                        size = (downloadInfo.fullSize / (1024.0));
                        bytes.append(format.format(size)).append("KB");

                    } else {
                        bytes.append((int) downloadInfo.fullSize).append("B");
                    }
                    filesize = bytes.toString();
//                    vdUpdateDownloadStatus("Downloading...\n" + downloadInfo.fileName + "\n"+ filesize + "\n" + s);
//                    vdUpdateDownloadProcessBar(progress);
                    Log.d(TAG, "got update: ");
                }

            } else {
                Log.i(TAG, "No update from CTMS server: ");
            }
        }

        @Override
        public void diagnosticCallback(int i) throws RemoteException {
            Log.i("CTMS-cb " + i, "diagnosticCallback: callback when run diag");
            vdUpdateDownloadStatus("Diagnostic...");
        }

        @Override
        public void installCallback(int i, castles.ctms.module.commonbusiness.PackageInfo packageInfo) throws RemoteException {
            Log.i("CTMS-cb " + i, "installCallback: " + packageInfo.fileName);
            if(i == 4096)//4096 start, 0 end
                installNum++;
            String strInstallNum = "[install " + installNum + "/" + totalNum + "]\n";
            vdUpdateDownloadStatus("Installing...\n" + strInstallNum +  packageInfo.fileName);
        }
    };*/
}
