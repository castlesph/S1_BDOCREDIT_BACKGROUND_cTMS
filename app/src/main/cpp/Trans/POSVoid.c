/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <typedef.h>


#include "..\Includes\POSTypedef.h"


#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Includes\POSbatch.h"
#include "..\ui\Display.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Accum\Accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"
#include "..\Debug\Debug.h"
#include "../Includes/POSSmac.h"
#include "../Erm/PosErm.h"


//enhance ecr
extern BOOL fECRTxnFlg;

/* For ECR -- sidumili */
extern BOOL fECRBuildSendOK; 
extern BOOL fTimeOutFlag; /*BDO: Flag for timeout --sidumili*/
/* For ECR -- sidumili */

extern VS_BOOL fPreConnectEx;


int inCTOS_VoidFlowProcess(void)
{
    int inRet = d_NO;

	fECRBuildSendOK = FALSE; /* BDO: For ECR -- sidumili */
	fTimeOutFlag = FALSE; /*BDO: Flag for timeout --sidumili*/
    vdCTOS_SetTransType(VOID);
    
    //display title
    //vdDispTransTitle(VOID); 

    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
		return inRet;

		
    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

#if 1
    //fPreConnectEx = FALSE;
    //if ((strTCT.fSingleComms) && (fPreConnectEx == FALSE) || (srTransRec.usTerminalCommunicationMode != WIFI_MODE)){ 
    if((strTCT.fSingleComms) && (fPreConnectEx == FALSE) || (srTransRec.usTerminalCommunicationMode != GPRS_MODE)){
		    inRet = inCTOS_PreConnectEx();

		if (inRet != d_OK)
			return inRet;
	}
#endif
	
    inRet = inCTOS_PreConnectAndInit(); /*for GPRS and WIFI only*/
    if(d_OK != inRet)
        return inRet;

	while(1)
	{
	    inRet = inCTOS_GeneralGetInvoice();
	    if(d_EDM_USER_CANCEL == inRet || 0xFF == inRet)
	        return inRet;

	    if (inMultiAP_CheckMainAPStatus() == d_OK)
	    {
	        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_VOID_SALE);
			if(d_NOT_RECORD == inRet)
			{
				if(fGetECRTransactionFlg() == TRUE)
					return inRet;
				else
					continue;
			}
	        else if(d_OK != inRet)
	            return inRet;

	    }
	    else
	    {
	        if (inMultiAP_CheckSubAPStatus() == d_OK)
	        {
				vdDebug_LogPrintf("inCTOS_GeneralGetInvoice > inMultiAP_CheckSubAPStatus > ok");		//Tine:log for debugging use only
	            inRet = inCTOS_MultiAPGetVoid();
	            if(d_OK != inRet)
	                return inRet;
	        }       
	        inRet = inCTOS_BatchSearch();
			if(d_NOT_RECORD == inRet)
			{
				if(fGetECRTransactionFlg() == TRUE)
					return inRet;
				else
					continue;
			}
	        else if(d_OK != inRet)
	            return inRet;

	    }
		
		srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;//If Sale used Comms Fallback, need to set srTransRec.usTerminalCommunicationMode to value in CPT.
		
		memcpy(srTransRec.szTempTime, srTransRec.szTime, TIME_BCD_SIZE); /*get original time*/

		//android:  for debugging only, check transaction year
		vdDebug_LogPrintf("srTransRec.szYear = [%s]", srTransRec.szYear);
		
	    inRet = inCTOS_CheckVOID();
	    if(d_OK != inRet)
	    {
			if (fGetECRTransactionFlg() == TRUE)
			{
			    strcpy(srTransRec.szRespCode,"");
				strcpy(srTransRec.szECRRespCode, ECR_OPER_CANCEL_RESP);
				return inRet;
			}
			else
			{
				//vdCTOS_SetTransType(VOID);//Set transtype to void to fix issue where void header is overwritten by CARD VER/BAL INQ.
				continue;
			}
	    }
		else
			break;

	}
    inRet = inCTOS_LoadCDTandIIT();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_VoidSelectHostNoPreConnect();
    if(d_OK != inRet)
        return inRet;

	//if(strTCT.fATPBinRoute == TRUE && srTransRec.HDTid == 1 && srTransRec.fDualBrandedCredit == FALSE)//Use ATPBinRouting NII if ATPBinRoute is enabled for void.
	if(srTransRec.fBINRouteApproved == TRUE && srTransRec.HDTid == 1 && srTransRec.fDualBrandedCredit == FALSE)//Use ATPBinRouting NII if ATPBinRoute is enabled for void.
		memcpy(strHDT.szTPDU,strTCT.ATPTPDU,5);

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet){
		
		//if (fGetECRTransactionFlg() == TRUE){
		    strcpy(srTransRec.szRespCode,"");
			strcpy(srTransRec.szECRRespCode, ECR_OPER_CANCEL_RESP);
		//}
			
        return inRet;

    }
    
    inRet = inCTOS_ConfirmInvAmt();
	if(d_OK != inRet){		
		    strcpy(srTransRec.szRespCode,"");
			strcpy(srTransRec.szECRRespCode, ECR_OPER_CANCEL_RESP);		
        return inRet;
	}
	
	

	//if ((strTCT.fSingleComms != TRUE) || (srTransRec.usTerminalCommunicationMode == WIFI_MODE)){	
	if ((strTCT.fSingleComms != TRUE) || (srTransRec.usTerminalCommunicationMode == GPRS_MODE)){
		inRet = inCTOS_PreConnect();
		if(d_OK != inRet)
			return inRet;
	}


	if(inCheckIfSMCardTransRec() == TRUE)//Set the below values before voiding for the handling of card name and balance on void printing.
	{
		srTransRec.fPrintSMCardHolder = FALSE;
		srTransRec.fPrintCardHolderBal = FALSE;
		memset(srTransRec.SmacBalance,0x00,sizeof(srTransRec.SmacBalance));
		memset(srTransRec.szSMACBDORewardsBalance,0x00,sizeof(srTransRec.szSMACBDORewardsBalance));
	}

	vdDebug_LogPrintf("inBuildAndSendIsoData - START!!!");
    inRet = inBuildAndSendIsoData();
	vdDebug_LogPrintf("inBuildAndSendIsoData - END!!!, inRet=[%d]", inRet);		//Tine:  log for debugging only
    if(d_OK != inRet){

		/*BDO: Set ECR response code to EN - COMM ERROR -- sidumili*/
		if ((strlen(srTransRec.szRespCode) <= 0) || (srTransRec.shTransResult == TRANS_TERMINATE)){
			strcpy(srTransRec.szRespCode,"");
			strcpy(srTransRec.szECRRespCode, ECR_COMMS_ERR);
		}
		
		return inRet;
    }

	//vdUpdateSMACAmount(); /*BDO: For SMAC which not save the DE04 response due its smac amount balance*/

    if(memcmp(srTransRec.szTempTime, srTransRec.szTime, TIME_BCD_SIZE) == 0) /*if time are the same it means there is no DE12 receive, get new date and time*/
        vdGetTimeDate(&srTransRec);
	
    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

	//if(strTCT.fDCC && srTransRec.fDCC)
	//{ 		
		//inMyFile_TransLogSave(&srTransRec, DCC_LOGGING);
	//} 

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

	// patrick add code 20141205 start, modified -- sidumili
	fECRBuildSendOK = TRUE;	
	if (fGetECRTransactionFlg()){	
	    inRet = inMultiAP_ECRSendSuccessResponse();
	}
	// patrick add code 20141205 start, modified -- sidumili


	inRet=inDisconnectVoidIfNoPendingADVICEandUPLOAD(&srTransRec, strHDT.inNumAdv);


    inRet = ushCTOS_printReceipt();
    if(d_OK != inRet)
        return inRet;
    else
        vdSetErrorMessage("");

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);

    if (!srTransRec.byOffline)
    {
        CTOS_LCDTClearDisplay();

		if(strTCT.fDCC && srTransRec.fDCC)
        {

            inRet=inProcessTransLogTrans(&srTransRec, 1, 2, GET_ALL_EXCEPT_TIP);
            if(d_OK != inRet)
                return inRet;
            else
                vdSetErrorMessage("");
		}

        inRet=inProcessAdviceTrans(&srTransRec, strHDT.inNumAdv);
        if(d_OK != inRet)
            return inRet;
        else
            vdSetErrorMessage("");
		
        if(strTCT.fDCC && srTransRec.fDCC)
        {
#if 0
            inRet=inProcessTransLogTrans(&srTransRec, 1, 2, GET_ALL_EXCEPT_TIP);
            if(d_OK != inRet)
                return inRet;
            else
                vdSetErrorMessage("");
#endif            
            inRet=inProcessTransLogTrans(&srTransRec, 1, 2, GET_TIP_ONLY);
            if(d_OK != inRet)
                return inRet;
            else
                vdSetErrorMessage("");
        }
    }
	
    return d_OK;
}

int inCTOS_VOID(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();

    
    vdCTOS_TxnsBeginInit();

	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;
	
    if(fGetECRTransactionFlg() != TRUE)
        inCTOSS_MultiStopResumeAP("SHARLS_ECRBDO");
    
    inRet = inCTOS_VoidFlowProcess();

	vdDebug_LogPrintf("inCTOS_VoidFlowProcess inRet[%d]", inRet);
	
    inCTOS_inDisconnect();

	
	vdDebug_LogPrintf("inCTOS_inDisconnect inRet[%d]", inRet);

	if(d_OK == inRet)
	inCTOSS_UploadReceipt();
    
    if(fGetECRTransactionFlg() != TRUE)
        inCTOSS_MultiResumeContinueAP("SHARLS_ECRBDO");
	
	/* Send response to ECR -- sidumili */
	if (fGetECRTransactionFlg()){	
		inMultiAP_ECRSendSuccessResponse();
	}	
	fECRBuildSendOK = FALSE;
	/* Send response to ECR -- sidumili */

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();
	vdDebug_LogPrintf("vdCTOS_TransEndReset inRet[%d]", inRet);


    return inRet;
}
