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
#include "..\Includes\POSTipAdjust.h"
#include "..\Includes\POSbatch.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\ui\Display.h"
#include "..\accum\accum.h"
#include "..\print\Print.h"
#include "..\Comm\V5Comm.h"
#include "..\Includes\MultiApLib.h"
#include "..\Aptrans\MultiAptrans.h"
#include "../Debug/debug.h"


int inCTOS_TipAdjustFlowProcess(void)
{
    int inRet = d_NO,
    inResult = 0;
	char szTemp[200 + 1] = {0};
	char szTipAmt[20 + 1] = {0};
	char szAmtTmp1[20 + 1] = {0};
	char szAmtTmp2[20 + 1] = {0};
	char szDisplayBuf[512 + 1] = {0};
	
    vdCTOS_SetTransType(SALE_TIP);
    
    //display title
    //vdDispTransTitle(SALE_TIP);

    inRet = inCTOS_CheckTipAllowd();
    if(d_OK != inRet)
        return inRet;
       
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckTranAllowd();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_GeneralGetInvoice();
    //Tine:  02April2019
    vdDebug_LogPrintf("DONE: inCTOS_GeneralGetInvoice, inret=[%d]",inRet);
    if(d_OK != inRet)
        return inRet;

	//Tine:  02April2019
	vdDebug_LogPrintf("DONE: inCTOS_GeneralGetInvoice = d_OK");
	
    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inRet = inCTOS_MultiAPBatchSearch(d_IPC_CMD_TIP_ADJUST);
        if(d_OK != inRet)
            return inRet;

    }
    else
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
        {
            inRet = inCTOS_MultiAPGetVoid();
            if(d_OK != inRet)
                return inRet;
        }       
        inRet = inCTOS_BatchSearch();
		//Tine:  02April2019
		vdDebug_LogPrintf("DONE: inCTOS_BatchSearch");
		
        if(d_OK != inRet)
            return inRet;
    }

	//srTransRec.byUploaded = CN_FALSE;
	
	inRet = inCTOS_CheckHOSTTipAllowd();
	//Tine:  02April2019
	vdDebug_LogPrintf("DONE: inCTOS_CheckHOSTTipAllowd");
	if(d_OK != inRet)
		return inRet;

    inRet = inCTOS_CheckTipadjust();
	//Tine:  02April2019
	vdDebug_LogPrintf("DONE: inCTOS_CheckTipadjust");
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_LoadCDTandIIT();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_VoidSelectHostNoPreConnect();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_CheckMustSettle();
    if(d_OK != inRet)
        return inRet;
/*		
		inRet=inCTOS_PreConnect();
    if(d_OK != inRet)
        return inRet;
*/
    inRet = inCTOS_GetTipAfjustAmount();
    if(d_OK != inRet)
        return inRet;

	inRet = inCTOS_UpdateTxnTotalAmount();
    if(d_OK != inRet)
        return inRet;
	
	inRet = inCTOS_ConfirmInvAmt();
    if(d_OK != inRet)
        return inRet;

    inRet = inBuildAndSendIsoData();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_SaveBatchTxn();
    if(d_OK != inRet)
        return inRet;

    inRet = inCTOS_UpdateAccumTotal();
    if(d_OK != inRet)
        return inRet;

		/*sidumili: Fix on Issue#: 000181*/
		if (strTCT.fPrintTipReceipt){
				inRet = ushCTOS_printReceipt();
				if(d_OK != inRet)
    		    return inRet;
    		else
    		    vdSetErrorMessage("");
		}else{
				//CTOS_LCDTPrintXY(1, 8, "TIP ADJUST SUCCESS");
				//vdDisplayMessage("TIP ADJUST SUCCESS","","");

			// Tip Adjust -> Original Tip / Adjustment / Total Amount -- sidumili
			memset(szTipAmt, 0x00, sizeof(szTipAmt));
			memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
			wub_hex_2_str(srTransRec.szTipAmount, szTipAmt, 6);
			vdCTOS_FormatAmount(strCST.szAmountFormat, szTipAmt, szAmtTmp2);

			memset(szTipAmt, 0x00, sizeof(szTipAmt));
			sprintf(szTipAmt,"%s %s",strCST.szCurSymbol,szAmtTmp2);

			// Total Amount
			memset(szAmtTmp2, 0x00, sizeof(szAmtTmp2));
			wub_hex_2_str(srTransRec.szTotalAmount, szAmtTmp1, 6);
			vdCTOS_FormatAmount(strCST.szAmountFormat, szAmtTmp1, szAmtTmp2);
			memset(szAmtTmp1, 0x00, sizeof(szAmtTmp1));
			sprintf(szAmtTmp1,"%s %s",strCST.szCurSymbol,szAmtTmp2);
			
			memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));			
			strcat(szDisplayBuf,szTipAmt); // Original Tip
			strcat(szDisplayBuf,"|");
			strcat(szDisplayBuf,szTipAmt); // Adjustment
			strcat(szDisplayBuf,"|");
			strcat(szDisplayBuf,szAmtTmp1); // Total Amount

			vdDebug_LogPrintf("usCTOSS_DisplayTipAdjustApprovedUI, len=[%d], szDisplayBuf=[%s]", strlen(szDisplayBuf), szDisplayBuf);
			
		   	usCTOSS_DisplayTipAdjustApprovedUI(szDisplayBuf);
		   
			CTOS_Beep();
			CTOS_Delay(1000);
			inRet = d_OK;
		}

		//BDO: Should be able to reprint TIP ADJ txn even when tip adj receipt printing is off - start -- jzg
		memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
		
		if((inResult = inTCTSave(1)) != ST_SUCCESS)
			vdDisplayErrorMsg(1, 8, "Update TCT fail");
		//BDO: Should be able to reprint TIP ADJ txn even when tip adj receipt printing is off - start -- jzg
    
    return d_OK;
}

int inCTOS_TIPADJUST(void)
{
    int inRet = d_NO;
    
    CTOS_LCDTClearDisplay();
    
    vdCTOS_TxnsBeginInit();

	inRet = inCheckBattery();	
	if(d_OK != inRet)
		return inRet;
	
    if(fGetECRTransactionFlg() != TRUE)
        inCTOSS_MultiStopResumeAP("SHARLS_ECRBDO");
    
    inRet = inCTOS_TipAdjustFlowProcess();
    
    if(fGetECRTransactionFlg() != TRUE)
        inCTOSS_MultiResumeContinueAP("SHARLS_ECRBDO");

    inCTOS_inDisconnect();

    vdCTOS_TransEndReset();

    return inRet;
}


int inCTOS_CheckHOSTTipAllowd(void)
{   
#if 1
	/* BDO: Tip Allow flag moved to HDT - start -- jzg */
	inHDTRead(srTransRec.HDTid);
    vdDebug_LogPrintf("DONE: inCTOS_CheckHOSTTipAllowd, strHDT.fHDTTipAllow=[%d]", strHDT.fHDTTipAllow);

	if(strHDT.fHDTTipAllow != TRUE)
	{
		//vdSetErrorMessage("TIP NOT ALLOWED");
		CTOS_LCDTClearDisplay();
		vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
		return d_NO;
	}
	else
		return d_OK;
	/* BDO: Tip Allow flag moved to HDT - end -- jzg */
#else
    // allow only tip for bdo, amex, bpi, diners and bankard
	if ((srTransRec.HDTid == 1) || (srTransRec.HDTid == 2) || (srTransRec.HDTid == 3) ||(srTransRec.HDTid == 4) ||(srTransRec.HDTid == 6) ||(srTransRec.HDTid == 23) ||(srTransRec.HDTid == 24)){    
		return d_OK;
	}else{
		vdSetErrorMessage("TIP NOT ALLOWED");
		return d_NO;
	}
#endif
}


