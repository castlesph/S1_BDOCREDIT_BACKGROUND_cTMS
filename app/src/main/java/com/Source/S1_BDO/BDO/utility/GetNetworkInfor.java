package com.Source.S1_BDO.BDO.utility;

import android.content.Context;
import android.net.LinkProperties;
import android.net.Network;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;  
import android.net.DhcpInfo;


import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import static android.content.Context.WIFI_SERVICE;
import static android.content.Context.CONNECTIVITY_SERVICE;


public class GetNetworkInfor {
    private static final String TAG = "GetNetworkInfor";
    private  Context mContext;

	private static String sMobileDNSServer = "";
	private static String sMobileDNS1 = "";
	private static String sMobileDefaultIP = "0.0.0.0";
	private static String sMobileDefaultSubNetMask = "255.255.255.0";

    public GetNetworkInfor(Context context) {
        mContext = context;
    }

	private String dns1;
	private String dns2;
	private String gateway;
	private String ipAddress;
	private String leaseDuration;
	private String netmask;
	private String serverAddress;
	
	private DhcpInfo dhcpInfo;
	private WifiManager wifiManager;


 
    public  String getWifiSSID() {
        String s = "";
        WifiManager wm = (WifiManager) mContext.getSystemService(WIFI_SERVICE);
        if (wm != null) {
            WifiInfo winfo = wm.getConnectionInfo();
            if (winfo != null) {
                s = winfo.getSSID();
                Log.i(TAG, "saturn getWifiSSID: " + s);
                if (s.length() > 2 && s.charAt(0) == '"' && s.charAt(s.length() - 1) == '"'){
                    return s.substring(1, s.length() - 1);
                }
            }
        }
        return s;
    }

    public String getWifiBSSID() {
        WifiManager wm = (WifiManager) mContext.getSystemService(WIFI_SERVICE);
        if (wm != null) {
            WifiInfo winfo = wm.getConnectionInfo();
            return  winfo.getBSSID();
        }
        return null;
    }


	public boolean isOnline() {
		ConnectivityManager cm = (ConnectivityManager) mContext.getSystemService(CONNECTIVITY_SERVICE);
    	NetworkInfo netInfo = cm.getActiveNetworkInfo();
    	if (netInfo != null && netInfo.isConnected()) {
			//Log.i(TAG, "saturn isOnline : true ");
        	return true;
    	}else {  
			//Log.i(TAG, "saturn isOnline : false ");
        	return false;
    	}
    }
	

	public String GetIPSettings() {

        String OutputStr= "";
		WifiManager wm = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
	
		//if (wm == null) {
		//	return false;
		//}

		dhcpInfo = wm.getDhcpInfo();

		//if (dhcpInfo == null) {
		//	return false;
		//}
	
		//Log.i(TAG, "saturn dns1: " + int2Ip(dhcpInfo.dns1));
		//Log.i(TAG, "saturn dns2: " + int2Ip(dhcpInfo.dns2));
		//Log.i(TAG, "saturn Gateway: " + int2Ip(dhcpInfo.gateway));
		//Log.i(TAG, "saturn ipAddress: " + int2Ip(dhcpInfo.ipAddress));
		//Log.i(TAG, "saturn Netmask: " + int2Ip(dhcpInfo.netmask));

		
		OutputStr = int2Ip(dhcpInfo.dns1)+'*'+int2Ip(dhcpInfo.dns2)+'*'+int2Ip(dhcpInfo.gateway)
			        +'*'+int2Ip(dhcpInfo.ipAddress)+'*'+int2Ip(dhcpInfo.netmask);

        

		//OutputStr = int2Ip(dhcpInfo.ipAddress)+'*'+int2Ip(dhcpInfo.gateway)+'*'+"255.255.255.255"
						//	+'*'+int2Ip(dhcpInfo.dns1)+'*'+"1.1.1.1";

		//Log.i(TAG, "saturn OutputStr: " + OutputStr);

		return OutputStr;
	}

	public String getNetworkType(){
		//boolean WIFI = false;
		
		//boolean MOBILE = false;
		String inType = "0";
		String OutputStr= "";
		
		ConnectivityManager cm = (ConnectivityManager) mContext.getSystemService(CONNECTIVITY_SERVICE);
		
		NetworkInfo[] networkInfo = cm.getAllNetworkInfo();
		
		for (NetworkInfo netInfo : networkInfo) {
		
			if (netInfo.getTypeName().equalsIgnoreCase("WIFI")){
				
				if (netInfo.isConnected())
					inType = "4";

				
                Log.i(TAG, "saturn conenction is wifi: " + inType);
			}
		
			if (netInfo.getTypeName().equalsIgnoreCase("MOBILE")){
				if (netInfo.isConnected())
					inType = "2";

				
                Log.i(TAG, "saturn conenction is mobile: " + inType);
			}
		}
		
       return inType;

	}

	
	private String int2Ip(int i) {
		return (( i & 0xFF) + "." +
				((i >> 8) & 0xFF) + "." +
				((i >> 16) & 0xFF) + "." +
				((i >> 24) & 0xFF));
	}

	// sidumili: added to get mobile ip, gateway, subnet
	public String GetDeviceipMobileData(){
		String sTerminalIP = "";
		String sGateway = "";
		String sSubNetMask = sMobileDefaultSubNetMask;
		String sMobileInfo = "";

		Log.i("sidumili", "GetDeviceipMobileData: run");

		try {
			for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces();
				 en.hasMoreElements();) {
				NetworkInterface networkinterface = en.nextElement();
				for (Enumeration<InetAddress> enumIpAddr = networkinterface.getInetAddresses(); enumIpAddr.hasMoreElements();) {
					InetAddress inetAddress = enumIpAddr.nextElement();
					if (!inetAddress.isLoopbackAddress()) {
						sTerminalIP = inetAddress.getHostAddress().toString();
						sGateway = inetAddress.getHostAddress().toString();
					}
				}
			}
		} catch (Exception ex) {
			Log.e("Current IP", ex.toString());
		}

		Log.i(TAG, "GetDeviceipMobileData: sTerminalIP="+sTerminalIP);
		Log.i(TAG, "GetDeviceipMobileData: sGateway="+sGateway);
		Log.i(TAG, "GetDeviceipMobileData: sSubNetMask="+sSubNetMask);

		sMobileInfo = sTerminalIP + "|" + sGateway + "|" + sSubNetMask + "|";

		Log.i(TAG, "GetDeviceipMobileData: Result sMobileInfo="+sMobileInfo);

		if (!isValidIP(sTerminalIP) || sTerminalIP.length() <= 0)
		{
			sMobileInfo = sMobileDefaultIP + "|" + sMobileDefaultIP + "|" + sMobileDefaultIP + "|";
		}

		return sMobileInfo;
	}

	// sidumili: added to get mobile dns1, dns2
	public static String GetDNSServer(Context context)
	{
		String sDNS = "";

		sMobileDNSServer= "";
		sMobileDNS1 = "";
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
			getDnsServers(context).toString();
		}

		sDNS = sMobileDNSServer;
		Log.i(TAG, "GetDNSServer: Result sDNS="+sDNS);
		Log.i(TAG, "GetDNSServer: Result sMobileDNS1="+sMobileDNS1);

		if (!isValidIP(sMobileDNS1) || sMobileDNS1.length() <= 0)
		{
			sDNS = sMobileDefaultIP + "|" + sMobileDefaultIP + "|";
		}

		return sDNS;
	}

	@RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
	public static List<InetAddress> getDnsServers(Context context) {
		String sTempDNS = "";
		String sDNS = "";
		boolean isDNS1 = false;

		List<InetAddress> servers = new ArrayList<>();
		ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
		Network[] networks = new Network[0];
		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
			networks = connectivityManager == null ? null : new Network[]{connectivityManager.getActiveNetwork()};
		}
		if (networks == null) {
			return servers;
		}
		for(int i = 0; i < networks.length; ++i) {
			LinkProperties linkProperties = connectivityManager.getLinkProperties(networks[i]);
			if (linkProperties != null) {
				servers.addAll(linkProperties.getDnsServers());
				sTempDNS = linkProperties.getDnsServers().toString();
				//Log.i(TAG, "getDnsServers: sDNS="+sTempDNS);
			}
		}

		// Loop for dns1, dns2
		for(InetAddress server : servers) {
			//Log.i(TAG, "DNS server: " + server.getHostName() + "Host Address:" + server.getHostAddress());

			// sidumili: I just want to get the DNS1
			if (!isDNS1)
			{
				sMobileDNS1 = server.getHostAddress();
				isDNS1 = true;
			}
			sDNS = sDNS + server.getHostAddress() + "|";
		}

		sMobileDNSServer = sDNS;

		return servers;
	}

	// sidumili: simple check for valid ip address
	public static boolean isValidIP(String sIP)
	{
		boolean isValid = false;
		//char arrayList[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'};
		char arrayList[] = {'A', 'B', 'C', 'D', 'E', 'F'};
		int iIndexOf;

		sIP = sIP.toUpperCase();

		Log.i(TAG, "isValidIP: Start, sIP="+sIP);
		Log.i(TAG, "isValidIP: arrayList.length="+arrayList.length);

		for (int i = 0; i < arrayList.length ; i++)
		{
			iIndexOf =sIP.indexOf(arrayList[i]);
			Log.i(TAG, "isValidIP: Loop, sIP="+sIP +  ", i=" + i + ", arrayList[i]="+arrayList[i]+",iIndexOf="+iIndexOf);

			if (iIndexOf >= 0)
			{
				isValid = false;
				break;
			}
			else
			{
				isValid = true;
			}
		}

		Log.i(TAG, "isValidIP: isValid="+isValid);

		return  isValid;
	}


}


