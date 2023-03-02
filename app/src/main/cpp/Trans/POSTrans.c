/*******************************************************************************

*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctosapi.h>
#include <EMVAPLib.h>
#include <EMVLib.h>
#include <emv_cl.h>
#include <vwdleapi.h>

#include <sys/stat.h>
#include <vegaapi.h>
#include "..\Includes\POSTypedef.h"
#include "..\FileModule\myFileFunc.h"

#include "..\Includes\msg.h"
#include "..\Includes\CTOSInput.h"
#include "..\ui\display.h"

#include "..\Debug\Debug.h"

#include "..\Includes\CTOSInput.h"

#include "..\comm\v5Comm.h"
#include "..\Accum\Accum.h"
#include "..\Database\DatabaseFunc.h"
#include "..\POWRFAIL\POSPOWRFAIL.h"

#include "..\Includes\POSMain.h"
#include "..\Includes\POSTrans.h"
#include "..\Includes\POSHost.h"
#include "..\Includes\POSSale.h"
#include "..\Comm\V5Comm.h"
#include "..\debug\debug.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\CardUtil.h"
#include "..\Debug\Debug.h"
#include "..\Database\DatabaseFunc.h"
#include "..\Includes\myEZLib.h"
#include "..\ApTrans\MultiShareEMV.h"
#include "..\Includes\MultiApLib.h"
#include "..\Includes\V5IsoFunc.h"
#include "..\Ctls\POSCtls.h"
#include "..\Ctls\PosWave.h"
#include "..\Aptrans\MultiAptrans.h"
#include "..\Includes\Possetting.h"
#include "..\ApTrans\MultiShareCOM.h"

#include "../UIapi.h"
#include "../Includes/POSSettlement.h"
#include "../Print/Print.h"
#include "../Includes/showbmp.h"
#include "../TMS/TMS.h"
#include "../Includes/CfgExpress.h"
#include "../Includes/POSTypedef.h"
#include "../Ui/Display.h"
#include "../Database/DatabaseFunc.h"
#include "../Filemodule/myFileFunc.h"
#include "../Includes/POSSetting.h"
#include "../Includes/POSTrans.h"
#include "../Debug/debug.h"
//#include "..\Ctls\POSMifare.h"
//#include "..\Includes\DESFire.h"


extern USHORT usPreconnectStatus;
int inGblCtlsErr = 0;

// patrck fix code 20141208
extern EMVCL_RC_DATA_ANALYZE stRCDataAnalyze;\
extern BOOL fAmountEntered;

extern BOOL fManualSettle;
extern BOOL fAUTOManualSettle;

int inFallbackToMSR = FAIL;
static char szBaseAmount[6+1];

BOOL fECRTxnFlg = 0;

extern int ing_KeyPressed;

//issue:218
extern BOOL fEntryCardfromIDLE;
extern BOOL fIdleInsert;
//issue218

extern BOOL fBINVer;
extern BOOL fGPRSConnectOK;

extern int isPredial;
extern int inReversalType;
extern BOOL fRouteToSpecificHost;
extern char gblszAmt[20+1];
extern BOOL fnGlobalOrigHostEnable;
//extern int inHostOrigNumber;

BOOL fAMEXHostEnable;
BOOL fDinersHostEnable;

extern BOOL fUSDSelected;

BOOL fBancnetAIDEnable;
BOOL fSkipBINRoutingForDebit;
BOOL fSkipBINRoutingForCUP;
BOOL fMagStripeDebit;

BOOL fBinRouteDCC;

BOOL fDualBrandedCard;

char szGlobalAPName[50];

VS_BOOL fPreConnectEx;

//extern BOOL fUSDSelected;

extern BOOL fInstApp; //BDO: Parameterized manual key entry for installment --jzg

extern char szFuncTitleName [21 + 1]; //aaronnino for BDOCLG ver 9.0 fix on issue #0093 Have a function title for function keys shorcut 3 of 6

extern BOOL fIdleSwipe;//aaronnino for BDOCLG ver 9.0 fix on issue #00059 Card entry is recognized even on non Card Entry Prompt or non Idle Screen display 7 of 8

//Display Issuer logo: Clear the display first, then re-display trans title -- jzg
extern void displayAppbmpDataEx(unsigned int x,unsigned int y, const char *path);

/* BDO: For ECR, variable -- sidumili */
BOOL fECRBuildSendOK = FALSE; /* For ECR -- sidumili */
/* BDO: For ECR, variable -- sidumili */

BOOL fTimeOutFlag = FALSE; /*For ECR timeout flag --sidumili*/

BOOL fApplNotAvailable;
extern BOOL fBINVerPreConnectEx;
extern BOOL fRePrintFlag;
BOOL fNoCTLSSupportforBinRoute = FALSE;
BOOL fMagStripeCUP = FALSE;
extern BOOL fReEnterOfflinePIN;
extern BOOL fNoEMVProcess;
extern BYTE byPackTypeBeforeDCCLog;
extern BOOL fAMEXHostEnable;
extern BOOL fBuildandSendProcess;
extern BOOL fBDOOptOutHostEnabled;

//ADC
int inADLStart1;
int inADLEnd1;
int inADLStart2;
int inADLEnd2;
int inADLStart3;
int inADLEnd3;
int inADLTimeRangeUsed;
int inADLLimit;
int inMaxTime;
//ADC
extern BOOL fSettleTrans;

void vdSetECRTransactionFlg(BOOL flg)
{
	put_env_int("ECRTRANS",flg);
    fECRTxnFlg = flg;
}

BOOL fGetECRTransactionFlg(void)
{
	fECRTxnFlg = get_env_int("ECRTRANS");
    return fECRTxnFlg;
}

extern int isdigit ( int c );

#if 0
USHORT shCTOS_GetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS)
{

    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[128];
    USHORT  usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof(bInputStrData));

    if(usTimeOutMS > 0)
        CTOS_TimeOutSet (TIMER_ID_1 , usTimeOutMS);

    vdDebug_LogPrintf("start [%d] data[%s]", strlen(baBuf), baBuf);
    if(strlen(baBuf) > 0 )
    {
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bDisplayStr, 0x20, usMaxLen*2);
        usInputStrLen = strlen(baBuf);
        strcpy(bInputStrData, baBuf);
        if(0x01 == usLeftRight)
        {
            strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
        }
        else
        {
            memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
            CTOS_LCDTPrintXY(1, usY, bDisplayStr);
        }
    }

    while(1)
    {
//        vduiLightOn(); // patrick remark for flash light always
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
        	fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        }

        CTOS_KBDHit(&bKey);

        switch(bKey)
        {
        case d_KBD_DOT:
            break;
        case d_KBD_CLEAR:
            if (usInputStrLen)
            {
                usInputStrLen--;
                bInputStrData[usInputStrLen] = 0x00;

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, usMaxLen*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                    CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                }
            }
            break;
        case d_KBD_CANCEL:
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            if (usInputStrLen < usMaxLen )
            {
                bInputStrData[usInputStrLen++] = bKey;

                memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                memset(bDisplayStr, 0x20, usMaxLen*2);
                if(0x01 == usLeftRight)
                {
                    strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                    CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                }
                else
                {
                    memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                    CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                }
            }
            break;
        case d_KBD_ENTER:
            if(usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen)
            {
                *usStrLen = usInputStrLen;
                strcpy(baBuf, bInputStrData);
                return *usStrLen;
            }
            else if(usByPassAllow && 0 == usInputStrLen)
            {
                *usStrLen = usInputStrLen;
                baBuf[0] = 0x00;
                return *usStrLen;
            }
            break;
        default :
            break;
        }
    }

    return 0;
}
#endif

// code cleanup
#if 0
USHORT shCTOS_GetExpDate(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usTimeOutMS)
{

    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[20];
    BYTE    bInputFormatStr[20];
    USHORT  usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof(bInputStrData));

    if(usTimeOutMS > 0)
        CTOS_TimeOutSet (TIMER_ID_1 , usTimeOutMS);

    vduiLightOn();
    while(1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            return d_KBD_CANCEL ;
        }

        CTOS_KBDHit(&bKey);

        switch(bKey)
        {
            case d_KBD_DOT:
                break;
            case d_KBD_CLEAR:
                if (usInputStrLen)
                {
                    usInputStrLen--;
                    bInputStrData[usInputStrLen] = 0x00;

                    memset(bInputFormatStr, 0x00, sizeof(bInputFormatStr));
                    if(usInputStrLen >= 2)
                    {
                        memcpy(bInputFormatStr, bInputStrData, 2);
                        strcat(bInputFormatStr, "/");
                        if(usInputStrLen > 2)
                            strcat(bInputFormatStr, &bInputStrData[2]);
                    }
                    else
                    {
                        strcpy(bInputFormatStr, bInputStrData);
                    }

                    memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                    memset(bDisplayStr, 0x20, (usMaxLen+1)*2);

                    if(0x01 == usLeftRight)
                    {
                        strcpy(&bDisplayStr[(usMaxLen+1-strlen(bInputFormatStr))*2], bInputFormatStr);
                        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                    }
                    else
                    {
                        memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                        CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                    }
                }
                break;
            case d_KBD_CANCEL:
                *usStrLen = 0;
                baBuf[0] = 0x00;
                return d_KBD_CANCEL ;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                if (usInputStrLen < usMaxLen )
                {
                    bInputStrData[usInputStrLen++] = bKey;

                    memset(bInputFormatStr, 0x00, sizeof(bInputFormatStr));
                    if(usInputStrLen >= 2)
                    {
                        memcpy(bInputFormatStr, bInputStrData, 2);
                        strcat(bInputFormatStr, "/");
                        if(usInputStrLen > 2)
                            strcat(bInputFormatStr, &bInputStrData[2]);
                    }
                    else
                    {
                        strcpy(bInputFormatStr, bInputStrData);
                    }

                    memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                    memset(bDisplayStr, 0x20, (usMaxLen+1)*2);
                    if(0x01 == usLeftRight)
                    {
                        strcpy(&bDisplayStr[(usMaxLen+1-strlen(bInputFormatStr))*2], bInputFormatStr);
                        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - (usMaxLen+1)*2, usY, bDisplayStr);
                    }
                    else
                    {
                        memcpy(bDisplayStr, bInputFormatStr, strlen(bInputFormatStr));
                        CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                    }
                }
                break;
            case d_KBD_ENTER:
                if(usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen)
                {
                    *usStrLen = usInputStrLen;
                    strcpy(baBuf, bInputStrData);
                    return *usStrLen;
                }
                break;
            default :
                break;
        }
    }

    return 0;
}
#endif

// code cleanup
#if 0
USHORT getExpDate( OUT BYTE *baBuf)
{
    BYTE    szMonth[3];
    USHORT  usRet;
    USHORT  usLens;
    USHORT  usMinLen = 4;
    USHORT  usMaxLen = 4;
    USHORT usInputLine = 8;

	
	BYTE szTitle[16];
	BYTE szDisplay[512];
	
	USHORT usTk1Len=TRACK_I_BYTES, usTk2Len=TRACK_II_BYTES, usTk3Len=TRACK_III_BYTES;
	BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
	
	memset(szTitle, 0x00, sizeof(szTitle));
	//szGetTransTitle(srTransRec.byTransType, szTitle);
	szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);
	
	vdDebug_LogPrintf("srTransRec.szPAN");
	vdDebug_LogPrintf(srTransRec.szPAN);
	vdDebug_LogPrintf("szTitle");
	vdDebug_LogPrintf(szTitle);
	
	memset(szDisplay, 0x00, sizeof(szDisplay));
	
	strcpy(szDisplay, "4");
	strcat(szDisplay, "|");
	strcat(szDisplay, "4");
	strcat(szDisplay, "|");
	strcat(szDisplay, szTitle);
	strcat(szDisplay, "|");
	strcat(szDisplay, "Please enter card number");
	strcat(szDisplay, "|");
	strcat(szDisplay, srTransRec.szPAN);
	strcat(szDisplay, "|");
	strcat(szDisplay, "Please enter expiry date");

    while(1)
    {
        //usRet = shCTOS_GetExpDate(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, d_INPUT_TIMEOUT);
        //usRet = shCTOS_GetExpDate(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

		//TINE:  android
		usRet = InputExpiryDateUI(baBuf, &usLens, usMinLen, d_INPUT_TIMEOUT, szDisplay);
		
		if (usRet == d_KBD_CANCEL )
		{
			vdDebug_LogPrintf("*************************");
  	   		CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);	//clear buffer on idle swipe on term reg.

			return (d_EDM_USER_CANCEL);
		}

		/*   TINE:  Input validation handled by android UI
        memset(szMonth, 0x00, sizeof(szMonth));
        memcpy(szMonth, baBuf, 2);
        if(atol(szMonth) > 12 || 0 == atol(szMonth))
        {
            baBuf[0] = 0x00;
            clearLine(8);
            vdDisplayErrorMsg(1, 8, "INVALID FORMAT");
            clearLine(8);
            continue;
        }
        else
        {
            return (d_OK);
        }
        */
        return (d_OK);
    }
}
#endif

USHORT getCardNO(OUT BYTE *baBuf)
{
    USHORT usRet;
    USHORT usLens;
    USHORT usMinLen = 12;
    USHORT usMaxLen = 19;
    USHORT usInputLine = 8;

	//TINE:  android
	BYTE szTitle[16];
    BYTE szDisplay[100];

	memset(szTitle, 0x00, sizeof(szTitle));
    //szGetTransTitle(srTransRec.byTransType, szTitle);
    szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);

	memset(szDisplay, 0x00, sizeof(szDisplay));

    strcpy(szDisplay, "12");		//min length
    strcat(szDisplay, "|");
    strcat(szDisplay, "20");		//max length
    strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "Please enter card number");

    while(1)
    {
        //usRet = shCTOS_GetNum(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 0, d_INPUT_TIMEOUT);
        //usRet = shCTOS_GetNum(usInputLine, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 0, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
		//TINE:  android
		usRet = InputStringUI(0x01, 0x02, baBuf, &usLens, usMinLen, d_INPUT_TIMEOUT, szDisplay);
        vdDebug_LogPrintf("usRet[%d]atoi(baBuf)=[%d]usLens[%d]", usRet, atoi(baBuf), usLens);
		
		if (usRet == d_KBD_CANCEL )
            return (d_EDM_USER_CANCEL);

		if (usRet == 0xFF)
			return d_NO;
		
        //if (usRet >= usMinLen && usRet <= usMaxLen)
        if (usRet == 'A')
        {
			//TINE:  save PAN to srTransRec
            if (usLens >= usMinLen && usLens <= usMaxLen) 
			{
				strcpy(srTransRec.szPAN, baBuf);			
				return (d_OK);
            }
                
        }

        baBuf[0] = 0x00;
    }
}


unsigned char WaitKey(short Sec)
{
    unsigned char c;
    long mlsec;

    mlsec=Sec*100;
    BOOL isKey;
    CTOS_TimeOutSet(TIMER_ID_3,mlsec);
    while(1)//loop for time out
    {
        CTOS_KBDInKey(&isKey);
        if (isKey){ //If isKey is TRUE, represent key be pressed //

            vduiLightOn();
            CTOS_KBDGet(&c);
            return c;
        }
        else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {
            fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            return d_KBD_CANCEL;
        }
    }
}


void vduiApiAmount(unsigned char *ou, unsigned char *ascamt, unsigned char len)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned int    ii, jj, tt;
    unsigned char   ch;
    unsigned char   aa;
    unsigned char   buf[3];
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    jj = 0;
    tt = 0;
    ii = 0;


    ou[jj++] = strCST.szCurSymbol[0];
    ou[jj++] = strCST.szCurSymbol[1];
    ou[jj++] = strCST.szCurSymbol[2];

    for(ii = 0; ii < len; ii++)
    {
        ch = ascamt[ii];
        if((tt == 0) && (ch == 'C'))
        {
            tt = 1;
        }
        else if((tt == 0) && (ch == 'D'))
        {
            tt = 1;
            ou[jj++] = '-';
        }
        else if(ch<0x30 && ch>0x39 )
        {
            break;
        }
    }


    len = ii;
    aa = 0;
    for(ii = tt; (ii + 3) < len; ii++)
    {
        ch = ascamt[ii];
        if((ch == '0') && (aa == 0))
        {
            continue;
        }

        if(ch>0x29 && ch<0x40 )//if(isdigit(ch) /* && (ch !='0') */ )
        {
            aa = 1;
            ou[jj++] = ch;
        }
    }

    tt = ii;
    len = len - ii;
    buf[0] = '0', buf[1] = '0', buf[2] = '0';
    for(ii = 0; ii < len; ii++)
    {
        buf[3 - len + ii] = ascamt[tt++];
    }

    ou[jj++] = buf[0];
    ou[jj++] = '.';
    ou[jj++] = buf[1];
    ou[jj++] = buf[2];
    ou[jj++] = '\0';
}

void vduiApiPoint(unsigned char *ou, unsigned char *ascamt, unsigned char len)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~*/
    unsigned int    ii, jj, tt;
    unsigned char   ch;
    unsigned char   aa;
    unsigned char   buf[3];
    /*~~~~~~~~~~~~~~~~~~~~~~~*/

    jj = 0;
    tt = 0;

    ou[jj++] = 'P';
    ou[jj++] = 'T';
    ou[jj++] = 'S';
    ou[jj++] = ' ';
    for(ii = 0; ii < len; ii++)
    {
        ch = ascamt[ii];
        if((tt == 0) && (ch == 'C'))
        {
            tt = 1;
        }
        else if((tt == 0) && (ch == 'D'))
        {
            tt = 1;
            ou[jj++] = '-';
        }
        else if(ch<0x30 && ch>0x39 )
        {
            break;
        }
    }


    len = ii;
    aa = 0;
    for(ii = tt; (ii + 3) < len; ii++)
    {
        ch = ascamt[ii];
        if((ch == '0') && (aa == 0))
        {
            continue;
        }

        if(ch>0x29 && ch<0x40 )
        {
            aa = 1;
            ou[jj++] = ch;
        }
    }

    tt = ii;
    len = len - ii;
    buf[0] = '0', buf[1] = '0', buf[2] = '0';
    for(ii = 0; ii < len; ii++)
    {
        buf[3 - len + ii] = ascamt[tt++];
    }

    ou[jj++] = buf[0];
    ou[jj++] = '.';
    ou[jj++] = buf[1];
    ou[jj++] = buf[2];
    ou[jj++] = '\0';
}

//mode  1=amount , 2=string, 3=IP  4=password, 5=Point
unsigned char struiApiGetStringSub
        (
                unsigned char   *strDisplay,
                short   x,
                short   y,
                unsigned char   *ou,
                unsigned char   mode,
                short   minlen,
                short   maxlen
        )
{

    unsigned char srDestIP[MAX_CHAR_PER_LINE+1];
    unsigned char amtdis[MAX_CHAR_PER_LINE+1];
    unsigned char c;
    int n;
    int i;

    memset(srDestIP,0x00,sizeof(srDestIP));
    n= 0;

    vduiClearBelow(y);

    if(mode == MODE_AMOUNT)
    {
        vduiClearBelow(8);
        vduiApiAmount(amtdis, srDestIP, n);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);

    }
    else if(mode == MODE_POINT)
    {
        vduiClearBelow(8);
        vduiApiPoint(amtdis, srDestIP, n);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);

    }

    while(1)
    {

        CTOS_LCDTPrintXY(x,y,strDisplay);
        c=WaitKey(30);//CTOS_KBDGet(&c);
        vduiLightOn();

        if (c == d_KBD_ENTER)
        {
            if((n>=minlen) && (n<=maxlen))
            {
                ou[n]=0;
                memcpy(ou,srDestIP,n+1);
                if(mode==MODE_FOODITEM && atoi(ou)==0)
                {
                    return d_KBD_CANCEL;
                }

                if(mode==MODE_AMOUNT && atoi(ou)==0)
                {
                    i = atoi(ou);
                    memset(srDestIP,0x00,sizeof(srDestIP));
                    n=0;
                    vduiWarningSound();
                }
                else
                    return d_KBD_ENTER;
            }

        }
        else if((c == d_KBD_CANCEL) && (n==0))
        {
            memset(srDestIP,0x00,sizeof(srDestIP));
            return d_KBD_CANCEL;
        }

        else
        {
            if (c==d_KBD_CLEAR)
            {
                if(n>0)
                {
                    n--;
                    srDestIP[n]='\0';
                }
            }
            else if((c == d_KBD_CANCEL))
            {
                memset(srDestIP,0x00,sizeof(srDestIP));
                n=0;
            }
            else if (((c == d_KBD_DOT) & (mode==MODE_IPADDRESS)) || ((c == d_KBD_DOWN) & (mode==MODE_IPADDRESS)))
            {
                srDestIP[n]='.';
                n++;
            }
            else if (c == d_KBD_DOT || c == d_KBD_F3  || c == d_KBD_F4 || c == d_KBD_00)
            {
                ;
            }
            else if(c == d_KBD_UP || c == d_KBD_DOWN)
            {
                return c;
            }
            else if(n<maxlen)
            {
                srDestIP[n]=c;
                n++;
            }
            else
            {

            }

            if(mode == MODE_AMOUNT)
            {
                vduiClearBelow(8);
                vduiApiAmount(amtdis, srDestIP, n);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);

            }
            else if(mode == MODE_POINT)
            {
                vduiClearBelow(8);
                vduiApiPoint(amtdis, srDestIP, n);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(amtdis)*2,8,amtdis);

            }
            else if(mode == MODE_PASSWORD)
            {
                for(i=0;i<n;i++)
                    amtdis[i]='*';
                amtdis[n]=0;
                vduiClearBelow(8);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-n*2,8,amtdis);
            }
            else
            {
                vduiClearBelow(8);
                CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-n*2,8,srDestIP);
            }
        }

    }

}

void vdCTOSS_GetMemoryStatus(char *Funname)
{
    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];

    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];


    memset(szUsedDiskSize,0,sizeof(szUsedDiskSize));
    memset(szTotalDiskSize,0,sizeof(szTotalDiskSize));
    memset(szUsedRamSize,0,sizeof(szUsedRamSize));
    memset(szTotalRamSize,0,sizeof(szTotalRamSize));
    memset(szAvailableRamSize,0,sizeof(szAvailableRamSize));
    memset(szAvailableDiskSize,0,sizeof(szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;

    sprintf(szTotalDiskSize,"%s:%ld","Total disk",ulTotalDiskSize);
    sprintf(szUsedDiskSize,"%s:%ld","Used   disk",ulUsedDiskSize);
    sprintf(szAvailableDiskSize,"%s:%ld","Avail disk",ulAvailDiskSize);

    sprintf(szTotalRamSize,"%s:%ld","Total RAM",ulTotalRamSize);
    sprintf(szUsedRamSize,"%s:%ld","Used   RAM",ulUsedRamSize);
    sprintf(szAvailableRamSize,"%s:%ld","Avail RAM",ulAvailableRamSize);
    vdDebug_LogPrintf("[%s][%ld],[%ld][%ld][%ld]",Funname,ulUsedDiskSize,ulTotalRamSize,ulUsedRamSize,ulAvailableRamSize);

    CTOS_LCDTClearDisplay();

    CTOS_LCDTPrintXY(1, 1, szTotalDiskSize);
    CTOS_LCDTPrintXY(1, 2, szUsedDiskSize);
    CTOS_LCDTPrintXY(1, 3, szAvailableDiskSize);

    CTOS_LCDTPrintXY(1, 5, szTotalRamSize);
    CTOS_LCDTPrintXY(1, 6, szUsedRamSize);
    CTOS_LCDTPrintXY(1, 7, szAvailableRamSize);
    WaitKey(60);

}

int inCTOSS_CheckMemoryStatus()
{
#define SAFE_LIMIT_SIZE 5000
#define SAFE_FLASH_SIZE 20000


    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];

    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    memset(szUsedDiskSize,0,sizeof(szUsedDiskSize));
    memset(szTotalDiskSize,0,sizeof(szTotalDiskSize));
    memset(szUsedRamSize,0,sizeof(szUsedRamSize));
    memset(szTotalRamSize,0,sizeof(szTotalRamSize));
    memset(szAvailableRamSize,0,sizeof(szAvailableRamSize));
    memset(szAvailableDiskSize,0,sizeof(szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    vdDebug_LogPrintf("[%ld],[%ld][%ld][%ld]",ulUsedDiskSize,ulTotalDiskSize,ulUsedRamSize,ulTotalRamSize);
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;

    sprintf(szTotalDiskSize,"%s:%ld","Total disk",ulTotalDiskSize);
    sprintf(szUsedDiskSize,"%s:%ld","Used   disk",ulUsedDiskSize);
    sprintf(szAvailableDiskSize,"%s:%ld","Avail disk",ulAvailDiskSize);

    sprintf(szTotalRamSize,"%s:%ld","Total RAM",ulTotalRamSize);
    sprintf(szUsedRamSize,"%s:%ld","Used   RAM",ulUsedRamSize);
    sprintf(szAvailableRamSize,"%s:%ld","Avail RAM",ulAvailableRamSize);
    vdDebug_LogPrintf("ulAvailDiskSize[%ld],ulAvailableRamSize[%ld]",ulAvailDiskSize,ulAvailableRamSize);

    if (ulAvailDiskSize < SAFE_FLASH_SIZE)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1, 7, "Settle  soon");
        vdDisplayErrorMsg(1, 8,  "Insufficient Memory");
        return FAIL;
    }

    if (ulAvailableRamSize < SAFE_LIMIT_SIZE)
    {
        CTOS_LCDTClearDisplay();
        vdSetErrorMessage("Insufficient RAM");
        return FAIL;
    }

    return d_OK;

}

void vdCTOS_SyncHostDateTime()
{
    CTOS_RTC SetRTC;
    char szDate[4+1];
    char szTime[6+1];
    char szBuf[2+1];

    if(srTransRec.byOffline == CN_TRUE)
        return;
    CTOS_RTCGet(&SetRTC);
    vdDebug_LogPrintf("sys year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]",SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay,SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);

    vdDebug_LogPrintf("year[%02X],time[%02x:%02x:%02x]date[%02x][%02x]",SetRTC.bYear,srTransRec.szTime[0],srTransRec.szTime[1],srTransRec.szTime[2],srTransRec.szDate[0],srTransRec.szDate[1]);

    memset(szDate,0,sizeof(szDate));
    memset(szTime,0,sizeof(szTime));

    wub_hex_2_str(srTransRec.szDate, szDate, 2);
    wub_hex_2_str(srTransRec.szTime, szTime, 3);
    if((strlen(szDate)<=0) ||(strlen(szTime)<=0))
        return;

    vdDebug_LogPrintf("srTransRec.fSetDate: %d", srTransRec.fSetDate);
    vdDebug_LogPrintf("srTransRec.fSetTime: %d", srTransRec.fSetTime);

    if(srTransRec.fSetDate == TRUE)
    {
        sprintf(szBuf,"%02x",(unsigned int)atol(szDate)%100);
        wub_str_2_hex(szBuf, &(SetRTC.bDay), 2);

        sprintf(szBuf,"%02x",(unsigned int)atol(szDate)/100);
        wub_str_2_hex(szBuf, &(SetRTC.bMonth), 2);
    }

    if(srTransRec.fSetTime == TRUE)
    {
        sprintf(szBuf,"%02x",(unsigned int)atol(szTime)/10000);
        wub_str_2_hex(szBuf, &(SetRTC.bHour), 2);

        sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%10000/100);
        wub_str_2_hex(szBuf, &(SetRTC.bMinute), 2);

        sprintf(szBuf,"%02x",(unsigned int)atol(szTime)%100);
        wub_str_2_hex(szBuf, &(SetRTC.bSecond), 2);
    }

    //Host date & time synchronization fix -- jzg
    CTOS_RTCSet(&SetRTC);

    vdDebug_LogPrintf("set year[%02x],Date[%02x][%02x]time[%02x][%02x][%02x]",SetRTC.bYear,SetRTC.bMonth,SetRTC.bDay,SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);

    return;
}

int file_exist (char *filename)
{
    struct stat buffer;
    return (stat (filename, &buffer) == 0);
}



void vdCTOS_TxnsBeginInit(void)
{
    BYTE bEntryMode = 0;
    int inRet = d_NO;

	BYTE szPAN[PAN_SIZE + 1];
    USHORT usTrack1Len = 0;
    USHORT usTrack2Len = 0;
    USHORT usTrack3Len = 0;
    BYTE szTrack1Data[TRACK_I_BYTES + 1];
    BYTE szTrack2Data[TRACK_II_BYTES + 1];
    BYTE szTrack3Data[TRACK_III_BYTES + 1];

    BYTE byPanLen;
    BYTE szCardholderName[CARD_HOLD_NAME_DIGITS + 1];
    BYTE szExpireDate[EXPIRY_DATE_BCD_SIZE + 1];
    BYTE szDataTmp[5] = {0};
    BYTE szServiceCode[3+1];
    BYTE byPINEntryCapability;

    //inTCTRead(1);
    //vdSetIdleEvent(1);
    //inCTOSS_GetRAMMemorySize("TXN INIT");

    /*albert - do not prompt swipe/insert/etc... card*/
	vdDebug_LogPrintf("--vdCTOS_TxnsBeginInit-- Retain MSR Card Details");
    if(srTransRec.byEntryMode == CARD_ENTRY_MSR || srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
		bEntryMode = srTransRec.byEntryMode;
		vdDebug_LogPrintf("bEntryMode = [%d]", bEntryMode);
        if (srTransRec.byEntryMode == CARD_ENTRY_MSR) {

            usTrack1Len = srTransRec.usTrack1Len;
            usTrack2Len = srTransRec.usTrack2Len;
            usTrack3Len = srTransRec.usTrack3Len;

            memset(szTrack1Data, 0, sizeof(szTrack1Data));
            memset(szTrack2Data, 0, sizeof(szTrack2Data));
            memset(szTrack3Data, 0, sizeof(szTrack3Data));
            memset(szPAN, 0, sizeof(szPAN));

            memcpy(szTrack1Data, srTransRec.szTrack1Data, srTransRec.usTrack1Len);
            memcpy(szTrack2Data, srTransRec.szTrack2Data, srTransRec.usTrack2Len);
            memcpy(szTrack3Data, srTransRec.szTrack3Data, srTransRec.usTrack3Len);
            strcpy(szPAN, srTransRec.szPAN);

            vdDebug_LogPrintf("TrackData and PAN saved");

            //AnalysisTrack1
            memset(szCardholderName, 0, sizeof(szCardholderName));
            memset(szExpireDate, 0, sizeof(szExpireDate));

            byPanLen = srTransRec.byPanLen;
            vdDebug_LogPrintf("byPanLen Saved...");

            strcpy(szCardholderName, srTransRec.szCardholderName);
            vdDebug_LogPrintf("szCardholderName Saved...");
            vdDebug_LogPrintf("szCardholderName:  %s", szCardholderName);


            szDataTmp[0] = srTransRec.szExpireDate[0];
            szDataTmp[1] = srTransRec.szExpireDate[1];
            vdDebug_LogPrintf("szExpireDate saved");

            memcpy(szServiceCode, srTransRec.szServiceCode, 3);
            vdDebug_LogPrintf("szServiceCode saved");

            byPINEntryCapability = srTransRec.byPINEntryCapability;
            vdDebug_LogPrintf("byPINEntryCapability saved");
        }
    }
        

    if(d_OK == inCTOS_ValidFirstIdleKey())
        bEntryMode = srTransRec.byEntryMode;

    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));

    //gcitra
    //inCLearTablesStructure();
    //gcitra

    //1128
    inFallbackToMSR = FAIL;
    fSkipBINRoutingForDebit = FALSE;
    fDualBrandedCard = FALSE;
    fSkipBINRoutingForCUP = FALSE;
	
    fSettleTrans = VS_FALSE;
	fRePrintFlag = FALSE;

	strGBLVar.fGBLvDCC = FALSE;

    if(0 != bEntryMode)
    {
		srTransRec.byEntryMode = bEntryMode;
		if (srTransRec.byEntryMode == CARD_ENTRY_MSR) {

            srTransRec.usTrack1Len = usTrack1Len;
            srTransRec.usTrack2Len = usTrack2Len;
            srTransRec.usTrack3Len = usTrack3Len;
            memcpy(srTransRec.szTrack1Data, szTrack1Data, srTransRec.usTrack1Len);
            memcpy(srTransRec.szTrack2Data, szTrack2Data, srTransRec.usTrack2Len);
            memcpy(srTransRec.szTrack3Data, szTrack3Data, srTransRec.usTrack3Len);
            strcpy(srTransRec.szPAN, szPAN);
            srTransRec.byPanLen = byPanLen;
            strcpy(srTransRec.szCardholderName, szCardholderName);
            srTransRec.szExpireDate[0] = szDataTmp[0];
            srTransRec.szExpireDate[1] = szDataTmp[1];
            memcpy(srTransRec.szServiceCode, szServiceCode, 3);
            srTransRec.byPINEntryCapability = byPINEntryCapability;

        }
    }
        

    vdSetErrorMessage("");
    vdCTOSS_SetWaveTransType(0);
    vdSetInit_Connect(0);

    vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check if main app");
//add for ECR only MianAP get the ECR command, if Sub AP will cannot error
    //if (inMultiAP_CheckMainAPStatus() == d_OK)
    if (inMultiAP_CheckSubAPStatus() != d_OK)//only 1 APP or main APP
    {
        vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check ECR on?[%d]", strTCT.fECR);
        if ((strTCT.fECR) &&  (fECRTxnFlg == 1))//glad0129 - only check if ECR triggered
        {
            vdDebug_LogPrintf("vdCTOS_TxnsBeginInit check Database exist");
            if (file_exist (DB_MULTIAP))
            {
                vdDebug_LogPrintf("vdCTOS_TxnsBeginInit Read ECR Data");
                inRet = inCTOS_MultiAPGetData();
                vdDebug_LogPrintf("vdCTOS_TxnsBeginInit Read ECR Data ret[%d]", inRet);

                //set as ECRtriggered trans
                srTransRec.fECRTriggerTran = TRUE;
                srTransRec.fECRTrxFlg = TRUE; /*Flag for multiAP database and flag for multi app -- sidumili*/
                put_env_int("CREDITBUSY",1);

                if(d_OK != inRet)
                    return ;
            }
        }

    }
}

void vdCTOS_TransEndReset(void)
{
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES_50], szTk3Buf[TRACK_III_BYTES];
    char szErrMsg[30+1], szErrMsg1[30+1];

    vdDebug_LogPrintf("vdCTOS_TransEndReset ECR?[%d]", fGetECRTransactionFlg());
    //CTOS_LCDTClearDisplay();
    // patrick fix code 20141209
    //clearLine(1);
    //vduiClearBelow(2);
    vdSetATPBinRouteFlag(0);

    memset(&stRCDataAnalyze,0x00,sizeof(EMVCL_RC_DATA_ANALYZE));

    memset(szErrMsg,0x00,sizeof(szErrMsg));
    if (inGetErrorMessage(szErrMsg) > 0)
    {
        CTOS_LCDTClearDisplay();
        vdDisplayErrorMsg(1, 8, szErrMsg);
    }

#if 0
    if(CARD_ENTRY_ICC == srTransRec.byEntryMode || CARD_ENTRY_WAVE == srTransRec.byEntryMode)
    {
		CTOS_LCDTClearDisplay();
        vdRemoveCard();
    }
    else
        CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
#endif

    //CTOS_LCDTClearDisplay();
    vdRemoveCard();

    //if(!fGetECRTransactionFlg())
    //{
    memset( &srTransRec, 0x00, sizeof(TRANS_DATA_TABLE));
    //}

    CTOS_KBDBufFlush();

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
    vdSetErrorMessage("");
    vdCTOSS_SetWaveTransType(0);

    //gcitra
    inCLearTablesStructure();
    //1128
    inFallbackToMSR = FAIL;
    //gcitra

    //inMultiAP_Database_BatchDelete();
    //if(d_OK != bret)
    //{
    //	vdSetErrorMessage("MultiAP BatchDelete ERR");
    //}

    fBINVer = FALSE;

    //06112015
    fCommAlreadyOPen = FALSE;
    isPredial=0;
    //06112015

//enhance ecr
    //vdSetECRTransactionFlg(0);

    fManualSettle = FALSE;
    fAUTOManualSettle = FALSE;

//reset SMAC Flag
    fSMACTRAN = VS_FALSE;
    fAdviceTras = VS_FALSE;
    fnGlobalOrigHostEnable = 0;

    //inReversalType =0;
    fSkipBINRoutingForDebit = FALSE;
    fDualBrandedCard = FALSE;
    fSkipBINRoutingForCUP = FALSE;
    fPreConnectEx = FALSE;
    fBINVerPreConnectEx = FALSE;
    fRePrintFlag = FALSE;
    fNoCTLSSupportforBinRoute = FALSE;
    fReEnterOfflinePIN = FALSE;
    fNoEMVProcess = FALSE;
    byPackTypeBeforeDCCLog = d_OK;
    fRouteToSpecificHost = 0;
    //vdCTOSS_MainAPMemoryRecover();

    //inCTOSS_GetRAMMemorySize("TRANS END");
    //vdCTOSS_MainAPMemoryRecover();
    inRecoverRAM();
    put_env_int("CREDITBUSY",0);
    inCPTRead(1); //aaa fix on issue DIAL-UP icon display (handset) was changed to LAN icon display (network). Setting is pure dial-up. prevention only failed to replicate


    put_env_int("CONNECTED",0);
    vdSetIdleEvent(1);//set stgIdleEvent to 1 to reset sleep timer.
    put_env_int("BINROUTE",0);
    fBuildandSendProcess = VS_FALSE;
	inCTOSS_PutEnvDB("#BDOTRANSTYPE","");
	fBDOOptOutHostEnabled = 0;
    fInstApp = FALSE;

    ing_KeyPressed = 0;
    fEntryCardfromIDLE = FALSE;
	fAmountEntered = VS_FALSE;
	inCTOSS_PutEnvDB("DIGIWALLETAMT","0");
	ushCTOS_ClearePadSignature();
	inMultiAP_Database_BatchDelete();
	inCTOSS_GetRAMMemorySize("TRANS END");

	memset(gblszAmt, 0x00, sizeof(gblszAmt));

	//DisplayStatusLine("");

	if(get_env_int("ECRPROCESS")==1)
	 put_env_int("ECRPROCESS",0);

	strGBLVar.fGBLvDCC = FALSE;
	
	vdDebug_LogPrintf("vdCTOS_TransEndReset, end!");
	
    return;
}

void vdCTOS_SetTransEntryMode(BYTE bEntryMode)
{
    srTransRec.byEntryMode = bEntryMode;
}

void vdCTOS_SetTransType(BYTE bTxnType)
{
    srTransRec.byTransType = bTxnType;

    if (bTxnType == SALE)
        srTransRec.fOnlineSALE = CN_TRUE;
    else
        srTransRec.fOnlineSALE = CN_FALSE;

    if (bTxnType != SALE_OFFLINE)
        return;
    else
    {
        if (bTxnType == SALE_OFFLINE)
            srTransRec.fCompletion = CN_TRUE;
        else
            srTransRec.fCompletion = CN_FALSE;
    }

    inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
}
#if 1
void vdCTOSS_PackOfflinepinDisplay(OFFLINEPINDISPLAY_REC *szDisplayRec)
{
    BYTE szCurSymbol[20+1], szTotalAmount[20+1], szAmtBuff[20+1], szCurAmtBuff[20+1];
    int inCount=0;

	vdDebug_LogPrintf("vdCTOSS_PackOfflinepinDisplay - START");

    szDisplayRec->inDispFlag = 1;
    switch(srTransRec.byTransType)
    {
        //for improve transaction speed
        case SALE:
            strcpy(szDisplayRec->szDisplay1,"SALE");
            break;
        case REFUND:
            strcpy(szDisplayRec->szDisplay1,"REFUND");
            break;
        case PRE_AUTH:
            strcpy(szDisplayRec->szDisplay1,"CARD VER");
            break;
        default:
            strcpy(szDisplayRec->szDisplay1,"SALE");
            break;
    }

    //inLen=strlen(srTransRec.szCardLable);

    for(inCount=(strlen(szDisplayRec->szDisplay1)); inCount < (20-strlen(srTransRec.szCardLable));inCount++)
        strcat(szDisplayRec->szDisplay1,"  ");

    strcat(szDisplayRec->szDisplay1,srTransRec.szCardLable);

    memset(szTotalAmount,0x00,sizeof(szTotalAmount));
    memset(szAmtBuff,0x00,sizeof(szAmtBuff));
    memset(szCurAmtBuff,0x00,sizeof(szCurAmtBuff));

    strcpy(szDisplayRec->szDisplay3,"TOTAL:");

    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);

    if(srTransRec.fDCC && strTCT.fFormatDCCAmount == TRUE)
        vdDCCModifyAmount(szTotalAmount,szAmtBuff); //vdDCCModifyAmount
    else
        vdCTOSS_FormatAmount(strCST.szAmountFormat, szTotalAmount,szAmtBuff);

    sprintf(szCurAmtBuff,"         %s %s",strCST.szCurSymbol, szAmtBuff);
    //setLCDPrint(4, DISPLAY_POSITION_CENTER, szCurAmtBuff);
    strcpy(szDisplayRec->szDisplay4,szCurAmtBuff);

    strcpy(szDisplayRec->szDisplay5,"LAST PIN TRY");
    if(fReEnterOfflinePIN == FALSE)
        strcpy(szDisplayRec->szDisplay6,"Enter PIN:");
    else
        strcpy(szDisplayRec->szDisplay6,"Re-Enter PIN:");
}
#endif


short shCTOS_SetMagstripCardTrackData(BYTE* baTk1Buf, USHORT usTk1Len, BYTE* baTk2Buf, USHORT usTk2Len, BYTE* baTk3Buf, USHORT usTk3Len) //Invalid card reading fix -- jzg
{


    short shRet = d_OK;
    //int inTrack1AnalysisResult;


    //track 1
    if (usTk1Len > 0){
    	memcpy(srTransRec.szTrack1Data, &baTk1Buf[1], (usTk1Len -3));
    	srTransRec.usTrack1Len = (usTk1Len - 3);// REMOVE %, ? AND LRC
		vdAnalysisTrack1(&baTk1Buf[1], usTk1Len);
    }


    if (usTk2Len > 40)
        usTk2Len = 40;

    //track 2
    memcpy(srTransRec.szTrack2Data, &baTk2Buf[1], (usTk2Len - 3));
    srTransRec.usTrack2Len = (usTk2Len - 3);// REMOVE %, ? AND LRC

    //track 3
    memcpy(srTransRec.szTrack3Data, baTk3Buf, usTk3Len);
    srTransRec.usTrack3Len = usTk3Len;

    vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);

    if (usTk2Len >= 20)
    {
        shRet = shAnalysisTrack2(&baTk2Buf[1], usTk2Len); 

        vdDebug_LogPrintf("shAnalysisTrack2 shRet[%d]", shRet);


        if(shRet != d_OK){
            return INVALID_CARD;
        }
    }
    else
    {
        vdDebug_LogPrintf("JEFF::T2 INVALID! [%d]", usTk2Len);
        return INVALID_CARD;
    }

    return shRet;
}

void vdCTOS_ResetMagstripCardData(void)
{
    memset(srTransRec.szTrack1Data, 0x00, sizeof(srTransRec.szTrack1Data));
    srTransRec.usTrack1Len=0;

    memset(srTransRec.szTrack2Data, 0x00, sizeof(srTransRec.szTrack2Data));
    srTransRec.usTrack2Len=0;

    memset(srTransRec.szTrack3Data, 0x00, sizeof(srTransRec.szTrack3Data));
    srTransRec.usTrack3Len=0;

    memset(srTransRec.szCardholderName, 0x00, sizeof(srTransRec.szCardholderName));

    memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
    srTransRec.byPanLen = 0;
    memset(srTransRec.szExpireDate, 0x00, sizeof(srTransRec.szExpireDate));
    memset(srTransRec.szServiceCode, 0x00, sizeof(srTransRec.szServiceCode));

    srTransRec.byEntryMode = 0;
}

int inCTOS_CheckEMVFallbackTimeAllow(char* szStartTime, char* szEndTime, int inAllowTime)
{
    char szTempBuf[20];
    int inHH1, inHH2, inMM1, inMM2, inSS1, inSS2, inGap;

    if ((strlen(szStartTime) == 0) || (strlen(szStartTime) == 0))
        return (d_OK);

    if(0 == atoi(szStartTime))
        return (d_OK);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szStartTime[0], 2);
    inHH1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szStartTime[2], 2);
    inMM1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szStartTime[4], 2);
    inSS1 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szEndTime[0], 2);
    inHH2 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szEndTime[2], 2);
    inMM2 = atoi(szTempBuf);

    memset(szTempBuf, 0, sizeof(szTempBuf));
    memcpy(szTempBuf, &szEndTime[4], 2);
    inSS2 = atoi(szTempBuf);

    inGap = ((inHH2*3600)+(inMM2*60)+inSS2) - ((inHH1*3600)+(inMM1*60)+inSS1);

    if (inGap < 0 )
        return (d_OK);

    if(inGap > inAllowTime)
        return (d_NO);

    return d_OK;;

}


int inCTOS_CheckEMVFallback(void)
{
    BYTE szFallbackStartTime[20];
    BYTE szCurrentTime[20];
    int inRet;
    CTOS_RTC SetRTC;

    vdDebug_LogPrintf("saturn check emv fallback %d", srTransRec.szServiceCode[0]);
    if (strCDT.fChkServiceCode)
    {
        if(((srTransRec.szServiceCode[0] == '2') || (srTransRec.szServiceCode[0] == '6'))
           && (CARD_ENTRY_ICC != srTransRec.byEntryMode))
        {
            if(inFallbackToMSR == SUCCESS || (inFallbackToMSR == FAIL && fApplNotAvailable == TRUE))
            {
                CTOS_RTCGet(&SetRTC);
                sprintf(szCurrentTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
                inRet = inCTOS_CheckEMVFallbackTimeAllow(strTCT.szFallbackTime, szCurrentTime, strTCT.inFallbackTimeGap);

                inFallbackToMSR = FAIL;
                memset(strTCT.szFallbackTime,0x00,sizeof(strTCT.szFallbackTime));

                if(d_OK != inRet)
                    return FAIL;
                else
                {
                    //if(fApplNotAvailable == TRUE && (strCDT.IITid == BPI_EXPRESS || strCDT.IITid == DINERS))
                    if((fApplNotAvailable == TRUE && strCDT.IITid == BPI_EXPRESS) || strIIT.fEMVFallbackEnable == FALSE)
                        vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
                    else
                        vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                }
            }
            else
                return FAIL;
        }
    }
    else
    {
        if(fApplNotAvailable == TRUE)
            vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
    }

    return d_OK;;
}

int inCTOS_CheckIssuerEnable(void)
{
    int inEnable = 0;

    inEnable = strIIT.inCheckHost;
    vdDebug_LogPrintf("inCTOS_CheckIssuerEnable: [%d]", inEnable);

    if(0 == inEnable)
    {
        vdSetErrorMessage("TRANSACTION NOT ALLOWED");
        return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}


int inCTOS_CheckTranAllowd(void)
{
    int inEnable = 0;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    inEnable = strPIT.fTxnEnable;
    vdDebug_LogPrintf("inCTOS_CheckTranAllowd: [%d]", inEnable);

    if(0 == inEnable)
    {
        //vdSetErrorMessage("TRANS NOT ALLOWED");
        CTOS_LCDTClearDisplay();
        vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
        return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}

int inCTOS_CheckMustSettle(void)
{
    int inTxnCount=0;
    int inEnable = 0;
    BYTE szCurrDate[8] = {0};
    BYTE szTitle[20+1];
    BYTE szDisMsg[100];
    CTOS_RTC SetRTC;
    STRUCT_FILE_SETTING strFile;
    ACCUM_REC srAccumRec;


    if(strTCT.fChangeDateMustSettle == TRUE)
    {
        memset(szCurrDate, 0x00, sizeof(szCurrDate));
        CTOS_RTCGet(&SetRTC);
        sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
        vdDebug_LogPrintf("CURRENT DATE: [%s] :: SETTLE DATE: [%s]",szCurrDate,strMMT[0].szSettleDate);

        if ((strMMT[0].fMMTEnable == TRUE) && (wub_str_2_long(szCurrDate) >= wub_str_2_long(strMMT[0].szSettleDate)))
        {

            srTransRec.MITid=strMMT[0].MITid;
            vdCTOS_GetAccumName(&strFile, &srAccumRec);
            if ((inMyFile_CheckFileExist(strFile.szFileName)) > 0)
            {
                inTxnCount = inCheckHostBatchEmtpy(srTransRec.HDTid,srTransRec.MITid);

                if(inTxnCount > 0)
                {
                    strMMT[0].fMustSettFlag = 1;
                    inMMTSave(strMMT[0].MMTid);
                }
            }
        }
    }

    inEnable = strMMT[0].fMustSettFlag;
    vdDebug_LogPrintf("fMustSettFlag[%d] :: inEnable[%d]",strMMT[0].fMustSettFlag,inEnable);
    if(1 == inEnable)
    {
        //vdSetErrorMessage("MUST SETTLE");
        //memset(szDisMsg, 0x00, sizeof(szDisMsg));
        //memset(szTitle, 0x00, sizeof(szTitle));
        //szGetTransTitle(srTransRec.byTransType, szTitle);
        //strcpy(szDisMsg, szTitle);
        //strcat(szDisMsg, "|");
        //strcat(szDisMsg, "MUST SETTLE");
        //usCTOSS_LCDDisplay(szDisMsg);
        CTOS_Beep();
		inDisplayMessageBoxWithButtonConfirmation(1,8,"","Must settle","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
        CTOS_Delay(1500);
        return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}

void vdCTOS_FormatPAN(char *szFmt,char* szInPAN,char* szOutPAN)
{
    char szCurrentPAN[20];
    int inFmtIdx = 0;
    int inPANIdx = 0;
    int inFmtPANSize;

    inFmtPANSize = strlen(szFmt);
    if (strlen(szFmt) == 0)
    {
        strncpy(szOutPAN,szInPAN,inFmtPANSize);
        return;
    }

    memset(szCurrentPAN, 0x00, sizeof(szCurrentPAN));
    memcpy(szCurrentPAN,szInPAN,strlen(szInPAN));

    while(szFmt[inFmtIdx]!= 0x00)
    {
        if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
        {
            szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
            inFmtIdx++;
            inPANIdx++;
        }
        else if (szFmt[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' ||szFmt[inFmtIdx] == '*')
        {

            memcpy(&szOutPAN[inFmtIdx],&szFmt[inFmtIdx],1);
            inFmtIdx++;
            inPANIdx++;
        }
        else if (!isdigit(szFmt[inFmtIdx]))
        {
            szOutPAN[inFmtIdx] = szFmt[inFmtIdx];
            inFmtIdx++;
        }
    }

    //while(szCurrentPAN[inPANIdx]!= 0x00)
    //{
    // szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
    // inFmtIdx++;
    // inPANIdx++;
    //}

    return;
}

//0721
void vdCTOS_FormatPAN2(char *szFmt,char* szInPAN,char* szOutPAN)
{
    char szCurrentPAN[20];
    int inFmtIdx = 0;
    int inPANIdx = 0;
    int inFmtPANSize;
    int inPANless4;

    int i=0;

    inFmtPANSize = strlen(szInPAN);

    inPANless4 = inFmtPANSize - 4;



    if (strlen(szFmt) == 0)
    {
        strncpy(szOutPAN,szInPAN,inFmtPANSize);
        return;
    }

    memset(szCurrentPAN, 0x00, sizeof(szCurrentPAN));
    memcpy(szCurrentPAN,szInPAN,strlen(szInPAN));

    while(i<=inFmtPANSize)
    {

        if (i >= inPANless4){


            if (szFmt[inFmtIdx] == ' '){
                szOutPAN[inFmtIdx] = szFmt[inFmtIdx];
                inFmtIdx++;
            }else{
                szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx]; ;
                inFmtIdx++;
                inPANIdx++;
                i++;
            }
            continue;

        }


        if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
        {
            szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
            inFmtIdx++;
            inPANIdx++;
            i++;
        }
        else if (szFmt[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' ||szFmt[inFmtIdx] == '*')
        {

            memcpy(&szOutPAN[inFmtIdx],&szFmt[inFmtIdx],1);
            inFmtIdx++;
            inPANIdx++;
            i++;
        }
        else if (!isdigit(szFmt[inFmtIdx]))
        {
            szOutPAN[inFmtIdx] = szFmt[inFmtIdx];
            inFmtIdx++;
        }
    }

    //while(szCurrentPAN[inPANIdx]!= 0x00)
    //{
    // szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
    // inFmtIdx++;
    // inPANIdx++;
    //}

    return;
}


//0721

void vdCTOS_FormatAmount(char *szFmt,char* szInAmt,char* szOutAmt)
{
    char szCurrentAmt[20];
    char szBuf[20];
    char szFinalFmt[20];
    int inFmtIdx = 0;
    int inTotaActNum = 0;
    int inPadNum = 0;
    int inignore = 0;
    int inAmtIdx = 0;
    int inFmtAmtSize;

    inFmtAmtSize = strlen(szFmt);
    if (strlen(szFmt) == 0)
    {
        strncpy(szOutAmt,szInAmt,inFmtAmtSize);
        return;
    }

    inFmtIdx = 0;
    while(szFmt[inFmtIdx]!= 0x00)
    {
        if(szFmt[inFmtIdx] == 'n')
            inPadNum ++;

        if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
            inTotaActNum ++;

        inFmtIdx ++;
    }

    inFmtIdx = 0;
    while(0x30 == szInAmt[inFmtIdx])
    {
        inFmtIdx ++;
    }

    memset(szCurrentAmt, 0x00, sizeof(szCurrentAmt));
    strcpy(szCurrentAmt,&szInAmt[inFmtIdx]);

    if(inPadNum > strlen(szCurrentAmt))
    {
        memset(szBuf, 0x00, sizeof(szBuf));
        memset(szBuf, 0x30, inPadNum-strlen(szCurrentAmt));
        strcat(szBuf, szCurrentAmt);

        strcpy(szCurrentAmt, szBuf);
        strcpy(szFinalFmt, szFmt);
    }

    if(inTotaActNum > strlen(szCurrentAmt))
    {
        inFmtIdx = 0;
        while(szFmt[inFmtIdx]!= 0x00)
        {
            if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
                inignore ++;

            inFmtIdx ++;

            if((inignore >= (inTotaActNum - strlen(szCurrentAmt))) && (szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n'))
                break;
        }

        strcpy(szFinalFmt, &szFmt[inFmtIdx]);
    }

    inFmtIdx = 0;
    inAmtIdx = 0;
    while(szFinalFmt[inFmtIdx]!= 0x00)
    {
        if(szFinalFmt[inFmtIdx] == 'N' || szFinalFmt[inFmtIdx] == 'n')
        {
            szOutAmt[inFmtIdx] = szCurrentAmt[inAmtIdx];
            inFmtIdx++;
            inAmtIdx++;
        }
        else if ((szFinalFmt[inFmtIdx]) == ' ' || (szFinalFmt[inFmtIdx]) == ',' || (szFinalFmt[inFmtIdx]) == '.')
        {
            szOutAmt[inFmtIdx] = szFinalFmt[inFmtIdx];
            inFmtIdx++;
        }
        else
        {
            inFmtIdx ++ ;
        }
    }

    szOutAmt[inFmtIdx] = 0x00;

    return;
}


int inGetIssuerRecord(int inIssuerNumber)
{
    int inRec = 1;
    do
    {
        //if (inIITRead(inRec) != d_OK)
        if (inIITRead(inIssuerNumber) != d_OK)
        {
            return(d_NO);
        }
        inRec++;
    } while (inIssuerNumber != strIIT.inIssuerNumber);

    return(d_OK);
}



#if 0
int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine)
{
    char szStr[50 + 1];
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE szTemp1[30+1];
	//gcitra
    unsigned char key;


	CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);


   // CTOS_LCDTClearDisplay();
	//gcitra

    memset(szStr,0x00,sizeof(szStr));
    memset(EMVtagVal,0x00,sizeof(EMVtagVal));

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;

        if(EMVtagLen > 0)
        {
            sprintf(szStr, "%s", EMVtagVal);
            vdDebug_LogPrintf("Card label(str): %s",szStr);
        }
    }

    if(0 == strlen(szStr))
    {
        sprintf(szStr, "%s", strCDT.szCardLabel);
        vdDebug_LogPrintf("Card label: %s",strCDT.szCardLabel);
    }

    memset(szTemp1,0,sizeof(szTemp1));
    vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);

    if(0>= usCardTypeLine || 0 >= usPANLine)
    {
        usCardTypeLine = 3;
        usPANLine = 4;
    }

	//gcitra

    CTOS_LCDTPrintXY(1, usCardTypeLine, szStr);
  	//setLCDPrint27(usCardTypeLine, DISPLAY_POSITION_RIGHT, szStr);
    CTOS_LCDTPrintXY(1, usPANLine, szTemp1);



    //gcitra-add PAN confirmation
	if (strTCT.fConfirmPAN){
		key=WaitKey(15);

		if(key != d_KBD_ENTER)
			return d_NO;
	}
	//gcitra

    return(d_OK);
}
#endif

int inCTOS_DisplayCardTitle(USHORT usCardTypeLine, USHORT usPANLine) {
    char szStr[50 + 1];
    USHORT EMVtagLen;
    BYTE EMVtagVal[64];
    BYTE szTemp1[30 + 1];

    BYTE szPAN1[20 + 1];
    BYTE szPAN2[20 + 1];
    int inRemaining = 0;


    BYTE szTitle[16 + 1];
    BYTE szDisMsg[100];

    //Tine:  variables for ConfirmCardDetails display
    int inKey = 0;
    BYTE szCardDisplay[1024 + 1];
    BYTE szExpYear[3];
    BYTE szExpMonth[3];
    BYTE szTemp[5];
    BYTE szTotalAmt[12 + 1];
    char szAmountBuff[45 + 1];
    char szTemp4[45 + 1];
	//tine:  android
	BYTE szTemp2[10+1];
	BYTE szAmtMsg[45 + 1];

	// uneeded by request (BDO) -- sidumili
	#ifdef ANDROID_NEW_UI
		return d_OK;
	#endif

    //gcitra-0806
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    //gcitra-0806

    //CTOS_LCDTClearDisplay();


    //1102
#if 0
    if (srTransRec.byTransType != BIN_VER){ //aaa temp tatanong ko kay glads ito
        vdDebug_LogPrintf("Issuer logo = [%s]", strIIT.szIssuerLogo);
        if ((strCDT.HDTid != 5) && (strCDT.fFleetCard != TRUE)) /* BDOCLG-00319: Revised no logo for fleet cards -- jzg */
            displayAppbmpDataEx(1,1, strIIT.szIssuerLogo);

    }
#endif
    //1102




    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);



//issue-00399 - Fix overlapping display for installment
    if ((fInstApp == TRUE) && (srTransRec.byTransType == SALE))
        strcpy(szTitle, "INSTALLMENT");
//end


#if 1

    memset(szStr, 0x00, sizeof(szStr));
    memset(EMVtagVal, 0x00, sizeof(EMVtagVal));

    //Display Issuer logo: Clear the display first, then re-display trans title - start -- jzg
    //CTOS_LCDTClearDisplay();
    //DispTransTitle(srTransRec.byTransType);


    /* BDO-00141: Used setLCDPrint instead of setLCDPrint27 to avoid garbage display - start -- jzg */
    //setLCDPrint27(1, DISPLAY_POSITION_RIGHT, strCDT.szCardLabel);
    //setLCDPrint27(2, DISPLAY_POSITION_RIGHT, szTitle);

    if (srTransRec.byTransType != BIN_VER) { //aaa temp tatanong ko kay glads ito
        vdDebug_LogPrintf("Issuer logo = [%s]", strIIT.szIssuerLogo);
        //if ((strCDT.HDTid != 53) && (strCDT.fFleetCard != TRUE)) /* BDOCLG-00319: Revised no logo for fleet cards -- jzg */
        //displayAppbmpDataEx(1,1, strIIT.szIssuerLogo);  -- Tine, on comment

    }
//Tine: android - removed
/*
	if(strTCT.fATPBinRoute == TRUE && srTransRec.IITid == BDO_CREDIT)
		CTOS_LCDTPrintAligned(1, " ", d_LCD_ALIGNRIGHT);
	else
	{
		if(srTransRec.byTransType != BIN_VER)
			CTOS_LCDTPrintAligned(1, strCDT.szCardLabel, d_LCD_ALIGNRIGHT);
	}
	CTOS_LCDTPrintAligned(2, szTitle, d_LCD_ALIGNRIGHT);

	//Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg
*/
    if (srTransRec.byEntryMode == CARD_ENTRY_ICC) {
        EMVtagLen = 0;

        if (EMVtagLen > 0) {
            sprintf(szStr, "%s", EMVtagVal);
            vdDebug_LogPrintf("Card label(str): %s", szStr);
        }
    }

/*
    if(0 == strlen(szStr))
    {
        //1102
        if (srTransRec.byTransType != BIN_VER){
        	sprintf(szStr, "%s", strCDT.szCardLabel);
        	vdDebug_LogPrintf("Card label: %s",strCDT.szCardLabel);
        }
		//1102
    }
*/
	
    strcpy(szStr, "PAN:");
    memset(szTemp1, 0, sizeof(szTemp1));
    //if (strTCT.fMaskPanDisplay == TRUE){
    //inIITRead(strCDT.IITid);
    inIITRead(srTransRec.IITid);//Fix for incorrect Issuer loaded on bin routing
	
    if (strIIT.fMaskPanDisplay == TRUE) {
        vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
        strcpy(szTemp1, srTransRec.szPAN);
        cardMasking(szTemp1, 5);
    } else {
        vdCTOS_FormatPAN2(strTCT.DisplayPANFormat, srTransRec.szPAN, szTemp1);
    }


//	displayAppbmpDataEx(140, 35, "mc.bmp");

    if (0 >= usCardTypeLine || 0 >= usPANLine) {
        usCardTypeLine = 5;
        usPANLine = 6;
    }
    //Display Issuer logo: Clear the display first, then re-display trans title - end -- jzg

    //CTOS_LCDTPrintXY(1, usCardTypeLine, szStr); --Tine:  android - removed



    //0721
    if (strlen(szTemp1) > 20) {
        memset(szPAN1, 0x00, sizeof(szPAN1));
        memset(szPAN2, 0x00, sizeof(szPAN2));
        inRemaining = strlen(szTemp1) - 20;

        memcpy(szPAN1, szTemp1, 20);
        memcpy(szPAN2, &szTemp1[20], inRemaining);
        //setLCDPrint(7, DISPLAY_POSITION_RIGHT, szPAN1); /* BDO-00141: Used setLCDPrint instead of setLCDPrint27 to avoid garbage display -- jzg */

        //Tine, on if statement
/*
        CTOS_LCDTPrintXY(1, 7, szPAN1);
		CTOS_LCDTPrintXY(1, 8, szPAN2);
		*/
    }
    //else
    //setLCDPrint(7, DISPLAY_POSITION_RIGHT, szTemp1); /* BDO-00141: Used setLCDPrint instead of setLCDPrint27 to avoid garbage display -- jzg */
    //CTOS_LCDTPrintXY(1, 7, szTemp1);

    //0721

    //TINE:  android - add total amount in Card Details display
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
    memset(szTemp4, 0x00, sizeof(szTemp4));
    memset(szAmountBuff, 0x00, sizeof(szAmountBuff));

    if (srTransRec.fDCC && strTCT.fFormatDCCAmount == TRUE)
        vdDCCModifyAmount(szTotalAmt, szTemp4);//vdDCCModifyAmount(&szTemp4);
    else
        vdCTOS_FormatAmount(strCST.szAmountFormat, szTotalAmt, szTemp4);// patrick add code 20141216
    sprintf(szAmountBuff, "%s %s", strCST.szCurSymbol, szTemp4);

	memset(szAmtMsg, 0x00, sizeof(szAmtMsg));
    sprintf(szAmtMsg, "TOTAL AMOUNT\n%s", szAmountBuff);
	 
    //TINE:  display ExpiryDate
    memset(szTemp, 0x00, sizeof(szTemp));
    memset(szExpYear, 0x00, sizeof(szExpYear));
    memset(szExpMonth, 0x00, sizeof(szExpMonth));

    wub_hex_2_str(srTransRec.szExpireDate, szTemp, 2);
    memcpy(szExpYear, &szTemp[0], 2);
    memcpy(szExpMonth, &szTemp[2], 2);
	//android:  tine--
	memset(szTemp2, 0x00, sizeof(szTemp2));
	if(strIIT.fCustExpDate == 1)  //mask expiry date display (android)
		strcpy(szTemp2, "**/**");
	else
		strcpy(szTemp2, " ");
		
		
#endif

#if 0
    //Tine:  add ConfirmCardDetails display - 12Mar2019
    memset(szCardDisplay, 0x00, sizeof(szCardDisplay));
    strcpy(szCardDisplay, szTitle);
    strcat(szCardDisplay, "|");
    strcat(szCardDisplay, szTemp1);     //PAN
    strcat(szCardDisplay, "|");
    strcat(szCardDisplay, szTemp2);		//expiry date
    strcat(szCardDisplay, "|");
    strcat(szCardDisplay, srTransRec.szCardholderName);
    strcat(szCardDisplay, "|");
    strcat(szCardDisplay,  (srTransRec.byTransType == BALANCE_INQUIRY ? " " : "Total Amount: "));
    strcat(szCardDisplay, (srTransRec.byTransType == BALANCE_INQUIRY ? " " : szAmountBuff));
    strcat(szCardDisplay, "|");
    strcat(szCardDisplay, strIIT.szIssuerLogo);

    vdDebug_LogPrintf("ConfirmCardDetails szCardDisplay[%s]", szCardDisplay);
	inKey = ConfirmCardDetails(szCardDisplay);
#else
   // if (srTransRec.HDTid == 53){
   // 	strcpy(strIIT.szIssuerLogo,"visa.bmp");
    //}

	/*
	memset(szCardDisplay, 0x00, sizeof(szCardDisplay));
    strcpy(szCardDisplay, szTitle);
    strcat(szCardDisplay, "|");
	strcat(szCardDisplay, "PAN:");
	strcat(szCardDisplay, "|");
    strcat(szCardDisplay, szTemp1);     //PAN
    strcat(szCardDisplay, "|");
	//TEST-REMOVE
    strcat(szCardDisplay, strIIT.szIssuerLogo);
    //strcat(szCardDisplay, "test1.bmp");
	strcat(szCardDisplay, "|");
    if (strcmp(strIIT.szIssuerLabel,"CREDIT") == 0)
    	strcat(szCardDisplay, " ");
	else	
        strcat(szCardDisplay, strIIT.szIssuerLabel);
     */

	memset(szCardDisplay, 0x00, sizeof(szCardDisplay));
	strcpy(szCardDisplay, szTitle);
	strcat(szCardDisplay, "|");
	strcat(szCardDisplay, szTemp1);	//PAN
	strcat(szCardDisplay, "|");
	strcat(szCardDisplay, szExpMonth);
	strcat(szCardDisplay, "/");
	strcat(szCardDisplay, szExpYear);
	strcat(szCardDisplay, "|");
	strcat(szCardDisplay, srTransRec.szCardholderName);	
	strcat(szCardDisplay, "|");
	strcat(szCardDisplay, (srTransRec.byTransType == BALANCE_INQUIRY ? "" : szAmtMsg));
	strcat(szCardDisplay, "|");
	//strcat(szCardDisplay, (strIIT.szIssuerLogo == "CREDIT" ? " " : strIIT.szIssuerLogo));
	strcat(szCardDisplay, strIIT.szIssuerLogo);

    vdDebug_LogPrintf("saturn ConfirmDetails szCardDisplay[%s]", szCardDisplay);

	inKey = ConfirmDetails(szCardDisplay);
#endif

    

    vdDebug_LogPrintf("saturn confirm PAN %d", inKey);

	if (inKey== d_USER_CANCEL) /*key cancel*/
	{
		 //vdSetErrorMessage("Amt entry cancelled");
		 
		 //DisplayStatusLine("USER CANCEL");
		 //vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		 //inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		 CTOS_Beep();
		 //CTOS_Delay(1500);
		 vdSetErrorMessage("");
		 return d_USER_CANCEL;
	}
	else if (inKey == 0xFF) /*timeout*/
	{
		//DisplayStatusLine("TIME OUT");
		//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1500);
		return d_TIMEOUT;
	}
	
	if (inKey != d_OK)
		return d_NO;

    vdDisplayMessageStatusBox(1, 8, "Processing", MSG_PLS_WAIT, MSG_TYPE_PROCESS);

    return (d_OK);
}


short inCTOS_LoadCDTIndex(void)
{
    signed int inRetVal;
    short   shStatus;
    int  i=0, selectedRDTIndex;
    BYTE    shSuitableRDTIndex[10];
    int inIssuer = 0;
    int inNumberOfMatches = 0;
    int inRecNumArray[4];
    char szChoiceMsg[20 + 1], szStr[24+1];
    int    inCardLen, j;
    int inMaxCDTid = 0;
    int inFindRecordNum=0;

    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1], szAmtBuff[20+1], szCurAmtBuff[20+1];
    BYTE szDisMsg[200];

	vdDebug_LogPrintf("--inCTOS_LoadCDTIndex--");
	vdDebug_LogPrintf("fInstApp=[%d]", fInstApp);
	vdDebug_LogPrintf("fEntryCardfromIDLE=[%d]", fEntryCardfromIDLE);
	vdDebug_LogPrintf("fIdleInsert=[%d]", fIdleInsert);
	
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

    //CTOS_LCDTClearDisplay();
    //gcitra-0728
    //CTOS_LCDTPrintXY(1, 8, "Checking Card... ");
    //gcitra-0728


    if (fInstApp == TRUE)
        inInstallmentCDTReadMulti(srTransRec.szPAN, &inFindRecordNum);
    else
        inCDTReadMulti(srTransRec.szPAN, &inFindRecordNum);

    if(inFindRecordNum == 0)
    {
        vdDebug_LogPrintf("Not find in CDT");
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    //BDO PHASE 2: [Fix for manual entry] -- glady
#if 0
    for(j=0;j<inFindRecordNum;j++)
    {
    	//if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD) && !(!strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL))
		if (!(strTCT.fDebitFlag == VS_FALSE && strMCDT[j].inType == DEBIT_CARD) && !(strMCDT[j].fManEntry && srTransRec.byEntryMode == CARD_ENTRY_MANUAL)) //BDO: should get correct record during manual entry -- jzg
    	{
        	if ((strMCDT[j].inType == DEBIT_CARD) || (strMCDT[j].inType == EBT_CARD) || (strMCDT[j].IITid != inIssuer))
      		{
            	if (strMCDT[j].inType != DEBIT_CARD && strMCDT[j].inType != EBT_CARD)
        			inIssuer = strMCDT[j].IITid;

        		inRecNumArray[inNumberOfMatches++] = strMCDT[j].CDTid;

        		if(inNumberOfMatches > 0)
              		break;

                if (inNumberOfMatches > 1)
                    szChoiceMsg[strlen(szChoiceMsg)] = '~';

          		switch (strMCDT[j].inType)
          		{
                    case DEBIT_CARD:
              			break;
            		case EBT_CARD:
              			break;
            		case PURCHASE_CARD:
              			break;
            		default:
              			break;
          		}

          		if (inNumberOfMatches > 3)
              		break;
			}
      	}
    }
#endif

    inNumberOfMatches = 1;
    inRecNumArray[0] = strMCDT[0].CDTid;
    //BDO PHASE 2: [Fix for manual entry] -- glady

    if (inNumberOfMatches == 1)
    {
        inRetVal = inRecNumArray[0];
    }
    else if (inNumberOfMatches > 1)
    {
        //CTOS_LCDTClearDisplay();

        /* BDO CLG: Fleet card support - start -- jzg */
        //if(srTransRec.fFleetCard == TRUE)
        //	vdDispTransTitle(FLEET_SALE);
        //else
        /* BDO CLG: Fleet card support - end -- jzg */
        //vdDispTransTitle(srTransRec.byTransType);
    }

    if (inRetVal >= 0)
    {
#if 1
        inDatabase_TerminalOpenDatabase();
        inCDTReadEx(inRetVal);
        srTransRec.CDTid = inRetVal;// save the current CDT that is loaded
		strCDT.CDTid = srTransRec.CDTid;
		
        if(inIITReadEx(strCDT.IITid) != d_OK)
        {
            inDatabase_TerminalCloseDatabase();
            vdSetErrorMessage("LOAD IIT ERROR");
            return(d_NO);
        }

        vdDebug_LogPrintf("inCTOS_LoadCDTIndex strCDT.HDTid: %d", strCDT.HDTid);

        if(inHDTReadEx(strCDT.HDTid) != d_OK)
        {
            inDatabase_TerminalCloseDatabase();
            //vdSetErrorMessage("LOAD HDT ERROR");
            inHDTReadData(strCDT.HDTid);
            memset(szStr, 0x00, sizeof(szStr));
            strcpy(szStr,strHDT_Temp.szHostLabel);
            memset(strHDT_Temp.szHostLabel,0x00,sizeof(strHDT_Temp.szHostLabel));
            sprintf(strHDT_Temp.szHostLabel," %s ",szStr);

#if 1
            vdDisplayErrorMsgResp2(strHDT_Temp.szHostLabel, "TRANSACTION", "NOT ALLOWED");

            //vdDebug_LogPrintf("AAA - szDisMsg[%s]", szDisMsg);

#else
            memset(szDisMsg,"",sizeof(szDisMsg));
            memset(szTitle, "", sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, szTitle);
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "NOT ALLOWED");
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "BDO TRANSACTION");
            usCTOSS_LCDDisplay(szDisMsg);
			
            CTOS_Beep();
            CTOS_Delay(1500);
            CTOS_Beep();
#endif
            inDatabase_TerminalCloseDatabase();
            return(d_NO);
        }
        inDatabase_TerminalCloseDatabase();
#else
        inCDTRead(inRetVal);
        srTransRec.CDTid = inRetVal;// save the current CDT that is loaded

        /* Get the CDT also for card labels */
        inGetIssuerRecord(strCDT.IITid);
#endif
    }
    else
    {
        vdDebug_LogPrintf("Not find in CDT");
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return INVALID_CARD;
    }

    //-----------------------------------------------------
    vdDebug_LogPrintf("LOAD CDT INDEX [START]");
	vdDebug_LogPrintf("srTransRec.CDTid[%d]", srTransRec.CDTid);
    vdDebug_LogPrintf("strCDT.CDTid[%d]", strCDT.CDTid);
    vdDebug_LogPrintf("strCDT.HDTid[%d]", strCDT.HDTid);
    vdDebug_LogPrintf("strCDT.IITid[%d]", strCDT.IITid);
    vdDebug_LogPrintf("strCDT.szPANLo[%s]", strCDT.szPANLo);
    vdDebug_LogPrintf("strCDT.szPANHi[%s]", strCDT.szPANHi);
    vdDebug_LogPrintf("strCDT.szCardLabel[%s]", strCDT.szCardLabel);
    vdDebug_LogPrintf("strCDT.fManEntry[%d]", strCDT.fManEntry);
    vdDebug_LogPrintf("strCDT.inType[%d]", strCDT.inType);
	vdDebug_LogPrintf("strCDT.fInstallmentEnable[%d]", strCDT.fInstallmentEnable);
	vdDebug_LogPrintf("strCDT.inInstGroup[%d]", strCDT.inInstGroup);
	vdDebug_LogPrintf("strCDT.fBalInqAllowed[%d]", strCDT.fBalInqAllowed);
	vdDebug_LogPrintf("strCDT.fCDTATPEnable[%d]", strCDT.fCDTATPEnable);

    vdDebug_LogPrintf("strCDT.fPANCatchAll[%d]", strCDT.fPANCatchAll);
	vdDebug_LogPrintf("strCDT.fDCCEnable[%d]", strCDT.fDCCEnable);
	vdDebug_LogPrintf("strCDT.fDualCurrencyEnable[%d]", strCDT.fDualCurrencyEnable);
	vdDebug_LogPrintf("strTCT.fATPBinRoute[%d]", strTCT.fATPBinRoute);
	vdDebug_LogPrintf("strIIT.fGetPolicyNumber[%d]", strIIT.fGetPolicyNumber);
	

    vdDebug_LogPrintf("strCDT.HDTid[%d]", strCDT.HDTid);
    vdDebug_LogPrintf("LOAD CDT INDEX [END  ]");
    //-----------------------------------------------------

    /* Check for proper card length */
    inCardLen = strlen(srTransRec.szPAN);

    vdDebug_LogPrintf("card len gcitra [%d]- %d-%d",inCardLen, strCDT.inMinPANDigit, strCDT.inMaxPANDigit);
    if ((inCardLen < strCDT.inMinPANDigit) ||
        (inCardLen > strCDT.inMaxPANDigit))
    {
        vdDebug_LogPrintf("BAD CARD LEN");
        vdSetErrorMessage("BAD CARD LEN");
        return INVALID_CARD;
    }

    if (strCDT.fluhnCheck == VS_TRUE)   /* Check Luhn */
    {
        if (chk_luhn(srTransRec.szPAN))
        {
            vdDisplayErrorMsg(1, 8, "INVALID LUHN");
            return INVALID_CARD;
        }
    }


    if (strTCT.fDebitFlag != DEBIT && strCDT.inType == DEBIT_CARD )
    {
        vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
        return INVALID_CARD;
    }

    if(strCDT.fExpDtReqd)
    {
        if(shChk_ExpireDate() != d_OK)
        {
            if(srTransRec.byEntryMode == CARD_ENTRY_WAVE)
                return CTLS_EXPIRED_CARD;
            else
            {
                vdDisplayErrorMsg(1, 8, "EXPIRED CARD");
                return CARD_EXPIRED;
            }
        }
    }

    //for save Accum file
    srTransRec.IITid= strCDT.IITid;
    srTransRec.HDTid = strCDT.HDTid;
    srTransRec.inCardType = strCDT.inType;

    srTransRec.fFleetCard = strCDT.fFleetCard; // BDO CLG: Fleet card support -- jzg

    /* BDO CLG: Fleet card support - start -- jzg */
    //if(srTransRec.fFleetCard == TRUE)
    //{
    //	if(strCDT.fFleetCard != TRUE)
    //	{
    //    	vdDebug_LogPrintf("NOT FLEET CARD!");
    //    	vdSetErrorMessage("CARD NOT SUPPORTED");
    //		return INVALID_CARD;
    //	}
    //}
    /* BDO CLG: Fleet card support - end -- jzg */

    //CTOS_LCDTClearDisplay();
//    CTOS_LCDTPrintXY(1, 8, "                   ");
    return d_OK;
}


int inCTOS_EMVCardReadProcess (void)
{
    short shResult = 0;
    USHORT usMsgFailedResult = 0;

    //CTOS_LCDTClearDisplay();
    //vdClearBelowLine(2);

    vdDebug_LogPrintf("saturn -------shCT0S_EMVInitialize1---[%d]--",shResult);
    shResult = shCTOS_EMVAppSelectedProcess();


    vdDebug_LogPrintf("saturn -------shCT0S_EMVInitialize---[%d]--",shResult);

    if(shResult == EMV_CHIP_FAILED)
    {
        usMsgFailedResult = MSG_TRANS_ERROR;
        return usMsgFailedResult;
    }
    else if(shResult == EMV_USER_ABORT)
    {
        usMsgFailedResult = MSG_USER_CANCEL;
        return usMsgFailedResult;
    }
    else if(shResult == EMV_TRANS_NOT_ALLOWED)
    {
        //usMsgFailedResult = EMV_TRANS_NOT_ALLOWED;
        usMsgFailedResult = EMV_APPL_NOT_AVAILABLE;
        return usMsgFailedResult;
    }

	//certification
	else if(shResult == EMV_CARD_BLOCKED){
        usMsgFailedResult = EMV_CARD_BLOCKED;
        return usMsgFailedResult;

	}

    shCTOS_EMVGetChipDataReady();

    return (d_OK);
}


int inCTOS_ManualEntryProcess (BYTE *szPAN)
{
    USHORT  usMaxLen = 19;
    BYTE    szTempBuf[10];
    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    char    szTitle[20+1];

    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(srTransRec.byTransType);
#if 1
    if (srTransRec.byTransType == SMAC_BALANCE)
        CTOS_LCDTPrintXY(1, 3, "SWIPE/CANCEL");
        //else if (srTransRec.byTransType == BALANCE_INQUIRY)
        //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/CANCEL");
    else
    {
        szGetTransTitle(srTransRec.byTransType,szTitle);
        vdDebug_LogPrintf("AAA - szTitle[%s] fInstApp[%d]", szTitle, fInstApp);
        if ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == CASH_ADVANCE || srTransRec.byTransType == BIN_VER || srTransRec.byTransType == BALANCE_INQUIRY
                                                 || srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == SALE_OFFLINE))
        {
            if(fInstApp==FALSE)
            {
                // CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/ENTER");
                // CTOS_LCDTPrintXY(1, 4, "TAP CARD"); //aaronnino for BDOCLG ver 9.0 fix on issue #00125 Incorrect terminal display
                //if(srTransRec.byTransType == BIN_VER || srTransRec.byTransType == BALANCE_INQUIRY)
                //vduiClearBelow(2);
                //CTOS_LCDTPrintXY(1, 4, "PAN:");
            }
            else
            {
                // CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/ENTER");
                //  CTOS_LCDTPrintXY(1, 4, "CARD");
                //vduiClearBelow(2);
                //CTOS_LCDTPrintXY(1, 4, "PAN:");
            }
        }
    }
#endif
    // patrick add code 20141209
    //szPAN[0] = chGetFirstIdleKey();

    if(getCardNO(szPAN) != d_OK)
    {
        return USER_ABORT;
    }

    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(srTransRec.byTransType);
    //setLCDPrint(4, DISPLAY_POSITION_LEFT, "CARD NO: ");
    memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
    memset(bDisplayStr, 0x20, usMaxLen*2);
    strcpy(&bDisplayStr[(usMaxLen-strlen(szPAN))*2], szPAN);
    //CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-usMaxLen*2, 5, bDisplayStr);
    //setLCDPrint(7, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

    memset(szTempBuf,0,sizeof(szTempBuf));
    //if(getExpDate(szTempBuf) != d_OK)
    if(getExpiryDate(szTempBuf) != d_OK) // ui android for get expiry date -- sidumili
    {
		USHORT usTk1Len=TRACK_I_BYTES, usTk2Len=TRACK_II_BYTES, usTk3Len=TRACK_III_BYTES;
		BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];

  		CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);	//clear buffer on idle swipe on term reg.
  	
		return USER_ABORT;
    }
    wub_str_2_hex(szTempBuf, &srTransRec.szExpireDate[1], 2);
    wub_str_2_hex(&szTempBuf[2], srTransRec.szExpireDate, 2);
    CTOS_KBDBufFlush ();

    vdCTOS_SetTransEntryMode(CARD_ENTRY_MANUAL);
    return d_OK;
}


#if 1
int inCTOS_GetCardFields(void)
{
    int inRet2 = 0;
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES_50], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES_50 ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
    ULONG ulAPRtn;
    BYTE temp[64];
    char szTotalAmount[AMT_ASC_SIZE+1];
    int inchipswiped = 0; //aaalcantara issue number 000113 1 of 3
    EMVCL_RC_DATA_EX stRCDataEx;
    BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[3+1],szCurrCode[3+1];

    CTOS_RTC SetRTC;

    BYTE szBaseAmt[30+1] = {0};
    //0826
    int inChipTries=0;
    int inEntryMode=0;
    BOOL fClearScreen=FALSE;

    BYTE szTitle[20+1];
    BYTE szDisMsg[200];
    BYTE    szTotalAmt[12+1];
    BYTE    szStr[45];
    BYTE szManualPan[25];

    USHORT usPanLen = 20;
    BYTE   szInvNoAsc[100+1];
    BYTE   szInvNoBcd[3];
    BYTE bRet;
    BYTE szDisplay[200];


    BYTE pbDisplay[512+1];  //Tine:  for Cancel UI
    int inKey=0;
    int swipe_loopcount = 0;
    int insert_loopcount = 0;
	BYTE szManualEntryFlag[2];
	char szAmtMsg[45 + 1];

    /*
	1= insert only
	2= swipe only
	0= will accept al
	*/

    //CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - start -- jzg
    char szBaseAmount[20] = {0};
    char szTipAmount[20] = {0};
    //CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - end -- jzg

#define INSERT_ONLY 1
#define SWIPE_ONLY	2
#define READ_ALL 0
    //0826

    short shReturn = d_OK; //Invalid card reading fix -- jzg


    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
    BOOL fMaxCTLSAmt = FALSE;
    long amt1 = 0;
    long amt2 = 0;
    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- jzg */


    /* BDO CLG: MOTO setup -- jzg */
    int inMOTOResult;

	char szAmountBuff[45 + 1];

	VS_BOOL fCTLSEnable=VS_FALSE;

	vdDebug_LogPrintf("saturn inCTOS_GetCardFields...");
	vdDebug_LogPrintf("fIdleInsert=[%d]", fIdleInsert);
	vdDebug_LogPrintf("fIdleSwipe=[%d]", fIdleSwipe);
	vdDebug_LogPrintf("fBINVer=[%d]", fBINVer);
	
    fApplNotAvailable = FALSE;

    // get transaction title
    memset(szTitle, 0x00, sizeof(szTitle));
    //szGetTransTitle(srTransRec.byTransType, szTitle);
    szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);

	// get manual entry flag
	memset(szManualEntryFlag, 0x00, sizeof(szManualEntryFlag));
	if(strTCT.fEnableManualKeyEntry == 1)
        strcpy(szManualEntryFlag, "1");
    else
        strcpy(szManualEntryFlag, "0");

	// Get Amount
	memset(szAmtMsg, 0x00, sizeof(szAmtMsg));
	memset(szTotalAmt, 0x00, sizeof(szTotalAmt));
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
	vdCTOS_FormatAmount(strCST.szAmountFormat, szTotalAmt, szAmtMsg);
	
	if (strlen(srTransRec.szPAN) == 0){
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
    strcpy(szDisMsg, szTitle);
    strcat(szDisMsg, "|");
	if(srTransRec.byTransType == PRE_AUTH || srTransRec.byTransType == SALE_OFFLINE)
		strcat(szDisMsg, "Insert/Swipe Card");
	else
    	strcat(szDisMsg, "Tap/Insert Card");

	strcat(szDisMsg, "|");
    strcat(szDisMsg, strCST.szCurSymbol);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, szAmtMsg);
	strcat(szDisMsg, "|");
    strcat(szDisMsg, szManualEntryFlag);
    //usCTOSS_LCDDisplay(szDisMsg);
    vdDebug_LogPrintf("saturn swipe/insert/tap 1");
    inKey=usCARDENTRY(szDisMsg);			//Tine:  24Apr2019
	}


    //vdDebug_LogPrintf("inCTOS_WaveGetCardFields......... %d-%d ", srTransRec.byEntryMode,fEntryCardfromIDLE);



    //vdDebug_LogPrintf("inCTOS_WaveGetCardFields......... %s-%d ", srTransRec.szPAN,fIdleSwipe);

    srTransRec.fPrintSMCardHolder = FALSE;
    srTransRec.fPrintCardHolderBal = FALSE;
    if ((strlen(srTransRec.szPAN) > 0) && (fIdleSwipe == TRUE || fIdleInsert == TRUE))
        return d_OK;



    /* BDO CLG: MOTO setup - start -- jzg */
    if (strTCT.fMOTO == 1)
    {
 		 vdDebug_LogPrintf("saturn moto");

        //CTOS_LCDTClearDisplay();
        //Tine:  add SWIPE/INSERT CARD display 08Mar2019
        //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");

        vdDispTransTitle(srTransRec.byTransType);
        //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
        CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

        while (1)
        {


            //CTOS_LCDTPrintXY(1, 3, "Enter/Cancel");

            if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            {
                fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
                return READ_CARD_TIMEOUT ;
            }

            CTOS_KBDInKey(&byKeyBuf);
            if (byKeyBuf)
            {
                CTOS_KBDGet(&byKeyBuf);
                switch(byKeyBuf)
                {
                    case d_KBD_0:
                    case d_KBD_1:
                    case d_KBD_2:
                    case d_KBD_3:
                    case d_KBD_4:
                    case d_KBD_5:
                    case d_KBD_6:
                    case d_KBD_7:
                    case d_KBD_8:
                    case d_KBD_9:
                    case d_KBD_CANCEL:
                        if (byKeyBuf == d_KBD_CANCEL)
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }


                        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

                        vdSetFirstIdleKey(byKeyBuf);
                        vdDebug_LogPrintf("szPAN2[%s]", srTransRec.szPAN);

                        //get the card number and ger Expire Date
                        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
                        {
                            vdSetFirstIdleKey(0x00);
                            CTOS_KBDBufFlush ();
                            return USER_ABORT;
                        }

                        inMOTOResult = inCTOS_LoadCDTIndex();
                        //Load the CDT table
                        if (d_OK != inMOTOResult)
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }

                        if(strCDT.fManEntry == FALSE)
                        {
                            //vdDisplayErrorMsgResp2(" ", " ", "MKE NOT ALLOWED");
                            CTOS_KBDBufFlush();
                            break;
                            //return USER_ABORT;
                        }


                        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);

                        if(inMOTOResult == d_OK)
                            return d_OK;

                        break;
                }
            }

        }

        return d_OK;

    }
    /* BDO CLG: MOTO setup - end -- jzg */


    if (fIdleInsert == TRUE){

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if (!(bySC_status & d_MK_SC_PRESENT)){
            fEntryCardfromIDLE = FALSE;
            fIdleInsert=FALSE;
            srTransRec.byEntryMode = 0;
        }
    }

    inEntryMode = READ_ALL;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    //CTOS_LCDTClearDisplay();

    // patrick fix code 20141222 case 179
    if (fEntryCardfromIDLE != TRUE)
    {
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        fClearScreen=TRUE;
    }

	
	vdDebug_LogPrintf("saturn fEntryCardfromIDLE %d", fEntryCardfromIDLE);

    ing_KeyPressed = 0;

    SWIPE_AGAIN:
    vdDebug_LogPrintf("saturn entry mode %d", inEntryMode);

    EntryOfStartTrans:
    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        //CTOS_LCDTClearDisplay();
        //vdDispTransTitle(srTransRec.byTransType); //Issue 660 Removed flashing Header
        //gcitra-0728
        //inCTOS_DisplayIdleBMP();
        //gcitra-0728
    }

    memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
    memset(szOtherAmt,0x00,sizeof(szOtherAmt));
    memset(szTransType,0x00,sizeof(szTransType));
    memset(szCatgCode,0x00,sizeof(szCatgCode));
    memset(szCurrCode,0x00,sizeof(szCurrCode));
    memset(temp,0x00,sizeof(temp));
    memset(szTotalAmount,0x00,sizeof(szTotalAmount));
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    //vdDebug_LogPrintf("szTotalAmount[%s].strTCT.szCTLSLimit=[%s]........",szTotalAmount,strTCT.szCTLSLimit);

//gcitra
#if 0
    CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	CTOS_LCDTPrintXY(1, 3, "    Present Card   ");
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
	sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
	CTOS_LCDTPrintXY(1, 4, temp);
#endif

    //CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - start -- jzg
    memset(szBaseAmount, 0x00, sizeof(szBaseAmount));
    memset(szTipAmount, 0x00, sizeof(szTipAmount));
    wub_hex_2_str(srTransRec.szTipAmount, szTipAmount, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);
    // patrick fix code 20141216
    sprintf(szTotalAmount, "%012.0f", atof(szBaseAmount) + atof(szTipAmount));
    wub_str_2_hex(szTotalAmount, srTransRec.szTotalAmount, 12);
//	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6); //CTLS - Fix for CTLS reader not accepting PayPass Cards  -- jzg
    //CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - end -- jzg

    if (srTransRec.byTransType == REFUND)
        szTransType[0] = 0x20;
    //CTLS - Fix for CTLS reader not accepting PayPass Cards - start  -- jzg
    sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
    strcpy(szCurrCode, szCatgCode);
    //CTLS - Fix for CTLS reader not accepting PayPass Cards  - end -- jzg


    /* BDO: Revised amount comparison to accomodate large values - start -- jzg */
    if (strcmp(szTotalAmount, strTCT.szMaxCTLSAmount) >= 0)
        fMaxCTLSAmt = TRUE;
    else
        fMaxCTLSAmt = FALSE;
    /* BDO: Revised amount comparison to accomodate large values - end -- jzg */


    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
    if ((inEntryMode == READ_ALL) && (!fMaxCTLSAmt))
//	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR())
        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        {
            ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
            if(ulAPRtn != d_EMVCL_NO_ERROR)
            {
                vdSetErrorMessage("CTLS InitTrans Fail!");
                return d_NO;
            }



        }
    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- jzg */


    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);

    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

    //gcitra-0728
    //vdDispTransTitle(srTransRec.byTransType);//Issue 660 Removed flashing Header

    if (!fBINVer)

	
	vdDebug_LogPrintf("saturn !binver entrymode %d", inEntryMode);
        if (inEntryMode  == SWIPE_ONLY)
        {
            //memset(szTitle, 0x00, sizeof(szTitle));
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "PLEASE SWIPE CUSTOMER CARD");
			//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "CARD ENTRY");
			//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "0");
            //usCTOSS_LCDDisplay(szDisMsg);

			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "Please swipe customer card");
			strcat(szDisMsg, "|");
			strcat(szDisMsg, strCST.szCurSymbol);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, szAmtMsg);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "0");
	
            if(swipe_loopcount == 0)
            {
                ing_KeyPressed = 0;
                inKey = usCARDENTRY(szDisMsg);
            }
            //onFailedCtlsFallback(szDisMsg);
            //vdDispTransTitle(srTransRec.byTransType);
            //CTOS_LCDTPrintXY(1, 3, "Please Swipe");
            //CTOS_LCDTPrintXY(1, 4, "Customer Card");

            swipe_loopcount = 1;
        }
            //aaalcantara issue number 000113 2 of 3 START
        else if (inchipswiped == 1)
        {
            //vdDispTransTitle(srTransRec.byTransType);
            //CTOS_LCDTPrintXY(1, 3, "              ");
            //CTOS_LCDTPrintXY(1, 4, "              ");
            inchipswiped = 0;
			//memset(szTitle, 0x00, sizeof(szTitle));
            //szGetTransTitle(srTransRec.byTransType, szTitle);
			//memset(szDisMsg, 0x00, sizeof(szDisMsg));
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "PLEASE SWIPE CUSTOMER CARD");
			//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "CARD ENTRY");
			//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "0");

			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "Please swipe customer card");
			strcat(szDisMsg, "|");
			strcat(szDisMsg, strCST.szCurSymbol);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, szAmtMsg);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "0");
			inKey = usCARDENTRY(szDisMsg);
			
        }
            //aaalcantara issue number 000113 2 of 3 END
        else
        {
#if 1
            //issue:218
            if (fEntryCardfromIDLE != TRUE)
            {

			vdDebug_LogPrintf("saturn fEntryCardfromIDLE not true");
                if(strTCT.fSMMode == TRUE)
                {
                    vdDispTransTitle(srTransRec.byTransType);

                    if(strTCT.fEnableManualKeyEntry == TRUE)
                    {
                        CTOS_LCDTPrintXY(1, 3, "TAP/INSERT/SWIPE");
                        CTOS_LCDTPrintXY(1, 4, "ENTER CARD");
                    }
                    else
                    {
                        CTOS_LCDTPrintXY(1, 3, "TAP/INSERT/SWIPE");
                        CTOS_LCDTPrintXY(1, 4, "CARD");
                    }
                }
            }
#else
            /// -- do nothing
			  // SHARLS_CTLS already display this message  -- sidumili.
#endif
        }
    //gcitra-0728


    //0826
    INSERT_AGAIN:
    //0826
    //ing_KeyPressed = 0;
    while (1)
    {
#if 0
        inKey=usCARDENTRY(szDisMsg);			//Tine:  28MAY2019
        if(inKey==d_USER_CANCEL)
        {
            return inKey;
        }

#endif
		//vdDebug_LogPrintf("inCTOS_WaveGetCardFields, ing_KeyPressed[%d]", ing_KeyPressed);

        if (ing_KeyPressed == 'C')
        {
            CTOS_KBDBufPut('C');
            vdDebug_LogPrintf("saturn putchar C");
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisplay, szTitle);
            //strcat(szDisplay, "|");
            //strcat(szDisplay, "USER CANCEL");
            //usCTOSS_LCDDisplay(szDisplay);
            //vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
            CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
            //CTOS_Delay(1500);
            return USER_ABORT;
        }
        else if (ing_KeyPressed == 'T')
        {
            CTOS_KBDBufPut('T');
            vdDebug_LogPrintf("saturn putchar T");
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisplay, szTitle);
            //strcat(szDisplay, "|");
            //strcat(szDisplay, "TIME OUT");
            //usCTOSS_LCDDisplay(szDisplay);
            //vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
            CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
            //CTOS_Delay(1500);
            return READ_CARD_TIMEOUT ;
        }
		else if (ing_KeyPressed == 'M')     //Manual Entry
	{
		CTOS_KBDBufPut('M');
		vdDebug_LogPrintf("saturn putchar M");
        inCTOSS_CLMCancelTransaction();
        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

		usCTOSS_LCDDisplay(" ");

        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
        {
            vdSetFirstIdleKey(0x00);
            CTOS_KBDBufFlush ();
            //vdSetErrorMessage("Get Card Fail M");
            return USER_ABORT;
        }

        //Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

		if(strCDT.fManEntry == FALSE)
        {
             if (strTCT.fSMMode==FALSE)
               	vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
             else
                vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");

             CTOS_KBDBufFlush();
             return USER_ABORT;
         }

        break;
	}

	//vdDebug_LogPrintf("saturn check ing_KeyPressed");
#if 0
        if(CTOS_TimeOutCheck(TIMER_ID_1)  == d_YES)
        {
            fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();
			
			szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisplay, szTitle);
            strcat(szDisplay, "|");
            strcat(szDisplay, "TIME OUT");
            usCTOSS_LCDDisplay(szDisplay);
            CTOS_Beep();
            CTOS_Delay(1500);
            return READ_CARD_TIMEOUT ;
        }

#endif
#if 0		
        //CTOS_KBDInKey(&byKeyBuf);
        // patrick add code 20141209
        if (byKeyBuf)
        {
            vdDebug_LogPrintf("saturn manual entry 2");
            //CTOS_KBDGet(&byKeyBuf);
            switch(byKeyBuf)
            {
                case d_KBD_0:
                case d_KBD_1:
                case d_KBD_2:
                case d_KBD_3:
                case d_KBD_4:
                case d_KBD_5:
                case d_KBD_6:
                case d_KBD_7:
                case d_KBD_8:
                case d_KBD_9:

                    //fEnableBinVerMKE = MKE for BIN VER
                    if(srTransRec.byTransType == BIN_VER)
                    {
                        if ((strTCT.fEnableBinVerMKE == FALSE) && (strTCT.fSMMode==FALSE))
                        {
                            vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                            //return USER_ABORT;
                        }
                        else if ((strTCT.fEnableBinVerMKE == FALSE) && (strTCT.fSMMode==TRUE))
                        {
                            CTOS_KBDBufFlush();
                            break;
                        }
                    }
                        //fEnableInstMKE = MKE General flag for installment
                    else if (fInstApp == TRUE)
                    {
                        if ((strTCT.fEnableInstMKE == FALSE) && (strTCT.fSMMode==FALSE))
                        {
                            //vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");
                            vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                            //return USER_ABORT;
                        }
                        else if ((strTCT.fEnableInstMKE == FALSE) && (strTCT.fSMMode==TRUE))
                        {
                            CTOS_KBDBufFlush();
                            break;
                        }
                        //fEnableManualKeyEntry - Flag for all other hosts
                    }
                    else if(inFallbackToMSR == SUCCESS) //aaronnino for BDOCLG ver 9.0 fix on issue #00487 Able to press keypad for MKE upon re-inserting card for fallback transactions
                    {
                        //vdDisplayErrorMsgResp2(" ", " ", "MKE NOT ALLOWED");
                        CTOS_KBDBufFlush();
                        break;
                        //return USER_ABORT;
                    }
                    else if ((strTCT.fEnableManualKeyEntry == FALSE) && (strTCT.fSMMode==FALSE))
                    {
                        //vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");
                        vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                    }
                    else if ((strTCT.fEnableManualKeyEntry == FALSE) && (strTCT.fSMMode==TRUE))
                    {
                        CTOS_KBDBufFlush();
                        break;
                    }


                case d_KBD_CANCEL:
                    if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                        inCTOSS_CLMCancelTransaction();

                    //gcitra-0728
                    if (byKeyBuf == d_KBD_CANCEL)
                    {
                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                    }
                    //gcitra-0728

                    vdSetFirstIdleKey(byKeyBuf);

                    memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
                    //gcitra
                    //if(d_OK == inCTOS_ValidFirstIdleKey())
                    //       srTransRec.szPAN[0] = chGetFirstIdleKey();
                    //gcitra

                    vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
                    //get the card number and ger Expire Date
                    if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
                    {
                        vdSetFirstIdleKey(0x00);
                        CTOS_KBDBufFlush ();
                        //vdSetErrorMessage("Get Card Fail M");
                        return USER_ABORT;
                    }

                    //Load the CDT table
                    if (d_OK != inCTOS_LoadCDTIndex())
                    {
                        //gcitra-120214
                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                        //gcitra-120214
                    }

                    if(strCDT.fManEntry == FALSE)
                    {
                        if (strTCT.fSMMode==FALSE)
                            vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                        else
                            vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");

                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                    }

                    break;
            }
        }
#endif

		//vdDebug_LogPrintf("saturn entrymode=[%d]", inEntryMode);
		//vdDebug_LogPrintf("inFallbackToMSR=[%d]", inFallbackToMSR);

        if (inEntryMode != SWIPE_ONLY)
        {
//INSERT_AGAIN:

            if (inEntryMode == INSERT_ONLY)
            {
                byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
                vdCTOS_ResetMagstripCardData();
                //vdDispTransTitle(srTransRec.byTransType);
                //CTOS_LCDTPrintXY(1, 4, "PLEASE INSERT CARD/");
                //CTOS_LCDTPrintXY(1, 5, "CANCEL");
                //memset(szTitle, 0x00, sizeof(szTitle));
                //szGetTransTitle(srTransRec.byTransType, szTitle);
                //strcpy(szDisMsg, szTitle);
                //strcat(szDisMsg, "|");
                //strcat(szDisMsg, "PLEASE INSERT CARD");
				//strcat(szDisMsg, "|");
	            //strcat(szDisMsg, "CARD ENTRY");
				//strcat(szDisMsg, "|");
	            //strcat(szDisMsg, "0");
                //usCTOSS_LCDDisplay(szDisMsg);

				memset(szDisMsg, 0x00, sizeof(szDisMsg));
				strcpy(szDisMsg, szTitle);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "Please insert card");
				strcat(szDisMsg, "|");
				strcat(szDisMsg, strCST.szCurSymbol);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, szAmtMsg);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "0");
                if(insert_loopcount == 0)
                {
                    //ing_KeyPressed = 0;
                    inKey = usCARDENTRY(szDisMsg);
                }
                insert_loopcount = 1;

            }

//0826

            CTOS_SCStatus(d_SC_USER, &bySC_status);
            if(bySC_status & d_MK_SC_PRESENT)
            {
                vdDebug_LogPrintf("saturn chip entry");
                inCTOSS_CLMCancelTransaction();
                //clearLine(8);
                vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

                //vdDebug_LogPrintf("--EMV flow----" );
                if (fClearScreen == TRUE && srTransRec.byTransType != BALANCE_INQUIRY)
                {
                    //vdDispTransTitle(srTransRec.byTransType);
                    //vdClearBelowLine(2);
                    //strcpy(szDisMsg, szTitle);
                    //strcat(szDisMsg, "|");
                    //strcat(szDisMsg, "PROCESSING...");
                    //usCTOSS_LCDDisplay(szDisMsg);        //Tine:  24Apr2019
				
                    vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
					//CTOS_Delay(10);
                }
                shReturn = inCTOS_EMVCardReadProcess();




                //if (d_OK != inCTOS_EMVCardReadProcess ())
                if (shReturn != d_OK)
                {
                    vdDebug_LogPrintf("-------inCTOS_EMVCardReadProcess---[%d]-- %d %d",shReturn, inFallbackToMSR, inChipTries);

                    if ((shReturn==EMV_TRANS_NOT_ALLOWED) || (shReturn == EMV_FAILURE_EX) || (shReturn == EMV_APPL_NOT_AVAILABLE))
                    {
                        vdDebug_LogPrintf("-------inCTOS_EMVCardReadProcess 3---[%d]-- %d %d",shReturn, inFallbackToMSR, inChipTries);
                        vduiClearBelow(2);
                        vdCTOS_ResetMagstripCardData();
                        vdRemoveCard();
                        inEntryMode = SWIPE_ONLY;
                        if(shReturn == EMV_APPL_NOT_AVAILABLE)
                        {
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
                            inFallbackToMSR = FAIL;
                            fApplNotAvailable = TRUE;
                        }
                        else
                        {
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                            inFallbackToMSR = SUCCESS;
                        }

                        //vduiClearBelow(2);
                        goto SWIPE_AGAIN;
                    }

                    if(inFallbackToMSR == SUCCESS)
                    {

                        vdDebug_LogPrintf("-------inCTOS_EMVCardReadProcess 2---[%d]-- %d %d",shReturn, inFallbackToMSR, inChipTries);
                        vdCTOS_ResetMagstripCardData();
                        //0826
                        //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                        //goto SWIPE_AGAIN;
                        vduiClearBelow(2);
                        vdRemoveCard();
                        clearLine(7);
                        inChipTries= inChipTries+1;
                        if (inChipTries < 3)
                        {
                            inEntryMode = INSERT_ONLY;
                            insert_loopcount = 0;
                            goto INSERT_AGAIN;
                        }
                        else
                        {
                            inEntryMode = SWIPE_ONLY;

                            //1125
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                            //1125
                            goto SWIPE_AGAIN;
                        }
                        //0826
                    }
                    else
                    {
                        //vdSetErrorMessage("Get Card Fail C");
                        return USER_ABORT;
                    }
                }

                //vdDebug_LogPrintf("--EMV Read succ----" );
                //Load the CDT table
                if (d_OK != inCTOS_LoadCDTIndex())
                {
                    CTOS_KBDBufFlush();
                    return USER_ABORT;
                }

                break;
            }


            if (inEntryMode == INSERT_ONLY)
                goto INSERT_AGAIN;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
        {
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail");
                return USER_ABORT;
            }

            if(d_OK != inCTOS_CheckEMVFallback())
            {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "3.PLS INSERT CARD");

                //issue#11
                vduiClearBelow(2);
                inEntryMode = INSERT_ONLY;
                goto INSERT_AGAIN;
                //goto SWIPE_AGAIN;
                //issue#11

            }

            break;

        }

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

        //vdDebug_LogPrintf("msr status %d", byMSR_status);

        /* BDOCLG-00187: Return to idle if card is incorrectly swipe at idle screen - start -- jzg */
        if(((byMSR_status != d_OK) && (fEntryCardfromIDLE == TRUE) && (inEntryMode == READ_ALL)) || (byMSR_status == 0x02))
        {
            inCTOSS_CLMCancelTransaction();
            //vduiClearBelow(1);
            //vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
            //Tine:  23MAY2019
            //memset(szTitle, 0x00, sizeof(szTitle));
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "CARD READ ERROR");
            //usCTOSS_LCDDisplay(szDisMsg);
            //CTOS_Beep();
            //CTOS_Delay(1500);
            //CTOS_Beep();
            vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
            return INVALID_CARD;
        }
        /* BDOCLG-00187: Return to idle if card is incorrectly swipe at idle screen - end -- jzg */

        //gcitra
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
            //gcitra

            //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        {
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();


            vdDebug_LogPrintf("mag entry");

            //Invalid card reading fix - start -- jzg
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

            if (shReturn == INVALID_CARD)
            {
                CTOS_KBDBufFlush();
                vduiClearBelow(1);
                vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
                return INVALID_CARD;
            }
            //Invalid card reading fix - end -- jzg

            //1125
            if(inFallbackToMSR == SUCCESS){
                vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
            }
            //1125

            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            if(d_OK != inCTOS_CheckEMVFallback())
            {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "4.PLS INSERT CARD");
                inchipswiped = 1; //aaalcantara issue number 000113 3 of  3

                //issue#11
                vduiClearBelow(2);
                inEntryMode = INSERT_ONLY;
                goto INSERT_AGAIN;
                //goto SWIPE_AGAIN;
                //issue#11

            }

            break;
        }



        if(inEntryMode == SWIPE_ONLY){
            //inCTOSS_CLMCancelTransaction();
            goto SWIPE_AGAIN;
        }

        /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
//		if(!fMaxCTLSAmt)
        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        {
            ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);

            // V3 contactless reader
//		EMVCL_StopIdleLEDBehavior(NULL);
//		EMVCL_SetLED(0x0F, 0x08);

// patrick test code 20141230 start
#define d_EMVCL_RC_SEE_PHONE		0xA00000AF
// patrick test code 20141230 end

            if(ulAPRtn == d_EMVCL_RC_DEK_SIGNAL)
            {
                vdDebug_LogPrintf("saturn DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen,stRCDataEx.baChipData);
            }
                //EMV: Added error message handling "PLEASE SEE PHONE" - start -- jzg
            else if(ulAPRtn == d_EMVCL_RC_SEE_PHONE)
            {
                vdDisplayErrorMsg(1, 8, "PLEASE SEE PHONE");
                CTOS_Delay(3000);
                vdDisplayErrorMsg(1, 8, "                ");
                goto SWIPE_AGAIN;
            }
                //EMV: Added error message handling "PLEASE SEE PHONE" - end -- jzg
            else if(ulAPRtn == d_EMVCL_TX_CANCEL)
            {
                vdDisplayErrorMsg(1, 8, "USER CANCEL");
                return USER_ABORT;
            }
            else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
            {
                if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                    inCTOSS_CLMCancelTransaction();

                //CTOS_Beep();
                //CTOS_Delay(50);
                //CTOS_Beep();
                //CTOS_Delay(50);
                vdDisplayErrorMsg(1, 8, "TIMEOUT");
                fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
                return USER_ABORT;
            }
            else if((ulAPRtn == d_EMVCL_RC_NO_AP_FOUND) || (ulAPRtn == d_EMVCL_RSP_ID_ERROR))
            {
                if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                    inCTOSS_CLMCancelTransaction();

                //CTOS_Beep();
                //CTOS_Delay(50);
                //CTOS_Beep();
                //CTOS_Delay(50);
                CTOS_LCDTClearDisplay();
                setLCDPrint(3, DISPLAY_POSITION_CENTER, "CTLS TRANSACTION");
                setLCDPrint(4, DISPLAY_POSITION_CENTER, "LIMIT EXCEEDED PLS");
                setLCDPrint(5, DISPLAY_POSITION_CENTER, "SWIPE/ INSERT CARD");
                CTOS_Delay(1500);

                return USER_ABORT;
            }
            else if(ulAPRtn != d_EMVCL_PENDING)
            {
                //CTOS_Beep();
                //CTOS_Delay(50);
                //CTOS_Beep();
                //CTOS_Delay(50);
                //CTOS_Beep();
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                break;
            }
        }
        else
        {
			if(fCTLSEnable == VS_FALSE)
                continue;
			
            memset(szTransType,0x00,sizeof(szTransType));
            strcpy(szOtherAmt,"000000000000");
            if (srTransRec.byTransType == REFUND)
                strcpy(szTransType,"20");
            else
                strcpy(szTransType,"00");
            ulAPRtn = usCTOSS_CtlsV3Trans(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode,&stRCDataEx);
			 vdDebug_LogPrintf("saturn test 3 %d", ulAPRtn);
            //CTOS_LCDTClearDisplay();
            //CTOS_Beep();
            //CTOS_Delay(50);
            //CTOS_Beep();
            //CTOS_Delay(50);
            //CTOS_Beep();
            if(ulAPRtn == d_OK)
            {
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                ulAPRtn = d_EMVCL_RC_DATA;
                break;
            }
#if 0
            if (ulAPRtn == d_NO)
            {
                memset(temp,0x00,sizeof(temp));
                inCTOSS_GetEnvDB("CTLSRESP", temp);
                ulAPRtn = atof(temp);
                vdDebug_LogPrintf("CTLSRESP[%s],ulAPRtn=[%x]", temp,ulAPRtn);

                /*BDO: Timeout --sidumili*/
                if (ulAPRtn == d_EMVCL_RC_NO_CARD || ing_KeyPressed == 'T')
                {
                    fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
                    return USER_ABORT;// should return if no card is detected.
                }
                /*BDO: Timeout --sidumili*/

                /*BDO: Separate error for CTLS read card error --sidumili*/
                if (ulAPRtn == d_EMVCL_RC_FAILURE)
                {

                    memset(szBaseAmt, 0x00, sizeof(szBaseAmt));
                    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmt, AMT_BCD_SIZE);

                    vdDebug_LogPrintf("saturn CTLS TEST %s - %s",szBaseAmt,strTCT.szCTLSLimit);

                    vdDebug_LogPrintf("saturn CTLS TEST2 %f - %f",atof(szBaseAmt),atof(strTCT.szCTLSLimit));

                    //temporary fix


                    if (atof(szBaseAmt) > atof(strTCT.szCTLSLimit))
                    {
                        //setLCDPrint(3, DISPLAY_POSITION_CENTER, "CTLS TRANSACTION");
                        //setLCDPrint(4, DISPLAY_POSITION_CENTER, "LIMIT EXCEEDED PLS");
                        //setLCDPrint(5, DISPLAY_POSITION_CENTER, "SWIPE/INSERT CARD");
                        //CTOS_Delay(1500);
                        inGblCtlsErr = 1;

                    }else{

                        //end temporary fix
                        //setLCDPrint(3, DISPLAY_POSITION_CENTER, "CLTS READ CARD ERROR");
                        //setLCDPrint(4, DISPLAY_POSITION_CENTER, "PLEASE SWIPE/");
                        //setLCDPrint(5, DISPLAY_POSITION_CENTER, "INSERT CARD");
                        //CTOS_Delay(1500);
                        inGblCtlsErr = 2;
                    }
                }
                /*BDO: Separate error for CTLS read card error --sidumili*/

                if(ulAPRtn == d_EMVCL_RC_EXCEED_OR_EQUAL_CL_TX_LIMIT)
                {
                    //setLCDPrint(3, DISPLAY_POSITION_CENTER, "CTLS TRANSACTION");
                    //setLCDPrint(4, DISPLAY_POSITION_CENTER, "LIMIT EXCEEDED PLS");
                    //setLCDPrint(5, DISPLAY_POSITION_CENTER, "SWIPE/INSERT CARD");
                    //CTOS_Delay(1500);
                    inGblCtlsErr = 3;
                }
                return (inCTOS_GetCardFieldsFailedCtlsFallback());
            }
#else
            else if (ulAPRtn == d_NO)
            {
                memset(temp,0x00,sizeof(temp));
                inCTOSS_GetEnvDB("CTLSRESP", temp);
                ulAPRtn = atof(temp);
                vdDebug_LogPrintf("saturn test CTLSRESP[%s],ulAPRtn=[%x]", temp,ulAPRtn);

			    if(ulAPRtn == d_EMVCL_RC_FALLBACK)
                    vdDisplayErrorMsg("CTLS ERROR PLEASE", "INSERT/SWIPE CARD", MSG_TYPE_ERROR);
                else if (ulAPRtn == d_EMVCL_RC_FAILURE)
                    vdDisplayErrorMsg("CTLS CARD READ ERROR", "PLS INSERT CARD", MSG_TYPE_ERROR);
                else if(ulAPRtn == d_EMVCL_RC_EXCEED_OR_EQUAL_CL_TX_LIMIT)
                    vdDisplayErrorMsg("CTLS TRANSACTION", "LIMIT EXCEEDED PLS", "INSERT/SWIPE CARD");
                else if(ulAPRtn == d_EMVCL_RC_SEE_PHONE)
                    vdDisplayErrorMsg("","", "PLEASE SEE PHONE");
	      	
                if(ing_KeyPressed == 'T' || CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES) {
					vdDisplayMessageBox(1, 8 , "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
					CTOS_Beep();
					//CTOS_Delay(300);
                } else if(ing_KeyPressed == 'D') {
                	//vdDisplayMessageBox(1, 8,  "", "Please try again", "", MSG_TYPE_ERROR);
					CTOS_Beep();
					//CTOS_Delay(300);
					//CTOS_Delay(1500);
                } else{
                	vdDebug_LogPrintf("saturn ctls CARD READ ERROR");
                    //vdDisplayErrorMsg("CTLS READ CARD ERROR", "PLS INSERT CARD", MSG_TYPE_ERROR);
                    vdDisplayMessageBox(1, 8 , "", "CTLS CARD READ ERROR", "", MSG_TYPE_TIMEOUT);
				}

					vdDebug_LogPrintf("saturn user abort");
                return USER_ABORT;
            }

#endif




            if(srTransRec.byEntryMode == CARD_ENTRY_WAVE)
            {
                if (ulAPRtn == d_MORE_PROCESSING || ulAPRtn == d_CTLS_EVENT)
                    return (inCTOS_GetCardFields());
            }
        }

    }

    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime -- jzg */
    if ((srTransRec.byEntryMode == CARD_ENTRY_WAVE) && (!fMaxCTLSAmt))
    {


        // add to still continue with mag stripe once ctls card was read partially
        if((ulAPRtn == d_EMVCL_RC_DDA_AUTH_FAILURE) || (ulAPRtn == d_EMVCL_RC_FAILURE))
            goto SWIPE_AGAIN;



        if(ulAPRtn != d_EMVCL_RC_DATA)
        {
            vdCTOSS_WaveCheckRtCode(ulAPRtn);
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            return d_NO;
        }

        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        {
            if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx))
            {
                inCTOSS_CLMCancelTransaction();
                return d_NO;
            }
        }
        else
        {
            inRet2 = inCTOSS_V3AnalyzeTransaction(&stRCDataEx);
            if (inRet2 != d_OK)
            {
                return CTLS_ANALYZE_ERROR;
            }
        }

        //Load the CDT table
        inRet2 = inCTOS_LoadCDTIndex();
        if (d_OK != inRet2)
        {
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            CTOS_KBDBufFlush();

            if(inRet2 == CTLS_EXPIRED_CARD)
                return inRet2;
            else
                return USER_ABORT;
        }



#if 0
        //check if CARD is DEBIT CARD - must Swipe AGAIN if card entry is CTLS
		 if (strCDT.HDTid == 5 && strCDT.fPANCatchAll == TRUE){
		 	inCTOSS_CLMCancelTransaction();
		 	vdCTOS_ResetMagstripCardData();
			vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
			inEntryMode = SWIPE_ONLY;
			vduiClearBelow(2);
			memset(&stRCDataAnalyze,0x00,sizeof(EMVCL_RC_DATA_ANALYZE));
			srTransRec.byUploaded = CN_FALSE;
			srTransRec.byOffline = CN_TRUE;
			if (srTransRec.byTransType == SALE_OFFLINE){
				vdCTOS_SetTransType(SALE);
				vdDispTransTitle(SALE);
			}
			 goto SWIPE_AGAIN;
		 }
		 //end
#endif
        //Enhancement to prompt user to PLEASE INSERT/SWIPE CARD if CTLS Debit card is tapped
        if (strTCT.fATPBinRoute == 0 &&
            //(strCDT.HDTid == DEBIT_HDT_INDEX ||  strCDT.HDTid == DEBIT2_HDT_INDEX || strCDT.HDTid == FLEET_HDT_INDEX || strCDT.HDTid == DINERS_HDT_INDEX
            //diners emv
            (strCDT.HDTid == DEBIT_HDT_INDEX ||	strCDT.HDTid == DEBIT2_HDT_INDEX || strCDT.HDTid == FLEET_HDT_INDEX

             || (strCDT.fDCCEnable == TRUE && inFLGGet("fDCCCTLS") == FALSE)))
        {
            vdResetAllCardData();
            fNoCTLSSupportforBinRoute = TRUE;
            return (inCTOS_GetCardFieldsFailedCtlsFallback());

        }

        //Chekck if CARD is PAYPASS MAG and compare with Floor Limit
        vdDebug_LogPrintf("**** inCTOSS_WaveAnalyzeTransaction 2 ****");

        if (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MAG_STRIPE){

            if (fAmountLessThanFloorLimit() == d_OK)
                srTransRec.bWaveSCVMAnalysis = d_CVM_REQUIRED_NONE;
            else
                srTransRec.bWaveSCVMAnalysis = d_CVM_REQUIRED_SIGNATURE;
            vdDebug_LogPrintf("stRCDataAnalyze.bCVMAnalysis 2 %d %x",srTransRec.bWaveSCVMAnalysis, srTransRec.bWaveSCVMAnalysis);
        }


        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())

            inCTOSS_CLMCancelTransaction();
    }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        vdDebug_LogPrintf("inCTOS_WaveGetCardFields, CARD_ENTRY_ICC");		//Tine:  07MAY2019
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);

	if (srTransRec.byEntryMode != CARD_ENTRY_WAVE)
		CTOS_Beep(); // add beep sound after pass on CDT validation -- sidumili
	
    return d_OK;
}

#else
int inCTOS_GetCardFields(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf = '0';
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES_50], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES_50 ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
    BOOL fManualFlag = TRUE, fClearScreen=FALSE;

    BYTE szTitle[20+1];
    BYTE szDisMsg[100];

    BYTE szDisplay[200];
	int swipe_loopcount = 0;
    int insert_loopcount = 0;

    CTOS_RTC SetRTC;

    //0826
    int inChipTries=0;
    int inEntryMode=0;
    /*
	1= insert only
	2= swipe only
	0= will accept al
	*/

#define INSERT_ONLY 1
#define SWIPE_ONLY  2
#define READ_ALL 0
    //0826

    short shReturn = d_OK; //Invalid card reading fix -- jzg

    /* BDO CLG: MOTO setup -- jzg */
    int inMOTOResult;
    int inKey=0;

	BYTE szTotalAmt[12+1];
	BYTE szAmtMsg[45 + 1];
	BYTE szManualEntryFlag[2];

    fApplNotAvailable = FALSE;

    vdDebug_LogPrintf("saturn inCTOS_GetCardFields...");
	vdDebug_LogPrintf("srTransRec.byTransType=[%d]", srTransRec.byTransType);
	
    srTransRec.fPrintSMCardHolder = FALSE;
    srTransRec.fPrintCardHolderBal = FALSE;

#if 1
	if ((strlen(srTransRec.szPAN) > 0) && (fIdleSwipe == TRUE || fIdleInsert == TRUE))
    	return d_OK;
#else
	if ((strlen(srTransRec.szPAN) > 0) && (fIdleInsert == TRUE))
    	return d_OK;

    if ((strlen(srTransRec.szPAN) > 0) && (fIdleSwipe == TRUE))
    {
		//tine:  android - to handle idleswipe of emv card
		vdDebug_LogPrintf("idle swipe emv card fallback");
		if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail");
                return USER_ABORT;
            }

        if(d_OK != inCTOS_CheckEMVFallback())
            {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "3.PLS INSERT CARD");

                //issue#11
                vduiClearBelow(2);
                inEntryMode = INSERT_ONLY;
                goto INSERT_AGAIN;
                //goto SWIPE_AGAIN;
                //issue#11

            }
		return d_OK;
    }
#endif

	// get transaction title
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);

	// get manual entry flag
	memset(szManualEntryFlag, 0x00, sizeof(szManualEntryFlag));
	if(strTCT.fEnableManualKeyEntry == 1)
        strcpy(szManualEntryFlag, "1");
    else
        strcpy(szManualEntryFlag, "0");

	// Get Amount
	memset(szAmtMsg, 0x00, sizeof(szAmtMsg));
	memset(szTotalAmt, 0x00, sizeof(szTotalAmt));
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
	vdCTOS_FormatAmount(strCST.szAmountFormat, szTotalAmt, szAmtMsg);

    //strcpy(szDisMsg, szTitle);
    //strcat(szDisMsg, "|");
    //strcat(szDisMsg, "SWIPE/INSERT");
    //strcat(szDisMsg, "|");
    //strcat(szDisMsg, "CARD ENTRY");
	//if(fEntryCardfromIDLE ==  FALSE)
    // 	inKey=usCARDENTRY(szDisMsg);
	
    /* BDO CLG: MOTO setup - start -- jzg */
    if (strTCT.fMOTO == 1)
    {

        CTOS_LCDTClearDisplay();

        //issue#00159
        if(srTransRec.byTransType == BIN_VER)
        {
            vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
            //CTOS_KBDBufFlush();
            return USER_ABORT;
        }
        //issue00159

        vdDispTransTitle(srTransRec.byTransType);
        //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
        CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

        while (1)
        {
            CTOS_LCDTPrintXY(1, 3, "Enter/Cancel");

            if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
            {
                fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
                return READ_CARD_TIMEOUT ;
            }

            CTOS_KBDInKey(&byKeyBuf);
            if(byKeyBuf)
            {
                CTOS_KBDGet(&byKeyBuf);
                switch(byKeyBuf)
                {
                    case d_KBD_0:
                    case d_KBD_1:
                    case d_KBD_2:
                    case d_KBD_3:
                    case d_KBD_4:
                    case d_KBD_5:
                    case d_KBD_6:
                    case d_KBD_7:
                    case d_KBD_8:
                    case d_KBD_9:
                        if(srTransRec.byTransType == BALANCE_INQUIRY)
                        {
                            if (strTCT.fSMMode==FALSE)
                                vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                            else
                                vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");

                            CTOS_KBDBufFlush();
                            break;
                        }
                    case d_KBD_CANCEL:
                        if (byKeyBuf == d_KBD_CANCEL)
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }

                        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

                        vdSetFirstIdleKey(byKeyBuf);
                        vdDebug_LogPrintf("szPAN2[%s]", srTransRec.szPAN);

                        //get the card number and ger Expire Date
                        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
                        {
                            vdSetFirstIdleKey(0x00);
                            CTOS_KBDBufFlush ();
                            return USER_ABORT;
                        }

                        inMOTOResult = inCTOS_LoadCDTIndex();
                        //Load the CDT table
                        if (d_OK != inMOTOResult)
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }


                        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
                        //issue#-00159
#if 0
                    if(strCDT.fManEntry == FALSE)
						{
							vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");
							CTOS_KBDBufFlush();
							return USER_ABORT;
						}
#endif
                        //issue00159

                        return d_OK; //BDO-00160: To properly exit the function if there's a valid entry -- jzg
                        break;
                }
            }
        }

        return d_OK;

    }
    /* BDO CLG: MOTO setup - end -- jzg */

    if (fIdleInsert == TRUE){

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if (!(bySC_status & d_MK_SC_PRESENT)){
            fEntryCardfromIDLE = FALSE;
            fIdleInsert=FALSE;
            srTransRec.byEntryMode = 0;
        }
    }


    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if(fEntryCardfromIDLE != TRUE)
    {
        CTOS_LCDTClearDisplay();
        //vdClearBelowLine(2);
        vdDispTransTitle(srTransRec.byTransType);
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        fClearScreen=TRUE;
    }

    // patrick fix code 20141222 case 179
    //if (fEntryCardfromIDLE != TRUE)
    //byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

    //if(fIdleSwipe != TRUE) //aaronnino for BDOCLG ver 9.0 fix on issue #00059 Card entry is recognized even on non Card Entry Prompt or non Idle Screen display 8 of 8
    //byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

    ing_KeyPressed = 0;

    SWIPE_AGAIN:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {

        //CTOS_LCDTClearDisplay();

        /* BDO CLG: Fleet card support - start -- jzg */
        //if(srTransRec.fFleetCard == TRUE)
        //	vdDispTransTitle(FLEET_SALE);
        //else
        /* BDO CLG: Fleet card support - end -- jzg */
        //vdDispTransTitle(srTransRec.byTransType);

        //gcitra-0728
        //inCTOS_DisplayIdleBMP();
        //gcitra-0728
    }
// patrick ECR 20140516 start
#if 0
    if (strTCT.fECR) // tct
    {
    	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
    	{
    		char szDisplayBuf[30] = {0};
    		BYTE szTemp1[30+1] = {0};
			BYTE szTemp2[30+1] = {0};

			// sidumili: Issue#:000076 [check transaction maximum amount]
			if (inCTOS_ValidateTrxnAmount()!= d_OK){
				return(d_NO);
			}

			CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    		//wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
    		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, 6);  /*BDO: Display the amount properly via ecr -- sidumili*/
			vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp1, szTemp2);
			sprintf(szDisplayBuf, "%s %s", strCST.szCurSymbol, szTemp2);
    		CTOS_LCDTPrintXY(1, 8, szDisplayBuf);

    	}
    }
#endif
// patrick ECR 20140516 end
    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

    //CTOS_LCDTClearDisplay();
    //vdMyEZLib_Printf("srTransRec.byEntryMode: (%02x)", srTransRec.byEntryMode);

    /* BDO CLG: Fleet card support - start -- jzg */
    //if(srTransRec.fFleetCard == TRUE)
    //	vdDispTransTitle(FLEET_SALE);
    //else
    /* BDO CLG: Fleet card support - end -- jzg */
    //vdDispTransTitle(srTransRec.byTransType);

    if (!fBINVer)
        //if(srTransRec.byEntryMode != CARD_ENTRY_MSR && srTransRec.byEntryMode != CARD_ENTRY_ICC)
        //{
        //vdDispTransTitle(srTransRec.byTransType);
        if (inEntryMode  == SWIPE_ONLY)
        {
            //vdDispTransTitle(srTransRec.byTransType);
            //CTOS_LCDTPrintXY(1, 3, "Please Swipe");
            //CTOS_LCDTPrintXY(1, 4, "Customer Card");
			
			//memset(szTitle, 0x00, sizeof(szTitle));
			//szGetTransTitle(srTransRec.byTransType, szTitle);			
			//memset(szDisMsg, 0x00, sizeof(szDisMsg));
			//strcpy(szDisMsg, szTitle);
			//strcat(szDisMsg, "|");
			//strcat(szDisMsg, "PLEASE SWIPE CUSTOMER CARD");
			//strcat(szDisMsg, "|");
			//strcat(szDisMsg, "CARD ENTRY");
			//strcat(szDisMsg, "|");
			//strcat(szDisMsg, "0");

			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "Please swipe customer card");
			strcat(szDisMsg, "|");
			strcat(szDisMsg, strCST.szCurSymbol);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, szAmtMsg);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "0");
			if((swipe_loopcount == 0) && (fEntryCardfromIDLE ==  FALSE))
            {
                ing_KeyPressed = 0;
                inKey = usCARDENTRY(szDisMsg);
            }
			swipe_loopcount = 1;
			
        }
        else
        {

            /* Issue# 000113 - start -- jzg */
            if (inFallbackToMSR != SUCCESS)
            {
                //BDO: Parameterized manual key entry for installment - start --jzg
                //if((fInstApp == TRUE) && (strTCT.fEnableInstMKE == FALSE))
                if (strTCT.fEnableManualKeyEntry == FALSE) //aaronnino for BDOCLG ver 9.0 fix on issue #0061 Manual Entry should not be allowed for BIN Check transactions 6 of 7
                {
                    if(fEntryCardfromIDLE == FALSE)
                    {
                        if (strTCT.fEnableManualKeyEntry == TRUE)
                        {
                            //vdDispTransTitle(srTransRec.byTransType);
                            //vdClearBelowLine(2);
                            //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/ENTER");
                            //CTOS_LCDTPrintXY(1, 4, "CARD"); //aaronnino for BDOCLG ver 9.0 fix on issue #00126 Incorrect terminal display

							//memset(szTitle, 0x00, sizeof(szTitle));
							//szGetTransTitle(srTransRec.byTransType, szTitle);							
							//memset(szDisMsg, 0x00, sizeof(szDisMsg));
							//strcpy(szDisMsg, szTitle);
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "SWIPE/INSERT/ENTER");
							//strcat(szDisMsg, "\n");
							//strcat(szDisMsg, "CARD");
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "CARD ENTRY");

							memset(szDisMsg, 0x00, sizeof(szDisMsg));
							strcpy(szDisMsg, szTitle);
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "Please swipe/insert/enter card");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, strCST.szCurSymbol);
							strcat(szDisMsg, "|");
							strcat(szDisMsg, szAmtMsg);
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "1");
			
							if(fEntryCardfromIDLE ==  FALSE)
								inKey=usCARDENTRY(szDisMsg);

                            
                        }
                        else
                        {
                            //vdDispTransTitle(srTransRec.byTransType);
                            //vdClearBelowLine(2);
                            //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");
                            
							//memset(szTitle, 0x00, sizeof(szTitle));
							//szGetTransTitle(srTransRec.byTransType, szTitle);							
							//memset(szDisMsg, 0x00, sizeof(szDisMsg));
							//strcpy(szDisMsg, szTitle);
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "SWIPE/INSERT CARD");							
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "CARD ENTRY");

							memset(szDisMsg, 0x00, sizeof(szDisMsg));
							strcpy(szDisMsg, szTitle);
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "Please swipe/insert card");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, strCST.szCurSymbol);
							strcat(szDisMsg, "|");
							strcat(szDisMsg, szAmtMsg);
							strcat(szDisMsg, "|");			
							if(fInstApp && (strTCT.fEnableInstMKE == 0))
							{
								strcat(szDisMsg, "|");
								strcat(szDisMsg, "0");  //disable Manual Entry button
							}
								
							if(fEntryCardfromIDLE ==  FALSE)
								inKey=usCARDENTRY(szDisMsg);							
                        }
                    }
                }
                else
                {

                    if (srTransRec.byTransType == BALANCE_INQUIRY)
                    {
                        //vdDispTransTitle(srTransRec.byTransType);
                        vdClearBelowLine(2);
                        if(strTCT.fEnableBalInqMKE == TRUE)
                        {
							//memset(szTitle, 0x00, sizeof(szTitle));
							//szGetTransTitle(srTransRec.byTransType, szTitle);							
							//memset(szDisMsg, 0x00, sizeof(szDisMsg));
							//strcpy(szDisMsg, szTitle);
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "");
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "");
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "");
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "1");
							memset(szDisMsg, 0x00, sizeof(szDisMsg));
							strcpy(szDisMsg, szTitle);
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "Present card");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "1");
			
							if(fEntryCardfromIDLE ==  FALSE)
								inKey=usCARDENTRY(szDisMsg);	

							
                        }
                        else
                        {
								//memset(szTitle, 0x00, sizeof(szTitle));
								//szGetTransTitle(srTransRec.byTransType, szTitle);						
								//memset(szDisMsg, 0x00, sizeof(szDisMsg));
								//strcpy(szDisMsg, szTitle);
								//strcat(szDisMsg, "|");
								//strcat(szDisMsg, "");
								//strcat(szDisMsg, "|");
								//strcat(szDisMsg, "");
								//strcat(szDisMsg, "|");
								//strcat(szDisMsg, "");
								//strcat(szDisMsg, "|");
								//strcat(szDisMsg, "0");

							memset(szDisMsg, 0x00, sizeof(szDisMsg));
							strcpy(szDisMsg, szTitle);
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "Present card");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "0");
							
							if(fEntryCardfromIDLE ==  FALSE)
								inKey=usCARDENTRY(szDisMsg);								
                        }
                            

                    }
                    else if(srTransRec.byTransType == BIN_VER)
                    {
                        //vdDispTransTitle(srTransRec.byTransType);
                        vdClearBelowLine(2);
                        if(strTCT.fEnableBinVerMKE == TRUE)
                        {

                            //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/ENTER");
                            //CTOS_LCDTPrintXY(1, 4, "CARD");
							//memset(szTitle, 0x00, sizeof(szTitle));
							//szGetTransTitle(srTransRec.byTransType, szTitle);							
							//memset(szDisMsg, 0x00, sizeof(szDisMsg));
							//strcpy(szDisMsg, szTitle);
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "SWIPE/INSERT/ENTER");
							//strcat(szDisMsg, "\n");
							//strcat(szDisMsg, "CARD");
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "CARD ENTRY");

							memset(szDisMsg, 0x00, sizeof(szDisMsg));
							strcpy(szDisMsg, szTitle);
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "Please swipe/insert/enter card");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "1");
							
							if(fEntryCardfromIDLE ==  FALSE)
								inKey=usCARDENTRY(szDisMsg);							
                        }
                        else
                        {
							//CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");
							//memset(szTitle, 0x00, sizeof(szTitle));
							//szGetTransTitle(srTransRec.byTransType, szTitle);							
							//memset(szDisMsg, 0x00, sizeof(szDisMsg));
							//strcpy(szDisMsg, szTitle);
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "SWIPE/INSERT CARD");							
							//strcat(szDisMsg, "|");
							//strcat(szDisMsg, "CARD ENTRY");

							memset(szDisMsg, 0x00, sizeof(szDisMsg));
							strcpy(szDisMsg, szTitle);
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "Please swipe/insert card");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "");
							strcat(szDisMsg, "|");
							strcat(szDisMsg, "0");
							
							if(fEntryCardfromIDLE ==  FALSE)
								inKey=usCARDENTRY(szDisMsg);
                        }
                            
                    }
                    else
                    {
                        if(fEntryCardfromIDLE == FALSE)
                        {
                            if(fInstApp == TRUE && strTCT.fEnableInstMKE == FALSE)
                            {
                                //vdDispTransTitle(srTransRec.byTransType);
                                //vdClearBelowLine(2);
                                //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");

								//memset(szTitle, 0x00, sizeof(szTitle));
							    //szGetTransTitle(srTransRec.byTransType, szTitle);
								memset(szDisMsg, 0x00, sizeof(szDisMsg));
								strcpy(szDisMsg, szTitle);
								strcat(szDisMsg, "|");
								strcat(szDisMsg, "Present card");
								strcat(szDisMsg, "|");
								strcat(szDisMsg, strCST.szCurSymbol);
								strcat(szDisMsg, "|");
								strcat(szDisMsg, szAmtMsg);
								strcat(szDisMsg, "|");
								strcat(szDisMsg, "0");
	
				                if(fEntryCardfromIDLE ==  FALSE)
				    				inKey=usCARDENTRY(szDisMsg);
                            }
                            else
                            {
                                if (strTCT.fEnableManualKeyEntry == TRUE)
                                {
                                    //vdDispTransTitle(srTransRec.byTransType);
                                    //vdClearBelowLine(2);
                                    //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/ENTER");
                                    //CTOS_LCDTPrintXY(1, 4, "CARD");

									//memset(szTitle, 0x00, sizeof(szTitle));
								    //szGetTransTitle(srTransRec.byTransType, szTitle);
									memset(szDisMsg, 0x00, sizeof(szDisMsg));
									strcpy(szDisMsg, szTitle);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "Present card");
									strcat(szDisMsg, "|");
									strcat(szDisMsg, strCST.szCurSymbol);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, szAmtMsg);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "1");
								
					                if(fEntryCardfromIDLE ==  FALSE)
					    				inKey=usCARDENTRY(szDisMsg);
                                }
                                else
                                {
                                    //vdDispTransTitle(srTransRec.byTransType);
                                    //vdClearBelowLine(2);
                                    //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");

									//memset(szTitle, 0x00, sizeof(szTitle));
								    //szGetTransTitle(srTransRec.byTransType, szTitle);
									memset(szDisMsg, 0x00, sizeof(szDisMsg));
									strcpy(szDisMsg, szTitle);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "Present card");
									strcat(szDisMsg, "|");
									strcat(szDisMsg, strCST.szCurSymbol);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, szAmtMsg);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "0");
								
					                if(fEntryCardfromIDLE ==  FALSE)
					    				inKey=usCARDENTRY(szDisMsg);
                                }
                            }
                        }
                    }
                }

                fEntryCardfromIDLE = FALSE;
            }
            /* Issue# 000113 - end -- jzg */
        }
    //}



//0826
    INSERT_AGAIN:
//0826


    while (1)
    {
		//vdDebug_LogPrintf("inCTOS_GetCardFields, ing_KeyPressed[%d]", ing_KeyPressed);
		
        if (ing_KeyPressed == 'C')
        {
            CTOS_KBDBufPut('C');
            vdDebug_LogPrintf("putchar C");
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisplay, szTitle);
            //strcat(szDisplay, "|");
            //strcat(szDisplay, "USER CANCEL");
            //usCTOSS_LCDDisplay(szDisplay);
            //vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
            CTOS_Beep();
			inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
            //CTOS_Delay(1500);
            return USER_ABORT;
        }
        else if (ing_KeyPressed == 'T')
        {
            CTOS_KBDBufPut('T');
            vdDebug_LogPrintf("putchar T");
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisplay, szTitle);
            //strcat(szDisplay, "|");
            //strcat(szDisplay, "TIME OUT");
           // usCTOSS_LCDDisplay(szDisplay);
           	//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
            CTOS_Beep();
			inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
            //CTOS_Delay(1500);
            return READ_CARD_TIMEOUT ;
        }
		else if (ing_KeyPressed == 'M')     //Manual Entry
		{
			CTOS_KBDBufPut('M');
			vdDebug_LogPrintf("putchar M");
	        inCTOSS_CLMCancelTransaction();
	        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

			usCTOSS_LCDDisplay(" ");

	        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
	        {
	            vdSetFirstIdleKey(0x00);
	            CTOS_KBDBufFlush ();
	            //vdSetErrorMessage("Get Card Fail M");
	            return USER_ABORT;
	        }

	        //Load the CDT table
	        if (d_OK != inCTOS_LoadCDTIndex())
	        {
	            CTOS_KBDBufFlush();
	            return USER_ABORT;
	        }

			if(strCDT.fManEntry == FALSE)
	        {
	             if (strTCT.fSMMode==FALSE)
	               	vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
	             else
	                vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");

	             CTOS_KBDBufFlush();
	             return USER_ABORT;
	         }

	        break;
		}

#if 0
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES){
            fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            return READ_CARD_TIMEOUT ;
        }
#endif

        /*sidumili: Issue#: 000086*/
        //enhance ecr - removed
        //if ((strTCT.fECR) &&(fECRTxnFlg)){
        //vdSetErrorMessage("ECR TIMEOUT");
        //}
        //enhance ecr - removed

        CTOS_KBDInKey(&byKeyBuf);

//gcitra-removed - remove part where card entry is allowed during IDLE mode

        // patrick add code 20141209
        if (byKeyBuf)
        {
            CTOS_KBDGet(&byKeyBuf);
            switch(byKeyBuf)
            {
                case d_KBD_0:
                case d_KBD_1:
                case d_KBD_2:
                case d_KBD_3:
                case d_KBD_4:
                case d_KBD_5:
                case d_KBD_6:
                case d_KBD_7:
                case d_KBD_8:
                case d_KBD_9:

                    //fEnableBinVerMKE = MKE for BIN VER
                    if(srTransRec.byTransType == BIN_VER)
                    {
                        if ((strTCT.fEnableBinVerMKE == FALSE) && (strTCT.fSMMode==TRUE))
                            CTOS_KBDBufFlush();
                        else if ((strTCT.fEnableBinVerMKE == FALSE) && (strTCT.fSMMode==FALSE))
                        {
                            vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }
                    }
                    else if(srTransRec.byTransType == BALANCE_INQUIRY)
                    {
                        if ((strTCT.fEnableBalInqMKE == FALSE) && (strTCT.fSMMode==TRUE))
                            CTOS_KBDBufFlush();
                        else if ((strTCT.fEnableBalInqMKE == FALSE) && (strTCT.fSMMode==FALSE))
                        {
                            vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }

                    }
                        //fEnableInstMKE = MKE General flag for installment
                    else if (fInstApp == TRUE)
                    {
                        if ((strTCT.fEnableInstMKE == FALSE) && (strTCT.fSMMode==TRUE))
                            CTOS_KBDBufFlush();
                        else if ((strTCT.fEnableInstMKE == FALSE) && (strTCT.fSMMode==FALSE))
                        {
                            vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }
                        //fEnableManualKeyEntry - Flag for all other hosts
                    }
                    else if ((strTCT.fEnableManualKeyEntry == FALSE) && (strTCT.fSMMode==FALSE))
                    {
                        //vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");
                        vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                    }
                    else if ((strTCT.fEnableManualKeyEntry == FALSE) && (strTCT.fSMMode==TRUE))
                    {
                        CTOS_KBDBufFlush();
                        break;
                    }

                case d_KBD_CANCEL:

                    //gcitra-0728
                    if (byKeyBuf == d_KBD_CANCEL)
                    {
                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                    }
                    //gcitra-0728

                    if(srTransRec.byTransType == BIN_VER && strTCT.fEnableBinVerMKE == FALSE || srTransRec.byTransType == BALANCE_INQUIRY && strTCT.fEnableBalInqMKE == FALSE)
                        fManualFlag = 0;
                    if(fInstApp == TRUE && strTCT.fEnableInstMKE == FALSE)
                        fManualFlag = 0;
                    if(inFallbackToMSR == SUCCESS)
                        fManualFlag = 0;
                    if(strTCT.fEnableManualKeyEntry == FALSE)
                        fManualFlag = 0;

                    if(fManualFlag == 1)
                    {
                        vdSetFirstIdleKey(byKeyBuf);
                        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

                        //get the card number and ger Expire Date
                        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
                        {
                            //gcitra - remove part where card entry is allowed during IDLE mode
                            //vdSetFirstIdleKey(0x00);
                            //gcitra
                            CTOS_KBDBufFlush();
                            //vdSetErrorMessage("Get Card Fail M");
                            return USER_ABORT;
                        }

                        //Load the CDT table
                        if (d_OK != inCTOS_LoadCDTIndex())
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }

                        if(fInstApp != TRUE)
                            if(strCDT.fManEntry == FALSE)
                            {
                                //vdDisplayErrorMsgResp2("MANUAL", "KEY ENTRY ", "NOT ALLOWED");
                                vdDisplayErrorMsgResp2("", "MANUAL ENTRY", "NOT ALLOWED");
                                CTOS_KBDBufFlush();
                                return USER_ABORT;
                            }

                        break;
                    }
            }
        }

//gcitra
//0826

        if (inEntryMode != SWIPE_ONLY){
//INSERT_AGAIN:

            if (inEntryMode == INSERT_ONLY){
                byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
                vdCTOS_ResetMagstripCardData();

                /* BDO CLG: Fleet card support - start -- jzg */
                //if(srTransRec.fFleetCard == TRUE)
                //	vdDispTransTitle(FLEET_SALE);
                //else
                /* BDO CLG: Fleet card support - end -- jzg */
                vdDispTransTitle(srTransRec.byTransType);

                //CTOS_LCDTPrintXY(1, 4, "PLEASE INSERT CARD/");
                //CTOS_LCDTPrintXY(1, 5, "CANCEL");
                //memset(szTitle, 0x00, sizeof(szTitle));
			    //szGetTransTitle(srTransRec.byTransType, szTitle);
				//memset(szDisMsg, 0x00, sizeof(szDisMsg));
			    //strcpy(szDisMsg, szTitle);
			    //strcat(szDisMsg, "|");
			    //strcat(szDisMsg, "PLEASE INSERT CARD");
			    //strcat(szDisMsg, "|");
			    //strcat(szDisMsg, "CARD ENTRY");
				//strcat(szDisMsg, "|");
				//strcat(szDisMsg, "0");

				memset(szDisMsg, 0x00, sizeof(szDisMsg));
				strcpy(szDisMsg, szTitle);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "Please insert card");
				strcat(szDisMsg, "|");
				strcat(szDisMsg, strCST.szCurSymbol);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, szAmtMsg);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "0");
				
			
				if((insert_loopcount == 0) && (fEntryCardfromIDLE ==  FALSE))
	            {
	                ing_KeyPressed = 0;
	                inKey = usCARDENTRY(szDisMsg);
	            }
				insert_loopcount = 1;
            }
//0826

            CTOS_SCStatus(d_SC_USER, &bySC_status);
            if(bySC_status & d_MK_SC_PRESENT)
            {
                //1010
                //CTOS_LCDTPrintXY(1,8,"");
                //1010
                //CTOS_Delay(2000);

                vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

                //vdDebug_LogPrintf("--EMV flow----" );
                //if (d_OK != inCTOS_EMVCardReadProcess ())
                if (inEntryMode == INSERT_ONLY || fClearScreen == TRUE)
                    vdClearBelowLine(2);

                    //vdDispTransTitle(srTransRec.byTransType);
                    //vdClearBelowLine(2);
                    
					memset(szTitle, 0x00, sizeof(szTitle));
					szGetTransTitle(srTransRec.byTransType, szTitle);
					
                    //strcpy(szDisMsg, szTitle);
                    //strcat(szDisMsg, "|");
                    //strcat(szDisMsg, "PROCESSING...");
                    //usCTOSS_LCDDisplay(szDisMsg);

					vdDisplayMessageBox(1, 8, "Processing", "transaction", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
					CTOS_Delay(10);
					
                shReturn = inCTOS_EMVCardReadProcess();

                if (shReturn != d_OK)
                {


                    if ((shReturn==EMV_TRANS_NOT_ALLOWED) || (shReturn == EMV_FAILURE_EX || shReturn == EMV_APPL_NOT_AVAILABLE)){
                        vduiClearBelow(2);
                        vdCTOS_ResetMagstripCardData();
                        vdRemoveCard();
                        inEntryMode = SWIPE_ONLY;
                        if(shReturn == EMV_APPL_NOT_AVAILABLE)
                        {
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
                            inFallbackToMSR = FAIL;
                            fApplNotAvailable = TRUE;
                        }
                        else
                        {
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                            inFallbackToMSR = SUCCESS;
                        }

                        vduiClearBelow(2);
                        goto SWIPE_AGAIN;
                    }


                    if(inFallbackToMSR == SUCCESS)
                    {
                        vdCTOS_ResetMagstripCardData();
                        //0826
                        //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                        //goto SWIPE_AGAIN;
                        //vduiClearBelow(2);
                        vdRemoveCard();
                        //clearLine(7);
                        inChipTries= inChipTries+1;
                        if (inChipTries < 3){
                            inEntryMode = INSERT_ONLY;
							insert_loopcount = 0;
                            goto INSERT_AGAIN;
                        }else{
                            inEntryMode = SWIPE_ONLY;
                            //1125
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                            //1125
                            goto SWIPE_AGAIN;
                        }
                        //0826
                    }else{
                        //vdSetErrorMessage("Get Card Fail C");
                        return USER_ABORT;
                    }
                }

                //0826
                inEntryMode = READ_ALL;
                //0826

                vdDebug_LogPrintf("--EMV Read succ----" );
                //Load the CDT table
                if (d_OK != inCTOS_LoadCDTIndex())
                {
                    CTOS_KBDBufFlush();
                    return USER_ABORT;
                }

                break;
            }


            //0826
            if (inEntryMode == INSERT_ONLY)
                goto INSERT_AGAIN;


        }
        //0826

        //for Idle swipe card

        if (fBINVer)
            break;

        if (strlen(srTransRec.szPAN) > 0)
        {


            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail");
                return USER_ABORT;
            }

            if(d_OK != inCTOS_CheckEMVFallback())
            {

                //version 11
                vduiClearBelow(2);
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "1.PLS INSERT CARD");

                //issue#11
                vduiClearBelow(2);
                inEntryMode = INSERT_ONLY;
                goto INSERT_AGAIN;
                //goto SWIPE_AGAIN;
                //issue#11

            }

            break;

        }


        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

        /* BDOCLG-00187: Return to idle if card is incorrectly swipe at idle screen - start -- jzg */
        if(((byMSR_status != d_OK) && (fEntryCardfromIDLE == TRUE) && (inEntryMode == READ_ALL)) || (byMSR_status == 0x02))
        {
            //inCTOSS_CLMCancelTransaction();
            //vduiClearBelow(1);
            //vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
            //Tine:  23MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, szTitle);
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "CARD READ ERROR");
            //usCTOSS_LCDDisplay(szDisMsg);
            inDisplayMessageBoxWithButtonConfirmation(1,8,"","CARD READ ERROR","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
            CTOS_Beep();
            CTOS_Delay(1500);
            CTOS_Beep();
            return INVALID_CARD;
        }
        /* BDOCLG-00187: Return to idle if card is incorrectly swipe at idle screen - end -- jzg */

        //gcitra
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
            //gcitra
        {
            //Invalid card reading fix - start -- jzg
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

            if (shReturn == INVALID_CARD)
            {
                CTOS_KBDBufFlush();
                //vduiClearBelow(1);
                //vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
                //Tine:  23MAY2019
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "CARD READ ERROR");
                //usCTOSS_LCDDisplay(szDisMsg);
                inDisplayMessageBoxWithButtonConfirmation(1,8,"","CARD READ ERROR","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                CTOS_Beep();
                CTOS_Delay(1500);
                CTOS_Beep();
                return INVALID_CARD;
            }
            //Invalid card reading fix - end -- jzg

            if(inFallbackToMSR == SUCCESS)
            {
                vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
            }

            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            if(d_OK != inCTOS_CheckEMVFallback())
            {

                //version 11
                vduiClearBelow(2);
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "2.PLS INSERT CARD");

                //issue#11
                vduiClearBelow(2);
                inEntryMode = INSERT_ONLY;
                goto INSERT_AGAIN;
                //goto SWIPE_AGAIN;
                //issue#11

            }

            break;
        }
#if 0
        else //aaronnino for BDOCLG ver 9.0 fix on issue #00187 Terminal cannot re-swipe card if 1st attempt is swiped improperly from idle menu A. SCENARIO start
		{
			 if(fIdleSwipe == TRUE)
				{
						vdSetErrorMessage("READ CARD ERROR");
						return ST_ERROR;
				}
		}
						//aaronnino for BDOCLG ver 9.0 fix on issue #00187 Terminal cannot re-swipe card if 1st attempt is swiped improperly from idle menu  A. SCENARIO end
#endif
    }



    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);
    return d_OK;
}

#endif

void InsertCardUI(void)
{

    BYTE szTitle[20+1];
    BYTE szDisMsg[200];
    BYTE    szTotalAmt[12+1];
    BYTE    szStr[45];

	int inKey;

#if 0


    memset(szTitle, 0x00, sizeof(szTitle));
    memset(szDisMsg, 0x00, sizeof(szDisMsg));

    strcpy(szDisMsg, "cardidle");
    strcat(szDisMsg, "|");
    strcat(szDisMsg, "back");

    szGetTransTitle(srTransRec.byTransType, szTitle);


    vdDebug_LogPrintf("szTitle[%s]", szTitle);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, szTitle);


    memset(szTotalAmt, 0x00, sizeof(szTotalAmt));
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);


    vdTrimLeadZeroes(szTotalAmt);

    memset(szStr, 0x00, sizeof(szStr));
    vdCTOSS_FormatAmount("NN,NNN,NNN,NNn.nn", szTotalAmt, szStr);
    vdDebug_LogPrintf("szStr[%s]", szStr);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, szStr);
    //strcat(szDisMsg, "Amount:");
    strcat(szDisMsg, "|");
    //strcat(szDisMsg, szStr);
    strcat(szDisMsg, "Amount:");

    usCTOSS_DisplayUI(szDisMsg);

#else
	 strcpy(szDisMsg, szTitle);
	 strcat(szDisMsg, "|");
	 strcat(szDisMsg, "TAP/INSERT CARD");
	 strcat(szDisMsg, "|");
	 strcat(szDisMsg, "CARD ENTRY");
	 //usCTOSS_LCDDisplay(szDisMsg);
	 inKey = usCARDENTRY(szDisMsg); 	

#endif
}

void ProcessingUI(void)
{

    BYTE szTitle[20+1];
    BYTE szDisMsg[200];


    memset(szDisMsg, 0x00, sizeof(szDisMsg));
    strcpy(szDisMsg, "process");
    strcat(szDisMsg, "|");
    strcat(szDisMsg, "front");

    usCTOSS_DisplayUI(szDisMsg);


}

/*
void DisplayMessage(void)
{
    BYTE szTitle[20+1];
    BYTE szDispMsg[200];


    memset(szDispMsg, 0x00, sizeof(szDispMsg));
    strcpy(szDispMsg, "SWIPE/INSERT/TAP CARD");

    vdDebug_LogPrintf("saturn DisplayMessage swipe/insert/tap");

    usCTOSS_LCDDisplay(szDispMsg);
}
*/

int inCTOS_WaveGetCardFields(void)
{
    int inRet2 = 0;
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES_50], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES_50 ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
    ULONG ulAPRtn;
    BYTE temp[64];
    char szTotalAmount[AMT_ASC_SIZE+1];
    int inchipswiped = 0; //aaalcantara issue number 000113 1 of 3
    EMVCL_RC_DATA_EX stRCDataEx;
    BYTE szOtherAmt[12+1],szTransType[2+1],szCatgCode[3+1],szCurrCode[3+1];

    CTOS_RTC SetRTC;

    BYTE szBaseAmt[30+1] = {0};
    //0826
    int inChipTries=0;
    int inEntryMode=0;
    BOOL fClearScreen=FALSE;

    BYTE szTitle[20+1];
    BYTE szDisMsg[200];
    BYTE    szTotalAmt[12+1];
    BYTE    szStr[45];
    BYTE szManualPan[25];

    USHORT usPanLen = 20;
    BYTE   szInvNoAsc[100+1];
    BYTE   szInvNoBcd[3];
    BYTE bRet;
    BYTE szDisplay[200];


    BYTE pbDisplay[512+1];  //Tine:  for Cancel UI
    int inKey=0;
    int swipe_loopcount = 0;
    int insert_loopcount = 0;
	BYTE szManualEntryFlag[2];
	char szAmtMsg[45 + 1];

    /*
	1= insert only
	2= swipe only
	0= will accept al
	*/

    //CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - start -- jzg
    char szBaseAmount[20] = {0};
    char szTipAmount[20] = {0};
    //CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - end -- jzg

#define INSERT_ONLY 1
#define SWIPE_ONLY	2
#define READ_ALL 0
    //0826

    short shReturn = d_OK; //Invalid card reading fix -- jzg


    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
    BOOL fMaxCTLSAmt = FALSE;
    long amt1 = 0;
    long amt2 = 0;
    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- jzg */


    /* BDO CLG: MOTO setup -- jzg */
    int inMOTOResult;

	char szAmountBuff[45 + 1];

	vdDebug_LogPrintf("saturn inCTOS_WaveGetCardFields...");
	vdDebug_LogPrintf("fIdleInsert=[%d]", fIdleInsert);
	vdDebug_LogPrintf("fIdleSwipe=[%d]", fIdleSwipe);
	vdDebug_LogPrintf("fBINVer=[%d]", fBINVer);
	
    fApplNotAvailable = FALSE;

    // get transaction title
    memset(szTitle, 0x00, sizeof(szTitle));
    //szGetTransTitle(srTransRec.byTransType, szTitle);
    szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);

	// get manual entry flag
	memset(szManualEntryFlag, 0x00, sizeof(szManualEntryFlag));
	if(strTCT.fEnableManualKeyEntry == 1)
        strcpy(szManualEntryFlag, "1");
    else
        strcpy(szManualEntryFlag, "0");

	// Get Amount
	memset(szAmtMsg, 0x00, sizeof(szAmtMsg));
	memset(szTotalAmt, 0x00, sizeof(szTotalAmt));
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
	vdCTOS_FormatAmount(strCST.szAmountFormat, szTotalAmt, szAmtMsg);
	
	if (strlen(srTransRec.szPAN) == 0){
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
    strcpy(szDisMsg, szTitle);
    strcat(szDisMsg, "|");
    //strcat(szDisMsg, "Present card");
    strcat(szDisMsg, "Tap/Insert Card");
	strcat(szDisMsg, "|");
    strcat(szDisMsg, strCST.szCurSymbol);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, szAmtMsg);
	strcat(szDisMsg, "|");
    strcat(szDisMsg, szManualEntryFlag);
    //usCTOSS_LCDDisplay(szDisMsg);
    vdDebug_LogPrintf("saturn swipe/insert/tap 1");
    inKey=usCARDENTRY(szDisMsg);			//Tine:  24Apr2019
	}


    //vdDebug_LogPrintf("inCTOS_WaveGetCardFields......... %d-%d ", srTransRec.byEntryMode,fEntryCardfromIDLE);



    //vdDebug_LogPrintf("inCTOS_WaveGetCardFields......... %s-%d ", srTransRec.szPAN,fIdleSwipe);

    srTransRec.fPrintSMCardHolder = FALSE;
    srTransRec.fPrintCardHolderBal = FALSE;
    if ((strlen(srTransRec.szPAN) > 0) && (fIdleSwipe == TRUE || fIdleInsert == TRUE))
        return d_OK;



    /* BDO CLG: MOTO setup - start -- jzg */
    if (strTCT.fMOTO == 1)
    {
 		 vdDebug_LogPrintf("saturn moto");

        //CTOS_LCDTClearDisplay();
        //Tine:  add SWIPE/INSERT CARD display 08Mar2019
        //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");

        vdDispTransTitle(srTransRec.byTransType);
        //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
        CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

        while (1)
        {


            //CTOS_LCDTPrintXY(1, 3, "Enter/Cancel");

            if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
            {
                fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
                return READ_CARD_TIMEOUT ;
            }

            CTOS_KBDInKey(&byKeyBuf);
            if (byKeyBuf)
            {
                CTOS_KBDGet(&byKeyBuf);
                switch(byKeyBuf)
                {
                    case d_KBD_0:
                    case d_KBD_1:
                    case d_KBD_2:
                    case d_KBD_3:
                    case d_KBD_4:
                    case d_KBD_5:
                    case d_KBD_6:
                    case d_KBD_7:
                    case d_KBD_8:
                    case d_KBD_9:
                    case d_KBD_CANCEL:
                        if (byKeyBuf == d_KBD_CANCEL)
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }


                        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

                        vdSetFirstIdleKey(byKeyBuf);
                        vdDebug_LogPrintf("szPAN2[%s]", srTransRec.szPAN);

                        //get the card number and ger Expire Date
                        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
                        {
                            vdSetFirstIdleKey(0x00);
                            CTOS_KBDBufFlush ();
                            return USER_ABORT;
                        }

                        inMOTOResult = inCTOS_LoadCDTIndex();
                        //Load the CDT table
                        if (d_OK != inMOTOResult)
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }

                        if(strCDT.fManEntry == FALSE)
                        {
                            //vdDisplayErrorMsgResp2(" ", " ", "MKE NOT ALLOWED");
                            CTOS_KBDBufFlush();
                            break;
                            //return USER_ABORT;
                        }


                        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);

                        if(inMOTOResult == d_OK)
                            return d_OK;

                        break;
                }
            }

        }

        return d_OK;

    }
    /* BDO CLG: MOTO setup - end -- jzg */


    if (fIdleInsert == TRUE){

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if (!(bySC_status & d_MK_SC_PRESENT)){
            fEntryCardfromIDLE = FALSE;
            fIdleInsert=FALSE;
            srTransRec.byEntryMode = 0;
        }
    }

    inEntryMode = READ_ALL;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    //CTOS_LCDTClearDisplay();

    // patrick fix code 20141222 case 179
    if (fEntryCardfromIDLE != TRUE)
    {
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        fClearScreen=TRUE;
    }

	
	vdDebug_LogPrintf("saturn fEntryCardfromIDLE %d", fEntryCardfromIDLE);

    ing_KeyPressed = 0;

    SWIPE_AGAIN:
    vdDebug_LogPrintf("saturn entry mode %d", inEntryMode);

    EntryOfStartTrans:
    if(d_OK != inCTOS_ValidFirstIdleKey())
    {
        //CTOS_LCDTClearDisplay();
        //vdDispTransTitle(srTransRec.byTransType); //Issue 660 Removed flashing Header
        //gcitra-0728
        //inCTOS_DisplayIdleBMP();
        //gcitra-0728
    }

    memset(&stRCDataEx,0x00,sizeof(EMVCL_RC_DATA_EX));
    memset(szOtherAmt,0x00,sizeof(szOtherAmt));
    memset(szTransType,0x00,sizeof(szTransType));
    memset(szCatgCode,0x00,sizeof(szCatgCode));
    memset(szCurrCode,0x00,sizeof(szCurrCode));
    memset(temp,0x00,sizeof(temp));
    memset(szTotalAmount,0x00,sizeof(szTotalAmount));
    wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
    //vdDebug_LogPrintf("szTotalAmount[%s].strTCT.szCTLSLimit=[%s]........",szTotalAmount,strTCT.szCTLSLimit);

//gcitra
#if 0
    CTOS_LCDTClearDisplay();
	vdDispTransTitle(srTransRec.byTransType);
	CTOS_LCDTPrintXY(1, 3, "    Present Card   ");
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6);
	sprintf(temp, " Amount: %lu.%02lu", atol(szTotalAmount)/100, atol(szTotalAmount)%100);
	CTOS_LCDTPrintXY(1, 4, temp);
#endif

    //CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - start -- jzg
    memset(szBaseAmount, 0x00, sizeof(szBaseAmount));
    memset(szTipAmount, 0x00, sizeof(szTipAmount));
    wub_hex_2_str(srTransRec.szTipAmount, szTipAmount, 6);
    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmount, 6);
    // patrick fix code 20141216
    sprintf(szTotalAmount, "%012.0f", atof(szBaseAmount) + atof(szTipAmount));
    wub_str_2_hex(szTotalAmount, srTransRec.szTotalAmount, 12);
//	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmount, 6); //CTLS - Fix for CTLS reader not accepting PayPass Cards  -- jzg
    //CTLS: Pass the correct amount to inCTOSS_CLMInitTransaction - end -- jzg

    if (srTransRec.byTransType == REFUND)
        szTransType[0] = 0x20;
    //CTLS - Fix for CTLS reader not accepting PayPass Cards - start  -- jzg
    sprintf(szCatgCode, "%04d", atoi(strCST.szCurCode));
    strcpy(szCurrCode, szCatgCode);
    //CTLS - Fix for CTLS reader not accepting PayPass Cards  - end -- jzg


    /* BDO: Revised amount comparison to accomodate large values - start -- jzg */
    if (strcmp(szTotalAmount, strTCT.szMaxCTLSAmount) >= 0)
        fMaxCTLSAmt = TRUE;
    else
        fMaxCTLSAmt = FALSE;
    /* BDO: Revised amount comparison to accomodate large values - end -- jzg */


    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
    if ((inEntryMode == READ_ALL) && (!fMaxCTLSAmt))
//	if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && 1 != chGetIdleEventSC_MSR())
        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        {
            ulAPRtn = inCTOSS_CLMInitTransaction(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode);
            if(ulAPRtn != d_EMVCL_NO_ERROR)
            {
                vdSetErrorMessage("CTLS InitTrans Fail!");
                return d_NO;
            }



        }
    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - end -- jzg */


    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);

    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

    //gcitra-0728
    //vdDispTransTitle(srTransRec.byTransType);//Issue 660 Removed flashing Header

    if (!fBINVer)

	
	vdDebug_LogPrintf("saturn !binver entrymode %d", inEntryMode);
        if (inEntryMode  == SWIPE_ONLY)
        {
            //memset(szTitle, 0x00, sizeof(szTitle));
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "PLEASE SWIPE CUSTOMER CARD");
			//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "CARD ENTRY");
			//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "0");
            //usCTOSS_LCDDisplay(szDisMsg);

			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "Please swipe customer card");
			strcat(szDisMsg, "|");
			strcat(szDisMsg, strCST.szCurSymbol);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, szAmtMsg);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "0");
	
            if(swipe_loopcount == 0)
            {
                ing_KeyPressed = 0;
                inKey = usCARDENTRY(szDisMsg);
            }
            //onFailedCtlsFallback(szDisMsg);
            //vdDispTransTitle(srTransRec.byTransType);
            //CTOS_LCDTPrintXY(1, 3, "Please Swipe");
            //CTOS_LCDTPrintXY(1, 4, "Customer Card");

            swipe_loopcount = 1;
        }
            //aaalcantara issue number 000113 2 of 3 START
        else if (inchipswiped == 1)
        {
            //vdDispTransTitle(srTransRec.byTransType);
            //CTOS_LCDTPrintXY(1, 3, "              ");
            //CTOS_LCDTPrintXY(1, 4, "              ");
            inchipswiped = 0;
			//memset(szTitle, 0x00, sizeof(szTitle));
            //szGetTransTitle(srTransRec.byTransType, szTitle);
			//memset(szDisMsg, 0x00, sizeof(szDisMsg));
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "PLEASE SWIPE CUSTOMER CARD");
			//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "CARD ENTRY");
			//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "0");

			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "Please swipe customer card");
			strcat(szDisMsg, "|");
			strcat(szDisMsg, strCST.szCurSymbol);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, szAmtMsg);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "0");
			inKey = usCARDENTRY(szDisMsg);
			
        }
            //aaalcantara issue number 000113 2 of 3 END
        else
        {
#if 1
            //issue:218
            if (fEntryCardfromIDLE != TRUE)
            {

			vdDebug_LogPrintf("saturn fEntryCardfromIDLE not true");
                if(strTCT.fSMMode == TRUE)
                {
                    vdDispTransTitle(srTransRec.byTransType);

                    if(strTCT.fEnableManualKeyEntry == TRUE)
                    {
                        CTOS_LCDTPrintXY(1, 3, "TAP/INSERT/SWIPE");
                        CTOS_LCDTPrintXY(1, 4, "ENTER CARD");
                    }
                    else
                    {
                        CTOS_LCDTPrintXY(1, 3, "TAP/INSERT/SWIPE");
                        CTOS_LCDTPrintXY(1, 4, "CARD");
                    }
                }
            }
#else
            /// -- do nothing
			  // SHARLS_CTLS already display this message  -- sidumili.
#endif
        }
    //gcitra-0728


    //0826
    INSERT_AGAIN:
    //0826
    //ing_KeyPressed = 0;
    while (1)
    {
#if 0
        inKey=usCARDENTRY(szDisMsg);			//Tine:  28MAY2019
        if(inKey==d_USER_CANCEL)
        {
            return inKey;
        }

#endif
		//vdDebug_LogPrintf("inCTOS_WaveGetCardFields, ing_KeyPressed[%d]", ing_KeyPressed);

        if (ing_KeyPressed == 'C')
        {
            CTOS_KBDBufPut('C');
            vdDebug_LogPrintf("saturn putchar C");
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisplay, szTitle);
            //strcat(szDisplay, "|");
            //strcat(szDisplay, "USER CANCEL");
            //usCTOSS_LCDDisplay(szDisplay);
            //vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
            CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
            //CTOS_Delay(1500);
            return USER_ABORT;
        }
        else if (ing_KeyPressed == 'T')
        {
            CTOS_KBDBufPut('T');
            vdDebug_LogPrintf("saturn putchar T");
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisplay, szTitle);
            //strcat(szDisplay, "|");
            //strcat(szDisplay, "TIME OUT");
            //usCTOSS_LCDDisplay(szDisplay);
            //vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
            CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
            //CTOS_Delay(1500);
            return READ_CARD_TIMEOUT ;
        }
		else if (ing_KeyPressed == 'M')     //Manual Entry
	{
		CTOS_KBDBufPut('M');
		vdDebug_LogPrintf("saturn putchar M");
        inCTOSS_CLMCancelTransaction();
        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

		usCTOSS_LCDDisplay(" ");

        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
        {
            vdSetFirstIdleKey(0x00);
            CTOS_KBDBufFlush ();
            //vdSetErrorMessage("Get Card Fail M");
            return USER_ABORT;
        }

        //Load the CDT table
        if (d_OK != inCTOS_LoadCDTIndex())
        {
            CTOS_KBDBufFlush();
            return USER_ABORT;
        }

		if(strCDT.fManEntry == FALSE)
        {
             if (strTCT.fSMMode==FALSE)
               	vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
             else
                vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");

             CTOS_KBDBufFlush();
             return USER_ABORT;
         }

        break;
	}

	//vdDebug_LogPrintf("saturn check ing_KeyPressed");
#if 0
        if(CTOS_TimeOutCheck(TIMER_ID_1)  == d_YES)
        {
            fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();
			
			szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisplay, szTitle);
            strcat(szDisplay, "|");
            strcat(szDisplay, "TIME OUT");
            usCTOSS_LCDDisplay(szDisplay);
            CTOS_Beep();
            CTOS_Delay(1500);
            return READ_CARD_TIMEOUT ;
        }

#endif
#if 0		
        //CTOS_KBDInKey(&byKeyBuf);
        // patrick add code 20141209
        if (byKeyBuf)
        {
            vdDebug_LogPrintf("saturn manual entry 2");
            //CTOS_KBDGet(&byKeyBuf);
            switch(byKeyBuf)
            {
                case d_KBD_0:
                case d_KBD_1:
                case d_KBD_2:
                case d_KBD_3:
                case d_KBD_4:
                case d_KBD_5:
                case d_KBD_6:
                case d_KBD_7:
                case d_KBD_8:
                case d_KBD_9:

                    //fEnableBinVerMKE = MKE for BIN VER
                    if(srTransRec.byTransType == BIN_VER)
                    {
                        if ((strTCT.fEnableBinVerMKE == FALSE) && (strTCT.fSMMode==FALSE))
                        {
                            vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                            //return USER_ABORT;
                        }
                        else if ((strTCT.fEnableBinVerMKE == FALSE) && (strTCT.fSMMode==TRUE))
                        {
                            CTOS_KBDBufFlush();
                            break;
                        }
                    }
                        //fEnableInstMKE = MKE General flag for installment
                    else if (fInstApp == TRUE)
                    {
                        if ((strTCT.fEnableInstMKE == FALSE) && (strTCT.fSMMode==FALSE))
                        {
                            //vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");
                            vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                            //return USER_ABORT;
                        }
                        else if ((strTCT.fEnableInstMKE == FALSE) && (strTCT.fSMMode==TRUE))
                        {
                            CTOS_KBDBufFlush();
                            break;
                        }
                        //fEnableManualKeyEntry - Flag for all other hosts
                    }
                    else if(inFallbackToMSR == SUCCESS) //aaronnino for BDOCLG ver 9.0 fix on issue #00487 Able to press keypad for MKE upon re-inserting card for fallback transactions
                    {
                        //vdDisplayErrorMsgResp2(" ", " ", "MKE NOT ALLOWED");
                        CTOS_KBDBufFlush();
                        break;
                        //return USER_ABORT;
                    }
                    else if ((strTCT.fEnableManualKeyEntry == FALSE) && (strTCT.fSMMode==FALSE))
                    {
                        //vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");
                        vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                    }
                    else if ((strTCT.fEnableManualKeyEntry == FALSE) && (strTCT.fSMMode==TRUE))
                    {
                        CTOS_KBDBufFlush();
                        break;
                    }


                case d_KBD_CANCEL:
                    if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                        inCTOSS_CLMCancelTransaction();

                    //gcitra-0728
                    if (byKeyBuf == d_KBD_CANCEL)
                    {
                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                    }
                    //gcitra-0728

                    vdSetFirstIdleKey(byKeyBuf);

                    memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
                    //gcitra
                    //if(d_OK == inCTOS_ValidFirstIdleKey())
                    //       srTransRec.szPAN[0] = chGetFirstIdleKey();
                    //gcitra

                    vdDebug_LogPrintf("szPAN[%s]", srTransRec.szPAN);
                    //get the card number and ger Expire Date
                    if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
                    {
                        vdSetFirstIdleKey(0x00);
                        CTOS_KBDBufFlush ();
                        //vdSetErrorMessage("Get Card Fail M");
                        return USER_ABORT;
                    }

                    //Load the CDT table
                    if (d_OK != inCTOS_LoadCDTIndex())
                    {
                        //gcitra-120214
                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                        //gcitra-120214
                    }

                    if(strCDT.fManEntry == FALSE)
                    {
                        if (strTCT.fSMMode==FALSE)
                            vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                        else
                            vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");

                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                    }

                    break;
            }
        }
#endif

		//vdDebug_LogPrintf("saturn entrymode=[%d]", inEntryMode);
		//vdDebug_LogPrintf("inFallbackToMSR=[%d]", inFallbackToMSR);

        if (inEntryMode != SWIPE_ONLY)
        {
//INSERT_AGAIN:

            if (inEntryMode == INSERT_ONLY)
            {
                byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
                vdCTOS_ResetMagstripCardData();
                //vdDispTransTitle(srTransRec.byTransType);
                //CTOS_LCDTPrintXY(1, 4, "PLEASE INSERT CARD/");
                //CTOS_LCDTPrintXY(1, 5, "CANCEL");
                //memset(szTitle, 0x00, sizeof(szTitle));
                //szGetTransTitle(srTransRec.byTransType, szTitle);
                //strcpy(szDisMsg, szTitle);
                //strcat(szDisMsg, "|");
                //strcat(szDisMsg, "PLEASE INSERT CARD");
				//strcat(szDisMsg, "|");
	            //strcat(szDisMsg, "CARD ENTRY");
				//strcat(szDisMsg, "|");
	            //strcat(szDisMsg, "0");
                //usCTOSS_LCDDisplay(szDisMsg);

				memset(szDisMsg, 0x00, sizeof(szDisMsg));
				strcpy(szDisMsg, szTitle);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "Please insert card");
				strcat(szDisMsg, "|");
				strcat(szDisMsg, strCST.szCurSymbol);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, szAmtMsg);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "0");
                if(insert_loopcount == 0)
                {
                    //ing_KeyPressed = 0;
                    inKey = usCARDENTRY(szDisMsg);
                }
                insert_loopcount = 1;

            }

//0826

            CTOS_SCStatus(d_SC_USER, &bySC_status);
            if(bySC_status & d_MK_SC_PRESENT)
            {
                vdDebug_LogPrintf("saturn chip entry");
                inCTOSS_CLMCancelTransaction();
                //clearLine(8);
                vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

                //vdDebug_LogPrintf("--EMV flow----" );
                if (fClearScreen == TRUE && srTransRec.byTransType != BALANCE_INQUIRY)
                {
                    //vdDispTransTitle(srTransRec.byTransType);
                    //vdClearBelowLine(2);
                    //strcpy(szDisMsg, szTitle);
                    //strcat(szDisMsg, "|");
                    //strcat(szDisMsg, "PROCESSING...");
                    //usCTOSS_LCDDisplay(szDisMsg);        //Tine:  24Apr2019
				
                    vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
					//CTOS_Delay(10);
                }
                shReturn = inCTOS_EMVCardReadProcess();




                //if (d_OK != inCTOS_EMVCardReadProcess ())
                if (shReturn != d_OK)
                {
                    vdDebug_LogPrintf("-------inCTOS_EMVCardReadProcess---[%d]-- %d %d",shReturn, inFallbackToMSR, inChipTries);

                    if ((shReturn==EMV_TRANS_NOT_ALLOWED) || (shReturn == EMV_FAILURE_EX) || (shReturn == EMV_APPL_NOT_AVAILABLE))
                    {
                        vdDebug_LogPrintf("-------inCTOS_EMVCardReadProcess 3---[%d]-- %d %d",shReturn, inFallbackToMSR, inChipTries);
                        vduiClearBelow(2);
                        vdCTOS_ResetMagstripCardData();
                        vdRemoveCard();
                        inEntryMode = SWIPE_ONLY;
                        if(shReturn == EMV_APPL_NOT_AVAILABLE)
                        {
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
                            inFallbackToMSR = FAIL;
                            fApplNotAvailable = TRUE;
                        }
                        else
                        {
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                            inFallbackToMSR = SUCCESS;
                        }

                        //vduiClearBelow(2);
                        goto SWIPE_AGAIN;
                    }

                    if(inFallbackToMSR == SUCCESS)
                    {

                        vdDebug_LogPrintf("-------inCTOS_EMVCardReadProcess 2---[%d]-- %d %d",shReturn, inFallbackToMSR, inChipTries);
                        vdCTOS_ResetMagstripCardData();
                        //0826
                        //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                        //goto SWIPE_AGAIN;
                        vduiClearBelow(2);
                        vdRemoveCard();
                        clearLine(7);
                        inChipTries= inChipTries+1;
                        if (inChipTries < 3)
                        {
                            inEntryMode = INSERT_ONLY;
                            insert_loopcount = 0;
                            goto INSERT_AGAIN;
                        }
                        else
                        {
                            inEntryMode = SWIPE_ONLY;

                            //1125
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                            //1125
                            goto SWIPE_AGAIN;
                        }
                        //0826
                    }
                    else
                    {
                        //vdSetErrorMessage("Get Card Fail C");
                        return USER_ABORT;
                    }
                }

                //vdDebug_LogPrintf("--EMV Read succ----" );
                //Load the CDT table
                if (d_OK != inCTOS_LoadCDTIndex())
                {
                    CTOS_KBDBufFlush();
                    return USER_ABORT;
                }

                break;
            }


            if (inEntryMode == INSERT_ONLY)
                goto INSERT_AGAIN;
        }

        //for Idle swipe card
        if (strlen(srTransRec.szPAN) > 0)
        {
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail");
                return USER_ABORT;
            }

            if(d_OK != inCTOS_CheckEMVFallback())
            {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "3.PLS INSERT CARD");

                //issue#11
                vduiClearBelow(2);
                inEntryMode = INSERT_ONLY;
                goto INSERT_AGAIN;
                //goto SWIPE_AGAIN;
                //issue#11

            }

            break;

        }

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

        //vdDebug_LogPrintf("msr status %d", byMSR_status);

        /* BDOCLG-00187: Return to idle if card is incorrectly swipe at idle screen - start -- jzg */
        if(((byMSR_status != d_OK) && (fEntryCardfromIDLE == TRUE) && (inEntryMode == READ_ALL)) || (byMSR_status == 0x02))
        {
            inCTOSS_CLMCancelTransaction();
            //vduiClearBelow(1);
            //vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
            //Tine:  23MAY2019
            //memset(szTitle, 0x00, sizeof(szTitle));
            //szGetTransTitle(srTransRec.byTransType, szTitle);
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "CARD READ ERROR");
            //usCTOSS_LCDDisplay(szDisMsg);
            //CTOS_Beep();
            //CTOS_Delay(1500);
            //CTOS_Beep();
            vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
            return INVALID_CARD;
        }
        /* BDOCLG-00187: Return to idle if card is incorrectly swipe at idle screen - end -- jzg */

        //gcitra
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
            //gcitra

            //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        {
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();


            vdDebug_LogPrintf("mag entry");

            //Invalid card reading fix - start -- jzg
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

            if (shReturn == INVALID_CARD)
            {
                CTOS_KBDBufFlush();
                vduiClearBelow(1);
                vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
                return INVALID_CARD;
            }
            //Invalid card reading fix - end -- jzg

            //1125
            if(inFallbackToMSR == SUCCESS){
                vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
            }
            //1125

            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            if(d_OK != inCTOS_CheckEMVFallback())
            {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "4.PLS INSERT CARD");
                inchipswiped = 1; //aaalcantara issue number 000113 3 of  3

                //issue#11
                vduiClearBelow(2);
                inEntryMode = INSERT_ONLY;
                goto INSERT_AGAIN;
                //goto SWIPE_AGAIN;
                //issue#11

            }

            break;
        }



        if(inEntryMode == SWIPE_ONLY){
            //inCTOSS_CLMCancelTransaction();
            goto SWIPE_AGAIN;
        }

        /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime - start -- jzg */
//		if(!fMaxCTLSAmt)
        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        {
            ulAPRtn = inCTOSS_CLMPollTransaction(&stRCDataEx, 5);

            // V3 contactless reader
//		EMVCL_StopIdleLEDBehavior(NULL);
//		EMVCL_SetLED(0x0F, 0x08);

// patrick test code 20141230 start
#define d_EMVCL_RC_SEE_PHONE		0xA00000AF
// patrick test code 20141230 end

            if(ulAPRtn == d_EMVCL_RC_DEK_SIGNAL)
            {
                vdDebug_LogPrintf("saturn DEK Signal Data[%d][%s]", stRCDataEx.usChipDataLen,stRCDataEx.baChipData);
            }
                //EMV: Added error message handling "PLEASE SEE PHONE" - start -- jzg
            else if(ulAPRtn == d_EMVCL_RC_SEE_PHONE)
            {
                vdDisplayErrorMsg(1, 8, "PLEASE SEE PHONE");
                CTOS_Delay(3000);
                vdDisplayErrorMsg(1, 8, "                ");
                goto SWIPE_AGAIN;
            }
                //EMV: Added error message handling "PLEASE SEE PHONE" - end -- jzg
            else if(ulAPRtn == d_EMVCL_TX_CANCEL)
            {
                vdDisplayErrorMsg(1, 8, "USER CANCEL");
                return USER_ABORT;
            }
            else if(ulAPRtn == d_EMVCL_RX_TIMEOUT)
            {
                if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                    inCTOSS_CLMCancelTransaction();

                //CTOS_Beep();
                //CTOS_Delay(50);
                //CTOS_Beep();
                //CTOS_Delay(50);
                vdDisplayErrorMsg(1, 8, "TIMEOUT");
                fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
                return USER_ABORT;
            }
            else if((ulAPRtn == d_EMVCL_RC_NO_AP_FOUND) || (ulAPRtn == d_EMVCL_RSP_ID_ERROR))
            {
                if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                    inCTOSS_CLMCancelTransaction();

                //CTOS_Beep();
                //CTOS_Delay(50);
                //CTOS_Beep();
                //CTOS_Delay(50);
                CTOS_LCDTClearDisplay();
                setLCDPrint(3, DISPLAY_POSITION_CENTER, "CTLS TRANSACTION");
                setLCDPrint(4, DISPLAY_POSITION_CENTER, "LIMIT EXCEEDED PLS");
                setLCDPrint(5, DISPLAY_POSITION_CENTER, "SWIPE/ INSERT CARD");
                CTOS_Delay(1500);

                return USER_ABORT;
            }
            else if(ulAPRtn != d_EMVCL_PENDING)
            {
                //CTOS_Beep();
                //CTOS_Delay(50);
                //CTOS_Beep();
                //CTOS_Delay(50);
                //CTOS_Beep();
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                break;
            }
        }
        else
        {
            memset(szTransType,0x00,sizeof(szTransType));
            strcpy(szOtherAmt,"000000000000");
            if (srTransRec.byTransType == REFUND)
                strcpy(szTransType,"20");
            else
                strcpy(szTransType,"00");
            ulAPRtn = usCTOSS_CtlsV3Trans(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode,&stRCDataEx);
			 vdDebug_LogPrintf("saturn test 3 %d", ulAPRtn);
            //CTOS_LCDTClearDisplay();
            //CTOS_Beep();
            //CTOS_Delay(50);
            //CTOS_Beep();
            //CTOS_Delay(50);
            //CTOS_Beep();
            if(ulAPRtn == d_OK)
            {
                vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
                ulAPRtn = d_EMVCL_RC_DATA;
                break;
            }
#if 0
            if (ulAPRtn == d_NO)
            {
                memset(temp,0x00,sizeof(temp));
                inCTOSS_GetEnvDB("CTLSRESP", temp);
                ulAPRtn = atof(temp);
                vdDebug_LogPrintf("CTLSRESP[%s],ulAPRtn=[%x]", temp,ulAPRtn);

                /*BDO: Timeout --sidumili*/
                if (ulAPRtn == d_EMVCL_RC_NO_CARD || ing_KeyPressed == 'T')
                {
                    fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
                    return USER_ABORT;// should return if no card is detected.
                }
                /*BDO: Timeout --sidumili*/

                /*BDO: Separate error for CTLS read card error --sidumili*/
                if (ulAPRtn == d_EMVCL_RC_FAILURE)
                {

                    memset(szBaseAmt, 0x00, sizeof(szBaseAmt));
                    wub_hex_2_str(srTransRec.szBaseAmount, szBaseAmt, AMT_BCD_SIZE);

                    vdDebug_LogPrintf("saturn CTLS TEST %s - %s",szBaseAmt,strTCT.szCTLSLimit);

                    vdDebug_LogPrintf("saturn CTLS TEST2 %f - %f",atof(szBaseAmt),atof(strTCT.szCTLSLimit));

                    //temporary fix


                    if (atof(szBaseAmt) > atof(strTCT.szCTLSLimit))
                    {
                        //setLCDPrint(3, DISPLAY_POSITION_CENTER, "CTLS TRANSACTION");
                        //setLCDPrint(4, DISPLAY_POSITION_CENTER, "LIMIT EXCEEDED PLS");
                        //setLCDPrint(5, DISPLAY_POSITION_CENTER, "SWIPE/INSERT CARD");
                        //CTOS_Delay(1500);
                        inGblCtlsErr = 1;

                    }else{

                        //end temporary fix
                        //setLCDPrint(3, DISPLAY_POSITION_CENTER, "CLTS READ CARD ERROR");
                        //setLCDPrint(4, DISPLAY_POSITION_CENTER, "PLEASE SWIPE/");
                        //setLCDPrint(5, DISPLAY_POSITION_CENTER, "INSERT CARD");
                        //CTOS_Delay(1500);
                        inGblCtlsErr = 2;
                    }
                }
                /*BDO: Separate error for CTLS read card error --sidumili*/

                if(ulAPRtn == d_EMVCL_RC_EXCEED_OR_EQUAL_CL_TX_LIMIT)
                {
                    //setLCDPrint(3, DISPLAY_POSITION_CENTER, "CTLS TRANSACTION");
                    //setLCDPrint(4, DISPLAY_POSITION_CENTER, "LIMIT EXCEEDED PLS");
                    //setLCDPrint(5, DISPLAY_POSITION_CENTER, "SWIPE/INSERT CARD");
                    //CTOS_Delay(1500);
                    inGblCtlsErr = 3;
                }
                return (inCTOS_GetCardFieldsFailedCtlsFallback());
            }
#else
            else if (ulAPRtn == d_NO)
            {
                memset(temp,0x00,sizeof(temp));
                inCTOSS_GetEnvDB("CTLSRESP", temp);
                ulAPRtn = atof(temp);
                vdDebug_LogPrintf("saturn test CTLSRESP[%s],ulAPRtn=[%x]", temp,ulAPRtn);

			    if(ulAPRtn == d_EMVCL_RC_FALLBACK)
                    vdDisplayErrorMsg("CTLS ERROR PLEASE", "INSERT/SWIPE CARD", MSG_TYPE_ERROR);
                else if (ulAPRtn == d_EMVCL_RC_FAILURE)
                    vdDisplayErrorMsg("CTLS CARD READ ERROR", "PLS INSERT CARD", MSG_TYPE_ERROR);
                else if(ulAPRtn == d_EMVCL_RC_EXCEED_OR_EQUAL_CL_TX_LIMIT)
                    vdDisplayErrorMsg("CTLS TRANSACTION", "LIMIT EXCEEDED PLS", "INSERT/SWIPE CARD");
                else if(ulAPRtn == d_EMVCL_RC_SEE_PHONE)
                    vdDisplayErrorMsg("","", "PLEASE SEE PHONE");
	      	
                if(ing_KeyPressed == 'T' || CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES) {
					vdDisplayMessageBox(1, 8 , "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
					CTOS_Beep();
					//CTOS_Delay(300);
                } else if(ing_KeyPressed == 'D') {
                	//vdDisplayMessageBox(1, 8,  "", "Please try again", "", MSG_TYPE_ERROR);
					CTOS_Beep();
					//CTOS_Delay(300);
					//CTOS_Delay(1500);
                } else{
                	vdDebug_LogPrintf("saturn ctls CARD READ ERROR");
                    //vdDisplayErrorMsg("CTLS READ CARD ERROR", "PLS INSERT CARD", MSG_TYPE_ERROR);
                    vdDisplayMessageBox(1, 8 , "", "CTLS CARD READ ERROR", "", MSG_TYPE_TIMEOUT);
				}

					vdDebug_LogPrintf("saturn user abort");
                return USER_ABORT;
            }

#endif




            if(srTransRec.byEntryMode == CARD_ENTRY_WAVE)
            {
                if (ulAPRtn == d_MORE_PROCESSING || ulAPRtn == d_CTLS_EVENT)
                    return (inCTOS_GetCardFields());
            }
        }

    }

    /* CTLS: Added szMaxCTLSAmount to check CTLS max txn amount during runtime -- jzg */
    if ((srTransRec.byEntryMode == CARD_ENTRY_WAVE) && (!fMaxCTLSAmt))
    {


        // add to still continue with mag stripe once ctls card was read partially
        if((ulAPRtn == d_EMVCL_RC_DDA_AUTH_FAILURE) || (ulAPRtn == d_EMVCL_RC_FAILURE))
            goto SWIPE_AGAIN;



        if(ulAPRtn != d_EMVCL_RC_DATA)
        {
            vdCTOSS_WaveCheckRtCode(ulAPRtn);
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            return d_NO;
        }

        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
        {
            if (d_OK != inCTOSS_WaveAnalyzeTransaction(&stRCDataEx))
            {
                inCTOSS_CLMCancelTransaction();
                return d_NO;
            }
        }
        else
        {
            inRet2 = inCTOSS_V3AnalyzeTransaction(&stRCDataEx);
            if (inRet2 != d_OK)
            {
                return CTLS_ANALYZE_ERROR;
            }
        }

        //Load the CDT table
        inRet2 = inCTOS_LoadCDTIndex();
        if (d_OK != inRet2)
        {
            if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
                inCTOSS_CLMCancelTransaction();

            CTOS_KBDBufFlush();

            if(inRet2 == CTLS_EXPIRED_CARD)
                return inRet2;
            else
                return USER_ABORT;
        }



#if 0
        //check if CARD is DEBIT CARD - must Swipe AGAIN if card entry is CTLS
		 if (strCDT.HDTid == 5 && strCDT.fPANCatchAll == TRUE){
		 	inCTOSS_CLMCancelTransaction();
		 	vdCTOS_ResetMagstripCardData();
			vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
			inEntryMode = SWIPE_ONLY;
			vduiClearBelow(2);
			memset(&stRCDataAnalyze,0x00,sizeof(EMVCL_RC_DATA_ANALYZE));
			srTransRec.byUploaded = CN_FALSE;
			srTransRec.byOffline = CN_TRUE;
			if (srTransRec.byTransType == SALE_OFFLINE){
				vdCTOS_SetTransType(SALE);
				vdDispTransTitle(SALE);
			}
			 goto SWIPE_AGAIN;
		 }
		 //end
#endif
        //Enhancement to prompt user to PLEASE INSERT/SWIPE CARD if CTLS Debit card is tapped
        if (strTCT.fATPBinRoute == 0 &&
            //(strCDT.HDTid == DEBIT_HDT_INDEX ||  strCDT.HDTid == DEBIT2_HDT_INDEX || strCDT.HDTid == FLEET_HDT_INDEX || strCDT.HDTid == DINERS_HDT_INDEX
            //diners emv
            (strCDT.HDTid == DEBIT_HDT_INDEX ||	strCDT.HDTid == DEBIT2_HDT_INDEX || strCDT.HDTid == FLEET_HDT_INDEX

             || (strCDT.fDCCEnable == TRUE && inFLGGet("fDCCCTLS") == FALSE)))
        {
            vdResetAllCardData();
            fNoCTLSSupportforBinRoute = TRUE;
            return (inCTOS_GetCardFieldsFailedCtlsFallback());

        }

        //Chekck if CARD is PAYPASS MAG and compare with Floor Limit
        vdDebug_LogPrintf("**** inCTOSS_WaveAnalyzeTransaction 2 ****");

        if (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MAG_STRIPE){

            if (fAmountLessThanFloorLimit() == d_OK)
                srTransRec.bWaveSCVMAnalysis = d_CVM_REQUIRED_NONE;
            else
                srTransRec.bWaveSCVMAnalysis = d_CVM_REQUIRED_SIGNATURE;
            vdDebug_LogPrintf("stRCDataAnalyze.bCVMAnalysis 2 %d %x",srTransRec.bWaveSCVMAnalysis, srTransRec.bWaveSCVMAnalysis);
        }


        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())

            inCTOSS_CLMCancelTransaction();
    }

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        vdDebug_LogPrintf("inCTOS_WaveGetCardFields, CARD_ENTRY_ICC");		//Tine:  07MAY2019
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);

	if (srTransRec.byEntryMode != CARD_ENTRY_WAVE)
		CTOS_Beep(); // add beep sound after pass on CDT validation -- sidumili
	
    return d_OK;
}

int inCTOS_GetCardFieldsFailedCtlsFallback(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTitle[20];
    BYTE szDisMsg[50];
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES_50], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES_50 ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
    int inRetVal;
    BOOL fManualFlag = TRUE;
    int inKey;
    BYTE szDisplay[200];

    CTOS_RTC SetRTC;

    //0826
    int inChipTries=0;
    int inEntryMode=0;
    /*
	1= insert only
	2= swipe only
	0= will accept al
	*/

	BYTE szManualEntryFlag[2];
	char szAmtMsg[45 + 1];
	char szTotalAmt[45 + 1];

#define INSERT_ONLY 1
#define SWIPE_ONLY  2
#define READ_ALL 0
    //0826

    short shReturn = d_OK; //Invalid card reading fix -- jzg

    /* BDO CLG: MOTO setup -- jzg */
    int inMOTOResult;

	vdDebug_LogPrintf("saturn inCTOS_GetCardFieldsFailedCtlsFallback...");

	// get transaction title
    memset(szTitle, 0x00, sizeof(szTitle));
    //szGetTransTitle(srTransRec.byTransType, szTitle);
    szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);

	// get manual entry flag
	memset(szManualEntryFlag, 0x00, sizeof(szManualEntryFlag));
	if(strTCT.fEnableManualKeyEntry == 1)
        strcpy(szManualEntryFlag, "1");
    else
        strcpy(szManualEntryFlag, "0");

	// Get Amount
	memset(szAmtMsg, 0x00, sizeof(szAmtMsg));
	memset(szTotalAmt, 0x00, sizeof(szTotalAmt));
	wub_hex_2_str(srTransRec.szTotalAmount, szTotalAmt, AMT_BCD_SIZE);
	vdCTOS_FormatAmount(strCST.szAmountFormat, szTotalAmt, szAmtMsg);

    fApplNotAvailable = FALSE;

	vdDebug_LogPrintf("saturn inCTOS_GetCardFieldsFailedCtlsFallback %d", inGblCtlsErr);
	vdDebug_LogPrintf("fEntryCardfromIDLE=[%d]", fEntryCardfromIDLE);
	vdDebug_LogPrintf("fIdleInsert=[%d]", fIdleInsert);
	vdDebug_LogPrintf("fIdleSwipe=[%d]", fIdleSwipe);
	
    srTransRec.fPrintSMCardHolder = FALSE;
    srTransRec.fPrintCardHolderBal = FALSE;
    /* BDO CLG: MOTO setup - start -- jzg */
    if (strTCT.fMOTO == 1)
    {

        CTOS_LCDTClearDisplay();

        //issue#00159
        if(srTransRec.byTransType == BIN_VER)
        {
            vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
            //CTOS_KBDBufFlush();
            return USER_ABORT;
        }
        //issue00159

        vdDispTransTitle(srTransRec.byTransType);
        //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
        CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

        while (1)
        {
            CTOS_LCDTPrintXY(1, 3, "Enter/Cancel");

            if(CTOS_TimeOutCheck(TIMER_ID_1) == d_YES)
            {
                fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
                return READ_CARD_TIMEOUT ;
            }

            CTOS_KBDInKey(&byKeyBuf);
            if(byKeyBuf)
            {
                CTOS_KBDGet(&byKeyBuf);
                switch(byKeyBuf)
                {
                    case d_KBD_0:
                    case d_KBD_1:
                    case d_KBD_2:
                    case d_KBD_3:
                    case d_KBD_4:
                    case d_KBD_5:
                    case d_KBD_6:
                    case d_KBD_7:
                    case d_KBD_8:
                    case d_KBD_9:
                        if(srTransRec.byTransType == BALANCE_INQUIRY)
                        {
                            if (strTCT.fSMMode==FALSE)
                                vdDisplayErrorMsgResp2(" ","MANUAL ENTRY","NOT ALLOWED");
                            else
                                vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");

                            CTOS_KBDBufFlush();
                            break;
                        }
                    case d_KBD_CANCEL:
                        if (byKeyBuf == d_KBD_CANCEL)
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }

                        memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

                        vdSetFirstIdleKey(byKeyBuf);
                        vdDebug_LogPrintf("szPAN2[%s]", srTransRec.szPAN);

                        //get the card number and ger Expire Date
                        if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
                        {
                            vdSetFirstIdleKey(0x00);
                            CTOS_KBDBufFlush ();
                            return USER_ABORT;
                        }

                        inMOTOResult = inCTOS_LoadCDTIndex();
                        //Load the CDT table
                        if (d_OK != inMOTOResult)
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }


                        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
                        //issue#-00159
#if 0
                    if(strCDT.fManEntry == FALSE)
						{
							vdDisplayErrorMsg(1, 8, "MKE NOT ALLOWED");
							CTOS_KBDBufFlush();
							return USER_ABORT;
						}
#endif
                        //issue00159

                        return d_OK; //BDO-00160: To properly exit the function if there's a valid entry -- jzg
                        break;
                }
            }
        }

        return d_OK;

    }
    /* BDO CLG: MOTO setup - end -- jzg */

    if (fIdleInsert == TRUE){

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if (!(bySC_status & d_MK_SC_PRESENT)){
            fEntryCardfromIDLE = FALSE;
            fIdleInsert=FALSE;
            srTransRec.byEntryMode = 0;
        }
    }


    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    //gcitra-0806
    //CTOS_LCDTClearDisplay();
    //gcitra-0806

    // patrick fix code 20141222 case 179
    if (fEntryCardfromIDLE != TRUE)
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

    if(fIdleSwipe != TRUE) //aaronnino for BDOCLG ver 9.0 fix on issue #00059 Card entry is recognized even on non Card Entry Prompt or non Idle Screen display 8 of 8
        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);


    SWIPE_AGAIN:


    if(d_OK != inCTOS_ValidFirstIdleKey())
    {

        CTOS_LCDTClearDisplay();

        /* BDO CLG: Fleet card support - start -- jzg */
        //if(srTransRec.fFleetCard == TRUE)
        //	vdDispTransTitle(FLEET_SALE);
        //else
        /* BDO CLG: Fleet card support - end -- jzg */
        vdDispTransTitle(srTransRec.byTransType);

        //gcitra-0728
        //inCTOS_DisplayIdleBMP();
        //gcitra-0728
    }
// patrick ECR 20140516 start
#if 0
    if (strTCT.fECR) // tct
    {
    	if (memcmp(srTransRec.szBaseAmount, "\x00\x00\x00\x00\x00\x00", 6) != 0)
    	{
    		char szDisplayBuf[30] = {0};
    		BYTE szTemp1[30+1] = {0};
			BYTE szTemp2[30+1] = {0};

			// sidumili: Issue#:000076 [check transaction maximum amount]
			if (inCTOS_ValidateTrxnAmount()!= d_OK){
				return(d_NO);
			}

			CTOS_LCDTPrintXY(1, 7, "AMOUNT:");
    		memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
    		//wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, AMT_BCD_SIZE);
    		wub_hex_2_str(srTransRec.szBaseAmount, szTemp1, 6);  /*BDO: Display the amount properly via ecr -- sidumili*/
			vdCTOS_FormatAmount(strCST.szAmountFormat, szTemp1, szTemp2);
			sprintf(szDisplayBuf, "%s %s", strCST.szCurSymbol, szTemp2);
    		CTOS_LCDTPrintXY(1, 8, szDisplayBuf);

    	}
    }
#endif
// patrick ECR 20140516 end
    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

    //CTOS_LCDTClearDisplay();
    //vdMyEZLib_Printf("srTransRec.byEntryMode: (%02x)", srTransRec.byEntryMode);

    /* BDO CLG: Fleet card support - start -- jzg */
    //if(srTransRec.fFleetCard == TRUE)
    //	vdDispTransTitle(FLEET_SALE);
    //else
    /* BDO CLG: Fleet card support - end -- jzg */
    vdDispTransTitle(srTransRec.byTransType);

    if (!fBINVer)
        //if(srTransRec.byEntryMode != CARD_ENTRY_MSR && srTransRec.byEntryMode != CARD_ENTRY_ICC)
        //{
        //vdDispTransTitle(srTransRec.byTransType);
        
        if (inEntryMode  == SWIPE_ONLY)
        {
            //CTOS_LCDTPrintXY(1, 3, "Please Swipe");
            //CTOS_LCDTPrintXY(1, 4, "Customer Card");
			//memset(szTitle, 0x00, sizeof(szTitle));
    		//szGetTransTitle(srTransRec.byTransType, szTitle);

    		//strcpy(szDisMsg, szTitle);
    		//strcat(szDisMsg, "|");
    		//strcat(szDisMsg, "PLEASE SWIPE CUSTOMER CARD");
    		//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "CARD ENTRY");
			//strcat(szDisMsg, "|");
            //strcat(szDisMsg, "0");

			memset(szDisMsg, 0x00, sizeof(szDisMsg));
			strcpy(szDisMsg, szTitle);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "Please swipe customer card");
			strcat(szDisMsg, "|");
			strcat(szDisMsg, strCST.szCurSymbol);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, szAmtMsg);
			strcat(szDisMsg, "|");
			strcat(szDisMsg, "0");
			if(fEntryCardfromIDLE ==  FALSE)
    			inKey=usCARDENTRY(szDisMsg);
        }
        else
        {
        

            /* Issue# 000113 - start -- jzg */
            if (inFallbackToMSR != SUCCESS)
            {
                //BDO: Parameterized manual key entry for installment - start --jzg
                //if((fInstApp == TRUE) && (strTCT.fEnableInstMKE == FALSE))
#if 0
                if (strTCT.fEnableManualKeyEntry == FALSE) //aaronnino for BDOCLG ver 9.0 fix on issue #0061 Manual Entry should not be allowed for BIN Check transactions 6 of 7
            {
               if(fEntryCardfromIDLE == FALSE)
               {
                  if (strTCT.fEnableManualKeyEntry == TRUE)
                  {
                     CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/ENTER");
                     CTOS_LCDTPrintXY(1, 4, "CARD"); //aaronnino for BDOCLG ver 9.0 fix on issue #00126 Incorrect terminal display
                  }
                  else
                  {
                     CTOS_LCDTPrintXY(1, 3, "1. SWIPE/INSERT CARD");
                  }
               }
            }
			else
#endif
                {
                 

                    if (srTransRec.byTransType == BALANCE_INQUIRY)
                    {
                        if(strTCT.fEnableBalInqMKE == TRUE)
                        {
                            CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/ENTER");
                            CTOS_LCDTPrintXY(1, 4, "CARD");
                        }
                        else
                            CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");
                    }
                    else if(srTransRec.byTransType == BIN_VER)
                    {
                    
                        if(strTCT.fEnableBinVerMKE == TRUE)
                        {
                            CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/ENTER");
                            CTOS_LCDTPrintXY(1, 4, "CARD");
                        }
                        else
                            CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");
                    }
                    else
                    {
                      
					  vdDebug_LogPrintf("saturn test 10 %d", fEntryCardfromIDLE);
                        if(fEntryCardfromIDLE == FALSE)
                        {
                            vdDebug_LogPrintf("saturn 11 %d %d", fInstApp, strTCT.fEnableInstMKE);

							
                            if(fInstApp == TRUE && strTCT.fEnableInstMKE == FALSE)
                            {
                                
                                CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");
                            }
                            else
                            {
                                if (inGblCtlsErr == 1)
                                {
                                    
                                    vdDispTransTitle(srTransRec.byTransType);
                                    setLCDPrint(3, DISPLAY_POSITION_CENTER, "CTLS TRANSACTION");
                                    setLCDPrint(4, DISPLAY_POSITION_CENTER, "LIMIT EXCEEDED PLS");
                                    setLCDPrint(5, DISPLAY_POSITION_CENTER, "SWIPE/INSERT CARD");
                                    inGblCtlsErr = 0;
                                }
                                else if (inGblCtlsErr == 2)
                                {
                                    
                                    //vdDispTransTitle(srTransRec.byTransType);
                                    //setLCDPrint(3, DISPLAY_POSITION_CENTER, "CTLS CARD READ ERROR");
                                    //setLCDPrint(4, DISPLAY_POSITION_CENTER, "PLEASE SWIPE/");
                                    //setLCDPrint(5, DISPLAY_POSITION_CENTER, "INSERT CARD");
                                    //memset(szTitle, 0x00, sizeof(szTitle));
                                    //szGetTransTitle(srTransRec.byTransType, szTitle);
                                    //memset(szDisMsg, 0x00, sizeof(szDisMsg));
                                    //strcpy(szDisMsg, szTitle);
                                    //strcat(szDisMsg, "|");
                                    //strcat(szDisMsg, "PLEASE SWIPE/INSERT CARD");
									//strcat(szDisMsg, "|");
                                    //strcat(szDisMsg, "");
                                    //usCTOSS_LCDDisplay(szDisMsg);

									memset(szDisMsg, 0x00, sizeof(szDisMsg));
									strcpy(szDisMsg, szTitle);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "Please swipe/insert card");
									strcat(szDisMsg, "|");
									strcat(szDisMsg, strCST.szCurSymbol);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, szAmtMsg);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "0");
                                    usCARDENTRY(szDisMsg);
                                    //CTOS_Beep();
                                    //CTOS_Delay(1500);
                                    inGblCtlsErr = 0;
                                }
                                else if (inGblCtlsErr == 3)
                                {
                                    vdDispTransTitle(srTransRec.byTransType);
                                    setLCDPrint(3, DISPLAY_POSITION_CENTER, "CTLS TRANSACTION");
                                    setLCDPrint(4, DISPLAY_POSITION_CENTER, "LIMIT EXCEEDED PLS");
                                    setLCDPrint(5, DISPLAY_POSITION_CENTER, "SWIPE/INSERT CARD");
                                    inGblCtlsErr = 0;
                                }
                                else if (inGblCtlsErr == 0 && fNoCTLSSupportforBinRoute == TRUE)
                                {
                                    //vdDispTransTitle(srTransRec.byTransType);
                                    //CTOS_LCDTPrintXY(1, 3, "PLEASE INSERT/SWIPE");
                                    //CTOS_LCDTPrintXY(1, 4, "CARD");
                                    
                                    //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");
                                    vdDebug_LogPrintf("saturn test 15");
                                    //memset(szTitle, 0x00, sizeof(szTitle));
                                    //szGetTransTitle(srTransRec.byTransType, szTitle);
                                    //memset(szDisMsg, 0x00, sizeof(szDisMsg));
                                    //strcpy(szDisMsg, szTitle);
                                    //strcat(szDisMsg, "|");
                                    //strcat(szDisMsg, "PLEASE SWIPE/INSERT CARD");
									//strcat(szDisMsg, "|");
                                    //strcat(szDisMsg, "");
									//strcat(szDisMsg, "|");
									//strcat(szDisMsg, "0");
                                    //usCTOSS_LCDDisplay(szDisMsg);

									memset(szDisMsg, 0x00, sizeof(szDisMsg));
									strcpy(szDisMsg, szTitle);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "Please swipe/insert card");
									strcat(szDisMsg, "|");
									strcat(szDisMsg, strCST.szCurSymbol);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, szAmtMsg);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "0");
                                    usCARDENTRY(szDisMsg);
                                }
                                else if ((strTCT.fEnableManualKeyEntry == TRUE) && (inGblCtlsErr == 0))
                                {
                                    //vdDispTransTitle(srTransRec.byTransType);
                                    //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT/ENTER");
                                    //CTOS_LCDTPrintXY(1, 4, "CARD");
                                    
                                    //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");
                                    vdDebug_LogPrintf("saturn test 16");
                                    //memset(szTitle, 0x00, sizeof(szTitle));
                                    //szGetTransTitle(srTransRec.byTransType, szTitle);
                                    //memset(szDisMsg, 0x00, sizeof(szDisMsg));
                                    //strcpy(szDisMsg, szTitle);
                                    //strcat(szDisMsg, "|");
                                    //strcat(szDisMsg, "PLEASE SWIPE/INSERT CARD");
									//strcat(szDisMsg, "|");
                                    //strcat(szDisMsg, "");
                                    //usCTOSS_LCDDisplay(szDisMsg);

									memset(szDisMsg, 0x00, sizeof(szDisMsg));
									strcpy(szDisMsg, szTitle);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "Please swipe/insert card");
									strcat(szDisMsg, "|");
									strcat(szDisMsg, strCST.szCurSymbol);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, szAmtMsg);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "0");
                                    usCARDENTRY(szDisMsg);
                                }
                                else if (inGblCtlsErr == 0)
                                {
                                    //vdDispTransTitle(srTransRec.byTransType);
                                    //CTOS_LCDTPrintXY(1, 3, "SWIPE/INSERT CARD");
                                    vdDebug_LogPrintf("saturn test 14");
                                    //memset(szTitle, 0x00, sizeof(szTitle));
                                    //szGetTransTitle(srTransRec.byTransType, szTitle);
                                    //memset(szDisMsg, 0x00, sizeof(szDisMsg));
                                    //strcpy(szDisMsg, szTitle);
                                    //strcat(szDisMsg, "|");
                                    //strcat(szDisMsg, "PLEASE SWIPE/INSERT CARD");
									//strcat(szDisMsg, "|");
                                    //strcat(szDisMsg, "");
                                    //usCTOSS_LCDDisplay(szDisMsg);

									memset(szDisMsg, 0x00, sizeof(szDisMsg));
									strcpy(szDisMsg, szTitle);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "Please swipe/insert card");
									strcat(szDisMsg, "|");
									strcat(szDisMsg, strCST.szCurSymbol);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, szAmtMsg);
									strcat(szDisMsg, "|");
									strcat(szDisMsg, "0");
                                    usCARDENTRY(szDisMsg);
                                }
                            }
                        }
                    }
                }
                fEntryCardfromIDLE = FALSE;
            }
            /* Issue# 000113 - end -- jzg */
        }
    //}



//0826
    ing_KeyPressed = 0;
    INSERT_AGAIN:
//0826
    while (1)
    {

        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES){
            fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            return READ_CARD_TIMEOUT;
        }

		if (ing_KeyPressed == 'C')
		{
			CTOS_KBDBufPut('C');
			vdDebug_LogPrintf("putchar C");
			//szGetTransTitle(srTransRec.byTransType, szTitle);
			//strcpy(szDisplay, szTitle);
			//strcat(szDisplay, "|");
			//strcat(szDisplay, "USER CANCEL");
			//usCTOSS_LCDDisplay(szDisplay);
			//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			//CTOS_Delay(1500);
			return USER_ABORT;
		}
		else if (ing_KeyPressed == 'T')
		{
			CTOS_KBDBufPut('T');
			vdDebug_LogPrintf("putchar T");
			//szGetTransTitle(srTransRec.byTransType, szTitle);
			//strcpy(szDisplay, szTitle);
			//strcat(szDisplay, "|");
			//strcat(szDisplay, "TIME OUT");
			//usCTOSS_LCDDisplay(szDisplay);
			//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
			//CTOS_Delay(1500);
			return READ_CARD_TIMEOUT ;
		}

        /*sidumili: Issue#: 000086*/
        //enhance ecr - removed
        //if ((strTCT.fECR) &&(fECRTxnFlg)){
        //vdSetErrorMessage("ECR TIMEOUT");
        //}
        //enhance ecr - removed

        CTOS_KBDInKey(&byKeyBuf);

//gcitra-removed - remove part where card entry is allowed during IDLE mode


        // patrick add code 20141209
        if (byKeyBuf)
        {
            CTOS_KBDGet(&byKeyBuf);
            switch(byKeyBuf)
            {
                case d_KBD_0:
                case d_KBD_1:
                case d_KBD_2:
                case d_KBD_3:
                case d_KBD_4:
                case d_KBD_5:
                case d_KBD_6:
                case d_KBD_7:
                case d_KBD_8:
                case d_KBD_9:

                    //fEnableBinVerMKE = MKE for BIN VER
                    if(srTransRec.byTransType == BIN_VER){
                        if (strTCT.fEnableBinVerMKE == FALSE)
                            CTOS_KBDBufFlush();
                    }
                    else if(srTransRec.byTransType == BALANCE_INQUIRY){
                        if(strTCT.fEnableBalInqMKE == FALSE)
                            CTOS_KBDBufFlush();
                    }
                        //fEnableInstMKE = MKE General flag for installment
                    else if (fInstApp == TRUE){
                        if (strTCT.fEnableInstMKE == FALSE)
                            CTOS_KBDBufFlush();
                        //fEnableManualKeyEntry - Flag for all other hosts
                    }else{
                        //if (strTCT.fEnableManualKeyEntry == FALSE)
                        CTOS_KBDBufFlush();
                    }

                case d_KBD_CANCEL:

                    //gcitra-0728
                    if (byKeyBuf == d_KBD_CANCEL)
                    {
                        CTOS_KBDBufFlush();
                        return USER_ABORT;
                    }
                    //gcitra-0728

                    if(srTransRec.byTransType == BIN_VER && strTCT.fEnableBinVerMKE == FALSE || srTransRec.byTransType == BALANCE_INQUIRY && strTCT.fEnableBalInqMKE == FALSE)
                        fManualFlag = 0;
                    if(fInstApp == TRUE && strTCT.fEnableInstMKE == FALSE)
                        fManualFlag = 0;
                    if(inFallbackToMSR == SUCCESS)
                        fManualFlag = 0;
                    if(strTCT.fEnableManualKeyEntry == FALSE)
                        fManualFlag = 0;
#if 0
                if(fManualFlag == 1)
						 {
								vdSetFirstIdleKey(byKeyBuf);
								memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

								//get the card number and ger Expire Date
								if (d_OK != inCTOS_ManualEntryProcess(srTransRec.szPAN))
								{
									//gcitra - remove part where card entry is allowed during IDLE mode
									//vdSetFirstIdleKey(0x00);
									//gcitra
									CTOS_KBDBufFlush();
									//vdSetErrorMessage("Get Card Fail M");
									return USER_ABORT;
								}

								//Load the CDT table
								if (d_OK != inCTOS_LoadCDTIndex())
								{
									CTOS_KBDBufFlush();
									return USER_ABORT;
								}

								if(fInstApp != TRUE)
									if(strCDT.fManEntry == FALSE)
									{
										vdDisplayErrorMsgResp2("MANUAL", "KEY ENTRY ", "NOT ALLOWED");
										CTOS_KBDBufFlush();
										return USER_ABORT;
									}

								break;
						  }
#endif
            }
        }

//gcitra
//0826

        if (inEntryMode != SWIPE_ONLY){
//INSERT_AGAIN:

            if (inEntryMode == INSERT_ONLY){
                byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
                vdCTOS_ResetMagstripCardData();

                /* BDO CLG: Fleet card support - start -- jzg */
                //if(srTransRec.fFleetCard == TRUE)
                //	vdDispTransTitle(FLEET_SALE);
                //else
                /* BDO CLG: Fleet card support - end -- jzg */
                //vdDispTransTitle(srTransRec.byTransType);

                //CTOS_LCDTPrintXY(1, 4, "PLEASE INSERT CARD/");
                //CTOS_LCDTPrintXY(1, 5, "CANCEL");

                //memset(szTitle, 0x00, sizeof(szTitle));
                //szGetTransTitle(srTransRec.byTransType, szTitle);
                //memset(szDisMsg, 0x00, sizeof(szDisMsg));
                //strcpy(szDisMsg, szTitle);
                //strcat(szDisMsg, "|");
                //strcat(szDisMsg, "PLEASE INSERT CARD");
				//strcat(szDisMsg, "|");
	            //strcat(szDisMsg, "CARD ENTRY");
				//strcat(szDisMsg, "|");
	            //strcat(szDisMsg, "0");
                //usCTOSS_LCDDisplay(szDisMsg);

				memset(szDisMsg, 0x00, sizeof(szDisMsg));
				strcpy(szDisMsg, szTitle);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "Please insert card");
				strcat(szDisMsg, "|");
				strcat(szDisMsg, strCST.szCurSymbol);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, szAmtMsg);
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "0");						
                usCARDENTRY(szDisMsg);
                CTOS_Beep();
                CTOS_Delay(1500);

                ing_KeyPressed = 0;

                while(1){
                    if (ing_KeyPressed == 'C')
                    {
                        CTOS_KBDBufPut('C');
                        vdDebug_LogPrintf("putchar C");
                        return USER_ABORT;
                    }

                    CTOS_SCStatus(d_SC_USER, &bySC_status);
                    if(bySC_status & d_MK_SC_PRESENT)
                    {
                        //1010
                        CTOS_LCDTPrintXY(1,8,"");
                        //1010
                        //CTOS_Delay(2000);

                        vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

                        //vdDebug_LogPrintf("--EMV flow----" );
                        //if (d_OK != inCTOS_EMVCardReadProcess ())
                        vdClearBelowLine(2);
                        shReturn = inCTOS_EMVCardReadProcess();

                        if (shReturn != d_OK)
                        {


                            if ((shReturn==EMV_TRANS_NOT_ALLOWED) || (shReturn == EMV_FAILURE_EX || shReturn == EMV_APPL_NOT_AVAILABLE)){
                                vduiClearBelow(2);
                                vdCTOS_ResetMagstripCardData();
                                vdRemoveCard();
                                inEntryMode = SWIPE_ONLY;
                                if(shReturn == EMV_APPL_NOT_AVAILABLE)
                                {
                                    vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
                                    inFallbackToMSR = FAIL;
                                    fApplNotAvailable = TRUE;
                                }
                                else
                                {
                                    vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                                    inFallbackToMSR = SUCCESS;
                                }

                                vduiClearBelow(2);
                                goto SWIPE_AGAIN;
                            }


                            if(inFallbackToMSR == SUCCESS)
                            {
                                vdCTOS_ResetMagstripCardData();
                                //0826
                                //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                                //goto SWIPE_AGAIN;
                                vduiClearBelow(2);
                                vdRemoveCard();
                                clearLine(7);
                                inChipTries= inChipTries+1;
                                if (inChipTries < 3){
                                    inEntryMode = INSERT_ONLY;
                                    goto INSERT_AGAIN;
                                }else{
                                    inEntryMode = SWIPE_ONLY;
                                    //1125
                                    vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                                    //1125
                                    goto SWIPE_AGAIN;
                                }
                                //0826
                            }else{
                                //vdSetErrorMessage("Get Card Fail C");
                                return USER_ABORT;
                            }
                        }

                        //0826
                        inEntryMode = READ_ALL;
                        //0826

                        vdDebug_LogPrintf("--EMV Read succ----" );
                        //Load the CDT table
                        if(fNoCTLSSupportforBinRoute == TRUE && inGetATPBinRouteFlag() == TRUE && strTCT.fATPBinRoute == TRUE)
                        {
                            CTOS_LCDTClearDisplay();
                            inSETIssuerForCatchAll();//Set new issuer based on AID to overwrite the issuer of the tapped card.
                            inRetVal = d_OK;
                        }
                        else
                            inRetVal = inCTOS_LoadCDTIndex();

                        if (d_OK != inRetVal)
                        {
                            CTOS_KBDBufFlush();
                            return USER_ABORT;
                        }

                        break;
                    }

                }


            }
//0826
            if (ing_KeyPressed == 'C')
            {
                CTOS_KBDBufPut('C');
                vdDebug_LogPrintf("putchar C");
                return USER_ABORT;
            }

            CTOS_SCStatus(d_SC_USER, &bySC_status);
            if(bySC_status & d_MK_SC_PRESENT)
            {
                //1010
                CTOS_LCDTPrintXY(1,8,"");
                //1010
                //CTOS_Delay(2000);

                vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);

                //vdDebug_LogPrintf("--EMV flow----" );
                //if (d_OK != inCTOS_EMVCardReadProcess ())
                vdClearBelowLine(2);
                shReturn = inCTOS_EMVCardReadProcess();

                if (shReturn != d_OK)
                {


                    if ((shReturn==EMV_TRANS_NOT_ALLOWED) || (shReturn == EMV_FAILURE_EX || shReturn == EMV_APPL_NOT_AVAILABLE)){
                        vduiClearBelow(2);
                        vdCTOS_ResetMagstripCardData();
                        vdRemoveCard();
                        inEntryMode = SWIPE_ONLY;
                        if(shReturn == EMV_APPL_NOT_AVAILABLE)
                        {
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_MSR);
                            inFallbackToMSR = FAIL;
                            fApplNotAvailable = TRUE;
                        }
                        else
                        {
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                            inFallbackToMSR = SUCCESS;
                        }

                        vduiClearBelow(2);
                        goto SWIPE_AGAIN;
                    }


                    if(inFallbackToMSR == SUCCESS)
                    {
                        vdCTOS_ResetMagstripCardData();
                        //0826
                        //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
                        //goto SWIPE_AGAIN;
                        vduiClearBelow(2);
                        vdRemoveCard();
                        clearLine(7);
                        inChipTries= inChipTries+1;
                        if (inChipTries < 3){
                            inEntryMode = INSERT_ONLY;
                            goto INSERT_AGAIN;
                        }else{
                            inEntryMode = SWIPE_ONLY;
                            //1125
                            vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
                            //1125
                            goto SWIPE_AGAIN;
                        }
                        //0826
                    }else{
                        //vdSetErrorMessage("Get Card Fail C");
                        return USER_ABORT;
                    }
                }

                //0826
                inEntryMode = READ_ALL;
                //0826

                vdDebug_LogPrintf("--EMV Read succ----" );
                //Load the CDT table
                if(fNoCTLSSupportforBinRoute == TRUE && inGetATPBinRouteFlag() == TRUE && strTCT.fATPBinRoute == TRUE)
                {
                    CTOS_LCDTClearDisplay();
                    inSETIssuerForCatchAll();//Set new issuer based on AID to overwrite the issuer of the tapped card.
                    inRetVal = d_OK;
                }
                else
                    inRetVal = inCTOS_LoadCDTIndex();

                if (d_OK != inRetVal)
                {
                    CTOS_KBDBufFlush();
                    return USER_ABORT;
                }

                break;
            }


            //0826
            if (inEntryMode == INSERT_ONLY)
                goto INSERT_AGAIN;


        }
        //0826

        //for Idle swipe card

        if (fBINVer)
            break;

        if (strlen(srTransRec.szPAN) > 0)
        {


            if(fNoCTLSSupportforBinRoute == TRUE && inGetATPBinRouteFlag() == TRUE && strTCT.fATPBinRoute == TRUE)
            {
                CTOS_LCDTClearDisplay();
                inSETIssuerForCatchAll();//Set new issuer based on AID to overwrite the issuer of the tapped card.
                inRetVal = d_OK;
            }
            else
                inRetVal = inCTOS_LoadCDTIndex();

            if (d_OK != inRetVal)

            {
                CTOS_KBDBufFlush();
                //vdSetErrorMessage("Get Card Fail");
                return USER_ABORT;
            }

            if(d_OK != inCTOS_CheckEMVFallback())
            {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "5.PLS INSERT CARD");

                //issue#11
                vduiClearBelow(2);
                inEntryMode = INSERT_ONLY;
                goto INSERT_AGAIN;
                //goto SWIPE_AGAIN;
                //issue#11

            }

            break;

        }


        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

        /* BDOCLG-00187: Return to idle if card is incorrectly swipe at idle screen - start -- jzg */
        if(((byMSR_status != d_OK) && (fEntryCardfromIDLE == TRUE) && (inEntryMode == READ_ALL)) || (byMSR_status == 0x02))
        {
            inCTOSS_CLMCancelTransaction();
            //CTOS_LCDTClearDisplay();
            //vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
            //Tine:  23MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, szTitle);
            strcat(szDisMsg, "|");
            strcat(szDisMsg, "CARD READ ERROR");
            //usCTOSS_LCDDisplay(szDisMsg);
            inDisplayMessageBoxWithButtonConfirmation(1,8,"","CARD READ ERROR","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
            CTOS_Beep();
            CTOS_Delay(1500);
            CTOS_Beep();
            return INVALID_CARD;
        }
        /* BDOCLG-00187: Return to idle if card is incorrectly swipe at idle screen - end -- jzg */

        //gcitra
        //if((byMSR_status == d_OK ) && (usTk2Len > 35))
        if(byMSR_status == d_OK )
            //gcitra
        {
            //Invalid card reading fix - start -- jzg
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

            if (shReturn == INVALID_CARD)
            {
                CTOS_KBDBufFlush();
                //CTOS_LCDTClearDisplay();
                //vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
                //Tine: 23MAY2019
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "CARD READ ERROR");
                //usCTOSS_LCDDisplay(szDisMsg);
                inDisplayMessageBoxWithButtonConfirmation(1,8,"","CARD READ ERROR","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                CTOS_Beep();
                //CTOS_Delay(1500);
                //CTOS_Beep();
                return INVALID_CARD;
            }
            //Invalid card reading fix - end -- jzg

            if(inFallbackToMSR == SUCCESS)
            {
                vdCTOS_SetTransEntryMode(CARD_ENTRY_FALLBACK);
            }

            if(fNoCTLSSupportforBinRoute == TRUE && inGetATPBinRouteFlag() == TRUE && strTCT.fATPBinRoute == TRUE)
            {
                CTOS_LCDTClearDisplay();
                inRetVal = d_OK;
            }
            else
                inRetVal = inCTOS_LoadCDTIndex();

            if (d_OK != inRetVal)

            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }

            if(d_OK != inCTOS_CheckEMVFallback())
            {
                vdCTOS_ResetMagstripCardData();
                //vdDisplayErrorMsg(1, 8, "6.PLS INSERT CARD");		//Tine:  19JUN2019

                //issue#11
                //vduiClearBelow(2);
                inEntryMode = INSERT_ONLY;
                goto INSERT_AGAIN;
                //goto SWIPE_AGAIN;
                //issue#11

            }

            break;
        }
        else //aaronnino for BDOCLG ver 9.0 fix on issue #00187 Terminal cannot re-swipe card if 1st attempt is swiped improperly from idle menu A. SCENARIO start
        {
            if(fIdleSwipe == TRUE)
            {
                vdSetErrorMessage("CARD READ ERROR");
                return ST_ERROR;
            }
        }
        //aaronnino for BDOCLG ver 9.0 fix on issue #00187 Terminal cannot re-swipe card if 1st attempt is swiped improperly from idle menu  A. SCENARIO end

    }



    if (srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        EMVtagLen = 0;
        if(EMVtagLen > 0)
        {
            sprintf(srTransRec.szCardLable, "%s", EMVtagVal);
        }
        else
        {
            strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        }
    }
    else
    {
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    }
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);

	if (srTransRec.byEntryMode != CARD_ENTRY_WAVE)
		CTOS_Beep(); // add beep sound after pass on CDT validation -- sidumili
	
    return d_OK;
}



int inCTOS_PreConnectEx(void)
{
    int inRetVal;

	vdDebug_LogPrintf("--inCTOS_PreConnectEx--");
	usPreconnectStatus = 0;
	
    if (chGetInit_Connect() == TRUE)
        return(d_OK);

    vdDebug_LogPrintf("saturn PRE CONNECT EX");




    if(inCPTRead(1) != d_OK)
    {
        vdSetErrorMessage("LOAD CPT ERR");
        return(d_NO);
    }


    //vdSetInit_Connect(1);


    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;


	if ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) || (srTransRec.usTerminalCommunicationMode == WIFI_MODE  )/*|| (srTransRec.usTerminalCommunicationMode == ETHERNET_MODE)*/)
		return d_OK;

        //if ((fBINVer == VS_TRUE) && ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) || (srTransRec.usTerminalCommunicationMode == ETHERNET_MODE) || (srTransRec.usTerminalCommunicationMode == WIFI_MODE)))
        //		return(d_OK);




        //if ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) && (fGPRSConnectOK != TRUE)){
        //	vdSetErrorMessage("GPRS NOT ESTABLISHED");
        //  return(d_NO);
        //}


        vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);
		vdSetInit_Connect(1);


    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK)
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
	vdDebug_LogPrintf("inCTOS_CheckInitComm = [%d]", inRetVal);

    if (inRetVal != d_OK)
    {
        if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
        {
            vdDisplayErrorMsgResp2("1.", "GPRS Problem","Please Try Again");
            vdSetErrorMessage("");
        }
            //wifi-mod2
        else if (srTransRec.usTerminalCommunicationMode == WIFI_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "WIFI Problem","Please Try Again");
            vdSetErrorMessage("");
        }
            //wifi-mod2
        else if (srTransRec.usTerminalCommunicationMode == ETHERNET_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "Check LAN","Connectivity");
            vdSetErrorMessage("");
        }
        else if (srTransRec.usTerminalCommunicationMode == DIAL_UP_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "Check Phone Line", "Connectivity");
            vdSetErrorMessage("");
        }
        else
        {
            vdDisplayErrorMsgResp2(" ", "Connect Failed","Please Try Again");
            vdSetErrorMessage("");
        }
        return(d_NO);
    }

    if(srTransRec.usTerminalCommunicationMode == ETHERNET_MODE)
        return(d_OK);

    if (CN_FALSE == srTransRec.byOffline)
    {
        isPredial= 0;

        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);


    }

    return(d_OK);
}


int inCTOS_PreConnect(void)
{
    int inRetVal;
	BYTE szTitle[20+1];
	BYTE szDisMsg[40];
	BYTE szErrorMsg[40];


    //if (chGetInit_Connect() == TRUE)
    //    return(d_OK);

    vdDebug_LogPrintf("saturn inCTOS_PreConnect");

	vdDebug_LogPrintf("inHostIndex[%d]",strHDT.inHostIndex);
	
	vdDebug_LogPrintf("saturn strTCT.byTerminalType[%d]",strTCT.byTerminalType);
	//vdDebug_LogPrintf("ErmTrans_Approved[%d]",ErmTrans_Approved);
	vdDebug_LogPrintf("srTransRec.usTerminalCommunicationMode[%d]", srTransRec.usTerminalCommunicationMode);
    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);

	vdDebug_LogPrintf("srTransRec.byPackType[%d]",srTransRec.byPackType);
	vdDebug_LogPrintf("fSingleComms[%d]",strTCT.fSingleComms);
	vdDebug_LogPrintf("fDHCPEnable[%d]",strTCP.fDHCPEnable);
	vdDebug_LogPrintf("fShareComEnable[%d]",strTCT.fShareComEnable);
	vdDebug_LogPrintf("Primary[%s]port[%ld]",strCPT.szPriTxnHostIP,strCPT.inPriTxnHostPortNum);
	vdDebug_LogPrintf("Secondary[%s]port[%ld]",strCPT.szSecTxnHostIP,strCPT.inSecTxnHostPortNum);

    if (strTCT.fSingleComms || srTransRec.byPackType == DCC_RATEREQUEST || srTransRec.byPackType == DCC_RATEREQUEST_RETRY ||
        srTransRec.byPackType == DCC_LOGGING || srTransRec.byPackType == DCC_LOGGING_RETRY){
        if(inCPTRead(1) != d_OK)
        {
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }
    }


    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;

    if ((fBINVer == VS_TRUE) && ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) || (srTransRec.usTerminalCommunicationMode == ETHERNET_MODE) || (srTransRec.usTerminalCommunicationMode == WIFI_MODE)))
        return(d_OK);


	//vdSetInit_Connect(1);



    //if ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) && (fGPRSConnectOK != TRUE)){
    //	vdSetErrorMessage("GPRS NOT ESTABLISHED");
    //  return(d_NO);
    //}


    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);


    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK)
    {
        //vdSetErrorMessage("COMM INIT ERR");
        //vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
        vdDisplayErrorMsgResp2("","Initialization","Error");
        vdSetErrorMessage("");
        return(d_NO);
    }

    if(srTransRec.usTerminalCommunicationMode == WIFI_MODE)
        return(d_OK);

    if(VS_TRUE == strTCT.fDemo)
        return(d_OK);

    inRetVal = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);

    if (inRetVal != d_OK)
    {
        if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "GPRS Problem","Please Try Again");
            vdSetErrorMessage("");
        }
            //wifi-mod2
        else if (srTransRec.usTerminalCommunicationMode == WIFI_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "WIFI Problem","Please Try Again");
            vdSetErrorMessage("");
        }
            //wifi-mod2

        else
        {
            //vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
            vdDisplayErrorMsgResp2("","Initialization","Error");
            vdSetErrorMessage("");
        }

        if(srTransRec.byTransType == SETTLE)
            vdDisplayErrorMsgResp2(" ", " ","SETTLE FAILED");

        return(d_NO);
    }


    if (CN_FALSE == srTransRec.byOffline)
    {
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);
        //for improve transaction speed
    }

    return(d_OK);
}




int inCTOS_PreConnectAndInit(void)
{
    int inRetVal;

    vdDebug_LogPrintf("retrun saturn inCTOS_PreConnectAndInit");
	
	return inCTOS_PreConnect();
	
    vdDebug_LogPrintf("saturn inCTOS_PreConnectAndInit");

    if(strCPT.inCommunicationMode == DIAL_UP_MODE || strCPT.inCommunicationMode == ETHERNET_MODE)
        return d_OK;

    if ((fGPRSConnectOK != TRUE) && (strCPT.inCommunicationMode == GPRS_MODE)) // flag to determine whether need do comm connection or not.
    {
        vdDisplayErrorMsgResp2(" ", "GPRS Problem","Please Try Again");
        CTOS_Delay(2000);
        return d_NO;
    }


    vdDebug_LogPrintf("saturn inCTOS_PreConnectAndInit");
    vdSetInit_Connect(1);

    if (strTCT.fSingleComms || srTransRec.byPackType == DCC_RATEREQUEST || srTransRec.byPackType == DCC_RATEREQUEST_RETRY ||
        srTransRec.byPackType == DCC_LOGGING || srTransRec.byPackType == DCC_LOGGING_RETRY){
        if(inCPTRead(1) != d_OK)
        {
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }
    }


    srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;

    if ((fBINVer == VS_TRUE) && ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) || (srTransRec.usTerminalCommunicationMode == ETHERNET_MODE) || (srTransRec.usTerminalCommunicationMode == WIFI_MODE)))
        return(d_OK);




    //if ((srTransRec.usTerminalCommunicationMode == GPRS_MODE) && (fGPRSConnectOK != TRUE)){
    //	vdSetErrorMessage("GPRS NOT ESTABLISHED");
    //  return(d_NO);
    //}


    vdDebug_LogPrintf("saturn strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);


    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK)
    {
        //vdSetErrorMessage("COMM INIT ERR");
        //vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
        vdDisplayErrorMsgResp2("","Initialization","Error");
        vdSetErrorMessage("");
        return(d_NO);
    }

    if (get_env_int("CONNECTED") == 1)
        return(d_OK);

    //if(srTransRec.usTerminalCommunicationMode == WIFI_MODE)
    //return(d_OK);

    if(VS_TRUE == strTCT.fDemo)
        return(d_OK);

    inRetVal = inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);

    if (inRetVal != d_OK)
    {
        if (srTransRec.usTerminalCommunicationMode == GPRS_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "GPRS Problem","Please Try Again");
            vdSetErrorMessage("");
        }
            //wifi-mod2
        else if (srTransRec.usTerminalCommunicationMode == WIFI_MODE)
        {
            vdDisplayErrorMsgResp2(" ", "WIFI Problem","Please Try Again");
            vdSetErrorMessage("");
        }
            //wifi-mod2

        else
        {
            //vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
            vdDisplayErrorMsgResp2("","Initialization","Error");
            vdSetErrorMessage("");
        }

        if(srTransRec.byTransType == SETTLE)
            vdDisplayErrorMsgResp2(" ", " ","SETTLE FAILED");

        return(d_NO);
    }


    if (CN_FALSE == srTransRec.byOffline)
    {
        inRetVal = srCommFuncPoint.inCheckComm(&srTransRec);
        //for improve transaction speed
    }


    put_env_int("CONNECTED",1);

    return(d_OK);
}



int inCTOS_PreConnect2(void)
{
    int inRetVal;

    //srTransRec.usTerminalCommunicationMode = strCPT.inCommunicationMode;

    vdDebug_LogPrintf("strCPT.inCommunicationMode[%d]",strCPT.inCommunicationMode);


    if (inCTOS_InitComm(srTransRec.usTerminalCommunicationMode) != d_OK)
    {
        //vdSetErrorMessage("COMM INIT ERR");
        //vdDisplayErrorMsgResp2(" ", " ", "COMM INIT ERR");
        vdDisplayErrorMsgResp2("","Initialization","Error");
        vdSetErrorMessage("");
        return(d_NO);
    }
    inCTOS_CheckInitComm(srTransRec.usTerminalCommunicationMode);


    if (CN_FALSE == srTransRec.byOffline)
    {
        inRetVal = inModem_CheckComm2(&srTransRec);

    }

    return(d_OK);
}

extern isPredial;
int inModem_CheckComm2(TRANS_DATA_TABLE *srTransPara)
{

    int shRet;

    vdDebug_LogPrintf("byTransType=[%d],szPriTxnPhoneNumber=[%s],szPriSettlePhoneNumber=[%s]",srTransPara->byTransType,strCPT.szPriTxnPhoneNumber,strCPT.szPriSettlePhoneNumber);

    if ((srTransPara->byTransType == SETTLE) || (srTransPara->byTransType == CLS_BATCH))
    {
        if(strlen(strCPT.szSecSettlePhoneNumber)>0)
        {
            memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
            sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szSecSettlePhoneNumber);
        }
    }
        /*BDO: Separate dialing number for BIN Ver - start -- jzg */
    else if (srTransPara->byTransType == BIN_VER)
    {
        inBVTRead(1);
        memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
        sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strBVT.szBINVerSecNum);
    }
        /*BDO: Separate dialing number for BIN Ver - end -- jzg */
    else
    {
        if(strlen(strCPT.szSecTxnPhoneNumber)>0)
        {
            memset(srModemRec.strRemotePhoneNum,0x00,sizeof(srModemRec.strRemotePhoneNum));
            sprintf(srModemRec.strRemotePhoneNum,"%s%s",strTCT.szPabx,strCPT.szSecTxnPhoneNumber);
        }
    }

    vdDebug_LogPrintf("default phone num =[%s],fPreDial[%d]",srModemRec.strRemotePhoneNum,strCPT.fPreDial);
    isPredial = 0;

    if (strCPT.fPreDial == TRUE)
    {
        shRet = CTOS_ModemDialup((BYTE *)srModemRec.strRemotePhoneNum,strlen(srModemRec.strRemotePhoneNum));

        isPredial = 1;

        vdDebug_LogPrintf("inModem_CheckComm2 - isPredial = %d",isPredial);
    }

    return ST_SUCCESS;
}



int inCTOS_CheckAndSelectMutipleMID(void)
{
#define ITEMS_PER_PAGE          4

    char szMMT[50];
    char szDisplay[50];
    int inNumOfRecords = 0;
    short shCount = 0;
    short shTotalPageNum;
    short shCurrentPageNum;
    short shLastPageItems = 0;
    short shPageItems = 0;
    short shLoop;
    short shFalshMenu = 1;
    BYTE isUP = FALSE, isDOWN = FALSE;
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
    char szHeaderString[50] = "SELECT MERCHANT";
    char szMitMenu[1024];
    int inLoop = 0;
    unsigned char bstatus = 0;
    memset(szMitMenu, 0x00, sizeof(szMitMenu));
    short shMinLen = 4;
    short shMaxLen = 6;
    BYTE Bret;
    unsigned char szOutput[30];


    vdDebug_LogPrintf("inCTOS_CheckAndSelectMutipleMID=[%d]",strHDT.inHostIndex);
    inMMTReadNumofRecords(strHDT.inHostIndex,&inNumOfRecords);
    CTOS_KBDBufFlush();//cleare key buffer
    //if(srTransRec.fDebit == TRUE && (srTransRec.byTransType == BAL_INQ || srTransRec.byTransType == LOG_ON))
    //inNumOfRecords=1;

    if((strTCT.byTerminalType % 2) != 0)
        inSetTextMode();


    if(inNumOfRecords > 1)
    {
        for (inLoop = 0; inLoop < inNumOfRecords; inLoop++)
        {
            strcat((char *)szMitMenu, strMMT[inLoop].szMerchantName);
            if(inLoop + 1 != inNumOfRecords)
                strcat((char *)szMitMenu, (char *)" \n");
        }

        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);

        if (key == 0xFF)
        {
            CTOS_LCDTClearDisplay();
            setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
            vduiWarningSound();
            return -1;
        }

        if(key > 0)
        {
            if(d_KBD_CANCEL == key)
                return -1;

            vdDebug_LogPrintf("key[%d]-------", key);
            memcpy(&strMMT[0],&strMMT[key-1],sizeof(STRUCT_MMT));
        }
    }
#if 0
    shCurrentPageNum = 1;
    CTOS_KBDBufFlush();//cleare key buffer
    if(inNumOfRecords > 1)
    {
        if (inNumOfRecords > ITEMS_PER_PAGE)
            isDOWN = TRUE;

        shTotalPageNum = (inNumOfRecords/ITEMS_PER_PAGE == 0) ? (inNumOfRecords/ITEMS_PER_PAGE) :(inNumOfRecords/ITEMS_PER_PAGE + 1);
        shLastPageItems = (inNumOfRecords/ITEMS_PER_PAGE == 0) ? (ITEMS_PER_PAGE) : (inNumOfRecords%ITEMS_PER_PAGE);

        do
        {
                //display items perpage
                if(shTotalPageNum == 0)//the total item is amaller than ITEMS_PER_PAGE
                {
                    shPageItems = inNumOfRecords;
                }
                else if(shCurrentPageNum == shTotalPageNum)//Last page
                    shPageItems = shLastPageItems;
                else
                    shPageItems = ITEMS_PER_PAGE;


                if(shFalshMenu)
                {
                    CTOS_LCDTClearDisplay();
                    vdDispTitleString("SELECT MERCHANT");
                    CTOS_LCDTPrintXY(2,8,"PLS SELECT");
                    for(shLoop=0 ; shLoop < shPageItems/*ITEMS_PER_PAGE*/ ; shLoop++)
                    {

                        memset(szDisplay,0,sizeof(szDisplay));
                        sprintf(szDisplay,"%d: %s",shLoop+1,strMMT[shLoop + (shCurrentPageNum -1)*ITEMS_PER_PAGE].szMerchantName);
                        CTOS_LCDTPrintXY(2,shLoop + 3,szDisplay);

                    }

					//#issue#=000137
					if (shCurrentPageNum > 1)
						isUP = TRUE;
					else
						isUP = FALSE;

					//000137
                    vdCTOS_LCDGShowUpDown(isUP,isDOWN);
                }


                key=WaitKey(60);

                switch(key)
                {
                    case d_KBD_DOWN:
					case d_KBD_DOT:

                        shFalshMenu = 1;

                        shCurrentPageNum ++;
                        if(shCurrentPageNum > shTotalPageNum)
                            shCurrentPageNum = 1;
                        bstatus = 2;
                        break;

                    //issue#-000137
                    case d_KBD_UP:

                        shCurrentPageNum --;
                        if(shCurrentPageNum < 1)
                            shCurrentPageNum = shTotalPageNum;
                        bstatus = 2;
                        break;
                    //000137
                    case d_KBD_CANCEL:
                        return FAIL;
                    case d_KBD_1:
                        //set the unique MMT num

                        memcpy(&strMMT[0],&strMMT[0 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                        bstatus = 0;
                        break;
                    case d_KBD_2:
                        if(shPageItems < 2)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[1 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;
                    case d_KBD_3:
                        if(shPageItems < 3)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[2 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    case d_KBD_4:
                        if(shPageItems < 4)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[3 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;
                    case d_KBD_5:
                        if(ITEMS_PER_PAGE < 5)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 5)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[4 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    case d_KBD_6:
                        if(ITEMS_PER_PAGE < 6)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 6)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[5 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    case d_KBD_7:
                        if(ITEMS_PER_PAGE < 7)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 7)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[6 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    case d_KBD_8:   //Max 8 items for one page
                        if(ITEMS_PER_PAGE < 8)
                        {
                            bstatus = -1 ;
                            break;
                        }
                        else
                        {
                            if(shPageItems < 8)
                            {
                                bstatus = -1;
                            }
                            else
                            {
                                memcpy(&strMMT[0],&strMMT[7 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                                bstatus = 0;
                            }
                            break;

                        }
                    default:
                        shFalshMenu = 0;
                        bstatus = -1 ;
                        break;

                }

                if((-1) == bstatus)
                {
                    vdSetErrorMessage("LOAD MMT ERR");
                    return FAIL;

                }
                else if(0 == bstatus)
                {
                    break;
                }

            }while(1);

    }
    else
    {
        //One merchant only
        //vduiDisplayStringCenter(1,strMMT[0].szMID);

    }
#endif
    //vdDebug_LogPrintf("key[%d]--fEnablePSWD[%d]-----", key,strMMT[key-1].fEnablePSWD);
    if (strMMT[0].fEnablePSWD == 1)
    {
        CTOS_LCDTClearDisplay();

        /* BDO CLG: Fleet card support - start -- jzg */
        //if(srTransRec.fFleetCard == TRUE)
        //	vdDispTransTitle(FLEET_SALE);
        //else
        /* BDO CLG: Fleet card support - end -- jzg */
        vdDispTransTitle(srTransRec.byTransType);

        CTOS_LCDTPrintXY(1, 3,"ENTER PASSWORD:");

        while (1)
        {
            memset(szOutput,0x00,sizeof(szOutput));
            shMinLen = strlen(strMMT[0].szPassWord);
            shMaxLen = 6;
            Bret = InputString(1, 4, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
#if 0
            if(strncmp(szOutput,strMMT[0].szPassWord,shMinLen) == 0)
            //if(strcmp(szOutput,strMMT[0].szPassWord) == 0)
            {
            break;
            }
#endif
            if((Bret == d_KBD_CANCEL) || (Bret == 255) || (Bret == 0))
                return Bret;
            else if(Bret >= 1)
            {
                if(strcmp(szOutput,strMMT[0].szPassWord) == 0)
                {
                    break;
                }
                else
                {
                    vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                    CTOS_LCDTClearDisplay();

                    /* BDO CLG: Fleet card support - start -- jzg */
                    //if(srTransRec.fFleetCard == TRUE)
                    //    vdDispTransTitle(FLEET_SALE);
                    //else
                    /* BDO CLG: Fleet card support - end -- jzg */
                    vdDispTransTitle(srTransRec.byTransType);

                    CTOS_LCDTPrintXY(1, 3,"ENTER PASSWORD:");
                }
            }
        }
    }


    srTransRec.MITid = strMMT[0].MITid;
    strcpy(srTransRec.szTID, strMMT[0].szTID);
    strcpy(srTransRec.szMID, strMMT[0].szMID);
    memcpy(srTransRec.szBatchNo, strMMT[0].szBatchNo, 4);
    strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);

    vdDebug_LogPrintf("szATCMD1=[%s] [%s] [%s] [%s] [%s]",strMMT[0].szATCMD1,strMMT[0].szATCMD2,strMMT[0].szATCMD3,strMMT[0].szATCMD4,strMMT[0].szATCMD5);


    return SUCCESS;


}



int inCTOS_CheckAndSelectMID(void)
{
#define ITEMS_PER_PAGE          4

    char szMMT[50];
    char szDisplay[50];
    int inNumOfRecords = 0;
    short shCount = 0;
    short shTotalPageNum;
    short shCurrentPageNum;
    short shLastPageItems = 0;
    short shPageItems = 0;
    short shLoop;

    unsigned char key;
    unsigned char bstatus = 0;

    DebugAddSTR("inCTOS_CheckAndSelectMutipleMID","Processing...",20);

    //get the index , then get all MID from the MMT list and prompt to user to select
    inMMTReadNumofRecords(srTransRec.HDTid,&inNumOfRecords);
    shCurrentPageNum = 1;
    CTOS_KBDBufFlush();//cleare key buffer
    if(inNumOfRecords > 1)
    {
        shTotalPageNum = (inNumOfRecords/ITEMS_PER_PAGE == 0) ? (inNumOfRecords/ITEMS_PER_PAGE) :(inNumOfRecords/ITEMS_PER_PAGE + 1);
        shLastPageItems = (inNumOfRecords/ITEMS_PER_PAGE == 0) ? (ITEMS_PER_PAGE) : (inNumOfRecords%ITEMS_PER_PAGE);

        do
        {
            //display items perpage
            if(shTotalPageNum == 0)//the total item is amaller than ITEMS_PER_PAGE
            {
                shPageItems = inNumOfRecords;
            }
            else if(shCurrentPageNum == shTotalPageNum)//Last page
                shPageItems = shLastPageItems;
            else
                shPageItems = ITEMS_PER_PAGE;

            CTOS_LCDTClearDisplay();
            for(shLoop=0 ; shLoop < shPageItems/*ITEMS_PER_PAGE*/ ; shLoop++)
            {

                memset(szDisplay,0,sizeof(szDisplay));
                sprintf(szDisplay,"%d: %s",shLoop+1,strMMT[shLoop + (shCurrentPageNum -1)*ITEMS_PER_PAGE].szMerchantName);
                CTOS_LCDTPrintXY(2,shLoop + 2,szDisplay);

            }

            key=WaitKey(60);

            switch(key)
            {
                case d_KBD_DOWN:

                    shCurrentPageNum ++;
                    if(shCurrentPageNum > shTotalPageNum)
                        shCurrentPageNum = 1;
                    bstatus = 2;
                    break;


                case d_KBD_UP:

                    shCurrentPageNum --;
                    if(shCurrentPageNum < 1)
                        shCurrentPageNum = shTotalPageNum;
                    bstatus = 2;
                    break;

                case d_KBD_CANCEL:
                    return FAIL;
                case d_KBD_1:
                    //set the unique MMT num

                    memcpy(&strMMT[0],&strMMT[0 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                    //vduiDisplayStringCenter(2,strMMT[0].szMID);
                    bstatus = 0;
                    break;
                case d_KBD_2:
                    if(shPageItems < 2)
                    {
                        bstatus = -1;
                    }
                    else
                    {
                        memcpy(&strMMT[0],&strMMT[1 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                        bstatus = 0;
                    }
                    break;
                case d_KBD_3:
                    if(shPageItems < 3)
                    {
                        bstatus = -1;
                    }
                    else
                    {
                        memcpy(&strMMT[0],&strMMT[2 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                        bstatus = 0;
                    }
                    break;

                case d_KBD_4:
                    if(shPageItems < 4)
                    {
                        bstatus = -1;
                    }
                    else
                    {
                        memcpy(&strMMT[0],&strMMT[3 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                        bstatus = 0;
                    }
                    break;
                case d_KBD_5:
                    if(ITEMS_PER_PAGE < 5)
                    {
                        bstatus = -1 ;
                        break;
                    }
                    else
                    {
                        if(shPageItems < 5)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[4 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                case d_KBD_6:
                    if(ITEMS_PER_PAGE < 6)
                    {
                        bstatus = -1 ;
                        break;
                    }
                    else
                    {
                        if(shPageItems < 6)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[5 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                case d_KBD_7:
                    if(ITEMS_PER_PAGE < 7)
                    {
                        bstatus = -1 ;
                        break;
                    }
                    else
                    {
                        if(shPageItems < 7)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[6 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                case d_KBD_8:   //Max 8 items for one page
                    if(ITEMS_PER_PAGE < 8)
                    {
                        bstatus = -1 ;
                        break;
                    }
                    else
                    {
                        if(shPageItems < 8)
                        {
                            bstatus = -1;
                        }
                        else
                        {
                            memcpy(&strMMT[0],&strMMT[7 + (shCurrentPageNum -1)*ITEMS_PER_PAGE],sizeof(STRUCT_MMT));
                            bstatus = 0;
                        }
                        break;

                    }
                default:
                    bstatus = -1 ;
                    break;

            }

            if((-1) == bstatus)
            {
                return FAIL;

            }
            else if(0 == bstatus)
            {
                break;
            }

        }while(1);

    }
    else
    {
        //One merchant only
        //vduiDisplayStringCenter(1,strMMT[0].szMID);

    }

    srTransRec.MITid = strMMT[0].MITid;
    return SUCCESS;


}


int inCTOS_GetTxnPassword(void)
{
#define NO_PW           0
#define SUPER_PW            1
#define SYSTERM_PW          2
#define ENGINEERPW         3
#define BIN_ROUTEPW      4
#define BOTH_SUPER_ENGINEER      5
#define SUP_SYS_ENG_PW		6
#define SUP_SYS_PW			7


    DebugAddSTR("inCTOS_GetTxnPassword","Processing...",20);


    unsigned char szOutput[30+1],szPassword[256+1];
    int  inTxnTypeID;
    short ret = 0;
    short shMinLen = 4;
    //issue 00441
    //short shMaxLen = 12;
    short shMaxLen = 6;
    BYTE key;
    BYTE Bret;
    BYTE szTitle[16];
    BYTE szDisplay[128];
    BYTE szMsg[100];
    BYTE szDisMsg[100];
    short shCount =0;
    short shRsesult = FAIL;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
       return d_OK;

	vdDebug_LogPrintf("--inCTOS_GetTxnPassword--");
	vdDebug_LogPrintf("srTransRec.byTransType=[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("strPIT.inPasswordLevel=[%d]", strPIT.inPasswordLevel);
	
    //Tine:  08MAY2019
    memset(szTitle, 0x00, sizeof(szTitle));
    //szGetTransTitle(srTransRec.byTransType, szTitle);
    szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);

    memset(szOutput,0,sizeof(szOutput));
    memset(szDisplay,0,sizeof(szDisplay));
    memset(szPassword,0,sizeof(szPassword));
    memset(szMsg,0,sizeof(szMsg));

    strcpy(szMsg, "4");
    strcat(szMsg, "|");
    strcat(szMsg, "6");
    strcat(szMsg, "|");
    strcat(szMsg, szTitle);
    strcat(szMsg, "|");
    strcat(szMsg, "Enter password");

    inTxnTypeID = srTransRec.byTransType;

    //vduiLightOn();

    inPITRead(inTxnTypeID);

	if (srTransRec.byTransType == CLEAR_BATCH || srTransRec.byTransType == CLEAR_REVERSAL)
		strPIT.inPasswordLevel = SYSTERM_PW;

    //enhanced ecr - check if ecr is enable and If transaction is ecr triggered (should not propmt for passwrod)
    if ((strTCT.fECR) &&  (fECRTxnFlg == 1))
        return d_OK;
    //end

    if (NO_PW== strPIT.inPasswordLevel)
    {
        return d_OK;
    }

    //CTOS_KBDHit(&key);//clear key buffer
    //while(shCount < 3) //removed 3 retry limit on password input.
    while(1)
    {
        //vduiClearBelow(3); /* BDOCLG-V3-00005: Overlapping "WRONG PASSWORD" message -- jzg */
        switch(srTransRec.byTransType)
        {
            case SALE:
            case FLEET_SALE: //BDO CLG: Fleet card support -- jzg
            case PRE_AUTH:
            case REFUND:
            case SALE_OFFLINE:
            case EPP_SALE:
            case VOID:
            case SALE_TIP:
            case SALE_ADJUST:
            case SETTLE:
            case BATCH_REVIEW:
            case BATCH_TOTAL:
            case REPRINT_ANY:
            case SETUP: /*sidumili: Issue#:000087 [prompt password]*/
                //gcitra-012015
            case BIN_VER:
                //gcitra-012015
                //smac
            case SMAC_ACTIVATION:
                //samc
            case MANUAL_SETTLE: //aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 7 of 9
                shMaxLen=6; /*aaronnino for BDOCLG ver 9.0 fix on issue #0075 Can't Input 6 chars password after a failed 4 chars inputted password
				                           Issue #00441 - Max length should only be 6*/

            case BIN_ROUTING:
			case CTMS_UPDATE:

			case CLEAR_BATCH:
			case CLEAR_REVERSAL:


                if(SUPER_PW== strPIT.inPasswordLevel)
                {
                    //CTOS_LCDTClearDisplay();
#if 0
                    if (strlen(szFuncTitleName) <= 0)
                {
   					/* BDO CLG: Fleet card support - start -- jzg */
   					//if(srTransRec.fFleetCard == TRUE)
                    //	vdDispTransTitle(FLEET_SALE);
   					//else
   					/* BDO CLG: Fleet card support - end -- jzg */
                    vdDispTransTitle(srTransRec.byTransType);
               	}else
    				vdDispTitleString(szFuncTitleName); //aaronnino for BDOCLG ver 9.0 fix on issue #0093 Have a function title for function keys shorcut 4 of 6

				CTOS_LCDTPrintXY(1, 3,"ENTER PASSWORD:");
#endif
                    //Bret = InputString(1, 4, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
                    //Bret = InputString(1, 4, 0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
                    //Bret = S1InputString(1, 4, 0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE));
                    strcpy(szPassword,szMsg);
                    strcat(szPassword, "|");
                    strcat(szPassword,strTCT.szSuperPW);
                    memset(szOutput,0,sizeof(szOutput));
                    Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE), szPassword);
                    vdDebug_LogPrintf("1. SUPER_PW szOutput[%s]", szOutput);
                    //issue:285
                    if (Bret == 255) //timeout
                    {
                        fTimeOutFlag = TRUE;
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3,"                 ");
                        return Bret;
                    }
                    //issue:285

                    //#issue:231
                    if(d_KBD_CANCEL == Bret)
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3,"                 ");
                        return Bret;
                    }
                    else
                        //#issue:231
                        /*if((strcmp(szOutput,strTCT.szSuperPW) == 0) ||(strcmp(szOutput,strTCT.szSystemPW) == 0)||  (strcmp(szOutput,strTCT.szEngineerPW) == 0))*/
                    if (strcmp(szOutput,strTCT.szSuperPW) == 0)//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 8 of 9
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        return d_OK;
                    }
                        //#issue:23
                        /*else if(d_KBD_CANCEL == Bret)
                        return Bret;*/
                        //#issue:23
                    else
                    {
                        //CTOS_LCDTClearDisplay();
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }
                }
                else if(SYSTERM_PW== strPIT.inPasswordLevel)
                {
#if 0
                    if (strlen(szFuncTitleName) <= 0)
                   	{
    										/* BDO CLG: Fleet card support - start -- jzg */
    										//if(srTransRec.fFleetCard == TRUE)
                        					//	vdDispTransTitle(FLEET_SALE);
    										//else
    										/* BDO CLG: Fleet card support - end -- jzg */
                        						vdDispTransTitle(srTransRec.byTransType);
                   	}
									 else
									 	   vdDispTitleString(szFuncTitleName); //aaronnino for BDOCLG ver 9.0 fix on issue #0093 Have a function title for function keys shorcut 5 of 6

                    CTOS_LCDTPrintXY(1, 3,"ENTER PASSWORD:");
#endif
                    //Bret = InputString(1, 4, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
                    //Bret = InputString(1, 4, 0x02, 0x02,szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
                    //Bret = S1InputString(1, 4, 0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE));
                    strcpy(szPassword,szMsg);
                    strcat(szPassword, "|");
                    strcat(szPassword, strTCT.szSystemPW);
                    memset(szOutput,0,sizeof(szOutput));
                    Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE), szPassword);
                    vdDebug_LogPrintf("2. SYSTERM_PW szOutput[%s]", szOutput);
                    //issue:285
                    if (Bret == 255) //timeout
                    {
                        fTimeOutFlag = TRUE;
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3,"                 ");
                        return Bret;
                    }
                    //issue:285

                    //#issue:231
                    if(d_KBD_CANCEL == Bret)
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3,"                 ");
                        return Bret;
                    }
                    else
                        //#issue:231

                        /* if( (strcmp(szOutput,strTCT.szSystemPW) == 0) ||(strcmp(szOutput,strTCT.szEngineerPW) == 0))*/
                    if (strcmp(szOutput,strTCT.szSystemPW)==0) //aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 9 of 9
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        return d_OK;
                    }
                        //#issue:23
                        /*else if(d_KBD_CANCEL == Bret)
                        			return Bret;*/
                        //#issue:23
                    else
                    {
                        //CTOS_LCDTClearDisplay();
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }
                }
                else if(ENGINEERPW== strPIT.inPasswordLevel)
                {
#if 0
                    if (strlen(szFuncTitleName) <= 0)
                   {
                      /* BDO CLG: Fleet card support - start -- jzg */
                      //if(srTransRec.fFleetCard == TRUE)
                      //	vdDispTransTitle(FLEET_SALE);
                      //else
                      /* BDO CLG: Fleet card support - end -- jzg */
                      vdDispTransTitle(srTransRec.byTransType);
                   }
                   else
                      vdDispTitleString(szFuncTitleName); //aaronnino for BDOCLG ver 9.0 fix on issue #0093 Have a function title for function keys shorcut 6 of 6

					          CTOS_LCDTPrintXY(1, 3,"PASSWORD:");
#endif
                    //Bret = InputString(1, 4, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
                    //Bret = InputString(1, 4, 0x02, 0x02,szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
                    //Bret = S1InputString(1, 4, 0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE));
                    strcpy(szPassword,szMsg);
                    strcat(szPassword, "|");
                    strcat(szPassword, strTCT.szEngineerPW);
                    memset(szOutput,0,sizeof(szOutput));
                    Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE), szPassword);
                    vdDebug_LogPrintf("3. ENGINEERPW szOutput[%s]", szOutput);
                    //issue:285
                    if (Bret == 255) //timeout
                    {
                        fTimeOutFlag = TRUE;
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3,"                 ");
                        return Bret;
                    }
                    //issue:285

                    //#issue:231
                    if(d_KBD_CANCEL == Bret)
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3,"                 ");
                        return Bret;
                    }
                    else
                        //#issue:231

                    if(strcmp(szOutput,strTCT.szEngineerPW) == 0)
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        return d_OK;
                    }
                    //#issue:23
                    /*else if(d_KBD_CANCEL == Bret)
                        			return Bret;*/
                    //#issue:23
                    {
                        //CTOS_LCDTClearDisplay();
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }

                }
                else if(BIN_ROUTEPW== strPIT.inPasswordLevel)
                {
#if 0
                    //CTOS_LCDTClearDisplay();
					if (strlen(szFuncTitleName) <= 0)
					{
						/* BDO CLG: Fleet card support - start -- jzg */
						//if(srTransRec.fFleetCard == TRUE)
						//	vdDispTransTitle(FLEET_SALE);
						//else
						/* BDO CLG: Fleet card support - end -- jzg */
						vdDispTransTitle(srTransRec.byTransType);
					}else
						vdDispTitleString(szFuncTitleName); //aaronnino for BDOCLG ver 9.0 fix on issue #0093 Have a function title for function keys shorcut 4 of 6

					CTOS_LCDTPrintXY(1, 3,"ENTER PASSWORD:");
#endif
                    //Bret = InputString(1, 4, 0x01, 0x02, szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
                    //Bret = InputString(1, 4, 0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
                    //Bret = S1InputString(1, 4, 0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE));
                    strcpy(szPassword,strTCT.szBInRoutePW);
                    memset(szOutput,0,sizeof(szOutput));
                    Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE), szPassword);
                    vdDebug_LogPrintf("4. BIN_ROUTEP szDisplay[%s]", szOutput);
                    //issue:285
                    if (Bret == 255) //timeout
                    {
                        fTimeOutFlag = TRUE;
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        CTOS_LCDTPrintXY(1, 3," 				");
                        return Bret;
                    }
                    //issue:285

                    //#issue:231
                    if(d_KBD_CANCEL == Bret)
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3," 				");
                        return Bret;
                    }
                    else
                        //#issue:231
                        /*if((strcmp(szOutput,strTCT.szSuperPW) == 0) ||(strcmp(szOutput,strTCT.szSystemPW) == 0)||  (strcmp(szOutput,strTCT.szEngineerPW) == 0))*/
                    if (strcmp(szOutput,strTCT.szBInRoutePW) == 0)//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 8 of 9
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        return d_OK;
                    }
                        //#issue:23
                        /*else if(d_KBD_CANCEL == Bret)
							return Bret;*/
                        //#issue:23
                    else
                    {
                        //CTOS_LCDTClearDisplay();
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }
                }
                else if(SUP_SYS_ENG_PW== strPIT.inPasswordLevel)
                {
                    //Bret = InputString(1, 4, 0x02, 0x02,szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
                    strcpy(szPassword,szMsg);
                    strcat(szPassword, "|");
                    strcat(szPassword,strTCT.szSuperPW);
                    strcat(szPassword, "|");
                    strcat(szPassword, strTCT.szSystemPW);
                    strcat(szPassword, "|");
                    strcat(szPassword, strTCT.szEngineerPW);
                    memset(szOutput,0,sizeof(szOutput));
                    Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE), szPassword);
                    vdDebug_LogPrintf("5. SUP_SYS_ENG_PW szOutput[%s]", szOutput);
                    if (Bret == 255) //timeout
                    {
                        fTimeOutFlag = TRUE;
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        CTOS_LCDTPrintXY(1, 3,"                 ");
                        return Bret;
                    }

                    if(d_KBD_CANCEL == Bret)
                    {
                        //memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3,"                 ");
                        //memset(szTitle, 0x00, sizeof(szTitle));
                        //szGetTransTitle(srTransRec.byTransType, szTitle);
                        //strcpy(szDisMsg, szTitle);
                        //strcat(szDisMsg, "|");
                        //strcat(szDisMsg, "USER CANCEL");
                        //usCTOSS_LCDDisplay(szDisMsg);
                        //CTOS_Beep();
                        //CTOS_Delay(1500);

						//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
						//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
						//CTOS_Beep();
						//CTOS_Delay(1000);
                        return d_USER_CANCEL;
                    }

                    else if (strcmp(szOutput,strTCT.szSuperPW)==0 || strcmp(szOutput,strTCT.szSystemPW)==0 || strcmp(szOutput,strTCT.szEngineerPW)==0)
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        return d_OK;
                    }
                    else
                    {
                        //CTOS_LCDTClearDisplay();
                        vdDisplayErrorMsg(1, 8, "5. WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }

                }
                else if(SUP_SYS_PW== strPIT.inPasswordLevel)
                {
                    //Bret = InputString(1, 4, 0x02, 0x02,szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
                    //Bret = S1InputString(1, 4, 0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE));
                    strcpy(szPassword,szMsg);
                    strcat(szPassword, "|");
                    strcat(szPassword,strTCT.szSuperPW);
                    strcat(szPassword, "|");
                    strcat(szPassword, strTCT.szSystemPW);
                    memset(szOutput,0,sizeof(szOutput));
                    Bret = InputStringUI(0x02, 0x02, szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE), szPassword);
                    if (Bret == 255) //timeout
                    {
                        fTimeOutFlag = TRUE;
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3,"                 ");
                        return Bret;
                    }

                    if(d_KBD_CANCEL == Bret)
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        //CTOS_LCDTPrintXY(1, 3,"                 ");
                        return Bret;
                    }
                    else if (strcmp(szOutput,strTCT.szSuperPW)==0 || strcmp(szOutput,strTCT.szSystemPW)==0)
                    {
                        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                        return d_OK;
                    }
                    else
                    {
                        //CTOS_LCDTClearDisplay();
                        vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                        shRsesult = FAIL;
                        break;
                    }
                }
                else
                {
                    memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                    return d_OK;
                }


            default:
                memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                return d_OK;

        }
        //if(FAIL == shRsesult)
        //    shCount ++ ;
    }

    return shRsesult;

}



int inCTOS_SelectHost(void)
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szStr[16+1]={0};

    inDatabase_TerminalOpenDatabase();

    /* BDO: Make sure we use the BDO Credit host details first - start -- jzg */
    if(srTransRec.byTransType == BIN_VER)
        //inHostIndex = inGetCreditHostIndex();
        inHostIndex = inGetCreditHostIndexEx();
    else
        inHostIndex = (short)strCDT.HDTid;
    /* BDO: Make sure we use the BDO Credit host details first - end -- jzg */

    vdDebug_LogPrintf("saturn inCTOS_SelectHost::Txn Type = [%d]", srTransRec.byTransType);
    vdDebug_LogPrintf("saturn inCTOS_SelectHost = [%d]", inHostIndex);

    if(inMultiAP_CheckSubAPStatus() == d_OK)
    {
        inDatabase_TerminalCloseDatabase();
        return d_OK;
    }

    //if(inHDTRead(inHostIndex) != d_OK)
    if(inHDTReadEx(inHostIndex) != d_OK)
    {
        //vdSetErrorMessage("HOST SELECTION ERR");
        //inHDTReadData(inHostIndex);
        inHDTReadDataEx(inHostIndex);
        strcpy(szStr,strHDT_Temp.szHostLabel);
        memset(strHDT_Temp.szHostLabel,0x00,sizeof(strHDT_Temp.szHostLabel));
        sprintf(strHDT_Temp.szHostLabel," %s ",szStr);
        vdDisplayErrorMsgResp2(strHDT_Temp.szHostLabel, "TRANSACTION", "NOT ALLOWED");
        inDatabase_TerminalCloseDatabase();
        return(d_NO);
    }
    else
    {
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        //if(inCSTRead(inCurrencyIdx) != d_OK)
        if(inCSTReadEx(inCurrencyIdx) != d_OK)
        {
            vdSetErrorMessage("LOAD CST ERR");
            inDatabase_TerminalCloseDatabase();
            return(d_NO);
        }

        if(strTCT.fSingleComms == TRUE)
            inHostIndex=1;

        //if(inCPTRead(inHostIndex) != d_OK)
        if(inCPTReadEx(inHostIndex) != d_OK)
        {
            vdSetErrorMessage("LOAD CPT ERR");
            inDatabase_TerminalCloseDatabase();
            return(d_NO);
        }

        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
            if (strCST.inCurrencyIndex > 1)
            {
                memset(szAscBuf, 0x00, sizeof(szAscBuf));
                memset(szBcdBuf, 0x00, sizeof(szBcdBuf));

                sprintf(szAscBuf,"%4s",strCST.szCurCode);

                wub_str_2_hex(szAscBuf, szBcdBuf, 4);
                memcpy((char *)srTransRec.stEMVinfo.T5F2A, &szBcdBuf[0], 2);
                DebugAddHEX("T5F2A..",srTransRec.stEMVinfo.T5F2A,2);
            }
        inDatabase_TerminalCloseDatabase();
        return (d_OK);
    }


    vdDebug_LogPrintf("saturn inCTOS_SelectHost = [%d]", inHostIndex);
}



int inCTOS_SelectHostEx(void)
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szStr[16+1]={0};

    /* BDO: Make sure the ATP BIN Routing must use the first HDT- start -- jzg */
    inHostIndex = 1;
    /* BDO: Make sure the ATP BIN Routing must use the first HDT - end -- jzg */

    vdDebug_LogPrintf("inCTOS_SelectHostEx::Txn Type = [%d]", srTransRec.byTransType);
    vdDebug_LogPrintf("inCTOS_SelectHost = [%d]", inHostIndex);

    if(inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    if(inHDTRead(inHostIndex) != d_OK)
    {
        //vdSetErrorMessage("HOST SELECTION ERR");
        inHDTReadData(inHostIndex);
        strcpy(szStr,strHDT_Temp.szHostLabel);
        memset(strHDT_Temp.szHostLabel,0x00,sizeof(strHDT_Temp.szHostLabel));
        sprintf(strHDT_Temp.szHostLabel," %s ",szStr);
        vdDisplayErrorMsgResp2(strHDT_Temp.szHostLabel, "TRANSACTION", "NOT ALLOWED");
        return(d_NO);
    }
    else
    {
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if(inCSTRead(inCurrencyIdx) != d_OK)
        {
            vdSetErrorMessage("LOAD CST ERR");
            return(d_NO);
        }

        if(inCPTRead(inHostIndex) != d_OK)
        {
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }

        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
            if (strCST.inCurrencyIndex > 1)
            {
                memset(szAscBuf, 0x00, sizeof(szAscBuf));
                memset(szBcdBuf, 0x00, sizeof(szBcdBuf));

                sprintf(szAscBuf,"%4s",strCST.szCurCode);

                wub_str_2_hex(szAscBuf, szBcdBuf, 4);
                memcpy((char *)srTransRec.stEMVinfo.T5F2A, &szBcdBuf[0], 2);
                DebugAddHEX("T5F2A..",srTransRec.stEMVinfo.T5F2A,2);
            }
        return (d_OK);
    }
}

int inCTOS_SelectHostEx2(void)
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szStr[16+1]={0};

    inDatabase_TerminalOpenDatabase();

    /* BDO: Make sure we use the BDO Credit host details first - start -- jzg */
    if(srTransRec.byTransType == BIN_VER)
        //inHostIndex = inGetCreditHostIndex();
        inHostIndex = inGetCreditHostIndexEx();
    else
        inHostIndex = (short)strCDT.HDTid;
    /* BDO: Make sure we use the BDO Credit host details first - end -- jzg */

    vdDebug_LogPrintf("inCTOS_SelectHost::Txn Type = [%d]", srTransRec.byTransType);
    vdDebug_LogPrintf("inCTOS_SelectHost = [%d]", inHostIndex);

    if(inMultiAP_CheckSubAPStatus() == d_OK)
    {
        //inDatabase_TerminalCloseDatabase();
        return d_OK;
    }

    //if(inHDTRead(inHostIndex) != d_OK)
    if(inHDTReadEx(inHostIndex) != d_OK)
    {
        //vdSetErrorMessage("HOST SELECTION ERR");
        //inHDTReadData(inHostIndex);

        //inHDTReadDataEx(inHostIndex);
        //strcpy(szStr,strHDT_Temp.szHostLabel);
        //memset(strHDT_Temp.szHostLabel,0x00,sizeof(strHDT_Temp.szHostLabel));
        
        getHostLabel(inHostIndex, strHDT_Temp.szHostLabel); // get hostlabel -- sidumili
        vdDisplayErrorMsgResp2(strHDT_Temp.szHostLabel, "TRANSACTION", "NOT ALLOWED");
        inDatabase_TerminalCloseDatabase();
        return(d_NO);
    }
    else
    {
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        //if(inCSTRead(inCurrencyIdx) != d_OK)
        if(inCSTReadEx(inCurrencyIdx) != d_OK)
        {
            vdSetErrorMessage("LOAD CST ERR");
            inDatabase_TerminalCloseDatabase();
            return(d_NO);
        }

        if(strTCT.fSingleComms == TRUE)
            inHostIndex=1;
        //if(inCPTRead(inHostIndex) != d_OK)
        if(inCPTReadEx(inHostIndex) != d_OK)
        {
            vdSetErrorMessage("LOAD CPT ERR");
            inDatabase_TerminalCloseDatabase();
            return(d_NO);
        }

        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
            if (strCST.inCurrencyIndex > 1)
            {
                memset(szAscBuf, 0x00, sizeof(szAscBuf));
                memset(szBcdBuf, 0x00, sizeof(szBcdBuf));

                sprintf(szAscBuf,"%4s",strCST.szCurCode);

                wub_str_2_hex(szAscBuf, szBcdBuf, 4);
                memcpy((char *)srTransRec.stEMVinfo.T5F2A, &szBcdBuf[0], 2);
                DebugAddHEX("T5F2A..",srTransRec.stEMVinfo.T5F2A,2);
            }
        //inDatabase_TerminalCloseDatabase();
        return (d_OK);
    }


    vdDebug_LogPrintf("saturn inCTOS_SelectHost = [%d]", inHostIndex);
}


int inCTOS_getCardCVV2(BYTE *baBuf)
{
    USHORT usRet;
    USHORT usInputLen;
    USHORT usLens;
    USHORT usMinLen = 3;
    USHORT usMaxLen = 4;
    BYTE bBuf[4+1];
    BYTE bDisplayStr[MAX_CHAR_PER_LINE+1];

    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(srTransRec.byTransType);

    if(CARD_ENTRY_MANUAL == srTransRec.byEntryMode)
    {
        setLCDPrint(2, DISPLAY_POSITION_LEFT, "CARD NO: ");
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        strcpy(bDisplayStr, srTransRec.szPAN);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 3, bDisplayStr);
        setLCDPrint(5, DISPLAY_POSITION_LEFT, "EXPIRY DATE(MM/YY):");

        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[1], &bBuf[0], 1);
        memcpy(bDisplayStr, bBuf, 2);
        bDisplayStr[2] = '/';
        memset(bBuf, 0x00, sizeof(bBuf));
        wub_hex_2_str(&srTransRec.szExpireDate[0], &bBuf[0], 1);
        memcpy(bDisplayStr+3, bBuf, 2);
        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE-strlen(bDisplayStr)*2, 6, bDisplayStr);

        usInputLen = 7;
    }
//gcitra
/*
    else
    {
		inCTOS_DisplayCardTitle(6, 7); //Display Issuer logo: re-aligned Issuer label and PAN lines -- jzg
        usInputLen = 7;
    }
*/
//gcitra

    //setLCDPrint(usInputLen, DISPLAY_POSITION_LEFT, "CVV2: ");

    while(1)
    {
        //usRet = shCTOS_GetNum(usInputLen+1, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 1, d_INPUT_TIMEOUT);
        usRet = shCTOS_GetNum(usInputLen+1, 0x01, baBuf, &usLens, usMinLen, usMaxLen, 1, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
        if (usRet == d_KBD_CANCEL )
            return (d_EDM_USER_CANCEL);
        if (usRet >= usMinLen && usRet <= usMaxLen)
        {
            return (d_OK);
        }
        else if(0 == usRet)
        {
            return (d_OK);
        }

        baBuf[0] = 0x00;
    }
}

int inCTOS_GetCVV2()
{
    BYTE szCVV2Code[CVV2_SIZE + 1];
    int inResult = FAIL;
    BYTE key;
    short shCount = 0;
    vdDebug_LogPrintf("inCTOS_GetCVV2[START]");

    memset(srTransRec.szCVV2 , 0x00, sizeof(srTransRec.szCVV2));
    memset(szCVV2Code , 0x00, sizeof(szCVV2Code));

    if(CVV2_NONE == strCDT.inCVV_II)
    {
        return d_OK;
    }

    /*sidumili: Issue#:000051 [REMOVE CVV2 ENTRY ON MANUAL ENTRY]*/
    if (srTransRec.byEntryMode == CARD_ENTRY_MANUAL){
        return d_OK;
    }

    while(shCount < 3)
    {

        if(((CVV2_MANUAL == strCDT.inCVV_II) &&( CARD_ENTRY_MANUAL == srTransRec.byEntryMode))
           || ((CVV2_MSR == strCDT.inCVV_II) &&(( CARD_ENTRY_MSR == srTransRec.byEntryMode) ||( CARD_ENTRY_FALLBACK == srTransRec.byEntryMode)))
           || ((CVV2_MANUAL_MSR == strCDT.inCVV_II) &&(( CARD_ENTRY_MANUAL == srTransRec.byEntryMode) ||( CARD_ENTRY_MSR == srTransRec.byEntryMode) ||( CARD_ENTRY_FALLBACK == srTransRec.byEntryMode)))
           || ((CVV2_MANUAL_MSR_CHIP == strCDT.inCVV_II) &&(( CARD_ENTRY_MANUAL == srTransRec.byEntryMode) ||( CARD_ENTRY_MSR == srTransRec.byEntryMode) ||( CARD_ENTRY_FALLBACK == srTransRec.byEntryMode) || ( CARD_ENTRY_ICC == srTransRec.byEntryMode))))
        {
            CTOS_KBDBufFlush();

            inResult = inCTOS_getCardCVV2(szCVV2Code);
            if(d_OK == inResult)
            {
                strcpy(srTransRec.szCVV2,szCVV2Code);
                return d_OK;
            }
            else
            {
                if(d_EDM_USER_CANCEL == inResult)
                {
                    vdSetErrorMessage("USER CANCEL");
                    return inResult;
                }

                memset(szCVV2Code , 0x00, sizeof(szCVV2Code));
                vdDisplayErrorMsg(1, 8, "INVALID CVV");

                break;
            }
        }
        else
        {
            return d_OK;
        }

        shCount ++ ;
    }
    vdSetErrorMessage("Get CVV ERR");
    return FAIL;

}

void vdCTOSS_EMV_SetAmount(void)
{
    BYTE szBaseAmount[20];
    BYTE szTipAmount[20];
    BYTE szTotalAmount[20];
    BYTE   EMVtagVal[64];
    BYTE   szStr[64];
    BYTE  byDataTmp1[32];
    BYTE  byDataTmp2[32];
    BYTE  bPackSendBuf[2560];
    USHORT usPackSendLen = 0;
    USHORT ushEMVtagLen;
    ULONG lnTmp;
    OFFLINEPINDISPLAY_REC szDisplayRec;

    memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
    memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
    wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
    lnTmp = atol(byDataTmp1);
    wub_long_2_array(lnTmp, byDataTmp2);

    inCTOSS_PutEnvDB ("BDOAMOUT", byDataTmp1);
    inCTOSS_PutEnvDB ("BDOCURS", strCST.szCurSymbol);

    {
        memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
        memset(byDataTmp2, 0x00, sizeof(byDataTmp2));
        wub_hex_2_str(srTransRec.szTotalAmount, byDataTmp1, 6);
        lnTmp = atol(byDataTmp1);
        wub_long_2_array(lnTmp, byDataTmp2);

        memcpy(&bPackSendBuf[usPackSendLen++], "\x81", 1);
        bPackSendBuf[usPackSendLen++] = 0x04;
        memcpy(&bPackSendBuf[usPackSendLen], byDataTmp2, 4);
        usPackSendLen += 4;

        memcpy(srTransRec.stEMVinfo.T9F02, srTransRec.szTotalAmount, 6);

        memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x02", 2);
        usPackSendLen += 2;
        bPackSendBuf[usPackSendLen++] = 0x06;
        memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F02, 6);
        usPackSendLen += 6;

        if(atol(szTipAmount) > 0)
        {
            memcpy(srTransRec.stEMVinfo.T9F03, srTransRec.szTipAmount, 6);
        }
        else
        {
            memset(szTipAmount, 0x00, sizeof(szTipAmount));
            memcpy(srTransRec.stEMVinfo.T9F03, szTipAmount, 6);
        }

        memcpy(&bPackSendBuf[usPackSendLen], "\x9F\x03", 2);
        usPackSendLen += 2;
        bPackSendBuf[usPackSendLen++] = 0x06;
        memcpy(&bPackSendBuf[usPackSendLen], srTransRec.stEMVinfo.T9F03, 6);
        usPackSendLen += 6;

        //usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);
    }

#if 1
    //for offline pin display
    memcpy(&bPackSendBuf[usPackSendLen], "\xFF\xFE", 2);
    usPackSendLen += 2;
    //bPackSendBuf[usPackSendLen++] = 0x06;
    memset(&szDisplayRec,0x00,sizeof(OFFLINEPINDISPLAY_REC));
    vdCTOSS_PackOfflinepinDisplay(&szDisplayRec);
    memcpy(&bPackSendBuf[usPackSendLen], &szDisplayRec, sizeof(OFFLINEPINDISPLAY_REC));
    usPackSendLen += sizeof(OFFLINEPINDISPLAY_REC);


    //vdPCIDebug_HexPrintf("EMV_SetAmount",bPackSendBuf,usPackSendLen);
    usCTOSS_EMV_MultiDataSet(usPackSendLen, bPackSendBuf);
#endif

}


int inCTOS_EMVProcessing()
{
    int inRet;
    BYTE   EMVtagVal[64];
    BYTE bySC_status; //Issue# 000065 -- jzg
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szErrMsg[30+1];
    BYTE szTitle[20];
    BYTE szDisMsg[100];

    //gcitra-0728
    //CTOS_LCDTClearDisplay();
    //gcitra-0728

    if( CARD_ENTRY_ICC == srTransRec.byEntryMode)
    {
        inCTOS_EMVSetTransType(srTransRec.byTransType);
        //inCTOSS_CheckNSR(0);

        if (strCST.inCurrencyIndex > 1){
            memset(szAscBuf, 0x00, sizeof(szAscBuf));
            memset(szBcdBuf, 0x00, sizeof(szBcdBuf));

            sprintf(szAscBuf,"%4s",strCST.szCurCode);

            wub_str_2_hex(szAscBuf, szBcdBuf, 4);
            memcpy((char *)srTransRec.stEMVinfo.T5F2A, &szBcdBuf[0], 2);

            /* BDO-00168: Make sure to populate EMV Tag 5F2A with the correct txn currency code -- jzg */
            ushCTOS_EMV_NewTxnDataSet(TAG_5F2A_TRANS_CURRENCY_CODE, 2, srTransRec.stEMVinfo.T5F2A);
        }

        if (srTransRec.byTransType == SALE_OFFLINE){
            srTransRec.stEMVinfo.T9F33[1] = 0x028;
            ushCTOS_EMV_NewTxnDataSet(TAG_9F33_TERM_CAB,3,srTransRec.stEMVinfo.T9F33);
        }

        vdCTOSS_EMV_SetAmount();

        inRet = usCTOSS_EMV_TxnPerform();
		
        if(inRet == PIN_CANCELLED)
			return EMV_CRITICAL_ERROR;
		
        //inCTOS_EMVSetTransType(srTransRec.byTransType);

        inCTOS_FirstGenACGetAndSaveEMVData();

        //inCTOSS_CheckNSR(1);

        EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;

        memset(szErrMsg,0x00,sizeof(szErrMsg));
        vdDebug_LogPrintf("EMVtagVal[0] = [%d]", EMVtagVal[0]);

        switch( EMVtagVal[0] & 0xC0)
        {
            case 0: //Declined --- AAC
                strcpy(srTransRec.szAuthCode,"Z1");

                /* Issue# 000065 - start -- jzg */
                CTOS_SCStatus(d_SC_USER, &bySC_status);
                inGetErrorMessage(szErrMsg);
                if (!(bySC_status & d_MK_SC_PRESENT))
                {
						vdSetErrorMessage("CHIP MALFUNCTION");
                }
                else if(strcmp(szErrMsg,"ABORTED") == 0)
                    vdSetErrorMessage("");
                else
                    vdSetErrorMessage("EMV Decline");
                /* Issue# 000065 - end -- jzg */

                vdDebug_LogPrintf("1st ACs, card dec");

                return EMV_CRITICAL_ERROR;

            case 0x40: //Approval --- TC
                strcpy(srTransRec.szAuthCode,"Y1");
                srTransRec.shTransResult = TRANS_AUTHORIZED;

                srTransRec.fVoidOffline = CN_TRUE;

                vdDebug_LogPrintf("1nd AC app");
                break;

            case 0x80: //ARQC
                vdDebug_LogPrintf("go online");
                break;

            default:
                strcpy(srTransRec.szAuthCode,"Z1");

                /* Issue# 000065 - start -- jzg */
                CTOS_SCStatus(d_SC_USER, &bySC_status);
                if (!(bySC_status & d_MK_SC_PRESENT))
                {
						vdSetErrorMessage("CARD READ ERROR");
                }
                else
                    vdSetErrorMessage("EMV Decline");
                /* Issue# 000065 - end -- jzg */

                inGetErrorMessage(szErrMsg);
                if(strcmp(szErrMsg,"ABORTED") == 0)
                    vdSetErrorMessage("");
                return EMV_CRITICAL_ERROR;

        }

		
        vdDebug_LogPrintf("usCTOSS_EMV_TxnPerform return[%d]", inRet);
        if (inRet != d_OK)
            vdSetErrorMessage("First GenAC ERR");

        inGetErrorMessage(szErrMsg);
        if(strcmp(szErrMsg,"ABORTED") == 0)
            vdSetErrorMessage("");

        return inRet;

    }

    return d_OK;
}

int inCTOS_CheckTipAllowd()
{

    if(SALE_TIP == srTransRec.byTransType)
    {
        if (inMultiAP_CheckSubAPStatus() == d_OK)
            return d_OK;
    }

    if (TRUE !=strTCT.fTipAllowFlag)
    {
        if (SALE_TIP == srTransRec.byTransType)
        {
            //vdSetErrorMessage("TIP NOT ALLOWED");
            CTOS_LCDTClearDisplay();
            vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
        }
        return d_NO;
    }

//1010
    if(srTransRec.byVoided == TRUE)
    {
        //vdSetErrorMessage("TIP NOT ALLOWED");
        CTOS_LCDTClearDisplay();
        vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
        return d_NO;
    }
//1010

    return d_OK;
}



SHORT shCTOS_EMVAppSelectedProcess(void)
{
    short shResult;
    BYTE SelectedAID[16];
    BYTE szTitle[16];
    BYTE szDisMsg[50];
    USHORT SelectedAIDLen = 0;
    BYTE label[32];
    USHORT label_len = 0;
    BYTE PreferAID[128];
    CTOS_RTC SetRTC;

    //vdDebug_LogPrintf("-------EMV_TxnAppSelect-----");
    memset(PreferAID,0,sizeof(PreferAID));
    memset(label,0,sizeof(label));
    memset(szDisMsg,0,sizeof(szDisMsg));

    shResult = usCTOSS_EMV_TxnAppSelect(PreferAID, 0, SelectedAID, &SelectedAIDLen, label, &label_len);
    vdDebug_LogPrintf("saturn -EMV_TxnAppSelect=[%d] SelectedAIDLen[%d] label[%s]fback[%d]",shResult, SelectedAIDLen, label,strEMVT.inEMVFallbackAllowed);


    //check if BANCNET AID

    //check if BANCNET AID is already enable (host already respond with "70" on BIN routing

    if ((memcmp(SelectedAID,"\xA0\x00\x00\x00\x06",5) == 0) || (memcmp(SelectedAID,"\xa0\x00\x00\x00\x06",5) == 0)){
        fBancnetAIDEnable = get_env_int("BANCNETAID");

        if (fBancnetAIDEnable == 1)
            fSkipBINRoutingForDebit = TRUE;

        vdDebug_LogPrintf("saturn fBancnetAIDEnable %d %d", fBancnetAIDEnable, fSkipBINRoutingForDebit);

    }


    if ((memcmp(SelectedAID,"\xA0\x00\x00\x03\x33",5) == 0) || (memcmp(SelectedAID,"\xa0\x00\x00\x03\x33",5) == 0)){
        fSkipBINRoutingForCUP = TRUE;
    }

    if(d_OK == shResult)
        DebugAddHEX("SelectedAIDLen", SelectedAID, SelectedAIDLen);

    if((shResult != PP_OK) && (shResult != EMV_USER_ABORT))
    {
        //EMV: should display "CHIP NOT DETECTED" instead of doing fallback - start -- jzg
        if (shResult == EMV_CHIP_NOT_DETECTED)
        {
            vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
            //TINE:  10MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, "CHIP NOT DETECTED");
            vdDisplayErrorMsg(1, 8, szDisMsg);
            return EMV_CHIP_FAILED;
        }
        //EMV: should display "CHIP NOT DETECTED" instead of doing fallback - end -- jzg

        //EMV: If AID not found display "TRANS NOT ALLOWED" - start -- jzg
        if (shResult == EMV_TRANS_NOT_ALLOWED)
        {
            //0424
            //vdDisplayErrorMsg(1, 8, "TRANS NOT ALLOWED");
            //vduiClearBelow(2);
            //vdDisplayErrorMsg(1, 8, "APPL NOT AVAILABLE");
            //TINE:  10MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, "APPL NOT AVAILABLE");
            vdDisplayErrorMsg(1, 8, szDisMsg);
            return EMV_TRANS_NOT_ALLOWED;
            //0424
        }
        //EMV: If AID not found display "TRANS NOT ALLOWED" - end -- jzg

        //VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - start -- jzg
        if (shResult == EMV_CARD_BLOCKED)
        {
            inFallbackToMSR = FAIL;
            //vdDisplayErrorMsg(1, 8, "CARD BLOCKED");
            //TINE:  10MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, "CARD BLOCKED");
            vdDisplayErrorMsg(1, 8, szDisMsg);
            return EMV_CHIP_FAILED;
        }
        //VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - end -- jzg
        if(EMV_FALLBACK == shResult)
        {
            //0826
            //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
            //TINE:  10MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, "CHIP NOT DETECTED");
            vdDisplayErrorMsg(1, 8, szDisMsg);
            //0826

            CTOS_RTCGet(&SetRTC);
            inFallbackToMSR = SUCCESS;
            sprintf(strTCT.szFallbackTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
        }
        else if (EMV_FAILURE_EX == shResult){

            //vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
            //TINE:  10MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, "READ CARD FAILED");
            vdDisplayErrorMsg(1, 8, szDisMsg);
            return EMV_FAILURE_EX;

        }
        else
        {
            //vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
            //TINE:  10MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, "READ CARD FAILED");
            vdDisplayErrorMsg(1, 8, szDisMsg);
        }

        vdDebug_LogPrintf("saturn emv chip failed");
        return EMV_CHIP_FAILED;
    }

    if(shResult == EMV_USER_ABORT)
    {


        if(strEMVT.inEMVFallbackAllowed)
        {
            //0826
            //vdDisplayErrorMsg(1, 8, "PLS SWIPE CARD");
            //vdDisplayErrorMsg(1, 8, "CHIP NOT DETECTED");
            //TINE:  10MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, "CHIP NOT DETECTED");
            vdDisplayErrorMsg(1, 8, szDisMsg);
            //0826

            CTOS_RTCGet(&SetRTC);
            inFallbackToMSR = SUCCESS;
            sprintf(strTCT.szFallbackTime,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
        }
        else
        {
            //vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
            //TINE:  10MAY2019
            memset(szTitle, 0x00, sizeof(szTitle));
            szGetTransTitle(srTransRec.byTransType, szTitle);
            strcpy(szDisMsg, "READ CARD FAILED");
            vdDisplayErrorMsg(1, 8, szDisMsg);
        }

        vdDebug_LogPrintf("saturn emv user abort");

        return EMV_USER_ABORT;
    }

    return d_OK;

}

short shCTOS_EMVSecondGenAC(BYTE *szIssuerScript, UINT inIssuerScriptlen)
{
#define ACT_ONL_APPR 1
#define ACT_ONL_DENY 2
#define ACT_UNAB_ONL 3
#define ACT_ONL_ISSUER_REFERRAL 4           //From Host
#define ACT_ONL_ISSUER_REFERRAL_APPR 4
#define ACT_ONL_ISSUER_REFERRAL_DENY 5

    USHORT usResult;
    EMV_ONLINE_RESPONSE_DATA st2ACResponseData;
    BYTE   EMVtagVal[64];
    BYTE bySC_status; //Issue# 000065 -- jzg
    BYTE szTitle[20+1];
    BYTE szDisMsg[100];

#if 0
    int inCase15 = get_env_int("AMEXCASE15");

#endif

    memset(&st2ACResponseData,0,sizeof(st2ACResponseData));

    //st2ACData.iAction will decide trans approve or not
    if(srTransRec.shTransResult == TRANS_COMM_ERROR)
        st2ACResponseData.bAction = ACT_UNAB_ONL;
    else if(srTransRec.shTransResult == TRANS_REJECTED)
        st2ACResponseData.bAction = ACT_ONL_DENY;
    else if(srTransRec.shTransResult == TRANS_CALL_BANK)
        st2ACResponseData.bAction = ACT_ONL_ISSUER_REFERRAL;
    else if(srTransRec.shTransResult == TRANS_AUTHORIZED)
        st2ACResponseData.bAction = ACT_ONL_APPR;

	vdDebug_LogPrintf("saturn bAction %d %d", st2ACResponseData.bAction, srTransRec.shTransResult );


#if 0
    if (inCase15 ==1){
		if(st2ACResponseData.bAction == ACT_ONL_DENY)
    	{
       		vdDebug_LogPrintf("saturn baction change resp code");
        	strcpy(srTransRec.szRespCode, "Z3");
    	}
    }else{
    	if(st2ACResponseData.bAction == ACT_ONL_DENY)
    	{
       		vdDebug_LogPrintf("saturn baction change resp code");
        	strcpy(srTransRec.szRespCode, "05");
    	}
    }
#else
	    if(st2ACResponseData.bAction == ACT_ONL_DENY)
    	{
       		vdDebug_LogPrintf("saturn baction change resp code");
        	strcpy(srTransRec.szRespCode, "05");
    	}


#endif


	vdDebug_LogPrintf("saturn t91 len %d", srTransRec.stEMVinfo.T91Len );


#if 0	
    if (inCase15 ==1){
    	if (srTransRec.shTransResult != TRANS_AUTHORIZED)
    	{
   			memset(srTransRec.stEMVinfo.T91, 0x00, sizeof(srTransRec.stEMVinfo.T91));
			srTransRec.stEMVinfo.T91Len = 0;
    	}
    }
#endif



    //memset(szIssuerScript,0,sizeof(szIssuerScript));
    st2ACResponseData.pAuthorizationCode = srTransRec.szRespCode;
    st2ACResponseData.pIssuerAuthenticationData = srTransRec.stEMVinfo.T91;
    st2ACResponseData.IssuerAuthenticationDataLen = srTransRec.stEMVinfo.T91Len;
    st2ACResponseData.pIssuerScript = szIssuerScript;
    st2ACResponseData.IssuerScriptLen = inIssuerScriptlen;

#if 0
    if (inCase15 == 1){
    //For online processing host replied with IAD"0A 28 FB D1 79 30 77 8E F6 30 31", 
    //however when terminal send 2nd generate AC command, it changed this response code from "30 31" to "30 32" . 
    //Maybe terminal is copying the response code from DF39 in 0210 but not IAD
    	if (srTransRec.shTransResult != TRANS_AUTHORIZED)
    	{
        	if (srTransRec.stEMVinfo.T91Len == 10)
        	{
            	memset(srTransRec.szRespCode,0x00,sizeof(srTransRec.szRespCode));
            	memcpy(srTransRec.szRespCode, &srTransRec.stEMVinfo.T91[8], 2);
        	}
    	}
    }
#endif
    /*no need this for production, may for certification only*/

    #if 0
    if(st2ACResponseData.bAction == ACT_ONL_DENY)
    {
        strcpy(srTransRec.szRespCode, "Z3");
    }
    #endif
	
    usResult = TRANS_AUTHORIZED;

    usResult = usCTOSS_EMV_TxnCompletion(&st2ACResponseData);

    vdDebug_LogPrintf("PP_iCompletion:%d ", usResult);

    inCTOS_SecondGenACGetAndSaveEMVData();

    if(VS_TRUE == strTCT.fDemo)
    {
        usResult = PP_OK;
        EMVtagVal[0] = 0x40;
    }
    if(usResult != PP_OK)
    {
        if(strcmp((char *)srTransRec.szRespCode, "00") ==  0)
        {
            CTOS_SCStatus(d_SC_USER, &bySC_status);
            if (!(bySC_status & d_MK_SC_PRESENT))
            {
                //vdSetErrorMessage("CHIP MALFUNCTION");               
                memset(szDisMsg, 0x00, sizeof(szDisMsg));
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "CARD READ ERROR");
                //usCTOSS_LCDDisplay(szDisMsg);
                inDisplayMessageBoxWithButtonConfirmation(1,8,"","CARD READ ERROR","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                CTOS_Beep();
                //CTOS_Delay(1500);
            }
            else
            {
                //vdSetErrorMessage("EMV Decline");
                memset(szDisMsg, 0x00, sizeof(szDisMsg));
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "EMV Decline");
                //usCTOSS_LCDDisplay(szDisMsg);
                inDisplayMessageBoxWithButtonConfirmation(1,8,"","EMV Decline","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                CTOS_Beep();
            }

            return EMV_CRITICAL_ERROR;
        }
    }

    EMVtagVal[0] = srTransRec.stEMVinfo.T9F27;

    switch( EMVtagVal[0] & 0xC0)
    {
        case 0: //Declined --- AAC
            if(strcmp((char *)srTransRec.szRespCode, "00") ==  0)  //approve by host, but not by card
            {

                /* Issue# 000065 - start -- jzg */
                CTOS_SCStatus(d_SC_USER, &bySC_status);
                if (!(bySC_status & d_MK_SC_PRESENT))
                {
                    //vdSetErrorMessage("CHIP MALFUNCTION");
                    memset(szDisMsg, 0x00, sizeof(szDisMsg));
                    memset(szTitle, 0x00, sizeof(szTitle));
                    szGetTransTitle(srTransRec.byTransType, szTitle);
                    strcpy(szDisMsg, szTitle);
                    strcat(szDisMsg, "|");
                    strcat(szDisMsg, "CARD READ ERROR");
                    //usCTOSS_LCDDisplay(szDisMsg);
                    inDisplayMessageBoxWithButtonConfirmation(1,8,"","CARD READ ERROR","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                    CTOS_Beep();
                    //CTOS_Delay(1500);
                }
                else
                {
                    //vdSetErrorMessage("EMV Decline");
                    memset(szDisMsg, 0x00, sizeof(szDisMsg));
                    memset(szTitle, 0x00, sizeof(szTitle));
                    szGetTransTitle(srTransRec.byTransType, szTitle);
                    strcpy(szDisMsg, szTitle);
                    strcat(szDisMsg, "|");
                    strcat(szDisMsg, "EMV Decline");
                    //usCTOSS_LCDDisplay(szDisMsg);
                    inDisplayMessageBoxWithButtonConfirmation(1,8,"","EMV Decline","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                    CTOS_Beep();
                }
                /* Issue# 000065 - end -- jzg */

                vdDebug_LogPrintf("Host app, card dec");
            }
            else
            {
                vdDebug_LogPrintf("Host reject");
            }
            return EMV_CRITICAL_ERROR;

        case 0x40: //Approval --- TC
            vdDebug_LogPrintf("2nd AC app");
            if (srTransRec.shTransResult != TRANS_AUTHORIZED)  //not approve by host, but approve by card
            {

                /* Issue# 000065 - start -- jzg */
                CTOS_SCStatus(d_SC_USER, &bySC_status);
                if (!(bySC_status & d_MK_SC_PRESENT))
                {
                    //vdSetErrorMessage("CHIP MALFUNCTION");
                    memset(szDisMsg, 0x00, sizeof(szDisMsg));
                    memset(szTitle, 0x00, sizeof(szTitle));
                    szGetTransTitle(srTransRec.byTransType, szTitle);
                    strcpy(szDisMsg, szTitle);
                    strcat(szDisMsg, "|");
                    strcat(szDisMsg, "CARD READ ERROR");
                    //usCTOSS_LCDDisplay(szDisMsg);
                    inDisplayMessageBoxWithButtonConfirmation(1,8,"","CARD READ ERROR","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                    CTOS_Beep();
                    //CTOS_Delay(1500);
                }
                else
                {
                    //vdSetErrorMessage("EMV Decline");
                    memset(szDisMsg, 0x00, sizeof(szDisMsg));
                    memset(szTitle, 0x00, sizeof(szTitle));
                    szGetTransTitle(srTransRec.byTransType, szTitle);
                    strcpy(szDisMsg, szTitle);
                    strcat(szDisMsg, "|");
                    strcat(szDisMsg, "EMV Decline");
                    //usCTOSS_LCDDisplay(szDisMsg);
                    inDisplayMessageBoxWithButtonConfirmation(1,8,"","EMV Decline","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                    CTOS_Beep();
                }
                /* Issue# 000065 - end -- jzg */

                vdDebug_LogPrintf("Host decline, card approve");
                return EMV_CRITICAL_ERROR;
            }
            break;

        default:

            /* Issue# 000065 - start -- jzg */
            CTOS_SCStatus(d_SC_USER, &bySC_status);
            if (!(bySC_status & d_MK_SC_PRESENT))
            {
                //vdSetErrorMessage("CHIP MALFUNCTION");
                memset(szDisMsg, 0x00, sizeof(szDisMsg));
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "CARD READ ERROR");
                //usCTOSS_LCDDisplay(szDisMsg);
                inDisplayMessageBoxWithButtonConfirmation(1,8,"","CARD READ ERROR","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                CTOS_Beep();
                //CTOS_Delay(1500);
            }
            else
            {
                //vdSetErrorMessage("EMV Decline");
                memset(szDisMsg, 0x00, sizeof(szDisMsg));
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "EMV Decline");
                //usCTOSS_LCDDisplay(szDisMsg);
                inDisplayMessageBoxWithButtonConfirmation(1,8,"","EMV Decline","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                CTOS_Beep();
            }
            /* Issue# 000065 - end -- jzg */

            return EMV_CRITICAL_ERROR;

    }


    vdDebug_LogPrintf("End 2nd GenAC shTransResult=%d iAction=%d",srTransRec.shTransResult, st2ACResponseData.bAction);
    return PP_OK;

}

int inCTOS_EMVTCUpload(void)
{
    int inRet;


//1010
    if ((strTCT.fTrickleFeedEMVUpload == VS_FALSE) && (srTransRec.byTransType != SETTLE))
        return d_OK;
//1010

//0929
    //if( CARD_ENTRY_ICC == srTransRec.byEntryMode)
    if((CARD_ENTRY_ICC == srTransRec.byEntryMode)||

       ((CARD_ENTRY_WAVE == srTransRec.byEntryMode) &&
        //((srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_QVSDC) ||
        ((srTransRec.bWaveSID == 0x65) ||
         (srTransRec.bWaveSID == d_VW_SID_AE_EMV) ||
         (srTransRec.bWaveSID == d_VW_SID_CUP_EMV) ||
         (srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP) ||
         //(srTransRec.bWaveSID == d_VW_SID_JCB_WAVE_2) ||
         (srTransRec.bWaveSID == 0x63) ||
         (srTransRec.bWaveSID == d_EMVCL_SID_DISCOVER_DPAS) ||
         (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC))))
//0929
    {
        inCTLOS_Updatepowrfail(PFR_IDLE_STATE);
        //inRet = inProcessEMVTCUpload(&srTransRec, -1);// TC upload

        //if(inCheckConnection() != d_OK) //REMOVE CONDITION AS ANDROID WILL NOT SUPPORT ETHERNET. NO NEED TO CHECK FOR CONNECTION
        //return ST_ERROR;

        inRet = inProcessEMVTCUpload(&srTransRec, 1);// TC upload

        vdDebug_LogPrintf("szFileName, %s%02d%02d.rev"
        , strHDT.szHostLabel
        , strHDT.inHostIndex
        , srTransRec.MITid);
//        inCTOS_inDisconnect();
    }

    return d_OK;
}


int inCTOS_GetPubKey(const char *filename, unsigned char *modulus, int *mod_len, unsigned char *exponent, int *exp_len)
{
    unsigned char tmp[1024];
    int iRead;
    int iMod;
    int iExp;
    FILE  *fPubKey;
    UINT uintRet ;

    fPubKey = fopen( (char*)filename, "rb" );
    if (fPubKey == NULL)
        return CTOS_RET_PARAM;

    uintRet = fread( tmp, 1, sizeof(tmp), fPubKey );
    fclose(fPubKey);
    vdDebug_LogPrintf("CAPK=Len[%d]==[%s]",uintRet,tmp);

    if(uintRet >0)
    {
        iMod=(int)(tmp[0]-0x30)*100+(tmp[1]-0x30)*10+(tmp[2]-0x30);
        vdDebug_LogPrintf("iMod===[%d]",iMod);
        if(iMod%8 != 0)
            return(CTOS_RET_PARAM);

        if(iMod > CTOS_PED_RSA_MAX)
            return(CTOS_RET_PARAM);

        *mod_len=iMod;
        wub_str_2_hex((tmp+3), (modulus), iMod*2);

        vdDebug_LogPrintf("*mod_len===[%d]",*mod_len);
        DebugAddHEX("Module HEX string===", modulus, iMod);


        iExp=(int)tmp[iMod*2+4] - 0x30;
        wub_str_2_hex((&tmp[5+(iMod*2)]), (exponent), iExp*2);

        vdDebug_LogPrintf("iExp===[%d]",iExp);
        DebugAddHEX("Exponent HEX string===", exponent, iExp);

        *exp_len = iExp;
    }
    else
        return(CTOS_RET_CALC_FAILED);

    return(CTOS_RET_OK);
}

USHORT ushCTOS_EMV_NewDataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue)
{
    USHORT usResult;
    USHORT usTagLen;
    static USHORT usGetEMVTimes = 0;

    usResult = usCTOSS_EMV_DataGet(usTag, &usTagLen, pValue);
    *pLen = usTagLen;

    usGetEMVTimes ++;
    vdDebug_LogPrintf("ushCTOS_EMV_NewDataGet Times[%d] usTagLen[%d]",usGetEMVTimes, usTagLen);
    return usResult;

}

USHORT ushCTOS_EMV_NewTxnDataSet(IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue)
{
    USHORT usResult;
    static USHORT usSetEMVTimes = 0;

    usResult = usCTOSS_EMV_DataSet(usTag, usLen, pValue);
    usSetEMVTimes ++;
    vdDebug_LogPrintf("ushCTOS_EMV_NewTxnDataSet Times[%d] usResult[%d]",usSetEMVTimes, usResult);

    return usResult;
}

short shCTOS_EMVGetChipDataReady(void)
{
    short       shResult;
    BYTE        byDataTmp1[64];
    BYTE        byVal[64];
    USHORT      usLen;
    USHORT      inIndex ;
    BYTE        szDataTmp[5];
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];

    USHORT inTagLen = 0;




    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szOutEMVData,0,sizeof(szOutEMVData));

    //for improve transaction speed
    //shResult = usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_SELECT_APP, &inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);
    DebugAddHEX("GET_EMV_TAG_AFTER_SELECT_APP",szOutEMVData,inTagLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_57, byVal, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_57[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, byVal[0], byVal[1], byVal[2]);

    memset(byDataTmp1, 0x00, sizeof(byDataTmp1));
    wub_hex_2_str(byVal, byDataTmp1, usLen);
    memcpy(srTransRec.szTrack2Data, byDataTmp1, (usLen*2));
    for(inIndex = 0; inIndex < (usLen*2); inIndex++)
    {
        if(byDataTmp1[inIndex] == 'F')
            srTransRec.szTrack2Data[inIndex]=0;
    }
    vdDebug_LogPrintf("szTrack2Data: %s %d", srTransRec.szTrack2Data, inIndex);

    srTransRec.usTrack2Len = inIndex - 1; //BDOCLG-00110: CUP EMV contact has no DE35 -- jzg

    for(inIndex = 0; inIndex < (usLen*2); inIndex++)
    {
        if(byDataTmp1[inIndex] != 'D')
            srTransRec.szPAN[inIndex] = byDataTmp1[inIndex];
        else
            break;
    }
    srTransRec.byPanLen = inIndex;
    vdDebug_LogPrintf("PAN: %s %d", srTransRec.szPAN, inIndex);
    inIndex++;
    memset(szDataTmp, 0x00, sizeof(szDataTmp));
    wub_str_2_hex(&byDataTmp1[inIndex], szDataTmp, 4);
    srTransRec.szExpireDate[0] = szDataTmp[0];
    srTransRec.szExpireDate[1] = szDataTmp[1];
    vdMyEZLib_LogPrintf("EMV functions Expiry Date [%02x%02x]",srTransRec.szExpireDate[0],srTransRec.szExpireDate[1]);
    inIndex = inIndex + 4;
    memcpy(srTransRec.szServiceCode, &byDataTmp1[inIndex], 3);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5A_PAN, srTransRec.stEMVinfo.T5A, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_5A_PAN[%d] usLen[%d] [%02X %02X %02X]--", shResult, usLen, srTransRec.stEMVinfo.T5A[0], srTransRec.stEMVinfo.T5A[1], srTransRec.stEMVinfo.T5A[2]);

    srTransRec.stEMVinfo.T5A_len = (BYTE)usLen;
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F30_SERVICE_CODE, srTransRec.stEMVinfo.T5F30, &usLen, szOutEMVData, inTagLen);

    memset(byVal, 0x00, sizeof(byVal));
    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F34_PAN_IDENTFY_NO, byVal, &usLen, szOutEMVData, inTagLen);
    vdMyEZLib_LogPrintf("5F34: %02x %d", byVal[0], usLen);
    srTransRec.stEMVinfo.T5F34_len = usLen;
    srTransRec.stEMVinfo.T5F34 = byVal[0];

    shResult = usCTOSS_FindTagFromDataPackage(TAG_82_AIP, srTransRec.stEMVinfo.T82, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_82_AIP-[%02x][%02x]-", srTransRec.stEMVinfo.T82[0], srTransRec.stEMVinfo.T82[1]);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_84_DF_NAME, srTransRec.stEMVinfo.T84, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T84_len = (BYTE)usLen;

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F24_EXPIRE_DATE, srTransRec.stEMVinfo.T5F24, &usLen, szOutEMVData, inTagLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F08_IC_VER_NUMBER, szDataTmp, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_9F08_IC_VER_NUMBER-[%02x][%02x]-",szDataTmp[0],szDataTmp[1]);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("-------TAG_9F09_TERM_VER_NUMBER-[%02x][%02x]-",srTransRec.stEMVinfo.T9F09[0],srTransRec.stEMVinfo.T9F09[1]);
    if(usLen == 0)
        memcpy(srTransRec.stEMVinfo.T9F09, "\x00\x4C", 2);// can not get value from api like verifone,so i hardcode a value from EMV level 2 cert document
    vdDebug_LogPrintf("9F09: %02x%02x %d", srTransRec.stEMVinfo.T9F09[0],srTransRec.stEMVinfo.T9F09[1], usLen);

    shResult = usCTOSS_FindTagFromDataPackage(TAG_5F20, srTransRec.szCardholderName, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("5F20,szCardholderName: %s", srTransRec.szCardholderName);
}

int inCTOS_FirstGenACGetAndSaveEMVData(void)
{
    USHORT usLen = 64;
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;
    int ret = 0;
    unsigned char szTransSeqCounter[6+1];
    unsigned char szHEXTransSeqCounter[3+1];

    BYTE bAppLabel[64] = {0}; // EMV: Get Application Label -- jzg
    BYTE bAppPrefName[64] = {0}; // EMV: Get Application Preferred Name -- jzg
    BYTE szTermSerialNum[8+1];

    //1026
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    //1026

    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szGetEMVData,0,sizeof(szGetEMVData));

    //for improve transaction speed
    //usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_1STAC, &inTagLen, szOutEMVData);
    inDatabase_TerminalOpenDatabaseEx(DB_EMV);
    inMultiAP_Database_EMVTransferDataReadEx(&inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataInitEx();
    inDatabase_TerminalCloseDatabase();
    DebugAddHEX("GET_EMV_TAG_AFTER_1STAC",szOutEMVData,inTagLen);

    vdDebug_LogPrintf("inCTOS_FirstGenACGetAndSaveEMVData");


    //1026
    if (strCST.inCurrencyIndex > 1){
        memset(szAscBuf, 0x00, sizeof(szAscBuf));
        memset(szBcdBuf, 0x00, sizeof(szBcdBuf));

        sprintf(szAscBuf,"%4s",strCST.szCurCode);

        wub_str_2_hex(szAscBuf, szBcdBuf, 4);
        memcpy((char *)srTransRec.stEMVinfo.T5F2A, &szBcdBuf[0], 2);

        /* BDO-00168: Make sure to populate EMV Tag 5F2A with the correct txn currency code -- jzg */
        ushCTOS_EMV_NewTxnDataSet(TAG_5F2A_TRANS_CURRENCY_CODE, 2, srTransRec.stEMVinfo.T5F2A);
    }else
        //1026
        usCTOSS_FindTagFromDataPackage(TAG_5F2A_TRANS_CURRENCY_CODE, srTransRec.stEMVinfo.T5F2A, &usLen, szOutEMVData, inTagLen);

    memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof(srTransRec.stEMVinfo.szChipLabel));

    /* EMV: Get Application Preferred Name - start -- jzg */
    usCTOSS_FindTagFromDataPackage(TAG_9F12, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);
    vdDebug_LogPrintf("TAG 9F12 = [%s]", srTransRec.stEMVinfo.szChipLabel);
    /* EMV: Get Application Preferred Name - end -- jzg */

    if(strlen(srTransRec.stEMVinfo.szChipLabel)<=0)
        usCTOSS_FindTagFromDataPackage(TAG_50, srTransRec.stEMVinfo.szChipLabel, &usLen, szOutEMVData, inTagLen);

#if 0
    /* EMV: Get Application Preferred Name - start -- jzg */
	usCTOSS_FindTagFromDataPackage(TAG_9F12, bAppPrefName, &usLen, szOutEMVData, inTagLen);
	vdDispAppLabel(bAppPrefName, usLen, srTransRec.stEMVinfo.szChipLabel);
	vdDebug_LogPrintf("TAG 9F12 = [%s]", srTransRec.stEMVinfo.szChipLabel);
	/* EMV: Get Application Preferred Name - end -- jzg */

	vdDebug_LogPrintf("TAG 9F12 = [0x%02X]", srTransRec.stEMVinfo.szChipLabel[0]);

	/* EMV: Get Application Label - start -- jzg */
	if ((!((srTransRec.stEMVinfo.szChipLabel[0] >= 'a') && (srTransRec.stEMVinfo.szChipLabel[0] <= 'z'))) ||
	(!((srTransRec.stEMVinfo.szChipLabel[0] >= 'A') && (srTransRec.stEMVinfo.szChipLabel[0] <= 'Z'))))
	{
		//memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof(srTransRec.stEMVinfo.szChipLabel));
		usCTOSS_FindTagFromDataPackage(TAG_50, bAppLabel, &usLen, szOutEMVData, inTagLen);

		if (strlen(bAppLabel) > 0){
			memset(srTransRec.stEMVinfo.szChipLabel, 0, sizeof(srTransRec.stEMVinfo.szChipLabel));
			vdDispAppLabel(bAppLabel, usLen, srTransRec.stEMVinfo.szChipLabel);
			vdDebug_LogPrintf("TAG 50 = [%s]", srTransRec.stEMVinfo.szChipLabel);
		}
	}
#endif

    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9A_TRANS_DATE, srTransRec.stEMVinfo.T9A, &usLen, szOutEMVData, inTagLen);


    usCTOSS_FindTagFromDataPackage(TAG_84_DF_NAME, srTransRec.stEMVinfo.T84, &usLen, szOutEMVData, inTagLen);


    vdDebug_LogPrintf("TAG_84_DF_NAME %02x %02x %02x %02x %02x",srTransRec.stEMVinfo.T84[0],srTransRec.stEMVinfo.T84[1], srTransRec.stEMVinfo.T84[2],srTransRec.stEMVinfo.T84[3], srTransRec.stEMVinfo.T84[4]);

    /* BDO: Quasi should be parametrized per issuer - start -- jzg */
    if((strIIT.fQuasiCash)  && ((srTransRec.byTransType == SALE) || (srTransRec.byTransType == SALE_OFFLINE)))
        if ((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x00\x03",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x00\x03",5) == 0))
            srTransRec.stEMVinfo.T9C = 0x11;
        else
            usCTOSS_FindTagFromDataPackage(TAG_9C_TRANS_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9C), &usLen, szOutEMVData, inTagLen);
        /* BDO: Quasi should be parametrized per issuer - end -- jzg */
    else
        usCTOSS_FindTagFromDataPackage(TAG_9C_TRANS_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9C), &usLen, szOutEMVData, inTagLen);
    //Issue# 000141 - end -- jzg


    vdDebug_LogPrintf("inCTOS_FirstGenACGetAndSaveEMVData %d", TAG_9C_TRANS_TYPE);

    usCTOSS_FindTagFromDataPackage(TAG_9F06, srTransRec.stEMVinfo.T9F06, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F09_TERM_VER_NUMBER, srTransRec.stEMVinfo.T9F09, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F10_len = usLen;


    //1026
    //if (strCST.inCurrencyIndex > 1){
    //	memset(szAscBuf, 0x00, sizeof(szAscBuf));
    //	memset(szBcdBuf, 0x00, sizeof(szBcdBuf));

    //	sprintf(szAscBuf,"%4s",strCST.szCurCode);

    //	wub_str_2_hex(szAscBuf, szBcdBuf, 4);
    //	memcpy((char *)srTransRec.stEMVinfo.T9F1A, &szBcdBuf[0], 2);
    //}else
    //1026
    usCTOSS_FindTagFromDataPackage(TAG_9F1A_TERM_COUNTRY_CODE, srTransRec.stEMVinfo.T9F1A, &usLen, szOutEMVData, inTagLen);

    //usCTOSS_FindTagFromDataPackage(TAG_9F1E, srTransRec.stEMVinfo.T9F1E, &usLen, szOutEMVData, inTagLen);
    memset(szTermSerialNum, 0x00, sizeof(szTermSerialNum));
    CTOS_GetFactorySN(szTermSerialNum);
    memcpy(srTransRec.stEMVinfo.T9F1E,&szTermSerialNum[8],8);

    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, szOutEMVData, inTagLen);

    srTransRec.stEMVinfo.T9F27 = 0x00; //Reset T9F27 to reset go online value on previous EMV Processing before connecting Bin Routing host.
    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F33_TERM_CAB, srTransRec.stEMVinfo.T9F33, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F35_TERM_TYPE, (BYTE *)&(srTransRec.stEMVinfo.T9F35), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F36_ATC, srTransRec.stEMVinfo.T9F36, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F36_len = usLen;

    usCTOSS_FindTagFromDataPackage(TAG_9F37_UNPREDICT_NUM, srTransRec.stEMVinfo.T9F37, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F53, (BYTE *)&(srTransRec.stEMVinfo.T9F53), &usLen, szOutEMVData, inTagLen);

    inDatabase_TerminalOpenDatabaseEx(DB_TERMINAL);
    ret = inIITReadEx(srTransRec.IITid);
    vdDebug_LogPrintf("inIITRead[%d]",ret);
    sprintf(szTransSeqCounter, "%06ld", strIIT.ulTransSeqCounter);
    wub_str_2_hex(szTransSeqCounter, (char *)szHEXTransSeqCounter, 6);
    memcpy(srTransRec.stEMVinfo.T9F41,szHEXTransSeqCounter,3);
    strIIT.ulTransSeqCounter++;
    ret = inIITSaveEx(srTransRec.IITid);
    inDatabase_TerminalCloseDatabase();
    vdDebug_LogPrintf(" ret[%d] srTransRec.IITid[%d]strIIT.ulTransSeqCounter[%ld]",ret, srTransRec.IITid,strIIT.ulTransSeqCounter);
    //usCTOSS_FindTagFromDataPackage(TAG_9F41, srTransRec.stEMVinfo.T9F41, &usLen, szOutEMVData, inTagLen);

    return d_OK;
}

int inCTOS_SecondGenACGetAndSaveEMVData(void)
{

    USHORT usLen = 64;
    BYTE szGetEMVData[128];
    BYTE szOutEMVData[2048];
    USHORT inTagLen = 0;

    memset(szGetEMVData,0,sizeof(szGetEMVData));
    memset(szGetEMVData,0,sizeof(szGetEMVData));

    //usCTOSS_EMV_MultiDataGet(GET_EMV_TAG_AFTER_2NDAC, &inTagLen, szOutEMVData);
    inMultiAP_Database_EMVTransferDataRead(&inTagLen, szOutEMVData);//Improve 2nd Gen AC speed
    DebugAddHEX("GET_EMV_TAG_AFTER_2NDAC",szOutEMVData,inTagLen);

    vdDebug_LogPrintf("inCTOS_SecondGenACGetAndSaveEMVData");

    usCTOSS_FindTagFromDataPackage(TAG_95, srTransRec.stEMVinfo.T95, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F10_IAP, srTransRec.stEMVinfo.T9F10, &usLen, szOutEMVData, inTagLen);
    srTransRec.stEMVinfo.T9F10_len = usLen;

    usCTOSS_FindTagFromDataPackage(TAG_9F26_EMV_AC, srTransRec.stEMVinfo.T9F26, &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F27, (BYTE *)&(srTransRec.stEMVinfo.T9F27), &usLen, szOutEMVData, inTagLen);

    usCTOSS_FindTagFromDataPackage(TAG_9F34_CVM, srTransRec.stEMVinfo.T9F34, &usLen, szOutEMVData, inTagLen);

    return d_OK;
}

int inCTOS_showEMV_TagLog (void)
{
#define TOTAL_TAGS  24
    int i;
    unsigned short tagLen;
    char outp[40];

    typedef struct
    {
        unsigned short Tags;
        char description[20];
    }print_tag;

    print_tag EMVTag[TOTAL_TAGS] =
            {
                    {   TAG_95,                     "TVR"},
                    {   TAG_9B,                     "TSI"},
                    {   TAG_9F26_EMV_AC,            "ARQC"},
                    {   TAG_9F27,                   "Crypt Info Data"},
                    {   TAG_9F10_IAP,               "Issuer Appl Data"},
                    {   TAG_9F37_UNPREDICT_NUM,     "Unpredicte number"},
                    {   TAG_9F36_ATC,               "Appl Trans Counter"},
                    {   TAG_9A_TRANS_DATE,          "Trans Date"},
                    {   TAG_9C_TRANS_TYPE,          "Trans Type"},
                    {   TAG_5A_PAN,                 "PAN"},
                    {   TAG_5F34_PAN_IDENTFY_NO,    "Sequence Num"},
                    {   TAG_9F02_AUTH_AMOUNT,       "Amount Authorized"},
                    {   TAG_9F03_OTHER_AMOUNT,      "Add Amount"},
                    {   TAG_5F2A_TRANS_CURRENCY_CODE, "Trans Currency Code"},
                    {   TAG_82_AIP,                 "AIP"},
                    {   TAG_9F1A_TERM_COUNTRY_CODE, "Term Country Code"},
                    {   TAG_9F34_CVM,               "CVR"},
                    {   TAG_9F10_IAP,               "Issuer auth Data"},
                    {   TAG_9F06,                   "AID"},
                    {   TAG_50,                     "Appl Lable Name"},
                    {   TAG_8F,                     "CA Public Key Index"},
                    {   TAG_9F0D,                   "IAC Default"},
                    {   TAG_9F0E,                   "IAC Denial"},
                    {   TAG_9F0F,                   "IAC Online"}

            };


    for(i = 0; i<TOTAL_TAGS; i++)
    {
        memset(outp,0x00,sizeof(outp));
        ushCTOS_EMV_NewDataGet(EMVTag[i].Tags, &tagLen, outp);
        vdDebug_LogPrintf("----TAG[%s][%x]=====Len[%d]----",EMVTag[i].description,EMVTag[i].Tags,tagLen);
        DebugAddHEX("Value===",outp,tagLen);
    }
    return d_OK;
}

void vdCTOSS_GetAmt(void)
{
    memcpy(srTransRec.szBaseAmount, szBaseAmount, 6);
}

void vdCTOSS_SetAmt(BYTE *baAmount)
{
    BYTE szTemp[20];

    memset(szTemp, 0x00, sizeof(szTemp));
    // patrick add code 20141216
    sprintf(szTemp, "%012.0f", atof(baAmount));
    wub_str_2_hex(szTemp, szBaseAmount,12);
}


//Installment - Promo selection function -- jzg
#if 0
int inCTOS_SelectInstallmentPromo(void)
{
    int inNumRecs = 0,
            key = 0,
            i,
            bHeaderAttr = 0x01+0x04;
    char szHeaderString[50+1];
    char szPromoLabelList[2000+1] = {0};
	char szDisMsg[100];
	char szTitle[20];


    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;


    vdDebug_LogPrintf("saturn inCTOS_SelectInstallmentPromo");

    /*  if (strCDT.fInstallmentEnable == 0){
		CTOS_LCDTPrintXY(1, 8, "INST NOT ALLOWED");
		CTOS_Delay(3000);
		return d_NO;
    }*/

    //Select Instalment Promo type

    memset(szHeaderString, 0x00,sizeof(szHeaderString));
    memset(szPromoLabelList, 0x00,sizeof(szPromoLabelList));
    strcpy(szHeaderString,"SELECT PROMO");

	if(strCDT.inInstGroup == HSBC_INST_GROUP)
	{
		strCDT.HDTid = HSBC_HDT_INDEX;
		srTransRec.PRMid = 37;
		
		return d_OK;
	}
		
	memset(strMultiPRM, 0, sizeof(strMultiPRM));
	inPRMReadbyinInstGroup(strCDT.inInstGroup, &inNumRecs);
	
	for (i=0; i<inNumRecs; i++)
	{
		if (strMultiPRM[i].szPromoLabel[0] != 0)
		{
			strcat((char *)szPromoLabelList, strMultiPRM[i].szPromoLabel);
			if (strMultiPRM[i+1].szPromoLabel[0] != 0)
				strcat((char *)szPromoLabelList, (char *)" \n");			
		}
	}

    //inSetColorMenuMode();
    key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szPromoLabelList, TRUE);
    //key = MenuDisplayEx(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szPromoLabelList, TRUE, inGetIdleTimeOut(TRUE)); /*Menu with timeout parameter*/
    //inSetTextMode();


    vdDebug_LogPrintf("saturn selected installment promo %d", key);
    //TINE:  30MAY2019
    //if (key > 0)
    //{
    memset(szDisMsg, 0x00, sizeof(szDisMsg));
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType,szTitle);
	
    if (key == 0xFF) //TimeOut
    {
		vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		CTOS_Beep();
		CTOS_Delay(1000);
        return -1;
    }
    else if (key == d_KBD_CANCEL)
    {
		vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		CTOS_Beep();
		CTOS_Delay(1000);
        return -1;
    }

    strCDT.HDTid = strMultiPRM[key-1].HDTid;
    //strcpy(srTransRec.szPromo, strMultiPRM[key-1].szPromoCode);
    srTransRec.PRMid=strMultiPRM[key-1].PRMid;
    key = d_OK;

    vdDebug_LogPrintf("saturn selected installment promo exit, KEY = [%d]", key);

    return(key);
}
#else
int inCTOS_SelectInstallmentPromo(void)
{
    int inNumRecs = 0,
            key = 0,
            i,
            bHeaderAttr = 0x01+0x04;
    char szHeaderString[50+1];
    char szPromoLabelList[2000+1] = {0};
	char szDisMsg[100];
	char szTitle[20];
	BYTE szMessage[2000 + 1] = {0};
	BYTE szAmount[40 + 1] = {0};
	
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;


    vdDebug_LogPrintf("saturn inCTOS_SelectInstallmentPromo");
	
    //Select Instalment Promo type

    memset(szHeaderString, 0x00,sizeof(szHeaderString));
    memset(szPromoLabelList, 0x00,sizeof(szPromoLabelList));
    strcpy(szHeaderString,"SELECT PROMO");

	if(strCDT.inInstGroup == HSBC_INST_GROUP)
	{
		strCDT.HDTid = HSBC_HDT_INDEX;
		srTransRec.PRMid = 37;
		
		return d_OK;
	}
		
	memset(strMultiPRM, 0, sizeof(strMultiPRM));
	inPRMReadbyinInstGroup(strCDT.inInstGroup, &inNumRecs);
	
	for (i=0; i<inNumRecs; i++)
	{
		if (strMultiPRM[i].szPromoLabel[0] != 0)
		{
			strcat((char *)szPromoLabelList, strMultiPRM[i].szPromoLabel);
			if (strMultiPRM[i+1].szPromoLabel[0] != 0)
				strcat((char *)szPromoLabelList, (char *)"|");
		}
	}

	vdDebug_LogPrintf("szPromoLabelList=[%s]", szPromoLabelList);

	GetTransAmount(szAmount);
	memset(szMessage, 0x00, sizeof(szMessage));
	sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_VIEW, MENU_1_COL_COUNT, "Installment", "Select a promo");
	strcat(szMessage, szPromoLabelList);
	
	key = inMenuSelection(szMessage);	
    vdDebug_LogPrintf("inMenuSelection=[%d]", key);
   
    //memset(szDisMsg, 0x00, sizeof(szDisMsg));
	//memset(szTitle, 0x00, sizeof(szTitle));
	//szGetTransTitle(srTransRec.byTransType,szTitle);
	
    if (key == 0xFF || key == d_TIMEOUT) //TimeOut
    {
		//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return d_NO;
    }
    else if (key == d_KBD_CANCEL || key == d_USER_CANCEL)
    {
		//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return d_NO;
    }

	key = inGetMenuKeySelected(szPromoLabelList, strGBLVar.szGBLvProcessDesc);
    vdDebug_LogPrintf("inGetMenuKeySelected=[%d]", key);

	if (key > 0)
	{
		strCDT.HDTid = strMultiPRM[key-1].HDTid;
    	//strcpy(srTransRec.szPromo, strMultiPRM[key-1].szPromoCode);
  	 	 srTransRec.PRMid=strMultiPRM[key-1].PRMid;
   		 key = d_OK;
	}
	else
	{
		return d_NO;
	}
    
    vdDebug_LogPrintf("saturn selected installment promo exit, KEY = [%d]", key);

    return(key);
}

#endif
//gcitra

int inCLearTablesStructure(void){

    memset(&strCDT,0x00, sizeof(STRUCT_CDT));
    memset(&strIIT,0x00, sizeof(STRUCT_IIT));
    memset(&strEMVT,0x00, sizeof(STRUCT_EMVT));
    memset(&strHDT,0x00, sizeof(STRUCT_HDT));
    memset(srTransRec.szBaseAmount, 0x00, sizeof(srTransRec.szBaseAmount));
    memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));
    memset(&srTransRec,0x00, sizeof(TRANS_DATA_TABLE));
	
	//SMACPAY
	#ifdef SMAC_MIFARE
	memset(&strPersonal_Info,0x00,sizeof(STRUCT_SMAC_PERSONAL_INFO));
	memset(&strMembership_Info,0x00,sizeof(STRUCT_SMAC_MEMBERSHIP_INFO));
	memset(&strDemographic_Info,0x00,sizeof(STRUCT_SMAC_DEMOGRAPHIC_INFO));
	memset(&strEmployment_Info,0x00,sizeof(STRUCT_SMAC_EMPLOYMENT_INFO));
	memset(&strVersion,0x00,sizeof(VERSION_INFO));
	#endif
	
    return d_OK;
}



int inConfirmPAN(void){

    unsigned char key;
    int inRet;

	return d_OK; // commented: uneeded for android -- sidumili
	
    //gcitra-0806
    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    //gcitra-0806


    if (strTCT.fConfirmPAN){
        while(1){
            key=WaitKey(15);

            if (key == d_KBD_CANCEL){
                inRet = d_NO;
                break;

            }

            if(key == d_KBD_ENTER){
                inRet =  d_OK;
                break;
            }

        }
    }

        //smac
#if 0

        if (srTransRec.HDTid == SMAC_HDT_INDEX)
		{
		if (fSMACTRAN == FALSE) {

			CTOS_LCDTClearDisplay();
			//vdDisplayErrorMsg(1, 8, "TRANS NOT ALLOWED");
			//vduiWarningSound();
			//CTOS_Delay(2000);
			vdSetErrorMessage("TRANS NOT ALLOWED");
			inRet = d_NO;
		}
	}
#endif
    //smac

    vdDebug_LogPrintf("inConfirmPAN");

    return inRet;
}

int inCTOS_SelectAccountType(void)
{
    BYTE key = 0;
    int inRet = 0;
    char szDebug[40+1] = {0};

    vdDebug_LogPrintf("inCTOS_SelectAccountType[START]");

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);

    do
    {
        setLCDPrint27(3,DISPLAY_POSITION_LEFT, "SELECT ACCOUNT:");
        CTOS_LCDTPrintXY(1, 4, "[F1]SAVINGS");
        CTOS_LCDTPrintXY(1, 5, "[F2]CURRENT");

        key = WaitKey(60);
        if (key == d_KBD_F1)
        {
            inRet = d_OK;
            inAccountType = SAVINGS_ACCOUNT;
            srTransRec.inAccountType =SAVINGS_ACCOUNT;
            break;
        }
        else if (key == d_KBD_F4)
        {
            inRet = d_OK;
            inAccountType = CURRENT_ACCOUNT;
            srTransRec.inAccountType =CURRENT_ACCOUNT;
            break;
        }
        else if (key == d_KBD_CANCEL)
        {
            inRet = d_NO;
            break;
        }
        else
            vduiWarningSound();

    }
    while(1);

    // display selected account
    if (inRet != d_NO)
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);

        switch (inAccountType)
        {
            case CURRENT_ACCOUNT:
                setLCDPrint27(7,DISPLAY_POSITION_CENTER, "CURRENT ACCOUNT");
                break;
            case SAVINGS_ACCOUNT:
                setLCDPrint27(7,DISPLAY_POSITION_CENTER, "SAVINGS ACCOUNT");
                break;
            default:
                break;
        }
        WaitKey(1);
    }

    vdDebug_LogPrintf("inCTOS_SelectAccountType[END]");

    return inRet;
}




int inCTOS_SelectCurrency(void)
{
    BYTE key = 0;
    int inRet = 0;
    char szDebug[41] = {0},
            szAscBuf[5] = {0},
            szBcdBuf[3] = {0};

    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04;
    BYTE Menukey = 0;
    int inCurrencyIdx;
    char szCurrDsp1[4+1], szCurrDsp2[4+1];

    fUSDSelected = FALSE;
    vdDebug_LogPrintf("inCTOS_SelectCurrency[START]");

    vdDebug_LogPrintf("fDualCurrency[%d] :: fDualCurrencyEnable[%d]",strTCT.fDualCurrency, strCDT.fDualCurrencyEnable);

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;


    if (strTCT.fDualCurrency == VS_FALSE)
        return d_OK;

    if (strCDT.fDualCurrencyEnable == VS_FALSE)
        return d_OK;

    //CTOS_LCDTClearDisplay();
#if 0
    memset(szHeaderString, 0x00, sizeof(szHeaderString));
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));

    strcpy(szHeaderString, "Select Currency");
    strcat(szChoiceMsg,"PHP \n");
    strcat(szChoiceMsg,"USD");
		inSetColorMenuMode();
    //Menukey = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
	Menukey = MenuDisplayEx(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE, inGetIdleTimeOut(TRUE)); /*Menu with timeout parameter*/
	vdDebug_LogPrintf("::inCTOS_SelectCurrency::MenuDisplayEx::Menukey[%d]", Menukey);
    inSetTextMode();
#endif

    memset(szCurrDsp1,0,sizeof(szCurrDsp1));
    memset(szCurrDsp2,0,sizeof(szCurrDsp2));

    vdDispTransTitle(srTransRec.byTransType);
    setLCDPrint27(3,DISPLAY_POSITION_LEFT,"SELECT CURRENCY");
    inCSTRead(1);
    sprintf(szCurrDsp1,"1.%4s", strCST.szCurSymbol);
    setLCDPrint27(4,DISPLAY_POSITION_LEFT,szCurrDsp1);
    inCSTRead(2);
    sprintf(szCurrDsp2,"2.%4s", strCST.szCurSymbol);
    setLCDPrint27(5,DISPLAY_POSITION_LEFT,szCurrDsp2);

    inCurrencyIdx = strHDT.inCurrencyIdx;
    inCSTRead(inCurrencyIdx);

    while(1){

        Menukey=WaitKey(inGetIdleTimeOut(TRUE));

        if(Menukey == d_KBD_1)
        {
            Menukey = 1;
            break;
        }
        else if(Menukey == d_KBD_2)
        {
            Menukey = 2;
            break;
        }
        else if(Menukey == d_KBD_CANCEL)
        {
            Menukey = d_KBD_CANCEL;
            break;
        }
        else if (Menukey == 0xFF) /*BDO: For timeout occured -- sidumili*/
        {
            fTimeOutFlag = TRUE;	/*BDO: Flag for timeout --sidumili*/
            Menukey = 0xFF;
            break;
        }

    }


    memset(szCurrDsp1,0,sizeof(szCurrDsp1));
    memset(szCurrDsp2,0,sizeof(szCurrDsp2));

    if(Menukey == 1)
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
        inCSTRead(1);
        sprintf(szCurrDsp1,"%4s SELECTED?", strCST.szCurSymbol);
        setLCDPrint27(4,DISPLAY_POSITION_CENTER,szCurrDsp1);

        inRet = d_OK;
    }
    else if(Menukey == 2)
    {
        CTOS_LCDTClearDisplay();
        vdDispTransTitle(srTransRec.byTransType);
        inCSTRead(2);
        sprintf(szCurrDsp2,"%4s SELECTED?", strCST.szCurSymbol);
        setLCDPrint27(4,DISPLAY_POSITION_CENTER,szCurrDsp2);
        inRet = d_OK;

        strCDT.HDTid = strCDT.inDualCurrencyHost;

        inCSTRead(strCDT.HDTid);
        memset(szAscBuf, 0x00, sizeof(szAscBuf));
        memset(szBcdBuf, 0x00, sizeof(szBcdBuf));
        sprintf(szAscBuf,"%4s",strCST.szCurCode);
        wub_str_2_hex(szAscBuf, szBcdBuf, 4);
        memcpy((char *)srTransRec.stEMVinfo.T5F2A, &szBcdBuf[0], 2);
        DebugAddHEX("T5F2A..",srTransRec.stEMVinfo.T5F2A,2);
        fUSDSelected = TRUE;
    }
    else if (Menukey == 0xFF) /*BDO: For timeout occured -- sidumili*/
    {
        fTimeOutFlag = TRUE;	/*BDO: Flag for timeout --sidumili*/
        return(d_NO);
    }
    else if(Menukey == d_KBD_CANCEL)
    {
        return -1;
    }

    do
    {
        setLCDPrint27(8,DISPLAY_POSITION_CENTER,"ENTER TO CONFIRM");
        key=WaitKey(60);
        if (key ==d_KBD_ENTER)
        {
            inRet = d_OK;
            break;
        }
        else if (key == d_KBD_CANCEL)
        {
            inRet = d_NO;
            break;
        }
        else
            vduiWarningSound();
    }
    while(1);

    return inRet;
}


/* EMV: Get Application Label - start -- jzg */
void vdDispAppLabel(unsigned char *ucHex, int inLen, char *szOutStr)
{
    int i;
    char szBuf[80] = {0};

    for (i = 0; i < inLen; i++)
        szBuf[i] = ucHex[i];
    szBuf[i] = 0x00;

    memcpy(szOutStr, szBuf, inLen);
}
/* EMV: Get Application Label - end -- jzg */


int inCTOS_EMVSetTransType(BYTE byTransType)
{
    switch(byTransType)
    {
        /* BDOCLG-00161/00162: Enable quasi cash support for CUP cards - start -- jzg */
        case SALE:
            if(strIIT.fQuasiCash == TRUE && ((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x00\x03",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x00\x03",5) == 0)))//for VISA only
                ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x11");
            break;
            /* BDOCLG-00161/00162: Enable quasi cash support for CUP cards - end -- jzg */
        case REFUND:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x20");
            break;
        case PRE_AUTH:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x00");
            break;
        case CASH_ADVANCE:
            ushCTOS_EMV_NewTxnDataSet(TAG_9C_TRANS_TYPE,1,"\x01");
            break;
    }
}

//gcitra


//sidumili: pad string
void vdCTOS_Pad_String(char* str,int padlen,char padval,int padtype) {
    int padno;

    if ((padno = padlen - strlen(str)) > 0) {
        if (padtype == POSITION_LEFT)
            memmove(str + padno, str, strlen(str) + 1);
        else
            str += strlen(str);
        memset(str, padval, padno);
        if (padtype == POSITION_RIGHT)
            *(str + padno) = '\0';
    } else if (padno < 0) {
        // Truncate string if too long!!
        memmove(str, str + abs(padno), padlen + 1);
    }
}
//sidumili: pad string

/*********************************************************/
/*sidumili: Issue#000136 [code by albert]*/
/*********************************************************/


BYTE InputAmount2(USHORT usX, USHORT usY, BYTE *szCurSymbol, BYTE exponent, BYTE first_key, BYTE *baAmount, ULONG *ulAmount, USHORT usTimeOutMS, BYTE bIgnoreEnter)
{
    char szTemp[24+1];
    USHORT inRet;
    int inCtr=0;
    char szAmount[24+1];
    char chAmount=0x00;
    char szDisplay[24+1];
    unsigned char c;
    BOOL isKey;;
    memset(szAmount, 0x00, sizeof(szAmount));

    // patrick fix code 20141211
    if (first_key != 0x00)
    {
        szAmount[0] = first_key;
        inCtr = 1;
    }

    while(1)
    {
        memset(szTemp, 0x00, sizeof(szTemp));
        if(strlen(szAmount) > 0)
        {
            //vdCTOS_FormatAmount(strCST.szAmountFormat, szAmount, szTemp);
            vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szAmount, szTemp);
        }
        else
        {
            //vdCTOS_FormatAmount(strCST.szAmountFormat, "0", szTemp);
            if(srTransRec.fDCC && strTCT.fFormatDCCAmount == TRUE)
                vdDCCModifyAmount("000000000000",&szTemp); //vdDCCModifyAmount(&szAmtBuff);
            else
                vdCTOS_FormatAmount(strCST.szAmountFormat, "0", szTemp);
        }

        clearLine(usY);
        CTOS_LCDTPrintXY(usX, usY, szCurSymbol);
        int x=0;
        int len, index;
        len=strlen(szTemp);
        for(index=0; index < len; index++)
        {
            if(szTemp[index] == '.')
                x+=1;
            else
                x+=2;
        }

        CTOS_LCDTPrintXY(36-x, usY, szTemp);
        CTOS_TimeOutSet(TIMER_ID_3,usTimeOutMS);
        while(1)//loop for time out
        {
            CTOS_KBDInKey(&isKey);
            if (isKey)
            { //If isKey is TRUE, represent key be pressed //
                vduiLightOn();
                //Get a key from keyboard //
                CTOS_KBDGet(&c);
                inRet=c;
                break;
            }
            else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
            {
                return 0xFF;
            }
        }

        if(inRet >= 48 && inRet <= 57)
        {
            if(inCtr < 10)
            {
                memset(szTemp, 0x00, sizeof(szTemp));
                sprintf(szTemp, "%c", inRet);
                strcat(szAmount, szTemp);
                inCtr++;

                if(inCtr == 1 && szAmount[0] == 48)
                {
                    memset(szAmount, 0x00, sizeof(szAmount));
                    inCtr=0;
                }
            }
        }
        else if(inRet == 67) /*cancel key*/
        {
            return d_USER_CANCEL;
        }
        else if(inRet == 65) /*entery key*/
        {
            if(strlen(szAmount) > 0)
            {
                memcpy(baAmount, szAmount, strlen(szAmount));
                return d_OK;
            }
            else
            {
                if(bIgnoreEnter == 1)
                    return d_OK;
            }
        }
        else if(inRet == 82) /*clear key*/
        {
            if(inCtr > 0)
                inCtr--;
            szAmount[inCtr]=0x00;
        }
    }
}


//copied from LibInput ctosapi.c
USHORT shCTOS_InputGetNum(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS)
{

    BYTE    bDisplayStr[MAX_CHAR_PER_LINE+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[128];
    USHORT  usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof(bInputStrData));

    if(usTimeOutMS > 0)
        CTOS_TimeOutSet (TIMER_ID_1 , usTimeOutMS);

    //vdInputDebug_LogPrintf("start [%d] data[%s]", strlen(baBuf), baBuf);
    if(strlen(baBuf) > 0 )
    {
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bDisplayStr, 0x20, usMaxLen*2);
        usInputStrLen = strlen(baBuf);
        strcpy(bInputStrData, baBuf);
        if(0x01 == usLeftRight)
        {
            strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
            CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
        }
        else
        {
            memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
            CTOS_LCDTPrintXY(1, usY, bDisplayStr);
        }
    }

    while(1)
    {
//        vduiLightOn(); // patrick remark for flash light always
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            //return d_KBD_CANCEL ;
            return 0xFF;
        }

        CTOS_KBDHit(&bKey);

        switch(bKey)
        {
            case d_KBD_DOT:
                break;
            case d_KBD_CLEAR:
                if (usInputStrLen)
                {
                    usInputStrLen--;
                    bInputStrData[usInputStrLen] = 0x00;

                    memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                    memset(bDisplayStr, 0x20, usMaxLen*2);
                    if(0x01 == usLeftRight)
                    {
                        strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                    }
                    else
                    {
                        memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                        CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                    }

                    //if(1 == fGetCardNO && usInputStrLen == 0)
                    //{
                    //	*usStrLen = 0;
                    //	baBuf[0] = 0x00;
                    //	return d_KBD_CANCEL ;
                    //}
                }
                break;
            case d_KBD_CANCEL:
                *usStrLen = 0;
                baBuf[0] = 0x00;
                return d_KBD_CANCEL ;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                if (usInputStrLen < usMaxLen )
                {
                    bInputStrData[usInputStrLen++] = bKey;

                    memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                    memset(bDisplayStr, 0x20, usMaxLen*2);
                    if(0x01 == usLeftRight)
                    {
                        strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                        CTOS_LCDTPrintXY(MAX_CHAR_PER_LINE - usMaxLen*2, usY, bDisplayStr);
                    }
                    else
                    {
                        memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                        CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                    }
                }
                break;
            case d_KBD_ENTER:
                if(usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen)
                {
                    *usStrLen = usInputStrLen;
                    strcpy(baBuf, bInputStrData);
                    return *usStrLen;
                }
                else if(usByPassAllow && 0 == usInputStrLen)
                {
                    *usStrLen = usInputStrLen;
                    baBuf[0] = 0x00;
                    return *usStrLen;
                }
                break;
            default :
                break;
        }
    }

    return 0;
}

/* BDO: Last 4 PAN digit checking - start -- jzg */
int inBDOEnterLast4Digits(BOOL fInstallment)
{
    int inRet = 0,
            inPANLen = 0;
    BYTE strOut[5] = {0};
    short shMaxLen = 4,
            shMinLen = 4;
    USHORT usLen;
    BOOL fDisplayTitle=FALSE;

    int inRetryCount;
    int inMaxRetrycount = 3;
    /* BDOCLG: Last 4 digit checking not applicable for debit cards - start -- jzg */
    if(strCDT.inType == DEBIT_CARD)
        return d_OK;
    /* BDOCLG: Last 4 digit checking not applicable for debit cards - end -- jzg */

    if(strIIT.fLast4Digits != TRUE)
        return d_OK;

    if((srTransRec.byEntryMode != CARD_ENTRY_MSR) &&
       (srTransRec.byEntryMode != CARD_ENTRY_FALLBACK))
        return d_OK;

    //displayAppbmpDataEx(75, 45, strIIT.szIssuerLogo);
    inPANLen = (strlen(srTransRec.szPAN) - 4);
    inRetryCount = 0;

    while(1)
    {
        if(fDisplayTitle == FALSE)
        {
            CTOS_LCDTClearDisplay();
            vduiLightOn();
            if (fInstallment == TRUE)
                vdDispTitleString("INSTALLMENT");
            else
                vdDispTransTitle(srTransRec.byTransType);
            fDisplayTitle=TRUE;
        }

        vduiClearBelow(8);
        //CTOS_LCDTPrintXY(1, 6, "PLEASE ENTER PAN");
        //CTOS_LCDTPrintXY(1, 7, "LAST 4 DIGITS:");
        CTOS_LCDTPrintXY(1, 4, "PAN LAST FOUR DIGITS:");
        memset(strOut,0x00, sizeof(strOut));
        //inRet = InputString(1, 8, 0x00, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
        //inRet = shCTOS_InputGetNum(8, 0x01,  strOut, &usLen, shMinLen, shMaxLen, 0, d_INPUT_TIMEOUT);
        //inRet = shCTOS_InputLAST4Digit(5, 0x01,  strOut, &usLen, shMinLen, shMaxLen, 0, d_INPUT_TIMEOUT);
        inRet = shCTOS_InputLAST4Digit(5, 0x01,  strOut, &usLen, shMinLen, shMaxLen, 0, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
        inRetryCount++;
        if (inRet == d_KBD_CANCEL )
        {
            vdSetErrorMessage("USER CANCEL");
            return d_NO;
        }
        else if(inRet == 0)
            return d_NO;
        else if(inRet == 0xFF)
        {
            //vdSetErrorMessage("TIME OUT");
            fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            return d_NO;
        }
        else if(inRet >= 1)
        {
            if(memcmp(strOut, srTransRec.szPAN+inPANLen, 4) == 0)
                break;
            else{
                vduiClearBelow(4);
                vdDisplayErrorMsgResp2(" ","INCORRECT LAST 4", "DIGITS");
                if (inRetryCount >= inMaxRetrycount)
                    return d_NO;
                fDisplayTitle=FALSE;
            }
        }
    }

    return d_OK;
}
/* BDO: Last 4 PAN digit checking - end -- jzg */



int inCTOS_TEMPCheckAndSelectMutipleMID(void)
{
#define ITEMS_PER_PAGE          4

    char szMMT[50];
    char szDisplay[50];
    int inNumOfRecords = 0;
    short shCount = 0;
    short shTotalPageNum;
    short shCurrentPageNum;
    short shLastPageItems = 0;
    short shPageItems = 0;
    short shLoop;
    short shFalshMenu = 1;
    BYTE isUP = FALSE, isDOWN = FALSE;


    BYTE szTitle[20+1];
    BYTE szDisMsg[100];

    unsigned char key;
    unsigned char bstatus = 0;

//1217
    short shMinLen = 4;
    short shMaxLen = 6;
    BYTE Bret;
    unsigned char szOutput[30];
//1217

    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    char szHeaderString[50] = "SELECT MERCHANT";
    char szMitMenu[1024];
    int inLoop = 0;


    if(inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    inNMTReadNumofRecords(&inNumOfRecords);
    CTOS_KBDBufFlush();//cleare key buffer

    if(inNumOfRecords > 1)
    {

        //issue-00378 - remove trailing key entry for amount if multi merchant setup
        //SIT
        //vdSetFirstIdleKey(0x00);

        //issue-00436: clear the buffer first to avoid garbage display
        memset(szMitMenu, 0x00, sizeof(szMitMenu));

        for (inLoop = 0; inLoop < inNumOfRecords; inLoop++)
        {
            strcat((char *)szMitMenu, strNMT[inLoop].szMerchName);
            if(inLoop + 1 != inNumOfRecords)
                strcat((char *)szMitMenu, (char *)" \n");
        }

        if((strTCT.byTerminalType % 2) == 0)
        {
            CTOS_LCDForeGndColor(RGB(13, 43, 112));
            CTOS_LCDBackGndColor(RGB(255, 255, 255));
        }

        key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);

        inSetTextMode();

        if(key > 0)
        {
            if(key == 0xFF)
            {
                //CTOS_LCDTClearDisplay();
                //setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
                //vduiWarningSound();
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "TIME OUT");
                //usCTOSS_LCDDisplay(szDisMsg);
                inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                CTOS_Beep();
                CTOS_Delay(1500);
                CTOS_Beep();
                return d_NO;
            }
            else if(d_KBD_CANCEL == key)
                return -1;
            else
            {
                vdDebug_LogPrintf("key[%d]-------", key);
                memcpy(&strNMT[0],&strNMT[key-1],sizeof(STRUCT_MMT));
            }
        }
        else
            return -1;

    }


    if (strNMT[0].fMerchEnablePassword == 1)
    {
        CTOS_LCDTClearDisplay();

        vdDispTitleString("MULTI MERCHANT");
        CTOS_LCDTPrintXY(1, 3,strNMT[0].szMerchName);
        CTOS_LCDTPrintXY(1, 4,"ENTER PASSWORD:");

        while (1)
        {
            memset(szOutput,0x00,sizeof(szOutput));
            shMinLen = strlen(strNMT[0].szMercPassword);

            //Bret = InputString(1, 5, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
            Bret = InputString(1, 5, 0x01, 0x02,szOutput, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/
            if (Bret == d_KBD_CANCEL)
                return -1;

            if(Bret == 0xFF){
                fTimeOutFlag = TRUE;
                return -1;
            }

            if(strcmp(szOutput,strNMT[0].szMercPassword) == 0)
                break;
            else
            {
                vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
                vduiClearBelow(5);
            }
        }
    }

    srTransRec.MITid = strNMT[0].NMTID;

    return d_OK;
}

/* BDO: PAN Format requirements -- sidumili */
void vdFormatPANForECR(char* szInPAN, char* szOutPAN){
    int inLen1, inLen2;
    char szTemp1[6 + 1] = {0};
    char szTemp2[4 + 1] = {0};
    char szTemp3[10 + 1] = {0};

	vdDebug_LogPrintf("--vdFormatPANForECR--");

    inLen1 = strlen(szInPAN);

    memset(szTemp1, 0x00, sizeof(szTemp1));
    memset(szTemp2, 0x00, sizeof(szTemp2));
    memset(szTemp3, 0x00, sizeof(szTemp3));

    memcpy(szTemp1, &szInPAN[0], 6);
    memcpy(szTemp2, &szInPAN[inLen1 - 4], 4);

    inLen2 = inLen1 - 10;

    vdCTOS_Pad_String(szTemp3, inLen2, '0', POSITION_LEFT);

    sprintf(szOutPAN, "%s%s%s", szTemp1, szTemp3, szTemp2);

    vdDebug_LogPrintf("szTemp1[%s]|szTemp2[%s]|szTemp3[%s]", szTemp1, szTemp2, szTemp3);
    vdDebug_LogPrintf("inLen1[%d]|inLen2[%d]", inLen1, inLen2);
    vdDebug_LogPrintf("szOutPAN[%s]", szOutPAN);

}
/* BDO: PAN Format requirements -- sidumili */

//aaronnino for BDOCLG ver 9.0 fix on issue#00146 Debit is accepting on CASH ADVANCE start 2 of 3
int inCTOS_CardNotSuppoted(int inHDTCardType)
{

    if (inHDTCardType == srTransRec.HDTid)
    {
        vdSetErrorMessage("CARD NOT SUPPORTED");
        return ST_ERROR;
    }
    else
        return ST_SUCCESS;

}
//aaronnino for BDOCLG ver 9.0 fix on issue#00146 Debit is accepting on CASH ADVANCE end 2 of 3

/*BDO: Flag for selection of transaction via ecr -- sidumili*/
int inCTOS_SelectECRTranType(void)
{
    BYTE key = 0;
    int inRet = 0;
    char szDebug[41] = {0},
            szAscBuf[5] = {0},
            szBcdBuf[3] = {0};
    int inMenuNumRecs = 0,
            inCtr = 0,
    //inKey = 0,
            inHeaderAttr = 0x01+0x04;

    char szMenuItems[9000] = {0},
            szHeader[30] = {0};
    BYTE inKey = 0;
    int inTrxnType;

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    srTransRec.inECRTxnType = 0;

    fECRBuildSendOK = FALSE;
    fTimeOutFlag = FALSE; /*BDO: Flag for timeout --sidumili*/

    vdSelectECRMenu(szMenuItems); /*ECR menu selection --sidumili*/

    do
    {
#if 0
        setLCDPrint27(4,DISPLAY_POSITION_LEFT, "SELECT TRANSACTION:");
		CTOS_LCDTPrintXY(1, 5, "1. BINVER");
		CTOS_LCDTPrintXY(1, 6, "2. SALE");

		key=WaitKey(60);
#endif

        strcpy(szHeader, "SELECT TRANSACTION");
#if 0
        memset(szMenuItems, 0, sizeof(szMenuItems));
		strcat(szMenuItems, "BINVER");
		strcat(szMenuItems, "\n");
		strcat(szMenuItems, "SALE");
		strcat(szMenuItems, "\n");
		strcat(szMenuItems, "INSTALLMENT");
		strcat(szMenuItems, "\n");
#endif
        inCtr = strlen(szMenuItems) - 1;
        szMenuItems[inCtr] = 0x00;
        inSetColorMenuMode();
        //android-removed
        //inKey = MenuDisplayEx(szHeader, strlen(szHeader), inHeaderAttr, 1, 1, szMenuItems, TRUE, inGetIdleTimeOut(TRUE)); /*Menu with timeout parameter*/
        vdDebug_LogPrintf("::inCTOS_SelectECRTranType::MenuDisplayEx::inKey[%d]", inKey);
        inSetTextMode();

        if (inKey == d_KBD_CANCEL)
        {
            inRet = d_NO;
            break;
        }


        inTrxnType = inReturnTrxnType(inKey);
        if (inKey > 0){
            srTransRec.inECRTxnType = inTrxnType;
            break;
        }
        else if (inKey == 0xFF) /*TimeOut -- sidumili*/
        {
            fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            inRet = d_NO;
            vdDisplayErrorMsg(1, 8, "TIMEOUT");;
            break;
        }
//		else if (inKey == d_KBD_CANCEL)
//		{
//			inRet = d_NO;
//			break;
//		}
        else
            vduiWarningSound();

#if 0
        if(inKey == 1)
		{
			//CTOS_LCDTClearDisplay();
			//vdDispTransTitle(BIN_VER);
			//CTOS_LCDTPrintXY(5, 4, "BINVER SELECTED");
			//setLCDPrint27(5, DISPLAY_POSITION_CENTER, "BINVER SELECTED");
			srTransRec.inECRTxnType = ECR_BINVER;
			inRet = d_OK;
			//CTOS_Delay(1000);
			break;
		}
		else if(inKey == 2)
		{
			//CTOS_LCDTClearDisplay();
			//vdDispTransTitle(SALE);
			//CTOS_LCDTPrintXY(5, 4, "SALE SELECTED");
			//setLCDPrint27(5, DISPLAY_POSITION_CENTER, "SALE SELECTED");
			srTransRec.inECRTxnType = ECR_SALE;
			inRet = d_OK;
			//CTOS_Delay(1000);
			break;
		}
		else if(inKey == 3)
		{
			//CTOS_LCDTClearDisplay();
			//vdDispTransTitle(SALE);
			//CTOS_LCDTPrintXY(5, 4, "SALE SELECTED");
			//setLCDPrint27(5, DISPLAY_POSITION_CENTER, "SALE SELECTED");
			srTransRec.inECRTxnType = ECR_INSTALLMENT;
			inRet = d_OK;
			//CTOS_Delay(1000);
			break;
		}

		else if (inKey == 0xFF) /*TimeOut -- sidumili*/
		{
			fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
			inRet = d_NO;
			vdDisplayErrorMsg(1, 8, "TIMEOUT");;
			break;
		}
		else if (inKey == d_KBD_CANCEL)
		{
			inRet = d_NO;
			break;
		}
		else
			vduiWarningSound();
#endif

    }
    while(1);

    vdDebug_LogPrintf("inCTOS_SelectECRTranType::inRet[%d]|inECRTxnType[%d]", inRet, srTransRec.inECRTxnType);

    if (inRet == d_NO){


        //vdDebug_LogPrintf("TEST ECR send");

        strcpy(srTransRec.szRespCode,"");
        strcpy(srTransRec.szECRRespCode, ECR_OPER_CANCEL_RESP);


        /* Send response to ECR -- sidumili */
        fECRBuildSendOK = FALSE;
        if (fGetECRTransactionFlg()){
            inMultiAP_ECRSendSuccessResponse();
        }
        fECRBuildSendOK = FALSE;
        /* Send response to ECR -- sidumili */

        return d_NO;
    }

    return inRet;
}
/*BDO: Flag for selection of transaction via ecr -- sidumili*/

int inCTOS_MerchantCheckBatchAllOperation(void)
{
    int inRet = d_NO;
    int inNumOfHost = 0,inNum;
    char szBcd[INVOICE_BCD_SIZE+1];
    char szErrMsg[30+1];
    char szAPName[25];
    int inAPPID;

    CTOS_LCDTClearDisplay();

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        inCTOS_MerchantTagMustSettle();
        inCTOS_MultiAPALLAppEventID(d_IPC_CMD_MERCHANT_CHECK_BATCH_ALL);
    }
    else
        inCTOS_MerchantTagMustSettle();

    CTOS_LCDTClearDisplay();
    return ST_SUCCESS;
}

void inCTOS_MerchantTagMustSettle(void)
{
    unsigned char chkey;
    short shHostIndex;
    int inResult,i,inCount,inRet;
    //int inTranCardType;
    //int inReportType;
    int inBatchNumOfRecord;
    //int *pinTransDataid;

    STRUCT_FILE_SETTING strFile;
    ACCUM_REC srAccumRec;

    //BYTE baTemp[PAPER_X_SIZE * 64];
    //char szStr[d_LINE_SIZE + 1];
    //int inRet = d_NO;
    int inNumOfHost = 0, inNum = 0;
    char szBcd[INVOICE_BCD_SIZE+1] = {0};
    char szErrMsg[31] = {0};
    char szAPName[25] = {0};
    int inAPPID = 0;
    int inMerchantCount=0;
    int inBatchRecordNum = 0;
    int  *pinTransDataid = NULL;

    vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);


    int inMMTTotalNumofMerchants=0;

    CTOS_RTC SetRTC;
    BYTE szCurrDate[8] = {0};


    memset(szCurrDate, 0x00, sizeof(szCurrDate));
    CTOS_RTCGet(&SetRTC);
    sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
    //inMMTTotalNumofMerchants=inMMTNumAlbertRecord();

    //pinTransDataid = (int*)malloc(inMMTTotalNumofMerchants * sizeof(int));

    //inBatchReviewByMerchandHostAlbert(pinTransDataid);

    memset(szAPName,0x00,sizeof(szAPName));
    inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

    inNumOfHost = inHDTNumRecord();
    vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);

    vduiDisplayStringCenter(3 ,"CHECKING MERCHANT");
    vduiDisplayStringCenter(4 ,"SETTLE STATUS");
    vduiDisplayStringCenter(5 ,"PLEASE WAIT...");


    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        if(inHDTRead(inNum) == d_OK)
        {
            if(strHDT.fHostEnable != TRUE)
                continue;

            vdDebug_LogPrintf("szAPName=[%s]-[%s]----",szAPName,strHDT.szAPName);





            if (strcmp(szAPName, strHDT.szAPName)!=0)
                continue;
            else
            {
                srTransRec.HDTid=strHDT.inHostIndex;
                inMMTTotalNumofMerchants=inMMTNumAlbertRecord(srTransRec.HDTid);
                pinTransDataid = (int*)malloc(inMMTTotalNumofMerchants * sizeof(int));
                inBatchReviewByMerchandHostAlbert(pinTransDataid,srTransRec.HDTid);
                vdDebug_LogPrintf("inMMTTotalNumofMerchants=[%d]----",inMMTTotalNumofMerchants);

                for (inMerchantCount = 1; inMerchantCount<=inMMTTotalNumofMerchants; inMerchantCount++)
                {
                    inMMTReadRecord(srTransRec.HDTid, pinTransDataid[inMerchantCount]);
                    if(strMMT[0].fMustSettFlag == TRUE)
                        continue;

                    vdDebug_LogPrintf("srTransRec.HDTid %d", srTransRec.HDTid);

                    vdDebug_LogPrintf("current date %s", szCurrDate);

                    vdDebug_LogPrintf("settle date %s", strMMT[0].szSettleDate);

                    if ((strMMT[0].fMMTEnable == TRUE) &&
                        (wub_str_2_long(szCurrDate) >= wub_str_2_long(strMMT[0].szSettleDate)))
                    {
                        srTransRec.MITid=strMMT[0].MITid;
                        vdCTOS_GetAccumName(&strFile, &srAccumRec);
                        if ((inResult = inMyFile_CheckFileExist(strFile.szFileName)) > 0)
                        {
                            strMMT[0].fMustSettFlag = 1;
                            inMMTSave(strMMT[0].MMTid);
                        }
                    }
                }

                free(pinTransDataid);
            }
        }
    }


}

USHORT shCTOS_InputLAST4Digit(IN  USHORT usY, IN  USHORT usLeftRight, OUT BYTE *baBuf, OUT  USHORT *usStrLen, USHORT usMinLen, USHORT usMaxLen, USHORT usByPassAllow, USHORT usTimeOutMS)
{

    BYTE    bDisplayStr[20+1];
    BYTE    bKey = 0x00;
    BYTE    bInputStrData[128];
    USHORT  usInputStrLen;

    usInputStrLen = 0;
    memset(bInputStrData, 0x00, sizeof(bInputStrData));

    if(usTimeOutMS > 0)
        CTOS_TimeOutSet (TIMER_ID_1 , usTimeOutMS);

    //vdInputDebug_LogPrintf("start [%d] data[%s]", strlen(baBuf), baBuf);
    if(strlen(baBuf) > 0 )
    {
        memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
        memset(bDisplayStr, 0x20, usMaxLen*2);
        usInputStrLen = strlen(baBuf);
        strcpy(bInputStrData, baBuf);
        if(0x01 == usLeftRight)
        {
            strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
            CTOS_LCDTPrintXY(20 - usMaxLen*2, usY, bDisplayStr);
        }
        else
        {
            memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
            CTOS_LCDTPrintXY(1, usY, bDisplayStr);
        }
    }

    while(1)
    {
//        vduiLightOn(); // patrick remark for flash light always
        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
            *usStrLen = 0;
            baBuf[0] = 0x00;
            //return d_KBD_CANCEL ;
            return 0xFF;
        }

        CTOS_KBDHit(&bKey);

        switch(bKey)
        {
            case d_KBD_DOT:
                break;
            case d_KBD_CLEAR:
                if (usInputStrLen)
                {
                    usInputStrLen--;
                    bInputStrData[usInputStrLen] = 0x00;

                    memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                    memset(bDisplayStr, 0x20, usMaxLen*2);
                    if(0x01 == usLeftRight)
                    {
                        clearLine(usY);
                        strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                        CTOS_LCDTPrintXY(23 - usMaxLen*2, usY, bDisplayStr);
                    }
                    else
                    {
                        memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                        CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                    }

                    if ((strTCT.byTerminalType%2) == 0)
                        clearLine(14);
                    else
                        clearLine(8);

                    //if(1 == fGetCardNO && usInputStrLen == 0)
                    //{
                    //	*usStrLen = 0;
                    //	baBuf[0] = 0x00;
                    //	return d_KBD_CANCEL ;
                    //}
                }
                break;
            case d_KBD_CANCEL:
                *usStrLen = 0;
                baBuf[0] = 0x00;
                return d_KBD_CANCEL ;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                if (usInputStrLen < usMaxLen )
                {
                    bInputStrData[usInputStrLen++] = bKey;

                    memset(bDisplayStr, 0x00, sizeof(bDisplayStr));
                    memset(bDisplayStr, 0x20, usMaxLen*2);
                    if(0x01 == usLeftRight)
                    {
                        clearLine(usY);
                        strcpy(&bDisplayStr[(usMaxLen-strlen(bInputStrData))*2], bInputStrData);
                        CTOS_LCDTPrintXY(23 - usMaxLen*2, usY, bDisplayStr);
                    }
                    else
                    {
                        memcpy(bDisplayStr, bInputStrData, strlen(bInputStrData));
                        CTOS_LCDTPrintXY(1, usY, bDisplayStr);
                    }
                }

                if (usInputStrLen == 4){
                    vdCTOS_DispStatusMessage("PRESS OK TO CONFIRM");
                    //    *usStrLen = usInputStrLen;
                    //    strcpy(baBuf, bInputStrData);
                    //    return *usStrLen;
                }else{
                    if ((strTCT.byTerminalType%2) == 0)
                        clearLine(14);
                    else
                        clearLine(8);
                }

                break;
            case d_KBD_ENTER:
                if(usInputStrLen >= usMinLen && usInputStrLen <= usMaxLen)
                {
                    *usStrLen = usInputStrLen;
                    strcpy(baBuf, bInputStrData);
                    return *usStrLen;
                }
                else if(usByPassAllow && 0 == usInputStrLen)
                {
                    *usStrLen = usInputStrLen;
                    baBuf[0] = 0x00;
                    return *usStrLen;
                }
                break;
            default :
                break;
        }
    }

    return 0;
}

int szSetupMenuFunction(void){

	int inRet = d_NO;
	
    memcpy(szFuncTitleName,"SETUP",5);//aaronnino for BDOCLG ver 9.0 fix on issue #00114 Incorrect user password for manual settlement, if settlements fails for online transaction 4 of 9
    vdCTOS_SetTransType(SETUP);

	inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
		return inRet;
	
	// uneeded, code cleanup for android -- sidumili
	#if 0
    inCTOS_PromptPassword();

    inTCTRead(1);
    if((strTCT.byTerminalType % 2) == 0)
    {
        CTOS_LCDForeGndColor(RGB(13, 43, 112));
        CTOS_LCDBackGndColor(RGB(255, 255, 255));
    }
	#endif

    vdDisplaySetup();
    return SUCCESS;

}

int inCTOS_Settle_Selection(void){

    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, Menukey=0;
    int inRet;
    int key;
	BYTE szSettleMsg[128];
    USHORT shRet = 0,
            shLen = 0,
            iInitX = 0,
            shMinLen = 4,
            shMaxLen = 6;
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE + 1] = {0},
            szTitle[MAX_CHAR_PER_LINE + 1] = {0};
	
	BYTE szMessage[2000 + 1] = {0};

    //aaronnino for bdoclg ver 9.0 fix on issue #00694 Incorrect order of prompt � Settle All Card Types first before merchant selection then password start
    //CTOS_LCDTClearDisplay();

    vdCTOS_SetTransType(SETTLE);
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;

    //CTOS_LCDTClearDisplay();

    inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;
    //aaronnino for bdoclg ver 9.0 fix on issue #00694 Incorrect order of prompt � Settle All Card Types first before merchant selection then password end

    //CTOS_LCDTClearDisplay();

    memset(szTitle, 0x00, sizeof(szTitle));
    strcpy(szTitle, "SETTLEMENT");
    //iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2;
    //memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    //memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    //memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle));
    //CTOS_LCDTSetReverse(TRUE);
    //CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    //CTOS_LCDTSetReverse(FALSE);

    //setLCDPrint(3, DISPLAY_POSITION_LEFT, "SETTLE ALL");
    //setLCDPrint(4, DISPLAY_POSITION_LEFT,"CARD TYPES?");
    //setLCDPrint(6, DISPLAY_POSITION_LEFT, "1.YES");
    //setLCDPrint(7, DISPLAY_POSITION_LEFT,"2.NO");
    //memset(szSettleMsg, 0x00, sizeof(szSettleMsg));
	//strcpy(szSettleMsg, szTitle);
	//strcat(szSettleMsg, "|");
	//strcat(szSettleMsg, "SETTLE ALL CARD TYPES?");
    //key = usCTOSS_ConfirmYesNo(szSettleMsg);
	//key = inDisplayMessageBoxWithButtonConfirmation(1,8,szTitle,"SETTLE ALL CARD TYPES?","", MSG_TYPE_QUESTION, BUTTON_TYPE_NO_YES);
	//vdDebug_LogPrintf("inDisplayMessageBoxWithButtonConfirmation, key=[%d]", key);

	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    strcpy(szChoiceMsg,"No|");
	strcat(szChoiceMsg,"Yes");
	vdDebug_LogPrintf("szChoiceMsg=[%s]", szChoiceMsg);

	memset(szMessage, 0x00, sizeof(szMessage));
	sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_VIEW, MENU_2_COL_COUNT, "Settlement", "Settle all card types?");
	strcat(szMessage, szChoiceMsg);

	key = inMenuSelection(szMessage);	
    vdDebug_LogPrintf("inMenuSelection=[%d]", key);

	if (key == 0xFF || key == d_TIMEOUT) //TimeOut
    {
		//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return -1;
    }
    else if (key == d_KBD_CANCEL || key == d_USER_CANCEL)
    {
		//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return -1;
    }

	key = inGetMenuKeySelected(szChoiceMsg, strGBLVar.szGBLvProcessDesc);
    vdDebug_LogPrintf("inGetMenuKeySelected=[%d]", key);

	if (key > 0)
	{
		switch (key)
		{
			case 1: //No
				inCTOS_SETTLEMENT();
				break;
			case 2: // Yes
				inTCTRead(1);
	            strTCT.fReprintSettleStatus=0;
	            inTCTSave(1);
	            inCTOS_SETTLE_ALL();
				break;
		}
	}
	else
	{
		return d_NO;
	}
	
    return SUCCESS;


}

/*BDO: Get Idle TimeOut -- sidumili*/
int inGetIdleTimeOut(BOOL fSecond){
    int inTimeOut;

    inTimeOut = strTCT.inIdleTimeOut;

    if (inTimeOut <= 0) inTimeOut = 30;

// MS
    if (fSecond == FALSE)
        inTimeOut = (inTimeOut * 100);

    vdDebug_LogPrintf("::inGetIdleTimeOut inTimeOut[%d]", inTimeOut);

    return(inTimeOut);

}


#if 0
int inCheckSubAPPHost(void){


	if (inCheckHostEnable_Per_APPLICATION("V5S_BDODEBIT") <= 0)
	  	fIncludeDebit = FALSE;
	else
	  	fIncludeDebit = TRUE;

	if (inCheckHostEnable_Per_APPLICATION("V5S_BDOINST") <= 0)
	  	fIncludeInst = FALSE;
	else
	 	fIncludeInst = TRUE;

	if (inCheckHostEnable_Per_APPLICATION("V5S_BDOINST") <= 0)
		fIncludeCUP = FALSE;
	else
		fIncludeCUP = TRUE;

	return d_OK;

}
#endif



int inAnalyzeBinResponseCode(TRANS_DATA_TABLE *srTransPara){

	int inRespcode;
	int inResult = TRANS_BINROUTE_REJECTED;
	int inIssuerNum;
	
	vdDebug_LogPrintf("--inAnalyzeBinResponseCode--");
	vdDebug_LogPrintf("srTransRec.szRespCode=[%s]", srTransRec.szRespCode);
	vdDebug_LogPrintf("srTransRec.byTransType=[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("srTransRec.byEntryMode=[%d]", srTransRec.byEntryMode);

	inRespcode = atoi(srTransRec.szRespCode);
    memset(srTransRec.szBinRouteRespCode, 0, sizeof(srTransRec.szBinRouteRespCode));
    if((inRespcode == 60) || (inRespcode == 69) || (inRespcode == 70)|| (inRespcode == 71) || (inRespcode == 72) || (inRespcode == 73) || (inRespcode == 74) || (inRespcode == 79))
    {
	    inMyFile_ReversalDelete();
		strcpy(srTransRec.szBinRouteRespCode, srTransRec.szRespCode);
		strcpy(srTransRec.szRespCode_Temp,srTransRec.szRespCode);
    }
	
    switch(inRespcode){
        case 60: /*dual branded cards*/  
			if(srTransRec.byTransType == SALE_OFFLINE || srTransRec.byTransType == PRE_AUTH)
			{
				vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");			   
				inResult=TRANS_BINROUTE_REJECTED;
				break;
			}		   
#if 0//Removed. If card used is CTLS, application will prompt user with "PLEASE INSERT/SWIPE CARD"
            if (srTransRec.byEntryMode == CARD_ENTRY_WAVE){
                strCDT.HDTid = 1;
                fDualBrandedCard = TRUE;       
            }
    		else
            {	
#endif
                if (inCheckIfHostEnable(53) == 1)
				{	
					if(srTransRec.byTransType == CASH_ADVANCE)
	                {
	                    strCDT.HDTid = 1;
                    	fDualBrandedCard = TRUE;
	                    
	                }
					else
					{
						if(inFLGGet("fDebitDualCurrency") == FALSE  && strCST.inCurrencyIndex != CURRENCY_PHP)
						{
						
								vdDisplayErrorMsgResp2("DEBIT", "NOT ALLOWED", "FOR USD");
								inResult=TRANS_BINROUTE_REJECTED;
							 	break;
							
						}

						if(srTransRec.byEntryMode == CARD_ENTRY_MANUAL)
						{
		                     vdDisplayErrorMsgResp2(" ", "MANUAL ENTRY", "NOT ALLOWED"); 			
		                     inResult=TRANS_BINROUTE_REJECTED;
		                     break;
		                }
						else
						{
		                    strCDT.HDTid = 53;
		                    srTransRec.inCardType = DEBIT_CARD;
		                    srTransRec.IITid = 1;
						}
					}
                }
                else
                {
                    strCDT.HDTid = 1;
                    fDualBrandedCard = TRUE;
					srTransRec.fDualBrandedCredit = TRUE;
                }
//            }
            
            //strcpy(srTransRec.szRespCode,"00");
            inResult = TRANS_AUTHORIZED;				 	
        break;
		
		case 69: /*magstripe debit*/
                 if(srTransRec.byTransType == SALE_OFFLINE || srTransRec.byTransType == PRE_AUTH 
				 || srTransRec.byTransType == CASH_ADVANCE)
                 {
                     vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");    			
                     inResult=TRANS_BINROUTE_REJECTED;
                     break;
                 }

				 if(srTransRec.byEntryMode == CARD_ENTRY_MANUAL)
				 {
                     vdDisplayErrorMsgResp2(" ", "MANUAL ENTRY", "NOT ALLOWED"); 			
                     inResult=TRANS_BINROUTE_REJECTED;
                     break;
                 }

				 if(inFLGGet("fDebitDualCurrency") == FALSE)
				 {
					if(strCST.inCurrencyIndex != CURRENCY_PHP)
					{
						vdDisplayErrorMsgResp2("DEBIT", "NOT ALLOWED", "FOR USD");
						inResult=TRANS_BINROUTE_REJECTED;
                     	break;
					}
				 }
				 
			     strCDT.HDTid = 53;
				 //strcpy(srTransRec.szRespCode,"00");
				 inResult = TRANS_AUTHORIZED;
				 srTransRec.inCardType = DEBIT_CARD;
				 srTransRec.IITid = 1;
				 fMagStripeDebit=TRUE;
				 break;
		case 70: /*EMV debit*/
				 if(srTransRec.byTransType == SALE_OFFLINE || srTransRec.byTransType == PRE_AUTH 
                 || srTransRec.byTransType == CASH_ADVANCE)
                 {
                     vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");			   
                     inResult=TRANS_BINROUTE_REJECTED;
                     break;
                 }

				 if(srTransRec.byEntryMode == CARD_ENTRY_MANUAL)
				 {
                     vdDisplayErrorMsgResp2(" ", "MANUAL ENTRY", "NOT ALLOWED"); 			
                     inResult=TRANS_BINROUTE_REJECTED;
                     break;
                 }

				 if(inFLGGet("fDebitDualCurrency") == FALSE)
				 {
					if(strCST.inCurrencyIndex != CURRENCY_PHP)
					{
						vdDisplayErrorMsgResp2("DEBIT", "NOT ALLOWED", "FOR USD");
						inResult=TRANS_BINROUTE_REJECTED;
                     	break;
					}
				 }
				 
				 strCDT.HDTid = 53;	 
				 put_env_int("BANCNETAID",1);
		         //strcpy(srTransRec.szRespCode,"00");
		         inResult = TRANS_AUTHORIZED;
				 srTransRec.inCardType = DEBIT_CARD;
				 srTransRec.IITid = 1;
				 break;
				 
        case 71: /*DCC*/
			//memset(srTransRec.szRespCode_Temp,0x00,RESP_CODE_SIZE+1);
			if(strTCT.fDCC == FALSE)
				inResult=TRANS_BINROUTE_REJECTED;//Fix for issue 1160. Terminal still processes bin routing response 71 even if DCC parameter is disabled.
			else
			{
				vdDebug_LogPrintf("inCTOS_WaveFlowProcess 9F34 = [%02X %02X %02X]", srTransRec.stEMVinfo.T9F34[0], srTransRec.stEMVinfo.T9F34[1], srTransRec.stEMVinfo.T9F34[2]);
				if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
				{
					//strcpy(srTransRec.szRespCode_Temp,srTransRec.szRespCode);
					if((srTransRec.stEMVinfo.T9F34[2] & 0x0F) == 0x02)
						fReEnterOfflinePIN = TRUE;
				}

				//strcpy(srTransRec.szRespCode,"00");
	            inResult = TRANS_AUTHORIZED;	
	            fBinRouteDCC=TRUE;
			}
        break;
		/*Diners*/
		case 72: 
#if 0			
				 if (fUSDSelected)
				 	strCDT.HDTid = 106;
				 else
			     	strCDT.HDTid = 59;
#else
				if(fDinersHostEnable == TRUE)
				{
					if (fUSDSelected)
			        	strHDT.inHostIndex=srTransRec.HDTid=strCDT.HDTid=106;
			        else
			        	strHDT.inHostIndex=srTransRec.HDTid=strCDT.HDTid=59;
				}
				else
				{
					if (fUSDSelected)
			        	strHDT.inHostIndex=srTransRec.HDTid=strCDT.HDTid=3;
			        else
			        	strHDT.inHostIndex=srTransRec.HDTid=strCDT.HDTid=1;
				}	
#endif				
				
				 //strcpy(srTransRec.szRespCode,"00");
		         inResult = TRANS_AUTHORIZED;
				 srTransRec.IITid = 8;
				 break;
		case 73: /*fleet cards*/
                if(srTransRec.byTransType == SALE_OFFLINE || srTransRec.byTransType == PRE_AUTH 
                || srTransRec.byTransType == CASH_ADVANCE)
                {
                    vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");			   
                    inResult=TRANS_BINROUTE_REJECTED;
                    break;
                }	   
                strCDT.HDTid = 56;
                //strcpy(srTransRec.szRespCode,"00");
                inResult = TRANS_AUTHORIZED;
                srTransRec.IITid = 11;
                break;
		case 74: /*cup magstripe*/
                 if (fUSDSelected)
				 	strCDT.HDTid = 37;
				 else
			     	strCDT.HDTid = 36;
				 
                 inHDTReadData(strCDT.HDTid);
                 if(strHDT_Temp.fHostEnable != TRUE)
                 {
                     vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED"); 			
                     inResult=TRANS_BINROUTE_REJECTED;
                     break;
                 }

				 if(srTransRec.byEntryMode == CARD_ENTRY_MANUAL && inFLGGet("fATPCUPMANUAL") == FALSE)
				 {
                     vdDisplayErrorMsgResp2(" ", "MANUAL ENTRY", "NOT ALLOWED"); 			
                     inResult=TRANS_BINROUTE_REJECTED;
                     break;
                 }

				 //strcpy(srTransRec.szRespCode,"00");
		         inResult = TRANS_AUTHORIZED;
				 srTransRec.IITid = 9;
				 fMagStripeCUP = TRUE;
				 break;

		case 79: /*AMEX*/
				if(fAMEXHostEnable == TRUE)
				{
					if (fUSDSelected)
			        	strHDT.inHostIndex=srTransRec.HDTid=strCDT.HDTid=4;
			        else
			        	strHDT.inHostIndex=srTransRec.HDTid=strCDT.HDTid=2;
				}
				else
				{
					if (fUSDSelected)
			        	strHDT.inHostIndex=srTransRec.HDTid=strCDT.HDTid=3;
			        else
			        	strHDT.inHostIndex=srTransRec.HDTid=strCDT.HDTid=1;
				}	
				
				inHDTReadData(strCDT.HDTid);
				if(strHDT_Temp.fHostEnable != TRUE)
				{
					vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED"); 			
					inResult=TRANS_BINROUTE_REJECTED;
					break;
				}

				//strcpy(srTransRec.szRespCode,"00");
				inResult = TRANS_AUTHORIZED;
				srTransRec.IITid = AMEX_ISSUER;
				break;		
	}


   if (inResult == TRANS_AUTHORIZED){
	   	   fRouteToSpecificHost = 1 ;
		   inIITRead(srTransRec.IITid);
		   strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
		   //inMyFile_ReversalDelete();
		   //inCTOS_inDisconnect();
   }else
   	   fRouteToSpecificHost = 0;

	vdDebug_LogPrintf("inAnalyzeBinResponseCode %d %d %d %d", inRespcode, fRouteToSpecificHost, srTransPara->shTransResult, strCDT.HDTid);

	return inResult;

	}





void vdFunctionOne(void)
{
    int inRet=d_NO;
    BYTE key = 0;

    vdDebug_LogPrintf("vdFunctionOne[START]");

    //CTOS_LCDTClearDisplay();
    //vdDispTitleString("CLEAR");

    //setLCDPrint27(3,DISPLAY_POSITION_LEFT, "1.CLEAR BATCH");
    //setLCDPrint27(4,DISPLAY_POSITION_LEFT, "2.CLEAR REVERSAL");

    while(1)
    {
        key=WaitKey(inGetIdleTimeOut(TRUE));

        if(key == d_KBD_1)
        {
            memcpy(szFuncTitleName,"CLEAR BATCH",11);
            if(inFunctionKeyPassword("CLEAR BATCH", SUPER_ENGINEER_PWD)==d_OK)
            {
                memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                inCTOS_ManualSettle();
                break;
            }
        }
        else if(key == d_KBD_2)
        {
            memcpy(szFuncTitleName,"CLEAR REVERSAL",14);

            if(inFunctionKeyPassword("CLEAR REVERSAL", SUPER_ENGINEER_PWD)==d_OK)
            {
                memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                vdCTOS_DeleteReversal();
                break;
            }
        }
        else if(key == d_KBD_CANCEL)
        {
            memset(szFuncTitleName,0,sizeof(szFuncTitleName));
            break;
        }
        else if (key == 0xFF)
        {
            memset(szFuncTitleName,0,sizeof(szFuncTitleName));
            break;
        }

    }
    vdDebug_LogPrintf("vdFunctionOne[END]");

}

int inSetupSelection(void)
{
    char szChoiceMsg[100];
    int bHeaderAttr = 0x01+0x04, key=0;
    char szHeaderString2[30 + 1];
    BYTE szDisMsg[100];

    //CTOS_LCDTClearDisplay();
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    memset(szHeaderString2, 0x00, sizeof(szHeaderString2));
    strcpy(szHeaderString2, "SETUP");

    strcat(szChoiceMsg,"CLEAR BATCH \n");
    strcat(szChoiceMsg,"CLEAR REVERSAL \n");
    strcat(szChoiceMsg, "EDIT TABLE");
    //strcat(szChoiceMsg,"HOST INFO \n");

    key = MenuDisplay(szHeaderString2,strlen(szHeaderString2), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    //key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    //key = MenuDisplay(0,0, bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    if (key == 0xFF)
    {
        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
        //strcpy(szDisMsg, szHeaderString2);
        //strcat(szDisMsg, "|");
        //strcat(szDisMsg, "TIME OUT");
        //usCTOSS_LCDDisplay(szDisMsg);
        //CTOS_Beep();
        //CTOS_Delay(1500);

		//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(1000);
    }
    else if (key == d_KBD_CANCEL)
    {
        memset(szFuncTitleName,0,sizeof(szFuncTitleName));
        //strcpy(szDisMsg, szHeaderString2);
        //strcat(szDisMsg, "|");
        //strcat(szDisMsg, "USER CANCEL");
        //usCTOSS_LCDDisplay(szDisMsg);
        //CTOS_Beep();
        //CTOS_Delay(1500);

		//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(1000);
    }
    else if (key == 1)   //CLEAR BATCH
    {
        //return d_OK;
        memcpy(szFuncTitleName,"CLEAR BATCH",11);
        if(inFunctionKeyPassword("CLEAR BATCH", SUPER_ENGINEER_PWD)==d_OK)
        {
            memset(szFuncTitleName,0,sizeof(szFuncTitleName));
            inCTOS_ManualSettle();
        }
    }
    else if (key == 2)   //CLEAR REVERSAL
    {
        //return d_OK;
        vdDebug_LogPrintf("CLEAR REVERSAL --  START");
        memcpy(szFuncTitleName,"CLEAR REVERSAL",14);

        if(inFunctionKeyPassword("CLEAR REVERSAL", SUPER_ENGINEER_PWD)==d_OK)
        {
			memset(szFuncTitleName,0,sizeof(szFuncTitleName));
            vdCTOS_DeleteReversal();
        }
    }
    else if (key == 3)   //EDIT TABLE
    {
        vdDebug_LogPrintf("EDIT TABLE --  START");
        memcpy(szFuncTitleName,"EDIT TABLE",10);

        if(inFunctionKeyPassword("EDIT TABLE", SUPER_ENGINEER_PWD)==d_OK)
            return key;
    }
    return 0;
}

int inFunctionKeyPassword(unsigned char *ptrTitle, int inPasswordLevel) {
    //char szPassword[6+1]={0};

    BYTE strOut[6 + 1] = {0};
    int inRet;
    USHORT shMaxLen = 4, shMinLen = 4;
    BYTE szMsg[100];
    BYTE szTitle[16];
    BYTE szPassword[128 + 1];
    BYTE szDisMsg[128];

    inTCTRead(1);
    vdDebug_LogPrintf("inFunctionKeyPassword -- START --");
    memset(szTitle, 0x00, sizeof(szTitle));
    strcpy(szTitle, ptrTitle);
    vdDebug_LogPrintf("inFunctionKeyPassword, szTitle = [%s]", szTitle);

    //CTOS_LCDTClearDisplay();
    //vdDispTitleString(ptrTitle);
    memset(szMsg, 0x00, sizeof(szMsg));
    strcpy(szMsg, "1");
    strcat(szMsg, "|");
    strcat(szMsg, "4");
    strcat(szMsg, "|");
    strcat(szMsg, szTitle);
    strcat(szMsg, "|");
    strcat(szMsg, "Enter password");

    memset(szPassword, 0x00, sizeof(szPassword));

    if (inPasswordLevel == SUPER_PWD) {
        strcpy(szPassword, szMsg);
        strcat(szPassword, "|");
        strcat(szPassword, strTCT.szSuperPW);
    } else if (inPasswordLevel == ENGINEER_PWD) {
        strcpy(szPassword, szMsg);
        strcat(szPassword, "|");
        strcat(szPassword, strTCT.szEngineerPW);
    } else if (inPasswordLevel == SUPER_ENGINEER_PWD) {
        strcpy(szPassword, szMsg);
        strcat(szPassword, "|");
        strcat(szPassword, strTCT.szEngineerPW);
        strcat(szPassword, "|");
        strcat(szPassword, strTCT.szSuperPW);
        vdDebug_LogPrintf("strTCT.szEngineerPW [%s], strTCT.szSuperPW [%s]", strTCT.szEngineerPW,
                          strTCT.szSuperPW);
    }

    while (1) {
        //if (strTCT.byTerminalType == 2)
        //    clearLine(V3_ERROR_LINE_ROW);
        //else
        //    clearLine(8);
        //CTOS_LCDTPrintXY(1, 3, "PASSWORD:");
        memset(strOut, 0x00, sizeof(strOut));
        //inRet = InputString(1, 8, 0x01, 0x02, strOut, &shMaxLen, shMinLen, d_INPUT_TIMEOUT);
        inRet = InputStringUI(0x02, 0x02, strOut, &shMaxLen, shMinLen, inGetIdleTimeOut(FALSE),
                              szPassword);

#if 0
        if (inRet == d_KBD_CANCEL )
        {
            memset(szFuncTitleName,0,sizeof(szFuncTitleName));
            return d_NO;
        }
        else if(0 == inRet )
        {
            memset(szFuncTitleName,0,sizeof(szFuncTitleName));
            return d_NO;
        }
        else if(inRet == 0xFF)
        {
            memset(szFuncTitleName,0,sizeof(szFuncTitleName));
            vdDisplayErrorMsg(1, 8, "TIME OUT");
            return d_NO;
        }	
        else if(inRet>=1)
        {
            if(inPasswordLevel == SUPER_PWD)
            {
                if(strcmp(strOut,strTCT.szSuperPW) == 0)
                {
                    memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                    return d_OK;
                }
            }
            else if(inPasswordLevel == ENGINEER_PWD)
            {
                if(strcmp(strOut,strTCT.szEngineerPW) == 0)
                {
                    memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                    return d_OK;
                }
            }
            else if(inPasswordLevel == SUPER_ENGINEER_PWD)
            {
                if((strcmp(strOut,strTCT.szEngineerPW) == 0) || (strcmp(strOut,strTCT.szSuperPW) == 0))
                {
                    memset(szFuncTitleName,0,sizeof(szFuncTitleName));
                    return d_OK;
                }
            }
            vdDisplayErrorMsg(1, 8, "WRONG PASSWORD");
            clearLine(8);
        }
#else
        vdDebug_LogPrintf("inRet = [%d]", inRet);
        memset(szDisMsg, 0x00, sizeof(szDisMsg));
        if (inRet == d_KBD_CANCEL) {
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "USER CANCEL");
            //usCTOSS_LCDDisplay(szDisMsg);
            //CTOS_Beep();
            //CTOS_Delay(1500);

			//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			CTOS_Beep();
			CTOS_Delay(1000);

            memset(szFuncTitleName, 0, sizeof(szFuncTitleName));
            return d_NO;
        } else if (inRet == 0xFF) {
            //strcpy(szDisMsg, szTitle);
            //strcat(szDisMsg, "|");
            //strcat(szDisMsg, "TIME OUT");
            //usCTOSS_LCDDisplay(szDisMsg);
            //CTOS_Beep();
            //CTOS_Delay(1500);

			//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
			inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
			CTOS_Beep();
			CTOS_Delay(1000);

            memset(szFuncTitleName, 0, sizeof(szFuncTitleName));
            return d_NO;
        } else if (inRet == 'A') {
            memset(szFuncTitleName, 0, sizeof(szFuncTitleName));
            return d_OK;
        }
#endif
    }
}

void vdCTOS_BINRoutingNII(void)
{

    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];

    int inNum = 0;

    inTCTRead(1);

    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "NII");
        wub_hex_2_str(strTCT.ATPNII,szTempBuf,2);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szTempBuf);

        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 4, 4, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==4)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strTCT.ATPNII, 0x00, sizeof(strTCT.ATPNII));
                wub_str_2_hex(strOut, strtemp, NII_BYTES);
                memcpy(strTCT.ATPNII, strtemp, NII_BYTES/2);
                inTCTSave(1);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }

    inTCTRead(1);

    while(1)
    {
        vduiClearBelow(3);
        setLCDPrint(3, DISPLAY_POSITION_LEFT, "TPDU");
        wub_hex_2_str(strTCT.ATPTPDU,szTempBuf,5);
        setLCDPrint(4, DISPLAY_POSITION_LEFT, szTempBuf);

        strcpy(strtemp,"New:");
        CTOS_LCDTPrintXY(1, 7, strtemp);
        memset(strOut,0x00, sizeof(strOut));
        ret= shCTOS_GetNum(8, 0x01,  strOut, &usLen, 10, 10, 0, d_INPUT_TIMEOUT);
        if (ret == d_KBD_CANCEL )
            break;
        else if(0 == ret )
            break;
        else if(ret==10)
        {
            inNum = inBatchNumRecord();
            if (inNum == 0)
            {
                memset(strTCT.ATPTPDU, 0x00, sizeof(strTCT.ATPTPDU));
                wub_str_2_hex(strOut,strtemp,TPDU_BYTES);
                memcpy(strTCT.ATPTPDU, strtemp, TPDU_BYTES/2);
                inTCTSave(1);
                break;
            }
            else
            {
                vduiWarningSound();
                vduiClearBelow(5);
                vduiDisplayStringCenter(6,"BATCH NOT");
                vduiDisplayStringCenter(7,"EMPTY,SKIPPED.");
                CTOS_Delay(2000);
                break;
            }
        }
        if (ret == d_KBD_CANCEL )
            break ;
    }
}

void vdGetTimeDate(TRANS_DATA_TABLE *srTransPara)
{
#if 0
    char temp[40+1];

    memset(temp,0x00,sizeof(temp));
    sprintf(temp, "vdGetTimeDate::[%d]", srTransPara->byPackType);
        CTOS_PrinterPutString(temp);
#endif

    //if(srTransPara->byPackType == SALE || srTransPara->byPackType == PRE_AUTH
    //|| srTransPara->byPackType == LOG_ON
    //||srTransPara->byPackType == BAL_INQ ||srTransPara->byPackType == CASH_ADV
    //||srTransPara->byPackType == PRE_COMP ||srTransPara->byPackType == REVERSAL)
    {
        CTOS_RTC SetRTC;
        BYTE szCurrentTime[20];

//        CTOS_PrinterPutString("vdGetTimeDate.1");

        CTOS_RTCGet(&SetRTC);
        memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d",SetRTC.bMonth, SetRTC.bDay);
        wub_str_2_hex(szCurrentTime,srTransPara->szDate,DATE_ASC_SIZE);

        memset(szCurrentTime, 0, sizeof(szCurrentTime));
        sprintf(szCurrentTime,"%02d%02d%02d", SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);
        wub_str_2_hex(szCurrentTime,srTransPara->szTime,TIME_ASC_SIZE);


        memset(srTransPara->szDateTime, 0x00, sizeof(srTransPara->szDateTime));
        sprintf(srTransPara->szDateTime,"20%02d%02d%02d%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour, SetRTC.bMinute, SetRTC.bSecond);

        //memcpy(srTransPara->szTime, szCurrentTime, TIME_ASC_SIZE);
    }
}


int inReadIDLEMSD(void){

    BYTE byMSR_status;

    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES_50], szTk3Buf[TRACK_III_BYTES];

    BYTE byKeyBuf;
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE bySC_status;
    int shReturn;

    BYTE szTitle[20+1];
    BYTE szDisMsg[100];

    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES_50 ;
    usTk3Len = TRACK_III_BYTES ;


    byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);


    shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

    vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

    if (shReturn == INVALID_CARD)
    {
        CTOS_KBDBufFlush();
        //vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
        //Tine:  23MAY2019
        memset(szTitle, 0x00, sizeof(szTitle));
        szGetTransTitle(srTransRec.byTransType, szTitle);
        strcpy(szDisMsg, szTitle);
        strcat(szDisMsg, "|");
        strcat(szDisMsg, "CARD READ ERROR");
        //usCTOSS_LCDDisplay(szDisMsg);
        inDisplayMessageBoxWithButtonConfirmation(1,8,"","CARD READ ERROR","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
        CTOS_Beep();
        CTOS_Delay(1500);
        CTOS_Beep();
        return INVALID_CARD;
    }

    if (d_OK != inCTOS_LoadCDTIndex())
    {
        CTOS_KBDBufFlush();
        //vdSetErrorMessage("Get Card Fail");
        return USER_ABORT;
    }

    if(d_OK != inCTOS_CheckEMVFallback())
    {
        vdCTOS_ResetMagstripCardData();
        //vdDisplayErrorMsg(1, 8, "7.PLS INSERT CARD");

        vduiClearBelow(2);
    }else{

        //vdCTOS_ResetMagstripCardData();
        strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
        srTransRec.IITid = strIIT.inIssuerNumber;
        return d_OK;
    }


    vdDispTransTitle(srTransRec.byTransType);
    CTOS_LCDTPrintXY(1, 4, "PLEASE INSERT CARD/");
    CTOS_LCDTPrintXY(1, 5, "CANCEL");

    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

    while(1){


        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES)
        {
            fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            return READ_CARD_TIMEOUT ;
        }

        CTOS_KBDInKey(&byKeyBuf);
        if (byKeyBuf)
        {
            CTOS_KBDGet(&byKeyBuf);
            if (byKeyBuf == d_KBD_CANCEL)
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
        }

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        if(bySC_status & d_MK_SC_PRESENT)
            break;


    }

    return d_OK;

}

int inCTLSEMVCheck(void)
{
/*
	if (srTransRec.byEntryMode == CARD_ENTRY_WAVE && (srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_2 ||
		srTransRec.bWaveSID == d_VW_SID_VISA_WAVE_QVSDC || srTransRec.bWaveSID == d_VW_SID_AE_EMV ||
		srTransRec.bWaveSID == d_VW_SID_CUP_EMV || srTransRec.bWaveSID == 0x64 || srTransRec.bWaveSID == 0x65 ||
		srTransRec.bWaveSID == d_VW_SID_PAYPASS_MCHIP))
		return d_OK;
*/

/*FIX - ALL CTLS (chip or MAG) will got to CREDIT or CUP*/
    if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
        return d_OK;


    return FAIL;
}

int inCTLSCUPCheck(void)
{
    if(srTransRec.byEntryMode == CARD_ENTRY_WAVE && (memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x03\x33",5) == 0))
    {
        if(fUSDSelected == TRUE) /*CUP USD*/
            strCDT.HDTid=37;
        else /*CUP PHP*/
            strCDT.HDTid=36;
        return d_OK;
    }

    return FAIL;
}

int inEMVCheckCUPEnable(void)
{
    if(memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x03\x33",5) == 0)
    {
        if(fUSDSelected == TRUE) /*CUP USD*/
            strCDT.HDTid=37;
        else /*CUP PHP*/
            strCDT.HDTid=36;

        inHDTReadData(strCDT.HDTid);
        if(strHDT_Temp.fHostEnable != TRUE)
        {
            vdDisplayErrorMsgResp2(" ", "TRANSACTION", "NOT ALLOWED");
            return FAIL;
        }
        return d_OK;
    }

    return d_OK;
}

BOOL fCUPBinRoute(void) /*cup will not go to bin routing*/
{
    if(memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x03\x33",5) == 0) /*CUP AID*/
        return d_OK;
    return FAIL;
}

int inCTOS_GetCardFieldsSwipeOnly(void)
{
    USHORT EMVtagLen;
    BYTE   EMVtagVal[64];
    BYTE byKeyBuf;
    BYTE bySC_status;
    BYTE byMSR_status;
    BYTE szTempBuf[10];
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES_50], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES_50 ;
    usTk3Len = TRACK_III_BYTES ;
    int  usResult;
    BYTE szTitle[20+1];
    BYTE szDisMsg[100];

	int inKey;

    CTOS_RTC SetRTC;

    //0826
    int inChipTries=0;
    int inEntryMode=0;
    /*
	1= insert only
	2= swipe only
	0= will accept al
	*/

#define INSERT_ONLY 1
#define SWIPE_ONLY  2
#define READ_ALL 0
    //0826

    short shReturn = d_OK; //Invalid card reading fix -- jzg

    /* BDO CLG: MOTO setup -- jzg */
    int inMOTOResult;

    vdDebug_LogPrintf("saturn inCTOS_GetCardFieldsSwipeOnly...");

    /* BDO CLG: MOTO setup - start -- jzg */
    /* BDO CLG: MOTO setup - end -- jzg */

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    CTOS_LCDTClearDisplay();

    byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);


    SWIPE_AGAIN:

    if(d_OK != inCTOS_ValidFirstIdleKey())
    {

        CTOS_LCDTClearDisplay();

        /* BDO CLG: Fleet card support - start -- jzg */
        //if(srTransRec.fFleetCard == TRUE)
        //	vdDispTransTitle(FLEET_SALE);
        //else
        /* BDO CLG: Fleet card support - end -- jzg */
        vdDispTransTitle(srTransRec.byTransType);

        //gcitra-0728
        //inCTOS_DisplayIdleBMP();
        //gcitra-0728
    }
// patrick ECR 20140516 start

// patrick ECR 20140516 end
    //CTOS_TimeOutSet (TIMER_ID_1 , GET_CARD_DATA_TIMEOUT_VALUE);
    CTOS_TimeOutSet (TIMER_ID_1 , inGetIdleTimeOut(FALSE)); /*BDO: Parameterized idle timeout --sidumili*/

    //CTOS_LCDTClearDisplay();
    //vdDispTransTitle(srTransRec.byTransType);
    //CTOS_LCDTPrintXY(1, 3, "Please Swipe");
    //CTOS_LCDTPrintXY(1, 4, "Customer Card");

	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
			
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "PLEASE SWIPE CUSTOMER CARD");
	strcat(szDisMsg, "|");
	strcat(szDisMsg, "CARD ENTRY");
	if(fEntryCardfromIDLE ==  FALSE)
		inKey=usCARDENTRY(szDisMsg);	

    while (1)
    {

        if(CTOS_TimeOutCheck(TIMER_ID_1 )  == d_YES){
            fTimeOutFlag = TRUE; /*BDO: Flag for timeout --sidumili*/
            return READ_CARD_TIMEOUT ;
        }

        CTOS_KBDInKey(&byKeyBuf);

        if (byKeyBuf)
        {
            CTOS_KBDGet(&byKeyBuf);
            switch(byKeyBuf)
            {
                case d_KBD_0:
                case d_KBD_1:
                case d_KBD_2:
                case d_KBD_3:
                case d_KBD_4:
                case d_KBD_5:
                case d_KBD_6:
                case d_KBD_7:
                case d_KBD_8:
                case d_KBD_9:
                    break;

                case d_KBD_CANCEL:

                    vdSetFirstIdleKey(byKeyBuf);
                    memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

                    CTOS_KBDBufFlush();
                    return USER_ABORT;
                    break;
            }
        }


        if (strlen(srTransRec.szPAN) > 0)
        {
            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            break;
        }

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);

        if(byMSR_status == d_OK )
        {
            shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf, usTk3Len);

            if (shReturn == INVALID_CARD)
            {
                CTOS_KBDBufFlush();
                vdDisplayErrorMsg(1, 8, "CARD READ ERROR");
                memset(szTitle, 0x00, sizeof(szTitle));
                szGetTransTitle(srTransRec.byTransType, szTitle);
                strcpy(szDisMsg, szTitle);
                strcat(szDisMsg, "|");
                strcat(szDisMsg, "CARD READ ERROR");
                //usCTOSS_LCDDisplay(szDisMsg);
                CTOS_Beep();
                //CTOS_Delay(1500);
                //CTOS_Beep();
                return INVALID_CARD;
            }

            if (d_OK != inCTOS_LoadCDTIndex())
            {
                CTOS_KBDBufFlush();
                return USER_ABORT;
            }
            break;
        }
    }

    strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    srTransRec.IITid = strIIT.inIssuerNumber;

    vdDebug_LogPrintf("srTransRec.byTransType[%d]srTransRec.IITid[%d]", srTransRec.byTransType, srTransRec.IITid);

	if (srTransRec.byEntryMode != CARD_ENTRY_WAVE)
		CTOS_Beep(); // add beep sound after pass on CDT validation -- sidumili
	
    return d_OK;
}


void vdUpdateDynamicMenu(void)
{
    int inRet = 0;

    inRet = inUpdateInstallmentMenu(3);
    vdDebug_LogPrintf("inUpdateInstallmentMenu[%d]",inRet);

    if(strTCT.fCheckout == 1)
        inSetOfflineLabel("CHECKOUT");
    else
        inSetOfflineLabel("COMPLETION");

    inSetTipAllowedLabel("TIP ALLOWED");

    inDatabase_IPReportInsert(); //Insert IP Report on the loaded MenuID

	if(strTCT.inSetMobileWalletLabel == 1)
	{
		char szButtonLabel[20+1] = {0};

		strcpy(szButtonLabel, strTCT.szMobileWalletLabel);
		vdDebug_LogPrintf("wallet label [%s]", szButtonLabel);

		if (strlen(szButtonLabel) <=0)
			strcpy(szButtonLabel, "MOBILE WALLET");
			
		inSetMobileWalletLabel(szButtonLabel);
	}
}

void vdPrintReportDisplayBMP(int inStatus)
{
	int inNumOfLine = 8;
	BYTE szTitle[20+1];
	BYTE szDisMsg[30];

   if (inStatus != 1) return;

   #if 0
   CTOS_LCDTClearDisplay();
   
   if((strTCT.byTerminalType % 2) == 0)
		 inNumOfLine = 16;
   
   if (strTCT.fSMMode	== TRUE)
   {
		if(strTCT.fDisplayPrintBitmap == TRUE)
		{	
			if((strTCT.byTerminalType % 2) == 0)
				vdDisplayAnimateBmp(0,0, "Print4.bmp", "Print5.bmp", "Print6.bmp", NULL, NULL);
			else
				vdDisplayAnimateBmp(0,0, "Print4#.bmp", "Print5#.bmp", "Print6#.bmp", NULL, NULL);
		}
		else
			
		memset(szTitle, 0x00, sizeof(szTitle));
		szGetTransTitle(srTransRec.byTransType, szTitle);
		strcpy(szDisMsg, szTitle);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "PRINTING...");
		usCTOSS_LCDDisplay(szDisMsg);		 //Tine:  24Apr2019
			//CTOS_LCDTPrintXY(1, inNumOfLine, "PRINTING...");
	  
	}
	else
	{
		if(strTCT.fDisplayPrintBitmap == TRUE)
		{	
			if((strTCT.byTerminalType % 2) == 0)
				vdDisplayAnimateBmp(0,0, "Print1.bmp", "Print2.bmp", "Print3.bmp", NULL, NULL);
			else
				vdDisplayAnimateBmp(0,0, "Print1#.bmp", "Print2#.bmp", "Print3#.bmp", NULL, NULL);
		}
		else
			
		memset(szTitle, 0x00, sizeof(szTitle));
		szGetTransTitle(srTransRec.byTransType, szTitle);
		strcpy(szDisMsg, szTitle);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "PRINTING...");
		usCTOSS_LCDDisplay(szDisMsg);		 //Tine:  24Apr2019
			//CTOS_LCDTPrintXY(1, inNumOfLine, "PRINTING...");
	}
	#else
		vdDisplayMessageStatusBox(1, 8, "PRINTING", MSG_PLS_WAIT, MSG_TYPE_PRINT);
		//CTOS_Delay(300);
	#endif
}


/*ECR menu selection --sidumili*/
void vdSelectECRMenu(char* szMenu){
    char szMenuItems[9000] = {0};

    memset(szMenuItems, 0x00, sizeof(szMenuItems));

    switch (strTCT.inECRTrxnMenu){
        case 1:
            strcat(szMenuItems, "SALE");
            strcat(szMenuItems, "\n");
            strcat(szMenuItems, "INSTALLMENT");
            strcat(szMenuItems, "\n");
            break;
        case 2:
            strcat(szMenuItems, "BINVER");
            strcat(szMenuItems, "\n");
            strcat(szMenuItems, "SALE");
            strcat(szMenuItems, "\n");
            break;
        case 3:
            strcat(szMenuItems, "INSTALLMENT");
            strcat(szMenuItems, "\n");
            break;
        default:
            strcat(szMenuItems, "BINVER");
            strcat(szMenuItems, "\n");
            strcat(szMenuItems, "SALE");
            strcat(szMenuItems, "\n");
            strcat(szMenuItems, "INSTALLMENT");
            strcat(szMenuItems, "\n");
            break;
    }

    strcpy(szMenu, szMenuItems);

}

int inReturnTrxnType(int inKey){
    int inTrxnType;
    switch (strTCT.inECRTrxnMenu){
        case 1:
            if (inKey == 1)
                inTrxnType = ECR_SALE;
            if (inKey == 2)
                inTrxnType = ECR_INSTALLMENT;
            break;
        case 2:
            if (inKey == 1)
                inTrxnType = ECR_BINVER;
            if (inKey == 2)
                inTrxnType = ECR_SALE;
            break;
        case 3:
            if (inKey == 1)
                inTrxnType = ECR_INSTALLMENT;
            break;
        default:
            if (inKey == 1)
                inTrxnType = ECR_BINVER;
            if (inKey == 2)
                inTrxnType = ECR_SALE;
            if (inKey == 3)
                inTrxnType = ECR_INSTALLMENT;
            break;

    }

    return(inTrxnType);

}

int inCheckIfDCCHost(void)
{
    if(srTransRec.HDTid >= 6 && srTransRec.HDTid <= 35 )
        return TRUE;

    return FALSE;
}

void vdSetECRResponse(char* szECRResponse)
{
    vdDebug_LogPrintf("vdSetECRResponse :: szECRResponse :: [%s]",szECRResponse);
    strcpy(srTransRec.szRespCode,"");
    strcpy(srTransRec.szECRRespCode, szECRResponse); //make responce code as -1 - for ECR transaction
}

int inCTOS_Reprint_Settle_Selection(void){

    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, Menukey=0;
    int inRet;
    int key;
    USHORT shRet = 0,
            shLen = 0,
            iInitX = 0,
            shMinLen = 4,
            shMaxLen = 6;
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE + 1] = {0},
            szTitle[MAX_CHAR_PER_LINE + 1] = {0};

#if 0
    //aaronnino for bdoclg ver 9.0 fix on issue #00694 Incorrect order of prompt � Settle All Card Types first before merchant selection then password start
    CTOS_LCDTClearDisplay();

    vdCTOS_SetTransType(SETTLE);
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
          return inRet;

    CTOS_LCDTClearDisplay();

    inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
    if(d_OK != inRet)
          return inRet;
    //aaronnino for bdoclg ver 9.0 fix on issue #00694 Incorrect order of prompt � Settle All Card Types first before merchant selection then password end
#endif
    CTOS_LCDTClearDisplay();

    memset(szTitle, 0x00, sizeof(szTitle));
    strcpy(szTitle, "SETTLEMENT");
    iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle));
    CTOS_LCDTSetReverse(TRUE);
    CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    CTOS_LCDTSetReverse(FALSE);

    setLCDPrint(3, DISPLAY_POSITION_LEFT, "REPRINT SETTLE ALL");
    setLCDPrint(4, DISPLAY_POSITION_LEFT,"CARD TYPES?");
    setLCDPrint(6, DISPLAY_POSITION_LEFT,"1.YES");
    setLCDPrint(7, DISPLAY_POSITION_LEFT,"2.NO");

    while (1){

        key = WaitKey(60);

        if(key == d_KBD_1){
            vdCTOS_RePrintSettleAll();
            break;
        }
        else if(key == d_KBD_2){
            ushCTOS_ReprintLastSettleReport();
            break;
        }
        else if (key == d_KBD_CANCEL)
            return(d_NO);
        else
            vduiWarningSound();

    }

    return SUCCESS;


}

int vdCTOS_RePrintSettleAll()
{
    ACCUM_REC srAccumRec;
    unsigned char chkey;
    short shHostIndex;
    int inResult,inRet;
    int inTranCardType;
    int inReportType;
    int inIITNum, inNum = 0, inNumOfHost = 0, i;
    BYTE baTemp[PAPER_X_SIZE * 64];
    int inHDTid[150];
    int inHDTIndex=0;

    vdDebug_LogPrintf("aaa vdCTOS_RePrintSettleAll start");

    //for MP200 no need print
    if ((strTCT.byTerminalType == 5) || (strTCT.byTerminalType == 6))
        return (d_OK);

    if( printCheckPaper()==-1)
        return d_NO;

    inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
    if(d_OK != inRet)
        return inRet;

    vdDebug_LogPrintf("AAA - inRet[%d]", inRet);

    srTransRec.MITid = strNMT[0].NMTID;

    vdDebug_LogPrintf("AAA - srTransRec.MITid[%d]", srTransRec.MITid);

    //inNumOfHost=inHDTReadOrderBySequence(inHDTid);
    //inNumOfHost = inHDTNumRecord();

    inNumOfHost=inMMTNumRecordwithSettleStatusSuccess(srTransRec.MITid, inHDTid);

    vdDebug_LogPrintf("inNumOfHost=[%d]-----",inNumOfHost);
    for(inNum =1 ;inNum <= inNumOfHost; inNum++)
    {
        inHDTIndex=inHDTid[inNum-1];

        if(inHDTRead(inHDTIndex) == d_OK)
        {
            if (strHDT.fHostEnable != 1)
                continue;

            //inMMTReadRecord(inHDTIndex,srTransRec.MITid);

            //if (strMMT[0].inSettleStatus!=1)
            //continue;

            inCSTRead(strHDT.inCurrencyIdx);

            //srTransRec.HDTid = inNum;
            srTransRec.HDTid = inHDTIndex;

            if (inMultiAP_CheckMainAPStatus() == d_OK)
            {
                vdDebug_LogPrintf("AAA - MAIN APP");
                ushCTOS_ReprintLastSettleReportAll();
            }
            else
            {
                if (inMultiAP_CheckSubAPStatus() == d_OK)
                {
                    vdDebug_LogPrintf("AAA - SUB APP");
                    inRet = inCTOS_MultiAPGetData();
                    if(d_OK != inRet)
                        return inRet;

                    inRet = inCTOS_MultiAPReloadHost();
                    if(d_OK != inRet)
                        return inRet;
                }
            }
        }
    }

}

int inHostInfoAppSelection(char *szAPName)
{
    BYTE key=0;
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    char szHeaderString[50] = "HOST INFO";
    char szHostMenu[50]={"CREDIT\nDEBIT\nINSTALLMENT\nCUP\n"};

    CTOS_LCDTClearDisplay();

	if(strTCT.inSetMobileWalletLabel == 1)
	{
			strcat(szHostMenu, strTCT.szMobileWalletLabel);
	}
	else
	{
			strcat(szHostMenu, "MOBILE WALLET");
	}

    inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);
    inSetTextMode();

    if (key == 0xFF)
    {
        CTOS_LCDTClearDisplay();
        //vduiWarningSound();
        return FAIL;
    }

    if(key > 0)
    {
        if (get_env_int("INVALIDKEY") == 1)
            return FAIL;

        if(key == 1)
            strcpy(szAPName,V5S_BDOCREDIT);
        else if(key == 2)
            strcpy(szAPName,V5S_BDODEBIT);
        else if(key == 3)
            strcpy(szAPName,V5S_BDOINST);
        else if(key == 4)
            strcpy(szAPName,V5S_BDOCUP);
        else if(key == 5)
            strcpy(szAPName,V5S_BDOPAY);

        return d_OK;
    }

}


int inCTOSS_CheckMemoryStatusEx(char *szFuncName)
{
#define SAFE_LIMIT_SIZE 5000

    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];

    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];

    // if (inMultiAP_CheckSubAPStatus() == d_OK)
    //     return d_OK;
    vdDebug_LogPrintf("%s.........",szFuncName);

    memset(szUsedDiskSize,0,sizeof(szUsedDiskSize));
    memset(szTotalDiskSize,0,sizeof(szTotalDiskSize));
    memset(szUsedRamSize,0,sizeof(szUsedRamSize));
    memset(szTotalRamSize,0,sizeof(szTotalRamSize));
    memset(szAvailableRamSize,0,sizeof(szAvailableRamSize));
    memset(szAvailableDiskSize,0,sizeof(szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    vdDebug_LogPrintf("[%ld],[%ld][%ld][%ld]",ulUsedDiskSize,ulTotalDiskSize,ulUsedRamSize,ulTotalRamSize);
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;

    sprintf(szTotalDiskSize,"%s:%ld","Total disk",ulTotalDiskSize);
    sprintf(szUsedDiskSize,"%s:%ld","Used   disk",ulUsedDiskSize);
    sprintf(szAvailableDiskSize,"%s:%ld","Avail disk",ulAvailDiskSize);

    sprintf(szTotalRamSize,"%s:%ld","Total RAM",ulTotalRamSize);
    sprintf(szUsedRamSize,"%s:%ld","Used   RAM",ulUsedRamSize);
    sprintf(szAvailableRamSize,"%s:%ld","Avail RAM",ulAvailableRamSize);
    vdDebug_LogPrintf("ulAvailDiskSize[%ld],ulAvailableRamSize[%ld]",ulAvailDiskSize,ulAvailableRamSize);

    if (ulAvailDiskSize < SAFE_LIMIT_SIZE)
    {
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(1, 7, "Settle  soon");
        vdDisplayErrorMsg(1, 8,  "Insufficient Memory");
        return FAIL;
    }

    if (ulAvailableRamSize < SAFE_LIMIT_SIZE)
    {
        CTOS_LCDTClearDisplay();
        vdSetErrorMessage("Insufficient RAM");
        return FAIL;
    }

    return d_OK;

}

int inCTOSS_GetRAMMemorySize(char *Funname)
{
    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    int i = 0;

    /*disable inCTOSS_GetRAMMemorySize*/
    return d_OK;

    for (i = 0; i < 10000; i++)	//RAM usage is fluctuating, loop first
        ;

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    vdDebug_LogPrintf("[%s], Used RAM[%ld], Total RAM[%ld], Available RAM[%ld]", Funname, ulUsedRamSize,ulTotalRamSize,ulAvailableRamSize);

    return d_OK;

}


int inRecoverRAM()
{
#define SAFE_LIMIT_SIZE 7000
#define RESTART_LIMIT_SIZE 6000

    ULONG ulUsedDiskSize = 0;
    ULONG ulTotalDiskSize = 0;
    ULONG ulUsedRamSize = 0;
    ULONG ulTotalRamSize = 0;

    ULONG ulAvailableRamSize = 0;
    ULONG ulAvailDiskSize = 0;

    UCHAR szUsedDiskSize[50];
    UCHAR szTotalDiskSize[50];
    UCHAR szUsedRamSize[50];
    UCHAR szTotalRamSize[50];

    UCHAR szAvailableRamSize[50];
    UCHAR szAvailableDiskSize[50];

    int inCTR;

    if (inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;

    memset(szUsedDiskSize,0,sizeof(szUsedDiskSize));
    memset(szTotalDiskSize,0,sizeof(szTotalDiskSize));
    memset(szUsedRamSize,0,sizeof(szUsedRamSize));
    memset(szTotalRamSize,0,sizeof(szTotalRamSize));
    memset(szAvailableRamSize,0,sizeof(szAvailableRamSize));
    memset(szAvailableDiskSize,0,sizeof(szAvailableDiskSize));

    usCTOSS_SystemMemoryStatus( &ulUsedDiskSize , &ulTotalDiskSize, &ulUsedRamSize, &ulTotalRamSize );
    vdDebug_LogPrintf("[%ld],[%ld][%ld][%ld]",ulUsedDiskSize,ulTotalDiskSize,ulUsedRamSize,ulTotalRamSize);
    ulAvailableRamSize = ulTotalRamSize - ulUsedRamSize;
    ulAvailDiskSize = ulTotalDiskSize - ulUsedDiskSize;

    sprintf(szTotalDiskSize,"%s:%ld","Total disk",ulTotalDiskSize);
    sprintf(szUsedDiskSize,"%s:%ld","Used   disk",ulUsedDiskSize);
    sprintf(szAvailableDiskSize,"%s:%ld","Avail disk",ulAvailDiskSize);

    sprintf(szTotalRamSize,"%s:%ld","Total RAM",ulTotalRamSize);
    sprintf(szUsedRamSize,"%s:%ld","Used   RAM",ulUsedRamSize);
    sprintf(szAvailableRamSize,"%s:%ld","Avail RAM",ulAvailableRamSize);
    vdDebug_LogPrintf("ulAvailDiskSize[%ld],ulAvailableRamSize[%ld]",ulAvailDiskSize,ulAvailableRamSize);


    vdDebug_LogPrintf("APP NAME %s",szGlobalAPName);

    if (ulAvailableRamSize < RESTART_LIMIT_SIZE){
        vdDebug_LogPrintf("CTOS_SystemReset");
        CTOS_SystemReset();
        //vdCTOSS_ReForkSubAP("SHARLS_COM");
        //vdCTOSS_ReForkSubAP("SHARLS_ECRBDO");
        //vdCTOSS_ReForkSubAP("SHARLS_EMV");
        //vdCTOSS_ReForkSubAP("SHARLS_CTLS");

    }

    if (ulAvailableRamSize < SAFE_LIMIT_SIZE)
    {
        put_env_int("CRDTCTR",0);
        put_env_int("MAINRESET",1);
        //android
        //vdCTOSS_MainAPMemoryRecover();
    }


    if (strcmp (szGlobalAPName, V5S_BDOCREDIT) == 0){

        inCTR = get_env_int("CRDTCTR");

        inCTR++;

        if (inCTR >= 15){
            put_env_int("CRDTCTR",0);
            //android
            //vdDebug_LogPrintf("vdCTOSS_MainAPMemoryRecover %d %s", inCTR, szGlobalAPName);

            put_env_int("MAINRESET",1);
            //android
            //vdCTOSS_MainAPMemoryRecover();
        }else
            put_env_int("CRDTCTR",inCTR);


    }else  if (strcmp(szGlobalAPName, V5S_BDOINST) == 0){

        inCTR = get_env_int("INSTCTR");

        inCTR++;

        if (inCTR >= 5){
            put_env_int("INSTCTR",0);
            //android-removed
            //vdDebug_LogPrintf("vdCTOSS_ReForkSubAP %d %s", inCTR, szGlobalAPName);
            //android-removed
            //vdCTOSS_ReForkSubAP(szGlobalAPName);
        }else
            put_env_int("INSTCTR",inCTR);


    }else  if (strcmp (szGlobalAPName, V5S_BDODEBIT) == 0){

        inCTR = get_env_int("DEBTCTR");

        inCTR++;

        if (inCTR >= 5){
            put_env_int("DEBTCTR",0);

            vdDebug_LogPrintf("vdCTOSS_ReForkSubAP %d %s", inCTR, szGlobalAPName);
            //android-removed
            //vdCTOSS_ReForkSubAP(szGlobalAPName);
        }else
            put_env_int("DEBTCTR",inCTR);


    }else  if (strcmp (szGlobalAPName, "V5S_CUP") == 0){

        inCTR = get_env_int("CUPCTR");

        inCTR++;

        if (inCTR >= 5){
            put_env_int("CUPCTR",0);

            vdDebug_LogPrintf("vdCTOSS_ReForkSubAP %d %s", inCTR, szGlobalAPName);
            //android-removed
            //vdCTOSS_ReForkSubAP(szGlobalAPName);
        }else
            put_env_int("CUPCTR",inCTR);


    }else  if (strcmp (szGlobalAPName, V5S_BDOPAY) == 0){

        inCTR = get_env_int("QRPAYCTR");

        inCTR++;

        if (inCTR >= 5){
            put_env_int("QRPAYCTR",0);

            vdDebug_LogPrintf("vdCTOSS_ReForkSubAP %d %s", inCTR, szGlobalAPName);
            //android-removed
            //vdCTOSS_ReForkSubAP(szGlobalAPName);
        }else
            put_env_int("QRPAYCTR",inCTR);


    }






    return d_OK;

}


int inReforkSubAPP(void){


    if (strcmp(szGlobalAPName, V5S_BDOINST) == 0){

        put_env_int("INSTCTR",0);
        vdDebug_LogPrintf("inReforkSubAPP %s", szGlobalAPName);
        //android-removed
        //vdCTOSS_ReForkSubAP(szGlobalAPName);

    }else  if (strcmp (szGlobalAPName, V5S_BDODEBIT) == 0){

        put_env_int("DEBTCTR",0);
        vdDebug_LogPrintf("inReforkSubAPP %s", szGlobalAPName);

        //android-removed
        //vdCTOSS_ReForkSubAP(szGlobalAPName);

    }else  if (strcmp (szGlobalAPName, "V5S_CUP") == 0){

        put_env_int("CUPCTR",0);
        vdDebug_LogPrintf("inReforkSubAPP %s", szGlobalAPName);

        //android-removed
        //vdCTOSS_ReForkSubAP(szGlobalAPName);

    }else  if (strcmp (szGlobalAPName, V5S_BDOPAY) == 0){

        put_env_int("QRPAYCTR",0);
        vdDebug_LogPrintf("inReforkSubAPP %s", szGlobalAPName);

        //android-removed
        //vdCTOSS_ReForkSubAP(szGlobalAPName);

    }

    return d_OK;

}

void vdHandPinPadtoCustomer(void)
{
    int iRow=3;
    BYTE key;

    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    if((strTCT.byTerminalType%2) == 0)
        iRow=6;

    setLCDPrint(iRow, DISPLAY_POSITION_LEFT, "PLEASE HAND PIN PAD");
    setLCDPrint(iRow+1, DISPLAY_POSITION_LEFT, "TO CUSTOMER");
    setLCDPrint(iRow+3, DISPLAY_POSITION_LEFT, "PRESS ENTER");

    while(1)
    {
        CTOS_KBDHit(&key);
        if(key == d_KBD_ENTER)
            break;
    }

}

void vdGetCRC(void)
{
    FILE* file;
    int nLen;
    unsigned char chBuffer[1024];
    BYTE baTemp[10+1];


    memset(chBuffer, 0, 1024);
    vdDebug_LogPrintf("*** vdGetCRC V5S_BDOCREDIT***");

    if ((file = fopen (V5S_BDOCREDIT, "rb")) != NULL)
    {
        vdDebug_LogPrintf("REINER :: fopen[%d]",file);
        while (nLen = fread (chBuffer, 1, 512, file))
        {
        }

        memset(baTemp, 0x00, sizeof(baTemp));
        sprintf(baTemp, "%02x%02x%02x%02x",
                wub_lrc((unsigned char *) &chBuffer[0], 128),
                wub_lrc((unsigned char *) &chBuffer[127], 128),
                wub_lrc((unsigned char *) &chBuffer[255], 128),
                wub_lrc((unsigned char *) &chBuffer[511], 128));

        vdDebug_LogPrintf("REINER :: baTemp[%s]", baTemp);
        //inPrint(baTemp);
        put_env_char("CREDITCRC",baTemp);
        fclose (file);
    }

}

void vdGetCRC_ALL(void)
{
    int inResult=0;

    vdDebug_LogPrintf("****vdGetCRC_ALL****");

    if (inMultiAP_CheckMainAPStatus() == d_OK)
    {
        vdGetCRC();
        inResult = inCTOS_MultiAPALLAppEventID(d_IPC_CMD_GET_CRC_ALL);
    }
    else
    {
        vdGetCRC();
    }
}

void vdClearBelowLine(int inLine)
{
    int i=0, inIndex=0;
    int inNoLine=8;
    char szTemp[360+1];

    memset(szTemp, 0, sizeof(szTemp));

    if ((strTCT.byTerminalType % 2) == 0)
        inNoLine=16;


    inNoLine-=inLine;
    inNoLine--;

    memset(szTemp, 0, sizeof(szTemp));
    for(i=0; i<=inNoLine; i++)
    {
        memcpy(&szTemp[inIndex], "                    \n", 21);
        inIndex+=21;
    }

    memcpy(&szTemp[inIndex], "                     ", 21);

    //vdDebug_LogPrintf("vdClearBelowLine");
    //vdDebug_LogPrintf("szTemp:[%s]", szTemp);
    //vdDebug_LogPrintf("inNoLine:[%d]", inNoLine);
    //vdDebug_LogPrintf("inLine:[%d]", inLine);

    //setLCDPrint(inLine, DISPLAY_POSITION_LEFT, szTemp);
    setLCDPrint27(inLine, DISPLAY_POSITION_LEFT, szTemp);
}

void vdClearBelowLineEx(int inLine)
{
    int i=0, inIndex=0;
    int inNoLine=8;
    char szTemp[360+1];

    memset(szTemp, 0, sizeof(szTemp));

    if ((strTCT.byTerminalType % 2) == 0)
        inNoLine=16;


    inNoLine-=inLine;
    inNoLine--;

    memset(szTemp, 0, sizeof(szTemp));
    for(i=0; i<=inNoLine; i++)
    {
        memcpy(&szTemp[inIndex], "                    \n", 21);
        inIndex+=21;
    }

    memcpy(&szTemp[inIndex], "                     ", 21);

    //vdDebug_LogPrintf("vdClearBelowLine");
    //vdDebug_LogPrintf("szTemp:[%s]", szTemp);
    //vdDebug_LogPrintf("inNoLine:[%d]", inNoLine);
    //vdDebug_LogPrintf("inLine:[%d]", inLine);

    setLCDPrint(inLine, DISPLAY_POSITION_LEFT, szTemp);
    //setLCDPrint27(inLine, DISPLAY_POSITION_LEFT, szTemp);
}


void vdDCCFormatAmount(BYTE *szAmtBuff, BYTE *szAmtBuffOut)
{
    BYTE szTempAmtBuff[20+1];
    BYTE szTempAmtBuff2[20+1];
    float fDCCCurAmt_Temp=0;
    int inLen;

    vdDebug_LogPrintf("*** vdDCCFormatAmount ***");

    if(strCST.inMinorUnit == 3)//if currency has 3 decimal places/minor units. Round off to two decimal places.
    {
        fDCCCurAmt_Temp = atof(szAmtBuff);
        vdDebug_LogPrintf("REINER :: szTempAmtBuff[%s]",szTempAmtBuff);
        memset(szTempAmtBuff,0x00,sizeof(szTempAmtBuff));
        sprintf(szTempAmtBuff,"%.2f",fDCCCurAmt_Temp);

        vdDebug_LogPrintf("0. REINER :: szTempAmtBuff[%s], len[%d]",szTempAmtBuff, strlen(szTempAmtBuff));

        *szAmtBuffOut=0;

        memset(szTempAmtBuff2, 0x00, sizeof(szTempAmtBuff2));

        inLen=strlen(szTempAmtBuff)-3;
        memcpy(szTempAmtBuff2, szTempAmtBuff, inLen);
        memcpy(&szTempAmtBuff2[inLen], &szTempAmtBuff[inLen+1], 2);
        memcpy(szAmtBuffOut, szTempAmtBuff2, strlen(szTempAmtBuff2));
        vdDebug_LogPrintf("1. REINER :: szAmtBuffOut[%s]",szAmtBuffOut);
    }
    else if(strCST.inMinorUnit == 0)
    {
        memset(szTempAmtBuff,0x00,sizeof(szTempAmtBuff));
        strcat(szTempAmtBuff,szAmtBuff);
        strcat(szTempAmtBuff,"00");

        memset(szAmtBuff,0x00,sizeof(szAmtBuff));
        strcpy(szAmtBuffOut,szTempAmtBuff);
    }
    else
    {
        *szAmtBuffOut=0;

        memset(szTempAmtBuff2, 0x00, sizeof(szTempAmtBuff2));
        vdDebug_LogPrintf("2.a REINER :: szAmtBuff[%s]",szAmtBuff);
        inLen=strlen(szAmtBuff)-3;
        memcpy(szTempAmtBuff2, szAmtBuff, inLen);
        vdDebug_LogPrintf("2.b REINER :: szTempAmtBuff2[%s]",szTempAmtBuff2);
        memcpy(&szTempAmtBuff2[inLen], &szAmtBuff[inLen+1], 2);
        vdDebug_LogPrintf("2.c REINER :: szTempAmtBuff2[%s]",szTempAmtBuff2);
        memcpy(szAmtBuffOut, szTempAmtBuff2, strlen(szTempAmtBuff2));
        vdDebug_LogPrintf("2. REINER :: szAmtBuffOut[%s]",szAmtBuffOut);
        //memcpy(szAmtBuffOut, szAmtBuff, strlen(szAmtBuff));
    }
}

void vdDCCModifyAmount(BYTE *szAmtBuffIn, BYTE *szAmtBuffOut)
{

    BYTE szAmtBuff2[20+1], szAmtBuff3[20+1];
    BYTE szAmtBuffTemp1[20+1];
    int inLength=0;

    vdDebug_LogPrintf("*** vdDCCModifyAmount :: szAmtBuffIn[%s]***",szAmtBuffIn);
    memset(szAmtBuff2, 0x00, sizeof(szAmtBuff2));
    memset(szAmtBuff3, 0x00, sizeof(szAmtBuff3));
    memset(szAmtBuffTemp1, 0x00, sizeof(szAmtBuff3));

    if(strlen(szAmtBuffIn) == 1 || strlen(szAmtBuffIn) == 2 || strlen(szAmtBuffIn) == 3)
    {
        if(strCST.inMinorUnit == 0 || strCST.inMinorUnit == 2)
            sprintf(szAmtBuffTemp1,"%03ld",atol(szAmtBuffIn));
        else if(strCST.inMinorUnit == 3)
            sprintf(szAmtBuffTemp1,"%04ld",atol(szAmtBuffIn));

        memset(szAmtBuffIn,0x00,sizeof(szAmtBuffIn));
        strcpy(szAmtBuffIn,szAmtBuffTemp1);
    }

    //inLength=12-strCST.inMinorUnit;
    inLength=strlen(szAmtBuffIn)-strCST.inMinorUnit;
    vdDebug_LogPrintf("*** vdDCCModifyAmount :: inLength[%d]***",inLength);
    vdDebug_LogPrintf("*** vdDCCModifyAmount :: strCST.inMinorUnit[%d]***",strCST.inMinorUnit);
    memcpy(szAmtBuff2, szAmtBuffIn, inLength);

    if(strCST.inMinorUnit > 0)
    {
        strcat(szAmtBuff2, ".");
        //memcpy(szAmtBuff2+inLength+1, srTransRec.szDCCCurAmt+inLength, strCST.inMinorUnit);
        memcpy(szAmtBuff2+inLength+1, szAmtBuffIn+inLength, strCST.inMinorUnit);
    }
    vdDebug_LogPrintf("REINER :: szAmtBuffIn[%s]",szAmtBuffIn);
    vdDebug_LogPrintf("REINER :: szAmtBuff2[%s]",szAmtBuff2);
    vdDCCFormatAmount(&szAmtBuff2, &szAmtBuff3);
    vdDebug_LogPrintf("REINER :: AFTER vdDCCFormatAmount szAmtBuff3[%s]",szAmtBuff3);
    vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szAmtBuff3, szAmtBuffOut);

}

void vdCTOS_FormatPAN3(char *szFmt,char* szInPAN,char* szOutPAN)
{
    char szCurrentPAN[20];
    int inFmtIdx = 0;
    int inPANIdx = 0;
    int inFmtPANSize;
    //int inPANless4;

    int i=0;

    inFmtPANSize = strlen(szInPAN);

    //inPANless4 = inFmtPANSize - 4;



    if (strlen(szFmt) == 0)
    {
        strncpy(szOutPAN,szInPAN,inFmtPANSize);
        return;
    }

    memset(szCurrentPAN, 0x00, sizeof(szCurrentPAN));
    memcpy(szCurrentPAN,szInPAN,strlen(szInPAN));

    while(i<=inFmtPANSize)
    {

        //if (i >= inPANless4)
        if (i >= inFmtPANSize){


            if (szFmt[inFmtIdx] == ' '){
                szOutPAN[inFmtIdx] = szFmt[inFmtIdx];
                inFmtIdx++;
            }else{
                szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx]; ;
                inFmtIdx++;
                inPANIdx++;
                i++;
            }
            continue;

        }


        if(szFmt[inFmtIdx] == 'N' || szFmt[inFmtIdx] == 'n')
        {
            szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
            inFmtIdx++;
            inPANIdx++;
            i++;
        }
        else if (szFmt[inFmtIdx] == 'X' || szFmt[inFmtIdx] == 'x' ||szFmt[inFmtIdx] == '*')
        {

            memcpy(&szOutPAN[inFmtIdx],&szFmt[inFmtIdx],1);
            inFmtIdx++;
            inPANIdx++;
            i++;
        }
        else if (!isdigit(szFmt[inFmtIdx]))
        {
            szOutPAN[inFmtIdx] = szFmt[inFmtIdx];
            inFmtIdx++;
        }
    }

    //while(szCurrentPAN[inPANIdx]!= 0x00)
    //{
    // szOutPAN[inFmtIdx] = szCurrentPAN[inPANIdx];
    // inFmtIdx++;
    // inPANIdx++;
    //}

    return;
}

int inCheckEthernetConnected(void)
{
    int shRet,usRtn;

    BYTE szTitle[20+1];
    BYTE szDisMsg[100];
    DWORD dwStatus;
    static USHORT usNetworkType = 0;
    static USHORT usEthType = 1;
    static BYTE szNetworkName[128+1] = {0};
    STRUCT_SHARLS_COM Sharls_COMData;

    vdDebug_LogPrintf("*** inCheckEthernetConnected start ***");
    //if ( CTOS_CradleAttached() == FALSE){
    //shRet = CTOS_EthernetOpenEx();
    //vdDebug_LogPrintf("*** CTOS_EthernetOpenEx [%d] ***");

    dwStatus = 0;
#if 1
    usRtn = CTOS_EthernetStatus(&dwStatus);
    //flag = 0;
    vdDebug_LogPrintf("CTOS_EthernetStatus,usRtn=[%x],dwStatus=[%x]", usRtn, dwStatus);
    if (dwStatus & d_STATUS_ETHERNET_PHYICAL_ONLINE)
    {
        vdDebug_LogPrintf("ETHERNET CONNECTED");
        vdDebug_LogPrintf("BEFORE SUCCESS WITHOUT DOCK");
        return SUCCESS;
    }
    else
    {
        vdDebug_LogPrintf("ETHERNET DISCONNECTED");
        return ST_COMMS_DISCONNECT;
    }

    //}

#else
	{

		inCTOSS_GetGPRSSignalEx1(&usNetworkType, szNetworkName, &usEthType, &Sharls_COMData);
		vdDebug_LogPrintf("usEthType[%s]",usEthType);
		if (usEthType != 1)//ETH connection not detected
		{
			vdDebug_LogPrintf("ETHERNET DISCONNECTED");
			return ST_COMMS_DISCONNECT;
		}
		else
		{
			vdDebug_LogPrintf("ETHERNET CONNECTED");
			vdDebug_LogPrintf("BEFORE SUCCESS WITH DOCK");
			return SUCCESS;
		}

	}
#endif
}

void vdResetAllCardData(void)
{
    inCTOSS_CLMCancelTransaction();
    vdCTOS_ResetMagstripCardData();
    memset(&stRCDataAnalyze,0x00,sizeof(EMVCL_RC_DATA_ANALYZE));
    memset(&strEMVT,0x00, sizeof(STRUCT_EMVT));
    srTransRec.bWaveSID = 0x00;
    srTransRec.usChipDataLen = 0;
    srTransRec.usAdditionalDataLen = 0;
    memset(srTransRec.baChipData,0x00,CHIP_DATA_LEN+1);
    memset(srTransRec.baAdditionalData,0x00,ADD_DATA_LEN+1);

}

int inSetIssuerforBINRouteDCC(void)
{
    char szPAN[8+1] = {0};
    long inPAN = 0;

    if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        if(srTransRec.IITid == 2 || srTransRec.IITid == 4 || srTransRec.IITid == 23 || srTransRec.IITid == 24)//Proceed to DCC if MasterCard or VISA
            return TRUE;
        else
        {
            if(fNoCTLSSupportforBinRoute == FALSE)//False indicates that initial card entry is ICC and should not perform inCTOSEMVProcessing(). If TRUE, initial card entry is CTLS, still need to perform inCTOS_EMVProcessing().
                fNoEMVProcess = TRUE;// Since terminal will not perform DCC, do not repeat inCTOS_EMVProcessing()
            return FALSE;
        }
    }

    memset(szPAN,0x00,8+1);
    memcpy(szPAN,srTransRec.szPAN,8);

    inPAN = atol(szPAN);
    vdDebug_LogPrintf("inSetIssuerforBINRouteDCC inPAN[%d]",inPAN);

    if(40000000 < inPAN && inPAN < 49999999)
        srTransRec.IITid = 2;
    else if((50000000 < inPAN && inPAN < 59999999) || (22210000 < inPAN && inPAN < 27209999))
        srTransRec.IITid = 4;
    else
        srTransRec.IITid = 21;

    inIITRead(srTransRec.IITid);
    strcpy(srTransRec.szCardLable, strIIT.szIssuerLabel);
    vdDebug_LogPrintf("strIIT.inIssuerNumber[%d]",strIIT.inIssuerNumber);

    if(srTransRec.IITid == 21)
        return FALSE;//Do not perform DCC transaction
    else
        return TRUE;

}

int inCheckifAllSpaces(char *szAmount)
{
    int inCount = 0;

    vdDebug_LogPrintf("szAmount[%s]",szAmount);
    while(1)
    {
        vdDebug_LogPrintf("szAmount[%d][%c]",inCount,szAmount[inCount]);
        if(szAmount[inCount] == IS_SPACE)
            inCount++;
        else
        {
            vdDebug_LogPrintf("inCheckifAllSpaces is FALSE");
            return FALSE;
        }

        if(inCount == 12)
            break;
    }

    vdDebug_LogPrintf("inCheckifAllSpaces is TRUE");
    return TRUE;


}

int inValidBinRouteRespCode(void)
{
    int inRespcode = 0;

    inRespcode = atoi(srTransRec.szRespCode);

    if((inRespcode == 60) || (inRespcode == 69) || (inRespcode == 70)|| (inRespcode == 71) || (inRespcode == 72) || (inRespcode == 73) || (inRespcode == 74) || (inRespcode == 79))
        return TRUE;
    else
        return FALSE;

}

void vdLeftShiftAmout(int inShift, BYTE *baAmount)
{
    int inLen = strlen(baAmount);
    int inCounter;

    vdDebug_LogPrintf("2. baAmount[%s]",baAmount);
    for(inCounter = 0; inCounter < inLen; inCounter++)
        baAmount[inCounter] = baAmount[inCounter+inShift];

    for(inCounter = inLen-1; inShift>0; inShift--)
        baAmount[inCounter] = 0x30;

    vdDebug_LogPrintf("2.1. baAmount[%s]",baAmount);
    return;
}

void vdRightShiftAmount(int inShift, BYTE *baAmount)
{
    int inLen = strlen(baAmount);
    int inCounter;

    inShift = abs(inShift);//convert - to +
    vdDebug_LogPrintf("3. baAmount[%s]",baAmount);
    for(inCounter = inLen-1; inCounter >= 0; inCounter--)
        baAmount[inCounter] = baAmount[inCounter-inShift];

    for(inCounter = 0; inCounter < inShift; inCounter++)
        baAmount[inCounter] = 0x30;

    vdDebug_LogPrintf("3.1. baAmount[%s]",baAmount);
    return;

}

void vdFormatDCCTipAmt(BYTE *baAmount)
{
    int inShift = 0;

    inShift= strCST.inMinorUnit - 2/*normal minor unit*/;
    vdDebug_LogPrintf("1. baAmount[%s]",baAmount);

    if(inShift > 0)	/*shift inShift places to the left*/
        vdLeftShiftAmout(inShift, baAmount);
    else if(inShift < 0)/*shift inShift places to the right*/
        vdRightShiftAmount(inShift,baAmount);

    return;
}

void vdSaveLastInvoiceNo(void)
{
    if(VS_FALSE == fRePrintFlag)
        memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);

    memcpy(strTCT.szLastInvoiceNo,srTransRec.szInvoiceNo,INVOICE_BCD_SIZE);
    if(inTCTSave(1) != ST_SUCCESS)
        vdDisplayErrorMsg(1, 8, "Update TCT fail");
}

int inCTOSS_MultiApp1Menu(void)
{
    inCTOSS_MultiAppMenu(V5S_BDOINST , d_IPC_CMD_FUN_KEY1);

    return d_OK;
}

int inCTOSS_MultiAppMenu(const char szAPName[25] , int IPC_EVENT_ID)
{
    //CTOS_stCAPInfo stinfo;
    //int inLoop = 0;
    //char szHostMenu[1024];
    //int inHostindex = 0;
    //char szHostName[d_MAX_APP+1][25];
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    //char szAPName[25];
    //int inAPPID;
#if 0
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    BYTE key;
	char szHeaderString[50] = "SELECT AP";


	memset(szAPName,0x00,sizeof(szAPName));
	inMultiAP_CurrentAPNamePID(szAPName, &inAPPID);

	vdDebug_LogPrintf("begain inCTOSS_MultiDislplayMenu............");
	memset(szHostMenu, 0x00, sizeof(szHostMenu));
    memset(szHostName, 0x00, sizeof(szHostName));

  	for (inLoop = 0; d_MAX_APP > inLoop; inLoop++)
	{
		memset(&stinfo, 0x00, sizeof(CTOS_stCAPInfo));

		if (CTOS_APGet(inLoop, &stinfo) != d_OK)
		{
 			CTOS_APGet(inLoop, &stinfo);
		}

 		vdDebug_LogPrintf("baName[%s] ", stinfo.baName);

		if (memcmp(stinfo.baName, "SHARLS_", 7) == 0)
			continue;

		if (stinfo.baName[0] != (char)0x00)
		{
			strncpy(szHostName[inHostindex++],stinfo.baName,25);
		}
	}

	for (inLoop = 0; inLoop < d_MAX_APP; inLoop++)
    {
        if (szHostName[inLoop][0]!= 0)
        {
            strcat((char *)szHostMenu, szHostName[inLoop]);
            if (szHostName[inLoop+1][0]!= 0)
                strcat((char *)szHostMenu, (char *)" \n");
        }
        else
            break;
    }

	vdDebug_LogPrintf("inHostindex=[%d]..........", inHostindex);
	if (inHostindex == 1)//only one app
	{
		vdCTOSS_DislplayMianAPMenu(IPC_EVENT_ID);
		return d_OK;
	}

    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szHostMenu, TRUE);

	if (key == 0xFF)
    {
        CTOS_LCDTClearDisplay();
        setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        vduiWarningSound();
        return -1;
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;

        vdDebug_LogPrintf("key=[%d],szHostName =[%s]", key, szHostName[key-1]);

		if (strcmp(szAPName, szHostName[key-1])==0)
		{
			vdCTOSS_DislplayMianAPMenu(IPC_EVENT_ID);
			return d_OK;
		}
		inTCTSave(1);

		inMultiAP_RunIPCCmdTypes(szHostName[key-1],IPC_EVENT_ID,"",0, outbuf,&out_len);

		inTCTRead(1);

		vdDebug_LogPrintf("end inCTOSS_MultiDislplayMenu............");

		if (outbuf[0] != IPC_EVENT_ID)
		{
			return d_NO;
		}

		if (outbuf[1] != d_SUCCESS)
		{
			return d_NO;
		}

	}
#else
    inMultiAP_RunIPCCmdTypes(szAPName, IPC_EVENT_ID,"",0, outbuf,&out_len);

    inTCTRead(1);

    vdDebug_LogPrintf("end inCTOSS_MultiDislplayMenu............");

    if (outbuf[0] != IPC_EVENT_ID)
        return d_NO;

    if (outbuf[1] != d_SUCCESS)
        return d_NO;
#endif
    return d_OK;
}


int inADLStart1;
int inADLEnd1;
int inADLStart2;
int inADLEnd2;
int inADLStart3;
int inADLEnd3;
int inADLTimeRangeUsed;
int inADLLimit;
int inMaxTime;


int inCTOS_ADC_Download(void){

    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, Menukey=0;
    int inRet;
    int key;
    USHORT shRet = 0,
            shLen = 0,
            iInitX = 0,
            shMinLen = 4,
            shMaxLen = 6;
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE + 1] = {0},
            szTitle[MAX_CHAR_PER_LINE + 1] = {0};

    CTOS_RTC rtcClock;
    CTOS_RTC SetRTC;
    BYTE szCurrTime[7] = {0};
    int inCurrTime = 0;

    int inAutoDLStart;
    int inAutoDLEnd;
    int inADLPerformed;

    int inADLTries;
    BYTE szCurrDate[8] = {0};

    BYTE szADLDate[8] = {0};



    if (get_env_int("ADLTYPE") != 2)
        return d_OK;

    if(inCheckBatchEmtpy() > 0)
        return d_NO;


    memset(szCurrDate, 0x00, sizeof(szCurrDate));
    CTOS_RTCGet(&SetRTC);
    sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);

    //inAutoDLTime = get_env_int("AUTODLTIME");

    memset(szADLDate, 0x00, sizeof(szADLDate));
    inCTOSS_GetEnvDB("ADLCURRDATE", szADLDate);

    vdDebug_LogPrintf("DATE %s %s %d", szADLDate, szCurrDate);

    if (strcmp(szADLDate,szCurrDate)!=0) {
        put_env_int("ADL",0);
        put_env_char("ADLCURRDATE",szCurrDate);
        put_env_int("ADLTRY1",0);
        put_env_int("ADLTRY2",0);
        put_env_int("ADLTRY3",0);

    }

    inADLPerformed = get_env_int("ADL");


    if (inADLPerformed == 1)
        return SUCCESS;


    vdDebug_LogPrintf("ADC  %d %d",  inADLStart1,inADLEnd1 );
    vdDebug_LogPrintf("ADC  %d %d",  inADLStart2,inADLEnd2 );
    vdDebug_LogPrintf("ADC  %d %d",  inADLStart3,inADLEnd3 );


    memset(szCurrTime, 0x00, sizeof(szCurrTime));
    CTOS_RTCGet(&rtcClock);
    sprintf(szCurrTime,"%02d%02d", rtcClock.bHour, rtcClock.bMinute);
    inCurrTime = wub_str_2_long(szCurrTime);


    if (inCurrTime >= inADLStart1 && inCurrTime <= inADLEnd1){
        inAutoDLStart = inADLStart1;
        inAutoDLEnd = inADLEnd1;
        inADLTimeRangeUsed = 1;
        vdDebug_LogPrintf("TIME1	%d %d",  inAutoDLStart,inAutoDLEnd );
    }else if (inCurrTime >= inADLStart2 && inCurrTime <= inADLEnd2){
        inAutoDLStart = inADLStart2;
        inAutoDLEnd = inADLEnd2;
        inADLTimeRangeUsed = 2;
        vdDebug_LogPrintf("TIME2	%d %d",  inAutoDLStart,inAutoDLEnd );
    }else if (inCurrTime >= inADLStart3 && inCurrTime <= inADLEnd3){
        inAutoDLStart = inADLStart3;
        inAutoDLEnd = inADLEnd3;
        inADLTimeRangeUsed = 3;
        vdDebug_LogPrintf("TIME3	%d %d",  inAutoDLStart,inAutoDLEnd );
    }else{
        return SUCCESS;
    }



    if (inADLTimeRangeUsed == 1){
        if (get_env_int("ADLTRY1") == 1)
            return SUCCESS;
    }else if (inADLTimeRangeUsed == 2){
        if (get_env_int("ADLTRY2") == 1)
            return SUCCESS;
    }else if (inADLTimeRangeUsed == 3){
        if (get_env_int("ADLTRY3") == 1)
            return SUCCESS;
    }

    vdDebug_LogPrintf("ADC %d %d %d", inCurrTime, inAutoDLStart,inAutoDLEnd );

    if (inCurrTime >= inAutoDLStart &&  inCurrTime <= inAutoDLEnd){
#if 0
        CTOS_LCDTClearDisplay();
    		vdCTOS_SetTransType(SETTLE);

    		CTOS_LCDTClearDisplay();

    		inRet = inCTOS_TEMPCheckAndSelectMutipleMID();
    		if(d_OK != inRet)
          	return inRet;
    		CTOS_LCDTClearDisplay();

		memset(szTitle, 0x00, sizeof(szTitle));
		strcpy(szTitle, "SETTLEMENT");
		iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2;
		memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
		memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
		memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle));
		CTOS_LCDTSetReverse(TRUE);
		CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
		CTOS_LCDTSetReverse(FALSE);

		inTCTRead(1);
		strTCT.fReprintSettleStatus=0;
		inTCTSave(1);


		inADL_SETTLE_ALL();
#else
        if ((inADLTimeRangeUsed+1 > inADLLimit)	|| (inCurrTime > inMaxTime))
            put_env_int("ADL",1);

        inCTOSS_ADLSettlementCheckTMSDownloadRequest();
#endif

    }
    return SUCCESS;


}


void vdSetADLParams(){

    CTOS_RTC rtcClock;
    CTOS_RTC SetRTC;
    BYTE szCurrTime[7] = {0};
    int inCurrTime = 0;

    BYTE szADLDate[8] = {0};
    BYTE szCurrDate[8] = {0};

    int inStart1;
    int inEnd1;
    int inStart2;
    int inEnd2;
    int inStart3;
    int inEnd3;


    memset(szCurrDate, 0x00, sizeof(szCurrDate));
    CTOS_RTCGet(&SetRTC);
    sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);

    inCTOSS_GetEnvDB("ADLCURRDATE", szADLDate);


    if ((strcmp(szADLDate,"")==0) || (strcmp(szADLDate,"000000")==0)){
        put_env_char("ADLCURRDATE",szCurrDate);

    }

    inADLStart1 = get_env_int("ADL1START");
    inADLEnd1 = get_env_int("ADL1END");


    inADLStart2= get_env_int("ADL2START");
    inADLEnd2= get_env_int("ADL2END");

    inMaxTime = 0;

    inADLStart3= get_env_int("ADL3START");
    inADLEnd3= get_env_int("ADL3END");

    if (inADLStart1 > 0 && inADLEnd1 > 0){
        inADLLimit= inADLLimit+1;
        inMaxTime = inADLEnd1;
    }

    if (inADLStart2 > 0 && inADLEnd2 > 0){
        inADLLimit= inADLLimit+1;
        inMaxTime = inADLEnd2;
    }

    if (inADLStart3 > 0 && inADLEnd3 > 0){
        inADLLimit= inADLLimit+1;
        inMaxTime = inADLEnd3;
    }

}

int inCheckIssuerforBINRoute(void)
{
    char szPAN[8+1] = {0};
    long inPAN = 0;

    if(srTransRec.byEntryMode == CARD_ENTRY_ICC)
    {
        if(srTransRec.IITid == 2 || srTransRec.IITid == 4 || srTransRec.IITid == 23 || srTransRec.IITid == 24)//Proceed to DCC if MasterCard or VISA
            return TRUE;
        else
            return FALSE;
    }

    memset(szPAN,0x00,8+1);
    memcpy(szPAN,srTransRec.szPAN,8);

    inPAN = atol(szPAN);
    vdDebug_LogPrintf("inCheckIssuerforBINRoute inPAN[%d]",inPAN);

    if((40000000 < inPAN && inPAN < 49999999) || (50000000 < inPAN && inPAN < 59999999) || (22210000 < inPAN && inPAN < 27209999))
        return TRUE;
    else
        return FALSE;

}

int inCTOS_SelectBinRouteHost(void)
{
    short shGroupId ;
    int inHostIndex;
    short shCommLink;
    int inCurrencyIdx=0;
    char szAscBuf[4 + 1], szBcdBuf[2 + 1];
    char szStr[16+1]={0};

    inDatabase_TerminalOpenDatabase();
    /* BDO: Make sure the ATP BIN Routing must use the first HDT- start -- jzg */
    //inHostIndex = 1;
    /* BDO: Make sure the ATP BIN Routing must use the first HDT - end -- jzg */

    if(fUSDSelected)
        strCDT.HDTid = 3;
    else
        strCDT.HDTid = 1;


    inHostIndex = strCDT.HDTid;

    vdDebug_LogPrintf("inCTOS_SelectBinRouteHost::Txn Type = [%d]", srTransRec.byTransType);
    vdDebug_LogPrintf("inCTOS_SelectBinRouteHost = [%d]", inHostIndex);

    if(inMultiAP_CheckSubAPStatus() == d_OK)
        return d_OK;
    vdDebug_LogPrintf("0.7 strCDT.HDTid [%d] ", strCDT.HDTid);
    if(inHDTRead(inHostIndex) != d_OK)
    {
        //vdSetErrorMessage("HOST SELECTION ERR");
        inHDTReadData(inHostIndex);
        strcpy(szStr,strHDT_Temp.szHostLabel);
        memset(strHDT_Temp.szHostLabel,0x00,sizeof(strHDT_Temp.szHostLabel));
        sprintf(strHDT_Temp.szHostLabel," %s ",szStr);
        vdDisplayErrorMsgResp2(strHDT_Temp.szHostLabel, "TRANSACTION", "NOT ALLOWED");
		
        return(d_NO);
    }
    else
    {
        srTransRec.HDTid = inHostIndex;

        inCurrencyIdx = strHDT.inCurrencyIdx;

        if(inCSTRead(inCurrencyIdx) != d_OK)
        {
            vdSetErrorMessage("LOAD CST ERR");
            return(d_NO);
        }

        if(strTCT.fSingleComms == TRUE)
            inHostIndex=1;
        if(inCPTRead(inHostIndex) != d_OK)
        {
            vdSetErrorMessage("LOAD CPT ERR");
            return(d_NO);
        }

        if (srTransRec.byEntryMode == CARD_ENTRY_WAVE)
            if (strCST.inCurrencyIndex > 1)
            {
                memset(szAscBuf, 0x00, sizeof(szAscBuf));
                memset(szBcdBuf, 0x00, sizeof(szBcdBuf));

                sprintf(szAscBuf,"%4s",strCST.szCurCode);

                wub_str_2_hex(szAscBuf, szBcdBuf, 4);
                memcpy((char *)srTransRec.stEMVinfo.T5F2A, &szBcdBuf[0], 2);
                DebugAddHEX("T5F2A..",srTransRec.stEMVinfo.T5F2A,2);
            }
        return (d_OK);
    }
}


void vdDispTitleAndCardProcessing(void)
{
    if(srTransRec.byTransType != SETTLE)
        vdDispTransTitleAndCardType(srTransRec.byTransType);
    else
        vdDispTransTitle(srTransRec.byTransType);

    //vdCTOS_DispStatusMessage("PROCESSING...");
    vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(100);
    //fAlreadyDisplayHeader=TRUE;
}

void vdTrimLeadZeroes (char *pchString)
{
    int inIndex, inLen = 0;

    inIndex = 0;
    while (pchString[inIndex] == '0')
        inIndex++;
    if (inIndex != 0)
    {
        inLen = strlen(pchString);
        strcpy(pchString, &(pchString[inIndex]));
        pchString[inLen - inIndex] = '\0';	// clean up remaining char
    }
}


int inCTOS_ByPassPIN(void)
{
    BYTE key = 0;
    int inRet = d_NO;
    char szDebug[41] = {0},
    szAscBuf[5] = {0}, 
    szBcdBuf[3] = {0};
    
    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04;
	BYTE Menukey = 0;

	vdDebug_LogPrintf("--inCTOS_ByPassPIN---");
	
	DisplayStatusLine(""); // clear window

    if (srTransRec.byEntryMode == CARD_ENTRY_ICC || srTransRec.byEntryMode == CARD_ENTRY_WAVE) 
    {
        srTransRec.fCUPPINEntry=FALSE; // CUP: Fix for double PIN entry -- jzg 
        return d_OK;
    }

    //CTOS_LCDTClearDisplay();
    memset(szHeaderString, 0x00, sizeof(szHeaderString));
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    
    strcpy(szHeaderString, "CARD WITH PIN?");
    strcat(szChoiceMsg,"YES \n");
    strcat(szChoiceMsg,"NO");
	
	//inSetColorMenuMode();
    //Menukey = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    //Menukey = MenuDisplayEx(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE, 30); /*Menu with timeout parameter*/
    Menukey = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE); /*Menu with timeout parameter*/
	vdDebug_LogPrintf("::inCTOS_SelectAccountType::MenuDisplayEx::Menukey[%d]", Menukey);
    inSetTextMode();
    
    if (Menukey > 0)
    {
        if(Menukey == 1)
        {
			srTransRec.fCUPPINEntry=TRUE;
            inRet = d_OK;
        }
        else if(Menukey == 2)
        {
		    srTransRec.fCUPPINEntry=FALSE;
			inRet = d_OK;
        }
        else if(Menukey == d_KBD_CANCEL)
        {
            vdSetErrorMessage("USER CANCEL");
        }
		else if (Menukey == 0xFF) /*BDO: For timeout occured -- sidumili*/
		{
			fTimeOutFlag = TRUE;	 /*BDO: Flag for timeout --sidumili*/
			inRet = d_NO;
		}
    }
    else
        inRet=d_NO;			

	return inRet;
}

int GetMenuTransTypes(void) {

    int inMenuNumRecs = 0,
            inCtr = 0,
            inMenu = 0,
            inKey = 0,
            inHeaderAttr = 0x01+0x04,
            inRetVal = 0;
    int inRet = d_NO;
    char szMenuItems[9000] = {0},
            szHeader[20] = {0};

	vdDebug_LogPrintf("--GetMenuTransTypes--");
	
    inMenu = 1;     //Credit

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inRetVat = [%d]", inRetVal);
    inRetVal = inFLGRead(1);
    vdDebug_LogPrintf("inFLGRead: inRetVal = [%d]", inRetVal);

GET_MENU:
    inMenuNumRecs = inReadDMTrx(inMenu);
    vdDebug_LogPrintf("inMenuNumRecs[%d]", inMenuNumRecs);

    memset(szMenuItems, 0, sizeof(szMenuItems));
    memset(szHeader, 0x00, sizeof(szHeader));

    strcpy(szHeader, " ");
    for(inCtr = 0; inCtr < inMenuNumRecs; ++inCtr)
    {
        strcat(szMenuItems, strDynamicMenu[inCtr].szButtonItemLabel);

        if (strlen(strDynamicMenu[inCtr + 1].szButtonItemLabel) > 0)
            strcat(szMenuItems, " \n");

    }

    vdDebug_LogPrintf("szMenuItems[%s]", szMenuItems);

    inKey = MenuDisplay(szHeader, strlen(szHeader), inHeaderAttr, 1, 1, szMenuItems, TRUE);

    vdDebug_LogPrintf("inKey[%d]", inKey);

    if(inKey == 0xFF)
    {
        //CTOS_LCDTClearDisplay();
        //setLCDPrint(1, DISPLAY_POSITION_CENTER, "WRONG INPUT!!!");
        //vduiWarningSound();
        //vdDisplayErrorMsg(1, 8, "TIME OUT");
        inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(1000);
        return d_NO;
    }
    else if(inKey == d_KBD_CANCEL)
    {
        //vdDisplayErrorMsg(1, 8, "USER CANCEL");
        inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(1000);
        return -1;
    }
    else
    {
        if(strlen(strDynamicMenu[inKey-1].szButtonItemFunc) > 0)
        {
            vdDebug_LogPrintf("strDynamicMenu.szButtonItemFunc[%s]", strDynamicMenu[inKey-1].szButtonItemFunc);
            inCTOSS_ExeFunction(strDynamicMenu[inKey-1].szButtonItemFunc);
            return d_OK;
        }
        else
        {
            vdDebug_LogPrintf("strDynamicMenu[inKey-1].szButtonItemLabel = [%s]", strDynamicMenu[inKey-1].szButtonItemLabel);
			if(strcmp(strDynamicMenu[inKey-1].szButtonItemLabel,"INSTALLMENT") == 0)
                inMenu = 2;
            else if (strcmp(strDynamicMenu[inKey-1].szButtonItemLabel,"HOTEL") == 0)
                inMenu = 3;
            else if (strcmp(strDynamicMenu[inKey-1].szButtonItemLabel,"REPRINT") == 0)
                inMenu = 4;
            else if (strcmp(strDynamicMenu[inKey-1].szButtonItemLabel,"REPORTS") == 0 || strcmp(strDynamicMenu[inKey-1].szButtonItemLabel,"REPORT") == 0)
                inMenu = 5;
            else if (strcmp(strDynamicMenu[inKey-1].szButtonItemLabel,"SETUP") == 0)
            {
                srTransRec.byTransType = SETUP;
                inRet = inCTOS_GetTxnPassword();
                if(d_OK != inRet)
                    return -1;

                inMenu = 6;
            }
			else if (strcmp(strDynamicMenu[inKey-1].szButtonItemLabel,"DETAIL") == 0)
                inMenu = 7;

			else if (strcmp(strDynamicMenu[inKey-1].szButtonItemLabel,"BDO REWARDS") == 0)
                inMenu = 8;
			
            vdDebug_LogPrintf("inMenu = [%d]", inMenu);
            goto GET_MENU;

        }
    }
}

int  inEditDatabase(void){

    int inRet;
    char cwd[1024];
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;


    srTransRec.byTransType = SETUP;

#if 0
    inRet = inCTOS_GetTxnPassword();
    if(d_OK != inRet)
        return inRet;
#endif

    vdDebug_LogPrintf("EDIT TABLE --  START");
    memcpy(szFuncTitleName,"EDIT TABLE",10);

    if(inFunctionKeyPassword("EDIT TABLE", SUPER_ENGINEER_PWD)==d_OK)
        usEditDatabase("edit");


#if 0
    //removed for testing

    inTCTRead(1);
    inCPTRead(1);
    inPCTRead(1);
    inCSTRead(1);
    inTCPRead(1);
	inTCTSave(1);
    inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_COM.SHARLS_COM", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);

    vdCTOS_TransEndReset();

#endif


    return d_OK;

}

BOOL fAmexBinRoute(void) /*amex chip and ctls will not go to bin routing*/
{
   DebugAddHEX("stEMVinfo.T84",srTransRec.stEMVinfo.T84,5);
   if((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x00\x25",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x00\x25",5) == 0) ||
   	(srTransRec.bWaveSID == d_VW_SID_AE_EMV)/*AMEX AID*/)
   {
   	vdDebug_LogPrintf("AMEX DISABLE BIN ROUTING");
   	return d_OK;
   }
	

   return FAIL;	
}

BOOL fDinersBinRoute(void) /*diners chip and ctls will not go to bin routing*/
{
   if ((memcmp(srTransRec.stEMVinfo.T84,"\xA0\x00\x00\x01\x52",5) == 0) || (memcmp(srTransRec.stEMVinfo.T84,"\xa0\x00\x00\x01\x52",5) == 0) ||
   	(srTransRec.bWaveSID == d_EMVCL_SID_DISCOVER_DPAS)/*DINERS AID*/)
   {
   		vdDebug_LogPrintf("DINERS DISABLE BIN ROUTING");
   		return d_OK;
   }

   return FAIL;	
}

int inCTOS_SelectCurrencyfromIDLE(void)
{
    BYTE key = 0;
    int inRet = 0;
    char szDebug[41] = {0},
    szAscBuf[5] = {0}, 
    szBcdBuf[3] = {0};
    
    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04;
	  BYTE Menukey = 0;
		int inCurrencyIdx;
		char szCurrDsp1[4+1], szCurrDsp2[4+1];
	
	fUSDSelected = FALSE;
    vdDebug_LogPrintf("inCTOS_SelectCurrencyfromIDLE[START]");
	
	
	
	if (inMultiAP_CheckSubAPStatus() == d_OK)
	   return d_OK;

    vdDebug_LogPrintf("fDualCurrency[%d]",strTCT.fDualCurrency);
    if (strTCT.fDualCurrency == VS_FALSE)
        return d_OK;
    
    //if (strCDT.fDualCurrencyEnable == VS_FALSE)
       // return d_OK;

    CTOS_LCDTClearDisplay();

	
  memset(szCurrDsp1,0,sizeof(szCurrDsp1));
  memset(szCurrDsp2,0,sizeof(szCurrDsp2));

	if (strTCT.fDefaultCashAdvance == 1)
		vdCTOS_SetTransType(CASH_ADVANCE);
	else
		vdCTOS_SetTransType(SALE);

	vdDispTransTitle(srTransRec.byTransType);
	
 	setLCDPrint27(3,DISPLAY_POSITION_LEFT,"SELECT CURRENCY");
	inCSTRead(1);
	sprintf(szCurrDsp1,"1.%4s", strCST.szCurSymbol);
	setLCDPrint27(4,DISPLAY_POSITION_LEFT,szCurrDsp1);
  	inCSTRead(2);
	sprintf(szCurrDsp2,"2.%4s", strCST.szCurSymbol);
	setLCDPrint27(5,DISPLAY_POSITION_LEFT,szCurrDsp2);

	inCurrencyIdx = strHDT.inCurrencyIdx;
	inCSTRead(inCurrencyIdx);

	while(1){
		
		Menukey=WaitKey(inGetIdleTimeOut(TRUE));

       if(Menukey == d_KBD_1)
        {
            Menukey = 1;
			break;
        }
        else if(Menukey == d_KBD_2)
        {
            Menukey = 2;
			break;
        }
        else if(Menukey == d_KBD_CANCEL)
        {
            Menukey = d_KBD_CANCEL;
			break;
        } 
		else if (Menukey == 0xFF) /*BDO: For timeout occured -- sidumili*/
		{
			fTimeOutFlag = TRUE;	/*BDO: Flag for timeout --sidumili*/
			Menukey = 0xFF;
			break;
		}

	}

 
     memset(szCurrDsp1,0,sizeof(szCurrDsp1));
     memset(szCurrDsp2,0,sizeof(szCurrDsp2));
 
    if(Menukey == 1)
    {
        vduiClearBelow(2);

		inCSTRead(1);
		sprintf(szCurrDsp1,"%4s SELECTED?", strCST.szCurSymbol);
        setLCDPrint27(4,DISPLAY_POSITION_CENTER,szCurrDsp1);
        
        inRet = d_OK;
    }
    else if(Menukey == 2)
    {
       	vduiClearBelow(2);

		inCSTRead(2);   
		sprintf(szCurrDsp2,"%4s SELECTED?", strCST.szCurSymbol);
        setLCDPrint27(4,DISPLAY_POSITION_CENTER,szCurrDsp2);
        inRet = d_OK; 
		fUSDSelected = TRUE;
    }
	else if (Menukey == 0xFF) /*BDO: For timeout occured -- sidumili*/
	{
		fTimeOutFlag = TRUE;	/*BDO: Flag for timeout --sidumili*/ 
		return(d_NO);
	}
    else if(Menukey == d_KBD_CANCEL)
    {
        return -1;
    }

	do
	{
		setLCDPrint27(8,DISPLAY_POSITION_CENTER,"ENTER TO CONFIRM");
		key=WaitKey(60);
		if (key ==d_KBD_ENTER)
		{			
			inRet = d_OK;	
			break;
		}
		else if (key == d_KBD_CANCEL)
		{
			inRet = d_NO;	
			break;
		}
		else
			vduiWarningSound();
	}
	while(1);

	return inRet;
}

void SetTransYear(void)
{
	CTOS_RTC SetRTC;
	char szYear[4 + 1];

	vdDebug_LogPrintf("--SetTransYear--");

	memset(szYear, 0x00, sizeof(szYear));
	CTOS_RTCGet(&SetRTC);
	sprintf(szYear, "%02lu", SetRTC.bYear);
	vdDebug_LogPrintf("szYear[%s]", szYear);

	memset(srTransRec.szYear, 0x00, sizeof(srTransRec.szYear));
	strcpy(srTransRec.szYear, szYear);

	vdDebug_LogPrintf("srTransRec.szYear[%s]", srTransRec.szYear);
}

#if 0
int inQRAppSelection(int inTransaction)
{
	BYTE key=0;
	BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
	int inHost = 0, inIndex = 0, inRet = 0, inCommand, i=0,inCount = 0;
	int inHDTid[10+1];
	int inHostvalue[6+1] = {0};
   // char szHeaderString[50] = "DIGITAL WALLET";
    char szHeaderString[50] = {0};
    char szHostMenu[500]={0};
	char szAppName[50];
	char szAmtBuff[AMT_ASC_SIZE+1];

	USHORT usNetworkType = 0;
	BYTE szNetworkName[128+1];
	
    static USHORT usEthType = 1;
	int iQRPAY = 0;

	vdDebug_LogPrintf("--inQRAppSelection--");
	vdDebug_LogPrintf("inTransaction=[%d]", inTransaction);
	vdDebug_LogPrintf("strTCT.inECRTrxnMenu=[%d]", strTCT.inECRTrxnMenu);

	vdCTOS_SetTransType(inTransaction);
	
	memset(szHeaderString, 0x00,sizeof(szHeaderString));
    memset(szHostMenu, 0x00,sizeof(szHostMenu));
	
    if (inTransaction == RETRIEVE){
		strcat(szHeaderString, "RETRIEVE");
    }else{
		strcat(szHeaderString, "MOBILE WALLET");
    }

	inCount = inHDTReadQRMenu("com.Source.S1_QRPAY.QRPAY",inHDTid);
		
	for(i=0; i < inCount; i++)
	{
		inHost = inHDTid[i];
		inHDTReadData(inHost);

		if((inHost == SODEXO_HDT_INDEX)  && (inTransaction == RETRIEVE )) //Sodexo not supported for Retrieve
			continue;

		if((inHost == BDOPAY_HDT_INDEX)  && (inTransaction == PAYMENT )) //BDOPAY not included in menu as per CR
			continue;

		vdDebug_LogPrintf("inHost=[%d], fHostEnable=[%d], szHostLabel=[%s]",inHost, strHDT_Temp.fHostEnable, strHDT_Temp.szHostLabel);
		if(strHDT_Temp.fHostEnable == TRUE)
		{
			if(szHostMenu[0] != NULL)
				strcat(szHostMenu," \n");

			strcat(szHostMenu,strHDT_Temp.szHostLabel);
			inHostvalue[inIndex] = inHost;
			inIndex++;
		}				
	}

	vdDebug_LogPrintf("inQRAppSelection, len[%d],szHeaderString=[%s]", strlen(szHeaderString),szHeaderString);
	vdDebug_LogPrintf("inQRAppSelection, len[%d],szHostMenu=[%s]", strlen(szHostMenu),szHostMenu);
	//inSetColorMenuMode();
    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, 1, 1, szHostMenu, TRUE);
    //inSetTextMode();
	
	if (key == 0xFF) 
    {
        CTOS_LCDTClearDisplay();
       //vduiWarningSound();
       	vdSetECRResponse(ECR_TIMEOUT_RESP);
		inRet = inMultiAP_ECRSendSuccessResponse();
        return FAIL;  
    }

	if(key == d_KBD_CANCEL)
    {
    	vdSetECRResponse(ECR_OPER_CANCEL_RESP);
		inRet = inMultiAP_ECRSendSuccessResponse();
        return FAIL;;
    } 
	
    if(key > 0)
    {
		//if (get_env_int("INVALIDKEY") == 1)
		//	return FAIL;
		
		if(67 == key)
			return -1;
        
		inHost = inHostvalue[key - 1];				
    }

   inHDTRead(inHost);
   vdDebug_LogPrintf("inTransaction=[%d], inHost=[%d], inCommand=[%d], strHDT.szHostLabel=[%s]", inTransaction, inHost, inCommand, strHDT.szHostLabel);
	
   if(inTransaction == PAYMENT)//PAYMENT
   {
   		switch(inHost)
		{
			case BDOPAY_HDT_INDEX:
				inCommand = d_IPC_CMD_BDOPAY_SALE;
				break;
			case WECHAT_HDT_INDEX:	
				inCommand = d_IPC_CMD_WECHAT_SALE;
				break;
			case ALIPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_ALIPAY_SALE;
				break;
			case GCASH_HDT_INDEX:	
				inCommand = d_IPC_CMD_GCASH_SALE;
				break;
			case GRABPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_GRABPAY_SALE;
				break;
			case SODEXO_HDT_INDEX:	
				inCommand = d_IPC_CMD_SODEXO_SALE;
				break;
			case PHQR_HDT_INDEX:
				inCommand = d_IPC_CMD_PHQR_SALE;
				break;	
   		}
   }
   else if(inTransaction == RETRIEVE)//RETRIEVE
   {
		switch(inHost)
		{
			case BDOPAY_HDT_INDEX:
				inCommand = d_IPC_CMD_BDOPAY_RETRIEVE;
				break;
			case WECHAT_HDT_INDEX:	
				inCommand = d_IPC_CMD_WECHAT_RETRIEVE;
				break;
			case ALIPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_ALIPAY_RETRIEVE;
				break;
			case GCASH_HDT_INDEX:	
				inCommand = d_IPC_CMD_GCASH_RETRIEVE;
				break;
			case GRABPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_GRABPAY_RETRIEVE;
				break;
			case PHQR_HDT_INDEX:
				inCommand = d_IPC_CMD_PHQR_RETRIEVE;
				break;	
			/*case SODEXO_HDT_INDEX:	//Sodexo not supported for Retrieve
				inCommand = d_IPC_CMD_SODEXO_RETRIEVE;
				break;
			*/
   		}
   }
  /*
   else//VOID
   {
		switch(inHost)
		{
			case BDOPAY_HDT_INDEX:
				inCommand = d_IPC_CMD_BDOPAY_VOID;
				break;
			case WECHAT_HDT_INDEX:	
				inCommand = d_IPC_CMD_WECHAT_VOID;
				break;
			case ALIPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_ALIPAY_VOID;
				break;
			case GCASH_HDT_INDEX:	
				inCommand = d_IPC_CMD_GCASH_VOID;
				break;
			case GRABPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_GRABPAY_VOID;
				break;
			case SODEXO_HDT_INDEX:	
				inCommand = d_IPC_CMD_SODEXO_VOID;
				break;
   		}
   }
  */

  if (inTransaction != RETRIEVE)
  {
  	inRet = inCTOS_GetTxnBaseAmount();
    if(d_OK != inRet)
	return inRet;		
  }
  
   memset(szAppName, 0x00, sizeof(szAppName));
   strcpy(szAppName, "com.Source.S1_QRPAY.QRPAY");

   vdDebug_LogPrintf("APPNAME %s COMMAND %d", szAppName, inCommand);

   wub_hex_2_str(srTransRec.szBaseAmount,szAmtBuff,AMT_BCD_SIZE);
   vdDebug_LogPrintf("DIGITAL WALLET AMOUT IS %s",szAmtBuff);
   inCTOSS_PutEnvDB("DIGIWALLETAMT",szAmtBuff);

   //fSharlsCommCrash = FALSE;

   if(strTCT.inECRTrxnMenu == 0){
   		memset(strHDT.szAPName, 0x00, sizeof(szAppName));
   		strcpy(strHDT.szAPName, "com.Source.S1_QRPAY.QRPAY");
   		inCTOS_MultiAPSaveData_Wallet(inCommand);
   }
   
   inRet = inCTOS_MultiSwitchApp(szAppName, inCommand);
   if(d_OK != inRet)
   {
	  vdDebug_LogPrintf("reserve app FAIL");
	  return inRet;
   }


    return d_OK;

}
#else
// modified -- sidumili
int inQRAppSelection(int inTransaction)
{
	int inHost = 0, inIndex = 0, inRet = 0, inCommand, i=0,inCount = 0;
	BYTE szAmount[40 + 1] = {0};
	BYTE szMessage[2000 + 1] = {0};
	char szAppName[50];
	char szAmtBuff[AMT_ASC_SIZE+1];
	int inHDTid[10+1];
	char szHostMenu[2000 + 1]={0};
	BYTE szImageFileName[110] = {0};
	BYTE szTemp[40 + 1] = {0};
	
	vdDebug_LogPrintf("--inQRAppSelection--");
	vdDebug_LogPrintf("inTransaction=[%d]", inTransaction);
	
	inCount = inHDTReadQRMenu("com.Source.S1_QRPAY.QRPAY",inHDTid);
		
	for(i=0; i < inCount; i++)
	{
		inHost = inHDTid[i];
		inHDTReadData(inHost);

		if((inHost == SODEXO_HDT_INDEX)  && (inTransaction == RETRIEVE )) //Sodexo not supported for Retrieve
			continue;

		//if((inHost == BDOPAY_HDT_INDEX)  && (inTransaction == PAYMENT )) //BDOPAY not included in menu as per CR
		//	continue;

		vdDebug_LogPrintf("inHost=[%d], fHostEnable=[%d], szHostLabel=[%s]",inHost, strHDT_Temp.fHostEnable, strHDT_Temp.szHostLabel);
		if(strHDT_Temp.fHostEnable == TRUE)
		{
			//if(szHostMenu[0] != NULL)
			//	strcat(szHostMenu,"#");

			
			memset(szTemp, 0x00, sizeof(szTemp));
			memset(szImageFileName, 0x00, sizeof(szImageFileName));
			strcpy(szTemp, strHDT_Temp.szHostLabel);
			SetToUpperCase((char*)szTemp);	
			getImageFileName(szTemp, szImageFileName);
			vdDebug_LogPrintf("inIndex=[%d],HostName=[%s],szImageFileName=[%s]", inIndex, strHDT_Temp.szHostLabel,szImageFileName);

			strcat(szHostMenu,strHDT_Temp.szHostLabel);
			strcat(szHostMenu, "#");
			strcat(szHostMenu, szImageFileName);
			strcat(szHostMenu, "|");
			
			inIndex++;
		}				
	}

	vdDebug_LogPrintf("szHostMenu=[%s]", szHostMenu);
	
	// Select wallet
	GetTransAmount(szAmount);
	memset(szMessage, 0x00, sizeof(szMessage));
	sprintf(szMessage, "%d|%d|%s|%s^", MENU_IMAGE_VIEW, MENU_2_COL_COUNT, (inTransaction == RETRIEVE ? "Retrieve" : "Mobile Wallet"), MSG_SELECT_WALLET);
	strcat(szMessage, szHostMenu);
	
	inRet = inMenuSelection(szMessage);
	vdDebug_LogPrintf("inMenuSelection, inRet=[%d]", inRet);
	
	if (inRet == 0xFF || inRet == d_TIMEOUT) //TimeOut
    {
	   CTOS_Beep();
	   return d_NO;
    }

	if (inRet == d_KBD_CANCEL || inRet == d_USER_CANCEL)
    {
		CTOS_Beep();
        return d_NO;
    }

	if (0 == strcmp(strGBLVar.szGBLvProcessDesc, "BDO PAY"))
		inHost = BDOPAY_HDT_INDEX;
	else if (0 == strcmp(strGBLVar.szGBLvProcessDesc, "GCASHCREDITGIVES") || 0 == strcmp(strGBLVar.szGBLvProcessDesc, "GCashCreditGives"))
		inHost = GCASH_HDT_INDEX;
	else if (0 == strcmp(strGBLVar.szGBLvProcessDesc, "ALIPAY"))
		inHost = ALIPAY_HDT_INDEX;
	else if (0 == strcmp(strGBLVar.szGBLvProcessDesc, "WECHAT"))
		inHost = WECHAT_HDT_INDEX;
	else if (0 == strcmp(strGBLVar.szGBLvProcessDesc, "GRABPAY"))
		inHost = GRABPAY_HDT_INDEX;
	else if (0 == strcmp(strGBLVar.szGBLvProcessDesc, "QRPH"))
		inHost = PHQR_HDT_INDEX;

	inHDTRead(inHost);

	fAmountEntered = FALSE;
	srTransRec.byTransType = PAYMENT;
	
	strCPT.HDTid = inHost; 
	strHDT.inCurrencyIdx = inHost;
	strCPT.HDTid = inHost;

	if (inTransaction != RETRIEVE)
	{
		inRet = inCTOS_GetTxnBaseAmount();
		if (inRet != d_OK)
			return d_NO;
	}
	
    vdDebug_LogPrintf("inTransaction=[%d], inHost=[%d], strHDT.szHostLabel=[%s]", inTransaction, inHost, strHDT.szHostLabel);

	if(inTransaction == PAYMENT)//PAYMENT
    {
   		switch(inHost)
		{
			case BDOPAY_HDT_INDEX:
				inCommand = d_IPC_CMD_BDOPAY_SALE;
				break;
			case WECHAT_HDT_INDEX:	
				inCommand = d_IPC_CMD_WECHAT_SALE;
				break;
			case ALIPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_ALIPAY_SALE;
				break;
			case GCASH_HDT_INDEX:	
				inCommand = d_IPC_CMD_GCASH_SALE;
				break;
			case GRABPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_GRABPAY_SALE;
				break;
			case SODEXO_HDT_INDEX:	
				inCommand = d_IPC_CMD_SODEXO_SALE;
				break;
			case PHQR_HDT_INDEX:
				inCommand = d_IPC_CMD_PHQR_SALE;
				break;	
   		}
    }
    else if(inTransaction == RETRIEVE)//RETRIEVE
    {
		switch(inHost)
		{
			case BDOPAY_HDT_INDEX:
				inCommand = d_IPC_CMD_BDOPAY_RETRIEVE;
				break;
			case WECHAT_HDT_INDEX:	
				inCommand = d_IPC_CMD_WECHAT_RETRIEVE;
				break;
			case ALIPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_ALIPAY_RETRIEVE;
				break;
			case GCASH_HDT_INDEX:	
				inCommand = d_IPC_CMD_GCASH_RETRIEVE;
				break;
			case GRABPAY_HDT_INDEX:	
				inCommand = d_IPC_CMD_GRABPAY_RETRIEVE;
				break;
			case PHQR_HDT_INDEX:
				inCommand = d_IPC_CMD_PHQR_RETRIEVE;
				break;	
			/*case SODEXO_HDT_INDEX:	//Sodexo not supported for Retrieve
				inCommand = d_IPC_CMD_SODEXO_RETRIEVE;
				break;
			*/
   		}
    }

	vdDebug_LogPrintf("inCommand=[%d]", inCommand);

    memset(szAppName, 0x00, sizeof(szAppName));
    strcpy(szAppName, "com.Source.S1_QRPAY.QRPAY");

    vdDebug_LogPrintf("APPNAME %s COMMAND %d", szAppName, inCommand);

	wub_hex_2_str(srTransRec.szBaseAmount,szAmtBuff,AMT_BCD_SIZE);
    vdDebug_LogPrintf("DIGITAL WALLET AMOUT IS %s",szAmtBuff);
    inCTOSS_PutEnvDB("DIGIWALLETAMT",szAmtBuff);
   
    if(strTCT.inECRTrxnMenu == 0){
   		memset(strHDT.szAPName, 0x00, sizeof(szAppName));
   		strcpy(strHDT.szAPName, "com.Source.S1_QRPAY.QRPAY");
   		inCTOS_MultiAPSaveData_Wallet(inCommand);
    }
   
    inRet = inCTOS_MultiSwitchApp(szAppName, inCommand);
    if(d_OK != inRet)
    {
	   vdDebug_LogPrintf("reserve app FAIL");
	  return inRet;
    }
   
	return d_OK;
}
#endif

int vdCTOS_EditHost(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0, inRet;
    BYTE inKey = d_NO;
    BYTE szOutputBuf[1024] = {0};
    BYTE szTemp[40 + 1] = {0};
    USHORT inLenMID =0;

	vdDebug_LogPrintf("--vdCTOS_EditHost--");

	EditAgain:
    shHostIndex = inCTOS_SelectHostSetting();
	vdDebug_LogPrintf("inCTOS_SelectHostSetting, shHostIndex=[%d]", shHostIndex);
    if (shHostIndex == -1)
        return -1;
	else if (shHostIndex == -2)
        return -2;

	//CTOS_LCDTClearDisplay();

    shHostIndex=strHDT.inHostIndex;

	vdDebug_LogPrintf("shHostIndex=[%d]", shHostIndex);

    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return -1;

	if(inCTOS_ChkBatchNotEmpty() != d_NO)
	{	
		//vdDisplayMessageBox(1, 8, "EDIT NOT ALLOWED", "BATCH NOT EMPTY", "", MSG_TYPE_ERROR);
		inDisplayMessageBoxWithButtonConfirmation(1,8,"Edit not allowed","Batch not empty","","",MSG_TYPE_ERROR, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(2000);
	    return -1;
	}

	vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(100);

	inTCTRead(1);
    inHDTRead(shHostIndex);
	inEFTPubRead(shHostIndex); // Read EFTSec

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Enable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fHostEnable", "Host Enable?", strHDT.fHostEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// TID
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTID", "Terminal ID", strMMT[0].szTID, 8, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/// MID
	if ((strHDT.inHostIndex==WECHAT_HDT_INDEX) || (strHDT.inHostIndex==ALIPAY_HDT_INDEX))
		inLenMID = 12;
	else if (strHDT.inHostIndex==PHQR_HDT_INDEX)
		inLenMID = 10;
	else
		inLenMID = 15;
	
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szMID", "Merchant ID", strMMT[0].szMID, inLenMID, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	           
	// TPDU
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strHDT.szTPDU,szTempBuf,5);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTPDU", "TPDU", szTempBuf, 10, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// NII
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strHDT.szNII,szTempBuf,2);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szNII", "NII", szTempBuf, 4, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Super PWD
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSuperPW", "Super PWD", strTCT.szSuperPW, PWD_LEN, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// System PWD
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSystemPW", "System PWD", strTCT.szSystemPW, PWD_LEN, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Engineer PWD
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szEngineerPW", "Engineer PWD", strTCT.szEngineerPW, PWD_LEN, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szPinPW
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szPinPW", "Pin Nomination PWD", strTCT.szPinPW, PWD_LEN, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");		

	// Batch No
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strMMT[0].szBatchNo,szTempBuf,3);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szBatchNo", "Batch No", szTempBuf, BATCH_NO_ASC_SIZE, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Trace No
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strHDT.szTraceNo,szTempBuf,3);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTraceNo", "Trace No", szTempBuf, BATCH_NO_ASC_SIZE, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Invoice No
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strTCT.szInvoiceNo,szTempBuf,3);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szInvoiceNo", "Invoice No", szTempBuf, BATCH_NO_ASC_SIZE, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintBankReceipt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintBankReceipt", "Print Bank Copy?", strHDT.fPrintBankReceipt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintCustReceipt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintCustReceipt", "Print Cust Copy?", strHDT.fPrintCustReceipt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintMercReceipt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintMercReceipt", "Print Merch Copy?", strHDT.fPrintMercReceipt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintVoidBankReceipt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintVoidBankReceipt", "Print Bank Void Copy?", strHDT.fPrintVoidBankReceipt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintVoidCustReceipt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintVoidCustReceipt", "Print Cust Void Copy?", strHDT.fPrintVoidCustReceipt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintVoidMercReceipt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintVoidMercReceipt", "Print Merch Void Copy?", strHDT.fPrintVoidMercReceipt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inEFTEnable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inEFTEnable", "EFTSec Enable?", strEFTPub.inEFTEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szDetailReportMask
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szDetailReportMask", "Detail Report Mask Format", strHDT.szDetailReportMask, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintFooterLogo
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintFooterLogo", "Print Footer Logo?", strHDT.fPrintFooterLogo, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Dummy
	//memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	//sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d", FLD_TYPE_NUMERIC, "szDummy", "Dummy Field", "Dummy", BATCH_NO_ASC_SIZE, FALSE);
	//strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	//strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("HOST SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("AAA - szOutputBuf[%s]", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
			//GetDelimitedString(szOutputBuf, 2, '|', ); // inHostIndex
			//GetDelimitedString(szOutputBuf, 3, '|', strHDT.szHostLabel); // szHostLabel

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 4, '|', szTemp); // fHostEnable
			strHDT.fHostEnable = (atoi(szTemp) > 0 ? TRUE: FALSE);
			
			GetDelimitedString(szOutputBuf, 5, '|', strMMT[0].szTID); // szTID
			
			GetDelimitedString(szOutputBuf, 6, '|', strMMT[0].szMID); // szMID

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 7, '|', szTemp); // szTPDU
			inAscii2Bcd(szTemp, strHDT.szTPDU, 5);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 8, '|', szTemp); // szNII
			inAscii2Bcd(szTemp, strHDT.szNII, 2);

			GetDelimitedString(szOutputBuf, 9, '|', strTCT.szSuperPW); // szSuperPW
			
			GetDelimitedString(szOutputBuf, 10, '|', strTCT.szSystemPW); // szSystemPW
			
			GetDelimitedString(szOutputBuf, 11, '|', strTCT.szEngineerPW); // szEngineerPW

			GetDelimitedString(szOutputBuf, 12, '|', strTCT.szPinPW); // szPinPW
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 13, '|', szTemp); // szBatchNo
			inAscii2Bcd(szTemp, strMMT[0].szBatchNo, 3);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 14, '|', szTemp); // szTraceNo
			inAscii2Bcd(szTemp, strHDT.szTraceNo, 3);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 15, '|', szTemp); // szInvoiceNo
			inAscii2Bcd(szTemp, strTCT.szInvoiceNo, 3);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 16, '|', szTemp); // fPrintBankReceipt
			strHDT.fPrintBankReceipt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 17, '|', szTemp); // fPrintCustReceipt
			strHDT.fPrintCustReceipt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 18, '|', szTemp); // fPrintMercReceipt
			strHDT.fPrintMercReceipt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 19, '|', szTemp); // fPrintVoidBankReceipt
			strHDT.fPrintVoidBankReceipt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 20, '|', szTemp); // fPrintVoidCustReceipt
			strHDT.fPrintVoidCustReceipt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 21, '|', szTemp); // fPrintVoidMercReceipt
			strHDT.fPrintVoidMercReceipt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 22, '|', szTemp); // inEFTEnable
			strEFTPub.inEFTEnable = atoi(szTemp);

			GetDelimitedString(szOutputBuf, 23, '|', strHDT.szDetailReportMask); // szDetailReportMask

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 24, '|', szTemp); // fPrintFooterLogo
			strHDT.fPrintFooterLogo= (atoi(szTemp) > 0 ? TRUE: FALSE);

			// Update MMT
			inMMTSave(strMMT[0].MMTid);

			// Update TCT
			inTCTSave(1);

			// Update HDT
			inHDTUpdate(strHDT.inHostIndex);

			// Update EFT
			inEFTPubSave(strHDT.inHostIndex);
		
			//vdDisplayMessageBox(1, 8, strHDT.szHostLabel, "HOST SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			inDisplayMessageBoxWithButtonConfirmation(1,8,"",strHDT.szHostLabel,"Host setting","Update complete",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case 0xFF:
			//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	
	//goto EditAgain;
	return d_OK;
	
}

int vdCTOS_EditIP(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0, inRet;
	BYTE inKey = d_NO;
	BYTE szOutputBuf[512] = {0};
	BYTE szTemp[40 + 1] = {0};

	vdDebug_LogPrintf("--vdCTOS_EditIP--");

	EditAgain:
    shHostIndex = inCTOS_SelectHostSetting();
	vdDebug_LogPrintf("inCTOS_SelectHostSetting, shHostIndex=[%d]", shHostIndex);
    if (shHostIndex == -1)
        return -1;
	else if (shHostIndex == -2)
        return -2;

	CTOS_LCDTClearDisplay();

    shHostIndex=strHDT.inHostIndex;
	
	//tine:  no need to check multiple MID for EditIP
    //inResult = inCTOS_CheckAndSelectMutipleMID();
    //if(d_OK != inResult)
    //    return -1;

	if(inCTOS_ChkBatchNotEmpty() != d_NO)
	{	
		//vdDisplayMessageBox(1, 8, "EDIT NOT ALLOWED", "BATCH NOT EMPTY", "", MSG_TYPE_ERROR);
		inDisplayMessageBoxWithButtonConfirmation(1,8,"Edit not allowed","Batch not empty","","",MSG_TYPE_ERROR, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(2000);
	    return -1;
	}

	vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(100);

	inTCTRead(1);
    inHDTRead(shHostIndex);
	inCPTRead(shHostIndex);
	inTCPRead(1);

	vdDeleteFile(EDIT_FILE); // Delete file
	
	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// inCommunicationMode
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inCommunicationMode", "Connection Type (1-LAN/2-GPRS/4-WIFI)", strCPT.inCommunicationMode, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inIPHeader
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inIPHeader", "IP Header", strCPT.inIPHeader, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szPriTxnHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szPriTxnHostIP", "Primary Host IP", strCPT.szPriTxnHostIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inPriTxnHostPortNum
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inPriTxnHostPortNum", "Primary Host Port", strCPT.inPriTxnHostPortNum, 6, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szSecTxnHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSecTxnHostIP", "Secondary Host IP", strCPT.szSecTxnHostIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inSecTxnHostPortNum
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inSecTxnHostPortNum", "Secondary Host Port", strCPT.inSecTxnHostPortNum, 6, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szPriSettlementHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szPriSettlementHostIP", "Settle Primary Host IP", strCPT.szPriSettlementHostIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inPriSettlementHostPort
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inPriSettlementHostPort", "Settle Primary Host Port", strCPT.inPriSettlementHostPort, 6, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szSecSettlementHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSecSettlementHostIP", "Settle Secondary Host IP", strCPT.szSecSettlementHostIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inSecSettlementHostPort
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inSecSettlementHostPort", "Settle Secondary Host Port", strCPT.inSecSettlementHostPort, 6, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fDHCPEnable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fDHCPEnable", "DHCP?[0-Static/1-DHCP]", strTCP.fDHCPEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");		

	// szTerminalIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTerminalIP", "Terminal IP", strTCP.szTerminalIP, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szSubNetMask
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSubNetMask", "SubNet Mask IP", strTCP.szSubNetMask, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szGetWay
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szGetWay", "Gateway IP", strTCP.szGetWay, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szHostDNS1
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostDNS1", "DNS1 IP", strTCP.szHostDNS1, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szHostDNS2
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostDNS2", "DNS2 IP", strTCP.szHostDNS2, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szAPN
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szAPN", "GPRS APN", strTCP.szAPN, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szUserName
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szUserName", "GPRS Username", strTCP.szUserName, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szPassword
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szPassword", "GPRS Password", strTCP.szPassword, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szWifiSSID
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szWifiSSID", "WIFI SSid", strTCP.szWifiSSID, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szWifiPassword
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szWifiPassword", "WIFI Password", strTCP.szWifiPassword, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szWifiProtocal
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szWifiProtocal", "WIFI Protocol", strTCP.szWifiProtocal, 8, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szWifiPairwise
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szWifiPairwise", "WIFI Pairwise", strTCP.szWifiPairwise, 8, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szWifiGroup
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szWifiGroup", "WIFI Group", strTCP.szWifiGroup, 8, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fSSLEnable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fSSLEnable", "SSL?[0-Disable/1-Enable]", strCPT.fSSLEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");	
	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("IP SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
			//GetDelimitedString(szOutputBuf, 2, '|', ); // inHostIndex
			//GetDelimitedString(szOutputBuf, 3, '|', strHDT.szHostLabel); // szHostLabel
						
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 4, '|', szTemp); // inCommunicationMode
			strCPT.inCommunicationMode = atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 5, '|', szTemp); // inIPHeader
			strCPT.inIPHeader= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 6, '|', strCPT.szPriTxnHostIP); // szPriTxnHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 7, '|', szTemp); // inPriTxnHostPortNum
			strCPT.inPriTxnHostPortNum= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 8, '|', strCPT.szSecTxnHostIP); // szSecTxnHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 9, '|', szTemp); // inSecTxnHostPortNum
			strCPT.inSecTxnHostPortNum= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 10, '|', strCPT.szPriSettlementHostIP); // szPriSettlementHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 11, '|', szTemp); // inPriSettlementHostPort
			strCPT.inPriSettlementHostPort= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 12, '|', strCPT.szSecSettlementHostIP); // szSecSettlementHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 13, '|', szTemp); // inSecSettlementHostPort
			strCPT.inSecSettlementHostPort= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 14, '|', szTemp); // fDHCPEnable
			strTCP.fDHCPEnable= (atoi(szTemp) > 0 ? TRUE: FALSE);

			GetDelimitedString(szOutputBuf, 15, '|', strTCP.szTerminalIP); // szTerminalIP
			GetDelimitedString(szOutputBuf, 16, '|', strTCP.szSubNetMask); // szSubNetMask
			GetDelimitedString(szOutputBuf, 17, '|', strTCP.szGetWay); // szGetWay
			GetDelimitedString(szOutputBuf, 18, '|', strTCP.szHostDNS1); // szHostDNS1
			GetDelimitedString(szOutputBuf, 19, '|', strTCP.szHostDNS2); // szHostDNS2

			GetDelimitedString(szOutputBuf, 20, '|', strTCP.szAPN); // szAPN
			GetDelimitedString(szOutputBuf, 21, '|', strTCP.szUserName); // szUserName
			GetDelimitedString(szOutputBuf, 22, '|', strTCP.szPassword); // szPassword
			GetDelimitedString(szOutputBuf, 23, '|', strTCP.szWifiSSID); // szWifiSSID
			GetDelimitedString(szOutputBuf, 24, '|', strTCP.szWifiPassword); // szWifiPassword
			GetDelimitedString(szOutputBuf, 25, '|', strTCP.szWifiProtocal); // szWifiProtocal
			GetDelimitedString(szOutputBuf, 26, '|', strTCP.szWifiPairwise); // szWifiPairwise
			GetDelimitedString(szOutputBuf, 27, '|', strTCP.szWifiGroup); // szWifiGroup

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 28, '|', szTemp); // fSSLEnable
			strCPT.fSSLEnable= (atoi(szTemp) > 0 ? TRUE: FALSE);

			// Update CPT
			inCPTSave(shHostIndex);

			// Update TCP
			inTCPSave(1);
		
			//vdDisplayMessageBox(1, 8, strHDT.szHostLabel, "IP SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			inDisplayMessageBoxWithButtonConfirmation(1,8,strHDT.szHostLabel,"IP setting","Update complete","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case 0xFF:
			//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	
	//goto EditAgain;
	return d_OK;
	
}

int vdCTOS_EditTerminal(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0, inRet;
	BYTE inKey = d_NO;
	BYTE szOutputBuf[1024] = {0};
	BYTE szTemp[40 + 1] = {0};
	int inTemp = 0;
    
	//CTOS_LCDTClearDisplay();
	
	shHostIndex = 1;
	inHDTRead(shHostIndex);
	inCPTRead(strHDT.inHostIndex);

	inResult = inCTOS_CheckAndSelectMutipleMID();
	if(d_OK != inResult)
	return -1;

    if(inCTOS_ChkBatchNotEmpty() != d_NO)
	{	
		//vdDisplayMessageBox(1, 8, "EDIT NOT ALLOWED", "BATCH NOT EMPTY", "", MSG_TYPE_ERROR);
		inDisplayMessageBoxWithButtonConfirmation(1,8,"Edit not allowed","Batch not empty","","",MSG_TYPE_ERROR, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(2000);
	    return -1;
	}

	vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(100);
	
	inTCTRead(1);
    inHDTRead(1);
	inFLGRead(1);
	inPASRead(1);

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// byTerminalType
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byTerminalType", "Terminal Type", strTCT.byTerminalType, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// byTerminalModel
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byTerminalModel", "Terminal Model", strTCT.byTerminalModel, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// inMenuid
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inMenuid", "Menu ID", strTCT.inMenuid, 3, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// fPrintISOMessage
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintISOMessage", "Print ISO?", strTCT.fPrintISOMessage, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inPrintISOOption
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inPrintISOOption", "ISO Parse Option", strTCT.inPrintISOOption, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fEnablePrintBalanceInq
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fEnablePrintBalanceInq", "Print Balance Inq?", strTCT.fEnablePrintBalanceInq, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fSignatureFlag
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fSignatureFlag", "Signature Enable?", strTCT.fSignatureFlag, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fDemo
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fDemo", "Demo?", strTCT.fDemo, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// byERMMode
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byERMMode", "ERM Mode?", strTCT.byERMMode, 3, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// byPinPadType
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byPinPadType", "Pinpad Type?", strTCT.byPinPadType, 3, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/*szPrintOption*/
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szPrintOption", "Print Receipt Sequence", strTCT.szPrintOption, 3, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// byRS232DebugPort
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byRS232DebugPort", "Debug Port", strTCT.byRS232DebugPort, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fChangeDateMustSettle
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fChangeDateMustSettle", "Change Date Must Settle?", strTCT.fChangeDateMustSettle, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fShareComEnable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fShareComEnable", "ShareCOM?", strTCT.fShareComEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szInvoiceNo
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strTCT.szInvoiceNo,szTempBuf,3);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szInvoiceNo", "Invoice No", szTempBuf, BATCH_NO_ASC_SIZE, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// feReceiptEnable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "feReceiptEnable", "Receipt Enable?", strTCT.feReceiptEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintReceiptPrompt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "feReceiptEnable", "Receipt Confirmation?", strTCT.fPrintReceiptPrompt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fTipAllowFlag
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fTipAllowFlag", "Tip Allow?", strTCT.fTipAllowFlag, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fATPBinRoute
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fATPBinRoute", "ATP Bin Route?", strTCT.fATPBinRoute, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// ATPTPDU
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strTCT.ATPTPDU,szTempBuf,5);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "ATPTPDU", "ATPTPDU", szTempBuf, 10, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// ATPNII
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	memset(szTempBuf, 0x00, sizeof(szTempBuf));
	wub_hex_2_str(strTCT.ATPNII,szTempBuf,2);
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "ATPNII", "ATPNII", szTempBuf, 4, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fDCC
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fDCC", "DCC Enable?", strTCT.fDCC, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fCheckout
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fCheckout", "Checkout?", strTCT.fCheckout, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fGrabpayRetieveLast
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fGrabpayRetieveLast", "GrabPay Retrieve Last?", strFLG.fGrabpayRetieveLast, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fNextDayMustSettle
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fNextDayMustSettle", "Next Day Must Settle?", strTCT.fNextDayMustSettle, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fEnable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fEnable", "Auto Settle Enable?", strPAS.fEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/*szSTLTime1*/
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSTLTime1", "Auto Settle Time1", strPAS.szSTLTime1, 8, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/*szSTLTime2*/
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSTLTime2", "Auto Settle Time2", strPAS.szSTLTime2, 8, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/*szSTLTime3*/
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSTLTime3", "Auto Settle Time3", strPAS.szSTLTime3, 8, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fOnGoing
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fOnGoing", "Auto Settle OnGoing?", strPAS.fOnGoing, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	/*szWaitTime*/
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szWaitTime", "Auto Settle Wait Time", strPAS.szWaitTime, 8, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inRetryTime
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inRetryTime", "Auto Settle Retry", strPAS.inRetryTime, 2, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fFirstBootUp
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fFirstBootUp", "Auto Settle BootUp?", strPAS.fFirstBootUp, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fSMMode
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fSMMode", "SM Mode?", strTCT.fSMMode, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fSMReceipt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fSMReceipt", "SM Receipt?", strTCT.fSMReceipt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	//fEnableBalInqMKE
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fEnableBalInqMKE", "MKE Balance Inq?", strTCT.fEnableBalInqMKE, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	//fDisplayAPPROVED
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fDisplayAPPROVED", "Display Approved?", strTCT.fDisplayAPPROVED, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	//fSingleComms
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fSingleComms", "Single Comms?", strTCT.fSingleComms, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inECRTrxnMenu
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inECRTrxnMenu", "ECR Trans Menu", strTCT.inECRTrxnMenu, 2, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// XMLPRINT
	inTemp = get_env_int("XMLPRINT");
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "szTemp", "XMLDebug?", inTemp, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// XMLDBG
	inTemp = get_env_int("XMLDBG");
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "szTemp", "XMLPrint?", inTemp, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fSMLogo
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fSMLogo", "SM Logo?", strTCT.fSMLogo, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/*szSMReceiptLogo*/
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSMReceiptLogo", "SM Receipt Logo", strTCT.szSMReceiptLogo, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/*szBDOReceiptLogo*/
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szBDOReceiptLogo", "BDO Receipt Logo", strTCT.szBDOReceiptLogo, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fLargeInvoice
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fLargeInvoice", "Large Invoice?", strFLG.fLargeInvoice, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fDutyFreeMode
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fDutyFreeMode", "Duty Free Mode?", strTCT.fDutyFreeMode, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fAutoDownloadEnable
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fAutoDownloadEnable", "ADC Enable?", strTCT.fAutoDownloadEnable, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// byCTMSCallAfterSettle
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byCTMSCallAfterSettle", "ADC After Settle?", strTCT.byCTMSCallAfterSettle, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fPrintTipReceipt
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintTipReceipt", "Print Tip Receipt?", strTCT.fPrintTipReceipt, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	//fPrintFooterLogo
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fPrintFooterLogo", "Print Footer Logo?", strTCT.fPrintFooterLogo, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inMinBattery
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inMinBattery", "Minimum Battery", strTCT.inMinBattery, 2, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fEnableManualKeyEntry
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fEnableManualKeyEntry", "Enable Manual Entry?", strTCT.fEnableManualKeyEntry, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fHotelSetup
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fHotelSetup", "Hotel Setup?", strTCT.fHotelSetup, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inPrinterHeatLevel
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inPrinterHeatLevel", "Printer Heat Level", strTCT.inPrinterHeatLevel, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inWaitTime
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inWaitTime", "Wait Time", strTCT.inWaitTime, 3, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fForExRate
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fForExRate", "Display Exchange Rate?", strFLG.fForExRate, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fDCCMarkUp
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fDCCMarkUp", "Display MarkUp?", strFLG.fDCCMarkUp, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inDCCMode
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inDCCMode", "DCC Mode?", strTCT.inDCCMode, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Dummy
	//memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	//sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d", FLD_TYPE_NUMERIC, "szDummy", "Dummy Field", "Dummy", BATCH_NO_ASC_SIZE, FALSE);
	//strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	//strcat(strWriteFile.szWriteData, "\n");
	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("TERMINAL SETTING|x", szOutputBuf);	
	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case d_OK:

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 4, '|', szTemp); // byTerminalType
			strTCT.byTerminalType= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 5, '|', szTemp); // byTerminalModel
			strTCT.byTerminalModel= atoi(szTemp);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 6, '|', szTemp); // inMenuid
			strTCT.inMenuid= atoi(szTemp);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 7, '|', szTemp); // fPrintISOMessage
			strTCT.fPrintISOMessage= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 8, '|', szTemp); // inPrintISOOption
			strTCT.inPrintISOOption= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 9, '|', szTemp); // fEnablePrintBalanceInq
			strTCT.fEnablePrintBalanceInq= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 10, '|', szTemp); // fSignatureFlag
			strTCT.fSignatureFlag= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 11, '|', szTemp); // fDemo
			strTCT.fDemo= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 12, '|', szTemp); // byERMMode
			strTCT.byERMMode= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 13, '|', szTemp); // byPinPadType
			strTCT.byPinPadType= atoi(szTemp);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 14, '|', szTemp); // szPrintOption
			strcpy(strTCT.szPrintOption, szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 15, '|', szTemp); // byRS232DebugPort
			strTCT.byRS232DebugPort= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 16, '|', szTemp); // fChangeDateMustSettle
			strTCT.fChangeDateMustSettle= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 17, '|', szTemp); // fShareComEnable
			strTCT.fShareComEnable= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 18, '|', szTemp); // szInvoiceNo
			inAscii2Bcd(szTemp, strTCT.szInvoiceNo, 3);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 19, '|', szTemp); // feReceiptEnable
			strTCT.feReceiptEnable= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 20, '|', szTemp); // fPrintReceiptPrompt
			strTCT.fPrintReceiptPrompt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 21, '|', szTemp); // fTipAllowFlag
			strTCT.fTipAllowFlag= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 22, '|', szTemp); // fATPBinRoute
			strTCT.fATPBinRoute= (atoi(szTemp) > 0 ? TRUE: FALSE);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 23, '|', szTemp); // ATPTPDU
			inAscii2Bcd(szTemp, strTCT.ATPTPDU, 5);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 24, '|', szTemp); // ATPNII
			inAscii2Bcd(szTemp, strTCT.ATPNII, 2);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 25, '|', szTemp); // fDCC
			strTCT.fDCC= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 26, '|', szTemp); // fCheckout
			strTCT.fCheckout= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 27, '|', szTemp); // fGrabpayRetieveLast
			strFLG.fGrabpayRetieveLast= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 28, '|', szTemp); // fNextDayMustSettle
			strTCT.fNextDayMustSettle= (atoi(szTemp) > 0 ? TRUE: FALSE);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 29, '|', szTemp); // fEnable
			strPAS.fEnable= (atoi(szTemp) > 0 ? TRUE: FALSE);

			GetDelimitedString(szOutputBuf, 30, '|', strPAS.szSTLTime1); // szSTLTime1

			GetDelimitedString(szOutputBuf, 31, '|', strPAS.szSTLTime2); // szSTLTime2

			GetDelimitedString(szOutputBuf, 32, '|', strPAS.szSTLTime3); // szSTLTime3

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 33, '|', szTemp); // fOnGoing
			strPAS.fOnGoing= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 34, '|', strPAS.szWaitTime); // szWaitTime

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 35, '|', szTemp); // inRetryTime
			strPAS.inRetryTime= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 36, '|', szTemp); // fFirstBootUp
			strPAS.fFirstBootUp= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 37, '|', szTemp); // fSMMode
			strTCT.fSMMode= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 38, '|', szTemp); // fSMReceipt
			strTCT.fSMReceipt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 39, '|', szTemp); // fEnableBalInqMKE
			strTCT.fEnableBalInqMKE= (atoi(szTemp) > 0 ? TRUE: FALSE);		

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 40, '|', szTemp); // fDisplayAPPROVED
			strTCT.fDisplayAPPROVED= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 41, '|', szTemp); // fSingleComms
			strTCT.fSingleComms= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 42, '|', szTemp); // inECRTrxnMenu
			strTCT.inECRTrxnMenu= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 43, '|', szTemp); // XMLPRINT
			inTemp= atoi(szTemp);
			put_env_int("XMLPRINT",inTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 44, '|', szTemp); // XMLDBG
			inTemp= atoi(szTemp);
			put_env_int("XMLDBG",inTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 45, '|', szTemp); // fSMLogo
			strTCT.fSMLogo= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 46, '|', szTemp); // szSMReceiptLogo
			strcpy(strTCT.szSMReceiptLogo, szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 47, '|', szTemp); // szBDOReceiptLogo
			strcpy(strTCT.szBDOReceiptLogo, szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 48, '|', szTemp); // fLargeInvoice
			strFLG.fLargeInvoice= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 49, '|', szTemp); // fDutyFreeMode
			strTCT.fDutyFreeMode= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 50, '|', szTemp); // fAutoDownloadEnable
			strTCT.fAutoDownloadEnable= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 51, '|', szTemp); // byCTMSCallAfterSettle
			strTCT.byCTMSCallAfterSettle= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 52, '|', szTemp); // fPrintTipReceipt
			strTCT.fPrintTipReceipt= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 53, '|', szTemp); // fPrintFooterLogo
			strTCT.fPrintFooterLogo= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 54, '|', szTemp); // inMinBattery
			strTCT.inMinBattery= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 55, '|', szTemp); // fEnableManualKeyEntry
			strTCT.fEnableManualKeyEntry= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 56, '|', szTemp); // strTCT
			strTCT.fHotelSetup= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 57, '|', szTemp); // inPrinterHeatLevel
			strTCT.inPrinterHeatLevel= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 58, '|', szTemp); // inWaitTime
			strTCT.inWaitTime= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 59, '|', szTemp); // fForExRate
			strFLG.fForExRate= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 60, '|', szTemp); // fDCCMarkUp
			strFLG.fDCCMarkUp= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 61, '|', szTemp); // inDCCMode
			strTCT.inDCCMode= (atoi(szTemp) > 0 ? TRUE: FALSE);
			
			// Update TCT
			inTCTSave(1);

			// Update FLG
			inFLGSet("fGrabpayRetieveLast", (BOOL)strFLG.fGrabpayRetieveLast);

			// Update FLG
			inFLGSet("fLargeInvoice", (BOOL)strFLG.fLargeInvoice);

			// Update FLG
			inFLGSet("fForExRate", (BOOL)strFLG.fForExRate);
			
			// Update FLG
			inFLGSet("fDCCMarkUp", (BOOL)strFLG.fDCCMarkUp);
			
			// Update PAS
			inPASSave(1);
		
			//vdDisplayMessageBox(1, 8, strHDT.szHostLabel, "TERMINAL SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			inDisplayMessageBoxWithButtonConfirmation(1,8,strHDT.szHostLabel,"Terminal setting","Update complete","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case 0xFF:
			//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");

	return d_OK;
}

int vdCTOS_EditPrinter(void)		////tine:  modify to return int value, for handling Cancel function
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0, inRet;
	BYTE inKey = d_NO;
	BYTE szOutputBuf[512] = {0};
	BYTE szTemp[40 + 1] = {0};

    EditAgain:
    shHostIndex = inCTOS_SelectHostSetting();
    if (shHostIndex == -1)
        return -1;
	else if (shHostIndex == -2)
        return -2;

	CTOS_LCDTClearDisplay();

    shHostIndex=strHDT.inHostIndex;

    inResult = inCTOS_CheckAndSelectMutipleMID();
    if(d_OK != inResult)
        return -1;

    if(inCTOS_ChkBatchNotEmpty() != d_NO)
	{
        //vdDisplayMessageBox(1, 8, "EDIT NOT ALLOWED", "BATCH NOT EMPTY", "", MSG_TYPE_ERROR);
        inDisplayMessageBoxWithButtonConfirmation(1,8,"Edit not allowed","Batch not empty","","",MSG_TYPE_ERROR, BUTTON_TYPE_NONE_OK);
        CTOS_Beep();
        CTOS_Delay(2000);
        return -1;
	}

    vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(100);

	inTCTRead(1);
    inHDTRead(shHostIndex);

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");		

	// szRctHdr1
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr1", "Receipt Header Line 1", strMMT[0].szRctHdr1, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctHdr2
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr2", "Receipt Header Line 2", strMMT[0].szRctHdr2, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctHdr3
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr3", "Receipt Header Line 3", strMMT[0].szRctHdr3, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctHdr4
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr4", "Receipt Header Line 4", strMMT[0].szRctHdr4, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// szRctHdr5
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctHdr5", "Receipt Header Line 5", strMMT[0].szRctHdr5, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctFoot1
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctFoot1", "Receipt Footer Line 1", strMMT[0].szRctFoot1, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctFoot2
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctFoot2", "Receipt Footer Line 2", strMMT[0].szRctFoot2, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szRctFoot3
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szRctFoot3", "Receipt Footer Line 3", strMMT[0].szRctFoot3, 40, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	
	// Dummy
	//memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	//sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d", FLD_TYPE_NUMERIC, "szDummy", "Dummy Field", "Dummy", BATCH_NO_ASC_SIZE, FALSE);
	//strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	//strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("PRINTER SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
			//GetDelimitedString(szOutputBuf, 2, '|', ); // inHostIndex
			//GetDelimitedString(szOutputBuf, 3, '|', strHDT.szHostLabel); // szHostLabel

			GetDelimitedString(szOutputBuf, 4, '|', strMMT[0].szRctHdr1); // szRctHdr1
			GetDelimitedString(szOutputBuf, 5, '|', strMMT[0].szRctHdr2); // szRctHdr2
			GetDelimitedString(szOutputBuf, 6, '|', strMMT[0].szRctHdr3); // szRctHdr3
			GetDelimitedString(szOutputBuf, 7, '|', strMMT[0].szRctHdr4); // szRctHdr4
			GetDelimitedString(szOutputBuf, 8, '|', strMMT[0].szRctHdr5); // szRctHdr5
			GetDelimitedString(szOutputBuf, 9, '|', strMMT[0].szRctFoot1); // szRctFoot1
			GetDelimitedString(szOutputBuf, 10, '|', strMMT[0].szRctFoot2); // szRctFoot2
			GetDelimitedString(szOutputBuf, 11, '|', strMMT[0].szRctFoot3); // szRctFoot3

			// Update MMT
			inMMTSave(strMMT[0].MMTid);
		
			//vdDisplayMessageBox(1, 8, strHDT.szHostLabel, "PRINTER SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			inDisplayMessageBoxWithButtonConfirmation(1,8,strHDT.szHostLabel,"Printer setting","Update complete","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case 0xFF:
			//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	
	//goto EditAgain;
	return d_OK;
}

/*
USHORT CTOS_KBDBufFlush(void)
{
	vdDebug_LogPrintf("overload..CTOS_KBDBufFlush");
	
	return d_OK;
}
*/

int vdCTOS_EditIssuer(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0, inRet;
    BYTE inKey = d_NO;
    BYTE szOutputBuf[512] = {0};
    BYTE szTemp[40 + 1] = {0};
    USHORT inLenMID =0;

	vdDebug_LogPrintf("--vdCTOS_EditIssuer--");

	EditAgain:
    shHostIndex = inCTOS_SelectIssuer();
	vdDebug_LogPrintf("inCTOS_SelectIssuer, shHostIndex=[%d]", shHostIndex);
    if (shHostIndex == -1)
        return -1;
	else if (shHostIndex == -2)
        return -2;

	//CTOS_LCDTClearDisplay();
	
	vdDebug_LogPrintf("shHostIndex=[%d]", shHostIndex);

	if(inCTOS_ChkBatchNotEmpty() != d_NO)
	{	
		//vdDisplayMessageBox(1, 8, "EDIT NOT ALLOWED", "BATCH NOT EMPTY", "", MSG_TYPE_ERROR);
		inDisplayMessageBoxWithButtonConfirmation(1,8,"Edit not allowed","Batch not empty","","",MSG_TYPE_ERROR, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(2000);
	    return -1;
	}

	vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(100);

	inIITRead(shHostIndex); // Read IIT

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide

	// inIssuerNumber
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inIssuerNumber", "Issuer Number", strIIT.inIssuerNumber, 2, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szIssuerAbbrev
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szIssuerAbbrev", "Issuer Abbrev", strIIT.szIssuerAbbrev, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// szIssuerLabel
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szIssuerLabel", "Issuer Label", strIIT.szIssuerLabel, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szPANFormat
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szPANFormat", "PAN Mask Format", strIIT.szPANFormat, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// szMaskDisplay
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szMaskDisplay", "PAN Mask Display", strIIT.szMaskDisplay, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fGetPolicyNumber
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fGetPolicyNumber", "Policy Number?", strIIT.fGetPolicyNumber, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// inPolicyNumMaxChar
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inPolicyNumMaxChar", "Policy Num Max", strIIT.inPolicyNumMaxChar, 2, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// fNSR
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fNSR", "NSR?", strIIT.fNSR, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/*szNSRLimit*/
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szNSRLimit", "NSR Limit", strIIT.szNSRLimit, 13, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fNSRCustCopy
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fNSRCustCopy", "NSR Copy?", strIIT.fNSRCustCopy, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	/*szIssuerLogo*/
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szIssuerLogo", "Issuer Logo", strIIT.szIssuerLogo, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// fLast4Digits
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fLast4Digits", "Last4 Digit?", strIIT.fLast4Digits, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fQuasiCash
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fQuasiCash", "QuasiCash?", strIIT.fQuasiCash, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fBalInqAllowed
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fBalInqAllowed", "Bal Inq Allow?", strIIT.fBalInqAllowed, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// fMaskPanDisplay
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fMaskPanDisplay", "PAN Mask Display?", strIIT.fMaskPanDisplay, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szMaskBankCopy
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szMaskBankCopy", "Mask Bank Format", strIIT.szMaskBankCopy, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szMaskMerchantCopy
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szMaskMerchantCopy", "Mask Merchant Format", strIIT.szMaskMerchantCopy, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szMaskCustomerCopy
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szMaskCustomerCopy", "Mask Customer Format", strIIT.szMaskCustomerCopy, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Dummy
	//memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	//sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d", FLD_TYPE_NUMERIC, "szDummy", "Dummy Field", "Dummy", BATCH_NO_ASC_SIZE, FALSE);
	//strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	//strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("ISSUER SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("AAA - szOutputBuf[%s]", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|

			GetDelimitedString(szOutputBuf, 5, '|', strIIT.szPANFormat); // szPANFormat

			GetDelimitedString(szOutputBuf, 6, '|', strIIT.szMaskDisplay); // szMaskDisplay
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 7, '|', szTemp); // fGetPolicyNumber
			strIIT.fGetPolicyNumber = (atoi(szTemp) > 0 ? TRUE: FALSE);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 8, '|', szTemp); // inPolicyNumMaxChar
			strIIT.inPolicyNumMaxChar= atoi(szTemp);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 9, '|', szTemp); // fNSR
			strIIT.fNSR= (atoi(szTemp) > 0 ? TRUE: FALSE);

			GetDelimitedString(szOutputBuf, 10, '|', strIIT.szNSRLimit); // szNSRLimit

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 11, '|', szTemp); // fNSRCustCopy
			strIIT.fNSRCustCopy= (atoi(szTemp) > 0 ? TRUE: FALSE);

			GetDelimitedString(szOutputBuf, 12, '|', strIIT.szIssuerLogo); // szIssuerLogo

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 13, '|', szTemp); // fLast4Digits
			strIIT.fLast4Digits= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 14, '|', szTemp); // fQuasiCash
			strIIT.fQuasiCash= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 15, '|', szTemp); // fBalInqAllowed
			strIIT.fBalInqAllowed= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 16, '|', szTemp); // fMaskPanDisplay
			strIIT.fMaskPanDisplay= (atoi(szTemp) > 0 ? TRUE: FALSE);

			GetDelimitedString(szOutputBuf, 17, '|', strIIT.szMaskBankCopy); // szMaskBankCopy
			GetDelimitedString(szOutputBuf, 18, '|', strIIT.szMaskMerchantCopy); // szMaskMerchantCopy
			GetDelimitedString(szOutputBuf, 19, '|', strIIT.szMaskCustomerCopy); // szMaskCustomerCopy

			inIITSave(shHostIndex);
			
			//vdDisplayMessageBox(1, 8, strIIT.szIssuerLabel, "ISSUER SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			inDisplayMessageBoxWithButtonConfirmation(1,8,strIIT.szIssuerLabel,"Issuer setting","Update complete","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case 0xFF:
			//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	
	//goto EditAgain;
	return d_OK;
	
}

// SMACPAY
void vdParseSMACPayCardData(TRANS_DATA_TABLE *srTransPara, char *szCardData, int inLen)
{
	int inIndex = 0;
	int inFileID02_Index = 0;
	int inFileID03_Index = 0;
	int inFileID04_Index = 0;
	int inFileID09_Index = 0;
	char szAmount[AMT_ASC_SIZE+1]= {0};

	vdDebug_LogPrintf("***vdParseSMACPayCardData***");
	DebugAddHEX("szCardData",szCardData,inLen);
	vdDebug_LogPrintf("inLen[%d]",inLen);
	while(1)
	{
		DebugAddHEX("COMPARE FIELD 02",&szCardData[inIndex],3);
		if(memcmp(&szCardData[inIndex],"02|",3) == 0)
		{
			inFileID02_Index = inIndex+3;

			memset(strPersonal_Info.bCardNo,0x00,sizeof(strPersonal_Info.bCardNo));
			memcpy(strPersonal_Info.bCardNo,(BYTE *)&szCardData[inFileID02_Index],16);
			inFileID02_Index+=16;
			
			memset(strPersonal_Info.bCardHolderName,0x00,sizeof(strPersonal_Info.bCardHolderName));
			memcpy(strPersonal_Info.bCardHolderName,(BYTE *)&szCardData[inFileID02_Index],26);
			inFileID02_Index+=26;//Skip Card Holder name

			memset(strPersonal_Info.bExpiryDate,0x00,sizeof(strPersonal_Info.bExpiryDate));
			memcpy(strPersonal_Info.bExpiryDate,(BYTE *)&szCardData[inFileID02_Index],10);
			inFileID02_Index+=10;

			memset(strPersonal_Info.bCardSeqNo,0x00,sizeof(strPersonal_Info.bCardSeqNo));
			memcpy(strPersonal_Info.bCardSeqNo,(BYTE *)&szCardData[inFileID02_Index],2);
			inFileID02_Index+=2;

			memset(strPersonal_Info.bCardStatus,0x00,sizeof(strPersonal_Info.bCardStatus));
			memcpy(strPersonal_Info.bCardStatus,(BYTE *)&szCardData[inFileID02_Index],2);
			inFileID02_Index+=2;				

			memset(strPersonal_Info.bCardPerDate,0x00,sizeof(strPersonal_Info.bCardPerDate));
			memcpy(strPersonal_Info.bCardPerDate,(BYTE *)&szCardData[inFileID02_Index],10);
			inFileID02_Index+=10;

			memset(strPersonal_Info.bLastDataSync,0x00,sizeof(strPersonal_Info.bLastDataSync));
			memcpy(strPersonal_Info.bLastDataSync,(BYTE *)&szCardData[inFileID02_Index],10);
			inFileID02_Index+=10;
			
			DebugAddHEX("bCardNo",strPersonal_Info.bCardNo,16);
			DebugAddHEX("bCardHolderName",strPersonal_Info.bCardHolderName,26);
			DebugAddHEX("bExpiryDate",strPersonal_Info.bExpiryDate,10);
			DebugAddHEX("bCardSeqNo",strPersonal_Info.bCardSeqNo,2);
			DebugAddHEX("bCardStatus",strPersonal_Info.bCardStatus,2);
			DebugAddHEX("bCardPerDate",strPersonal_Info.bCardPerDate,10);
			DebugAddHEX("bLastDataSync",strPersonal_Info.bLastDataSync,10);


			inIndex = inFileID02_Index;			
								
			break;
		}	
		
		inIndex++;
		
		if(inIndex >= inLen)
		{
			vdDebug_LogPrintf("***TOKEN NOT FOUND!!***");
			break;
		}
	}		

	
	
	inIndex = 0;
	while(1)
	{
		DebugAddHEX("COMPARE FIELD 03",&szCardData[inIndex],3);
		if(memcmp(&szCardData[inIndex],"03|",3) == 0)
		{
			if(srTransPara->byTransType == KIT_SALE)
			{
				inFileID03_Index = inIndex + 3;
				vdDebug_LogPrintf("inIndex[%d] :: inFileID03_Index[%d]",inIndex,inFileID03_Index);
			
				memset(strMembership_Info.bMemberSince,0x00,sizeof(strMembership_Info.bMemberSince));
				memcpy(strMembership_Info.bMemberSince,(BYTE *)&szCardData[inFileID03_Index],7);
				inFileID03_Index+=7;

				memset(strMembership_Info.bMembershipType,0x00,sizeof(strMembership_Info.bMembershipType));
				memcpy(strMembership_Info.bMembershipType,(BYTE *)&szCardData[inFileID03_Index],1);
				inFileID03_Index+=1;

				memset(strMembership_Info.bTenure,0x00,sizeof(strMembership_Info.bTenure));
				memcpy(strMembership_Info.bTenure,(BYTE *)&szCardData[inFileID03_Index],2);
				inFileID03_Index+=2;

				memset(strMembership_Info.bAcquiChannel,0x00,sizeof(strMembership_Info.bAcquiChannel));
				memcpy(strMembership_Info.bAcquiChannel,(BYTE *)&szCardData[inFileID03_Index],1);
				inFileID03_Index+=1;

				memset(strMembership_Info.bKitAcqDate,0x00,sizeof(strMembership_Info.bKitAcqDate));
				memcpy(strMembership_Info.bKitAcqDate,(BYTE *)&szCardData[inFileID03_Index],10);
				inFileID03_Index+=10;

				memset(strMembership_Info.bKitAcqCompany,0x00,sizeof(strMembership_Info.bKitAcqCompany));
				memcpy(strMembership_Info.bKitAcqCompany,(BYTE *)&szCardData[inFileID03_Index],4);
				inFileID03_Index+=4;

				memset(strMembership_Info.bKitAcqBranch,0x00,sizeof(strMembership_Info.bKitAcqBranch));
				memcpy(strMembership_Info.bKitAcqBranch,(BYTE *)&szCardData[inFileID03_Index],6);
				inFileID03_Index+=6;

				DebugAddHEX("bMemberSince",strMembership_Info.bMemberSince,7);
				DebugAddHEX("bMembershipType",strMembership_Info.bMembershipType,1);
				DebugAddHEX("bTenure",strMembership_Info.bTenure,2);
				DebugAddHEX("bAcquiChannel",strMembership_Info.bAcquiChannel,1);
				DebugAddHEX("bKitAcqDate",strMembership_Info.bKitAcqDate,10);
				DebugAddHEX("bKitAcqCompany",strMembership_Info.bKitAcqCompany,4);
				DebugAddHEX("bKitAcqBranch",strMembership_Info.bKitAcqBranch,6);
			}

			break;
		}

			
		inIndex++;

		if(inIndex >= inLen)
		{
			vdDebug_LogPrintf("***TOKEN NOT FOUND!!***");
			break;
		}
	}
	

	inIndex = 0;
	while(1)
	{
		DebugAddHEX("COMPARE FIELD 09",&szCardData[inIndex],3);
		if(memcmp(&szCardData[inIndex],"09|",3) == 0)
		{
			if(srTransPara->byTransType == RENEWAL || srTransPara->byTransType == PTS_AWARDING || inCheckSMACPayRedemption(srTransPara) == TRUE
				|| inCheckSMACPayBalanceInq(srTransPara) == TRUE || inCheckSMACPayVoid(srTransPara) == TRUE)
			{
				inFileID09_Index = inIndex+3;
				memcpy(srTransPara->szSMACPay_HostBalance,&szCardData[inFileID09_Index],12);
				DebugAddHEX("szSMACBDORewardsBalance",srTransPara->szSMACPay_HostBalance,12);
			}
			break;
		}
			
		inIndex++;

		if(inIndex >= inLen)
		{
			vdDebug_LogPrintf("***TOKEN NOT FOUND!!***");
			break;
		}
	}
			
		
	return;
	
}

int inCheckSMACPayRedemption(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("***inCheckSMACPayRedemption***");
	
	if(srTransPara->byTransType == SALE && (srTransPara->byEntryMode == CARD_ENTRY_WAVE || srTransPara->fVirtualCard == TRUE)
		&& srTransPara->HDTid == SMAC_HDT_INDEX)
	{
		vdDebug_LogPrintf("SMACPAY REDEMPTION");
		return TRUE;
	}

	vdDebug_LogPrintf("NOT SMACPAY REDEMPTION OR NOT CTLS/VIRTUAL CARD");
	return FALSE;
}

int inCheckSMACPayBalanceInq(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("***inCheckSMACPayBalanceInq***");
	
	if(srTransPara->byTransType == SMAC_BALANCE && srTransPara->byEntryMode == CARD_ENTRY_WAVE && srTransPara->HDTid == SMAC_HDT_INDEX)
	{
		vdDebug_LogPrintf("SMACPAY BALANCE INQUIRY");
		return TRUE;
	}

	vdDebug_LogPrintf("NOT SMACPAY BALANCE INQUIRY");
	return FALSE;
}

int inCheckSMACPayTransaction(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("***inCheckSMACPayRedemption***");
	
	if(((srTransPara->byTransType == SMAC_BALANCE || srTransPara->byTransType == SALE || srTransPara->byTransType == VOID) && srTransPara->byEntryMode == CARD_ENTRY_WAVE && srTransPara->HDTid == SMAC_HDT_INDEX)
		|| srTransPara->byTransType == KIT_SALE || srTransPara->byTransType == RENEWAL || srTransPara->byTransType == PTS_AWARDING)
	{
		vdDebug_LogPrintf("SMACPAY TRANSACTION");
		return TRUE;
	}

	vdDebug_LogPrintf("NOT SMACPAY TRANSACTION");
	return FALSE;
}


int inCheckSMACPayVoid(TRANS_DATA_TABLE *srTransPara)
{
	vdDebug_LogPrintf("***inCheckSMACPayVoid***");
	
	if(srTransPara->byTransType == VOID && srTransPara->byEntryMode == CARD_ENTRY_WAVE && srTransPara->HDTid == SMAC_HDT_INDEX)
	{
		vdDebug_LogPrintf("SMACPAY VOID");
		return TRUE;
	}

	vdDebug_LogPrintf("NOT SMACPAY VOID");
	return FALSE;
}

void vdLoadSMACPayCardDetails(void)
{
	char szTemp[4+1];

	vdDebug_LogPrintf("***vdLoadSMACPayCardDetails***");
	
	strcpy(strPersonal_Info.bCardNo, srTransRec.szPAN);

	memcpy(strPersonal_Info.bCardHolderName,srTransRec.szCardholderName,strlen(srTransRec.szCardholderName));
	
	memcpy(strPersonal_Info.bExpiryDate,srTransRec.bySMACPay_ExpiryDate, 10);
	
	memcpy(strPersonal_Info.bCardSeqNo,srTransRec.bySMACPay_CardSeqNo, 2);

	memcpy(strPersonal_Info.bCardStatus,srTransRec.bySMACPay_CardStatus,2);

	memcpy(strPersonal_Info.bCardPerDate,srTransRec.bySMACPay_CardPerDate,10);

	memcpy(strPersonal_Info.bLastDataSync,srTransRec.bySMACPay_LastDataSync,10);

	vdDebug_LogPrintf("CARD NUMBER     [%s]",strPersonal_Info.bCardNo);
	vdDebug_LogPrintf("CARD HOLDER NAME[%s]",strPersonal_Info.bCardHolderName);
	vdDebug_LogPrintf("EXPIRY          [%s]",strPersonal_Info.bExpiryDate);
	vdDebug_LogPrintf("CARD SEQ NUMBER [%s]",strPersonal_Info.bCardSeqNo);
	vdDebug_LogPrintf("CARD STATUS     [%s]",strPersonal_Info.bCardStatus);
	vdDebug_LogPrintf("CARD PER DATE   [%s]",strPersonal_Info.bCardPerDate);
	vdDebug_LogPrintf("LAST DATA SYNC  [%s]",strPersonal_Info.bLastDataSync);

	return;
	
}

// SMACPAY

// get transaction date/time -- sidumili
void GetTransDateTime(char* szDate, char* szTime)
{
	char szStr[d_LINE_SIZE + 1];
	char szTemp[d_LINE_SIZE + 1];
	char szTemp1[d_LINE_SIZE + 1];
	USHORT result;
	BYTE baTemp[PAPER_X_SIZE * 64];
	CTOS_RTC SetRTC;
	char szYear[3];
	char sMonth[4];
	char szTempMonth[3];
	char szMonthNames[40];

	memset(sMonth,0x00,4);
	memset(szMonthNames,0x00,40);
	strcpy(szMonthNames,"JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC");
	memset(szTempMonth, 0x00, 3);
	
	memset(szStr, ' ', d_LINE_SIZE);
	memset(szTemp, ' ', d_LINE_SIZE);

	CTOS_RTCGet(&SetRTC);
	sprintf(szYear ,"%02d",SetRTC.bYear);
	memcpy(srTransRec.szYear,szYear,2);
	vdDebug_LogPrintf("year[%s],date[%02x][%02x]",srTransRec.szYear,srTransRec.szDate[0],srTransRec.szDate[1]);
	wub_hex_2_str(srTransRec.szDate, szTemp,DATE_BCD_SIZE);
	wub_hex_2_str(srTransRec.szTime, szTemp1,TIME_BCD_SIZE);
	vdDebug_LogPrintf("date[%s],time[%s]atol(szTemp)=[%d](atol(szTemp1)=[%d]",szTemp,szTemp1,atol(szTemp),atol(szTemp1) );

	if(atol(szTemp) == 0)
	{
		sprintf(szTemp ,"%02d%02d",SetRTC.bMonth, SetRTC.bDay); 
	}
	
	if(atol(szTemp1) == 0)
	{
		sprintf(szTemp1 ,"%02d%02d%02d",SetRTC.bHour,SetRTC.bMinute,SetRTC.bSecond);	
	}
	vdDebug_LogPrintf("date[%s],time[%s]",szTemp,szTemp1);
	memcpy(szTempMonth,&szTemp[0],2);
	memcpy(sMonth, &szMonthNames[(atoi(szTempMonth) - 1)* 3], 3);

	//sprintf(szDate, "%s %02lu, %02lu", sMonth,atol(szTemp)%100,atol(srTransRec.szYear)); // eg. May 21,22 10:01:25 
	sprintf(szDate, "%02lu/%s/20%02lu", atol(szTemp)%100,szTempMonth,atol(srTransRec.szYear)); // eg. 21/05/2022 10:01:25
	sprintf(szTime, "%02lu:%02lu:%02lu", atol(szTemp1)/10000,atol(szTemp1)%10000/100, atol(szTemp1)%100);
	 
}

void vdPromptMessageUI(char *msg, char *msg2, char *msg3, int msgType)
{
	BYTE szDisMsg[200 + 1] = {0};
	BYTE szMsgType[40 + 1] = {0};

	vdDebug_LogPrintf("--vdPromptMessageUI--");
	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, msg);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, msg2);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, msg3);
	strcat(szDisMsg, "|");
	
	memset(szMsgType, 0x00, sizeof(szMsgType));
	inGetBoxMessageType(msgType, szMsgType);
	strcat(szDisMsg, szMsgType);

	vdDebug_LogPrintf("szDisMsg=[%s], msgType=[%d]", szDisMsg, msgType);
	usCTOSS_PromptMessageUI(szDisMsg);

}

// Menu selection -- sidumili
int inMenuSelection(char* szMessage)
{
	BYTE szDisMsg[9999 + 1] = {0};
	BYTE szMsgType[40 + 1] = {0};
	int inRet = d_NO;

	vdDebug_LogPrintf("--inMenuSelection--");
	vdDebug_LogPrintf("szMessage=[%s]", szMessage);
	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, szMessage);
	strcat(szDisMsg, "|");
		
	vdDebug_LogPrintf("szDisMsg=[%s]", szDisMsg);
	
	inRet = usCTOSS_MenuSelection(szDisMsg);

	//DisplayStatusLine("");
	
	return inRet;

}

void GetTransAmount(char* szAmount)
{
	BYTE szTemp[40 + 1] = {0};
	
	inCSTRead(1);
	wub_hex_2_str(srTransRec.szBaseAmount, szTemp, AMT_BCD_SIZE);
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szTemp,szAmount);// patrick add code 20141216  
	memset(szTemp, 0x00, sizeof(szTemp));
	sprintf(szTemp,"%s %s",strCST.szCurSymbol,szAmount);

	memset(szAmount, 0x00, sizeof(szAmount));
	strcpy(szAmount, szTemp);
}

// get key from selected in menu -- sidumili
int inGetMenuKeySelected(char* szMenu, char* szSelected)
{
	int inKey = 0;
	int i = 0;
	BYTE szTemp[100 + 1] = {0};
	BOOL isValid = FALSE;
	
	vdDebug_LogPrintf("--inGetMenuKeySelected--");
	vdDebug_LogPrintf("szMenu=[%s]", szMenu);
	vdDebug_LogPrintf("szSelected=[%s]", szSelected);
	
	for (i = 1; i < 100; i++)
	{
		memset(szTemp, 0x00, sizeof(szTemp));
		GetDelimitedString(szMenu, i, '|', szTemp);
		SetToUpperCase(szTemp);
		vdDebug_LogPrintf("Loop::i=[%d], szTemp=[%s] is equal with szSelected=[%s]", i, szTemp, szSelected);
		if (strlen(szTemp) > 0)
		{
			if(strcmp(szTemp,szSelected)==0)
			{
				vdDebug_LogPrintf("menu found..!!!");
				inKey = i;
				isValid = TRUE;
				break;
			}
		}
		else
		{
			break;
		}
	}

	vdDebug_LogPrintf("inKey=[%d]", inKey);

	if (!isValid)
	{
		inKey = 0;
		//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(1000);
	}
	
	return inKey;
}

int BatchReviewListView(void)
{
	int inKey = d_NO;
	int inRet = d_NO;
	BYTE szTitle[40 + 1] = {0};

	vdDebug_LogPrintf("--BatchReviewListView--");
	
	// Read Data
	inRet = ReadAndWriteDetailData();
	vdDebug_LogPrintf("ReadAndWriteDetailData::inRet=[%d]", inRet);
	if (inRet != d_OK)	
	return d_NO;	
	
	memset(strGBLVar.szGBLvProcessType, 0x00, sizeof(strGBLVar.szGBLvProcessType));
	memset(strGBLVar.szGBLvSelectedValue, 0x00, sizeof(strGBLVar.szGBLvSelectedValue));
	memset(strGBLVar.szGBLvProcessDesc, 0x00, sizeof(strGBLVar.szGBLvProcessDesc));

	memset(szTitle, 0x00, sizeof(szTitle));
	sprintf(szTitle, "%s %s|x", "Batch Review", "");
	
	inRet = usCTOSS_TransDetailListViewUI(szTitle);
	
	vdDebug_LogPrintf("usCTOSS_TransDetailListViewUI::inRet=[%d]", inRet);
	vdDebug_LogPrintf("strGBLVar.szGBLvProcessType=[%s]", strGBLVar.szGBLvProcessType);
	vdDebug_LogPrintf("strGBLVar.szGBLvSelectedValue=[%s]", strGBLVar.szGBLvSelectedValue);
	vdDebug_LogPrintf("strGBLVar.szGBLvProcessDesc=[%s]", strGBLVar.szGBLvProcessDesc);

	if (inRet == d_TIMEOUT || inRet == d_KBD_CANCEL)
		CTOS_Beep();
		
	return inRet;
}

int ReadAndWriteDetailData(void)
{
	int inResult,i,inCount;
	int inBatchNumOfRecord;
    int *pinTransDataid;
	BYTE szBatchNo[6 + 1] = {0};
	BYTE szInvoiceNo[6 + 1] = {0};
	BYTE szDate[12 + 1] = {0};
	BYTE szTime[12 + 1] = {0};
	BYTE szPAN[30 + 1] = {0};
	BYTE szAmountTemp[40 + 1] = {0};
	BYTE szAmount[40 + 1] = {0};
	BYTE szDateTime[40 + 1] = {0};
	BYTE szTitle[40 + 1] = {0};
	BYTE szTemp1[30+1] = {0};
	BYTE szPAN1[20+1] = {0};
	BYTE szPAN2[20+1] = {0};
	int inRemaining = 0;
	
	inBatchNumOfRecord = inBatchNumALLRecord();
	
	vdDeleteFile(TRANS_DETAIL_FILE); // Delete file
	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	memset(strWriteFile.szWriteTemp, 0x00, sizeof(strWriteFile.szWriteTemp));
	strWriteFile.inWriteSize = 0;
		
    pinTransDataid = (int*)malloc(inBatchNumOfRecord  * sizeof(int));
    inCount = 0;		
    inBatchByMerchandHost(inBatchNumOfRecord, srTransRec.HDTid, srTransRec.MITid, srTransRec.szBatchNo, pinTransDataid);
    for(i=0; i<inBatchNumOfRecord; i++)
    {
        inResult = inDatabase_BatchReadByTransId(&srTransRec, pinTransDataid[inCount]);
        inCount ++;
	
		inIITRead(srTransRec.IITid);; // Get IIT

		// Batch No
		memset(szBatchNo, 0x00, sizeof(szBatchNo));
		wub_hex_2_str(srTransRec.szBatchNo,szBatchNo,3);

		// Invoice No
		memset(szInvoiceNo, 0x00, sizeof(szInvoiceNo));
   	    wub_hex_2_str(srTransRec.szInvoiceNo, szInvoiceNo, INVOICE_BCD_SIZE);

		// PAN Format
		inIITRead(srTransRec.IITid);
		memset(szTemp1, 0x00, sizeof(szTemp1));
		memset(szPAN1, 0x00, sizeof(szPAN1));
		memset(szPAN2, 0x00, sizeof(szPAN2));
		memset(szPAN, 0x00, sizeof(szPAN));

		vdCTOS_FormatPAN(strIIT.szPANFormat, srTransRec.szPAN, szTemp1);
		strcpy(szTemp1, srTransRec.szPAN);
		cardMasking(szTemp1, 5);
		strcpy(szPAN, szTemp1);

		if (strlen(szTemp1) > 20)
		{
			memset(szPAN1, 0x00, sizeof(szPAN1));
			memset(szPAN2, 0x00, sizeof(szPAN2));
			inRemaining = strlen(szTemp1) - 20;
			
			memcpy(szPAN1, szTemp1, 20);
			memcpy(szPAN2, &szTemp1[20], inRemaining);
			strcpy(szPAN, szPAN2);
		}
				
		GetTransDateTime(szDate, szTime);
		sprintf(szDateTime, "%s %s", szDate, szTime);

		inCSTReadEx(strHDT.inCurrencyIdx);
		wub_hex_2_str(srTransRec.szTotalAmount, szAmountTemp, AMT_BCD_SIZE);
		vdCTOS_FormatAmount(strCST.szAmountFormat, szAmountTemp,szAmount);// patrick add code 20141216	
		memset(szAmountTemp, 0x00, sizeof(szAmountTemp));
		
		if (srTransRec.byTransType == VOID)
			sprintf(szAmountTemp,"-%s %s",strCST.szCurSymbol,szAmount);
		else
			sprintf(szAmountTemp,"%s %s",strCST.szCurSymbol,szAmount);
			

		memset(szTitle, 0x00, sizeof(szTitle));
		szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);
		
		memset(strWriteFile.szWriteTemp, 0x00, sizeof(strWriteFile.szWriteTemp));

		strcpy(strWriteFile.szWriteTemp, srTransRec.szHostLabel);
		strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
		strcat(strWriteFile.szWriteTemp, szPAN);
		strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
		strcat(strWriteFile.szWriteTemp, strIIT.szIssuerLabel);
		strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
		strcat(strWriteFile.szWriteTemp, szBatchNo);
		strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
		strcat(strWriteFile.szWriteTemp, szInvoiceNo);
		strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
		strcat(strWriteFile.szWriteTemp, srTransRec.szAuthCode);
		strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
		strcat(strWriteFile.szWriteTemp, szDateTime);
		strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
		strcat(strWriteFile.szWriteTemp, szTitle);
		strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
		strcat(strWriteFile.szWriteTemp, szAmountTemp);
		strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
		
		strcat(strWriteFile.szWriteTemp, "\n");
		strWriteFile.inWriteSize += strlen(strWriteFile.szWriteTemp);
		strcat(strWriteFile.szWriteData, strWriteFile.szWriteTemp);
				
		vdDebug_LogPrintf("inWriteSize=[%d] szWriteTemp=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteTemp);
		
    }

	free(pinTransDataid);

	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, TRANS_DETAIL_FILE);
	
	return d_OK;
}

// added history menu -- sidumili
int inCTOS_SelectHistoryMenu(void)
{
	int inRet=d_NO;
    char szDebug[40 + 1]={0};
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04;
	int key = 0; 	
    char szChoiceMsg[30 + 1];
    BYTE szTitle[20];
    BYTE szDisMsg[528];
	BYTE szMessage[2000 + 1] = {0};
	BYTE szAmount[40 + 1] = {0};

	vdDebug_LogPrintf("--inCTOS_SelectHistoryMenu--");
	
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    //strcpy(szChoiceMsg,"Reprint|");
    //strcpy(szChoiceMsg,"Report|");
	//strcat(szChoiceMsg,"Review");
    sprintf(szChoiceMsg, "%s", "Report#ic_report.png|Review#ic_review.png||");

	vdDebug_LogPrintf("szChoiceMsg=[%s]", szChoiceMsg);

	//memset(szTitle, 0x00, sizeof(szTitle));
	//szGetTransTitle(srTransRec.byTransType, szTitle);
	
	GetTransAmount(szAmount);
	memset(szMessage, 0x00, sizeof(szMessage));
	sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_IMAGE_VIEW, MENU_2_COL_COUNT, "", "History");
	strcat(szMessage, szChoiceMsg);
	
	key = inMenuSelection(szMessage);	
    vdDebug_LogPrintf("inMenuSelection=[%d]", key);

	if (key == 0xFF || key == d_TIMEOUT) //TimeOut
    {
		//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return d_NO;
    }
    else if (key == d_KBD_CANCEL || key == d_USER_CANCEL)
    {
		//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return d_NO;
    }

	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
	sprintf(szChoiceMsg, "%s", "Report|Review");
	key = inGetMenuKeySelected(szChoiceMsg, strGBLVar.szGBLvProcessDesc);
    vdDebug_LogPrintf("inGetMenuKeySelected=[%d]", key);

	if (key > 0)
	{
		switch (key)
		{
			//case 1: //Reprint
			//	inReprintSelection();
			//	break;
			case 1: // Report
				inReportSelecion();
				break;
			case 2: // Review
				inCTOS_SelectReviewMenu();
				break;
		}
	}
	else
	{
		return d_NO;
	}
		
	return d_OK;
}

// added review menu -- sidumili
int inCTOS_SelectReviewMenu(void)
{
	int inRet=d_NO;
    char szDebug[40 + 1]={0};
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04;
	int key = 0; 	
    char szChoiceMsg[30 + 1];
    BYTE szTitle[20];
    BYTE szDisMsg[528];
	BYTE szMessage[2000 + 1] = {0};
	BYTE szAmount[40 + 1] = {0};

	vdDebug_LogPrintf("--inCTOS_SelectReviewMenu--");
	
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    strcpy(szChoiceMsg,"Batch Total|");
    strcat(szChoiceMsg,"Batch Review");
	//strcat(szChoiceMsg,"Retrieve");

	vdDebug_LogPrintf("szChoiceMsg=[%s]", szChoiceMsg);

	//memset(szTitle, 0x00, sizeof(szTitle));
	//szGetTransTitle(srTransRec.byTransType, szTitle);
	
	GetTransAmount(szAmount);
	memset(szMessage, 0x00, sizeof(szMessage));
	sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_VIEW, MENU_2_COL_COUNT, "Review", "Select which info to review");
	strcat(szMessage, szChoiceMsg);
	
	key = inMenuSelection(szMessage);	
    vdDebug_LogPrintf("inMenuSelection=[%d]", key);

	if (key == 0xFF || key == d_TIMEOUT) //TimeOut
    {
		//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return -1;
    }
    else if (key == d_KBD_CANCEL || key == d_USER_CANCEL)
    {
		//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return -1;
    }

	key = inGetMenuKeySelected(szChoiceMsg, strGBLVar.szGBLvProcessDesc);
    vdDebug_LogPrintf("inGetMenuKeySelected=[%d]", key);

	if (key > 0)
	{
		switch (key)
		{
			case 1: //Batch Total
				strcpy(strGBLVar.szGBLvMenuTitle, "Batch Total");
				inCTOS_BATCH_TOTAL();
				break;
			case 2: // Batch Review
				strcpy(strGBLVar.szGBLvMenuTitle, "Batch Review");
				inCTOS_BATCH_REVIEW();
				break;
			//case 3: // Retrieve
			//	strcpy(strGBLVar.szGBLvMenuTitle, "Retrieve");
			//	inCTOS_Retrieve();
			//	break;
		}
	}
	else
	{
		return d_NO;
	}
	
	
	return d_OK;
}

// ui android for get expiry date -- sidumili
USHORT getExpiryDate(OUT BYTE *baBuf)
{
    USHORT usRet;
    USHORT usLens;
    USHORT usMinLen = 3;
    USHORT usMaxLen = 4;
    USHORT usInputLine = 8;

	//TINE:  android
	BYTE szTitle[16];
    BYTE szDisplay[100];

	USHORT usTk1Len=TRACK_I_BYTES, usTk2Len=TRACK_II_BYTES, usTk3Len=TRACK_III_BYTES;
	BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];

	memset(szTitle, 0x00, sizeof(szTitle));
    //szGetTransTitle(srTransRec.byTransType, szTitle);
    szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);

	memset(szDisplay, 0x00, sizeof(szDisplay));

    strcpy(szDisplay, "3");		//min length
    strcat(szDisplay, "|");
    strcat(szDisplay, "4");		//max length
    strcat(szDisplay, "|");
    strcat(szDisplay, szTitle);
    strcat(szDisplay, "|");
    strcat(szDisplay, "Please enter expiry date");

    while(1)
    {
		usRet = InputStringUI(0x01, 0x02, baBuf, &usLens, usMinLen, d_INPUT_TIMEOUT, szDisplay);
        vdDebug_LogPrintf("usRet[%d]atoi(baBuf)=[%d]usLens[%d]", usRet, atoi(baBuf), usLens);

		// cancel or timeout
		if (usRet == d_KBD_CANCEL || usRet == 0xFF)
		{
			vdDebug_LogPrintf("*************************");
  	   		CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);	//clear buffer on idle swipe on term reg.
			return (d_EDM_USER_CANCEL);
		}
            
        return d_OK;
    }
}

// prompt for manual posting -- sidumili
void vdPromptManualPosting(void)
{
	BYTE szBankName[40 +1] = {0};
	BYTE szTemp[100 +1] = {0};
	
	memset(szBankName, 0x00, sizeof(szBankName));
	inCTOSS_GetEnvDB ("BANKNAME", szBankName);
	memset(szTemp, 0x00, sizeof(szTemp));
	sprintf(szTemp, "%s for manual posting.", szBankName);

	CTOS_Beep();
	inDisplayMessageBoxWithButtonConfirmation(1,8,"Charge slip should be submitted to",szTemp,"","Clear Batch",MSG_TYPE_SUCCESS, BUTTON_TYPE_NONE_BACK_TO_MAIN);
}

int inCTOS_Management(void)
{

    char szChoiceMsg[30 + 1];
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04, Menukey=0;
    int inRet;
    int key;
	BYTE szSettleMsg[128];
    USHORT shRet = 0,
            shLen = 0,
            iInitX = 0,
            shMinLen = 4,
            shMaxLen = 6;
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE + 1] = {0},
            szTitle[MAX_CHAR_PER_LINE + 1] = {0};
	
	BYTE szMessage[2000 + 1] = {0};

    memset(szTitle, 0x00, sizeof(szTitle));
    strcpy(szTitle, "Management");
    
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    strcpy(szChoiceMsg,"Logon|");
	strcat(szChoiceMsg,"Settlement");
	vdDebug_LogPrintf("szChoiceMsg=[%s]", szChoiceMsg);

	memset(szMessage, 0x00, sizeof(szMessage));
	sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_VIEW, MENU_2_COL_COUNT, "Transactions", szTitle);
	strcat(szMessage, szChoiceMsg);

	key = inMenuSelection(szMessage);	
    vdDebug_LogPrintf("inMenuSelection=[%d]", key);

	if (key == 0xFF || key == d_TIMEOUT) //TimeOut
    {
		//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return -1;
    }
    else if (key == d_KBD_CANCEL || key == d_USER_CANCEL)
    {
		//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return -1;
    }

	key = inGetMenuKeySelected(szChoiceMsg, strGBLVar.szGBLvProcessDesc);
    vdDebug_LogPrintf("inGetMenuKeySelected=[%d]", key);

	if (key > 0)
	{
		switch (key)
		{
			case 1: //Log In/Log On
				inCTOS_LOGON();
				break;
			case 2: // Settlement
				inCTOS_Settle_Selection();
				break;
		}
	}
	else
	{
		return d_NO;
	}
	
    return SUCCESS;


}

int inCTOS_NewSelectHostSettingWithIndicator(int inIndicator, char* szHeader, char* szSubHeader)
{
	int inKey = d_NO;
	int inRet = d_NO;
	BYTE szTitle[40 + 1] = {0};
	int inHostIndex = -1;
	BYTE szHostIndex[10 + 1] = {0};

	vdDebug_LogPrintf("--inCTOS_NewSelectHostSettingWithIndicator--");
	vdDebug_LogPrintf("inIndicator=[%d]", inIndicator);
	vdDebug_LogPrintf("szHeader=[%s]", szHeader);
	vdDebug_LogPrintf("szSubHeader=[%s]", szSubHeader);
	
	// Read Data
	inRet = ReadAndWriteHostName(inIndicator);
	if (inRet != d_OK)	
	return d_NO;	
	
	memset(strGBLVar.szGBLvProcessType, 0x00, sizeof(strGBLVar.szGBLvProcessType));
	memset(strGBLVar.szGBLvSelectedValue, 0x00, sizeof(strGBLVar.szGBLvSelectedValue));
	memset(strGBLVar.szGBLvProcessDesc, 0x00, sizeof(strGBLVar.szGBLvProcessDesc));

	memset(szTitle, 0x00, sizeof(szTitle));
	sprintf(szTitle, "%s|%s", szHeader, szSubHeader);

	vdDebug_LogPrintf("szTitle=[%s]", szTitle);
	
	inRet = usCTOSS_SingleRecordDataListViewUI(szTitle);
	
	vdDebug_LogPrintf("usCTOSS_SingleRecordDataListViewUI,inRet=[%d]", inRet);
	vdDebug_LogPrintf("strGBLVar.szGBLvProcessType=[%s]", strGBLVar.szGBLvProcessType);
	vdDebug_LogPrintf("strGBLVar.szGBLvSelectedValue=[%s]", strGBLVar.szGBLvSelectedValue);
	vdDebug_LogPrintf("strGBLVar.szGBLvProcessDesc=[%s]", strGBLVar.szGBLvProcessDesc);

	if (inRet != d_OK)
		return d_NO;

	// HostIndex
	memset(szHostIndex, 0x00, sizeof(szHostIndex));
	inHostIndex = atoi(strGBLVar.szGBLvProcessType);
	vdDebug_LogPrintf("usCTOSS_SingleRecordDataListViewUI,inHostIndex=[%d]", inHostIndex);

	// Load Data
	srTransRec.HDTid = inHostIndex;
	strHDT.inHostIndex = inHostIndex;
	inHDTRead(inHostIndex);
	inCPTRead(inHostIndex);
	strcpy(srTransRec.szHostLabel, strHDT.szHostLabel);
	inCSTRead(strHDT.inCurrencyIdx); /*to load Currence and amount format*/
	
	return inHostIndex;
}

int ReadAndWriteHostName(int inIndicator)
{
	int inResult,i,inCount;
	int inBatchNumOfRecord;
    int *pinTransDataid;
	BYTE szBatchNo[6 + 1] = {0};
	BYTE szInvoiceNo[6 + 1] = {0};
	BYTE szDate[12 + 1] = {0};
	BYTE szTime[12 + 1] = {0};
	BYTE szPAN[30 + 1] = {0};
	BYTE szAmountTemp[40 + 1] = {0};
	BYTE szAmount[40 + 1] = {0};
	BYTE szDateTime[40 + 1] = {0};
	BYTE szTitle[40 + 1] = {0};

	char szHostMenu[HOSTNAME_BUFFER_SIZE];
	char szHostName[400][400];
	int inCPTID[400];
	int inLoop = 0;
	BYTE szHostIndex[5 + 1] = {0};
	BYTE szHostLabel[40 + 1] = {0};

	vdDebug_LogPrintf("--ReadAndWriteHostName--");
	vdDebug_LogPrintf("inIndicator=[%d]", inIndicator);
	
	memset(szHostMenu, 0x00, sizeof(szHostMenu));
	memset(szHostName, 0x00, sizeof(szHostName));
	memset((char*)inCPTID, 0x00, sizeof(inCPTID));

	inCTOS_TEMPCheckAndSelectMutipleMID();
	
	inHDTReadHostName(szHostName, inCPTID);

	vdDebug_LogPrintf("srTransRec.MITid %d", srTransRec.MITid);

	inMMTReadHostName(szHostName, inCPTID, srTransRec.MITid, inIndicator);
	
	vdDeleteFile(TRANS_DETAIL_FILE); // Delete file
	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	memset(strWriteFile.szWriteTemp, 0x00, sizeof(strWriteFile.szWriteTemp));
	strWriteFile.inWriteSize = 0;

	for (inLoop = 0; inLoop < 150; inLoop++)
	{	
		if (szHostName[inLoop][0]!= 0)
		{
			memset(szHostIndex, 0x00, sizeof(szHostIndex));
			memset(szHostLabel, 0x00, sizeof(szHostLabel));
			memset(strWriteFile.szWriteTemp, 0x00, sizeof(strWriteFile.szWriteTemp));	
		
			sprintf(szHostIndex, "%d", inCPTID[inLoop]);
			strcpy(szHostLabel, szHostName[inLoop]);

			strcpy(strWriteFile.szWriteTemp, szHostIndex);
			strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
			strcat(strWriteFile.szWriteTemp, szHostLabel);
			strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
			strcat(strWriteFile.szWriteTemp, "x");
			strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
			strcat(strWriteFile.szWriteTemp, "x");
			strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
			strcat(strWriteFile.szWriteTemp, "x");
			strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
			strcat(strWriteFile.szWriteTemp, "x");
			strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
			strcat(strWriteFile.szWriteTemp, "x");
			strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
			strcat(strWriteFile.szWriteTemp, "x");
			strcat(strWriteFile.szWriteTemp, DELIM_PIPE);
			strcat(strWriteFile.szWriteTemp, "x");
			strcat(strWriteFile.szWriteTemp, DELIM_PIPE);

			strcat(strWriteFile.szWriteTemp, "\n");
			strWriteFile.inWriteSize += strlen(strWriteFile.szWriteTemp);
			strcat(strWriteFile.szWriteData, strWriteFile.szWriteTemp);

			vdDebug_LogPrintf("inWriteSize=[%d] szWriteTemp=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteTemp);
		}
		else
			break;
	}

	vdDebug_LogPrintf("SaveFile->inWriteSize=[%d] szWriteTemp=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
	
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, TRANS_DETAIL_FILE);
	
	return d_OK;
}

// Get transaction name and image filename from array -- sidumili
int inGetTransImageList(char* szMenuImageList, int inMenu, char* szDelimeter)
{
	int inLoop = 0;
	BYTE szTemp[50 + 1] = {0}; 
	int inMenuNumRecs = 0;
	int inImageNumRecs = 0;
	int i = 0;
	int x = 0;
	char szMenuItems[9000] = {0};
	
	vdDebug_LogPrintf("--inGetTransImageList--");
	vdDebug_LogPrintf("inMenu=[%d]", inMenu);

	// inMenu
	// 1 = Base on menuid
	// 2 = Quick menu
	memset(szMenuItems, 0x00, sizeof(szMenuItems));

	inImageNumRecs = inIMGRead(); // Load Image Array
	//inMenuNumRecs = inReadDynamicMenu(1); by inMenuID
	inMenuNumRecs = inReadDynamicMenu(inMenu); // Quick Menu -- sidumili

	vdDebug_LogPrintf("inImageNumRecs=[%d], inMenuNumRecs=[%d]", inImageNumRecs, inMenuNumRecs);
	
	memset(szMenuItems, 0, sizeof(szMenuItems));
	for (i = 0; i < inMenuNumRecs; i++)
	{
		if((strlen(strDynamicMenu[i].szButtonItemLabel) > 0) && (i != inMenuNumRecs))
		{
			// Get image filename
			memset(szTemp, 0x00, sizeof(szTemp));
			sprintf(szTemp, "%s%s%s", strDynamicMenu[i].szButtonItemLabel, szDelimeter,  "ic_blank.png");
			for (x = 0; x < inImageNumRecs; x++)
			{
				if (strcmp(strDynamicMenu[i].szButtonItemLabel, strImageList[x].szTransactionTypeName) == 0)
				{
					//sprintf(szTemp, "%s%s%s", strDynamicMenu[i].szButtonItemLabel, szDelimeter, strImageList[x].szImageFileName);
					sprintf(szTemp, "%s%s%s", strImageList[x].szTranTitle, szDelimeter, strImageList[x].szImageFileName);
					break;
				}
			}

			strcat(szMenuItems, szTemp);
			strcat(szMenuItems, "|");
		}
	}
	
	vdDebug_LogPrintf("szMenuItems=[%s]", szMenuItems);
	strcpy(szMenuImageList, szMenuItems);

	return d_OK;
}

void vdCTOS_ECRSettings(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0, inRet;
	BYTE inKey = d_NO;
	BYTE szOutputBuf[512] = {0};
	BYTE szTemp[40 + 1] = {0};
    
	//CTOS_LCDTClearDisplay();

    /*if(inCheckAllBatchEmtpy() > 0)
    {
        vdDisplayErrorMsg3(1,8,"EDIT NOT ALLOWED","BATCH NOT EMPTY","");
        return;
    }*/

	vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(100);
	
	inTCTRead(1);
    inHDTRead(1);

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide
	
	// Host Index
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inHostIndex", "Host Index", strHDT.inHostIndex, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// Host Name
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szHostLabel", "Host Name", strHDT.szHostLabel, 16, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");		

	// ECR Flag
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fECR", "ECR Flag", strTCT.fECR, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// ECR Port
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "byRS232ECRPort", "ECR Port", strTCT.byRS232ECRPort, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// ECR Baud Rate
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "fECRBaudRate", "ECR Baud Rate", strTCT.fECRBaudRate, 1, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Dummy
	//memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	//sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d", FLD_TYPE_NUMERIC, "szDummy", "Dummy Field", "Dummy", BATCH_NO_ASC_SIZE, FALSE);
	//strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	//strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("ECR SETTING|x", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|
			//GetDelimitedString(szOutputBuf, 2, '|', ); // inHostIndex
			//GetDelimitedString(szOutputBuf, 3, '|', strHDT.szHostLabel); // szHostLabel

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 4, '|', szTemp); // fECR
			strTCT.fECR= (atoi(szTemp) > 0 ? TRUE: FALSE);

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 5, '|', szTemp); // byRS232ECRPort
			strTCT.byRS232ECRPort= atoi(szTemp);
			
			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 6, '|', szTemp); // fECRBaudRate
			strTCT.fECRBaudRate= (atoi(szTemp) > 0 ? TRUE: FALSE);

			// Update TCT
			inTCTSave(1);
		
			//vdDisplayMessageBox(1, 8, "ECR UPDATE SETTING", "COMPLETE", "PLEASE RESTART TERMINAL", MSG_TYPE_INFO);
			inDisplayMessageBoxWithButtonConfirmation(1,8,"ECR setting","Update complete","Please restart terminal","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case 0xFF:
			//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");	
}

USHORT CTOS_KBDBufFlush(void)
{
	vdDebug_LogPrintf("CTOS_KBDBufFlush, overload...");
}

void vdCTOSS_PrinterEnd(void)
{
	int inStatus = 0;
	
	vdDebug_LogPrintf("vdCTOSS_PrinterEnd overload...");

	do
    {
        // CTOS_Delay(2000); 
        inStatus = inCTOSS_PrinterEndStatus();
        // CTOS_Delay(300);
        vdDebug_LogPrintf("inCTOSS_PrinterEndStatus=[%d]",inStatus);
        if (inStatus == d_PRINTER_REDIRECT_TIMEOUT)
            break;
        else if (inStatus==d_PRINTER_HEAD_OVERHEAT)//for enhance S1E continue print.
        {
            break;
        }
        else
            CTOS_Delay(50);
    }while (inStatus != d_OK);
}

// added transactions menu -- sidumili
int inCTOS_SelectTransactionsMenu(void)
{
	int inRet=d_NO;
    char szDebug[40 + 1]={0};
    char szHeaderString[24+1];
    int bHeaderAttr = 0x01+0x04;
	int key = -1; 	
    char szChoiceMsg[9000 + 1];
    BYTE szTitle[20];
    BYTE szDisMsg[528];
	BYTE szMessage[9000 + 1] = {0};
	BYTE szAmount[40 + 1] = {0};

	vdDebug_LogPrintf("--inCTOS_SelectTransactionsMenu--");
	
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    inGetTransImageList(szChoiceMsg, 1, "#");
	vdDebug_LogPrintf("szChoiceMsg=[%s]", szChoiceMsg);

	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	
	GetTransAmount(szAmount);
	memset(szMessage, 0x00, sizeof(szMessage));
	sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_IMAGE_VIEW, MENU_2_COL_COUNT, "Transactions", "Select a type of transaction");
	strcat(szMessage, szChoiceMsg);
	
	key = inMenuSelection(szMessage);	
    vdDebug_LogPrintf("inMenuSelection=[%d], Selected=[%s]", key, strGBLVar.szGBLvProcessDesc);

	if (key == 0xFF || key == d_TIMEOUT) //TimeOut
    {
		//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return d_NO;
    }
    else if (key == d_KBD_CANCEL || key == d_USER_CANCEL)
    {
		//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
        return d_NO;
    }

	key = -1;
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
	inGetTransMenuList(szChoiceMsg, 1);
	key = inGetMenuKeySelected(szChoiceMsg, strGBLVar.szGBLvProcessDesc);
    vdDebug_LogPrintf("inGetMenuKeySelected=[%d],strGBLVar.szGBLvProcessDesc=[%s]", key, strGBLVar.szGBLvProcessDesc);

	if (key > 0)
	{	
		//vdDisplayMessageBox(1, 8, "", "", "", MSG_TYPE_NONE);
		//CTOS_Delay(100);

		memset(strGBLVar.szGBLButtonItemLabel, 0x00, sizeof(strGBLVar.szGBLButtonItemLabel));
		memset(strGBLVar.szGBLButtonItemFunc, 0x00, sizeof(strGBLVar.szGBLButtonItemFunc));
		memset(strGBLVar.szGBLSubMenuName, 0x00, sizeof(strGBLVar.szGBLSubMenuName));
		
		inGetDynamicMenuDetail(strGBLVar.szGBLvProcessDesc); // read dynamicmenu -- sidumili

		// execute selected transaction
		vdDebug_LogPrintf("inGetDynamicMenuDetail, strGBLVar.szGBLButtonItemFunc=[%s]", strGBLVar.szGBLButtonItemFunc);
		if (strlen(strGBLVar.szGBLButtonItemFunc) > 0)
		{
			inCTOSS_ExeFunction(strGBLVar.szGBLButtonItemFunc);
			if(strlen(strGBLVar.szGBLSubMenuName) > 0)
				inCTOSS_ExeFunction(strGBLVar.szGBLSubMenuName);
		}
		else
		{
			return d_NO;
		}
		
	}
	
	return d_OK;
}

int inGetTransMenuList(char* szOutMenuList, int inMenu)
{
	int inMenuNumRecs = 0;
	int i = 0;
	char szMenuItems[9000] = {0};
	
	vdDebug_LogPrintf("--inGetTransMenuList--");
	vdDebug_LogPrintf("inMenu=[%d]", inMenu);
	
	memset(szMenuItems, 0x00, sizeof(szMenuItems));

	inMenuNumRecs = inReadDynamicMenu(inMenu); // Quick Menu -- sidumili

	vdDebug_LogPrintf("inMenuNumRecs=[%d]", inMenuNumRecs);
	
	memset(szMenuItems, 0, sizeof(szMenuItems));
	for (i = 0; i < inMenuNumRecs; i++)
	{
		if((strlen(strDynamicMenu[i].szButtonItemLabel) > 0) && (i != inMenuNumRecs))
		{
			strcat(szMenuItems, strDynamicMenu[i].szButtonItemLabel);
			strcat(szMenuItems, "|");
		}
	}
	
	vdDebug_LogPrintf("szMenuItems=[%s]", szMenuItems);
	strcpy(szOutMenuList, szMenuItems);

	return d_OK;
}

void vdCTOS_TMSSetting(void)
{
    BYTE bRet;
    BYTE szInputBuf[15+1];
    int inResult,inResult1;
    TRANS_TOTAL stBankTotal;
    BYTE strOut[30],strtemp[17],key;
    USHORT ret;
    USHORT usLen;
    BYTE szTempBuf[12+1];
    BOOL isKey;
    int shHostIndex = 1;
    int inNum = 0, inRet;
    BYTE inKey = d_NO;
    BYTE szOutputBuf[512] = {0};
    BYTE szTemp[40 + 1] = {0};
    USHORT inLenMID =0;

	vdDebug_LogPrintf("--vdCTOS_TMSSetting--");

	EditAgain:
    
	if(inCTOS_ChkBatchNotEmpty() != d_NO)
	{	
		//vdDisplayMessageBox(1, 8, "EDIT NOT ALLOWED", "BATCH NOT EMPTY", "", MSG_TYPE_ERROR);
		inDisplayMessageBoxWithButtonConfirmation(1,8,"Edit not allowed","Batch not empty","","",MSG_TYPE_ERROR, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(2000);
	    return;
	}

	vdDisplayMessageBox(1, 8, "Processing", "", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
	CTOS_Delay(100);

	inTCTRead(1);
	inTMSEXRead(1); // Read TMSEX

	vdDeleteFile(EDIT_FILE); // Delete file

	memset(strWriteFile.szWriteData, 0x00, sizeof(strWriteFile.szWriteData));
	strWriteFile.inWriteSize = 0;

	// File format::Field Type|FieldName|Field Description|Field Value|Field Length|Flag Check Length|isHide

	// TMSEXid
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "TMSEXid", "TMSEX id", strTMSEX.TMSEXid, 2, FALSE, TRUE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szTMSID
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTMSID", "TMS ID", strTMSEX.szTMSID, 8, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// szSerialNo
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szSerialNo", "Serial Number", strTMSEX.szSerialNo, 16, TRUE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szGroupName
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szGroupName", "Group Name", strTMSEX.szGroupName, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szUserName
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szUserName", "User Name", strTMSEX.szUserName, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szPassword
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szPassword", "Password", strTMSEX.szPassword, 30, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// szTMSHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTMSHostIP", "TMS Host IP", strTMSEX.szTMSHostIP, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inTMSHostPortNum
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inTMSHostPortNum", "TMS Port Number", strTMSEX.inTMSHostPortNum, 4, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// szTLSHostIP
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTLSHostIP", "TLS Host IP", strTMSEX.szTLSHostIP, 20, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// inTLSHostPortNum
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "inTLSHostPortNum", "TLS Port Number", strTMSEX.inTLSHostPortNum, 4, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// szTMSMode
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d|%d", FLD_TYPE_NUMERIC, "szTMSMode", "TMS Mode (1-TCP/2-TLS)", strTMSEX.szTMSMode, 2, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");

	// usTMSGap
	memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	sprintf(strWriteFile.szTemp, "%d|%s|%s|%d|%d|%d|%d", FLD_TYPE_NUMERIC, "usTMSGap", "TMS Gap", strTCT.usTMSGap, 4, FALSE, FALSE);
	strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	strcat(strWriteFile.szWriteData, "\n");
	
	// Dummy
	//memset(strWriteFile.szTemp, 0x00, sizeof(strWriteFile.szTemp));
	//sprintf(strWriteFile.szTemp, "%d|%s|%s|%s|%d|%d", FLD_TYPE_NUMERIC, "szDummy", "Dummy Field", "Dummy", BATCH_NO_ASC_SIZE, FALSE);
	//strcat(strWriteFile.szWriteData, strWriteFile.szTemp);
	//strcat(strWriteFile.szWriteData, "\n");

	
	strWriteFile.inWriteSize += strlen(strWriteFile.szWriteData);
	vdDebug_LogPrintf("inWriteSize=[%d] szWriteData=[%s]", strWriteFile.inWriteSize, strWriteFile.szWriteData);
		
	// Save to file
	inSaveFile((char*)strWriteFile.szWriteData, strWriteFile.inWriteSize, EDIT_FILE);

	memset(szOutputBuf, 0x00, sizeof(szOutputBuf));
	inKey = usCTOSS_EditInfoListViewUI("TMS CONFIG|x", szOutputBuf);

	vdDebug_LogPrintf("AAA - szOutputBuf[%s]", szOutputBuf);

	vdDebug_LogPrintf("usCTOSS_EditInfoListViewUI::inKey=[%d]", inKey);

	switch (inKey)
	{
		case d_KBD_CANCEL:
			//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case d_OK:
			// Sample data receive from android: CONFIRM|19|MCC ONE APP|1|0|10000001|100000000000001|6003000000|0300|6226|6226|99999999|88888888|1|000002|340016|000009|

			GetDelimitedString(szOutputBuf, 4, '|', strTMSEX.szSerialNo); // szSerialNo
			GetDelimitedString(szOutputBuf, 5, '|', strTMSEX.szGroupName); // szGroupName
			GetDelimitedString(szOutputBuf, 6, '|', strTMSEX.szUserName); // szUserName
			GetDelimitedString(szOutputBuf, 7, '|', strTMSEX.szPassword); // szPassword
			GetDelimitedString(szOutputBuf, 8, '|', strTMSEX.szTMSHostIP); // szTMSHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 9, '|', szTemp); // inTMSHostPortNum
			strTMSEX.inTMSHostPortNum= atoi(szTemp);

			GetDelimitedString(szOutputBuf, 10, '|', strTMSEX.szTLSHostIP); // szTLSHostIP

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 11, '|', szTemp); // inTLSHostPortNum
			strTMSEX.inTLSHostPortNum= atoi(szTemp);
			
			GetDelimitedString(szOutputBuf, 12, '|', strTMSEX.szTMSMode); // szTMSMode

			memset(szTemp, 0x00, sizeof(szTemp));
			GetDelimitedString(szOutputBuf, 13, '|', szTemp); // usTMSGap
			strTCT.usTMSGap= atoi(szTemp);
		
			inTMSEXSave(1);
			inTCTSave(1);
			
			//vdDisplayMessageBox(1, 8, strIIT.szIssuerLabel, "ISSUER SETTING", " UPDATE COMPLETE", MSG_TYPE_INFO);
			inDisplayMessageBoxWithButtonConfirmation(1,8,strIIT.szIssuerLabel,"TMS config","Update complete","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
			break;
		case 0xFF:
			//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_WARNING);
			CTOS_Beep();
			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
			break;
	}
	
	CTOS_Beep();
	CTOS_Delay(1000);
	usCTOSS_LCDDisplay(" ");
	
	//goto EditAgain;
	return;
	
}

int inCTOS_Hotel(void)
{
	int inRet=d_NO;
	char szDebug[40 + 1]={0};
	char szHeaderString[24+1];
	int bHeaderAttr = 0x01+0x04;
	int key = 0;	
	char szChoiceMsg[30 + 1];
	BYTE szTitle[20];
	BYTE szDisMsg[528];
	BYTE szMessage[2000 + 1] = {0};
	BYTE szAmount[40 + 1] = {0};

	vdDebug_LogPrintf("--inCTOS_Hotel--");
	
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
	//strcpy(szChoiceMsg,"Reprint|");
	//strcpy(szChoiceMsg,"Report|");
	//strcat(szChoiceMsg,"Review");
	sprintf(szChoiceMsg, "%s", "Card Verify#ic_cardverify.png|Completion#ic_completion.png||");

	vdDebug_LogPrintf("szChoiceMsg=[%s]", szChoiceMsg);

	//memset(szTitle, 0x00, sizeof(szTitle));
	//szGetTransTitle(srTransRec.byTransType, szTitle);
	
	GetTransAmount(szAmount);
	memset(szMessage, 0x00, sizeof(szMessage));
	sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_IMAGE_VIEW, MENU_2_COL_COUNT, "Transaction", "Hotel");
	strcat(szMessage, szChoiceMsg);
	
	key = inMenuSelection(szMessage);	
	vdDebug_LogPrintf("inMenuSelection=[%d]", key);

	if (key == 0xFF || key == d_TIMEOUT) //TimeOut
	{
		//vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
		return d_NO;
	}
	else if (key == d_KBD_CANCEL || key == d_USER_CANCEL)
	{
		//vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		//CTOS_Delay(1000);
		return d_NO;
	}

	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
	sprintf(szChoiceMsg, "%s", "Card Verify|Completion");
	key = inGetMenuKeySelected(szChoiceMsg, strGBLVar.szGBLvProcessDesc);
	vdDebug_LogPrintf("inGetMenuKeySelected=[%d]", key);

	if (key > 0)
	{
		switch (key)
		{
			case 1: // Card Verify
				inCTOS_PREAUTH();
				break;
			case 2: // Completion
				inCTOS_SALE_OFFLINE();
				break;
		}
	}
	else
	{
		return d_NO;
	}
		
	return d_OK;
}

int inGetTransWalletImageList(char* szMenuImageList, int inMenu, char* szDelimeter)
{
	int inLoop = 0;
	BYTE szTemp[50 + 1] = {0}; 
	int inMenuNumRecs = 0;
	int inImageNumRecs = 0;
	int i = 0;
	int x = 0;
	char szMenuItems[9000] = {0};
	
	vdDebug_LogPrintf("--inGetTransImageList--");
	vdDebug_LogPrintf("inMenu=[%d]", inMenu);

	// inMenu
	// 1 = Base on menuid
	// 2 = Quick menu
	memset(szMenuItems, 0x00, sizeof(szMenuItems));

	inImageNumRecs = inIMGRead(); // Load Image Array
	//inMenuNumRecs = inReadDynamicMenu(1); by inMenuID
	inMenuNumRecs = inReadDynamicMenu(inMenu); // Quick Menu -- sidumili

	vdDebug_LogPrintf("inImageNumRecs=[%d], inMenuNumRecs=[%d]", inImageNumRecs, inMenuNumRecs);
	
	memset(szMenuItems, 0, sizeof(szMenuItems));
	for (i = 0; i < inMenuNumRecs; i++)
	{
		if((strlen(strDynamicMenu[i].szButtonItemLabel) > 0) && (i != inMenuNumRecs))
		{
			// Get image filename
			memset(szTemp, 0x00, sizeof(szTemp));
			sprintf(szTemp, "%s%s%s", strDynamicMenu[i].szButtonItemLabel, szDelimeter,  "ic_blank.png");
			for (x = 0; x < inImageNumRecs; x++)
			{
				if (strcmp(strDynamicMenu[i].szButtonItemLabel, strImageList[x].szTransactionTypeName) == 0)
				{
					//sprintf(szTemp, "%s%s%s", strDynamicMenu[i].szButtonItemLabel, szDelimeter, strImageList[x].szImageFileName);
					sprintf(szTemp, "%s%s%s", strImageList[x].szTranTitle, szDelimeter, strImageList[x].szImageFileName);
					break;
				}
			}

			strcat(szMenuItems, szTemp);
			strcat(szMenuItems, "|");
		}
	}
	
	vdDebug_LogPrintf("szMenuItems=[%s]", szMenuItems);
	strcpy(szMenuImageList, szMenuItems);

	return d_OK;
}

int inCTOS_CheckMustSettleBDOHost(void)
{
	int inTxnCount=0;
    int inEnable = 0;
	int inHostIndex_Temp = 0;
	BYTE szCurrDate[8] = {0};
	CTOS_RTC SetRTC;
    STRUCT_FILE_SETTING strFile;
    ACCUM_REC srAccumRec;
	BOOL fOptOut;

	vdDebug_LogPrintf("inCTOS_CheckMustSettleBDOHost");
#if 0	
	if(inHDTReadData(BDO_OPTOUT_HDT_INDEX) != d_OK)
	{
		vdDebug_LogPrintf("OPTOUT HOST DISABLED");
		//vdDisplayErrorMsgResp2("","HDT READ","ERROR");
		//return d_NO;
	}
#endif
	if (srTransRec.byTransType == SALE_OFFLINE){
		 fOptOut = inFLGGet("fCompOptOut");
		 vdDebug_LogPrintf("testlang compopt out");
	
		 if (fOptOut == 1){
			 vdDebug_LogPrintf("testlang reload comp data");
			 if(inHDTReadData(BDO_COMPLETION_OPTOUT_HDT_INDEX) != d_OK)
				  vdDebug_LogPrintf("COMPLETION OPTOUT HOST DISABLED"); 	 
		 }
			 
	}else{
	
		 fOptOut = inFLGGet("fStraightOptOut");	
		 if (fOptOut == 1){
			 if(inHDTReadData(BDO_OPTOUT_HDT_INDEX) != d_OK)
				 vdDebug_LogPrintf("OPTOUT HOST DISABLED");
		 }
	
	 }


	vdDebug_LogPrintf("test host enable %d", strHDT_Temp.fHostEnable);

	if(strHDT_Temp.fHostEnable == TRUE)
	{
		fBDOOptOutHostEnabled = TRUE;
	}

	vdDebug_LogPrintf("test MMTID %d", srTransRec.MITid);
	
	if(fBDOOptOutHostEnabled == TRUE){
		//inMMTReadBDOHost(BDO_OPTOUT_HDT_INDEX,srTransRec.MITid);		
		if (srTransRec.byTransType == SALE_OFFLINE)
		   inMMTReadBDOHost(BDO_COMPLETION_OPTOUT_HDT_INDEX,srTransRec.MITid);
		else   
		   inMMTReadBDOHost(BDO_OPTOUT_HDT_INDEX,srTransRec.MITid);
	}else
		inMMTReadBDOHost(BDO_HDT_INDEX,srTransRec.MITid);
	
	vdDebug_LogPrintf("fMustSettFlag[%d]",strMMT_Temp[0].fMustSettFlag);
	vdDebug_LogPrintf("MMTid[%d]",strMMT_Temp[0].MMTid);
	vdDebug_LogPrintf("HDTid[%d]",strMMT_Temp[0].HDTid);
	
	if(strTCT.fChangeDateMustSettle == TRUE)
	{
		memset(szCurrDate, 0x00, sizeof(szCurrDate));
		CTOS_RTCGet(&SetRTC);
		sprintf(szCurrDate,"%02d%02d%02d", SetRTC.bYear, SetRTC.bMonth, SetRTC.bDay);
		vdDebug_LogPrintf("CURRENT DATE: [%s] :: SETTLE DATE: [%s]",szCurrDate,strMMT_Temp[0].szSettleDate); 

		if ((strMMT_Temp[0].fMMTEnable == TRUE) && (wub_str_2_long(szCurrDate) >= wub_str_2_long(strMMT_Temp[0].szSettleDate)))
		{
			
			//srTransRec.MITid=strMMT[0].MITid; 
			inHostIndex_Temp = strHDT.inHostIndex;
			if(fBDOOptOutHostEnabled == TRUE){
				//strHDT.inHostIndex = BDO_OPTOUT_HDT_INDEX;
				if (srTransRec.byTransType == SALE_OFFLINE)
				   strHDT.inHostIndex = BDO_COMPLETION_OPTOUT_HDT_INDEX;
				else   
				   strHDT.inHostIndex = BDO_OPTOUT_HDT_INDEX;
			}else				
				strHDT.inHostIndex = BDO_HDT_INDEX;
			vdCTOS_GetAccumName(&strFile, &srAccumRec);
			if ((inMyFile_CheckFileExist(strFile.szFileName)) > 0)
			{
				inTxnCount = inCheckHostBatchEmtpy(strHDT.inHostIndex,srTransRec.MITid);
				
				 if(inTxnCount > 0)
				 {	
				 	 strMMT_Temp[0].fMustSettFlag = 1;
					 inMMTBDOMustSettleSave(strMMT_Temp[0].MMTid);
				 }
			}
		}
	}

    inEnable = strMMT_Temp[0].fMustSettFlag;
    vdDebug_LogPrintf("fMustSettFlag[%d] :: inEnable[%d]",strMMT_Temp[0].fMustSettFlag,inEnable);
	strHDT.inHostIndex = inHostIndex_Temp;
	
    if(1 == inEnable) 
    {
       //vdSetErrorMessage("MUST SETTLE");
       return(ST_ERROR);
    }
    else
        return(ST_SUCCESS);
}

// get approve transaction info -- sidumili
char* getApprovedTransInfo(void)
{
	BYTE byLen = 0;
    int inRet = 0;

    BYTE szInBuf[1024 + 1];
    BYTE szOutBuf[1024 + 1];

	char szTitle[40 + 1] = {0};
	BYTE szTemp[1024 + 1] = {0};
	BYTE szBatchNo[10 + 1] = {0};
	BYTE szInvoiceNo[10 + 1] = {0};
	BYTE szPAN[40 + 1] = {0};
	BYTE szDate[40 + 1] = {0};
	BYTE szTime[40 + 1] = {0};
	BYTE szDateTime[40 + 1] = {0};
	BYTE szAmountTemp[40 + 1] = {0};
	BYTE szAmount[40 + 1] = {0};
	BYTE szMsgType[40 + 1] = {0};

	vdDebug_LogPrintf("--getApprovedTransInfo--");
	
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

	wub_hex_2_str(srTransRec.szBatchNo,szBatchNo,3);
	wub_hex_2_str(srTransRec.szInvoiceNo, szInvoiceNo, INVOICE_BCD_SIZE);
	vdCTOS_FormatPAN2(strIIT.szMaskCustomerCopy, srTransRec.szPAN, szPAN);

	GetTransDateTime(szDate, szTime);
	sprintf(szDateTime, "%s %s", szDate, szTime);

	inCSTReadEx(strHDT.inCurrencyIdx);
	wub_hex_2_str(srTransRec.szTotalAmount, szAmountTemp, AMT_BCD_SIZE);
	vdCTOS_FormatAmount(strCST.szAmountFormat, szAmountTemp,szAmount);// patrick add code 20141216	
	memset(szAmountTemp, 0x00, sizeof(szAmountTemp));
	sprintf(szAmountTemp,"%s%s %s", (srTransRec.byTransType != VOID ? "" : "-"), strCST.szCurSymbol,szAmount);

	memset(szTemp, 0x00, sizeof(szTemp));
	inGetBoxMessageType(MSG_TYPE_SUCCESS, szMsgType);

	strcpy(szTemp, (fRePrintFlag ? "" : "Transaction successful"));
	strcat(szTemp, "|");
	strcat(szTemp, (fRePrintFlag ? "Tap to reprint receipt" : "Tap to print receipt"));
	strcat(szTemp, "|");
	strcat(szTemp, szMsgType);
	strcat(szTemp, "|");
	strcat(szTemp, srTransRec.szHostLabel);
	strcat(szTemp, "|");
	strcat(szTemp, szPAN);
	strcat(szTemp, "|");
	strcat(szTemp, strIIT.szIssuerLabel);
	strcat(szTemp, "|");
	strcat(szTemp, szBatchNo);
	strcat(szTemp, "|");
	strcat(szTemp, szInvoiceNo);
	strcat(szTemp, "|");
	strcat(szTemp, srTransRec.szAuthCode);
	strcat(szTemp, "|");
	strcat(szTemp, szDateTime);
	strcat(szTemp, "|");
	strcat(szTemp, szTitle);
	strcat(szTemp, "|");
	strcat(szTemp, szAmountTemp);

	vdDebug_LogPrintf("getApprovedTransInfo,szTemp=[%s]", szTemp);
	return szTemp;
}

