#ifndef __UI_API_H__
#define __UI_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <ctosapi.h>

USHORT CTOS_LCDTSetReverse(BOOL boReverse);
USHORT CTOS_LCDTPrintAligned(USHORT usY, UCHAR* pbBuf, BYTE bMode);

USHORT CTOS_LCDSelectModeEx(BYTE bMode,BOOL fClear);
USHORT CTOS_LCDGShowBMPPic(USHORT usX, USHORT usY, BYTE *baFilename);
USHORT CTOS_LCDTPrintXY(USHORT usX, USHORT usY, UCHAR* pbBuf);
USHORT CTOS_LCDTClearDisplay(void);
USHORT CTOS_LCDTTFSwichDisplayMode(USHORT usMode);
USHORT CTOS_LCDTTFSelect(BYTE *baFilename, BYTE bIndex);
USHORT CTOS_LCDTPrintAligned(USHORT usY, UCHAR* pbBuf, BYTE bMode);
USHORT CTOS_LCDBackGndColor(ULONG ulColor);
USHORT CTOS_LCDForeGndColor(ULONG ulColor);
USHORT CTOS_LCDFontSelectMode(BYTE bMode);
USHORT CTOS_LCDGShowPic(USHORT usX, USHORT usY, BYTE* baPat, ULONG ulPatLen, USHORT usXSize);
USHORT CTOS_LCDTClear2EOL(void);
USHORT CTOS_LCDTGotoXY(USHORT usX,USHORT usY);
USHORT CTOS_LCDTPutchXY (USHORT usX, USHORT usY, UCHAR bChar);
USHORT CTOS_LCDTPrint(UCHAR* sBuf);
USHORT CTOS_LCDTSetReverse(BOOL boReverse);
USHORT CTOS_LCDSetContrast(BYTE bValue);
USHORT CTOS_LCDTSelectFontSize(USHORT usFontSize);
//USHORT usCTOSS_Confirm(void);
USHORT usCTOSS_Confirm(BYTE *szDispString);
USHORT usCTOSS_ConfirmYesNo(BYTE *szMsg);
USHORT usCTOSS_ConfirmOK(BYTE *szDispString);
USHORT usCTOSS_BatchReviewUI(BYTE *szDispString);


USHORT usCTOSS_ConfirmInvAndAmt(BYTE *szDispString);
USHORT usCTOSS_DisplayUI(BYTE *szDispString);
USHORT usCTOSS_LCDDisplay(BYTE *szStringMsg);
USHORT usCARDENTRY(BYTE *szStringMsg);
USHORT onFailedCtlsFallback(BYTE *szStringMsg);
//USHORT usCARDENTRY2(BYTE *szStringMsg);
USHORT PrintReceiptUI(BYTE *szStringMsg);
USHORT PrintFirstReceiptUI(BYTE *szStringMsg);
USHORT EliteReceiptUI(BYTE *szStringMsg);
USHORT ConfirmCardDetails(BYTE *szConfirmCardDetails);
BYTE InputStringUI(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *szInput);
BYTE InputAlphaUI(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT usMaxLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *display);
USHORT UserCancel(BYTE *pbCancelDisplay);

BYTE InputExpiryDateUI(BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *szInput);
USHORT usEditDatabase(BYTE *szStringMsg);

USHORT usCTOSS_GetWifiInfo(void);
BOOL usGetConnectionStatus(int inCommType);
USHORT ConfirmDetails(BYTE *szConfirmCardDetails);
BYTE InputAmountWithMenu(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter);


USHORT usCTOSS_EditInfoListViewUI(BYTE *szDispString, BYTE *szOutputBuf);
USHORT usCTOSS_Confirm2(BYTE *szDispString);

USHORT usCTOSS_ConfirmOKCancel(BYTE *szDispString);

USHORT usCTOSS_GetMobileInfo(void);
USHORT usCTOSS_ConfirmTotal(BYTE *szDispString);
USHORT getAppPackageInfo(BYTE *szAppName, BYTE *szStringMsg);

USHORT usGetSerialNumber(BYTE baFactorySN[20]);

USHORT usCTOSS_DisplayApprovedUI(BYTE *szStringMsg);
USHORT usCTOSS_PromptMessageUI(BYTE *szStringMsg);
USHORT usCTOSS_MenuSelection(BYTE *szDispString);
USHORT usCTOSS_TransDetailListViewUI(BYTE *szDispString);
USHORT usCTOSS_SingleRecordDataListViewUI(BYTE *szDispString);
USHORT usCTOSS_ConfirmTipAdjust(BYTE *szDispString);
USHORT usCTOSS_DisplayTipAdjustApprovedUI(BYTE *szStringMsg);
USHORT usCTOSS_ConfirmDCC(BYTE *szDispString);
BYTE InputQWERTY(BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usMaxLen, USHORT usMinLen, USHORT usTimeOutMS, BYTE *display);
int inCheckBattery(void);
BYTE InputTip(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter);

USHORT usCTOSS_PrintAnimation(BOOL isFade);
USHORT usCTOSS_PrintAnimation2(BYTE *szDispString);
USHORT usCTOSS_PrintAnimation3(void);

#ifdef __cplusplus
}
#endif

#endif

