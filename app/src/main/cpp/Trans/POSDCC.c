/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>
#include <EMVAPLib.h>
#include <EMVLib.h>


#include "..\Includes\POSTypedef.h"
#include "..\Debug\Debug.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSLogon.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
//#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "../Database/dct.h"
#include "..\Includes\LocalAptrans.h"
#include "..\Includes\LocalFunc.h"
#include "../POWRFAIL/POSPOWRFAIL.h"
#include "..\Includes\POSDCC.h"

//BOOL fOptOutFlag = 0;
extern BOOL fECRBuildSendOK; 
extern BOOL fTimeOutFlag; /*BDO: Flag for timeout --sidumili*/
extern VS_BOOL fPreConnectEx;
char szFuncTitleName [21 + 1]; //aaronnino for BDOCLG ver 9.0 fix on issue #0093 Have a function title for function keys shorcut 1 of  6
BOOL fBDOOptOutHostEnabled = FALSE;

int inCTOS_ProcessDCCRate(void)
{
    int inRet = d_NO, inRet2 = d_NO;
    int result;
    char szTemp[15+1];
	int inHostIndex=0, inLen=0;
	char szStr[16+1]={0};

	VS_BOOL fOptOut = FALSE;
	
	memcpy(srTransRec.szDCCLocalCur, strCST.szCurCode,3);
    memcpy(srTransRec.szDCCLocalSymbol, strCST.szCurSymbol,3);
    
    inRet=inCTOS_LocalAPSaveData();
    if(d_OK != inRet)
        return inRet;
    
    strCDT.HDTid=6; //Dcc default host
    
    inRet = inCTOS_SelectHost();
    if(d_OK != inRet)
        return inRet;
    
    srTransRec.byPackType=DCC_RATEREQUEST;
    
    if(strTCT.fSingleComms != TRUE)
    {
        inRet = inCTOS_PreConnect();
        if(d_OK != inRet)
            return inRet;
    }

    /*commented - improve performance*/
	#if 0
	if(inCPTRead(1) != d_OK)
	{
		vdSetErrorMessage("LOAD CPT ERR");
		return(d_NO);
	} 
    #endif
	
    inRet = inProcessRequestDCC();
	vdDebug_LogPrintf("inProcessRequestDCC: (%d)", inRet);
    if(d_OK != inRet)
    {
		if(inRet == ST_CONNECT_FAILED)
		{
			vdSetECRResponse(ECR_COMMS_ERR);//Set the response to ECR to COMM ERROR 
            return inRet;
		}

		if(inRet == ST_COMMS_DISCONNECT)
		{
			vdSetECRResponse(ECR_COMMS_ERR);//Set the response to ECR to COMM ERROR 
			vdDisplayErrorMsgResp2(" Connection ","Terminated","Please Try Again");
			return inRet;
		}
		inRet2 = inCTOS_LocalAPGetData();
		if(d_OK != inRet2)
			return inRet2;

		vdDebug_LogPrintf("TESTLANG OPTOUT %d", srTransRec.byTransType);
       if (srTransRec.byTransType == SALE_OFFLINE){
	   	
			fOptOut = inFLGGet("fCompOptOut");

			vdDebug_LogPrintf("testlang compopt out");


			if (fOptOut == 1){

                vdDebug_LogPrintf("testlang reload comp data");
				if(inHDTReadData(BDO_COMPLETION_OPTOUT_HDT_INDEX) != d_OK)
					 vdDebug_LogPrintf("COMPLETION OPTOUT HOST DISABLED");		
	    
				if(strHDT_Temp.fHostEnable == TRUE)
					fBDOOptOutHostEnabled = TRUE;

			}
				
       }else{

	   		fOptOut = inFLGGet("fStraightOptOut");
	   
	   		if (fOptOut == 1){

	   			if(inHDTReadData(BDO_OPTOUT_HDT_INDEX) != d_OK)
					vdDebug_LogPrintf("OPTOUT HOST DISABLED");
		
				if(strHDT_Temp.fHostEnable == TRUE)
					fBDOOptOutHostEnabled = TRUE;
       		}

       	}
	   
		inRet2 = inCTOS_LocalAPReloadTable();
		if(d_OK != inRet2)
			return inRet2;

		srTransRec.usTerminalCommunicationMode=strCPT.inCommunicationMode;
			
        if(inRet == ST_SEND_DATA_ERR || (inRet == ST_RECEIVE_TIMEOUT_ERR && strCPT.inCommunicationMode != DIAL_UP_MODE))
        {
			inCTOS_inDisconnect();
			//vdCTOS_DispStatusMessage("PROCESSING...");

			#ifdef ANDROID_NEW_UI
				//vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
				//CTOS_Delay(300);
			#endif
							
            //inRet = inCTOS_PreConnect();
            inRet = inCTOS_PreConnectAndInit();
            if(d_OK != inRet)
                return inRet;
        }
        else if(inRet == TRANS_REJECT_APPROVED)
			inRet = d_OK;

		return inRet;
    }
	
    inRet = inDisplayDCCRateScreen();
    if(d_OK != inRet)
    {
		vdSetECRResponse(ECR_OPER_CANCEL_RESP);
        return inRet;
    }

	if( (srTransRec.byTransType == SALE || srTransRec.byTransType == SALE_OFFLINE) 
		&&  (strTCT.inDCCMode == AUTO_OPTOUT_MODE || strTCT.inDCCMode == MANUAL_OPTOUT_MODE))
	{
		srTransRec.fDCC = VS_TRUE;			
		memset(srTransRec.szDCCLocalAmount,0x00,sizeof(srTransRec.szDCCLocalAmount));
		memcpy(srTransRec.szDCCLocalAmount, srTransRec.szTotalAmount,sizeof(srTransRec.szTotalAmount));
	}
	else
	{
		vdDebug_LogPrintf("strTCT.inDCCMode[%d]",strTCT.inDCCMode);
		if(strTCT.inDCCMode == PRINT_DCC_RATES)
		{
			vdCTOSS_PrinterStart(100);
		    CTOS_PrinterSetHeatLevel(strTCT.inPrinterHeatLevel);
		    if( printCheckPaper()==-1)
		    	return d_NO;

			ushCTOS_PrintHeader(MERCHANT_COPY_RECEIPT);
  			ushCTOS_PrintDCCRateBody(MERCHANT_COPY_RECEIPT);
			ushCTOS_PrintDCCRateFooterEx(MERCHANT_COPY_RECEIPT);
			
			vdCTOSS_PrinterEnd();
			
		}
	}
	
#if 1
	if(strCPT.inCommunicationMode == ETHERNET_MODE)
	{
		inRet = inCheckEthernetConnected();
		vdDebug_LogPrintf("*** inCheckEthernetConnected END ***");
		//else if(strCPT.inCommunicationMode == DIAL_UP_MODE)
			//inRet = inModem_InitModem(&srTransRec,1);
			
		if(SUCCESS != inRet)
		{
			vdDisplayErrorMsgResp2(" Connection ","Terminated","Please Try Again");
			vdSetECRResponse(ECR_COMMS_ERR);
			return inRet;
		}
	}
#endif
    //vdCTOS_SetTransType(SALE);						   
    //vdDispTransTitle(SALE);
    
    //dcc selection
    if(srTransRec.fDCC == CN_TRUE)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        wub_str_2_hex(srTransRec.szDCCCurAmt, szTemp, 12);
        
        memset(srTransRec.szBaseAmount, 0x00, sizeof(srTransRec.szBaseAmount));
        memcpy(srTransRec.szBaseAmount, szTemp, sizeof(szTemp));
        
        memset(srTransRec.szTotalAmount, 0x00, sizeof(srTransRec.szTotalAmount));
        memcpy(srTransRec.szTotalAmount, srTransRec.szBaseAmount, sizeof(srTransRec.szBaseAmount));

	    inDatabase_TerminalOpenDatabase();
        //vdDebug_LogPrintf("srTransRec.szDCCCur: (%s)", srTransRec.szDCCCur); 
        inCSTReadHostIDEx(srTransRec.szDCCCur);

        //vdDebug_LogPrintf("1. strCST.HDTid: (%d)", strCST.HDTid);
        //vdDebug_LogPrintf("1. srTransRec.HDTid: (%d)", srTransRec.HDTid);
        
        inHostIndex = (short)strCST.HDTid;
        strCDT.HDTid=inHostIndex; 

		vdDebug_LogPrintf("1. inHostIndex: (%d)", inHostIndex);
		
        if(inHDTReadEx(inHostIndex) != d_OK)
        {
            //vdSetErrorMessage("HOST SELECTION ERR");
			inHDTReadDataEx(inHostIndex);
			strcpy(szStr,strHDT_Temp.szHostLabel);
			memset(strHDT_Temp.szHostLabel,0x00,sizeof(strHDT_Temp.szHostLabel));
			sprintf(strHDT_Temp.szHostLabel," %s ",szStr);
			inDatabase_TerminalCloseDatabase();
			vdDisplayErrorMsgResp2(strHDT_Temp.szHostLabel, "TRANSACTION", "NOT ALLOWED");
            return(d_NO);
        } 
        else 
        {
			srTransRec.HDTid=inHostIndex;

			if(strTCT.fSingleComms == TRUE)
				inHostIndex = 1;
			
			if(inCPTReadEx(inHostIndex) != d_OK)
			{
				inDatabase_TerminalCloseDatabase();
				vdSetErrorMessage("LOAD CPT ERR");
				return(d_NO);
			}
        }

	    inMMTReadRecordEx(srTransRec.HDTid,srTransRec.MITid);
		
		memset(strMMT[0].szTID,0x00,sizeof(strMMT[0].szTID));
        memcpy(strMMT[0].szTID, srTransRec.szDCCFXTID, 8);
		inLen=strlen(srTransRec.szDCCFXMID);
		memset(szTemp, 0x30, sizeof(szTemp));
        memcpy(&szTemp[15-inLen], srTransRec.szDCCFXMID, inLen);
		memset(strMMT[0].szMID,0x00,sizeof(strMMT[0].szMID));
        memcpy(strMMT[0].szMID, szTemp, 15);
		
        inMMTSaveEx(strMMT[0].MMTid);
		inDatabase_TerminalCloseDatabase();
        //inMMTReadRecord(srTransRec.HDTid,srTransRec.MITid);
    }
    else
    {
		inCTOS_LocalAPGetData();
		if(d_OK != inRet){
			inDatabase_TerminalCloseDatabase();
	       return inRet;
		}
			
        inRet = inCTOS_LocalAPReloadTable();
        if(d_OK != inRet){
			inDatabase_TerminalCloseDatabase();
            return inRet;
        }
    }

    //CTOS_LCDTClearDisplay();    
    //vdCTOS_DispStatusMessage("PROCESSING...       ");
   
    return inRet;
}

int inProcessRequestDCC(void)
{
    int inResult,i,iLine;
    char szErrorMessage[30+1];
    char szBcd[INVOICE_BCD_SIZE+1];
    TRANS_DATA_TABLE *srTransPara;
    TRANS_DATA_TABLE srTransParaTmp;
	int inRetry=2;
	
  	srTransRec.ulTraceNum = wub_bcd_2_long(strHDT.szDCCRateandLogTraceNo,3);
		
    
    if ((VOID != srTransRec.byTransType) && (SALE_TIP != srTransRec.byTransType))
        srTransRec.ulOrgTraceNum = srTransRec.ulTraceNum;

	memcpy(strHDT_Temp.szDCCRateandLogTraceNo, strHDT.szDCCRateandLogTraceNo , INVOICE_BCD_SIZE);
    inHDTDCCSave(6);

    srTransPara = &srTransRec;

	CTOS_LCDTClearDisplay();
	
	iLine = ((strTCT.byTerminalType%2)?3:4);
	vduiClearBelow(iLine);
	setLCDPrint(iLine, DISPLAY_POSITION_LEFT, "Connecting to");
	setLCDPrint(iLine+1, DISPLAY_POSITION_LEFT, "DCC FX RATE HOST...");

	//vdCTOS_DispStatusMessage("PROCESSING...");

#if 0				
	if (2 == strTCT.byTerminalType)
		setLCDPrint(V3_ERROR_LINE_ROW, DISPLAY_POSITION_LEFT, "Processing...        ");
	else	
    	setLCDPrint(8, DISPLAY_POSITION_LEFT, "Processing...        ");  
#endif

	srTransPara->byOffline = CN_FALSE;
	if(srTransRec.usTerminalCommunicationMode == GPRS_MODE || srTransRec.usTerminalCommunicationMode == WIFI_MODE)	
	{
		inResult = inCTOS_PreConnect();//Fix for CRITICAL ERROR on GPRS
		if(d_OK != inResult)
			return inResult;
	}

    if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS)
    {
        if(srTransPara->shTransResult == 0)
            srTransPara->shTransResult = TRANS_COMM_ERROR;
        inCTOS_inDisconnect();
        //vdSetErrorMessage("CONNECT FAILED");
        
		if(strCPT.fCommBackUpMode == CN_TRUE) //Comms fallback -- jzg
		{
		
			if(inCTOS_CommsFallback(strHDT.inHostIndex) != d_OK) //Comms fallback -- jzg
				return ST_CONNECT_FAILED;;
			
			if (srCommFuncPoint.inConnect(&srTransRec) != ST_SUCCESS){
				inCTOS_inDisconnect();
				return ST_CONNECT_FAILED;
			}else{
				fCommAlreadyOPen = VS_TRUE;
			}
			
		}else
        	return ST_CONNECT_FAILED;
    }	

    //vdIncSTAN(srTransPara);
    vdIncDCCSTAN(srTransPara);
			
    do
    {

        inResult = inBuildDCCOnlineMsg(srTransPara);

		if(srTransPara->shTransResult == TRANS_REJECTED)
			return ST_ERROR;

		if(srTransPara->shTransResult == TRANS_REJECT_APPROVED)
			return TRANS_REJECT_APPROVED;
		
		if(inResult != ST_RECEIVE_TIMEOUT_ERR)
			return inResult;
			
        if(inResult != ST_SUCCESS)
        {
			inRetry--;
			if(inRetry <= 0)
			{
                inTCTSave(1);
                if (ST_RESP_MATCH_ERR == inResult)
                {
                    vdDebug_LogPrintf("inBuildOnlineMsg %d",inResult);
                    return inResult;
                }
                return inResult;
			}
			else
                srTransPara->byPackType = DCC_RATEREQUEST_RETRY;
        }
        else
            break;			
    }while(inRetry != 0);

    vdDebug_LogPrintf("srTransPara->szRespCode (%s)", srTransPara->szRespCode);
	
	if (memcmp(srTransPara->szRespCode, "00", 2))
		return ST_ERROR;

    return ST_SUCCESS;
}

int inCheckDCCRespCode(TRANS_DATA_TABLE *srTransPara)
{
    int inResult=TRANS_COMM_ERROR;
    char szErrorMessage[30+1];
	
    vdDebug_LogPrintf("inCheckHostRespCode %s",srTransPara->szRespCode);
    if (!memcmp(srTransPara->szRespCode, "00", 2))
    {
        inResult = TRANS_AUTHORIZED;
        srTransPara->shTransResult = TRANS_AUTHORIZED;
        strcpy(srTransPara->szRespCode, "00");
        DebugAddSTR("txn approval",srTransPara->szAuthCode ,6);  
    }
    else
    {
	    if((srTransPara->szRespCode[0] >= '0' && srTransPara->szRespCode[0] <= '9') &&
	    (srTransPara->szRespCode[1] >= '0' && srTransPara->szRespCode[1] <= '9'))
	    {
			memset(szErrorMessage, 0x00, sizeof(szErrorMessage));
			sprintf(szErrorMessage,"RC:%02d", atoi(srTransPara->szRespCode));
			if(atoi(srTransPara->szRespCode) == 12 || atoi(srTransPara->szRespCode) == 16 || atoi(srTransPara->szRespCode) == 21
			|| atoi(srTransPara->szRespCode) == 31 || atoi(srTransPara->szRespCode) == 41 || atoi(srTransPara->szRespCode) > 90) 
			{
				vdDisplayErrorMsgResp3("PROCESSING TXN", "AS PHP", "PLEASE CALL BDO", szErrorMessage);
				inResult = TRANS_REJECT_APPROVED;
			}
			else
			{
				vdDisplayErrorMsgResp2(" ", "PLEASE CALL BDO", szErrorMessage);
				inResult = TRANS_REJECTED;
			}		
	        
	    }
	    else
	        inResult = ST_UNPACK_DATA_ERR;
    }
	
    return (inResult);
}


int inRevertToPHP(void)
{
	int inRet = d_NO;
    BYTE szTempAuthCode[AUTH_CODE_DIGITS+1];
	TRANS_DATA_TABLE srtmpTransRec;

	memset(&srtmpTransRec,0x00,sizeof(TRANS_DATA_TABLE));
	memcpy(&srtmpTransRec,&srTransRec,sizeof(TRANS_DATA_TABLE));

	inRet = inSendPendingDCCLogs();
	if(d_OK != inRet)
    {
		return VS_OPTOUT_FAILED;		
    }

	inRet = inCTOS_CheckMustSettleBDOHost();
	if(d_OK != inRet)
    {
		vdDisplayErrorMsgResp2("","MUST SETTLE","");
		vdSetECRResponse(ECR_OPER_CANCEL_RESP);
		return inRet;
    }

	if (strMMT_Temp[0].MMTid == 0 && strMMT_Temp[0].HDTid == 0 )
         {
              vdDisplayErrorMsgResp2(" ", "OPT OUT", "NOT ALLOWED");
              return d_NO;
         }

	if(srTransRec.usTerminalCommunicationMode == ETHERNET_MODE)
	{
		if(inCheckEthernetSocketConnected() == BROKEN_PIPE || inCheckEthernetConnected() == ST_COMMS_DISCONNECT)
		{
			vdDisplayErrorMsgResp2("PLEASE RETRY","OPT OUT","TRANSACTION");
			return ST_COMMS_DISCONNECT;	
		}
	}
	else if(strCPT.inCommunicationMode == DIAL_UP_MODE)
	{
		if(inCheckModemConnected() != d_OK)
		{
			vdDisplayErrorMsgResp2("PLEASE RETRY","OPT OUT","TRANSACTION");
			return ST_COMMS_DISCONNECT;
		}
	}
	
	inRet = inCTOS_CheckVOID();
    if(d_OK != inRet)
    {
		return inRet;		
    }

	if(srTransRec.byOrgTransType == SALE || srTransRec.byOrgTransType == SALE_OFFLINE)
	{
		DebugAddHEX("OptOutTransType INV",srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
		
		srTransRec.byOptOutOrigTransType = srTransRec.byOrgTransType;
		inRet = inDatabase_OptOutbyTransTypeBatchUpdate(srTransRec.byOptOutOrigTransType, srTransRec.szInvoiceNo);// SAVE TO BATCH INCASE SALE/OFFLINE FAILS
		
	}
	
	inRet = inVoidDCCTransaction();
	if(d_OK != inRet)
	{
		//vdDisplayErrorMsgResp2("CANNOT OPT OUT","CONTINUING","DCC TRANSACTION");
		vdDisplayErrorMsgResp2("PLEASE RETRY","OPT OUT","TRANSACTION");
		return inRet;
	}

	memcpy(srTransRec.szOptOutVoidInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
		
	if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
	{
		vdResetAllCardData();
		vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);//Fix where byEntryMode is set to 0 by vdResetAllCardData().
		inRet = inCTOS_EMVCardReadProcess();
		if(d_OK != inRet)
			return inRet;
	}
	
	srTransRec.fDCC = VS_FALSE;
	fAdviceTras = VS_FALSE;

		
	memset(szTempAuthCode,0x00,AUTH_CODE_DIGITS+1);
	memcpy(szTempAuthCode,srTransRec.szAuthCode,strlen(srTransRec.szAuthCode));
	
	inRet = inCTOS_LocalAPGetData();
	if(d_OK != inRet)
		return inRet;


	inRet = inCTOS_LocalAPReloadTableEx();
	if(d_OK != inRet)
		return inRet;

	if(fBDOOptOutHostEnabled == TRUE){
		//inMMTReadRecord(BDO_OPTOUT_HDT_INDEX,srTransRec.MITid);	
		if (srTransRec.byTransType == SALE_OFFLINE)
		   inMMTReadRecord(BDO_COMPLETION_OPTOUT_HDT_INDEX,srTransRec.MITid);	
		else   
		   inMMTReadRecord(BDO_OPTOUT_HDT_INDEX,srTransRec.MITid);	
	}else
		inMMTReadRecord(BDO_HDT_INDEX,srTransRec.MITid);
	
	memcpy(srTransRec.szAuthCode,szTempAuthCode,strlen(szTempAuthCode));//Fix for blank AUTH CODE on completion for DCC Semi-Aggressive mode.
	
	inRet = inCTOS_EMVProcessing();
	if(d_OK != inRet)
    	return inRet;  

	inRet = inCTOS_GetInvoice();
    if(d_OK != inRet)
        return inRet;
	
	inRet = inBuildAndSendIsoData();

    if(d_OK != inRet)
	{

		/*BDO: Set ECR response code to EN - COMM ERROR -- sidumili*/
		if (strlen(srTransRec.szRespCode) <= 0)
		{
			strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);
		}

		
        return VS_OPTOUT_FAILED;
    }

	inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

	inRet=inDisconnectIfNoPendingADVICEandUPLOAD(&srTransRec, strHDT.inNumAdv);
    if(d_OK != inRet)
        return inRet;
	
	fECRBuildSendOK = TRUE;	
	if (fECRBuildSendOK){	
	    inRet = inMultiAP_ECRSendSuccessResponse();
	}

    inRet = ushCTOS_printOptOutReceipt();
		
    if(d_OK != inRet)
        return inRet;

	inRet = inDatabase_OptOutCompleteBatchUpdate(srTransRec.szOptOutVoidInvoiceNo);
	if(d_OK != inRet)
	{
		vdDisplayErrorMsgResp2("OPT OUT", "BATCH UPDATE", "FAILED");
		return inRet;
	}
	
    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

	CTOS_LCDTClearDisplay(); //BDO UAT 0012: Merchant copy still being displayed during TC Upload -- jzg
		
    inRet = inCTOS_EMVTCUpload();
#if 0	
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
#endif
	CTOS_LCDTClearDisplay(); //BDO UAT 0012: Merchant copy still being displayed during Advice Upload -- sidumili
	
	inRet=inProcessAdviceTrans(&srTransRec, strHDT.inNumAdv);
#if 0
	if(d_OK != inRet)
		return inRet;
	else
		vdSetErrorMessage("");
#endif


	memset(&srTransRec,0x00,sizeof(TRANS_DATA_TABLE));
	memcpy(&srTransRec,&srtmpTransRec,sizeof(TRANS_DATA_TABLE));

    return d_OK;
	
}

int inSendPendingDCCLogs(void)
{
	int inRet = d_NO;

	if(srTransRec.byTransType != SALE_OFFLINE)
	{
		inRet = inCTOS_EMVTCUpload();
	    if(d_OK != inRet)
	        return inRet;
	    else
			vdSetErrorMessage("");
	}

				
    inRet=inProcessTransLogTrans(&srTransRec, 1, 2, GET_ALL_EXCEPT_TIP);
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

	return d_OK;

}

int inVoidDCCTransaction(void)
{
	int inRet = d_NO;

	srTransRec.fOptOut= VS_TRUE;
	srTransRec.byTransType = srTransRec.byPackType = VOID;
	fAdviceTras = VS_FALSE;

	inHDTRead(srTransRec.HDTid); 
	inRet = inBuildAndSendIsoData();
    if(d_OK != inRet){

		/*BDO: Set ECR response code to EN - COMM ERROR -- sidumili*/
		if ((strlen(srTransRec.szRespCode) <= 0) || (srTransRec.shTransResult == TRANS_TERMINATE)){
			strcpy(srTransRec.szRespCode,"");
			strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);
		}

		if(strTCT.inDCCMode == AUTO_OPTOUT_MODE)
			vdCTOS_SetTransType(SALE);
		
		return inRet;
    }

	if(memcmp(srTransRec.szTempTime, srTransRec.szTime, TIME_BCD_SIZE) == 0) /*if time are the same it means there is no DE12 receive, get new date and time*/
        vdGetTimeDate(&srTransRec);

	srTransRec.fOptOutVoided = TRUE;
	
    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

	strHDT.inHostIndex = srTransRec.HDTid;
    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;


    inRet=inProcessTransLogTrans(&srTransRec, 1, 2, GET_ALL_EXCEPT_TIP);
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");
	

	return d_OK;
}

