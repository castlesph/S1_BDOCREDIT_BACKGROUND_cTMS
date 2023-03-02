#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <ctosapi.h>

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <sys/shm.h>
#include <linux/errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "..\ApTrans\MultiApTrans.h"
#include "..\Database\DatabaseFunc.h"

#include "..\Includes\POSTypedef.h"
#include "..\Includes\Wub_lib.h"

#include "..\Debug\Debug.h"

#include "..\ApTrans\MultiShareCOM.h"
#include "..\ui\Display.h"
#include "..\Comm\V5Comm.h"
#include "MultiAptrans.h"
#include "../Debug/debug.h"
#include "../Includes/POSTypedef.h"
#include "MultiShareCOM.h"
#include "../Ui/Display.h"
#include "../FileModule/myFileFunc.h"
#include "../Includes/POSSetting.h"

static int gCommMode = -1;//for Sharls_COM modem
BOOL fGPRSConnectOK = 0;
USHORT usHWSupport = 0;
static USHORT usEthType = 0;

#define COMM_HEADER_FILE "/data/data/pub/commheader.txt"
#define COMM_BUFF_FILE "/data/data/pub/commbuff.txt"
#define COMM_EXT_FILE "/data/data/pub/commext.txt"

#define SEND_BUF	(20*1024)
#define RECEIVE_BUF	(50*1024) 

#define MAX_BUF	6000

#define d_KBD_INVALID_COM						0xFE
#define	MY_CA_FILE				"/data/data/pub/CACERT.PEM"
#define ENV_MY_SSL_VER			"VMSSLVER"
#define		SSL_CLI_AUTH_VAR			"SSL_CLI_AUTH"


BYTE szIP[100+1];
BYTE szDNS1[100+1];
BYTE szDNS2[100+1];
BYTE szGateWay[100+1];
BYTE szSubnetMask[100+1];


USHORT usPreconnectStatus = 0;

BYTE szSendData[SEND_BUF];
BYTE szReceiveData[RECEIVE_BUF];

static int init_connect = 0;

int g_inCURLConnect = 0;

extern BOOL fSettleTrans;

extern fAdviceTras;

BYTE chGetInit_Connect(void)
{
    return init_connect;
}

void vdSetInit_Connect(int Init_Connect)
{
    init_connect = Init_Connect;
}


static long inCTOSS_COMM_GetFileSize(const char* pchFileName)
{
    FILE  *fPubKey;
	long curpos,length;

	vdDebug_LogPrintf("lnGetFileSize[%s]", pchFileName);
	fPubKey = fopen( (char*)pchFileName, "rb" );
	vdDebug_LogPrintf("fPubKey[%x]", fPubKey);
	if(fPubKey == NULL)
		return -1;

	curpos=ftell(fPubKey);
	fseek(fPubKey,0L,SEEK_END);
	length=ftell(fPubKey);
	fseek(fPubKey,curpos,SEEK_SET);

	fclose(fPubKey);
	vdDebug_LogPrintf("lnGetFileSize[%d],length=[%d]", curpos,length);

    return(length);
}


static int inCTOSS_COMM_WriteFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	int times,i;
	
	vdDebug_LogPrintf("inWriteFile[%s],inMaxRecSize=[%d]", pchFileName,inMaxRecSize);
	
	fPubKey = fopen((char *)pchFileName, "wb+" );
	if(fPubKey == NULL)
		return -1;

	if (inMaxRecSize > MAX_BUF)
	{
		times = inMaxRecSize/MAX_BUF;
		for (i = 0;i<times;i++)
		{
			inRetVal = fwrite(&pchRecBuf[i*MAX_BUF],MAX_BUF, 1, fPubKey);
		}

		times = inMaxRecSize%MAX_BUF;
		if(times>0)
		{
			inRetVal = fwrite(&pchRecBuf[i*MAX_BUF],times, 1, fPubKey);
		}
		
	}
	else
	inRetVal = fwrite(pchRecBuf,inMaxRecSize, 1, fPubKey);
	fclose(fPubKey);

	chmod((char *)pchFileName, 0777);

	vdDebug_LogPrintf("inWriteFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}

static int inCTOSS_COMM_ReadFile(unsigned char *pchFileName, unsigned char *pchRecBuf, int inMaxRecSize)
{
	int h_file;
	int inRetVal = 0;
	FILE  *fPubKey;
	int times,i;
	
	vdDebug_LogPrintf("ReadFile[%s],inMaxRecSize=[%d]", pchFileName,inMaxRecSize);
	
	fPubKey = fopen((char *)pchFileName, "rb" );
	if(fPubKey == NULL)
		return -1;

	if (inMaxRecSize > MAX_BUF)
	{
		times = inMaxRecSize/MAX_BUF;
		for (i = 0;i<times;i++)
		{
			inRetVal = fread (&pchRecBuf[i*MAX_BUF], 1, MAX_BUF, fPubKey);
		}

		times = inMaxRecSize%MAX_BUF;
		if(times>0)
		{
			inRetVal = fread (&pchRecBuf[i*MAX_BUF], 1, times, fPubKey);
		}
		
	}
	else
	inRetVal = fread (pchRecBuf, 1, inMaxRecSize, fPubKey);
	fclose(fPubKey);

	vdDebug_LogPrintf("ReadFile[%d].inMaxRecSize=[%d]...", inRetVal,inMaxRecSize);

	return inRetVal;
}


int inMultiAP_Database_COM_ClearEx(void)
{
	int inRetVal = 0;

	memset(szSendData,0x00,SEND_BUF);
	memset(szReceiveData,0x00,RECEIVE_BUF);
	memset(&strCOM,0x00,sizeof(STRUCT_COM));
#if 1	
	inRetVal = inCTOSS_COMM_GetFileSize(COMM_HEADER_FILE);
	vdDebug_LogPrintf("[%s] =[%d] ", COMM_HEADER_FILE,inRetVal);
	if (inRetVal >= 0)
		remove(COMM_HEADER_FILE);

	inRetVal = inCTOSS_COMM_GetFileSize(COMM_BUFF_FILE);
	vdDebug_LogPrintf("[%s] =[%d] ", COMM_BUFF_FILE,inRetVal);
	if (inRetVal >= 0)
		remove(COMM_BUFF_FILE);
#endif

	/*clear COM ext data from file 20171012 start*/
	memset(&strCOMExt,0x00,sizeof(S_COM_EXT));
	inRetVal = inCTOSS_COMM_GetFileSize(COMM_EXT_FILE);
	vdDebug_LogPrintf("[%s] =[%d] ", COMM_EXT_FILE,inRetVal);
	if (inRetVal >= 0)
		remove(COMM_EXT_FILE);
	/*clear COM ext data from file 20171012 End*/
	
	return d_OK;
}

int inMultiAP_Database_COM_SaveEx(void)
{
	int inRetVal = 0;
	unsigned char pchRecBuf[1000];
	int inMaxRecSize = 0;
	
	memset(pchRecBuf,0x00,sizeof(pchRecBuf));
	inMaxRecSize = sizeof(STRUCT_COM);
	memcpy(pchRecBuf,&strCOM,inMaxRecSize);
	vdDebug_LogPrintf("Database_COM_SaveEx =[%d] ", inMaxRecSize);
	
	inRetVal = inCTOSS_COMM_WriteFile(COMM_HEADER_FILE,pchRecBuf,inMaxRecSize);
	if (inRetVal <= 0)
		return d_NO;

	vdDebug_LogPrintf("Database_COM_SaveEx inSendLen=[%d] ", strCOM.inSendLen);
	if (strCOM.inSendLen > 0)
	{
		inRetVal = inCTOSS_COMM_WriteFile(COMM_BUFF_FILE,szSendData,strCOM.inSendLen);
		if (inRetVal <= 0)
		return d_NO;
	}

	/*save COM ext data from file 20171012 start*/
	memset(pchRecBuf,0x00,sizeof(pchRecBuf));
	inMaxRecSize = sizeof(S_COM_EXT);
	memcpy(pchRecBuf,&strCOMExt,inMaxRecSize);	
	inRetVal = inCTOSS_COMM_WriteFile(COMM_EXT_FILE,pchRecBuf,inMaxRecSize);
	///if (inRetVal <= 0)
		//return d_NO;
	/*save COM ext data from file 20171012 End*/

	return d_OK;
}

int inMultiAP_Database_COM_ReadEx(void)
{
	int inRetVal = 0;
	unsigned char pchRecBuf[1000];
	int inMaxRecSize = 0;
	memset(pchRecBuf,0x00,sizeof(pchRecBuf));
	inMaxRecSize = sizeof(STRUCT_COM);

	vdDebug_LogPrintf("Database_COM_ReadEx =[%d] ", inMaxRecSize);
	inRetVal = inCTOSS_COMM_ReadFile(COMM_HEADER_FILE,pchRecBuf,inMaxRecSize);
	if (inRetVal <= 0)
		return d_NO;
	
	memset(&strCOM,0x00,inMaxRecSize);
	memcpy(&strCOM,pchRecBuf,inMaxRecSize);

	vdDebug_LogPrintf("Database_COM_ReadEx inReceiveLen=[%d] ", strCOM.inReceiveLen);
	if (strCOM.inReceiveLen > 0)
	{
		memset(szReceiveData,0x00,RECEIVE_BUF);
		inRetVal = inCTOSS_COMM_ReadFile(COMM_BUFF_FILE,szReceiveData,strCOM.inReceiveLen);
		if (inRetVal <= 0)
			return d_NO;
	}

	/*read COM ext data from file 20171012 start*/
	memset(pchRecBuf,0x00,sizeof(pchRecBuf));
	inMaxRecSize = sizeof(S_COM_EXT);
	inRetVal = inCTOSS_COMM_ReadFile(COMM_EXT_FILE,pchRecBuf,inMaxRecSize);
	//if (inRetVal <= 0)
		//return d_NO;

	memset(&strCOMExt,0x00,inMaxRecSize);
	memcpy(&strCOMExt,pchRecBuf,inMaxRecSize);
	/*read COM ext data from file 20171012 End*/

	return d_OK;
}



int inCTOSS_COMM_Initialize(TRANS_DATA_TABLE *srTransPara,int inMode)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

//check if signal is OK
	USHORT usNetworkType = 0;
	BYTE szNetworkName[128+1];
	int inRet = 0;

	vdDebug_LogPrintf("saturn --inCTOSS_COMM_Initialize,chGetInit_Connect=[%d]",chGetInit_Connect());
    //test
	if (chGetInit_Connect() == 1)
		return inCTOSS_COMM_InitializeAndConnect(srTransPara,inMode);
		
	//0615
	inRet  = inCTOSS_GetGPRSSignalEx(&usNetworkType, szNetworkName,&usEthType);
	if (inRet != d_OK)
		return d_NO;
	
    vdDebug_LogPrintf("saturn 1.inCTOSS_COMM_Initialize, inCTOSS_GetGPRSSignalEx -- END");
	// patrick must remark if fail connect GPRS need fallback to dial up mode 20170206
	/*

	if ((fGPRSConnectOK != TRUE) && (inMode == GPRS_MODE))
		return d_NO;
	//0615
	*/
	
    memset(bOutBuf, 0x00, sizeof(bOutBuf)); 

    //inMultiAP_Database_COM_Clear();//change DB to txt file
    inMultiAP_Database_COM_ClearEx();
	vdDebug_LogPrintf("saturn 2.inCTOSS_COMM_Initialize, inMultiAP_Database_COM_ClearEx -- END");

    //strCOM.inCommMode = GPRS_MODE;//for test
    //strCOM.inCommMode = ETHERNET_MODE;//for test
    strCOM.inCommMode = inMode;
	gCommMode = inMode;//for Sharls_COM modem
	//for Sharls_COM modem
	if (inMode == DIAL_UP_MODE)
	{
		strCOM.bPredialFlag = strCPT.fPreDial;
		strCOM.inParaMode = strCPT.inParaMode;
		strCOM.inHandShake = strCPT.inHandShake;
		strCOM.inCountryCode = strCPT.inCountryCode;

		strcpy(strCOM.szPriPhoneNum,strCPT.szPriTxnPhoneNumber);
		strcpy(strCOM.szSecPhoneNum,strCPT.szSecTxnPhoneNumber);

		if(srTransRec.byTransType == SETTLE)
		{
			strcpy(strCOM.szPriPhoneNum,strCPT.szPriSettlePhoneNumber);
			strcpy(strCOM.szSecPhoneNum,strCPT.szSecSettlePhoneNumber);

		}

		strcpy(strCOM.szATCMD1,strMMT[0].szATCMD1);
		strcpy(strCOM.szATCMD2,strMMT[0].szATCMD2);
		strcpy(strCOM.szATCMD3,strMMT[0].szATCMD3);
		strcpy(strCOM.szATCMD4,strMMT[0].szATCMD4);
		strcpy(strCOM.szATCMD5,strMMT[0].szATCMD5);
	}

	/*pass in the CA file, depend on host and application*/
	if (1 == strCPT.fSSLEnable)
		strcpy(strCOM.szCAFileName, MY_CA_FILE);
	vdDebug_LogPrintf("--szCAFileName=[%s]",strCOM.szCAFileName);

    //inMultiAP_Database_COM_Save();
	inMultiAP_Database_COM_SaveEx();
		vdDebug_LogPrintf("saturn --inCTOSS_COMM_Initialize,fGPRSConnectOK=[%d]",fGPRSConnectOK);

	//if ((fGPRSConnectOK != TRUE) && (strCPT.inCommunicationMode == GPRS_MODE)){
	//	return d_NO;
	//}
	if (inMode== GPRS_MODE)
	{
    	CTOS_KBDBufPut(d_KBD_INVALID_COM);
	}
	
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_INIT, bInBuf, usInLen, bOutBuf, &usOutLen);
	vdDebug_LogPrintf("saturn 3.inCTOSS_COMM_Initialize, inMultiAP_RunIPCCmdTypes -- END");
    if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC)            
            return d_OK;
        else
            return d_NO;
    }
    
    return usResult;
}

int inCTOSS_COMM_Connect(TRANS_DATA_TABLE *srTransPara)
{
    BYTE bInBuf[1024];
    BYTE bOutBuf[1024];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	
	USHORT usNetworkType = 0;
	BYTE szNetworkName[128+1];
	int inRet = 0;
	

	vdDebug_LogPrintf("saturn --inCTOSS_COMM_Connectb,chGetInit_Connect=[%d], g_inCURLConnect=[%d]",chGetInit_Connect(), g_inCURLConnect);
	#if 1
    USHORT ret;

	// if use CURL connect
	if (1 == g_inCURLConnect)
	{
		BYTE bDevice;
	
		if (srTransPara->usTerminalCommunicationMode == GPRS_MODE)
			bDevice = d_TCP_DEVICE_GPRS;

		if (srTransPara->usTerminalCommunicationMode == WIFI_MODE)
			bDevice = d_TCP_DEVICE_WIFI;

		if (srTransPara->usTerminalCommunicationMode == ETHERNET_MODE)
			bDevice = d_TCP_DEVICE_ETHERNET;

		memset(bInBuf, 0x00, sizeof(bInBuf));
		memset(bOutBuf, 0x00, sizeof(bOutBuf));
		bInBuf[0] = bDevice;
		
		usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_BIND_DEVICE, bInBuf, usInLen, bOutBuf, &usOutLen);
		
		return d_OK;
	}
	#endif

	if (chGetInit_Connect() == 1)
		return d_OK;


	inRet = inCTOSS_GetGPRSSignalEx(&usNetworkType, szNetworkName,&usEthType);
	if (inRet != d_OK)
		return d_NO;

	// patrick must remark if fail connect GPRS need fallback to dial up mode 20170206
	/*
	vdDebug_LogPrintf("--inCTOSS_COMM_Connect,fGPRSConnectOK=[%d],inCommunicationMode=[%d]",fGPRSConnectOK,strCPT.inCommunicationMode);
	if ((fGPRSConnectOK != TRUE) && (strCPT.inCommunicationMode == GPRS_MODE)){
		return d_NO;
	}
	*/
		
	vdDebug_LogPrintf("--usEthType=[%d],gCommMode=[%d]",usEthType,gCommMode);
	if (usEthType != 1 && gCommMode == ETHERNET_MODE)
	{
		vdDisplayErrorMsg(1, 8, "Cable Not Connect");
		return d_NO;
	}

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
    //inMultiAP_Database_COM_Clear();
	inMultiAP_Database_COM_ClearEx();

    if(srTransRec.byTransType != SETTLE)
    {
        strcpy(strCOM.szPriHostIP,strCPT.szPriTxnHostIP);
        strcpy(strCOM.szSecHostIP,strCPT.szSecTxnHostIP);
        strCOM.ulPriHostPort = strCPT.inPriTxnHostPortNum;
        strCOM.ulSecHostPort = strCPT.inSecTxnHostPortNum;
        vdDebug_LogPrintf("saturn -inCTOSS_COMM_Connect[%s]port[%ld]",strCOM.szPriHostIP,strCOM.ulPriHostPort);
 

	}
    else
    {
        strcpy(strCOM.szPriHostIP,strCPT.szPriSettlementHostIP);
        strcpy(strCOM.szSecHostIP,strCPT.szSecSettlementHostIP);
        strCOM.ulPriHostPort = strCPT.inPriSettlementHostPort;
        strCOM.ulSecHostPort = strCPT.inSecSettlementHostPort;
        vdDebug_LogPrintf("saturn --inCTOSS_COMM_Connect[%s]port[%ld]",strCOM.szPriHostIP,strCOM.ulPriHostPort);

    } 
	strCOM.bSSLFlag = strCPT.fSSLEnable;
	strCOM.inGPRSSingal = strCPT.fDialMode;//for GPRS connection mode,0=CTOS API, 1= Linux TCP/ip API
	strCOM.inReceiveTimeOut = strCPT.inTCPResponseTimeout;
	strCOM.inConnectionTimeOut = strCPT.inTCPConnectTimeout;

	if (1 == strCPT.fSSLEnable)
		strcpy(strCOM.szCAFileName, MY_CA_FILE);
	vdDebug_LogPrintf("strCOM.szCAFileName[%s]", strCOM.szCAFileName);
	
	vdDebug_LogPrintf("--inCTOSS_COMM_Connectb,SSLFlag=[%d],inGPRSSingal=[%d]",strCOM.bSSLFlag,strCOM.inGPRSSingal);

	/*hanld COM Ext data, here*/
	/*pass in SSL version*/
	strCOMExt.byEnableExt = 1;  // enable Com Ext data
	strCOMExt.bySSLVerCtrl = get_env_int(ENV_MY_SSL_VER);

	if (99 == strCOMExt.bySSLVerCtrl || 255 == strCOMExt.bySSLVerCtrl)
		strCOMExt.byEnableExt = 0;
	
	vdDebug_LogPrintf("strCOMExt.byEnableExt[%d]", strCOMExt.byEnableExt);
	vdDebug_LogPrintf("strCOMExt.bySSLVerCtrl[%d]", strCOMExt.bySSLVerCtrl);
	
	//inMultiAP_Database_COM_Save();
	inMultiAP_Database_COM_SaveEx();

	vdDebug_LogPrintf("saturn usPreconnectStatus %d", usPreconnectStatus);
	//test  
	usPreconnectStatus = 0;

	if (usPreconnectStatus == 1)
	{
    	CTOS_KBDBufPut(d_KBD_INVALID_COM);
		usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_CONNECT_PreConnect1, bInBuf, usInLen, bOutBuf, &usOutLen);	
	}
	else if (usPreconnectStatus == 2)
	{
    	CTOS_KBDBufPut(d_KBD_INVALID_COM);
		usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_CONNECT_PreConnect2, bInBuf, usInLen, bOutBuf, &usOutLen);	
	}
	else
	{
		usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_CONNECT, bInBuf, usInLen, bOutBuf, &usOutLen);
	}


	vdDebug_LogPrintf("saturn connect result %d %d %d", usResult, bOutBuf, bOutBuf[0]);

	//return d_OK;
    if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC)            
            return d_OK;
        else
            return d_NO;
    }
    
    return usResult;
}




int inCTOSS_COMM_InitializeAndConnect(TRANS_DATA_TABLE *srTransPara,int inMode)
{
    BYTE bInBuf[1024];
    BYTE bOutBuf[1024];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

//check if signal is OK
	USHORT usNetworkType = 0;
	BYTE szNetworkName[128+1];

	int inRet=0;
		
	//0615


	vdDebug_LogPrintf("saturn inCTOSS_COMM_InitializeAndConnect");
	vdDebug_LogPrintf("usTerminalCommunicationMode[%d],inCommunicationMode[%d]", srTransRec.usTerminalCommunicationMode, strCPT.inCommunicationMode);
	vdDebug_LogPrintf("inMode=[%d]", inMode);
	vdDebug_LogPrintf("srTransRec.byTransType=[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("Trxn->Primary[%s]port[%ld]",strCPT.szPriTxnHostIP,strCPT.inPriTxnHostPortNum);
	vdDebug_LogPrintf("Trxn->Secondary[%s]port[%ld]",strCPT.szSecTxnHostIP,strCPT.inSecTxnHostPortNum);
	vdDebug_LogPrintf("Settle->Primary[%s]port[%ld]",strCPT.szPriSettlementHostIP,strCPT.inPriSettlementHostPort);
	vdDebug_LogPrintf("Settle->Secondary[%s]port[%ld]",strCPT.szSecSettlementHostIP,strCPT.inSecSettlementHostPort);
	
	inRet = inCTOSS_GetGPRSSignalEx(&usNetworkType, szNetworkName,&usEthType);
	if (inRet != d_OK)
		return d_NO;
	
	//if(inRet != d_OK)
	//	return inRet;

	// patrick must remark if fail connect GPRS need fallback to dial up mode 20170206
	/*

	if ((fGPRSConnectOK != TRUE) && (inMode == GPRS_MODE))
		return d_NO;
	//0615
	*/
	
    memset(bOutBuf, 0x00, sizeof(bOutBuf)); 

    //inMultiAP_Database_COM_Clear();//change DB to txt file
    inMultiAP_Database_COM_ClearEx();

    //strCOM.inCommMode = GPRS_MODE;//for test
    //strCOM.inCommMode = ETHERNET_MODE;//for test
    strCOM.inCommMode = inMode;
	gCommMode = inMode;//for Sharls_COM modem
	//for Sharls_COM modem
	if (inMode == DIAL_UP_MODE)
	{
		strCOM.bPredialFlag = strCPT.fPreDial;
		strCOM.inParaMode = strCPT.inParaMode;
		strCOM.inHandShake = strCPT.inHandShake;
		strCOM.inCountryCode = strCPT.inCountryCode;

		strcpy(strCOM.szPriPhoneNum,strCPT.szPriTxnPhoneNumber);
		strcpy(strCOM.szSecPhoneNum,strCPT.szSecTxnPhoneNumber);

		if(srTransRec.byTransType == SETTLE)
		{
			strcpy(strCOM.szPriPhoneNum,strCPT.szPriSettlePhoneNumber);
			strcpy(strCOM.szSecPhoneNum,strCPT.szSecSettlePhoneNumber);

		}

		strcpy(strCOM.szATCMD1,strMMT[0].szATCMD1);
		strcpy(strCOM.szATCMD2,strMMT[0].szATCMD2);
		strcpy(strCOM.szATCMD3,strMMT[0].szATCMD3);
		strcpy(strCOM.szATCMD4,strMMT[0].szATCMD4);
		strcpy(strCOM.szATCMD5,strMMT[0].szATCMD5);
	}

	/*pass in the CA file, depend on host and application*/
	if (1 == strCPT.fSSLEnable)
		strcpy(strCOM.szCAFileName, MY_CA_FILE);
	vdDebug_LogPrintf("--szCAFileName=[%s]",strCOM.szCAFileName);

	
	if(srTransRec.byTransType != SETTLE)
	   {
		   strcpy(strCOM.szPriHostIP,strCPT.szPriTxnHostIP);
		   strcpy(strCOM.szSecHostIP,strCPT.szSecTxnHostIP);
		   strCOM.ulPriHostPort = strCPT.inPriTxnHostPortNum;
		   strCOM.ulSecHostPort = strCPT.inSecTxnHostPortNum;
		   vdDebug_LogPrintf("saturn -inCTOSS_COMM_Connect[%s]port[%ld]",strCOM.szPriHostIP,strCOM.ulPriHostPort);
	   }
	   else
	   {
		   strcpy(strCOM.szPriHostIP,strCPT.szPriSettlementHostIP);
		   strcpy(strCOM.szSecHostIP,strCPT.szSecSettlementHostIP);
		   strCOM.ulPriHostPort = strCPT.inPriSettlementHostPort;
		   strCOM.ulSecHostPort = strCPT.inSecSettlementHostPort;
		   vdDebug_LogPrintf("saturn --inCTOSS_COMM_InitializeAndConnect[%s]port[%ld]",strCOM.szPriHostIP,strCOM.ulPriHostPort);
	
	   } 
	   strCOM.bSSLFlag = strCPT.fSSLEnable;
	   strCOM.inGPRSSingal = strCPT.fDialMode;//for GPRS connection mode,0=CTOS API, 1= Linux TCP/ip API
	   strCOM.inReceiveTimeOut = strCPT.inTCPResponseTimeout;
	   strCOM.inConnectionTimeOut = strCPT.inTCPConnectTimeout;
	   
	   vdDebug_LogPrintf("saturn --inCTOSS_COMM_InitializeAndConnect,SSLFlag=[%d],inGPRSSingal=[%d]",strCOM.bSSLFlag,strCOM.inGPRSSingal);

    //inMultiAP_Database_COM_Save();
	inMultiAP_Database_COM_SaveEx();
		vdDebug_LogPrintf("saturn --inCTOSS_COMM_InitializeAndConnect,fGPRSConnectOK=[%d]",fGPRSConnectOK);

	//if ((fGPRSConnectOK != TRUE) && (strCPT.inCommunicationMode == GPRS_MODE)){
	//	return d_NO;
	//}
	if (inMode== GPRS_MODE)
	{
    	CTOS_KBDBufPut(d_KBD_INVALID_COM);
	}
	
    //usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_INIT, bInBuf, usInLen, bOutBuf, &usOutLen);
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_INIT_CONNECTEx, bInBuf, usInLen, bOutBuf, &usOutLen);
	vdDebug_LogPrintf("saturn ---inCTOSS_COMM_Initialize[%02X %02X %02X],usResult=[%d]", bOutBuf[0], bOutBuf[1], bOutBuf[2], usResult);
    if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC)            
            return d_OK;
        else
            return d_NO;
    }
    
    return usResult;
}



int inCTOSS_COMM_Send(TRANS_DATA_TABLE *srTransPara,unsigned char *uszSendData,unsigned long ulSendLen)
{
    BYTE bInBuf[1024];
    BYTE bOutBuf[1024];
    //BYTE bySendTemp[2048];
	BYTE bySendTemp[SEND_BUF];
    BYTE byTempLen[10];
    BYTE byTempHEXLen[10];
    int inHeader;
    
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    BYTE szDisMsg[64];
	BYTE szTitle[64];

	// remove sending base on updated ui -- sidumili
	/*
	if (srTransPara->byTransType != VOID)
	{
		vdDisplayMessageBox(1, 8, "Sending...", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
		CTOS_Delay(10);
	}
	*/
	
    if(fSettleTrans == TRUE)
    {
        memset(szTitle, 0x00, sizeof(szTitle));
        memset(szDisMsg, 0x00, sizeof(szDisMsg));
        szGetTransTitle(SETTLE, szTitle);
        //strcpy(szDisMsg, szTitle);
        //strcat(szDisMsg, "|");
        //strcat(szDisMsg, "SENDING...");
        //usCTOSS_LCDDisplay(szDisMsg); 

    }
    else
    {
    
		vdDebug_LogPrintf("saturn inCTOSS_COMM_Send %d", fAdviceTras);
        if(srTransPara->byPackType == TC_UPLOAD || fAdviceTras == VS_TRUE)
        {
        
		vdDebug_LogPrintf("saturn inCTOSS_COMM_Send tc upload and advice");
            memset(szTitle, 0x00, sizeof(szTitle));
            memset(szDisMsg, 0x00, sizeof(szDisMsg));
			if (fAdviceTras == VS_TRUE)
				strcpy(szTitle, "ADVICE");
			else	
            	szGetTransTitle(srTransPara->byTransType, szTitle);
			
			vdDebug_LogPrintf("saturn title %s", szTitle);
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "SENDING...");
            //usCTOSS_LCDDisplay(szDisMsg); 
            //CTOS_Delay(3000);

			#ifdef ANDROID_NEW_UI
			  //vdDisplayMessageStatusBox(1, 8, szTitle, "SENDING...", MSG_TYPE_SEND);
			  //CTOS_Delay(300);
			#endif
        }			
        else
        {
        	#ifdef ANDROID_NEW_UI
            	//vdDispTransTitleCardTypeandTotal(srTransPara->byTransType, "SENDING...");
			#endif
        }
    }

    memset(bOutBuf, 0x00, sizeof(bOutBuf)); 


	vdDebug_LogPrintf("saturn --inCTOSS_COMM_Send,chGetInit_Connect=[%d]",chGetInit_Connect());
    //inMultiAP_Database_COM_Clear();
	inMultiAP_Database_COM_ClearEx();

    inHeader = strCPT.inIPHeader;
    strCOM.bSSLFlag = strCPT.fSSLEnable;

	if (gCommMode == DIAL_UP_MODE)//for Sharls_COM modem
		inHeader = NO_HEADER_LEN;
    //inHeader = HEX_EXCLUDE_LEN;//for test
    vdDebug_LogPrintf("saturn inCTOSS_COMM_Send[%ld]",ulSendLen);
    
    switch(inHeader)
    {
        case NO_HEADER_LEN:
            strCOM.inSendLen = ulSendLen;
            vdDebug_LogPrintf("---***inCTOSS_COMM_Send[%ld]",ulSendLen);
            
            memcpy(bySendTemp, uszSendData, ulSendLen);
            memcpy(szSendData,bySendTemp,strCOM.inSendLen);
            break;
            
        case HEX_EXCLUDE_LEN:
            bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
            bySendTemp[1] = (ulSendLen & 0x000000FF);

            strCOM.inSendLen = ulSendLen + 2;
            
            vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",ulSendLen);
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);
            memcpy(szSendData,bySendTemp,strCOM.inSendLen);

            break;

        case BCD_EXCLUDE_LEN:
            sprintf(byTempLen,"%04ld",ulSendLen);
            wub_str_2_hex(byTempLen,byTempHEXLen,4);
            memcpy(bySendTemp,byTempHEXLen,2);           
            strCOM.inSendLen = ulSendLen + 2;
            
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);
            memcpy(szSendData,bySendTemp,strCOM.inSendLen);
            break;

        case HEX_INCLUDE_LEN:
            ulSendLen += 2;

            bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
            bySendTemp[1] = (ulSendLen & 0x000000FF);

            strCOM.inSendLen = ulSendLen;
            vdDebug_LogPrintf("---HEX_INCLUDE_LEN[%ld]",ulSendLen);
            memcpy(&bySendTemp[2], uszSendData, ulSendLen-2);
            memcpy(szSendData,bySendTemp,strCOM.inSendLen);   
            break;
            

        case BCD_INCLUDE_LEN:            
            ulSendLen += 2;

            sprintf(byTempLen,"%04ld",ulSendLen);
            wub_str_2_hex(byTempLen,byTempHEXLen,4);
            memcpy(bySendTemp,byTempHEXLen,2);           
            strCOM.inSendLen = ulSendLen;
            
            memcpy(&bySendTemp[2], uszSendData, ulSendLen-2);
            memcpy(szSendData,bySendTemp,strCOM.inSendLen);
            
            break;

        default:
            
            bySendTemp[0] = ((ulSendLen & 0x0000FF00) >> 8);
            bySendTemp[1] = (ulSendLen & 0x000000FF);

            strCOM.inSendLen = ulSendLen + 2;
            
            vdDebug_LogPrintf("---inCTOSS_COMM_Send[%ld]",ulSendLen);
            memcpy(&bySendTemp[2], uszSendData, ulSendLen);
            memcpy(szSendData,bySendTemp,strCOM.inSendLen);

            break;
    }

	DebugAddHEX("inCTOSS_COMM_Send, szSendData:", szSendData, strCOM.inSendLen);
	vdDebug_LogPrintf("inHeader=[%d]", inHeader);
	
    strCOM.inHeaderFormat = inHeader;
    
    //inMultiAP_Database_COM_Save();
	inMultiAP_Database_COM_SaveEx();

		//if (chGetInit_Connect() == 1)
			//usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_SENDEx, bInBuf, usInLen, bOutBuf, &usOutLen);
		//else
	        usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_SEND, bInBuf, usInLen, bOutBuf, &usOutLen);
    
    vdDebug_LogPrintf("saturn ---inCTOSS_COMM_Send[%ld] %d",usResult, bOutBuf[0]);
    if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC)            
            return d_OK;
        else
            return d_NO;
    }
    else
    {
    	vdDebug_LogPrintf("SATURN ---inCTOSS_COMM_Send.. FAILED");
		return d_NO;
    }
    
    //return usResult;
}

int inCTOSS_COMM_Receive(TRANS_DATA_TABLE *srTransPara,unsigned char *uszRecData)
{
    BYTE bInBuf[1024];
    BYTE bOutBuf[1024];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	int inHeader;
	
    BYTE szDisMsg[64];
	BYTE szTitle[64];

	vdDebug_LogPrintf("--inCTOSS_COMM_Receive--");
	vdDebug_LogPrintf("gCommMode=[%d]", gCommMode);
	vdDebug_LogPrintf("fSettleTrans=[%d]", fSettleTrans);
	
	if (srTransPara->byTransType != VOID)
	{
		vdDisplayMessageBox(1, 8, "Receiving...", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
		CTOS_Delay(10);
	}
	
    if(fSettleTrans == TRUE)
    {
        memset(szTitle, 0x00, sizeof(szTitle));
        memset(szDisMsg, 0x00, sizeof(szDisMsg));
        szGetTransTitle(SETTLE, szTitle);
        //strcpy(szDisMsg, szTitle);
        //strcat(szDisMsg, "|");
        //strcat(szDisMsg, "RECEIVING...");
        //usCTOSS_LCDDisplay(szDisMsg); 

		#ifdef ANDROID_NEW_UI
        	//vdDisplayMessageStatusBox(1, 8, szTitle, "RECEIVING...", MSG_TYPE_RECEIVE);
			//CTOS_Delay(300);
		#endif
    }
    else
    {
        if(srTransPara->byPackType == TC_UPLOAD || fAdviceTras == VS_TRUE)
        {
            memset(szTitle, 0x00, sizeof(szTitle));
            memset(szDisMsg, 0x00, sizeof(szDisMsg));
			
			if (fAdviceTras == VS_TRUE)
				strcpy(szTitle, "ADVICE");
			else	
            	szGetTransTitle(srTransPara->byTransType, szTitle);
			
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "RECEIVING...");
            //usCTOSS_LCDDisplay(szDisMsg); 
            //CTOS_Delay(3000);

			#ifdef ANDROID_NEW_UI
            	//vdDisplayMessageStatusBox(1, 8, szTitle, "RECEIVING...", MSG_TYPE_RECEIVE);
				//CTOS_Delay(300);
			#endif
        }			
        else
        {
        	#ifdef ANDROID_NEW_UI
            	//vdDispTransTitleCardTypeandTotal(srTransPara->byTransType, "RECEIVING...");
			#endif
        }
    }

	
    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    inHeader = strCPT.inIPHeader;
	if (gCommMode == DIAL_UP_MODE)//for Sharls_COM modem
		inHeader = NO_HEADER_LEN;

    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_RECEIVE, bInBuf, usInLen, bOutBuf, &usOutLen);
    
    vdDebug_LogPrintf("---inCTOSS_COMM_Receive.usResult[%d]bOutBuf[0]=[%d]",usResult,bOutBuf[0]);
    if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC) 
        {      
            //inMultiAP_Database_COM_Read();
			inMultiAP_Database_COM_ReadEx();
            vdDebug_LogPrintf("---strCOM.inReceiveLen[%d],.inIPHeader=[%d]",strCOM.inReceiveLen,strCPT.inIPHeader);
            vdDebug_LogPrintf("rec data==[%d][%s]",strCOM.inReceiveLen,szReceiveData);
            if(inHeader == NO_HEADER_LEN)
            {
                usResult = strCOM.inReceiveLen;
                memcpy(uszRecData,szReceiveData,strCOM.inReceiveLen);

				//puts("Server reply :");
				//puts(uszRecData);	
            }
            else
            {
            	if (HEX_EXCLUDE_LEN == strCPT.inIPHeader)
        		{
					short inLen = 0;
					int inReceivedDataIndex = 0;

                	inLen = szReceiveData[0] * 256;
                	inLen += szReceiveData[1];					
						
					vdDebug_LogPrintf("inLen [%ld] Receive data[%ld]", inLen, (strCOM.inReceiveLen-2));
					if (inLen != (strCOM.inReceiveLen-2))
					{						
						inReceivedDataIndex = usResult = strCOM.inReceiveLen-2;
						memcpy(uszRecData,&szReceiveData[2],strCOM.inReceiveLen-2);
					
						usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_RECEIVE, bInBuf, usInLen, bOutBuf, &usOutLen);
						
						vdDebug_LogPrintf("---inCTOSS_COMM_Receive.usResult[%d]bOutBuf[0]=[%d]",usResult,bOutBuf[0]);
						if(d_OK == usResult)
						{
							//status
							if(bOutBuf[0] == IPC_COMM_SUCC) 
							{	   
								//inMultiAP_Database_COM_Read();
								inMultiAP_Database_COM_ReadEx();
								vdDebug_LogPrintf("---strCOM.inReceiveLen[%d],.inIPHeader=[%d]",strCOM.inReceiveLen,strCPT.inIPHeader);
								vdDebug_LogPrintf("rec data==[%d][%s]",strCOM.inReceiveLen,szReceiveData);

								memcpy(&uszRecData[inReceivedDataIndex],szReceiveData,strCOM.inReceiveLen);
								usResult = inReceivedDataIndex + strCOM.inReceiveLen;
							}
						}
						usResult = d_OK;
						bOutBuf[0] = IPC_COMM_SUCC;
	        		}					
					else
					{
						usResult = strCOM.inReceiveLen-2;
						memcpy(uszRecData,&szReceiveData[2],strCOM.inReceiveLen-2);
					}
            	}
				else
				{
					usResult = strCOM.inReceiveLen-2;
					memcpy(uszRecData,&szReceiveData[2],strCOM.inReceiveLen-2);
				}
            }
			//after receive clear database
			//inMultiAP_Database_COM_Clear();
			inMultiAP_Database_COM_ClearEx();
            return usResult;
        }
		else //if(bOutBuf[0] == IPC_COMM_FAIL) 
		{
			//tine:  android - test (fix to handle disconnected connection on receiving host response
			vdDebug_LogPrintf("Connection disconnected on receiving");
			return ST_COMMS_DISCONNECT;
		}
        //else
        //    return 0;
    }
	else
	{
		vdDebug_LogPrintf("SATURN ---inCTOSS_COMM_Receive.. FAILED");
	}
    
    return usResult;
}

int inCTOSS_COMM_Disconnect(TRANS_DATA_TABLE *srTransPara)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf)); 
	gCommMode = -1;//for Sharls_COM modem

	#if 1
	if (1 == g_inCURLConnect)
	{
		return d_OK;
	}
	#endif

    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_DISCONNECT, bInBuf, usInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC)            
            return d_OK;
        else
            return d_NO;
    }
    
    return usResult;
}

int inCTOSS_COMM_End(TRANS_DATA_TABLE *srTransPara)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf)); 
	gCommMode = -1;//for Sharls_COM modem

    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_END, bInBuf, usInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC)            
            return d_OK;
        else
            return d_NO;
    }
    
    return usResult;
}

int inCTOSS_GetGPRSSignal(unsigned short* usNetworkType, char* szNetworkName,unsigned short* usEthType)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    int inSignal;

//	USHORT usType;
    BYTE szType[2+1];

    memset(bOutBuf, 0x00, sizeof(bOutBuf)); 

    usResult = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_GPRS_SIGNAL, bInBuf, usInLen, bOutBuf, &usOutLen);

	*usEthType = bOutBuf[1];
    inSignal = bOutBuf[2];
	
	memset(szType, 0x00, sizeof(szType));
	memcpy(szType, &bOutBuf[3], 2);
	*usNetworkType = (szType[0] | szType[1] << 8);

	strcpy(szNetworkName, &bOutBuf[6]);
	szNetworkName[strlen(szNetworkName)-1] = 0x00;
	
    vdDebug_LogPrintf("---NETWORK TYPE[%02X][%s]", *usNetworkType, szNetworkName);					
    vdDebug_LogPrintf("---inCTOSS_GetGPRSSignal[%02X %02X %02X]", bOutBuf[0], bOutBuf[1], bOutBuf[2]);
    if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC){
						if (inSignal < 1)
							fGPRSConnectOK = FALSE;
						else
							fGPRSConnectOK = TRUE;
							
            return inSignal;
        }	
        else{
						fGPRSConnectOK = FALSE;
						usResult = 0;
            return usResult;
        }
    }
		else{
			inSignal = 0;
			usResult = 0;
			fGPRSConnectOK = FALSE;
			return usResult;
		}
    
    return usResult;
}

int inCTOSS_GetGPRSSignalEx(unsigned short* usNetworkType, char* szNetworkName,unsigned short* usEthType)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[1000];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    int inSignal;
	int inGPRSConnected;
	int inmedomstatus;
	STRUCT_SHARLS_COM Sharls_COMData;

//	USHORT usType;
    BYTE szType[2+1];

	int inRet;

	vdDebug_LogPrintf("saturn inCTOSS_GetGPRSSignalEx");
	vdDebug_LogPrintf("strTCT.fDemo %d", strTCT.fDemo);
	vdDebug_LogPrintf("usPreconnectStatus=[%d]", usPreconnectStatus);
	
	#if 1
	
		inRet = usGetConnectionStatus(srTransRec.usTerminalCommunicationMode);	
		if (inRet == d_OK)
			fGPRSConnectOK = TRUE;
		else
			fGPRSConnectOK = FALSE;

		vdDebug_LogPrintf("saturn pre after get connection status, inRet=[%d]", inRet);
		return inRet;	
	
	#endif

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(&Sharls_COMData, 0x00, sizeof(STRUCT_SHARLS_COM));

	if (usPreconnectStatus == 1)
	{
    	CTOS_KBDBufPut(d_KBD_INVALID_COM);
	}

    //usResult = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_GPRS_SIGNALEX, bInBuf, usInLen, bOutBuf, &usOutLen);
	if(strTCT.fDemo == VS_FALSE)
    		usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_GPRS_SIGNALEX, bInBuf, usInLen, bOutBuf, &usOutLen);

	memcpy(&Sharls_COMData, &bOutBuf[1],sizeof(STRUCT_SHARLS_COM));
	
	*usEthType = Sharls_COMData.usEthType;
    inSignal = Sharls_COMData.bgGPRSSignal;
	inmedomstatus = Sharls_COMData.inReserved1;
	
	*usNetworkType = Sharls_COMData.usGPRSNetworkType;
	strcpy(szNetworkName, Sharls_COMData.szGPRSNetworkName);
	inGPRSConnected = Sharls_COMData.bgGPRSType;
	usHWSupport = (Sharls_COMData.szReserved1[0] | Sharls_COMData.szReserved1[1] << 8);

	vdDebug_LogPrintf("saturn ---inmedomstatus[%d],usHWSupport=[%04x],inSignal=[%d]", inmedomstatus,usHWSupport, inSignal);
    vdDebug_LogPrintf("saturn ---NETWORK TYPE[%02X][%s],inGPRSConnected=[%d]", *usNetworkType, szNetworkName,inGPRSConnected);					
    vdDebug_LogPrintf("saturn ---inCTOSS_GetGPRSSignal[%02X %02X %02X]", bOutBuf[0], bOutBuf[1], bOutBuf[2]);
    if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC){
						if (inGPRSConnected == TRUE)
							fGPRSConnectOK = TRUE;
						else
							fGPRSConnectOK = FALSE;
							
            return inSignal;
        }	
        else{
						fGPRSConnectOK = FALSE;
						usResult = 0;
            return usResult;
        }
    }
		else{
			inSignal = 0;
			usResult = 0;
			fGPRSConnectOK = FALSE;
			return usResult;
		}
    
    return usResult;
}

int inCTOSS_GetGPRSSignalEx1(unsigned short* usNetworkType, char* szNetworkName,unsigned short* usEthType, STRUCT_SHARLS_COM* Sharls_COMData)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[1000];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    int inSignal;
	int inGPRSConnected;
	int inmedomstatus;
//	STRUCT_SHARLS_COM Sharls_COMData;

//	USHORT usType;
    BYTE szType[2+1];

    memset(bOutBuf, 0x00, sizeof(bOutBuf));
//	memset(&Sharls_COMData, 0x00, sizeof(STRUCT_SHARLS_COM));
	if(strTCT.fDemo == VS_FALSE)
    		usResult = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_GPRS_SIGNALEX, bInBuf, usInLen, bOutBuf, &usOutLen);

	memcpy(Sharls_COMData, &bOutBuf[1],sizeof(STRUCT_SHARLS_COM));
	
	*usEthType = Sharls_COMData->usEthType;
    inSignal = Sharls_COMData->bgGPRSSignal;
	inmedomstatus = Sharls_COMData->inReserved1;
	
	*usNetworkType = Sharls_COMData->usGPRSNetworkType;
	strcpy(szNetworkName, Sharls_COMData->szGPRSNetworkName);
	inGPRSConnected = Sharls_COMData->bgGPRSType;
	usHWSupport = (Sharls_COMData->szReserved1[0] | Sharls_COMData->szReserved1[1] << 8);

	vdDebug_LogPrintf("---inmedomstatus[%d],usHWSupport=[%04x]", inmedomstatus,usHWSupport);
    vdDebug_LogPrintf("---NETWORK TYPE[%02X][%s],inGPRSConnected=[%d]", *usNetworkType, szNetworkName,inGPRSConnected);					
    vdDebug_LogPrintf("---inCTOSS_GetGPRSSignal[%02X %02X %02X]", bOutBuf[0], bOutBuf[1], bOutBuf[2]);
	vdDebug_LogPrintf("---wifi status[%d],WIFISSID=[%s], signal[%d]", Sharls_COMData->usWIFIType,Sharls_COMData->szWIFISSID, Sharls_COMData->inReserved2);
	vdDebug_LogPrintf("---BT[%d],", Sharls_COMData->usBLUETOOTHType);
	vdDebug_LogPrintf("---inCTOSS_GetGPRSSignalEx1 > inMultiAP_RunIPCCmdTypesEx > usResult = [%d] > insignal = [%d],", usResult,inSignal);
	if(d_OK == usResult)
    {
        //status
        if(bOutBuf[0] == IPC_COMM_SUCC){
						if (inGPRSConnected == TRUE)
							fGPRSConnectOK = TRUE;
						else
							fGPRSConnectOK = FALSE;
							
            return inSignal;
        }	
        else{
						fGPRSConnectOK = FALSE;
						usResult = 0;
            return usResult;
        }
    }
		else{
			inSignal = 0;
			usResult = 0;
			fGPRSConnectOK = FALSE;
			return usResult;
		}
    
    return usResult;
}


int inCTOSS_COMMSetFont(char *font)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

	 vdDebug_LogPrintf("**inCTOSS_COMMSetFont START**");	
	 
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	strcpy(bInBuf,font);
	usInLen = strlen(bInBuf);
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_SETFONT, bInBuf, usInLen, bOutBuf, &usOutLen);

    return ST_SUCCESS;
}	

int inCTOSS_SIMGetIMSI(void)
{
	BYTE bInBuf[100];
	BYTE bOutBuf[100];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	BYTE imsi[128];

	vdDebug_LogPrintf("**inCTOSS_SIMGetIMSI**");	
	 
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_GPRS_GETIMSI, bInBuf, usInLen, bOutBuf, &usOutLen);
	if(d_OK == usResult)
    {
        if(bOutBuf[0] == IPC_COMM_SUCC)
        {
        	memset(imsi,0x00,sizeof(imsi));
			strcpy(imsi,&bOutBuf[1]);
			vdDebug_LogPrintf("imsi=[%s]",imsi);
            return d_OK;
        }
        else
            return d_NO;
    }
	return ST_SUCCESS;
}


int inCTOSS_SIMGetCCID(void)
{
	BYTE bInBuf[100];
	BYTE bOutBuf[100];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	BYTE szCCID[22+1];

	vdDebug_LogPrintf("**inCTOSS_SIMGetCCID**");	
	 
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_GPRS_GETCCID, bInBuf, usInLen, bOutBuf, &usOutLen);
	if(d_OK == usResult)
    {
        if(bOutBuf[0] == IPC_COMM_SUCC)
        {
        	memset(szCCID,0x00,sizeof(szCCID));
			strcpy(szCCID,&bOutBuf[1]);
			vdDebug_LogPrintf("szCCID=[%s]",szCCID);
            return d_OK;
        }
        else
            return d_NO;
    }
	return ST_SUCCESS;
}


int inCTOSS_SIMGetGPRSIP(void)
{
	BYTE bInBuf[100];
	BYTE bOutBuf[100];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	BYTE szGPRSIP[50+1];

	vdDebug_LogPrintf("**inCTOSS_SIMGetGPRSIP**");	
	 
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_GETGPRSIP, bInBuf, usInLen, bOutBuf, &usOutLen);
	if(d_OK == usResult)
    {
        if(bOutBuf[0] == IPC_COMM_SUCC)
        {
        	memset(szGPRSIP,0x00,sizeof(szGPRSIP));
			strcpy(szGPRSIP,&bOutBuf[1]);
			vdDebug_LogPrintf("szGPRSIP=[%s]",szGPRSIP);
            return d_OK;
        }
        else
            return d_NO;
    }
	return ST_SUCCESS;
}



int inCTOSS_SIMGetGPRSIPInfo(void)
{
	BYTE bInBuf[100];
	BYTE bOutBuf[1000];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	BYTE szGPRSIP[50+1];
	STRUCT_IP_INFO IPInfo;

	vdDebug_LogPrintf("**inCTOSS_SIMGetGPRSIPInfo**");	
	 
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_GETGPRSIPINFO, bInBuf, usInLen, bOutBuf, &usOutLen);
	if(d_OK == usResult)
    {
        if(bOutBuf[0] == IPC_COMM_SUCC)
        {
        	memset(&IPInfo,0x00,sizeof(STRUCT_IP_INFO));
			memcpy(&IPInfo,&bOutBuf[1],sizeof(STRUCT_IP_INFO));
			vdDebug_LogPrintf("szIP=[%s]",IPInfo.szIP);
			vdDebug_LogPrintf("szGetWay=[%s]",IPInfo.szGetWay);
			vdDebug_LogPrintf("szSubNetMask=[%s]",IPInfo.szSubNetMask);
			vdDebug_LogPrintf("szHostDNS1=[%s]",IPInfo.szHostDNS1);
			vdDebug_LogPrintf("szHostDNS2=[%s]",IPInfo.szHostDNS2);
            return d_OK;
        }
        else
            return d_NO;
    }
	return ST_SUCCESS;
}



int inCTOSS_COMMWIFISCAN(void)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

	 vdDebug_LogPrintf("**inCTOSS_COMMScanWIFI START**");	
	 
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_WIFISCAN, bInBuf, usInLen, bOutBuf, &usOutLen);

    return ST_SUCCESS;
}

int inCTOSS_COMMSetSleepMode(void)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

	 vdDebug_LogPrintf("**inCTOSS_COMMSetSleepMode START**");	
	 
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_SLEEPMODE, bInBuf, usInLen, bOutBuf, &usOutLen);

    return ST_SUCCESS;
}

int inCTOSS_COMMSetSIMSlot(int inSIMSlot)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

	vdDebug_LogPrintf("**inCTOSS_COMMSetSIMSlot START**");	
	vdCTOS_DispStatusMessage("PLEASE WAITING..."); 
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	sprintf(bInBuf,"%d",inSIMSlot);
	usInLen = strlen(bInBuf);
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_SETSIMSLOT, bInBuf, usInLen, bOutBuf, &usOutLen);

    return ST_SUCCESS;
}

typedef struct
{
	USHORT usResult;
    int inMode;
	BYTE	szIP[30] ;
    BYTE	szURL[100] ;

} STRUCT_URLPARSEIP;

int inCTOSS_COMMUrlParseIP(int inMode, char *szURLOrIP)
{
    BYTE bInBuf[500];
    BYTE bOutBuf[500];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
	STRUCT_URLPARSEIP UrlToIP;

	vdDebug_LogPrintf("**inCTOSS_COMMUrlParseIP**");	
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));
	
	memset(&UrlToIP, 0x00, sizeof(STRUCT_URLPARSEIP));
	UrlToIP.inMode = inMode;
	strcpy(UrlToIP.szURL,szURLOrIP);
	usInLen = sizeof(STRUCT_URLPARSEIP);
	memcpy(bInBuf,&UrlToIP,usInLen);
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_URLPARSEIP, bInBuf, usInLen, bOutBuf, &usOutLen);

	if(d_OK == usResult)
    {
        if(bOutBuf[0] == IPC_COMM_SUCC)
        {
        	memset(&UrlToIP, 0x00, sizeof(STRUCT_URLPARSEIP));
			memcpy(&UrlToIP,&bOutBuf[1],sizeof(STRUCT_URLPARSEIP));
			vdDebug_LogPrintf("usResult=[%x]",UrlToIP.usResult);
			vdDebug_LogPrintf("inMode=[%d]",UrlToIP.inMode);
			vdDebug_LogPrintf("szIP=[%s]",UrlToIP.szIP);
			vdDebug_LogPrintf("szURL=[%s]",UrlToIP.szURL);
			if (UrlToIP.usResult == d_OK)
				strcpy(szURLOrIP,UrlToIP.szIP);

            return d_OK;
        }
        else
            return d_NO;
    }
	return ST_SUCCESS;
	
}

int inCTOSS_COMMResetGPRS(void)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

	vdDebug_LogPrintf("**inCTOSS_COMMResetGPRS START**");	
    memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

    usResult = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_RESETGPRS, bInBuf, usInLen, bOutBuf, &usOutLen);

    return ST_SUCCESS;
}


int inCTOSS_SFTPConnect()
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;
	BYTE tmpbuf[100];
	

	unsigned char uszCURLURL[1000+1];
	unsigned char uszCURLAccount[500+1];

	vdDebug_LogPrintf("vdCTOSS_SFTPConnect");

	inCPTRead(1);

	//strTCT.fShareComEnable = 1;
	strCPT.inPriTxnHostPortNum = get_env_int("SFTPHOSTPORT"); // ftp server definitely  is port 21
	memset(tmpbuf,0x00,sizeof(tmpbuf));
	if ( inCTOSS_GetEnvDB ("SFTPHOSTIP", tmpbuf) == d_OK )
	{
		if (strlen(tmpbuf) > 0)
			strcpy(strCPT.szPriTxnHostIP, tmpbuf);
	}
	
	if (inCTOS_InitComm(strCPT.inCommunicationMode) != d_OK) 
	{
		return d_NO;
	}

	inCTOS_CheckInitComm(strCPT.inCommunicationMode); 
	if (srCommFuncPoint.inCheckComm(&srTransRec) != d_OK)
	{
		inCTOS_inDisconnect();
		return d_NO;
	}

	if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
	{
		inCTOS_inDisconnect();
		return d_NO;
	}
	
	
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));


	inMultiAP_Database_COM_ClearEx();
	//strcpy(strCOM.szATCMD1, "test");// Username to login SFTP Server
	//strcpy(strCOM.szATCMD2, "test");// Password to login SFTP Server
	//strcpy(strCOM.szATCMD3, "castles"); //Passphrase for the public key and key pair
	//strcpy(strCOM.szATCMD4, "��/data/data/pub/host.pub"); //path to public key (usually using .pub)
	//strcpy(strCOM.szATCMD5, "��/data/data/pub/host"); //path to keypair (this one doesn't use the format extension)
	memset(tmpbuf,0x00,sizeof(tmpbuf));
	if ( inCTOSS_GetEnvDB ("SFTPUSER", tmpbuf) == d_OK )
	{
		if (strlen(tmpbuf) > 0)
			strcpy(strCOM.szATCMD1, tmpbuf);
	}

	memset(tmpbuf,0x00,sizeof(tmpbuf));
	if ( inCTOSS_GetEnvDB ("SFTPPSWD", tmpbuf) == d_OK )
	{
		if (strlen(tmpbuf) > 0)
			strcpy(strCOM.szATCMD2, tmpbuf);
	}

	memset(tmpbuf,0x00,sizeof(tmpbuf));
	if ( inCTOSS_GetEnvDB ("SFTPKEYPASS", tmpbuf) == d_OK )
	{
		if (strlen(tmpbuf) > 0)
			strcpy(strCOM.szATCMD3, tmpbuf);
	}

	memset(tmpbuf,0x00,sizeof(tmpbuf));
	if ( inCTOSS_GetEnvDB ("SFTPPUBKEY", tmpbuf) == d_OK )
	{
		if (strlen(tmpbuf) > 0)
			strcpy(strCOM.szATCMD4, tmpbuf);
	}

	memset(tmpbuf,0x00,sizeof(tmpbuf));
	if ( inCTOSS_GetEnvDB ("SFTPKEYPAIR", tmpbuf) == d_OK )
	{
		if (strlen(tmpbuf) > 0)
			strcpy(strCOM.szATCMD5, tmpbuf);
	}
	inMultiAP_Database_COM_SaveEx();

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_SFTP_CONNECT, bInBuf, usInLen, bOutBuf, &usOutLen);
	if(d_OK == usResult)
    {
        if(bOutBuf[0] == IPC_COMM_SUCC)
        {
            return d_OK;
        }
        else
            return d_NO;
    }
	
  return 0;
}



int inCTOSS_SFTPDownload(char *szfilename)
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	vdDebug_LogPrintf("**vdCTOSS_SFTPDownload START**");	
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	inMultiAP_Database_COM_ClearEx();
	strcpy(strCOM.szCAFileName, szfilename); //save file name
	inMultiAP_Database_COM_SaveEx();
	
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_SFTP_DOWNLOAD, bInBuf, usInLen, bOutBuf, &usOutLen);

	if(d_OK == usResult)
    {
        if(bOutBuf[0] == IPC_COMM_SUCC)
        {
            return d_OK;
        }
        else
            return d_NO;
    }
	
	return ST_SUCCESS;
}

int inCTOSS_SFTPUpload(char *szfilename)
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	vdDebug_LogPrintf("**inCTOSS_SFTPUpload START**");	
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	inMultiAP_Database_COM_ClearEx();
	strcpy(strCOM.szCAFileName, szfilename); //save file name
	inMultiAP_Database_COM_SaveEx();
	
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_SFTP_UPLOAD, bInBuf, usInLen, bOutBuf, &usOutLen);

	if(d_OK == usResult)
    {
        if(bOutBuf[0] == IPC_COMM_SUCC)
        {
            return d_OK;
        }
        else
            return d_NO;
    }
	
	return ST_SUCCESS;
}





int inCTOSS_SFTPDisconnect()
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	vdDebug_LogPrintf("**vdCTOSS_SFTPDisconnect START**");	
	memset(bOutBuf, 0x00, sizeof(bOutBuf));
	memset(bInBuf, 0x00, sizeof(bInBuf));

	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_COM.SHARLS_COM", d_IPC_CMD_COMM_SFTP_DISCONNECT, bInBuf, usInLen, bOutBuf, &usOutLen);

	if(d_OK == usResult)
    {
        if(bOutBuf[0] == IPC_COMM_SUCC)
        {
            return d_OK;
        }
        else
            return d_NO;
    }
	
	return ST_SUCCESS;
}

void SFPTDownload()
{
	char szFileName[50];
	USHORT usResult;
	char pchRecBuf[1000];
	int inMaxRecSize;

	strcpy(szFileName,"testsftp.log");
	usResult = inCTOSS_SFTPConnect();
	if(d_OK != usResult)
		return;
	
	usResult = inCTOSS_SFTPDownload(szFileName);
	if(d_OK != usResult)
		return;

	inCTOSS_COMM_GetFileSize("/data/data/pub/testsftp.log");
	memset(pchRecBuf,0x00,sizeof(pchRecBuf));
	inMaxRecSize = sizeof(pchRecBuf);
	
	inCTOSS_COMM_ReadFile("/data/data/pub/testsftp.log",pchRecBuf,inMaxRecSize);
	vdDebug_LogPrintf("pchRecBuf =[%s] ", pchRecBuf);
	usResult = inCTOSS_SFTPDisconnect();
	if(d_OK != usResult)
		return;
}

void SFPTUpload()
{
	char szFileName[50];
	USHORT usResult;
	char pchRecBuf[1000];
	int inMaxRecSize;

	strcpy(szFileName,"/data/data/pub/TERMINAL.S3DB");
	usResult = inCTOSS_SFTPConnect();
	if(d_OK != usResult)
		return;
	
	usResult = inCTOSS_SFTPUpload(szFileName);
	if(d_OK != usResult)
		return;

	usResult = inCTOSS_SFTPDisconnect();
	if(d_OK != usResult)
		return;
}

int inWIFI_GetConnectConfig(void)
{
	vdDebug_LogPrintf("--inWIFI_GetConnectConfig--");
	
  	memset(szIP,0,sizeof(szIP));
   	memset(szDNS1,0,sizeof(szDNS1));
   	memset(szDNS2,0,sizeof(szDNS2));
   	memset(szGateWay,0,sizeof(szGateWay));
   	memset(szSubnetMask,0,sizeof(szSubnetMask));

    usCTOSS_GetWifiInfo();

	//vdDebug_LogPrintf("saturn connection TYPE %d", inGetConnectionType());

	return ST_SUCCESS;
}

int inMOBILE_GetConnectConfig(void)
{
  	memset(szIP,0,sizeof(szIP));
   	memset(szDNS1,0,sizeof(szDNS1));
   	memset(szDNS2,0,sizeof(szDNS2));
   	memset(szGateWay,0,sizeof(szGateWay));
   	memset(szSubnetMask,0,sizeof(szSubnetMask));

    usCTOSS_GetMobileInfo();

	//vdDebug_LogPrintf("saturn connection TYPE %d", inGetConnectionType());

	return ST_SUCCESS;
}

