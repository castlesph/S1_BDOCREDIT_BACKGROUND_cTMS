#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>

#include "../Database/DatabaseFunc.h"
#include "pinpad.h"
#include "../Includes/wub_lib.h"
#include "../Includes/POSTypedef.h"
#include "..\Debug\Debug.h"
#include "../Ui/Display.h"
#include "..\FileModule\myFileFunc.h"
#include "../Includes/POSTrans.h"



#define PIN_POSITION_X	30
#define PIN_POSITION_Y	8


int inInitializePinPad(void)
{
    CTOS_KMS2Init();
    //TEST_Write3DES_Plaintext();
    return d_OK;
}

//For testing and development only, hardcode the key
void TEST_Write3DES_Plaintext(void)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	BYTE szInitialVector[8], szDataIn[100], szMAC[8];
	
	KeySet = 0x0002;
	KeyIndex = 0x0002;
	memcpy(KeyData, "\x11\x11\x11\x11\x22\x22\x22\x22\x33\x33\x33\x33\x44\x44\x44\x44", 16);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	para.Info.KeyAttribute = SBKMS2_KEYATTRIBUTE_PIN | SBKMS2_KEYATTRIBUTE_MAC;
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = KeyData;
	para.Value.KeyLength = IPP_TDES_KEY_SIZE;
	CTOS_KMS2KeyWrite(&para);

/*
	strcpy(srTransRec.szPAN,"1234567890123456");
	srTransRec.HDTid = 6;
	srTransRec.byTransType = SALE;
	strcpy(strCST.szCurSymbol,"SGD");
	strcpy(srTransRec.szTotalAmount,"1");
	inGetIPPPin();
	memset(szDataIn, 'A', 100);
	memset(szInitialVector, 0, 8);
	memset(szMAC, 0, 8);
	inIPPGetMAC(szDataIn, 8, szInitialVector, szMAC);*/
}

//aaronnino for BDOCLG ver 9.0 fix on issue #00209 Total amount display not align in the middle start
#if 0		
void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine)
{
	char szDisplayBuf[30];
	BYTE baAmount[20];

	DebugAddSTR("inCTOS_DisplayCurrencyAmount","Processing...",20);
	
	memset(baAmount, 0x00, sizeof(baAmount));
	wub_hex_2_str(szAmount, baAmount, 6);

	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
	sprintf(szDisplayBuf, "%s%10lu.%02lu", strCST.szCurSymbol, atol(baAmount)/100, atol(baAmount)%100);
	setLCDPrint(inLine, DISPLAY_POSITION_LEFT, szDisplayBuf);
	
}
//#else  //exisitng function for V3 version 12

void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine, int inDisplayBalancePos) //aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE start *added parameter inDisplayBalancePos 3 of 6
{
	char szDisplayBuf[30];
	char szRawAmount[20+1], szFmtAmount[20+1], szWholeAmt[20+1], szRemainderAmt[20+1];
	BYTE baAmount[20];

	DebugAddSTR("inCTOS_DisplayCurrencyAmount","Processing...",20);
	memset(baAmount, 0x00, sizeof(baAmount));
	wub_hex_2_str(szAmount, baAmount, 6);

	

	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	sprintf(szDisplayBuf, "%s", strCST.szCurSymbol);
	//sprintf(szDisplayBuf, "%s%10lu.%02lu", strCST.szCurSymbol, atol(baAmount)/100, atol(baAmount)%100);
    sprintf(szWholeAmt,"%lu",atol(baAmount)/100);
	sprintf(szRemainderAmt,"%02lu",atol(baAmount)%100);

	if(strcmp(szRemainderAmt,"00") == 0)
    {
		memset(szRemainderAmt,0,sizeof(szRemainderAmt));
		memcpy(&szRemainderAmt,"00",2);
    }

	strcat(szWholeAmt,szRemainderAmt);
	strcpy(szRawAmount,szWholeAmt);

	
//issue-00371
	vdCTOS_FormatAmount(strCST.szAmountFormat, szRawAmount,szFmtAmount);
	strcat(szDisplayBuf,szFmtAmount);

	setLCDPrint(inLine, inDisplayBalancePos, szDisplayBuf);
}
#else
void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine)
{
	char szDisplayBuf[30];
	BYTE baAmount[20];
    BYTE szStr[30];
	
	DebugAddSTR("inCTOS_DisplayCurrencyAmount","Processing...",20);
	
	memset(baAmount, 0x00, sizeof(baAmount));
	wub_hex_2_str(szAmount, baAmount, 6);

	memset(szStr, 0x00, sizeof(szStr));
	vdCTOS_FormatAmount("NNN,NNN,NNn.nn",baAmount, szStr);
	sprintf(szDisplayBuf, "%s %s", strCST.szCurSymbol, szStr);
	vdDebug_LogPrintf("AAA - inCTOS_DisplayCurrencyAmount szDisplayBuf:%s",szDisplayBuf);
	//setLCDPrint(inLine, DISPLAY_POSITION_LEFT, szDisplayBuf);
	setLCDPrint(inLine, DISPLAY_POSITION_CENTER, szDisplayBuf);
	
	
}

#endif

void OnGetPINDigit(BYTE NoDigits)
{
	BYTE i;
	
	for(i=0;i<NoDigits;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y,"*");
		
	for(i=NoDigits;i<12;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y," ");
	
   DebugAddINT("OnGetPINDigit", NoDigits);

}

void OnGetPINCancel(void)
{
   DebugAddINT("OnGetPINCancel", 1);

}

void OnGetPINBackspace(BYTE NoDigits)
{
	BYTE i;
	
	for(i=0;i<NoDigits;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y,"*");
		
	for(i=NoDigits;i<12;i++)
		CTOS_LCDTPrintXY(PIN_POSITION_X+i, PIN_POSITION_Y," ");
   DebugAddINT("OnGetPINBackspace", NoDigits);
}

#if 0
int inGetIPPPin(void)
{
	CTOS_KMS2PINGET_PARA_VERSION_7 stPinGetPara;

	BYTE str[40],key;
	USHORT ret;
	char szDebug[40 + 1]={0};

	int inRet;

	vdDebug_LogPrintf("saturn BDO inGetIPPPin");


	if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		return d_OK;


	
	usCTOSS_GetPinUI();
	
	DebugAddSTR("inGetIPPPin","Processing...",20);
	inDCTRead(srTransRec.HDTid,srTransRec.MITid);
	
	/*********************************/
	vdDebug_LogPrintf("saturn inGetIPPPin | HDTid[%d] %d", srTransRec.HDTid, srTransRec.MITid);
	vdDebug_LogPrintf("saturn inGetIPPPin | byTransType[%d]", (int)srTransRec.byTransType);
	vdDebug_LogPrintf("saturn inGetIPPPin | usKeySet[%d]", strDCT.usKeySet);
	vdDebug_LogPrintf("saturn inGetIPPPin | usKeyIndex[%d]", strDCT.usKeyIndex);
	vdDebug_LogPrintf("SATURN inGetIPPPin | strDCT.inMinPINDigit[%d]", strDCT.inMinPINDigit);
	vdDebug_LogPrintf("inGetIPPPin | strDCT.inMaxPINDigit[%d]", strDCT.inMaxPINDigit);
	vdDebug_LogPrintf("inGetIPPPin | strDCT.szDisplayLine1[%s]", strDCT.szDisplayLine1);
	vdDebug_LogPrintf("inGetIPPPin | strDCT.szDisplayLine2[%s]", strDCT.szDisplayLine2);
	vdDebug_LogPrintf("inGetIPPPin | strDCT.szPINKey[%s]", strDCT.szPINKey);
	/*********************************/

	/*check for keys if injected -- sidumili*/
	inRet = inCheckKeys(strDCT.usKeySet, strDCT.usKeyIndex);
	if (inRet != d_OK)
	return(inRet);
	/*check for keys if injected -- sidumili*/

	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);

	// sidumili: [NO AMOUNT DISPLAY DURING BALANCE INQUIRY]
	if (srTransRec.byTransType != BALANCE_INQUIRY){
//		inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
		inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3, DISPLAY_POSITION_LEFT);
	}
	
	setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
	
	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_7));
	stPinGetPara.Version = 0x07;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;

	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
//add start        
	stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
    stPinGetPara.Protection.pSK = strDCT.szPINKey;
//add end        
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
	stPinGetPara.Control.Timeout = 0;
	stPinGetPara.Control.NULLPIN = FALSE;
	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
	stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
	stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
	stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

	vdDebug_LogPrintf("SATURN BEFORE CTOS_KMS2PINGet");

	ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);


	/*********************************/
	vdDebug_LogPrintf("SATURN inGetIPPPin | ret[%d]", ret);
	vdDebug_LogPrintf("SATURN inGetIPPPin | szPINBlock[%02X][%02X][%02X]", (unsigned char)srTransRec.szPINBlock[0], (unsigned char)srTransRec.szPINBlock[1], (unsigned char)srTransRec.szPINBlock[2]);
	vdDebug_LogPrintf("SATURN inGetIPPPin | szPAN[%s]", srTransRec.szPAN);
	/*********************************/

	
	if(ret != d_OK)
	{
		if (ret = 10511){
			//vdDispErrMsg("PLEASE INJECT KEY");
			//vdDispErrMsg("USER CANCEL");
			vdSetErrorMessage("USER CANCEL");
		}
		else{
			sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
			CTOS_LCDTPrintXY(1, 8, str);
			CTOS_KBDGet(&key);
			vdDebug_LogPrintf("inGetIPPPin | key[%s]", key);		
		}
		
		return ret;
		
	}
	return d_OK;
}
#else
int inGetIPPPin(void)
{
	CTOS_KMS2PINGET_PARA_VERSION_7 stPinGetPara;

	BYTE str[40],key;
	USHORT ret;
	char szDebug[40 + 1]={0};
	char szDisplayBalAcctTitle [150+1];

	int inRet;
    BYTE szBaseAmt[20+1], szAmtBuff[20+1], szCurAmtBuff[50+1];

	vdDebug_LogPrintf("SATURN inGetIPPPin");

	vdDebug_LogPrintf("::byEntryMode[%d]", srTransRec.byEntryMode);
	
	if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
		return d_OK;

	
	usCTOSS_GetPinUI();
	
	vdDebug_LogPrintf("SATURN AFTER usCTOSS_GetPinUI");
	
	//inDCTRead(srTransRec.HDTid);
	inDCTRead(srTransRec.HDTid,srTransRec.MITid);
	vdDebug_LogPrintf("SATURN Processing");

	
	/*********************************/
#if 1
	vdDebug_LogPrintf("SATURN inGetIPPPin | MITid[%d]", srTransRec.MITid);
	vdDebug_LogPrintf("SATURN inGetIPPPin | HDTid[%d]", srTransRec.HDTid);
	vdDebug_LogPrintf("SATURN inGetIPPPin | byTransType[%d]", (int)srTransRec.byTransType);
	vdDebug_LogPrintf("SATURN inGetIPPPin | usKeySet[%d]", strDCT.usKeySet);
	vdDebug_LogPrintf("SATURN inGetIPPPin | usKeyIndex[%d]", strDCT.usKeyIndex);
	vdDebug_LogPrintf("SATURN inGetIPPPin | strDCT.inMinPINDigit[%d]", strDCT.inMinPINDigit);
	vdDebug_LogPrintf("SATURNinGetIPPPin | strDCT.inMaxPINDigit[%d]", strDCT.inMaxPINDigit);
	vdDebug_LogPrintf("saturn inGetIPPPin | strDCT.szDisplayLine1[%s]", strDCT.szDisplayLine1);
	vdDebug_LogPrintf("saturn inGetIPPPin | strDCT.szDisplayLine2[%s]", strDCT.szDisplayLine2);
	vdDebug_LogPrintf("saturn inGetIPPPin | strDCT.szPINKey[%s]", strDCT.szPINKey);
	//vdDebug_LogPrintf("inGetIPPPin | PAN[%s] | SourceCard[%s] | DestinationCard[%s]", srTransRec.szPAN, srTransRec.szSourceCardNo, srTransRec.szDestCardNo);
	/*********************************/
#endif

	vdDebug_LogPrintf("SATURN check keys");

#if 1
	
	/*check for keys if injected -- sidumili*/
	inRet = inCheckKeys(strDCT.usKeySet, strDCT.usKeyIndex);

	vdDebug_LogPrintf("SATURN check keys return %d", inRet);

	if (inRet != d_OK)
		return(inRet);
	/*check for keys if injected -- sidumili*/
#endif
	vdDebug_LogPrintf("SATURN AFTER CHECK KEYS");

	CTOS_LCDTClearDisplay();

    //CTOS_LCDSelectModeEx(d_LCD_TEXT_MODE, d_TRUE);
    CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
    //CTOS_LCDTTFSelect((BYTE *) "ca_default.ttf", 0);
    CTOS_LCDTSelectFontSize(d_FONT_20x40);
    //CTOS_LCDBackGndColor(0x0000ff00);c

	if (srTransRec.byTransType == SALE)
    {
       strcpy(szDisplayBalAcctTitle,"SALE                                                  CUP");   
	   setLCDPrint(1, DISPLAY_POSITION_LEFT, szDisplayBalAcctTitle);
       //vdDispTitleString(szDisplayBalAcctTitle);
	}
    

	setLCDPrint(5, DISPLAY_POSITION_CENTER, strDCT.szDisplayLine1);
	setLCDPrint(6, DISPLAY_POSITION_CENTER, strDCT.szDisplayLine2);

	
	memset(&stPinGetPara, 0x00, sizeof(CTOS_KMS2PINGET_PARA_VERSION_7));
	stPinGetPara.Version = 0x07;
	stPinGetPara.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	stPinGetPara.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	stPinGetPara.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;

	stPinGetPara.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	stPinGetPara.Protection.CipherKeySet = strDCT.usKeySet;
//add start        
	stPinGetPara.Protection.SK_Length = IPP_TDES_KEY_SIZE;
    stPinGetPara.Protection.pSK = strDCT.szPINKey;
//add end        
	stPinGetPara.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;


	
	stPinGetPara.AdditionalData.InLength = strlen(srTransRec.szPAN);
	stPinGetPara.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
        
	//stPinGetPara.Control.Timeout = 30; /*BDO: Set 30secs timeout --sidumili*/
	stPinGetPara.Control.Timeout = inGetIdleTimeOut(TRUE);
	stPinGetPara.Control.NULLPIN = FALSE;
	stPinGetPara.PINOutput.EncryptedBlockLength = 8;
	stPinGetPara.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
	stPinGetPara.EventFunction.OnGetPINBackspace = OnGetPINBackspace;
	stPinGetPara.EventFunction.OnGetPINCancel = OnGetPINCancel;
	stPinGetPara.EventFunction.OnGetPINDigit = OnGetPINDigit;

	vdDebug_LogPrintf("SATURN BEFORE CTOS_KMS2PINGet");

	//CTOS_LCDSelectModeEx(d_LCD_TEXT_320x240_MODE, FALSE); // Fix for undisplay of * during pin -- sidumili	
	ret = CTOS_KMS2PINGet((CTOS_KMS2PINGET_PARA *)&stPinGetPara);

	/*********************************/
	vdDebug_LogPrintf("saturn inGetIPPPin | ret[%d]", ret);
	vdDebug_LogPrintf("inGetIPPPin | szPINBlock[%02X][%02X][%02X]", (unsigned char)srTransRec.szPINBlock[0], (unsigned char)srTransRec.szPINBlock[1], (unsigned char)srTransRec.szPINBlock[2]);
	vdDebug_LogPrintf("inGetIPPPin | szPAN[%s]", srTransRec.szPAN);
	/*********************************/

	
	if(ret != d_OK)
	{
		if (ret == 10511){
			//vdDispErrMsg("PLEASE INJECT KEY");
			//vdDispErrMsg("USER CANCEL");
			vdSetErrorMessage("USER CANCEL");
			vdDisplayErrorMsg(1, 7, "PIN CANCELLED");

		}
		else if (ret == 10512) /*Timeout --sidumili*/ 
		{
			//fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
			vdSetErrorMessage("PIN TIMEOUT");
			vdDisplayErrorMsg(1, 7, "PIN TIMEOUT");

		}
		else{
			if(ret != 10500)
			{
				sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
				CTOS_LCDTPrintXY(1, 8, str);
				CTOS_KBDGet(&key);
				vdDebug_LogPrintf("inGetIPPPin | key[%s]", key);
			}
			else// if return is 10500, terminal will prompt user for PIN entry retry
			{
				CTOS_LCDTClearDisplay();			
				vdDisplayErrorMsgResp2("PIN CAPTURE","ERROR","PLEASE TRY AGAIN");
			}
		}
		
		return ret;
		
	}
	return d_OK;
}

#endif
int inIPPGetMAC(BYTE *szDataIn, int inLengthIn, BYTE *szInitialVector, BYTE *szMAC)
{
	CTOS_KMS2MAC_PARA para;
	USHORT ret;
	BYTE key,str[40];
	
	CTOS_LCDTClearDisplay();
	DebugAddSTR("inGetIPPMAC","Processing...",20);    
	
	memset(&para, 0x00, sizeof(CTOS_KMS2MAC_PARA));
	para.Version = 0x01;
	para.Protection.CipherKeySet = strDCT.usKeySet;
	para.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	para.Protection.CipherMethod = KMS2_MACMETHOD_CBC;
	para.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	para.Protection.pSK = strDCT.szMACKey;
	para.ICV.Length = 8;
	para.ICV.pData = szInitialVector;
	para.Input.Length = inLengthIn;
	para.Input.pData = szDataIn;
	para.Output.pData = szMAC;
	
	ret = CTOS_KMS2MAC(&para);
	if(ret != d_OK)
		return ret;
	return d_OK;		
}

int inCalculateMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC)
{
	BYTE szInitialVector[8];

	inIPPGetMAC(szDataIn, inLengthIn,  szInitialVector, szMAC);
}
	
void vdCTOS_PinEntryPleaseWaitDisplay(void){

/*************************************************************/
CTOS_LCDTClearDisplay();
vdDispTransTitle(srTransRec.byTransType);
setLCDPrint27(8,DISPLAY_POSITION_LEFT, "Please Wait...");

CTOS_KBDBufFlush(); // sidumili: clear buffer of keyboard
/*************************************************************/ 

}
	

/*sidumili: [check key if injected]*/
int inCheckKeys(USHORT ushKeySet, USHORT ushKeyIndex){
	USHORT rtn;

	vdDebug_LogPrintf("inCheckKeys SET[%04X] IDX[%04X]", ushKeySet, ushKeyIndex);

	/*just call once when terminal startup*/
	//CTOS_KMS2Init();

	rtn = CTOS_KMS2KeyCheck(ushKeySet, ushKeyIndex);

	vdDebug_LogPrintf("inCheckKeys CTOS_KMS2KeyCheck rtn[%d]", rtn);

	//if (rtn == d_OK)
              //vdDisplayErrorMsg(1, 7, "Key Check OK");
	//else
	if (rtn != d_OK)
    	vdDisplayErrorMsg(1, 7, "PLEASE INJECT KEY");

	return rtn;

}

int htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
    }
    return n;
}


int inCTOS_KMS2PINGetExDukpt(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, USHORT pinBypassAllow)
{
	BYTE szIndex[2]={0};
	USHORT inRet = 0;

         vdDebug_LogPrintf("inGetIPPPinEx Start KeySet[%d] KeyIndex[%d]", KeySet, KeyIndex);
         vdDebug_LogPrintf("pInData[%s] fPinBypassAllow[%d]", pInData, pinBypassAllow);
         DebugAddHEX("inCTOS_KMS2PINGetExDukpt szKSN*", szKSN, 10);
         inCallJAVA_inGetIPPPinEx(KeySet, KeyIndex, pInData, szPINBlock, szKSN,  szIndex, pinBypassAllow);
         vdDebug_LogPrintf("szIndex[%c]", szIndex[0]);
         vdDebug_LogPrintf("inGetIPPPinEx End szIndex[%c]", szIndex[0]);
         DebugAddHEX("szIndex*", szIndex, 2);
         DebugAddHEX("szPINBlock*", szPINBlock, 8);

	inRet = atoi(szIndex);

	if(inRet == 0)
	{
		return d_OK;
	}
	else
	{
        inRet = htoi(szKSN);	
		return inRet;
    }
}


USHORT inCTOS_KMS2PINGetEx3Des(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, USHORT pinBypassAllow)
{

    BYTE szIndex[2]={0};
    USHORT inRet = 0;

    vdDebug_LogPrintf("saturn inGetPIN_With_3DESDUKPTEx Start KeySet[%d] KeyIndex[%d]", KeySet, KeyIndex);
    vdDebug_LogPrintf("pInData[%s]", pInData);
    inCallJAVA_inGetPIN_With_3DESDUKPTEx(KeySet, KeyIndex, pInData, szPINBlock, szKSN,  szIndex, pinBypassAllow);
    vdDebug_LogPrintf("szIndex[%c]", szIndex[0]);
    vdDebug_LogPrintf("inGetPIN_With_3DESDUKPTEx End szIndex[%c]", szIndex[0]);

    inRet = atoi(szIndex);

    if(inRet == 0)
    {
        return d_OK;
    }
    else
    {
        inRet = htoi(szKSN);
        return inRet;
    }
}



int inGetIPPPinEx(void)
{
    int inRet;
	
	BYTE str[40],key;
	USHORT ret;
	BYTE PINBlockStr[20];
    BYTE ksn[20];
    BYTE ksnStr[20];;
	BYTE szBuffer[70];
    BYTE strPINKEY[32 + 1];
	BYTE szTitle[40 + 1] = {0};
		
	vdDebug_LogPrintf("saturn --inGetIPPPinEx");
	vdDebug_LogPrintf("HDTid=[%d], MITid=[%d]", srTransRec.HDTid, srTransRec.MITid);
	vdDebug_LogPrintf("srTransRec.fEMVPINEntered =[%d]", srTransRec.fEMVPINEntered);
	vdDebug_LogPrintf("srTransRec.fEMVPIN =[%d]", srTransRec.fEMVPIN);
	vdDebug_LogPrintf("srTransRec.fPINByPassAllow =[%d]", srTransRec.fPINByPassAllow);
	
	inDCTRead(srTransRec.HDTid, srTransRec.MITid);


	//CTOSS_Load_3DES_TMK_PlaintextNewPAN("1111111111111111");

	vdDebug_LogPrintf("CTOS_KMS2Init");
	CTOS_KMS2Init();


	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);

	memset(szTitle, 0x00, sizeof(szTitle));
	//szGetTransTitle(srTransRec.byTransType, szTitle);
	szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);

	if(srTransRec.byTransType != BALANCE_INQUIRY)
	    inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);
	

	vdDebug_LogPrintf("saturn before ");
     
	
	memset(PINBlockStr, 0x00, sizeof(PINBlockStr));
    memset(ksnStr, 0x00, sizeof(ksnStr));

    vdDebug_LogPrintf("saturn keyset = %d keyindex = %d", strDCT.usKeySet, strDCT.usKeyIndex);

    vdDebug_LogPrintf("saturn keyset = %x keyindex = %x", strDCT.usKeySet, strDCT.usKeyIndex);

    memset(szBuffer, 0x00, sizeof(szBuffer));
	strcpy(szBuffer, szTitle);
	strcat(szBuffer, "^");
    strcat(szBuffer, srTransRec.szPAN);
    strcat(szBuffer, "|");

    memset(strPINKEY, 0x00, sizeof(strPINKEY));
    wub_hex_2_str(strDCT.szPINKey, strPINKEY, 16);
    strcat(szBuffer, strPINKEY);

    vdDebug_LogPrintf("saturn pimdata %s", szBuffer);
	
	if(srTransRec.fPINByPassAllow == TRUE)
        ret = inCTOS_KMS2PINGetEx3Des(strDCT.usKeySet, strDCT.usKeyIndex, szBuffer, PINBlockStr, ksnStr, 1);
	else
		ret = inCTOS_KMS2PINGetEx3Des(strDCT.usKeySet, strDCT.usKeyIndex, szBuffer, PINBlockStr, ksnStr, 0);

	vdDebug_LogPrintf("saturn pin return %d", ret);
	
	wub_str_2_hex(PINBlockStr, srTransRec.szPINBlock, strlen(PINBlockStr));

	vdPCIDebug_HexPrintf("saturn GetPIN_With_3DESDUKPTEx srTransRec.szPINBlock", srTransRec.szPINBlock, 8);
	
	if (ret != d_OK) {
		DisplayStatusLine("");
        if (ret == d_KMS2_GET_PIN_NULL_PIN) {
            //CTOS_LCDTPrintXY(1, 8, "PIN BYPASSED");
            //CTOS_Delay(300);
            return d_KMS2_GET_PIN_NULL_PIN;
        } else if (ret == 10511) {
            vdDisplayErrorMsg(1, 8, "PIN CANCELLED");
            return (d_NO);
        } else if (ret == 10512) /*Timeout --sidumili*/
        {
            vdDisplayErrorMsg(1, 8, "PIN TIMEOUT");
            return (d_NO);
        } else if (ret == d_KMS2_KEY_NOT_EXIST) {
            vdDisplayErrorMsg(1, 8, "PLEASE INJECT KEY");
            return (d_NO);
        } else {
            /* MCCCLG: Added message if user cancelled during pin entry - start -- jzg*/
            if (ret == d_KMS2_GET_PIN_ABORT) {
                vdDisplayErrorMsg(1, 8, "USER CANCEL");
            } else {
                sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
                vdDisplayErrorMsg(1, 8, str);
            }
            /* MCCCLG: Added message if user cancelled during pin entry - end -- jzg*/
            return ret;
        }
    }
	
	return d_OK;
}

int CTOSS_Load_3DES_TMK_PlaintextNew(char *szKeyData) {
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;

	CTOS_LCDTClearDisplay();

	CTOS_LCDTPrintXY(1, 1, "Write 3DES PT");

	//---------------------------------------------------------------------
	// Write 3DES Key in plaintext	

	inDCTRead(srTransRec.HDTid, srTransRec.MITid);
	KeySet = strDCT.usKeySet;
	KeyIndex = strDCT.usKeyIndex;

	KeySet = 0xC001;
	KeyIndex = 0x01;

	vdDebug_LogPrintf("**CTOSS_Load_3DES_TMK_Plaintext*,strDCT.usKeySet[%x]strDCT.usKeyIndex[%x]*",
					  strDCT.usKeySet, strDCT.usKeyIndex);

	sprintf(str, "KeySet = %04X", KeySet);
	CTOS_LCDTPrintXY(1, 3, str);
	sprintf(str, "KeyIndex = %04X", KeyIndex);
	CTOS_LCDTPrintXY(1, 4, str);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
#ifdef ANDROID
	para.Info.KeyAttribute = SBKMS2_KEYATTRIBUTE_PIN;
#else
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
#endif
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = szKeyData;
	para.Value.KeyLength = 16;

	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("**TMK CTOS_KMS2KeyWrite*ret[%d]*", ret);
	if (ret != d_OK) {
		sprintf(str, "ret = 0x%04X", ret);
		CTOS_LCDTPrintXY(1, 8, str);
		return d_NO;
	} else
		return d_OK;


	CTOS_LCDTPrintXY(1, 7, "Write Key Done");
	__Leave:
	CTOS_KBDGet(&key);
}

int CTOSS_Load_3DES_TMK_PlaintextNewTest(char *szKeyData)
{
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE KeyData[16];
	BYTE str[17];
	BYTE key;
	
	CTOS_LCDTClearDisplay();
            
	CTOS_LCDTPrintXY(1, 1, "Write 3DES PT");
	
	//---------------------------------------------------------------------
	// Write 3DES Key in plaintext	
	//KeySet = 0xC001;
    //KeyIndex = 0x01;
	KeySet = 0xC000;
    KeyIndex = 0x04;
	
	vdDebug_LogPrintf("**CTOSS_Load_3DES_TMK_Plaintext*,KeySet[%x]KeyIndex[%x]*", KeySet, KeyIndex);
        
	sprintf(str, "KeySet = %04X", KeySet);
	CTOS_LCDTPrintXY(1, 3, str);
	sprintf(str, "KeyIndex = %04X", KeyIndex);
	CTOS_LCDTPrintXY(1, 4, str);
	
	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
#ifdef ANDROID
	para.Info.KeyAttribute = SBKMS2_KEYATTRIBUTE_PIN;
#else
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
#endif
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = szKeyData;
	para.Value.KeyLength = 16;

	ret = CTOS_KMS2KeyWrite(&para);
    
	vdDebug_LogPrintf("**TMK CTOS_KMS2KeyWrite*ret[%d]*",ret );
	if(ret != d_OK)
	{
		sprintf(str, "ret = 0x%04X", ret);
		CTOS_LCDTPrintXY(1, 8, str);
        return d_NO;
	}
    else
        return d_OK;
    

	CTOS_LCDTPrintXY(1, 7, "Write Key Done");
__Leave:
	CTOS_KBDGet(&key);
}

void vdCTOSS_GetKSN(void)
{
	BYTE baTmlKSN[10];
	BYTE baExtPPKSN[10];
	BYTE ksn_len = 10;
	USHORT ushRet = d_OK;

   /*Get Tml KSN*/
   memset(baTmlKSN, 0x00, sizeof(baTmlKSN));
   //ushRet = CTOS_KMS2DUKPTGetKSN(0xC000, 0x0004, baTmlKSN, &ksn_len);
   ushRet = CTOS_KMS2DUKPTGetKSN(strDCT.usKeySet, strDCT.usKeyIndex, baTmlKSN, &ksn_len);
   vdDebug_LogPrintf("CTOS_KMS2DUKPTGetKSN[%04X] ksn_len[%d]", ushRet, ksn_len);
   DebugAddHEX("TML KSN:", baTmlKSN, 10);
   memcpy(srTransRec.szKSN, baTmlKSN, 10);
   DebugAddHEX("srTransRec.szKSN", srTransRec.szKSN, 10);

}
void vdCTOSS_EFT_TestHardcodeKey(void)
{
    USHORT ret;
    BYTE msg[16];
    BYTE basekey[16];
    BYTE ksn[10];
    CTOS_KMS2KEYWRITE_PARA params;
    int i;
    BYTE testksn[10];
    BYTE testksnlen = 10;
	int inKeySet; 
	int inKeyIndex;
	
	BYTE  IPEK[16] = {0xB4, 0x60, 0xA5, 0x1C, 0x2D, 0x45, 0x88, 0x54, 0x5C, 0x58, 0x94, 0xAD, 0x3F, 0x97, 0xB0, 0x6D};
					//IPEK:      B460A51C2D4588545C5894AD3F97B06D
	BYTE  InitiallyLoadedKeySerialNumber[10] = {0x00, 0x11, 0x00, 0x10, 0x00, 0x00, 0x0A, 0x40, 0x00, 0x00};

    //CTOS_KMS2Erase();


    
    memset(&params, 0, sizeof(params));

	//inKeySet = 49152;//0xC000;
	//inKeyIndex = 4; //0x0004;

	inKeySet = 0xC000;
	inKeyIndex = 0x0004;

	//CTOS_KMS2DUKPTGetKSN[0]inKeySet[49152]inKeyIndex[4]
    //vdEFT_Debug_LogPrintf("IPEK[%02x %02x %02x %02x %02x]", IPEK[0], IPEK[1], IPEK[2], IPEK[3], IPEK[4]);
    //vdEFT_Debug_LogPrintf("KSN[%02x %02x %02x %02x %02x]", InitiallyLoadedKeySerialNumber[0], InitiallyLoadedKeySerialNumber[1], InitiallyLoadedKeySerialNumber[2], InitiallyLoadedKeySerialNumber[3], InitiallyLoadedKeySerialNumber[4]);
	DebugAddHEX("IPEK",(char *)IPEK,16);
	DebugAddHEX("KSN",(char *)InitiallyLoadedKeySerialNumber,10);
    memcpy(basekey, IPEK, 16);
    memcpy(ksn, InitiallyLoadedKeySerialNumber, 10);        
    params.Version = 0x01;
    params.Info.KeySet = inKeySet;
    params.Info.KeyIndex = inKeyIndex;
    params.Info.KeyType = KMS2_KEYTYPE_3DES_DUKPT;
    params.Info.KeyVersion = 0x01;
    params.Info.KeyAttribute = SBKMS2_KEYATTRIBUTE_PIN;
    params.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;        
    params.Value.KeyLength = 16;
    params.Value.pKeyData = basekey;
    params.DUKPT_DATA.KSNLength = 10;
    params.DUKPT_DATA.pKSN = ksn;        

    ret = CTOS_KMS2KeyWrite(&params);    

    vdDebug_LogPrintf("vdCTOSS_EFT_TestHardcodeKey[%d]", ret);

}


int inCTOS_InjectKeyTest(void)
{
	BYTE	key;
	BYTE	KeyType;
	int i;
	int inRet = d_NO;
	char szKeyData[32+1] ={'\0'};
	USHORT ushLen = 16;
 	char szResponseString[50],szResponseString2[50], szReResponseString[50];
	char szResponseStringHex[33],szResponseString2Hex[33];
 	char szMsg[23];
	BYTE bRet;

	CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("inCTOS_InjectKeyTest");

	{
		memcpy(szKeyData,"\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33",16);

		CTOS_KMS2Init();
		
		inRet = CTOSS_Load_3DES_TMK_PlaintextNewTest(szKeyData);

		CTOS_LCDTClearDisplay();
		vduiWarningSound();
		vduiDisplayStringCenter(7,"INJECT SUCCESS");
		CTOS_Delay(1500);
		vdSetErrorMessage("");
	}


	return d_OK;
}


int GetPIN_With_3DESDUKPTEx(void)
{
	USHORT ret;
	BYTE str[17];
	BYTE key;
	BYTE PINBlock[16];
	BYTE ksn_Len;
	int inRet;

	BYTE PINBlockStr[20];
    BYTE ksn[20];
    BYTE ksnStr[20];



	vdDebug_LogPrintf("--GetPIN_With_3DESDUKPTEx--");

	// No pin entry on installment transaction -- sidumili
	if (srTransRec.fInstallment == TRUE)
	{
		return d_OK;
	}
	
	DebugAddSTR("GetPIN_With_3DESDUKPTEx","Processing...",20);

	
	vdDebug_LogPrintf("inCTOS_InjectKeyTest");

	
	vdDebug_LogPrintf("CTOS_KMS2Init");
	CTOS_KMS2Init();
	
	inDCTRead(srTransRec.HDTid, srTransRec.MITid);

	vdDebug_LogPrintf("saturn before ksn");
  
   // memcpy(srTransRec.szKSN, ksn, 20);

	
    CTOS_LCDFontSelectMode(d_FONT_FNT_MODE); 

    vdDebug_LogPrintf("saturn before inCTOS_KMS2PINGetExDukpt");


        if (inCTOSS_CheckCVMOnlinepin() == d_OK && srTransRec.IITid == 8)
                return d_OK;
	

    memset(PINBlockStr, 0x00, sizeof(PINBlockStr));
    memset(ksnStr, 0x00, sizeof(ksnStr));

   // strDCT.usKeySet = 0xC001;
  //  strDCT.usKeyIndex = 0x01;

    vdCTOSS_GetKSN();

   //wub_str_2_hex(srTransRec.szPINBlock, PINBlockStr, strlen(PINBlockStr));
    //wub_str_2_hex(srTransRec.szKSN, ksn, strlen(ksnStr));


   // ret = inCTOS_KMS2PINGetExDukpt(strDCT.usKeySet, strDCT.usKeyIndex, srTransRec.szPAN, PINBlockStr, ksnStr, 0);
   ret = inCTOS_KMS2PINGetExDukpt(strDCT.usKeySet, strDCT.usKeyIndex, srTransRec.szPAN, srTransRec.szPINBlock, ksnStr, 1); //ksnStr has no value

   
    //wub_str_2_hex(PINBlockStr, srTransRec.szPINBlock, strlen(PINBlockStr));
    //wub_str_2_hex(srTransRec.szKSN, ksn, strlen(ksnStr));	   
   
    vdDebug_LogPrintf("inCTOS_KMS2PINGetExDukpt[%d]", ret);
    vdPCIDebug_HexPrintf("GetPIN_With_3DESDUKPTEx srTransRec.szPINBlock", srTransRec.szPINBlock, 8);
    vdPCIDebug_HexPrintf("GetPIN_With_3DESDUKPTEx srTransRec.szKSN", srTransRec.szKSN, 10);

	
    if(ret != d_OK)
    {
        srTransRec.fEMVPINBYPASS = FALSE;
        if(ret == d_KMS2_GET_PIN_NULL_PIN)
        {
            CTOS_LCDTPrintXY(1, 8, "PIN BYPASSED");
            CTOS_Delay(300);
	  srTransRec.fEMVPINBYPASS = TRUE;
            return d_KMS2_GET_PIN_NULL_PIN;
        }
        else if (ret == 10511)
        {
            vdSetErrorMessage("USER CANCEL");
            return(d_NO);
        }
        else if (ret == 10512) /*Timeout --sidumili*/ 
        {
            vdDisplayErrorMsg(1, 8, "PIN TIMEOUT");
            return(d_NO);
        }
        else if (ret == d_KMS2_KEY_NOT_EXIST)
        {
            vdDisplayErrorMsg(1, 8, "PLEASE INJECT KEY");
            return(d_NO);
        }
        else
        {
            /* MCCCLG: Added message if user cancelled during pin entry - start -- jzg*/
            if(ret == d_KMS2_GET_PIN_ABORT)
            {
                vdDisplayErrorMsg(1, 8, "USER CANCEL");
            }
            else
            {
                sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
                vdDisplayErrorMsg(1, 8, str);
            }
            /* MCCCLG: Added message if user cancelled during pin entry - end -- jzg*/
            return ret;
        }
    }
		
    return d_OK;
}




/*   
int inOldGetIPPPin1(void)
{
	CTOS_KMS2PINGET_PARA para;
	BYTE str[17],key;
	USHORT ret;
	
	DebugAddSTR("inGetIPPPin","Processing...",20);
	inDCTRead(srTransRec.HDTid);
	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	
	inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 3);
	setLCDPrint(4, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine1);
	setLCDPrint(5, DISPLAY_POSITION_LEFT, strDCT.szDisplayLine2);

	memset(&para, 0x00, sizeof(CTOS_KMS2PINGET_PARA));
	para.Version = 0x01;
	para.PIN_Info.BlockType = KMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0;
	para.PIN_Info.PINDigitMinLength = strDCT.inMinPINDigit;
	para.PIN_Info.PINDigitMaxLength = strDCT.inMaxPINDigit;
	para.Protection.CipherKeySet = strDCT.usKeySet;
	para.Protection.CipherKeyIndex = strDCT.usKeyIndex;
	para.Protection.CipherMethod = KMS2_PINCIPHERMETHOD_ECB;
	para.Protection.SK_Length = IPP_TDES_KEY_SIZE;
	para.Protection.pSK = strDCT.szPINKey;
	para.AdditionalData.InLength = strlen(srTransRec.szPAN);
	para.AdditionalData.pInData = (BYTE*)srTransRec.szPAN;
	para.PINOutput.EncryptedBlockLength = 8;
	para.PINOutput.pEncryptedBlock = srTransRec.szPINBlock;
	para.Control.Timeout = 0;
	para.Control.AsteriskPositionX = 8;
	para.Control.AsteriskPositionY = 7;
	para.Control.NULLPIN = FALSE;
	para.Control.piTestCancel = NULL;
	
	ret = CTOS_KMS2PINGet(&para);
	if(ret != d_OK)
	{
		sprintf(str, "%s=%04X", strDCT.szDisplayProcessing, ret);
		CTOS_LCDTPrintXY(1, 8, str);
		CTOS_KBDGet(&key);
		return ret;
	}
	return d_OK;
}
*/

//test-remove
int CTOSS_Load_3DES_TMK_PlaintextNewPAN(char *szKeyData) {
	USHORT KeySet;
	USHORT KeyIndex;
	CTOS_KMS2KEYWRITE_PARA para;
	USHORT ret;
	BYTE str17;
	BYTE key;
	char str[10];

	#ifdef ANDROID
		usCTOSS_BackToProgress(" ");
	#endif
	CTOS_LCDTClearDisplay();
	CTOS_KMS2Init();
	CTOS_LCDTPrintXY(1, 1, "Write 3DES PT");

	KeySet = 0xC100;
	KeyIndex = 0x0001;

	vdDebug_LogPrintf("saturn **CTOSS_Load_3DES_TMK_Plaintext*,strDCT.usKeySet[%x]strDCT.usKeyIndex[%x]*",KeySet,KeyIndex);

	//sprintf(str, "saturn KeySet = %04X", KeySet);
	//CTOS_LCDTPrintXY(1, 3, str);
	//sprintf(str, "saturn KeyIndex = %04X", KeyIndex);
	//CTOS_LCDTPrintXY(1, 4, str);

	memset(&para, 0x00, sizeof(CTOS_KMS2KEYWRITE_PARA));
	para.Version = 0x01;
	para.Info.KeySet = KeySet;
	para.Info.KeyIndex = KeyIndex;
	para.Info.KeyType = KMS2_KEYTYPE_3DES;
	para.Info.KeyVersion = 0x01;
	#ifndef ANDROID
	para.Info.KeyAttribute = KMS2_KEYATTRIBUTE_PIN | KMS2_KEYATTRIBUTE_ENCRYPT | KMS2_KEYATTRIBUTE_MAC | KMS2_KEYATTRIBUTE_KPK;
	#else
	para.Info.KeyAttribute = SBKMS2_KEYATTRIBUTE_MASTERKEY;
	#endif
	para.Protection.Mode = KMS2_KEYPROTECTIONMODE_PLAINTEXT;
	para.Value.pKeyData = szKeyData;
	para.Value.KeyLength = 16;

	vdDebug_LogPrintf("saturn before keywrite");

	ret = CTOS_KMS2KeyWrite(&para);

	vdDebug_LogPrintf("saturn**TMK CTOS_KMS2KeyWrite*ret[%d]*", ret);
	if(ret != d_OK) {
		vdDebug_LogPrintf("saturn ret = 0x%04X", ret);
		//CTOS_LCDTPrintXY(1, 8, str);
	return d_NO;
	}else
		return d_OK;

	vdDebug_LogPrintf("saturn write key success");
//__Leave:
//CTOS_KBDGet(&key);
}

USHORT usCheckSMACKeys(USHORT ushKeySet, USHORT ushKeyIndex){
	USHORT rtn;

	vdDebug_LogPrintf("inCheckKeys SET[%04x] IDX[%04x]", ushKeySet, ushKeyIndex);

	/*just call once when terminal startup*/
	//CTOS_KMS2Init();

	rtn = CTOS_KMS2KeyCheck(ushKeySet, ushKeyIndex);

	vdDebug_LogPrintf("inCheckKeys CTOS_KMS2KeyCheck rtn[%d]", rtn);

	//if (rtn == d_OK)
              //vdDisplayErrorMsg(1, 7, "Key Check OK");
	//else
	//if (rtn != d_OK)
    	//vdDisplayErrorMsg(1, 7, "PLEASE INJECT KEY");

	return rtn;

}

