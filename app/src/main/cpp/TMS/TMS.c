/*******************************************************************************

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>

#include <unistd.h>
#include <pwd.h>


#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>


#include "..\Includes\Wub_lib.h"

#include "..\Includes\POSTypedef.h"
#include "..\FileModule\myFileFunc.h"

#include "..\Includes\msg.h"
#include "..\Includes\CTOSInput.h"
#include "..\ui\Display.h"

#include "..\Includes\V5IsoFunc.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\Trans.h"   


#include "..\Includes\CTOSInput.h"


#include "..\debug\debug.h"
#include "..\Accum\Accum.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\myEZLib.h"
#include "..\accum\accum.h"
#include "..\Includes\POSSetting.h"
#include "..\Debug\Debug.h"
#include "..\filemodule\myFileFunc.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\CTOSInput.h"
#include "..\Ctls\PosWave.h"
#include "..\TMS\TMS.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"


#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Aptrans\MultiShareEMV.h"


//adc
extern int inADLTimeRangeUsed;
//adc

#if 0

#ifndef d_CTMS_INFO_LAST_UPDATE_TIME
#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
#endif

USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo)
{
    CTOS_RTC SetRTC;
    
    CTOS_RTCGet(&SetRTC);

    SetRTC.bMonth -= 2;

    memcpy(stInfo, &SetRTC, sizeof(CTOS_RTC));

    return d_OK;
}

#endif

int inCTOSS_TMSCheckIfDefaultApplication(void)
{
    CTOS_stCAPInfo stinfo;
	BYTE exe_dir[128]={0};
	BYTE exe_subdir[128]={0};
	USHORT inExeAPIndex = 0;

    getcwd(exe_dir, sizeof(exe_dir)); //Get current working dir string
	strcpy(exe_subdir, &exe_dir[9]);
	inExeAPIndex = atoi(exe_subdir);
	memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));
    if(CTOS_APGet(inExeAPIndex, &stinfo) != d_OK)
	{
		CTOS_APGet(inExeAPIndex, &stinfo);
	}

    vdDebug_LogPrintf("stinfo.bFlag[%02X]", stinfo.bFlag);
    if (stinfo.bFlag != d_AP_FLAG_DEF_SEL_EX)
        return d_NO;
    else
        return d_OK;
    
}

int inCTOSS_TMSChkBatchEmpty(void)
{
    int inResult;

    vdDebug_LogPrintf("inCTOSS_TMSChkBatchEmpty start");
    
    if (inMultiAP_CheckMainAPStatus() == d_OK)
	{
		inResult = inCTOSS_TMSChkBatchEmptyProcess();
        if(inResult != d_OK)
            return inResult;
        else
		    inResult = inCTOS_MultiAPALLAppEventID(d_IPC_CMD_CHK_BATCH_EMPTY);

        if(inResult != d_FAIL)
            inResult = d_OK;
	}
	else
	{
		inResult = inCTOSS_TMSChkBatchEmptyProcess();
	}
    
    vdDebug_LogPrintf("inCTOSS_TMSChkBatchEmpty return[%d]", inResult);
    
    return (inResult);
}


int inCTOSS_TMSChkBatchEmptyProcess(void)
{
    int inResult;
    int shHostIndex = 1;
    int inNum = 0;
    int inNumOfHost = 0;
    int inNumOfMerchant = 0;
    int inLoop =0 ;
    ACCUM_REC srAccumRec;
    STRUCT_FILE_SETTING strFile;
    char szAPName[25];
	int inAPPID;

	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    //check host num
    inNumOfHost = inHDTNumRecord();
    
    vdDebug_LogPrintf("[inNumOfHost]-[%d]", inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);
			if (strcmp(szAPName, strHDT.szAPName)!=0)
			{
				continue;
			}
            
            inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfMerchant);
        
            vdDebug_LogPrintf("[inNumOfMerchant]-[%d]strHDT.inHostIndex[%d]", inNumOfMerchant,strHDT.inHostIndex);
            for(inLoop=1; inLoop <= inNumOfMerchant;inLoop++)
            {
            /*
                if((inResult = inMMTReadRecord(strHDT.inHostIndex, strMMT[inLoop-1].MMTid)) !=d_OK)
                {
                    vdDebug_LogPrintf("[read MMT fail]-Mitid[%d]strHDT.inHostIndex[%d]inResult[%d]", strMMT[inLoop-1].MMTid,strHDT.inHostIndex,inResult);
                    continue;
                    //break;
                }
                else    // delete batch where hostid and mmtid is match  
                { 
                */
                    strMMT[0].HDTid = strHDT.inHostIndex;
                    strMMT[0].MITid = strMMT[inLoop-1].MITid;
                    srTransRec.MITid = strMMT[inLoop-1].MITid;

                    vdDebug_LogPrintf("srTransRec.MITid[%d]strHDT.inHostIndex[%d]", srTransRec.MITid, strHDT.inHostIndex);
                    memset(&srAccumRec,0,sizeof(srAccumRec));
                    memset(&strFile,0,sizeof(strFile));
                    memset(&srAccumRec, 0x00, sizeof(ACCUM_REC));
                    memset(&strFile,0,sizeof(strFile));
                    vdCTOS_GetAccumName(&strFile, &srAccumRec);

                if((inResult = inMyFile_CheckFileExist(strFile.szFileName)) > 0)
                {
                    vdDebug_LogPrintf("inCTOSS_TMSChkBatchEmpty Not Empty");
                    //vdDisplayErrorMsg(1, 8, "BATCH NOT", "EMPTY,SKIPPED.");
                    vdSetErrorMessage("");
                    return (d_FAIL);
                }
                else
                {
                    vdDebug_LogPrintf("Empty Batch");
                }
                
            //    }
            }
        }
        else
            continue;

    }
    
    return (d_OK);
}

int inCTOSS_TMSPreConfig(int inComType)
{    
    CTMS_GPRSInfo stgprs;    
    CTMS_ModemInfo stmodem;    
    CTMS_EthernetInfo st;
		CTMS_WIFIInfo stWF;
    int inNumOfRecords = 0;
    BYTE szSerialNum[17+1], szInputBuf[21+1];
    BYTE count = 2,i;
    BYTE tmpbuf[16 + 1];
    int len;
    USHORT usRet;
    unsigned char ckey;
    USHORT usStatusLine=8, usRes=0;
	
    inHDTRead(1);
    inMMTReadNumofRecords(strHDT.inHostIndex, &inNumOfRecords);
    
    memset(szSerialNum, 0x00, sizeof(szSerialNum));
	memset(tmpbuf, 0x00, sizeof(tmpbuf));

	CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
	if(strlen(szSerialNum) <= 0)
	    CTOS_GetFactorySN(szSerialNum);
	for (i=0; i<strlen(szSerialNum); i++)
	{
		if (szSerialNum[i] < 0x30 || szSerialNum[i] > 0x39)
		{
			szSerialNum[i] = 0;
			break;
		}
	}
	len = strlen(szSerialNum);
	strcpy(tmpbuf,"0000000000000000");
	memcpy(&tmpbuf[16-len],szSerialNum,len);
	vdDebug_LogPrintf("szSerialNum=[%s].tmpbuf=[%s]..",szSerialNum,tmpbuf);
	
    CTOS_CTMSSetConfig(d_CTMS_SERIALNUM, tmpbuf);//if TID is 12345678, SN is 0000000012345678
    CTOS_CTMSSetConfig(d_CTMS_RECOUNT, &count);

    if(strTCP.fDHCPEnable)
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, "0.0.0.0");
    else
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, strTCP.szTerminalIP);// If it is DHCP, also need to configure any value, otherwise please put the value from database

    if(ETHERNET_MODE == strTCT.inTMSComMode)
    {
        CTOS_LCDTClearDisplay();
        vdDispTitleString("ETHERNET SETTINGS");
	
    	vdDebug_LogPrintf("ETHERNET_MODE..");
        memset(&st, 0x00, sizeof (CTMS_EthernetInfo));
        
        strcpy(st.strGateway, strTCP.szGetWay);
        strcpy(st.strMask, strTCP.szSubNetMask);
        st.bDHCP = strTCP.fDHCPEnable;

        strcpy(st.strRemoteIP, strTCT.szTMSRemoteIP);
        st.usRemotePort = strTCT.usTMSRemotePort;


		CTOS_LCDTClearDisplay();
		vdDispTitleString("ETHERNET SETTINGS");

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", st.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", st.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
	
        usRes=CTOS_CTMSSetConfig(d_CTMS_ETHERNET_CONFIG, &st);
    }
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
    {
    	vdDebug_LogPrintf("DIAL_UP_MODE..");

        CTOS_LCDTClearDisplay();
        vdDispTitleString("DIALUP SETTINGS");

        memset(&stmodem, 0x00, sizeof (CTMS_ModemInfo));
        //stmodem.bMode = d_M_MODE_AYNC_FAST;
        //stmodem.bHandShake = d_M_HANDSHAKE_V32BIS_AUTO_FB;
        stmodem.bMode  = d_M_MODE_SDLC_FAST;
        stmodem.bHandShake = d_M_HANDSHAKE_V22_ONLY;
        stmodem.bCountryCode = d_M_COUNTRY_SINGAPORE;

        strcpy(stmodem.strRemotePhoneNum, strTCT.szTMSRemotePhone);
		vdDebug_LogPrintf("strRemotePhoneNum[%s]..",stmodem.strRemotePhoneNum);
        strcpy(stmodem.strID, strTCT.szTMSRemoteID);	
		vdDebug_LogPrintf("strID[%s]..",stmodem.strID);
        strcpy(stmodem.strPW, strTCT.szTMSRemotePW);
		vdDebug_LogPrintf("strPW[%s]..",stmodem.strPW);

        strcpy(stmodem.strRemoteIP, strTCT.szTMSRemoteIP);
        stmodem.usRemotePort = strTCT.usTMSRemotePort;

		stmodem.usPPPRetryCounter = 2;
		stmodem.ulPPPTimeout = 34463;
		stmodem.ulDialTimeout = 34463;
		vdDebug_LogPrintf("strRemoteIP[%s].usRemotePort=[%d].",stmodem.strRemoteIP,stmodem.usRemotePort);

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);


        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stmodem.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stmodem.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
        
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PHONE: %s", stmodem.strRemotePhoneNum);         
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf); 
		
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "ID: %s", stmodem.strID);         
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PASSWORD: %s", stmodem.strPW);         
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
		
        usRes=CTOS_CTMSSetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
    }
    else if(GPRS_MODE == strTCT.inTMSComMode)
    {
		CTMS_GPRSInfo stgprs;

        CTOS_LCDTClearDisplay();
        vdDispTitleString("GPRS SETTINGS");
		
		memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
		CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
		
        if(strlen(stgprs.strAPN) > 0)
            strcpy(strTCP.szAPN, stgprs.strAPN);

        if(strlen(stgprs.strID) > 0)
            strcpy(strTCP.szUserName, stgprs.strID);

        if(strlen(stgprs.strPW) > 0)
		    strcpy(strTCP.szPassword, stgprs.strPW);
		
              vdDebug_LogPrintf("GPRS_MODE..");
        memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
        strcpy(stgprs.strAPN, strTCP.szAPN);
        strcpy(stgprs.strID, strTCP.szUserName);
        strcpy(stgprs.strPW, strTCP.szPassword);
 
        strcpy(stgprs.strRemoteIP, strTCT.szTMSRemoteIP);
        stgprs.usRemotePort = strTCT.usTMSRemotePort;
        stgprs.ulSIMReadyTimeout = 10000;
        stgprs.ulGPRSRegTimeout = 10000;
        stgprs.usPPPRetryCounter = 5;
        stgprs.ulPPPTimeout = 10000;
        stgprs.ulTCPConnectTimeout = 10000;

		//test
		stgprs.ulTCPTxTimeout=10000;
		stgprs.ulTCPRxTimeout=10000;
		stgprs.bSIMSlot=1;

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);


        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stgprs.strRemoteIP);		 
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stgprs.usRemotePort);		 
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "APN: %s", stgprs.strAPN);		 
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "USER NAME: %s", stgprs.strID);		 
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
		
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PASSWORD: %s", stgprs.strPW);		 
        setLCDPrint(7, DISPLAY_POSITION_LEFT, szInputBuf);
		
        usRes=CTOS_CTMSSetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
    }
    else if(WIFI_MODE == strTCT.inTMSComMode)
    {
        CTOS_LCDTClearDisplay();
        vdDispTitleString("WIFI SETTINGS");
        
        vdDebug_LogPrintf("WIFI..");
        memset(&stWF, 0x00, sizeof (CTMS_WIFIInfo));
        usRet = CTOS_CTMSGetConfig(d_CTMS_WIFI_CONFIG, &stWF);


		strcpy(stWF.baPassword, strTCP.szWifiPassword);
		strcpy(stWF.baSSid, strTCP.szWifiSSID);
		//stWF.bProtocal = 3;
		//stWF.bGroup = 2;
		//stWF.bPairwise = 2;
		stWF.bDHCP = strTCP.fDHCPEnable;
		stWF.bSCAN_Mode = 1;
		stWF.IsAutoConnect =48;
		stWF.IsHidden =48;

		memset(stWF.strLocalIP, 0x00, sizeof(stWF.strLocalIP));
		memset(stWF.strMask, 0x00, sizeof(stWF.strMask));	
		memset(stWF.strGateway, 0x00, sizeof(stWF.strGateway));

		if (strTCP.fDHCPEnable){
			strcpy(stWF.strLocalIP, "0.0.0.0");
			strcpy(stWF.strMask, "0.0.0.0");
			strcpy(stWF.strGateway, "0.0.0.0");
		}else{
			strcpy(stWF.strLocalIP, strTCP.szTerminalIP);
			strcpy(stWF.strMask, strTCP.szSubNetMask);
			strcpy(stWF.strGateway, strTCP.szGetWay);
		}
	
		
        //if (usRet != d_OK)
        //{
        //   CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
        //   vdDisplayErrorMsg(1, 8, "CTMS Get Fail");
        //   return d_NO;
        //}
           CTOS_LCDTClearDisplay();
           vdDispTitleString("WIFI SETTINGS");
        
           memset(szSerialNum, 0x00, sizeof(szSerialNum));
           CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
           
           memset(szInputBuf, 0x00, sizeof(szInputBuf));
           sprintf(szInputBuf, "SN: %s", szSerialNum);         
           setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 
           
           memset(szInputBuf, 0x00, sizeof(szInputBuf));
           sprintf(szInputBuf, "IP: %s", stWF.strRemoteIP);         
           setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 
           
           memset(szInputBuf, 0x00, sizeof(szInputBuf));
           sprintf(szInputBuf, "PORT: %d", stWF.usRemotePort);         
           setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 


		   //bProtocal
		   if (strcmp(strTCP.szWifiProtocal, "WEP")  == 0)
		   {
			   stWF.bProtocal = d_WIFI_PROTOCOL_WEP;
		   }
		   else if (strcmp(strTCP.szWifiProtocal, "WPA")  == 0)
		   {
			   stWF.bProtocal = d_WIFI_PROTOCOL_WPA;
		   }
		   else if (strcmp(strTCP.szWifiProtocal, "WPA2")  == 0)
		   {
			   stWF.bProtocal = 3;
		   }
		   
		   //Pairwise
		   if (strcmp(strTCP.szWifiPairwise, "TKIP")  == 0)
		   {
			   stWF.bPairwise = d_WIFI_PAIRWISE_TKIP;
		   }
		   else if (strcmp(strTCP.szWifiPairwise, "CCMP")  == 0)
		   {
			   stWF.bPairwise = d_WIFI_PAIRWISE_CCMP;
		   }
		   else if (strcmp(strTCP.szWifiPairwise, "CCMPTKI")  == 0)
		   {
			   stWF.bPairwise = d_WIFI_PAIRWISE_TKIPCCMP;
		   }
		   
		   //Group
		   if (strcmp(strTCP.szWifiGroup, "TKIP")  == 0)
		   {
			   stWF.bGroup = d_WIFI_GROUP_TKIP;
		   }
		   else if (strcmp(strTCP.szWifiGroup, "CCMP") == 0)
		   {
			   stWF.bGroup = d_WIFI_GROUP_CCMP;
		   }
		   else if (strcmp(strTCP.szWifiGroup, "CCMPTKI")  == 0)
		   {
			  stWF.bGroup = d_WIFI_GROUP_TKIPCCMP;
		   }
           
           usRes=CTOS_CTMSSetConfig(d_CTMS_WIFI_CONFIG , &stWF);

		   
				   vdDebug_LogPrintf("stWF.IsAutoConnect - %d", stWF.IsAutoConnect);
		   vdDebug_LogPrintf("stWF.IsHidden - %d", stWF.IsHidden);
		   vdDebug_LogPrintf("stWF.baPassword - %s", stWF.baPassword);
		   vdDebug_LogPrintf("stWF.baSSid - %s", stWF.baSSid);
		   vdDebug_LogPrintf("stWF.bProtocal - %d", stWF.bProtocal);
		   vdDebug_LogPrintf("stWF.bGroup - %d", stWF.bGroup);
		   vdDebug_LogPrintf("stWF.bPairwise - %d", stWF.bPairwise);
		   vdDebug_LogPrintf("stWF.strRemoteIP - %s", stWF.strRemoteIP);
		   vdDebug_LogPrintf("stWF.usRemotePort - %d", stWF.usRemotePort);
		   vdDebug_LogPrintf("stWF.bDHCP - %d", stWF.bDHCP);
		   vdDebug_LogPrintf("stWF.strLocalIP - %s", stWF.strLocalIP);
		   vdDebug_LogPrintf("stWF.strMask - %s", stWF.strMask);
		   vdDebug_LogPrintf("stWF.strGateway - %s", stWF.strGateway);
		   vdDebug_LogPrintf("stWF.bSCAN_Mode - %d", stWF.bSCAN_Mode);
    }


	setLCDPrint(8, DISPLAY_POSITION_LEFT, "ANY KEY TO CONTINUE");
	CTOS_KBDGet(&ckey);

	return usRes;
}

int inCTOSS_CheckIntervialDateFrom2013(int y,int m,int d)
{
    int x[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    long i,s=0;
    
    for(i=2013;i<y;i++)
    {
        if(i%4==0&&i%100!=0||i%400==0)
            s+=366;
        else 
            s+=365;
    }
            
    if(y%4==0&&y%100!=0||y%400==0)
        x[2]=29;
    
    for(i=1;i<m;i++)
        s+=x[i];
        
    s+=d;

    vdDebug_LogPrintf("Date[%ld]", s);
    return s;
}


int inCTOSS_SettlementCheckTMSDownloadRequest(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
    int inYear, inMonth, inDate,inDateGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;

	vdDebug_LogPrintf("--inCTOSS_SettlementCheckTMSDownloadRequest--");
    //adc
	if (get_env_int("ADLTYPE") != 1)	
		return d_OK;	
	//adc

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

	inTCTRead(1);

	vdDebug_LogPrintf("inTMSComMode[%d]", strTCT.inTMSComMode);
	vdDebug_LogPrintf("usTMSGap[%d]", strTCT.usTMSGap);
	
    //only default APP support TMS download
    vdDebug_LogPrintf("Check Default APP");
    if(inCTOSS_TMSCheckIfDefaultApplication() != d_OK)
        return d_NO;
	
    vdDebug_LogPrintf("Check Main APP");    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_NO;

	//if(DIAL_UP_MODE == strTCT.inTMSComMode)
		//return d_NO;
    //#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
    //USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo);
    usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &SetRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	//if(d_OK != usResult && d_CTMS_NO_INFO_DATA != usResult)
    //    return d_NO;


	if (d_OK == usResult)
	{
	    inYear = SetRTC.bYear;
	    inMonth = SetRTC.bMonth;
	    inDate = SetRTC.bDay;
	    CTOS_RTCGet(&SetRTC);

	    //inDateGap = inCTOSS_CheckIntervialDateFrom2013(SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013(inYear, inMonth, inDate);
		inDateGap = inCTOSS_CheckIntervialDateFrom2013((SetRTC.bYear+2000), SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013((inYear+2000), inMonth, inDate);
		vdDebug_LogPrintf("inDateGap=[%d],strTCT.usTMSGap=[%d]",inDateGap,strTCT.usTMSGap);

		if(inDateGap < strTCT.usTMSGap)
	        return d_NO;
	}

    //check if batch settle
    //should check all application?
    vdDebug_LogPrintf("Check Batch Empty");
    //if(inCTOSS_TMSChkBatchEmpty() != d_OK)
        //return d_NO;
    if(inCheckBatchEmtpy() > 0)
        return d_NO;	

    //check if TMS is downloading
    //vdDebug_LogPrintf("Check Get Status");
    //usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    //if (usResult == d_CTMS_UPDATE_FINISHED)
    //{
    //    strTCT.usTMSStatus = FALSE;
    //    inTCTSave(1);        
    //}
    //else
    //{
    //    return d_NO;
    //}
    //vdDebug_LogPrintf("Check Get Status %d %d", usStatus, usReterr);
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_NAC_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;
	else if(WIFI_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_WIFI;

    
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig");
    usResult = inCTOSS_TMSPreConfig2(usComType);
	if(usResult != d_OK)
        return d_NO;
	
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);

//test only
/*
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "TMS Download");
    CTOS_Delay(10000);
    return d_OK;
*/

    CTOS_CTMSUtility(usComType);
    
    /*usResult = CTOS_CTMSInitDaemonProcess(usComType);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]", usResult);
    
    usResult = CTOS_CTMSGetUpdateType(&st);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]st.bNums[%d]", usResult, st.bNums);
    if(usResult == d_OK && st.bNums > 0)
    {
        strTCT.usTMSStatus = TRUE;
        inTCTSave(1);
        
        CTOS_CTMSUtility(usComType);
    }*/

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    return d_OK;
}

int inCTOSS_CheckIfPendingTMSDownload(void)
{
    USHORT usResult;
    USHORT usStatus, usReterr;
    BYTE bDisplayBuf[50];
    USHORT i, loop = 0;
    USHORT usComType = d_CTMS_NORMAL_MODE;


    //if(FALSE == strTCT.usTMSStatus)
    //    return d_OK;

    CTOS_LCDTClearDisplay();

    if(ETHERNET_MODE == strCPT.inCommunicationMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strCPT.inCommunicationMode)
        usComType = d_CTMS_DEFAULT_MODEM;
    else if(GPRS_MODE == strCPT.inCommunicationMode)
        usComType = d_CTMS_DEFAULT_GPRS;


    usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    vdDebug_LogPrintf("CTOS_CTMSGetStatus usResult[%d] usStatus[%d] usReterr[%d]", usResult, usStatus, usReterr);
    if (usResult == d_CTMS_UPDATE_FINISHED)
    {
        //strTCT.usTMSStatus = FALSE;
        //inTCTSave(1);
            
        return d_OK;
    }
    else
    {
        CTOS_CTMSUtility(usComType);
    }
}

#if 0
int inCTOSS_TMSDownloadRequest(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
    int inYear, inMonth, inDate,inDateGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;

	CTOS_LCDTClearDisplay();
	vdDispTitleString("CTMS Init");
    CTOS_LCDTPrintXY (1,8, "Please Wait");
	
    //only default APP support TMS download
    vdDebug_LogPrintf("Check Default APP");
    if(inCTOSS_TMSCheckIfDefaultApplication() != d_OK)
        return d_NO;
    vdDebug_LogPrintf("Check Main APP");    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_NO;
	
    //#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
    //USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo);
    usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &SetRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	//if(d_OK != usResult && d_CTMS_NO_INFO_DATA != usResult)
    //    return d_NO;

/*
	if (d_OK == usResult)
	{
	    inYear = SetRTC.bYear;
	    inMonth = SetRTC.bMonth;
	    inDate = SetRTC.bDay;
	    CTOS_RTCGet(&SetRTC);

	    inDateGap = inCTOSS_CheckIntervialDateFrom2013(SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013(inYear, inMonth, inDate);
		vdDebug_LogPrintf("inDateGap=[%d],strTCT.usTMSGap=[%d]",inDateGap,strTCT.usTMSGap);

		if(inDateGap < strTCT.usTMSGap)
	        return d_NO;
	}
*/
    //check if batch settle
    //should check all application?
    //vdDebug_LogPrintf("Check Batch Empty");
    //if(inCheckBatchEmtpy() > 0)
    //	return d_NO;	
    if(inCheckBatchEmtpy() > 0)
    {
		vdDisplayErrorMsgResp2("PLEASE SETTLE","FIRST","AND TRY AGAIN");
        return d_NO;
    }

    //check if TMS is downloading
    //vdDebug_LogPrintf("Check Get Status");
    //usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    //if (usResult == d_CTMS_UPDATE_FINISHED)
    //{
    //    strTCT.usTMSStatus = FALSE;
    //    inTCTSave(1);        
    //}
    //else
    //{
    //    return d_NO;
    //}
    //vdDebug_LogPrintf("Check Get Status %d %d", usStatus, usReterr);
    
    inCTOS_TMSPreConfigSetting();

	
    
	//if(inCTOS_TMSPreConfigSetting() != d_OK)
	//{
        //vdDisplayErrorMsg(1, 8, "USER CANCEL");
        //return d_NO;		
	//}
	
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;
		else if(WIFI_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_WIFI;

    
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig");
    usResult = inCTOSS_TMSPreConfig(usComType);
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);

//test only
/*
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "TMS Download");
    CTOS_Delay(10000);
    return d_OK;
*/

    CTOS_CTMSUtility(usComType);
    
    /*usResult = CTOS_CTMSInitDaemonProcess(usComType);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]", usResult);
    
    usResult = CTOS_CTMSGetUpdateType(&st);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]st.bNums[%d]", usResult, st.bNums);
    if(usResult == d_OK && st.bNums > 0)
    {
        strTCT.usTMSStatus = TRUE;
        inTCTSave(1);
        
        CTOS_CTMSUtility(usComType);
    }*/

    return d_OK;
}
#else
int inCTOSS_TMSDownloadRequest(void)
{
	int inRet = d_NO;
   
	vdDebug_LogPrintf("--inCTOSS_TMSDownloadRequest--");

	vdCTOS_SetTransType(CTMS_UPDATE);
	inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
		return inRet;

	/*
	if(inCheckAllBatchEmtpy() > 0)
    {
		vdDisplayErrorMsgResp2("Please settle first","","and try again");
        return d_NO;
    }
    */

	vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(500);

	inCTOS_CTMSUPDATE();//after settle call CTMS update, inside fun will check batch status

	vdSetErrorMessage("");
	
    vdCTOS_TransEndReset();

	return d_OK;
}

#endif

void inCTOSS_TMS_USBUpgrade(void)
{
    char szSystemCmdPath[250];
    //char szSystemTime[50];
    //CTOS_RTC SetRTC;

	//CTOS_RTCGet(&SetRTC);
    //memset(szSystemTime, 0x00, sizeof(szSystemTime));
    //sprintf(szSystemTime, "%02d" ,SetRTC.bDay);
    
	vduiClearBelow(3);
	CTOS_LCDTPrintXY (1,7, "PLEASE WAIT"); 	  

    memset(szSystemCmdPath, 0x00, sizeof(szSystemCmdPath));                    
    //sprintf(szSystemCmdPath, "cp -f /media/mdisk/vxupdate/V5S_MCCCREDITx.prm .%s", PUBLIC_PATH);
    sprintf(szSystemCmdPath, "cp -f /home/ap/pub/menu.bmp ./media/mdisk/vxupdate/");
    system(szSystemCmdPath);	

    vdDebug_LogPrintf(szSystemCmdPath); 	
}


int inCTOSS_TMSPreConfig2(int inComType)
{    
    CTMS_GPRSInfo stgprs;    
    CTMS_ModemInfo stmodem;    
    CTMS_EthernetInfo st;
    int inNumOfRecords = 0;
    BYTE szSerialNum[17+1], szInputBuf[21+1];
    BYTE count = 2,i;
	BYTE tmpbuf[16 + 1];
	int len;
    unsigned char ckey;
	USHORT usStatusLine=8, usRes=0;
	CTMS_WIFIInfo stWF;
	USHORT usRet;
	BYTE szTemp[4+1];

	vdDebug_LogPrintf("--inCTOSS_TMSPreConfig--");

	inTCTRead(1);
	inTCPRead(1);
    inHDTRead(1);
    inMMTReadNumofRecords(strHDT.inHostIndex, &inNumOfRecords);

	vdDebug_LogPrintf("usTMSGap[%d]", strTCT.usTMSGap);
	vdDebug_LogPrintf("inTMSComMode[%d]", strTCT.inTMSComMode);
	vdDebug_LogPrintf("szTMSRemoteIP[%s]", strTCT.szTMSRemoteIP);
	vdDebug_LogPrintf("usTMSRemotePort[%d]", strTCT.usTMSRemotePort);
	vdDebug_LogPrintf("fDHCPEnable[%d]", strTCP.fDHCPEnable);
	vdDebug_LogPrintf("szTerminalIP[%s]", strTCP.szTerminalIP);
	vdDebug_LogPrintf("szWifiSSID[%s]", strTCP.szWifiSSID);
	vdDebug_LogPrintf("szWifiPassword[%s]", strTCP.szWifiPassword);
	vdDebug_LogPrintf("szAPN[%s]", strTCP.szAPN);
	vdDebug_LogPrintf("szUserName[%s]", strTCP.szUserName);
	vdDebug_LogPrintf("szPassword[%s]", strTCP.szPassword);
	vdDebug_LogPrintf("inSIMSlot[%d]", strTCP.inSIMSlot);
    
    memset(szSerialNum, 0x00, sizeof(szSerialNum));
	memset(tmpbuf, 0x00, sizeof(tmpbuf));

	CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
	if(strlen(szSerialNum) <= 0)
	    CTOS_GetFactorySN(szSerialNum);
	for (i=0; i<strlen(szSerialNum); i++)
	{
		if (szSerialNum[i] < 0x30 || szSerialNum[i] > 0x39)
		{
			szSerialNum[i] = 0;
			break;
		}
	}
	len = strlen(szSerialNum);
	strcpy(tmpbuf,"0000000000000000");
	memcpy(&tmpbuf[16-len],szSerialNum,len);
	vdDebug_LogPrintf("szSerialNum=[%s].tmpbuf=[%s]..",szSerialNum,tmpbuf);
	
    CTOS_CTMSSetConfig(d_CTMS_SERIALNUM, tmpbuf);//if TID is 12345678, SN is 0000000012345678
    CTOS_CTMSSetConfig(d_CTMS_RECOUNT, &count);

    if(strTCP.fDHCPEnable)
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, "0.0.0.0");
    else
        CTOS_CTMSSetConfig(d_CTMS_LOCALIP, strTCP.szTerminalIP);// If it is DHCP, also need to configure any value, otherwise please put the value from database

    if(ETHERNET_MODE == strTCT.inTMSComMode)
    {
        //CTOS_LCDTClearDisplay();
        //vdDispTitleString("ETHERNET SETTINGS");
	
    	vdDebug_LogPrintf("ETHERNET_MODE..");
        memset(&st, 0x00, sizeof (CTMS_EthernetInfo));
        
        strcpy(st.strGateway, strTCP.szGetWay);
        strcpy(st.strMask, strTCP.szSubNetMask);
        st.bDHCP = strTCP.fDHCPEnable;

        strcpy(st.strRemoteIP, strTCT.szTMSRemoteIP);
        st.usRemotePort = strTCT.usTMSRemotePort;

/*
		CTOS_LCDTClearDisplay();
		vdDispTitleString("ETHERNET SETTINGS");

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", st.strRemoteIP);         
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", st.usRemotePort);         
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
*/		
        usRes=CTOS_CTMSSetConfig(d_CTMS_ETHERNET_CONFIG, &st);
    }
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
    {
			CTMS_NacInfo stNAC;
	
			vdDebug_LogPrintf("DIAL_UP_MODE..");

			memset(&stNAC, 0x00, sizeof (stNAC));
			CTOS_CTMSGetConfig(d_CTMS_NAC_CONFIG, &stNAC);
	
			DebugAddHEX("stNAC.baSourceAddr", stNAC.baSourceAddr, 2);
			DebugAddHEX("stNAC.baDestAddr", stNAC.baDestAddr, 2);
	
			vdDebug_LogPrintf("stNAC.baSourceAddr[%s]..",stNAC.baSourceAddr);
			vdDebug_LogPrintf("stNAC.baDestAddr[%s]..",stNAC.baDestAddr);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_str_2_hex(strTCT.szTMSNACProtocol, szTemp, 2);		  
			stNAC.bProtocol=szTemp[0];
			
			stNAC.usBlockSize=1024;
	
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_str_2_hex(strTCT.szTMSNACSourceAddr, szTemp, 4);		
			memcpy(stNAC.baSourceAddr, szTemp, 2);
	
			memset(szTemp, 0x00, sizeof(szTemp));
			wub_str_2_hex(strTCT.szTMSNACDestAddr, szTemp, 4);				  
			memcpy(stNAC.baDestAddr, szTemp, 2);
	
			
			stNAC.bLenType=0;
			stNAC.bAddLenFlag=0;
			//vdDebug_LogPrintf("DIAL_UP_MODE..");
			
			usRes=CTOS_CTMSSetConfig(d_CTMS_NAC_CONFIG, &stNAC);
			
			//CTOS_LCDTClearDisplay();
			//vdDispTitleString("DIALUP SETTINGS");
	
			memset(&stmodem, 0x00, sizeof (CTMS_ModemInfo));
			CTOS_CTMSGetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
			
			//stmodem.bMode = d_M_MODE_AYNC_FAST;
			//stmodem.bHandShake = d_M_HANDSHAKE_V32BIS_AUTO_FB;
			stmodem.bMode  = d_M_MODE_SDLC_FAST;
			stmodem.bHandShake = d_M_HANDSHAKE_V22_ONLY;
			stmodem.bCountryCode = d_M_COUNTRY_SINGAPORE;
	
			strcpy(stmodem.strRemotePhoneNum, strTCT.szTMSRemotePhone);
			vdDebug_LogPrintf("strRemotePhoneNum[%s]..",stmodem.strRemotePhoneNum);
			strcpy(stmodem.strID, strTCT.szTMSRemoteID);	
			vdDebug_LogPrintf("strID[%s]..",stmodem.strID);
			strcpy(stmodem.strPW, strTCT.szTMSRemotePW);
			vdDebug_LogPrintf("strPW[%s]..",stmodem.strPW);
	
			strcpy(stmodem.strRemoteIP, strTCT.szTMSRemoteIP);
			stmodem.usRemotePort = strTCT.usTMSRemotePort;
	
			stmodem.usPPPRetryCounter = 2;
			stmodem.ulPPPTimeout = 34463;
			stmodem.ulDialTimeout = 34463;
			vdDebug_LogPrintf("strRemoteIP[%s].usRemotePort=[%d].",stmodem.strRemoteIP,stmodem.usRemotePort);
			stmodem.bType = d_CTMS_MODEM; //d_CTMS_TCP_MODEM, d_CTMS_MODEM
			stmodem.ulBaudRate = 115200;
			stmodem.bParity = 'N'; 
			stmodem.bDataBits = 8;
			stmodem.bStopBits = 1;
			stmodem.usDialDuration = 30;				// Dialing maximum total duration in second. Range: 0~255
			stmodem.usDTMFOnTime = 95;				 // DTMF on time in ms.
			stmodem.usDTMFOffTime = 95; 			   // DTMF off time in ms.	  
			stmodem.bTxPowerLevel = 3;				   // Tx Power Level(refer to Modem Functions)
			stmodem.bRxPowerLevel = 3;
			
			//memset(szSerialNum, 0x00, sizeof(szSerialNum));
			//CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
	
	/*
			memset(szInputBuf, 0x00, sizeof(szInputBuf));
			sprintf(szInputBuf, "SN: %s", szSerialNum); 		
			setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 
	
			memset(szInputBuf, 0x00, sizeof(szInputBuf));
			sprintf(szInputBuf, "IP: %s", stmodem.strRemoteIP); 		
			setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 
	
			memset(szInputBuf, 0x00, sizeof(szInputBuf));
			sprintf(szInputBuf, "PORT: %d", stmodem.usRemotePort);		   
			setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 
			
			memset(szInputBuf, 0x00, sizeof(szInputBuf));
			sprintf(szInputBuf, "PHONE: %s", stmodem.strRemotePhoneNum);		 
			setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf); 
			
			memset(szInputBuf, 0x00, sizeof(szInputBuf));
			sprintf(szInputBuf, "ID: %s", stmodem.strID);		  
			setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf); 
	
			memset(szInputBuf, 0x00, sizeof(szInputBuf));
			sprintf(szInputBuf, "PASSWORD: %s", stmodem.strPW); 		
			setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
	*/
			usRes=CTOS_CTMSSetConfig(d_CTMS_MODEM_CONFIG, &stmodem);
		}
    else if(GPRS_MODE == strTCT.inTMSComMode)
    {
		CTMS_GPRSInfo stgprs;

        //CTOS_LCDTClearDisplay();
        //vdDispTitleString("GPRS SETTINGS");
		
		memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
		CTOS_CTMSGetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
		
        if(strlen(stgprs.strAPN) > 0)
            strcpy(strTCP.szAPN, stgprs.strAPN);

        if(strlen(stgprs.strID) > 0)
            strcpy(strTCP.szUserName, stgprs.strID);

        if(strlen(stgprs.strPW) > 0)
		    strcpy(strTCP.szPassword, stgprs.strPW);
		
              vdDebug_LogPrintf("GPRS_MODE..");
        memset(&stgprs, 0x00, sizeof (CTMS_GPRSInfo));
        strcpy(stgprs.strAPN, strTCP.szAPN);
        strcpy(stgprs.strID, strTCP.szUserName);
        strcpy(stgprs.strPW, strTCP.szPassword);
 
        strcpy(stgprs.strRemoteIP, strTCT.szTMSRemoteIP);
        stgprs.usRemotePort = strTCT.usTMSRemotePort;
        stgprs.ulSIMReadyTimeout = 10000;
        stgprs.ulGPRSRegTimeout = 10000;
        stgprs.usPPPRetryCounter = 5;
        stgprs.ulPPPTimeout = 10000;
        stgprs.ulTCPConnectTimeout = 10000;

		//test
		stgprs.ulTCPTxTimeout=10000;
		stgprs.ulTCPRxTimeout=10000;
		stgprs.bSIMSlot=1;

		memset(szSerialNum, 0x00, sizeof(szSerialNum));
		CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);

/*
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "SN: %s", szSerialNum);         
        setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "IP: %s", stgprs.strRemoteIP);		 
        setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PORT: %d", stgprs.usRemotePort);		 
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "APN: %s", stgprs.strAPN);		 
        setLCDPrint(5, DISPLAY_POSITION_LEFT, szInputBuf);

        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "USER NAME: %s", stgprs.strID);		 
        setLCDPrint(6, DISPLAY_POSITION_LEFT, szInputBuf);
		
        memset(szInputBuf, 0x00, sizeof(szInputBuf));
        sprintf(szInputBuf, "PASSWORD: %s", stgprs.strPW);		 
        setLCDPrint(7, DISPLAY_POSITION_LEFT, szInputBuf);
*/		
        usRes=CTOS_CTMSSetConfig(d_CTMS_GPRS_CONFIG, &stgprs);
    }

/*
	setLCDPrint(8, DISPLAY_POSITION_LEFT, "ANY KEY TO CONTINUE");
	CTOS_KBDGet(&ckey);
*/

    else if(WIFI_MODE == strTCT.inTMSComMode)
    {
        CTOS_LCDTClearDisplay();
        vdDispTitleString("WIFI SETTINGS");
        
        vdDebug_LogPrintf("WIFI..");
        memset(&stWF, 0x00, sizeof (CTMS_WIFIInfo));
        usRet = CTOS_CTMSGetConfig(d_CTMS_WIFI_CONFIG, &stWF);


		strcpy(stWF.strRemoteIP, strTCT.szTMSRemoteIP);
	    stWF.usRemotePort = strTCT.usTMSRemotePort;

		strcpy(stWF.baPassword, strTCP.szWifiPassword);
		strcpy(stWF.baSSid, strTCP.szWifiSSID);
		//stWF.bProtocal = 3;
		//stWF.bGroup = 2;
		//stWF.bPairwise = 2;
		stWF.bDHCP = strTCP.fDHCPEnable;
		stWF.bSCAN_Mode = 1;
		stWF.IsAutoConnect =48;
		stWF.IsHidden =48;

		memset(stWF.strLocalIP, 0x00, sizeof(stWF.strLocalIP));
		memset(stWF.strMask, 0x00, sizeof(stWF.strMask));	
		memset(stWF.strGateway, 0x00, sizeof(stWF.strGateway));

		if (strTCP.fDHCPEnable){
			strcpy(stWF.strLocalIP, "0.0.0.0");
			strcpy(stWF.strMask, "0.0.0.0");
			strcpy(stWF.strGateway, "0.0.0.0");
		}else{
			strcpy(stWF.strLocalIP, strTCP.szTerminalIP);
			strcpy(stWF.strMask, strTCP.szSubNetMask);
			strcpy(stWF.strGateway, strTCP.szGetWay);
		}
	
		
        //if (usRet != d_OK)
        //{
        //   CTOS_LCDTPrintXY(1, 7, "Please Set CTMS");
        //   vdDisplayErrorMsg(1, 8, "CTMS Get Fail");
        //   return d_NO;
       // }
           CTOS_LCDTClearDisplay();
           vdDispTitleString("WIFI SETTINGS");
        
           memset(szSerialNum, 0x00, sizeof(szSerialNum));
           CTOS_CTMSGetConfig(d_CTMS_SERIALNUM, szSerialNum);
           
           memset(szInputBuf, 0x00, sizeof(szInputBuf));
           sprintf(szInputBuf, "SN: %s", szSerialNum);         
           setLCDPrint(2, DISPLAY_POSITION_LEFT, szInputBuf); 
           
           memset(szInputBuf, 0x00, sizeof(szInputBuf));
           sprintf(szInputBuf, "IP: %s", stWF.strRemoteIP);         
           setLCDPrint(3, DISPLAY_POSITION_LEFT, szInputBuf); 
           
           memset(szInputBuf, 0x00, sizeof(szInputBuf));
           sprintf(szInputBuf, "PORT: %d", stWF.usRemotePort);         
           setLCDPrint(4, DISPLAY_POSITION_LEFT, szInputBuf); 


		   //bProtocal
		   if (strcmp(strTCP.szWifiProtocal, "WEP")  == 0)
		   {
			   stWF.bProtocal = d_WIFI_PROTOCOL_WEP;
		   }
		   else if (strcmp(strTCP.szWifiProtocal, "WPA")  == 0)
		   {
			   stWF.bProtocal = d_WIFI_PROTOCOL_WPA;
		   }
		   else if (strcmp(strTCP.szWifiProtocal, "WPA2")  == 0)
		   {
			   stWF.bProtocal = 3;
		   }
		   
		   //Pairwise
		   if (strcmp(strTCP.szWifiPairwise, "TKIP")  == 0)
		   {
			   stWF.bPairwise = d_WIFI_PAIRWISE_TKIP;
		   }
		   else if (strcmp(strTCP.szWifiPairwise, "CCMP")  == 0)
		   {
			   stWF.bPairwise = d_WIFI_PAIRWISE_CCMP;
		   }
		   else if (strcmp(strTCP.szWifiPairwise, "CCMPTKI")  == 0)
		   {
			   stWF.bPairwise = d_WIFI_PAIRWISE_TKIPCCMP;
		   }
		   
		   //Group
		   if (strcmp(strTCP.szWifiGroup, "TKIP")  == 0)
		   {
			   stWF.bGroup = d_WIFI_GROUP_TKIP;
		   }
		   else if (strcmp(strTCP.szWifiGroup, "CCMP") == 0)
		   {
			   stWF.bGroup = d_WIFI_GROUP_CCMP;
		   }
		   else if (strcmp(strTCP.szWifiGroup, "CCMPTKI")  == 0)
		   {
			  stWF.bGroup = d_WIFI_GROUP_TKIPCCMP;
		   }
           
           usRes=CTOS_CTMSSetConfig(d_CTMS_WIFI_CONFIG , &stWF);

		   	vdDebug_LogPrintf("stWF.IsAutoConnect - %d", stWF.IsAutoConnect);
	vdDebug_LogPrintf("stWF.IsHidden - %d", stWF.IsHidden);
	vdDebug_LogPrintf("stWF.baPassword - %s", stWF.baPassword);
	vdDebug_LogPrintf("stWF.baSSid - %s", stWF.baSSid);
	vdDebug_LogPrintf("stWF.bProtocal - %d", stWF.bProtocal);
	vdDebug_LogPrintf("stWF.bGroup - %d", stWF.bGroup);
	vdDebug_LogPrintf("stWF.bPairwise - %d", stWF.bPairwise);
	vdDebug_LogPrintf("stWF.strRemoteIP - %s", stWF.strRemoteIP);
	vdDebug_LogPrintf("stWF.usRemotePort - %d", stWF.usRemotePort);
	vdDebug_LogPrintf("stWF.bDHCP - %d", stWF.bDHCP);
	vdDebug_LogPrintf("stWF.strLocalIP - %s", stWF.strLocalIP);
	vdDebug_LogPrintf("stWF.strMask - %s", stWF.strMask);
	vdDebug_LogPrintf("stWF.strGateway - %s", stWF.strGateway);
	vdDebug_LogPrintf("stWF.bSCAN_Mode - %d", stWF.bSCAN_Mode);
    }

	return usRes;
}


//adc
int inCTOSS_ADLSettlementCheckTMSDownloadRequest(void)
{
    
    CTMS_UpdateInfo st;
    CTOS_RTC SetRTC;
    int inYear, inMonth, inDate,inDateGap;
    USHORT usStatus, usReterr;
    USHORT usResult;
    USHORT usComType = d_CTMS_NORMAL_MODE;

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    //only default APP support TMS download
    vdDebug_LogPrintf("Check Default APP");
    if(inCTOSS_TMSCheckIfDefaultApplication() != d_OK)
        return d_NO;
    vdDebug_LogPrintf("Check Main APP");    
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_NO;

	//if(DIAL_UP_MODE == strTCT.inTMSComMode)
		//return d_NO;
    //#define d_CTMS_INFO_LAST_UPDATE_TIME 0x01
    //USHORT CTOS_CTMSGetInfo(IN BYTE Info_ID, IN void *stInfo);
    usResult = CTOS_CTMSGetInfo(d_CTMS_INFO_LAST_UPDATE_TIME, &SetRTC);
	vdDebug_LogPrintf("CTOS_CTMSGetInfo usResult=[%x]",usResult);
	//if(d_OK != usResult && d_CTMS_NO_INFO_DATA != usResult)
    //    return d_NO;

	if (inADLTimeRangeUsed == 1)
		put_env_int("ADLTRY1",1);	
	else if (inADLTimeRangeUsed == 2)
		put_env_int("ADLTRY2",1);	
	else if (inADLTimeRangeUsed == 3)
		put_env_int("ADLTRY3",1);	


	if (d_OK == usResult)
	{
	    inYear = SetRTC.bYear;
	    inMonth = SetRTC.bMonth;
	    inDate = SetRTC.bDay;
	    CTOS_RTCGet(&SetRTC);

	    //inDateGap = inCTOSS_CheckIntervialDateFrom2013(SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013(inYear, inMonth, inDate);

		inDateGap = inCTOSS_CheckIntervialDateFrom2013((SetRTC.bYear+2000), SetRTC.bMonth, SetRTC.bDay) - inCTOSS_CheckIntervialDateFrom2013((inYear+2000), inMonth, inDate);
		 

		vdDebug_LogPrintf("inDateGap=[%d],strTCT.usTMSGap=[%d]",inDateGap,strTCT.usTMSGap);

		if(inDateGap < strTCT.usTMSGap)
	        return d_NO;
	}


    //check if batch settle
    //should check all application?
    //vdDebug_LogPrintf("Check Batch Empty");
    //if(inCTOSS_TMSChkBatchEmpty() != d_OK)
        //return d_NO;
    //if(inCheckBatchEmtpy() > 0)
    //    return d_NO;	

    //check if TMS is downloading
    //vdDebug_LogPrintf("Check Get Status");
    //usResult = CTOS_CTMSGetStatus(&usStatus, &usReterr);
    //if (usResult == d_CTMS_UPDATE_FINISHED)
    //{
    //    strTCT.usTMSStatus = FALSE;
    //    inTCTSave(1);        
    //}
    //else
    //{
    //    return d_NO;
    //}
    //vdDebug_LogPrintf("Check Get Status %d %d", usStatus, usReterr);
    if(ETHERNET_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_ETHERNET;
    else if(DIAL_UP_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_NAC_DEFAULT_MODEM;
    else if(GPRS_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_GPRS;
	else if(WIFI_MODE == strTCT.inTMSComMode)
        usComType = d_CTMS_DEFAULT_WIFI;
	
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig");
	CTOS_LCDTClearDisplay();
    usResult = inCTOSS_TMSPreConfig2(usComType);
    vdDebug_LogPrintf("inCTOSS_TMSPreConfig ret[%d] usComType[%d]", usResult, strTCT.inTMSComMode);

//test only
/*
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "TMS Download");
    CTOS_Delay(10000);
    return d_OK;
*/

    CTOS_CTMSUtility(usComType);
    
    /*usResult = CTOS_CTMSInitDaemonProcess(usComType);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]", usResult);
    
    usResult = CTOS_CTMSGetUpdateType(&st);
    vdDebug_LogPrintf("CTOS_CTMSInitDaemonProcess ret[%d]st.bNums[%d]", usResult, st.bNums);
    if(usResult == d_OK && st.bNums > 0)
    {
        strTCT.usTMSStatus = TRUE;
        inTCTSave(1);
        
        CTOS_CTMSUtility(usComType);
    }*/

	inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    return d_OK;
}

//adc


int inCTOS_CTMSUPDATE(void)
{
    BYTE szInbuf[1024 + 1];
    BYTE szOubuf[1024 + 1];
    int inLen;
	int inRet = d_NO;
	BYTE szTMSHostPort[6+1];
	BYTE szTLSHostPort[6+1];
	BYTE szCurrDate[8] = {0};
	BYTE szCommMode[6+1];
	CTOS_RTC SetRTC;

    vdDebug_LogPrintf("saturn =====1.inCTOS_CTMSUPDATE=====.");

	inTCTRead(1);
    inTMSEXRead(1);
	inCPTRead(1);

	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;
	
	// validate serial num -- sidumili
	if (strlen(strTMSEX.szSerialNo) < SERIAL_NUM_LEN || atoi(strTMSEX.szSerialNo) <= 0)
	{
		inDisplayMessageBoxWithButtonConfirmation(1,8,strTMSEX.szSerialNo,"Invalid serial number","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		return d_NO;
	}

	// szTMSHostIP/inTMSHostPortNum
	// checking for alive connection -- sidumili
	vdSetInit_Connect(1);
	inRet = inCTOS_CTMS_Connect(1);
	vdDebug_LogPrintf("inCTOS_CTMS_Connect,szTMSHostIP/inTMSHostPortNum,inRet=[%d]", inRet);
	if (inRet != d_OK)
	{
		vdCTOS_TransEndReset();
		return d_NO;
	}

	// szTLSHostIP/inTLSHostPortNum
	// checking for alive connection -- sidumili
	vdSetInit_Connect(1);
	inRet = inCTOS_CTMS_Connect(2);
	vdDebug_LogPrintf("inCTOS_CTMS_Connect,szTLSHostIP/inTLSHostPortNum,inRet=[%d]", inRet);
	if (inRet != d_OK)
	{
		vdCTOS_TransEndReset();
		return d_NO;
	}
	
	inRet = inCTOS_ChkBatchEmpty_AllHosts();
	vdDebug_LogPrintf("inCTOS_ChkBatchEmpty_AllHosts, inRet=[%d]", inRet);
	if (d_OK != inRet) {
        vdDebug_LogPrintf("saturn =====inCTOS_CTMSUPDATE Batch not Empty=====");
		vdCTOS_TransEndReset();
        return inRet;
	}

	CTOS_RTCGet(&SetRTC);
	memset(szCurrDate, 0x00, sizeof(szCurrDate));
    sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
	vdDebug_LogPrintf("AAA>> inCTOS_CTMSUPDATE szCurrDate[%s]", szCurrDate);
    put_env_char("AUTODLDATE",szCurrDate);
	
	vdDebug_LogPrintf("strTMSEX.szTMSMode=[%s]", strTMSEX.szTMSMode);
	vdDebug_LogPrintf("strTMSEX.szSerialNo=[%s]", strTMSEX.szSerialNo);
	vdDebug_LogPrintf("strTMSEX.szTMSHostIP/inTLSHostPortNum=[%s],[%ld]", strTMSEX.szTMSHostIP, strTMSEX.inTLSHostPortNum);
	vdDebug_LogPrintf("strTMSEX.szTLSHostIP/inTLSHostPortNum=[%s],[%ld]", strTMSEX.szTLSHostIP, strTMSEX.inTLSHostPortNum);

	memset(szInbuf, 0x00, sizeof(szInbuf));
	strcpy(szInbuf, strTMSEX.szSerialNo);
	strcat(szInbuf, "|");
	strcat(szInbuf, strTMSEX.szTMSHostIP);
	strcat(szInbuf, "|");

	memset(szTMSHostPort, 0x00,sizeof(szTMSHostPort));
	sprintf(szTMSHostPort, "%d", strTMSEX.inTMSHostPortNum);	
	strcat(szInbuf, szTMSHostPort);
	
	strcat(szInbuf, "|");
	strcat(szInbuf, strTMSEX.szTLSHostIP);
	strcat(szInbuf, "|");

	memset(szTLSHostPort, 0x00,sizeof(szTLSHostPort));
	sprintf(szTLSHostPort, "%d", strTMSEX.inTLSHostPortNum);	
	strcat(szInbuf, szTLSHostPort);
	
	strcat(szInbuf, "|");
	strcat(szInbuf, strTMSEX.szTMSMode);

	strcat(szInbuf, "|");
	memset(szCommMode, 0x00,sizeof(szCommMode));
	sprintf(szCommMode, "%d", strCPT.inCommunicationMode);	
	strcat(szInbuf, szCommMode);

	vdDebug_LogPrintf("len=[%d],szInbuf=[%s]", strlen(szInbuf), szInbuf);
	
    vdDebug_LogPrintf("saturn End.=====inCTOS_CTMSUPDATE=====");
	vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(500);
	
    inRet = inCallJAVA_CTMSUPDATE(szInbuf, szOubuf, &inLen);
	vdDebug_LogPrintf("inCallJAVA_CTMSUPDATE, inRet=[%d]", inRet);

    return d_OK;
}

int inCTOS_CTMSUPDATE_BackGround(void)
{
    BYTE szInbuf[1024 + 1];
    BYTE szOubuf[1024 + 1];
    int inLen;
    int inRet = d_NO;
    BYTE szTMSHostPort[6+1];
    BYTE szTLSHostPort[6+1];
    BYTE szCurrDate[8] = {0};
    BYTE szCommMode[6+1];
    CTOS_RTC SetRTC;

    vdDebug_LogPrintf("saturn =====1.inCTOS_CTMSUPDATE_BackGround=====.");

    inTCTRead(1);
    inTMSEXRead(1);
    inCPTRead(1);

    inRet = inCheckBattery();
    if(d_OK != inRet)
        return inRet;

    // validate serial num -- sidumili
    if (strlen(strTMSEX.szSerialNo) < SERIAL_NUM_LEN || atoi(strTMSEX.szSerialNo) <= 0)
    {
        inDisplayMessageBoxWithButtonConfirmation(1,8,strTMSEX.szSerialNo,"Invalid serial number","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
        return d_NO;
    }

    // szTMSHostIP/inTMSHostPortNum
    // checking for alive connection -- sidumili
    vdSetInit_Connect(1);
    inRet = inCTOS_CTMS_Connect(1);
    vdDebug_LogPrintf("inCTOS_CTMS_Connect,szTMSHostIP/inTMSHostPortNum,inRet=[%d]", inRet);
    if (inRet != d_OK)
    {
        vdCTOS_TransEndReset();
        return d_NO;
    }

    // szTLSHostIP/inTLSHostPortNum
    // checking for alive connection -- sidumili
    vdSetInit_Connect(1);
    inRet = inCTOS_CTMS_Connect(2);
    vdDebug_LogPrintf("inCTOS_CTMS_Connect,szTLSHostIP/inTLSHostPortNum,inRet=[%d]", inRet);
    if (inRet != d_OK)
    {
        vdCTOS_TransEndReset();
        return d_NO;
    }

    inRet = inCTOS_ChkBatchEmpty_AllHosts();
    vdDebug_LogPrintf("inCTOS_ChkBatchEmpty_AllHosts, inRet=[%d]", inRet);
    if (d_OK != inRet) {
        vdDebug_LogPrintf("saturn =====inCTOS_CTMSUPDATE Batch not Empty=====");
        vdCTOS_TransEndReset();
        return inRet;
    }

    CTOS_RTCGet(&SetRTC);
    memset(szCurrDate, 0x00, sizeof(szCurrDate));
    sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
    vdDebug_LogPrintf("AAA>> inCTOS_CTMSUPDATE szCurrDate[%s]", szCurrDate);
    put_env_char("AUTODLDATE",szCurrDate);

    vdDebug_LogPrintf("strTMSEX.szTMSMode=[%s]", strTMSEX.szTMSMode);
    vdDebug_LogPrintf("strTMSEX.szSerialNo=[%s]", strTMSEX.szSerialNo);
    vdDebug_LogPrintf("strTMSEX.szTMSHostIP/inTLSHostPortNum=[%s],[%ld]", strTMSEX.szTMSHostIP, strTMSEX.inTLSHostPortNum);
    vdDebug_LogPrintf("strTMSEX.szTLSHostIP/inTLSHostPortNum=[%s],[%ld]", strTMSEX.szTLSHostIP, strTMSEX.inTLSHostPortNum);

    memset(szInbuf, 0x00, sizeof(szInbuf));
    strcpy(szInbuf, strTMSEX.szSerialNo);
    strcat(szInbuf, "|");
    strcat(szInbuf, strTMSEX.szTMSHostIP);
    strcat(szInbuf, "|");

    memset(szTMSHostPort, 0x00,sizeof(szTMSHostPort));
    sprintf(szTMSHostPort, "%d", strTMSEX.inTMSHostPortNum);
    strcat(szInbuf, szTMSHostPort);

    strcat(szInbuf, "|");
    strcat(szInbuf, strTMSEX.szTLSHostIP);
    strcat(szInbuf, "|");

    memset(szTLSHostPort, 0x00,sizeof(szTLSHostPort));
    sprintf(szTLSHostPort, "%d", strTMSEX.inTLSHostPortNum);
    strcat(szInbuf, szTLSHostPort);

    strcat(szInbuf, "|");
    strcat(szInbuf, strTMSEX.szTMSMode);

    strcat(szInbuf, "|");
    memset(szCommMode, 0x00,sizeof(szCommMode));
    sprintf(szCommMode, "%d", strCPT.inCommunicationMode);
    strcat(szInbuf, szCommMode);

    vdDebug_LogPrintf("len=[%d],szInbuf=[%s]", strlen(szInbuf), szInbuf);

    vdDebug_LogPrintf("saturn End.=====inCTOS_CTMSUPDATE=====");
    vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
    CTOS_Delay(500);

    //inRet = inCallJAVA_CTMSUPDATE(szInbuf, szOubuf, &inLen);
    inRet = inCallJAVA_CTMSUPDATEBackGround(szInbuf, szOubuf, &inLen);
    
    vdDebug_LogPrintf("inCallJAVA_CTMSUPDATE, inRet=[%d]", inRet);

    return d_OK;
}

// CTMS connection checking -- sidumili
int inCTOS_CTMS_Connect(int index)
{
	int inRetVal = d_NO;
	char szTemp[40 + 1] = {0};

	vdDebug_LogPrintf("--inCTOS_CTMS_Connect--");
	vdDebug_LogPrintf("index=[%d]", index);

	inCPTRead(1);
	inTMSEXRead(1); // read table

	vdDebug_LogPrintf("->inTMSEXRead-----");
	vdDebug_LogPrintf("strTMSEX.szTMSID=[%s]", strTMSEX.szTMSID);
	vdDebug_LogPrintf("strTMSEX.szSerialNo=[%s]", strTMSEX.szSerialNo);
	vdDebug_LogPrintf("strTMSEX.szGroupName=[%s]", strTMSEX.szGroupName);
	vdDebug_LogPrintf("strTMSEX.szTMSHostIP=[%s]", strTMSEX.szTMSHostIP);
	vdDebug_LogPrintf("strTMSEX.inTMSHostPortNum=[%ld]", strTMSEX.inTMSHostPortNum);
	vdDebug_LogPrintf("strTMSEX.szUserName=[%s]", strTMSEX.szUserName);
	vdDebug_LogPrintf("strTMSEX.szPassword=[%s]", strTMSEX.szPassword);
	vdDebug_LogPrintf("strTMSEX.szTLSHostIP=[%s]", strTMSEX.szTLSHostIP);
	vdDebug_LogPrintf("strTMSEX.inTLSHostPortNum=[%ld]", strTMSEX.inTLSHostPortNum);
	vdDebug_LogPrintf("strTMSEX.szTMSMode=[%s]", strTMSEX.szTMSMode);
	vdDebug_LogPrintf("->inTMSEXRead-----");

	vdDebug_LogPrintf("->inCPTRead-----");
	vdDebug_LogPrintf("strCPT.inCommunicationMode=[%d]", strCPT.inCommunicationMode);
	vdDebug_LogPrintf("->inCPTRead-----");

	srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;

	switch (index)
	{
		case 1:
			//Trxn
			strcpy(strCPT.szPriTxnHostIP,strTMSEX.szTMSHostIP);
			strcpy(strCPT.szSecTxnHostIP,strTMSEX.szTMSHostIP);
			strCPT.inPriTxnHostPortNum = strTMSEX.inTMSHostPortNum;
			strCPT.inSecTxnHostPortNum = strTMSEX.inTMSHostPortNum;

			//Settle
			strcpy(strCPT.szPriSettlementHostIP,strTMSEX.szTMSHostIP);
			strcpy(strCPT.szSecSettlementHostIP,strTMSEX.szTMSHostIP);
			strCPT.inPriSettlementHostPort= strTMSEX.inTMSHostPortNum;
			strCPT.inSecSettlementHostPort = strTMSEX.inTMSHostPortNum;
			break;
		case 2:
			
			//Trxn
			strcpy(strCPT.szPriTxnHostIP,strTMSEX.szTLSHostIP);
			strcpy(strCPT.szSecTxnHostIP,strTMSEX.szTLSHostIP);
			strCPT.inPriTxnHostPortNum = strTMSEX.inTLSHostPortNum;
			strCPT.inSecTxnHostPortNum = strTMSEX.inTLSHostPortNum;

			//Settle
			strcpy(strCPT.szPriSettlementHostIP,strTMSEX.szTLSHostIP);
			strcpy(strCPT.szSecSettlementHostIP,strTMSEX.szTLSHostIP);
			strCPT.inPriSettlementHostPort= strTMSEX.inTLSHostPortNum;
			strCPT.inSecSettlementHostPort = strTMSEX.inTLSHostPortNum;
			break;
	}
	
	vdDebug_LogPrintf("CTMS->usTerminalCommunicationMode=[%ld],inCommunicationMode=[%ld]", srTransRec.usTerminalCommunicationMode, strCPT.inCommunicationMode);
	vdDebug_LogPrintf("CTMS->Primary=[%s],[%ld]", strCPT.szPriTxnHostIP, strCPT.inPriTxnHostPortNum);
	vdDebug_LogPrintf("CTMS->Secondary=[%s],[%ld]", strCPT.szSecTxnHostIP, strCPT.inSecTxnHostPortNum);

	inRetVal = inCTOS_InitComm(srTransRec.usTerminalCommunicationMode);
	vdDebug_LogPrintf("inCTOS_InitComm, inRetVal=[%d],index=[%d]", inRetVal, index);
	if (inRetVal!= d_OK)
    {
        //vdSetErrorMessage("COMM INIT ERR");
        //vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
        vdDisplayErrorMsgResp2("","Initialization","Error");
        vdSetErrorMessage("");
        return(d_NO);
    }

    if(VS_TRUE == strTCT.fDemo)
        return(d_OK);

    inRetVal = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);
	vdDebug_LogPrintf("inCTOS_CheckInitComm = [%d], index=[%d]", inRetVal, index);

    if (inRetVal != d_OK)
    {
        if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "GPRS Problem","Please Try Again");            
        }
            //wifi-mod2
        else if (srTransRec.usTerminalCommunicationMode == WIFI_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "WIFI Problem","Please Try Again");
        }
            //wifi-mod2
        else if (srTransRec.usTerminalCommunicationMode == ETHERNET_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "Check LAN","Connectivity");
        }
        else if (srTransRec.usTerminalCommunicationMode == DIAL_UP_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "Check Phone Line", "Connectivity");
        }
        else
        {
            vdDisplayErrorMsgResp2(" ", "Connect Failed","Please Try Again");
        }

		vdSetErrorMessage("");
		
		DisplayStatusLine("");
				
        return(d_NO);
    }

	if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
    {
    	memset(szTemp, 0x00, sizeof(szTemp));
		sprintf(szTemp, "%s:%ld", strCPT.szPriTxnHostIP, strCPT.inPriTxnHostPortNum);
        inCTOS_inDisconnect();
        vdDisplayErrorMsgResp2("CTMS", "Connect Failed",szTemp);

		DisplayStatusLine("");
		return(d_NO);
    }
	
	return d_OK;
}

