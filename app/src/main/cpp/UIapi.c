
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include "FeliCa/Util.h"
#include "Debug/debug.h"
#include "patrick-lib.h"
#include "Includes/POSTypedef.h"
#include "Filemodule/myFileFunc.h"
#include "../print/Print.h"
#include "..\Includes\POSTrans.h"

int gblinGPRSFallback = 0;

extern BOOL fRePrintFlag; 

USHORT CTOS_LCDTSetReverse(BOOL boReverse)
{
    return d_OK;
    BYTE szMultipleMsgStr[512];

    memset(szMultipleMsgStr, 0x00, sizeof(szMultipleMsgStr));
    strcpy(szMultipleMsgStr, "|");

    strcat(szMultipleMsgStr, "test11111");
    strcat(szMultipleMsgStr, "|");

    vdDebug_LogPrintf("CTOS_LCDTSetReverse.......");
    inCallJAVA_DisplayMultipleMessage(szMultipleMsgStr);
}

USHORT CTOS_LCDTPrintAligned1(USHORT usY, UCHAR* pbBuf, BYTE bMode)
{

    BYTE szMultipleMsgStr[512];

    memset(szMultipleMsgStr, 0x00, sizeof(szMultipleMsgStr));
    strcpy(szMultipleMsgStr, "|");

    strcat(szMultipleMsgStr, "test11111");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "test22222");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "test33333");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "test44444");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "test55555");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "test66666");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "test77777");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "test88888");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "test99999");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "test00000");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "testaaaaa");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "testbbbbb");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "testccccc");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "testddddd");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "testeeeee");
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, "testfffff");
    strcat(szMultipleMsgStr, "|");
    vdDebug_LogPrintf("CTOS_LCDTPrintAligned.....");
    inCallJAVA_DisplayMultipleMessage(szMultipleMsgStr);
}


/*
USHORT CTOS_LCDTPrintAligned(USHORT usY, UCHAR* pbBuf, BYTE bMode)
{

    BYTE szMultipleMsgStr[512];
    BYTE szLine[12];
    BYTE szMode[12];

    memset(szMultipleMsgStr, 0x00, sizeof(szMultipleMsgStr));
    strcpy(szMultipleMsgStr, "|");

    sprintf(szLine, "%d", usY);
    sprintf(szMode, "%d", bMode);


    strcat(szMultipleMsgStr, szLine);
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, pbBuf);
    strcat(szMultipleMsgStr, "|");
    strcat(szMultipleMsgStr, szMode);
    strcat(szMultipleMsgStr, "|");
    vdDebug_LogPrintf("CTOS_LCDTPrintAligned.....[%s]", szMultipleMsgStr);

    vdDebug_LogPrintf("CTOS_LCDTPrintAligned.....");
    inCallJAVA_LCDTPrintAligned(szMultipleMsgStr);
}
*/

USHORT CTOS_LCDSelectModeEx(BYTE bMode,BOOL fClear)
{
}
/*
USHORT CTOS_LCDGShowBMPPic(USHORT usX, USHORT usY, BYTE *baFilename)
{

	return 0;
	
	vdDebug_LogPrintf("CTOS_LCDGShowBMPPic Start");

	inCallJAVA_DisplayImage(usX, usY,baFilename);

	vdDebug_LogPrintf("CTOS_LCDGShowBMPPic End");

}*/

//USHORT CTOS_LCDBackGndColor(ULONG ulColor){

//}

//USHORT CTOS_LCDForeGndColor(ULONG ulColor){

//}

//USHORT CTOS_LCDFontSelectMode(BYTE bMode){

//}

USHORT CTOS_LCDGShowPic(USHORT usX, USHORT usY, BYTE* baPat, ULONG ulPatLen, USHORT usXSize){

}

USHORT CTOS_LCDTClear2EOL(void){

}

USHORT CTOS_LCDTGotoXY(USHORT usX,USHORT usY){

}

USHORT CTOS_LCDTPutchXY (USHORT usX, USHORT usY, UCHAR bChar){

}

USHORT CTOS_LCDTPrint(UCHAR* sBuf){

}

USHORT CTOS_LCDSetContrast(BYTE bValue){

}

//USHORT CTOS_LCDTSelectFontSize(USHORT usFontSize){

//}

USHORT CTOS_LCDTTFSwichDisplayMode(USHORT usMode){

}

//USHORT CTOS_LCDTTFSelect(BYTE *baFilename, BYTE bIndex){

//}

BYTE InputAmountEx(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter)
{
    vdDebug_LogPrintf("=====InputAmountEx=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

    vdDebug_LogPrintf("InputAmountEx");
    /*Still use the old way make it work first*/
    memset(szInBuf, 0x00, sizeof(szInBuf));
    memset(szOutBuf, 0x00, sizeof(szOutBuf));

    strcpy(szInBuf, szCurSymbol);

       inRet = inCallJAVA_GetAmountString(szInBuf, szOutBuf, &byLen);

    //inCallJAVA_EnterAnyNum(&byLen, szOutBuf);
    //Tine --19Mar2019, to fix app crashing upon CANCEL from GetAmountString
    if (0 == strcmp(szOutBuf, "TO"))
        return 0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL")) {
        vdDebug_LogPrintf("=====Cancel InputAmountEx=====");
        *ulAmount = 0;
        return d_USER_CANCEL;
    }
    else if (0 == strcmp(szOutBuf, "TIME OUT")) {
        vdDebug_LogPrintf("=====TIME OUT InputAmountEx=====");
        *ulAmount = 0;
        return d_TIMEOUT;
    }
    else {
        memset(baAmount, 0x00, sizeof(baAmount));
        strcpy(baAmount, szOutBuf);
        vdDebug_LogPrintf("baAmount[%s] byLen[%d]", baAmount, byLen);
    }

    if (strlen(baAmount)>0 && byLen>0) {
        *ulAmount = atol(baAmount);
        vdDebug_LogPrintf("ulAmount[%ul] byLen[%d]", ulAmount, byLen);
    }
    else
        *ulAmount = 0;


    vdDebug_LogPrintf("=====End InputAmountEx=====");
    return d_OK;

}

USHORT usCTOSS_Confirm(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_ConfirmMenu=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

	

    inRet = inCallJAVA_UserConfirmMenu(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_ConfirmMenu  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_ConfirmMenu=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return d_NO;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return d_USER_CANCEL;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
		return 0xFF;
    else
        return d_OK;
}

USHORT usCTOSS_ConfirmYesNo(BYTE *szMsg)
{
    vdDebug_LogPrintf("=====usCTOSS_ConfirmMenu=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];

	memset(szOutBuf,0x00,sizeof(szOutBuf));

    inRet = inCallJAVA_ConfirmYesNo(szMsg, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End inCallJAVA_ConfirmYesNo  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End inCallJAVA_ConfirmYesNo=====");

    if (0 == strcmp(szOutBuf, "YES"))
        return d_OK;
    else if (0 == strcmp(szOutBuf, "NO"))
        return d_NO;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
		return 0xFF;

}


USHORT usCTOSS_BatchReviewUI(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_BatchReviewUI=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

	

    inRet = inCallJAVA_BatchReviewUI(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_BatchReviewUI  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_BatchReviewUI=====");

    if (0 == strcmp(szOutBuf, "NEXT"))
        return d_KBD_DOWN;
    if (0 == strcmp(szOutBuf, "PREVIOUS"))
        return d_KBD_UP;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return d_KBD_CANCEL;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
		return 0xFF;
    else
        return d_OK;
}


USHORT usCTOSS_ConfirmOK(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_ConfirmMenu=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

	

    inRet = inCallJAVA_UserConfirmOKMenu(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_ConfirmOKMenu  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_ConfirmOKMenu=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return d_NO;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
		return 0xFF;
    else
        return d_OK;
}


USHORT ConfirmCardDetails(BYTE *szConfirmCardDetails) {
    vdDebug_LogPrintf("=====ConfirmCardDetails=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[1024];
    BYTE szOutBuf[512];

    memset(szInBuf, 0x00, sizeof(szInBuf));
    memset(szOutBuf, 0x00, sizeof(szOutBuf));
    strcpy(szInBuf, szConfirmCardDetails);

    vdDebug_LogPrintf("=====Start ConfirmCardDetails  szInBuf[%s]=====", szInBuf);
    inRet = inCallJAVA_UserConfirmCard(szInBuf, szOutBuf, &byLen);
    vdDebug_LogPrintf("=====End ConfirmCardDetails  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End ConfirmCardDetails=====");

    if (0 == strcmp(szOutBuf, "TIME OUT"))
        return  0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL")) {
        vdDebug_LogPrintf("=====ConfirmCardDetails =====");
        return d_USER_CANCEL;
    }
    else
        return d_OK;
}

USHORT UserCancel(BYTE *pbCancelDisplay) {
    vdDebug_LogPrintf("=====UserCancelUI=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

	memset(szInBuf, 0x00, sizeof(szInBuf));
	strcpy(szInBuf, pbCancelDisplay);


    inRet = inCallJAVA_UserCancelUI(szInBuf, szOutBuf, &byLen);
    vdDebug_LogPrintf("=====End UserCancelUI  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End UserCancelUI=====");

    if (0 == strcmp(szOutBuf, "CANCEL")) {
        return d_NO;
        vdDebug_LogPrintf("=====UserCancelUI =====");
    }
    //else
        //return d_OK;
}




BYTE InputString(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    vdDebug_LogPrintf("=====InputString=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

	memset(szInBuf, 0x00, sizeof(szInBuf));

    if (0x01 == bInputMode)
        strcpy(szInBuf, "1");
    else if (0x02 == bInputMode)
        strcpy(szInBuf, "2");
	else if(0x03 == bInputMode)
		strcpy(szInBuf, "3");
	else 
		strcpy(szInBuf, "0");

	
    vdDebug_LogPrintf("=====InputString=====szInBuf[%s]", szInBuf);

    /*Still use the old way make it work first*/
    inRet = inCallJAVA_UserInputString(szInBuf, szOutBuf, &byLen);
    strcpy(pbaStr, szOutBuf);
    *usStrLen = byLen;

    vdDebug_LogPrintf("pbaStr[%s] *usStrLen[%d]", pbaStr, *usStrLen);
    vdDebug_LogPrintf("=====End InputString=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return 0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return 'C';
    else
        return 'A';
}

BYTE InputStringUI(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *szInput) {
    vdDebug_LogPrintf("=====S1InputString=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[1024];
    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====InputStringUI=====");
	vdDebug_LogPrintf("bInputMode=[%d]", bInputMode);
	vdDebug_LogPrintf("usStrLen=[%d]", usStrLen);
	vdDebug_LogPrintf("usMinLen=[%d]", usMinLen);
	vdDebug_LogPrintf("usTimeOutMS=[%d]", usTimeOutMS);
	vdDebug_LogPrintf("szInput=[%s]", szInput);

    memset(szInBuf, 0x00, sizeof(szInBuf));
    memset(szOutBuf, 0x00, sizeof(szOutBuf));

    if (0x01 == bInputMode){
		vdDebug_LogPrintf("S1InputString bInputMode = 1");
        strcpy(szInBuf, "1");
        strcat(szInBuf, "|");
        strcat(szInBuf, szInput);
    }
    else if (0x02 == bInputMode) {
		vdDebug_LogPrintf("S1InputString bInputMode = 2");
        strcpy(szInBuf, "2");
        strcat(szInBuf, "|");
		strcat(szInBuf, szInput);
    }
    else if(0x03 == bInputMode) {
		vdDebug_LogPrintf("S1InputString bInputMode = 3");
        strcpy(szInBuf, "3");
		strcat(szInBuf, "|");
		strcat(szInBuf, szInput);
    }
    else
        strcpy(szInBuf, "0");

    vdDebug_LogPrintf("=====InputStringUI=====szInBuf[%s]", szInBuf);
    /*Still use the old way make it work first*/
    inRet = inCallJAVA_S1InputString(szInBuf, szOutBuf, &byLen);
    strcpy(pbaStr, szOutBuf);
    *usStrLen = byLen;
    //vdDebug_LogPrintf("pbaStr[%s] *usStrLen[%d]", pbaStr, *usStrLen);

	vdDebug_LogPrintf("szOutBuf=[%s]", szOutBuf);
    vdDebug_LogPrintf("=====End InputStringUI=====");
    if (0 == strcmp(szOutBuf, "TO")){
		CTOS_Beep();
        return 0xFF;
    }
    else if (0 == strcmp(szOutBuf, "CANCEL"))
    {
		vdDebug_LogPrintf("=====Cancel InputStringUI=====");
			CTOS_Beep();
			return 'C';
    }
    else
        {
			vdDebug_LogPrintf("=====OK InputStringUI, return [A]=====");
			return 'A';
		}

	
	//DisplayStatusLine("");

    return d_OK;
}

//TINE--
BYTE InputAlphaUI(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT usMaxLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *display) {
		vdDebug_LogPrintf("=====InputAlphaUI=====");
	
		BYTE byLen = 0;
		int inRet = 0;
	
		BYTE szInBuf[512];
		BYTE szOutBuf[512];
		BYTE szMaxLen[4];
		BYTE szMinLen[5];
	
		vdDebug_LogPrintf("saturn =====InputAlphaUI=====");
	
		memset(szInBuf, 0x00, sizeof(szInBuf));
		memset(szOutBuf, 0x00, sizeof(szOutBuf));
		memset(pbaStr, 0x00, sizeof(pbaStr));
		memset(szMaxLen, 0x00, sizeof(szMaxLen));
		memset(szMinLen, 0x00, sizeof(szMinLen));

		sprintf(szMaxLen, "%d", usMaxLen);
		sprintf(szMinLen, "%d", usMinLen);
	
			strcpy(szInBuf, szMinLen);
			strcat(szInBuf, "|");
			strcat(szInBuf, szMaxLen);
			strcat(szInBuf, "|");          
			strcat(szInBuf, display);
	
		vdDebug_LogPrintf("saturn =====InputAlphaUI=====szInBuf[%s]", szInBuf);
		/*Still use the old way make it work first*/
		inRet = inCallJAVA_S1InputStringAlpha(szInBuf, szOutBuf, &byLen);
		strcpy(pbaStr, szOutBuf);
		vdDebug_LogPrintf("pbaStr[%s]", pbaStr);
		vdDebug_LogPrintf("=====End InputAlphaUI=====");
		if (0 == strcmp(szOutBuf, "TIME OUT"))
			return 0xFF;
		else if (0 == strcmp(szOutBuf, "CANCEL"))
		{
			vdDebug_LogPrintf("=====Cancel InputAlphaUI=====");
				return 'C';
		}
		else
			{
				vdDebug_LogPrintf("=====OK InputAlphaUI=====");
				return 'A';
			}
	
		return d_OK;
	}




BYTE InputStringAlpha(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    vdDebug_LogPrintf("=====InputStringAlpha=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[128];
    BYTE szOutBuf[128];

    strcpy(szInBuf, "1");

    /*Still use the old way make it work first*/
    inRet = inCallJAVA_UserInputString(szInBuf, szOutBuf, &byLen);
    strcpy(pbaStr, szOutBuf);
    *usStrLen = byLen;

    vdDebug_LogPrintf("pbaStr[%s] *usStrLen[%d]", pbaStr, *usStrLen);
    vdDebug_LogPrintf("=====End InputStringAlpha=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return 0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return 'C';
    else
        return 'A';

    return d_OK;
}

BYTE InputStringAlphaEx(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    vdDebug_LogPrintf("=====InputStringAlphaEx=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[128];
    BYTE szOutBuf[128];

    strcpy(szInBuf, "1");

    /*Still use the old way make it work first*/
    inRet = inCallJAVA_UserInputString(szInBuf, szOutBuf, &byLen);
    strcpy(pbaStr, szOutBuf);
    *usStrLen = byLen;

    vdDebug_LogPrintf("pbaStr[%s] *usStrLen[%d]", pbaStr, *usStrLen);
    vdDebug_LogPrintf("=====End InputStringAlphaEx=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return 0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return 'C';
    else
        return 'A';

    return d_OK;
}

BYTE InputStringAlphaEx2(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS)
{
    vdDebug_LogPrintf("=====InputStringAlphaEx2=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[128];
    BYTE szOutBuf[128];

    strcpy(szInBuf, "1");

    /*Still use the old way make it work first*/
    inRet = inCallJAVA_UserInputString(szInBuf, szOutBuf, &byLen);
    strcpy(pbaStr, szOutBuf);
    *usStrLen = byLen;

    vdDebug_LogPrintf("pbaStr[%s] *usStrLen[%d]", pbaStr, *usStrLen);
    vdDebug_LogPrintf("=====End InputStringAlphaEx2=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return 0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return 'C';
    else
        return 'A';

    return d_OK;
}

USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS)
{
    vdDebug_LogPrintf("=====shCTOS_GetNum=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[128];
    BYTE szOutBuf[128];

    strcpy(szInBuf, "1");

    /*Still use the old way make it work first*/
    inRet = inCallJAVA_UserInputString(szInBuf, szOutBuf, &byLen);
    strcpy(baBuf, szOutBuf);
    *usStrLen = byLen;

    vdDebug_LogPrintf("baBuf[%s] *usStrLen[%d]", baBuf, *usStrLen);
    vdDebug_LogPrintf("=====End shCTOS_GetNum=====");

    return d_OK;
}

BYTE MenuDisplay(BYTE *sHeaderString, BYTE iHeaderStrLen, BYTE bHeaderAttr, BYTE iCol, BYTE center_x, BYTE *sItemsString, BYTE isReCount)
{
    vdDebug_LogPrintf("saturn =====MenuDisplay=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE bySelect = 0;

    BYTE szInBuf[9999+1];
    BYTE szOutBuf[9999+1];

    memset(szInBuf, 0, sizeof(szInBuf));
    memset(szOutBuf, 0, sizeof(szOutBuf));
	
    if (strlen(sHeaderString)>0)
    {
        strcpy(szInBuf, sHeaderString);
    }
    else
    {
        strcpy(szInBuf, "");
    }

    strcat(szInBuf, "|");

    /*After title string are menu items*/
    strcat(szInBuf, sItemsString);

    /*Still use the old way make it work first*/
    //inRet = inCallJAVA_DOptionMenuDisplay(szInBuf, szOutBuf, &byLen);
    inRet = inCallJAVA_DPopupMenuDisplay(szInBuf, szOutBuf, &byLen);
    //strcpy(baBuf, szOutBuf);
    //*usStrLen = byLen;
    
	//bySelect = atoi(szOutBuf);
    //vdDebug_LogPrintf("szOutBuf: %s", szOutBuf);
	
	//vdDebug_LogPrintf("=====szOutBuf:%s=====", szOutBuf);    
    vdDebug_LogPrintf("=====End MenuDisplay=====");
     if (0 == strcmp(szOutBuf, "TO"))
         return 0xFF;
     else if (0 == strcmp(szOutBuf, "CANCEL"))
         return 'C';
     else
     {
         bySelect = atoi(szOutBuf);
         return bySelect;
     }

    
}

USHORT usCTOSS_DisplayUI(BYTE *szDispString)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====usCTOSS_DisplayUI=====");

    inRet = inCallJAVA_DisplayUI(szDispString, szOutBuf, &byLen);

    vdDebug_LogPrintf("=====End usCTOSS_DisplayUI  szOutBuf[%s]=====", szOutBuf);

    return d_OK;
}

USHORT usCTOSS_LCDDisplay(BYTE *szStringMsg)
{
	BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf_disp[512];

	memset(szOutBuf_disp, 0x00, sizeof(szOutBuf_disp));

    vdDebug_LogPrintf("=====usCTOSS_LCDDisplay=====");

    inRet = inCallJAVA_LCDDisplay(szStringMsg, szOutBuf_disp, &byLen);

    //vdDebug_LogPrintf("=====End usCTOSS_LCDDisplay  szOutBuf[%s]=====", szOutBuf);

    return d_OK;
}

USHORT PrintReceiptUI(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====PrintReceiptUI=====");
    memset(szOutBuf,0x00,sizeof(szOutBuf));

    inRet = inCallJAVA_PrintReceiptUI(szStringMsg, szOutBuf, &byLen);

    vdDebug_LogPrintf("=====End PrintReceiptUI  szOutBuf[%s]=====", szOutBuf);

    //return d_OK;
    if (0 == strcmp(szOutBuf, "TIME OUT"))
        return  0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL")) {
        vdDebug_LogPrintf("=====PrintReceiptUI - CANCEL =====");
        return d_USER_CANCEL;
    }
    else
    {
		vdDebug_LogPrintf("=====PrintReceiptUI - OK =====");
		return d_OK;
    }

}

USHORT PrintFirstReceiptUI(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====PrintFirstReceiptUI=====");
    memset(szOutBuf,0x00,sizeof(szOutBuf));

    inRet = inCallJAVA_PrintFirstReceiptUI(szStringMsg, szOutBuf, &byLen);

    vdDebug_LogPrintf("=====End PrintFirstReceiptUI  szOutBuf[%s]=====", szOutBuf);

    //return d_OK;
    if (0 == strcmp(szOutBuf, "TIME OUT"))
        return  0xFF;
    else
    {
		vdDebug_LogPrintf("=====PrintFirstReceiptUI - OK =====");
		return d_OK;
    }

}

USHORT EliteReceiptUI(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====EliteReceiptUI=====");
    memset(szOutBuf,0x00,sizeof(szOutBuf));

    inRet = inCallJAVA_EliteReceiptUI(szStringMsg, szOutBuf, &byLen);

    vdDebug_LogPrintf("=====End EliteReceiptUI  szOutBuf[%s]=====", szOutBuf);

    //return d_OK;
    if (0 == strcmp(szOutBuf, "TIME OUT"))
        return  0xFF;
    else
    {
		vdDebug_LogPrintf("=====EliteReceiptUI - OK =====");
		return d_OK;
    }

}



USHORT usCARDENTRY(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====usCARDENTRY=====");
	vdDebug_LogPrintf("szStringMsg=[%s]", szStringMsg);

    inRet = inCallJAVA_usCARDENTRY(szStringMsg, szOutBuf, &byLen);

    vdDebug_LogPrintf("saturn =====End usCARDENTRY  szOutBuf[%s]=====", szOutBuf);

    //return d_OK;
    return d_OK;
}

USHORT onFailedCtlsFallback(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====onFailedCtlsFallback=====");

    inRet = inCallJAVA_onFailedCtlsFallback(szStringMsg, szOutBuf, &byLen);

    vdDebug_LogPrintf("=====End onFailedCtlsFallback  szOutBuf[%s]=====", szOutBuf);

    //return d_OK;
    return d_OK;
}

/*
USHORT usCARDENTRY2(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[1024];
    BYTE szOutBuf[528];

    vdDebug_LogPrintf("=====usCARDENTRY2=====");

    memset(szOutBuf,0, sizeof(szOutBuf));
    memset(szInBuf,0, sizeof(szInBuf));
    strcpy(szInBuf, szStringMsg);
    inRet = inCallJAVA_usCARDENTRY2(szInBuf, szOutBuf, &byLen);

    vdDebug_LogPrintf("=====End usCARDENTRY2  szOutBuf[%s]=====", szOutBuf);
    if (0 == strcmp(szOutBuf,"CANCEL")) {
        vdDebug_LogPrintf("=====End usCARDENTRY2 = [%s]=====", szOutBuf);
        return d_USER_CANCEL;
    }
    else if (0 == strcmp(szOutBuf, "CARD_ENTRY_MSR")) {
        vdDebug_LogPrintf("=====End usCARDENTRY2 = [%s]=====", szOutBuf);
        srTransRec.byEntryMode = CARD_ENTRY_MSR;
        return d_OK;
    }
    else if (0 == strcmp(szOutBuf, "CARD_ENTRY_ICC")) {
        vdDebug_LogPrintf("=====End usCARDENTRY2 = [%s]=====", szOutBuf);
        srTransRec.byEntryMode = CARD_ENTRY_ICC;
        return d_OK;
    }
	else if (0 == strcmp(szOutBuf, "CARD_ENTRY_WAVE")) {
        vdDebug_LogPrintf("=====End usCARDENTRY2 = [%s]=====", szOutBuf);
        srTransRec.byEntryMode = CARD_ENTRY_WAVE;
        return d_OK;
    }
	else if (0 == strcmp(szOutBuf, "TIME OUT")) {
        vdDebug_LogPrintf("=====End usCARDENTRY2 = [%s]=====", szOutBuf);
	    return d_NO;
    }
    else {
        vdDebug_LogPrintf("=====End usCARDENTRY2=====");
        return d_OK;
    }
}
*/


USHORT usCTOSS_ConfirmInvAndAmt(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_ConfirmInvAndAmt=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

	
	
    inRet = inCallJAVA_UserConfirmMenuInvandAmt(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_ConfirmInvAndAmt  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_ConfirmInvAndAmt=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return d_TIMER_INVALID_PARA;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return d_USER_CANCEL;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
        return 0xFF;
    else
        return d_OK;
}


USHORT usCTOSS_GetPinUI(void)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];
    BYTE szDispString[5]="TEST";

    vdDebug_LogPrintf("SATURN =====usCTOSS_GetPinUI=====");
    inRet = inCallJAVA_GetPinUI(szDispString, szOutBuf, &byLen);
    vdDebug_LogPrintf("SATURN =====End usCTOSS_GetPinUI  szOutBuf[%s]=====", szOutBuf);

    return d_OK;
}

BYTE InputExpiryDateUI(BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *szInput)
{
    vdDebug_LogPrintf("=====InputExpiryDateUI=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[1024];
    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====InputExpiryDateUI=====");
    vdDebug_LogPrintf("szInput");
    vdDebug_LogPrintf(szInput);

    memset(szInBuf, 0x00, sizeof(szInBuf));
    memset(szOutBuf, 0x00, sizeof(szOutBuf));

    if(strlen(szInput) > 0)
    {
        strcpy(szInBuf, szInput);
    }
    else
    {
        strcpy(szInBuf, "");
    }

    vdDebug_LogPrintf("=====InputExpiryDateUI=====szInBuf[%s]", szInBuf);
    /*Still use the old way make it work first*/
    inRet = inCallJAVA_GetExpiryDate(szInBuf, szOutBuf, &byLen);
    strcpy(pbaStr, szOutBuf);
    *usStrLen = byLen;
    //vdDebug_LogPrintf("pbaStr[%s] *usStrLen[%d]", pbaStr, *usStrLen);
    vdDebug_LogPrintf("=====End InputExpiryDateUI=====");
    if (0 == strcmp(szOutBuf, "TO"))
        return 0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
    {
        vdDebug_LogPrintf("=====Cancel InputExpiryDateUI=====");
        return 'C';
    }
    else
    {
        vdDebug_LogPrintf("=====OK InputExpiryDateUI=====");
        return 'A';
    }

    return d_OK;
}

USHORT usEditDatabase(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====usEditDatabase=====");

    //inRet = inCallJAVA_usCARDENTRY(szStringMsg, szOutBuf, &byLen);
    inRet = inCallJAVA_usEditDatabase(szStringMsg, szOutBuf, &byLen);
    vdDebug_LogPrintf("=====End usEditDatabase  szOutBuf[%s]=====", szOutBuf);

    if (0 == strcmp(szOutBuf, "EDITDB_OK"))
        return 'C';

    return d_OK;
}

USHORT usCTOSS_BackToProgress(BYTE *szDispString)
{
  
    BYTE byLen = 0;
    int inRet = 0;
	
    BYTE szOutBuf[512];

	vdDebug_LogPrintf("=====usCTOSS_BackToProgress=====");
    inRet = inCallJAVA_BackToProgress(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_BackToProgress  szOutBuf[%s]=====", szOutBuf);

    return d_OK;
}

extern BYTE szIP[100+1];
extern BYTE szDNS1[100+1];
extern BYTE szDNS2[100+1];
extern BYTE szGateWay[100+1];
extern BYTE szSubnetMask[100+1];

USHORT usCTOSS_GetWifiInfo(void)
{
    BYTE szOutBuf[512];


    vdDebug_LogPrintf("=====usCTOSS_GetWifiInfo=====");

    inCallJAVA_GetWifiInfo(szOutBuf);
	vdDebug_LogPrintf("saturn =====usCTOSS_GetWifiInfo===== %s", szOutBuf);

	SplitString(szOutBuf, szDNS1, szDNS2, '*');
	vdDebug_LogPrintf("test 1 saturn szDNS1[%s]", szDNS1);
	vdDebug_LogPrintf("test 1 saturn szDNS2[%s]", szDNS2);

	memset(szOutBuf, 0x00, sizeof(szOutBuf));
	strcpy(szOutBuf, szDNS2);
	memset(szGateWay, 0x00, sizeof(szGateWay));

	vdDebug_LogPrintf("test 2 saturnsz szOutBuf[%s]", szOutBuf);
	SplitString(szOutBuf, szDNS2, szGateWay, '*');
	vdDebug_LogPrintf("test 2 saturnsz DNS2[%s]", szDNS2);
	vdDebug_LogPrintf("test 2 saturnsz szGateWay[%s]", szGateWay);

	
	memset(szOutBuf, 0x00, sizeof(szOutBuf));
	strcpy(szOutBuf, szGateWay);
	memset(szIP, 0x00, sizeof(szIP));
	
	vdDebug_LogPrintf("test 3 saturnsz szOutBuf[%s]", szOutBuf);
	SplitString(szOutBuf, szGateWay, szIP, '*');
	vdDebug_LogPrintf("test 3 saturn szGateWay[%s]", szGateWay);
	vdDebug_LogPrintf("test 3 saturn szIP[%s]", szIP);

	
	memset(szOutBuf, 0x00, sizeof(szOutBuf));
	strcpy(szOutBuf, szIP);
	memset(szSubnetMask, 0x00, sizeof(szSubnetMask));
	SplitString(szOutBuf, szIP, szSubnetMask, '*');
	
	vdDebug_LogPrintf("test 4 saturn szIP[%s]", szIP);
	
	
	vdDebug_LogPrintf("saturn szSubnetMask[%s]", szSubnetMask);

    return d_OK;
}

BOOL usGetConnectionStatus(int inCommType)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[50];
	BYTE szStringMsg[3];

	memset(szStringMsg, 0x00, sizeof(szStringMsg));

	if (inCommType == 4)
		strcpy(szStringMsg, "4");
	else if (inCommType == 2)
		strcpy(szStringMsg, "2");
		
    vdDebug_LogPrintf("saturn =====usGetConnectionStatus=====");

	inRet = inCallJAVA_usGetConnectionStatus(szStringMsg, szOutBuf, &byLen);
	

    vdDebug_LogPrintf("saturn =====End usGetConnectionStatus  szOutBuf[%s]=====", szOutBuf);

	if (strcmp(szOutBuf, "YES") == 0)
		return d_OK;
	else
         {
          /*
               if ((inCommType == 4) && (strcmp(szOutBuf, "FALLBACK") == 0) && (strTCT.inGPRSFallback==1))
               {
                    gblinGPRSFallback = 1;
                    return TRUE;
               }
          */
			   
               return d_NO;
         }

}

USHORT ConfirmDetails(BYTE *szConfirmCardDetails) {
    vdDebug_LogPrintf("=====ConfirmCardDetails=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[1024];
    BYTE szOutBuf[512];

    memset(szInBuf, 0x00, sizeof(szInBuf));
    memset(szOutBuf, 0x00, sizeof(szOutBuf));
    strcpy(szInBuf, szConfirmCardDetails);

    vdDebug_LogPrintf("saturn =====Start ConfirmDetails  szInBuf[%s]=====", szInBuf);
    inRet = inCallJAVA_UserConfirmCard(szInBuf, szOutBuf, &byLen);
    vdDebug_LogPrintf("saturn =====End ConfirmDetails  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End ConfirmDetails=====");

    if ((0 == strcmp(szOutBuf, "TIME OUT")) || (0 == strcmp(szOutBuf, "TO")))
        return  0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL")) {
        vdDebug_LogPrintf("=====ConfirmDetails =====");
        return d_USER_CANCEL;
    }
    else
        return d_OK;
}

BYTE InputAmountWithMenu(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter)
{
    vdDebug_LogPrintf("=====InputAmountWithMenu=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

    vdDebug_LogPrintf("InputAmountWithMenu");
    /*Still use the old way make it work first*/
    memset(szInBuf, 0x00, sizeof(szInBuf));
    memset(szOutBuf, 0x00, sizeof(szOutBuf));

    strcpy(szInBuf, szCurSymbol);

    inRet = inCallJAVA_GetAmountStringWithMenu(szInBuf, szOutBuf, &byLen);

    //inCallJAVA_EnterAnyNum(&byLen, szOutBuf);
    //Tine --19Mar2019, to fix app crashing upon CANCEL from GetAmountString
    if (0 == strcmp(szOutBuf, "TO"))
        return 0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL")) {
        vdDebug_LogPrintf("=====Cancel InputAmountWithMenu=====");
        *ulAmount = 0;
        return d_USER_CANCEL;
    }
    else if (0 == strcmp(szOutBuf, "TIME OUT")) {
        vdDebug_LogPrintf("=====TIME OUT InputAmountWithMenu=====");
        *ulAmount = 0;
        return d_TIMEOUT;
    }else if (0 == strcmp(szOutBuf, "MENU")) {
        vdDebug_LogPrintf("=====saturn MENU=====");
        *ulAmount = 0;
        return d_MENU_SELECTION;
    }
    else {
        memset(baAmount, 0x00, sizeof(baAmount));
        strcpy(baAmount, szOutBuf);
        vdDebug_LogPrintf("baAmount[%s] byLen[%d]", baAmount, byLen);
    }

    if (strlen(baAmount)>0 && byLen>0) {
        *ulAmount = atol(baAmount);
        vdDebug_LogPrintf("ulAmount[%ul] byLen[%d]", ulAmount, byLen);
    }
    else
        *ulAmount = 0;


    vdDebug_LogPrintf("=====End InputAmountWithMenu=====");
    return d_OK;

}


USHORT usGetSerialNumber(BYTE baFactorySN[20])

{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[128+1];

    vdDebug_LogPrintf("saturn =====usGetSerialNumber=====");


	//inRet = inCallJAVA_usGetSerialNumber(szStringMsg, szOutBuf, &byLen);
	inRet = inCallJAVA_usGetSerialNumber(szOutBuf, &byLen);

    vdDebug_LogPrintf("saturn =====End usGetSerialNumber  szOutBuf[%s]=====", szOutBuf);

    strcpy(baFactorySN, szOutBuf);
    return d_OK;
}

USHORT usCTOSS_DisplayBox(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf_disp[512 + 1];

    vdDebug_LogPrintf("=====usCTOSS_DisplayBox=====");

    inRet = inCallJAVA_DisplayBox(szStringMsg, szOutBuf_disp, &byLen);

    vdDebug_LogPrintf("=====End usCTOSS_DisplayBox  szOutBuf[%s]=====", szOutBuf_disp);

	memset(strGBLVar.szGBLvOutBuffer, 0x00, sizeof(strGBLVar.szGBLvOutBuffer));
	strcpy(strGBLVar.szGBLvOutBuffer, szOutBuf_disp);

	if (0 == strcmp(szOutBuf_disp, "CONFIRM"))
        return d_OK;
    else if (0 == strcmp(szOutBuf_disp, "CANCEL"))
        return d_KBD_CANCEL;
	else if (0 == strcmp(szOutBuf_disp, "TIME OUT"))
		return 0xFF;
    else
        return d_OK;

}

USHORT usCTOSS_DisplayStatusBox(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf_dsbox[528 + 1];

    vdDebug_LogPrintf("=====usCTOSS_DisplayStatusBox=====");

    inRet = inCallJAVA_DisplayStatusBox(szStringMsg, szOutBuf_dsbox, &byLen);

    //vdDebug_LogPrintf("=====End usCTOSS_DisplayStatusBox  szOutBuf[%s]=====", szOutBuf_dsbox);

    return d_OK;
}

// sidumili: added for edit host info
USHORT usCTOSS_EditInfoListViewUI(BYTE *szDispString, BYTE *szOutputBuf)
{
    vdDebug_LogPrintf("=====usCTOSS_EditInfoListViewUI=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[9999+1];
    BYTE szOutBuf[9999+1];
	
    inRet = inCallJAVA_EditInfoListViewUI(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_EditInfoListViewUI  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_EditInfoListViewUI=====");

	// Sample data: CONFIRM|19|BDOTBG ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
	//GetDelimitedString(szOutBuf, 1, '|ul', strGBLVar.szGBLvProcessDesc);
	GetDelimitedString(szOutBuf, 1, '|', strGBLVar.szGBLvProcessDesc);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	strcpy(szOutputBuf, szOutBuf);
	
    if (0 == strcmp(strGBLVar.szGBLvProcessDesc, "CONFIRM"))
        return d_OK;
    else if (0 == strcmp(strGBLVar.szGBLvProcessDesc, "CANCEL"))
        return d_KBD_CANCEL;
	else if (0 == strcmp(strGBLVar.szGBLvProcessDesc, "TIME OUT"))
		return 0xFF;
    else
        return d_OK;
}

USHORT usCTOSS_Confirm2(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_ConfirmMenu2=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

    inRet = inCallJAVA_UserConfirmMenu2(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_ConfirmMenu2  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_ConfirmMenu2=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return d_TIMEOUT;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return d_USER_CANCEL;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
		return 0xFF;
    else
        return d_OK;
}

USHORT usCTOSS_ConfirmOKCancel(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_ConfirmOKCancel=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512 + 1];
    BYTE szOutBuf[512 + 1];	

    inRet = inCallJAVA_UserConfirmOKCancelMenu(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_ConfirmOKCancel  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_ConfirmOKCancel=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return d_NO;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return d_USER_CANCEL;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
		return d_TIMEOUT;
    else
        return d_OK;
}

USHORT usCTOSS_GetMobileInfo(void)
{
    BYTE szOutBuf[512];


    vdDebug_LogPrintf("=====usCTOSS_GetMobileInfo=====");

	memset(szOutBuf, 0x00, sizeof(szOutBuf));
    inCallJAVA_GetMobileInfo(szOutBuf);
	vdDebug_LogPrintf("saturn =====usCTOSS_GetMobileInfo===== %s", szOutBuf);

	memset(szIP, 0x00, sizeof(szIP));
	GetDelimitedString(szOutBuf, 1, '|', szIP); 

	memset(szGateWay, 0x00, sizeof(szGateWay));
	GetDelimitedString(szOutBuf, 2, '|', szGateWay); 

	memset(szSubnetMask, 0x00, sizeof(szSubnetMask));
	GetDelimitedString(szOutBuf, 3, '|', szSubnetMask); 

	memset(szDNS1, 0x00, sizeof(szDNS1));
	GetDelimitedString(szOutBuf, 4, '|', szDNS1); 

	memset(szDNS2, 0x00, sizeof(szDNS2));
	GetDelimitedString(szOutBuf, 5, '|', szDNS2); 

	vdDebug_LogPrintf("szIP=[%s]", szIP);
	vdDebug_LogPrintf("szDNS1=[%s]", szDNS1);
	vdDebug_LogPrintf("szDNS2=[%s]", szDNS2);
	vdDebug_LogPrintf("szGateWay=[%s]", szGateWay);
	vdDebug_LogPrintf("szSubnetMask=[%s]", szSubnetMask);

    return d_OK;
}

USHORT usCTOSS_ConfirmTotal(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_ConfirmTotal=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

	
	vdDebug_LogPrintf("saturn usCTOSS_ConfirmTotal");

    inRet = inCallJAVA_ConfirmTotal(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("saturn =====End usCTOSS_ConfirmTotal  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("saturn =====End usCTOSS_ConfirmTotal=====");

    if (0 == strcmp(szOutBuf, "TIMEOUT"))
        return d_TIMEOUT;
	else if (0 == strcmp(szOutBuf, "CONFIRM"))
		return d_OK;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return d_USER_CANCEL;
    else
        return d_OK;
}

USHORT getAppPackageInfo(BYTE *szAppName, BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];
	BYTE szInBuf[512];

    vdDebug_LogPrintf("=====getAppPackageInfo=====");

	memset(szInBuf, 0x00, sizeof(szInBuf));
    memset(szOutBuf, 0x00, sizeof(szOutBuf));
    memset(szStringMsg, 0x00, sizeof(szStringMsg));

	strcpy(szInBuf, szAppName);

    inRet = inCallJAVA_getAppPackageInfo(szInBuf, szOutBuf, &byLen);
    vdDebug_LogPrintf("=====End getAppPackageInfo  szOutBuf[%s]=====", szOutBuf);
	strcpy(szStringMsg, szOutBuf);

    return d_OK;
}

USHORT usCTOSS_DisplayApprovedUI(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[1024+1];

    vdDebug_LogPrintf("=====usCTOSS_DisplayApprovedUI=====");

    inRet = inCallJAVA_DisplayApprovedUI(szStringMsg, szOutBuf, &byLen);
	vdDebug_LogPrintf("saturn =====End inCallJAVA_DisplayApprovedUI  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("saturn =====End inCallJAVA_DisplayApprovedUI=====");

    return d_OK;
}

USHORT usCTOSS_PromptMessageUI(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf_disp[6144];

    vdDebug_LogPrintf("=====usCTOSS_PromptMessageUI=====");

    inRet = inCallJAVA_PromptMessageUI(szStringMsg, szOutBuf_disp, &byLen);

    return d_OK;
}

USHORT usCTOSS_MenuSelection(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_MenuSelection=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[9000+1];
    BYTE szOutBuf[9000+1];	

    inRet = inCallJAVA_MenuSelection(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_MenuSelection  szOutBuf[%s]=====", szOutBuf);

	memset(strGBLVar.szGBLvProcessDesc, 0x00, sizeof(strGBLVar.szGBLvProcessDesc));
	GetDelimitedString(szOutBuf, 1, '|', strGBLVar.szGBLvProcessDesc);

	vdDebug_LogPrintf("strGBLVar.szGBLvProcessDesc=[%s]", strGBLVar.szGBLvProcessDesc);
	
    if (0 == strcmp(szOutBuf, "TO"))
        return d_NO;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return d_USER_CANCEL;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
		return d_TIMEOUT;
    else
        return d_OK;
}

USHORT usCTOSS_TransDetailListViewUI(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_TransDetailListViewUI=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];
	
    inRet = inCallJAVA_TransDetailListViewUI(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_TransDetailListViewUI  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_TransDetailListViewUI=====");

	GetDelimitedString(szOutBuf, 1, '|', strGBLVar.szGBLvProcessType);
	GetDelimitedString(szOutBuf, 2, '|', strGBLVar.szGBLvSelectedValue);
	GetDelimitedString(szOutBuf, 3, '|', strGBLVar.szGBLvProcessDesc);

	vdDebug_LogPrintf("strGBLVar.szGBLvProcessType=[%s]", strGBLVar.szGBLvProcessType);
	vdDebug_LogPrintf("strGBLVar.szGBLvSelectedValue=[%s]", strGBLVar.szGBLvSelectedValue);
	vdDebug_LogPrintf("strGBLVar.szGBLvProcessDesc=[%s]", strGBLVar.szGBLvProcessDesc);

    if (0 == strcmp(strGBLVar.szGBLvProcessType, "CONFIRM"))
        return d_OK;
    else if (0 == strcmp(strGBLVar.szGBLvProcessType, "CANCEL"))
        return d_KBD_CANCEL;
	else if (0 == strcmp(strGBLVar.szGBLvProcessType, "TIME OUT"))
		return d_TIMEOUT;
    else
        return d_OK;
}

USHORT usCTOSS_SingleRecordDataListViewUI(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_SingleRecordDataListViewUI=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

	vdDebug_LogPrintf("szDispString=[%s]", szDispString);
	
    inRet = inCallJAVA_SingleRecordDataListViewUI(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_SingleRecordDataListViewUI  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_SingleRecordDataListViewUI=====");

	GetDelimitedString(szOutBuf, 1, '|', strGBLVar.szGBLvProcessType);
	GetDelimitedString(szOutBuf, 2, '|', strGBLVar.szGBLvSelectedValue);
	GetDelimitedString(szOutBuf, 3, '|', strGBLVar.szGBLvProcessDesc);

	vdDebug_LogPrintf("strGBLVar.szGBLvProcessType=[%s]", strGBLVar.szGBLvProcessType);
	vdDebug_LogPrintf("strGBLVar.szGBLvSelectedValue=[%s]", strGBLVar.szGBLvSelectedValue);
	vdDebug_LogPrintf("strGBLVar.szGBLvProcessDesc=[%s]", strGBLVar.szGBLvProcessDesc);

    if (0 == strcmp(strGBLVar.szGBLvProcessType, "CONFIRM"))
        return d_OK;
    else if (0 == strcmp(strGBLVar.szGBLvProcessType, "CANCEL"))
        return d_KBD_CANCEL;
	else if (0 == strcmp(strGBLVar.szGBLvProcessType, "TIME OUT"))
		return d_TIMEOUT;
    else
        return d_OK;
}

USHORT CTOS_LCDTPrintXY(USHORT usX, USHORT usY, UCHAR* pbBuf)
{
	vdDebug_LogPrintf("CTOS_LCDTPrintXY overload...");
	
	return d_OK;
}

// Tip adjust confirm -- sidumili
USHORT usCTOSS_ConfirmTipAdjust(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_ConfirmTipAdjust=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

	
	
    inRet = inCallJAVA_UserConfirmTipAdjust(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_ConfirmTipAdjust  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_ConfirmTipAdjust=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return d_TIMER_INVALID_PARA;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return d_USER_CANCEL;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
        return 0xFF;
    else
        return d_OK;
}

USHORT usCTOSS_DisplayTipAdjustApprovedUI(BYTE *szStringMsg)
{

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szOutBuf[512];

    vdDebug_LogPrintf("=====usCTOSS_DisplayTipAdjustApprovedUI=====");

    inRet = inCallJAVA_DisplayTipAdjustApprovedUI(szStringMsg, szOutBuf, &byLen);
	vdDebug_LogPrintf("saturn =====End usCTOSS_DisplayTipAdjustApprovedUI  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("saturn =====End usCTOSS_DisplayTipAdjustApprovedUI=====");

    return d_OK;
}

// for DCC -- sidumili
USHORT usCTOSS_ConfirmDCC(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_ConfirmDCC=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[1024 + 1];
    BYTE szOutBuf[1024 + 1];

	
	
    inRet = inCallJAVA_UserConfirmDCC(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_ConfirmDCC  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_ConfirmDCC=====");

    if (0 == strcmp(szOutBuf, "TO"))
        return d_TIMER_INVALID_PARA;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
        return d_USER_CANCEL;
	else if (0 == strcmp(szOutBuf, "TIME OUT"))
        return 0xFF;
    else
    {
    	memset(strGBLVar.szGBLvSelectedValue, 0x00, sizeof(strGBLVar.szGBLvSelectedValue));
		strcpy(strGBLVar.szGBLvSelectedValue, szOutBuf);
		
    	return d_OK;
    }
        
}

BYTE InputQWERTY(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usMaxLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *display) {
    vdDebug_LogPrintf("=====InputQWERTY=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];
    BYTE szMaxLen[4];
    BYTE szMinLen[5];

    vdDebug_LogPrintf("=====InputQWERTY=====");

    memset(szInBuf, 0x00, sizeof(szInBuf));
    memset(szOutBuf, 0x00, sizeof(szOutBuf));
    memset(pbaStr, 0x00, sizeof(pbaStr));
    memset(szMaxLen, 0x00, sizeof(szMaxLen));
    memset(szMinLen, 0x00, sizeof(szMinLen));

    sprintf(szMaxLen, "%d", usMaxLen);
    sprintf(szMinLen, "%d", usMinLen);

    strcpy(szInBuf, szMinLen); //min length
    strcat(szInBuf, "|");
    strcat(szInBuf, display);
    strcat(szInBuf, "|");
	strcat(szInBuf, szMaxLen); //max length
    vdDebug_LogPrintf("=====InputQWERTY=====szInBuf[%s]", szInBuf);

    inRet = inCallJAVA_InputQWERTY(szInBuf, szOutBuf, &byLen);
    strcpy(pbaStr, szOutBuf);
    vdDebug_LogPrintf("pbaStr[%s]", pbaStr);
    vdDebug_LogPrintf("=====End InputQWERTY=====");
    if (0 == strcmp(szOutBuf, "TIME OUT"))
        return 0xFF;
	else if (0 == strcmp(szOutBuf, "TO"))
        return 0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL"))
    {
        vdDebug_LogPrintf("=====Cancel InputQWERTY=====");
        return 'C';
    }
    else
    {
        vdDebug_LogPrintf("=====OK InputQWERTY=====");
        return 'A';
    }

    return d_OK;
}

int inCheckBattery(void)
{
    BYTE szOutBuf[5];
	int inCharginStatus;
	int BatteryLevel;
	BYTE szBuff[5];
	int inRet;

	vdDebug_LogPrintf("saturn =====inCheckBattery=====");

	memset(szBuff, 0x00, sizeof(szBuff));
    inCTOSS_GetEnvDB("TRANSBATLIMIT", szBuff);
			
	vdDebug_LogPrintf("strTCT.inMinBattery=[%d]", strTCT.inMinBattery);
	vdDebug_LogPrintf("TRANSBATLIMIT=[%s]", szBuff);

	if (strTCT.inMinBattery > 0){

    	memset(szOutBuf, 0x00, sizeof(szOutBuf));
		inCallJAVA_GetBatteryLevel(szOutBuf);
		vdDebug_LogPrintf("saturn =====inCheckBattery===== %s", szOutBuf);

		memset(szBuff, 0x00, sizeof(szBuff));
		szBuff[0] = szOutBuf[0];
		inCharginStatus = atoi(szBuff);

		memset(szBuff, 0x00, sizeof(szBuff));
		memcpy(szBuff, &szOutBuf[1], sizeof(szOutBuf)-1);
		BatteryLevel = atoi(szBuff);	

    	vdDebug_LogPrintf("saturn GET BATTERY LEVEL %d-%d", inCharginStatus, BatteryLevel);

		if (inCharginStatus == 0){
			
			if (BatteryLevel < strTCT.inMinBattery){
				vdDisplayErrorMsgResp2("BATTERY LOW", "PLEASE CHARGE", "");
				return d_NO;
			}	
			else
				return d_OK;

		}else if (inCharginStatus == 1){

			memset(szBuff, 0x00, sizeof(szBuff));
            inCTOSS_GetEnvDB("TRANSBATLIMIT", szBuff);

			vdDebug_LogPrintf("saturn trans battery limit %d", atoi(szBuff));
			
			if (BatteryLevel < atoi(szBuff)){
				vdDisplayErrorMsgResp2("INSUFFICIENT BATTERY", "CHARGE", "");
				return d_NO;
			}	
			else
				return d_OK;

		}
	}

    return d_OK;
}

BYTE InputTip(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter)
{
    vdDebug_LogPrintf("=====InputTip=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[512];
    BYTE szOutBuf[512];

    vdDebug_LogPrintf("InputTip");
    /*Still use the old way make it work first*/
    memset(szInBuf, 0x00, sizeof(szInBuf));
    memset(szOutBuf, 0x00, sizeof(szOutBuf));

    strcpy(szInBuf, szCurSymbol);

    inRet = inCallJAVA_GetTipString(szInBuf, szOutBuf, &byLen);

    //inCallJAVA_EnterAnyNum(&byLen, szOutBuf);
    //Tine --19Mar2019, to fix app crashing upon CANCEL from GetAmountString
    if (0 == strcmp(szOutBuf, "TO"))
        return 0xFF;
    else if (0 == strcmp(szOutBuf, "CANCEL")) {
        vdDebug_LogPrintf("=====Cancel InputTip=====");
        *ulAmount = 0;
        return d_USER_CANCEL;
    }
    else if (0 == strcmp(szOutBuf, "TIME OUT")) {
        vdDebug_LogPrintf("=====TIME OUT InputTip=====");
        *ulAmount = 0;
        return d_TIMEOUT;
    }
    else {
        memset(baAmount, 0x00, sizeof(baAmount));
        strcpy(baAmount, szOutBuf);
        vdDebug_LogPrintf("baAmount[%s] byLen[%d]", baAmount, byLen);
    }

    if (strlen(baAmount)>0 && byLen>0) {
        *ulAmount = atol(baAmount);
        vdDebug_LogPrintf("ulAmount[%ul] byLen[%d]", ulAmount, byLen);
    }
    else
        *ulAmount = 0;


    vdDebug_LogPrintf("=====End InputTip=====");
    return d_OK;

}

// sidumili -- printing animation
USHORT usCTOSS_PrintAnimation(BOOL isFade)
{
    vdDebug_LogPrintf("=====usCTOSS_PrintAnimation=====");

    BYTE byLen = 0;
    int inRet = 0;
    BYTE szOutBuf[1024 + 1];
	BYTE szTemp[1024 + 1] = {0};

	memset(szTemp, 0x00, sizeof(szTemp));
	strcat(szTemp, getApprovedTransInfo());
	strcat(szTemp, "|");
	strcat(szTemp, (isFade ? "FADE" : "ANIMATE"));

	vdDebug_LogPrintf("usCTOSS_PrintAnimation, len=[%d], szTemp=[%s]", strlen(szTemp), szTemp);
			
    inRet = inCallJAVA_PrintAnimation(szTemp, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_PrintAnimation  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_PrintAnimation=====");

    return d_OK;
        
}

USHORT usCTOSS_PrintAnimation2(BYTE *szDispString)
{
    vdDebug_LogPrintf("=====usCTOSS_PrintAnimation2=====");

    BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[1024 + 1];
    BYTE szOutBuf[1024 + 1];

	BYTE szTemp[1024 + 1] = {0};
	
	vdDebug_LogPrintf("usCTOSS_PrintAnimation2, len=[%d], szTemp=[%s]", strlen(szTemp), szTemp);
			
    inRet = inCallJAVA_PrintAnimation2(szDispString, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_PrintAnimation2  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_PrintAnimation2=====");

    return d_OK;
        
}

USHORT usCTOSS_PrintAnimation3(void)
{
    vdDebug_LogPrintf("=====usCTOSS_PrintAnimation3=====");

    BYTE byLen = 0;
    int inRet = 0;
    BYTE szOutBuf[1024 + 1];
	BYTE szTemp[1024 + 1] = {0};

	memset(szTemp, 0x00, sizeof(szTemp));
	strcat(szTemp, getApprovedTransInfo());

	vdDebug_LogPrintf("usCTOSS_PrintAnimation3, len=[%d], szTemp=[%s]", strlen(szTemp), szTemp);
			
    inRet = inCallJAVA_PrintAnimation3(szTemp, szOutBuf, &byLen);
	vdDebug_LogPrintf("=====End usCTOSS_PrintAnimation3  szOutBuf[%s]=====", szOutBuf);

    vdDebug_LogPrintf("=====End usCTOSS_PrintAnimation3=====");

	if (strcmp(szOutBuf, "CONFIRM PRINT MERCHANT COPY") == 0)
		inRet = MERCHANT_COPY_RECEIPT;
	else if (strcmp(szOutBuf, "CONFIRM PRINT CUSTOMER COPY") == 0)
		inRet = CUSTOMER_COPY_RECEIPT;
	else if (strcmp(szOutBuf, "CONFIRM PRINT BANK COPY") == 0)
		inRet = BANK_COPY_RECEIPT;

	vdDebug_LogPrintf("usCTOSS_PrintAnimation3, inRet=[%d]", inRet);
	
    return inRet;
        
}

