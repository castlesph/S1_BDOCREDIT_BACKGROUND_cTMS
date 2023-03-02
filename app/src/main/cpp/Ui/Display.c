#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>


#include "../Includes/msg.h"
#include "../Includes/wub_lib.h"
#include "../Includes/myEZLib.h"
#include "../Includes/POSTypedef.h"
#include "../Includes/POSTrans.h"

#include "display.h"
#include "../FileModule/myFileFunc.h"
#include "../print/Print.h"
#include "../Includes/CTOSinput.h"
#include "../UI/Display.h"
#include "../Comm/V5Comm.h"
#include "..\Debug\Debug.h"
#include "../PinPad/pinpad.h"
#include "../Includes/POSSale.h"
#include "../UIapi.h"
#include "../Includes/POSSetting.h"

extern BOOL fInstApp; 

extern char gblszAmt[20+1]; //aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE 1 of 8
extern USHORT GPRSCONNETSTATUS;

//smac
extern BOOL fSMACTRAN;
extern fAdviceTras;

#define ERRORLEN 30
#define ERRORLEN1 30
static char szErrorMessage[ERRORLEN+1];
static char szErrorMessage1[ERRORLEN1+1];
extern BYTE byPackTypeBeforeDCCLog;

extern BOOL	fRePrintFlag;

void setLCDReverse(int line,int position, char *pbBuf)
{
    int iInitX = 0;
    int lens = 0;

    //Set the reverse attribute of the character //
    CTOS_LCDTSetReverse(TRUE);  //the reverse enable // 
    
    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            CTOS_LCDTPrintXY(1, line, pbBuf);
            break;
        case DISPLAY_POSITION_CENTER:
            lens = strlen(pbBuf);
            iInitX = (16 - lens) / 2 + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
        case DISPLAY_POSITION_RIGHT:
            lens = strlen(pbBuf);
            iInitX = 16 - lens + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
    }

    //Set the reverse attribute of the character //
    CTOS_LCDTSetReverse(FALSE); //the reverse enable //     
}

/* BDO-00122: Change line busy to line busy, please try again - start -- jzg */
#if 1
void setLCDPrint(int line,int position, char *pbBuf)
{
	int iInitX = 0;
	int lens = 0;

	return; // uneeded for android
	
	CTOS_LCDTPrintXY(1, line, "                      ");
	
	switch(position)
	{
		case DISPLAY_POSITION_LEFT:
			CTOS_LCDTPrintXY(1, line, pbBuf);
			break;

		case DISPLAY_POSITION_CENTER:
			lens = strlen(pbBuf);

			if((strTCT.byTerminalType == 1) || (strTCT.byTerminalType == 2))
				iInitX = ((22 - lens) / 2) + 1;
			else
				iInitX = ((60 - lens) / 2) + 1;

			CTOS_LCDTPrintXY(iInitX, line, pbBuf);
			break;

		case DISPLAY_POSITION_RIGHT:
			lens = strlen(pbBuf);
			iInitX = 16 - lens + 1;
			CTOS_LCDTPrintXY(iInitX, line, pbBuf);
			break;
	}
}
/* BDO-00122: Change line busy to line busy, please try again - end -- jzg */
#else
void setLCDPrint(int line,int position, char *pbBuf)
{
    short shXPos=0, shLen=0;
    int iInitX=0, lens=0;

    shLen = strlen(pbBuf);
    
    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            CTOS_LCDTPrintXY(1, line, pbBuf);
            break;

        case DISPLAY_POSITION_CENTER:
            #if 0
            lens = strlen(pbBuf);
            
            if((strTCT.byTerminalType == 1) || (strTCT.byTerminalType == 2))
               iInitX = ((22 - lens) / 2) + 1;
            else
                iInitX = ((30 - lens) / 2) + 1;
            
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            #endif
            lens = strlen(pbBuf);
            if(lens >= 20)
                iInitX=1;
            else    
                iInitX = (MAX_CHAR_PER_LINE - lens*2) / 2 ;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
        break;


        case DISPLAY_POSITION_RIGHT:
            shXPos = MAX_CHAR_PER_LINE - (shLen * 2);
            if(shXPos == 0)
                shXPos = 1;
            CTOS_LCDTPrintXY(shXPos, line, pbBuf);
            break;
    }
}


#endif

void showAmount(IN  BYTE bY, BYTE bStrLen, BYTE *baBuf)
{
    int i;
    
    if(bStrLen > 2)
    {
        CTOS_LCDTPrintXY(13, bY, "0.00");
        for(i = 0;i < bStrLen; i++)
        {
            if ((16 - bStrLen + 1 + i) > 14)
                CTOS_LCDTPutchXY(16 - bStrLen + 1 + i, bY, baBuf[i]);
            else
                CTOS_LCDTPutchXY(16 - bStrLen + i, bY, baBuf[i]);
        }
    }
    else
    {
        CTOS_LCDTPrintXY(13, bY, "0.00");
        for(i = 0;i < bStrLen; i++)
        {
            CTOS_LCDTPutchXY(16 - bStrLen + 1 + i, bY, baBuf[i]);
        }
    }
}

void vduiDisplayInvalidTLE(void)
{
    
    vduiClearBelow(2);
    vduiWarningSound();
    vduiDisplayStringCenter(3,"INVALID SESSION");
    vduiDisplayStringCenter(4,"KEY, PLEASE");
    vduiDisplayStringCenter(5,"DWD SESSION KEY");
    vduiDisplayStringCenter(6,"--INSTRUCTION---");
    CTOS_LCDTPrintXY(1,7,"PRESS [F2] THEN");
    CTOS_LCDTPrintXY(1,8,"PRESS [3]");
    
    CTOS_Delay(2500);
}


void szGetTransTitle(BYTE byTransType, BYTE *szTitle)
{    
    int i;
    szTitle[0] = 0x00;
    
    //vdDebug_LogPrintf("**szGetTransTitle START byTransType[%d]Orig[%d]**", byTransType, srTransRec.byOrgTransType);

//smac
	if (fSMACTRAN){
		if (byTransType == SMAC_ACTIVATION)
			strcpy(szTitle, "SMAC LOGON");
		if (byTransType == SALE_OFFLINE)		
			strcpy(szTitle, "AWARD POINTS");
		if (byTransType == SMAC_BALANCE)	
			strcpy(szTitle, "PTS INQUIRY");
		if (byTransType == SALE)		
			strcpy(szTitle, "REDEMPTION");

    	i = strlen(szTitle);
    	szTitle[i]=0x00;
    	return ;
	}
		
//smac

	if ((fInstApp == TRUE) && (byTransType == SALE)){
		strcpy(szTitle, "Installment");
		return;
	}

    switch(byTransType)
    {
        case SALE:
            strcpy(szTitle, "SALE");
            break;
        case PRE_AUTH:
			//0826
            //strcpy(szTitle, "PRE AUTH");
            strcpy(szTitle, "Card Verify");
			//0826
            break;
        case PRE_COMP:
            strcpy(szTitle, "Auth Comp");
            break;
        case REFUND:
            strcpy(szTitle, "Refund");
            break;
        case VOID:
            if(REFUND == srTransRec.byOrgTransType)
                strcpy(szTitle, "VOID REFUND");
            else if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
				if(memcmp(srTransRec.szAuthCode,"Y1",2) == 0)
				{
                    strcpy(szTitle, "VOID OFFLINE");					
				}
                else
                {
                    if(strTCT.fCheckout == 1)	
                        strcpy(szTitle, "VOID CHECKOUT");
                    else
                        strcpy(szTitle, "VOID COMPLETION");			
                }
            }
			else if(srTransRec.byOrgTransType == SALE_TIP && (srTransRec.byPackType == OFFLINE_VOID || byPackTypeBeforeDCCLog == OFFLINE_VOID))
			{
				 if(strTCT.fCheckout == 1)	
                    strcpy(szTitle, "VOID CHECKOUT");
                 else
                    strcpy(szTitle, "VOID COMPLETION");			
			}
			else if(srTransRec.byOrgTransType == CASH_ADVANCE)
				strcpy(szTitle, "CASH ADV VOID");
			else	
                strcpy(szTitle, "VOID");
            break;
        case SALE_TIP:
            strcpy(szTitle, "TIP ADJUST");
            break;
			
        case SALE_OFFLINE:
        if(memcmp(srTransRec.szAuthCode,"Y1",2) == 0)
        {
            strcpy(szTitle, "SALE");        
        }
		else
		{
            if(strTCT.fCheckout == 1)	
                strcpy(szTitle, "CHECKOUT");
            else
                strcpy(szTitle, "COMPLETION");
		}
        break;
		
        case SALE_ADJUST: 
            strcpy(szTitle, "Adjust");
            break;
        case SETTLE:
            strcpy(szTitle, "Settle");
            break;
        case SIGN_ON:
            strcpy(szTitle, "Sign On");
            break;
        case BATCH_REVIEW:
            strcpy(szTitle, "Batch Review");
            break;
        case BATCH_TOTAL:
            strcpy(szTitle, "Batch Total");
            break;
        case REPRINT_ANY:
            strcpy(szTitle, "Reprint Receipt");
            break;
		//TINE:  04JUN2019
		case MANUAL_SETTLE:
			strcpy(szTitle, "Clear Batch");
			break;

		//gcitra
		case BIN_VER:
			strcpy(szTitle, "BIN Check");
			break;
		case CASH_LOYALTY:
			strcpy(szTitle, "REWARD INQUIRY");
			break;	
		case POS_AUTO_REPORT:
			strcpy(szTitle, "POS AUTO REPORT");
			break;	
		case CASH_ADVANCE:
			strcpy(szTitle, "CASH ADV"); //aaronnino for BDOCLG ver 9.0 fix on issue #00216 Cash advance txn title display should be CASH ADV instead of CASH ADVANCE
			break;	
		case BALANCE_INQUIRY:
			strcpy(szTitle, "Balance Inquiry"); //BDO-00143: Changed to BAL INQ -- jzg
			break;			
		//gcitra

		/* BDO CLG: Fleet card support - start -- jzg */
		case FLEET_SALE:
			strcpy(szTitle, "PTT Sale");
			break;
		/* BDO CLG: Fleet card support - end -- jzg */

		case RELOAD:
			strcpy(szTitle, "Reload");
			break;

		case SMAC_BALANCE:
			strcpy(szTitle, "Balance"); 
			break;

		case PAYMENT:
			strcpy(szTitle, strHDT.szHostLabel); 
			break;	

		case RETRIEVE:
			strcpy(szTitle, "Retrieve"); 
			break;	

		case SETUP:
			strcpy(szTitle, "Setup"); 
			break;

		//SMAC
#if 0
		case SMAC_ACTIVATION:	
			strcpy(szTitle, "SMAC LOGON");
			break;
		case SMAC_AWARD:		
			strcpy(szTitle, "AWARD POINTS");
			break;
		case SMAC_BALANCE:	
			strcpy(szTitle, "POINTS INQUIRY");
		//SMAC
#endif		
        default:
            strcpy(szTitle, "");
            break;
    }
    i = strlen(szTitle);
    szTitle[i]=0x00;
    return ;
}

void vdDispTransTitle(BYTE byTransType)
{
    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1];
    int iInitX = 1;
   
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(byTransType, szTitle);
    iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2 ;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    //memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle));  
    memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
    CTOS_LCDTSetReverse(TRUE);
    CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    CTOS_LCDTSetReverse(FALSE);

		
}


//aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE start 2 of 8
int vdDispTransTitleCardTypeandTotal(BYTE byTransType, char *msg)
{
    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1], szAmtBuff[20+1], szCurAmtBuff[20+1];
	BYTE szDisMsg[200];
    int iInitX = 1;
		int inCardLabellen, inCardDispStart, inMaxDisplen;
		char szDisplayCardLable [MAX_CHAR_PER_LINE+1];
		char szVoidCurrSymbol [10+1];
		int inType = 0;

	vdDebug_LogPrintf("--vdDispTransTitleCardTypeandTotal--");
	vdDebug_LogPrintf("msg=[%s]", msg);
	vdDebug_LogPrintf("srTransRec.byTransType=[%d]", srTransRec.byTransType);

	#ifdef ANDROID_NEW_UI
		return d_OK;
	#endif

	if (strcmp(msg, "SENDING...") == 0)
		inType = 1;
	else if (strcmp(msg, "RECEIVING...") == 0)
		inType = 2;
	else
		inType = 3;

	vdDebug_LogPrintf("inType=[%d]", inType);
	
//issue-00229: do not display amount on TC upload

    //if (inGetATPBinRouteFlag() == TRUE)		
		//return d_OK;

    //if(srTransRec.byPackType == TC_UPLOAD || fAdviceTras == TRUE)  
	//	return d_OK;
        

		/* BDOCLG-00318: Fix for garbage display problem - start -- jzg */
		//inTCTRead(1);
		//if(((strTCT.fFleetGetLiters == TRUE) || (strTCT.fGetDescriptorCode == TRUE)) && (srTransRec.fFleetCard == TRUE))
		//	CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
		//else
			CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
		/* BDOCLG-00318: Fix for garbage display problem - end -- jzg */


    memset(szDisMsg, 0x00, sizeof(szDisMsg));
    memset(szTitle, 0x00, sizeof(szTitle));
	if(byTransType == VOID)
		strcpy(szTitle,"VOID");
	else		
		szGetTransTitle(byTransType, szTitle);

	if(srTransRec.byPackType == TC_UPLOAD || fAdviceTras == TRUE)
    {
		//strcpy(szDisMsg, szTitle);
       	//strcat(szDisMsg, "|");
		//strcat(szDisMsg, msg);
       	//usCTOSS_LCDDisplay(szDisMsg);	

		switch (inType)
		{
			case 1:
				vdDisplayMessageStatusBox(1, 8, "SENDING", MSG_PLS_WAIT, MSG_TYPE_SEND);
				break;
			case 2:
				vdDisplayMessageStatusBox(1, 8, "RECEIVING", MSG_PLS_WAIT, MSG_TYPE_RECEIVE);
				break;
			default:
				vdDisplayMessageStatusBox(1, 8, szTitle, MSG_PLS_WAIT, MSG_TYPE_PROCESS);
				break;
		}
		CTOS_Delay(300);
		
		return d_OK;
    }
	

	iInitX = 40;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
	memset(szDisplayCardLable, 0x00, sizeof(szDisplayCardLable));

	if(strcmp(srTransRec.szCardLable,"CITI MASTER")==0)
		memcpy(&szDisplayCardLable[0],"MASTERCARD",10);
	else if(strcmp(srTransRec.szCardLable,"CITI VISA")==0)
		memcpy(&szDisplayCardLable[0],"VISA",4);
	else
		memcpy(&szDisplayCardLable[0],srTransRec.szCardLable,strlen(srTransRec.szCardLable));
		
    inCardLabellen = strlen(szDisplayCardLable);
		
   
   if ((srTransRec.byTransType == SALE) ||(srTransRec.byTransType == PRE_AUTH) || (srTransRec.byTransType == CASH_ADVANCE) || (srTransRec.byTransType == VOID) || (srTransRec.byTransType == BIN_VER) || srTransRec.byTransType == SALE_OFFLINE)
   {
          
		BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
		//BYTE szBaseAmt[20 + 1] = {0};

		/* BDOCLG-00318: Fix for garbage display problem - start -- jzg */
		//if(((strTCT.fFleetGetLiters == TRUE) || (strTCT.fGetDescriptorCode == TRUE)) && (srTransRec.fFleetCard == TRUE))
		//{
		//	CTOS_LCDTSetReverse(TRUE);
		//	CTOS_LCDTPrintAligned(1, szTitle, d_LCD_ALIGNLEFT);
		//	CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);
		//	CTOS_LCDTSetReverse(FALSE);
		//}else
			/* BDOCLG-00318: Fix for garbage display problem - end -- jzg */
		{
	        inCardDispStart = iInitX  - inCardLabellen * 2;
				
			//CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
			//memcpy(&szTitleDisplay[inCardDispStart], szDisplayCardLable, inCardLabellen);
			//CTOS_LCDTSetReverse(TRUE);
			//CTOS_LCDTPrintAligned(1, szTitle, d_LCD_ALIGNLEFT);
			//CTOS_LCDTPrintAligned(1, szTitleDisplay, d_LCD_ALIGNLEFT);
			//CTOS_LCDTPrintXY(1,1,szTitleDisplay);
			if ((inGetATPBinRouteFlag() != TRUE) && (srTransRec.byTransType != BIN_VER)	) {
                //CTOS_LCDTPrintXY(8, 1, szDisplayCardLable);
                //CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);  -- Tine:  02Apr2019
            }
			//CTOS_LCDTSetReverse(FALSE);
		}
    if (srTransRec.byTransType != BIN_VER)
    {
       //CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
       wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 

	   if(srTransRec.fDCC && strTCT.fFormatDCCAmount == TRUE)
		   vdDCCModifyAmount(szBaseAmt,&szAmtBuff); //vdDCCModifyAmount(&szAmtBuff);
	   else	   
	       vdCTOS_FormatAmount(strCST.szAmountFormat, szBaseAmt,szAmtBuff); // patrick fix case #229
       
       //setLCDPrint(3, DISPLAY_POSITION_LEFT, "TOTAL:");    --  Tine:  02Apr2019
        //CTOS_LCDTPrintXY(8, 4, "TOTAL:");
       
       
       if (srTransRec.byTransType == VOID)   
       {
          memset(szVoidCurrSymbol,0,sizeof(szVoidCurrSymbol));
          strcpy(szVoidCurrSymbol,strCST.szCurSymbol); 
          strcat(szVoidCurrSymbol,"-");
          sprintf(szCurAmtBuff,"%s%s",szVoidCurrSymbol, szAmtBuff);
          //setLCDPrint(5, DISPLAY_POSITION_CENTER, szCurAmtBuff);
          //CTOS_LCDTPrintXY(8, 3, szCurAmtBuff);
       }
       else
       {
          sprintf(szCurAmtBuff,"%s%s",strCST.szCurSymbol, szAmtBuff);
          //issue-00371
          //inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 5, DISPLAY_POSITION_CENTER);
       }
    }
       strcpy(szDisMsg, szTitle);
       strcat(szDisMsg, " > ");
       strcat(szDisMsg, szCurAmtBuff);
       strcat(szDisMsg, "|");
       //strcat(szDisMsg, szDisplayCardLable);
       //strcat(szDisMsg, " ");
       //strcat(szDisMsg, "|");
       //strcat(szDisMsg, "TOTAL:");
       //strcat(szDisMsg, "|");
	   strcat(szDisMsg, msg);
       //usCTOSS_LCDDisplay(szDisMsg);        //Tine:  24Apr2019

	   switch (inType)
	   {
			case 1:
				vdDisplayMessageStatusBox(1, 8, szTitle, szCurAmtBuff, MSG_TYPE_SEND);
				break;
			case 2:
				vdDisplayMessageStatusBox(1, 8, szTitle, szCurAmtBuff, MSG_TYPE_RECEIVE);
				break;
			default:
				vdDisplayMessageStatusBox(1, 8, szTitle, MSG_PLS_WAIT, MSG_TYPE_PROCESS);
				break;
	   }
	   CTOS_Delay(300);
	   

   }
	 #if 0
   else if(srTransRec.byTransType == BIN_VER)
   {
      inCardDispStart = iInitX  - inCardLabellen * 2;
      CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
      //memcpy(&szTitleDisplay[inCardDispStart], szDisplayCardLable, inCardLabellen);
      CTOS_LCDTSetReverse(TRUE);
      //CTOS_LCDTPrintAligned(1, szTitle, d_LCD_ALIGNLEFT);
      //CTOS_LCDTPrintAligned(1, szTitleDisplay, d_LCD_ALIGNLEFT);
	  
	  CTOS_LCDTPrintXY(1,1,szTitleDisplay);
	  if (inGetATPBinRouteFlag() == TRUE)	
      	CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);
      CTOS_LCDTSetReverse(FALSE);
   }
	 #endif
	 else
	 	CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);

#if 0
	 //TINE:  23MAY2019
	 if (srTransRec.byTransType == SETTLE)
	 {
	 	strcpy(szDisMsg, szTitle);
       	strcat(szDisMsg, "|");
       	strcat(szDisMsg, "RECEIVING...");
		usCTOSS_LCDDisplay(szDisMsg);
	 }
#endif
	
}
//aaronnino for BDOCLG ver 9.0 fix on issue #00139 HAVE A DEFAULT TITLE DISPLAY OF TXN TYPE end 2 of 8



void vdDispTitleString(BYTE *szTitle)
{
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1];
    int iInitX = 1;
       
    iInitX = (MAX_CHAR_PER_LINE - strlen(szTitle)*2) / 2;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[iInitX], szTitle, strlen(szTitle)); //aaronnino for BDOCLG ver 9.0 fix on issue #00072 Incorrrect transaction type displayed for INSTALLMENT 1 of 2 
    //memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
		CTOS_LCDTSetReverse(TRUE);
    CTOS_LCDTPrintXY(1, 1, szTitleDisplay);
    CTOS_LCDTSetReverse(FALSE);
}




USHORT clearLine(int line)
{
	return 0; // uneeded for android
	
    CTOS_LCDTGotoXY(1,line);
    CTOS_LCDTClear2EOL();
}

void vdDisplayTxnFinishUI(void)
{
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
	
	vdDebug_LogPrintf("--vdDisplayTxnFinishUI--");
	vdDebug_LogPrintf("fRePrintFlag=[%d]", fRePrintFlag);
	vdDebug_LogPrintf("srTransRec.byTransType=[%d]", srTransRec.byTransType);
	vdDebug_LogPrintf("strTCT.fDisplayAPPROVED=[%d]", strTCT.fDisplayAPPROVED);
	vdDebug_LogPrintf("strIIT.inIssuerID=[%d]", strIIT.inIssuerID);
	vdDebug_LogPrintf("strIIT.szIssuerLabel=[%s]", strIIT.szIssuerLabel);
	vdDebug_LogPrintf("strIIT.szMaskCustomerCopy=[%s]", strIIT.szMaskCustomerCopy);
	vdDebug_LogPrintf("srTransRec.szPAN=[%s]", srTransRec.szPAN);
	vdDebug_LogPrintf("strTCT.fDCC=[%d]", strTCT.fDCC);
	vdDebug_LogPrintf("strCDT.fDCCEnable=[%d]", strCDT.fDCCEnable);
	vdDebug_LogPrintf("srTransRec.fDCC=[%d]", srTransRec.fDCC);
	vdDebug_LogPrintf("srTransRec.fCUPPINEntry=[%d]", srTransRec.fCUPPINEntry);
	vdDebug_LogPrintf("srTransRec.szBinRouteRespCode=[%s]", srTransRec.szBinRouteRespCode);
	vdDebug_LogPrintf("srTransRec.HDTid=[%d]", srTransRec.HDTid);
	vdDebug_LogPrintf("srTransRec.shTransResult=[%d]", srTransRec.shTransResult);
	vdDebug_LogPrintf("srTransRec.byPackType=[%d]", srTransRec.byPackType);
	vdDebug_LogPrintf("srTransRec.byEntryMode=[%d]", srTransRec.byEntryMode);
	
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	
      //setLCDPrint(3, DISPLAY_POSITION_CENTER, "Transaction");
      //setLCDPrint(4, DISPLAY_POSITION_CENTER, "Approved");
      //setLCDPrint(5, DISPLAY_POSITION_CENTER, srTransRec.szAuthCode);     
      if(srTransRec.byTransType == LOG_ON)
      return;
	  
      if(strlen(srTransRec.szBinRouteRespCode))
      {
           if(memcmp(srTransRec.szBinRouteRespCode, "00", 2) != 0)
           {
                memset(srTransRec.szBinRouteRespCode, 0, sizeof(srTransRec.szBinRouteRespCode));
                return;
           }
      }
            
      if(strTCT.fDisplayAPPROVED == TRUE && srTransRec.byTransType != BALANCE_INQUIRY && srTransRec.byTransType != SMAC_ACTIVATION
      && srTransRec.byTransType != BIN_VER) // Terminal will display the SMAC balance instead of the "APPROVED" message. 
      {
      	   #if 0
           CTOS_LCDTClearDisplay();
           vdDisplayMessageBox(1, 8, "APPROVED", "Transaction", "", MSG_TYPE_SUCCESS);
		   #else
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

			vdDebug_LogPrintf("usCTOSS_DisplayApprovedUI, len=[%d], szTemp=[%s]", strlen(szTemp), szTemp);
			
		   	usCTOSS_DisplayApprovedUI(szTemp);
		   #endif
		   
		   CTOS_Beep();
      }
}
void vdDispErrMsg(IN BYTE *szMsg)
{
    char szDisplayMsg[40];
    BYTE byKeyBuf;
    
    CTOS_LCDTClearDisplay();
    if(srTransRec.byTransType != 0)
        vdDispTransTitle(srTransRec.byTransType);

    memset(szDisplayMsg, 0x00, sizeof(szDisplayMsg));
    strcpy(szDisplayMsg, szMsg);
    vduiClearBelow(8);
    setLCDPrint(8, DISPLAY_POSITION_LEFT, szDisplayMsg);
    CTOS_TimeOutSet (TIMER_ID_2 , 2*100);
    CTOS_Sound(1000, 50);
    
    while (1)
    {        
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL ||byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            return ;
        }
    }
}


int vdDispTransactionInfo(void)
{
    BYTE byKeyBuf;
    BYTE szTmp1[16+1];
    BYTE szTmp2[16+1];
	BYTE szTmp[130+1];
    
    CTOS_LCDTClearDisplay();
    vdDispTransTitle(srTransRec.byTransType);
    
    setLCDPrint(2, DISPLAY_POSITION_LEFT, "Card NO.");
    setLCDPrint(3, DISPLAY_POSITION_LEFT, srTransRec.szPAN);
    memset(szTmp1, 0x00, sizeof(szTmp1));
    memset(szTmp2, 0x00, sizeof(szTmp2));
	memset(szTmp, 0x00, sizeof(szTmp));
    wub_hex_2_str(srTransRec.szInvoiceNo, szTmp1, 3);
    sprintf(szTmp2,"Inv No:%s", szTmp1);
    setLCDPrint(4, DISPLAY_POSITION_LEFT, szTmp2);
    
    wub_hex_2_str(srTransRec.szTotalAmount, szTmp1, 6);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Amount:");
	//format amount 10+2
	vdCTOS_FormatAmount(strCST.szAmountFormat, szTmp1, szTmp);
	sprintf(szTmp2,"%s%s", strCST.szCurSymbol,szTmp);
    //sprintf(szTmp2,"SGD%7lu.%02lu", (atol(szTmp1)/100), (atol(szTmp1)%100));
    setLCDPrint(6, DISPLAY_POSITION_RIGHT, szTmp2);  
    setLCDPrint(8, DISPLAY_POSITION_LEFT, "PRS ENTR TO CONF");
    CTOS_TimeOutSet (TIMER_ID_2 , 30*100);
    
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_2 )  == d_OK)
            return  READ_CARD_TIMEOUT;
        
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL)
        {
            CTOS_KBDBufFlush ();
            return USER_ABORT;
        }
        else if (byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            return d_OK;
        }
    }
}

USHORT showBatchRecord(TRANS_DATA_TABLE *strTransData)
{
    char szStr[DISPLAY_LINE_SIZE + 1];
    char szTemp[DISPLAY_LINE_SIZE + 1];
    BYTE byKeyBuf;
    CTOS_LCDTClearDisplay();
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    sprintf(szStr, "%s", strTransData->szPAN);
    setLCDPrint(1, DISPLAY_POSITION_LEFT, "Card NO:");
    setLCDPrint(2, DISPLAY_POSITION_LEFT, szStr);
    
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    memset(szTemp, ' ', DISPLAY_LINE_SIZE);
    wub_hex_2_str(strTransData->szBaseAmount, szTemp, AMT_BCD_SIZE);
    sprintf(szStr, "%lu.%lu", atol(szTemp)/100, atol(szTemp)%100);
    setLCDPrint(3, DISPLAY_POSITION_LEFT, "Amount:");
    setLCDPrint(4, DISPLAY_POSITION_LEFT, szStr);

    
    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    sprintf(szStr, "%s", strTransData->szAuthCode);
    setLCDPrint(5, DISPLAY_POSITION_LEFT, "Auth Code:");
    setLCDPrint(6, DISPLAY_POSITION_LEFT,  szStr);


    memset(szStr, ' ', DISPLAY_LINE_SIZE);
    memset(szTemp, ' ', DISPLAY_LINE_SIZE);
    wub_hex_2_str(strTransData->szInvoiceNo, szTemp, INVOICE_BCD_SIZE);
    sprintf(szStr, "%s", szTemp);
    setLCDPrint(7, DISPLAY_POSITION_LEFT, "Invoice NO:");
    setLCDPrint(8, DISPLAY_POSITION_LEFT, szTemp);
     
    CTOS_TimeOutSet (TIMER_ID_2 , 30*100);   
    while (1)
    {
        if(CTOS_TimeOutCheck(TIMER_ID_2 )  == d_OK)
        {
            CTOS_LCDTClearDisplay();
            return  READ_CARD_TIMEOUT;
        }
        CTOS_KBDHit  (&byKeyBuf);
        if (byKeyBuf == d_KBD_CANCEL)
        {
            CTOS_KBDBufFlush ();
            CTOS_LCDTClearDisplay();
            return USER_ABORT;
        }
        else if (byKeyBuf == d_KBD_ENTER)
        {
            CTOS_KBDBufFlush ();
            CTOS_LCDTClearDisplay();
            return d_OK;
        }
    }
}

void vduiLightOn(void)
{
    if (strTCT.fHandsetPresent)  
        CTOS_BackLightSetEx(d_BKLIT_LCD,d_ON,80000);
    else
        CTOS_BackLightSet (d_BKLIT_LCD, d_ON);
}

void vduiKeyboardBackLight(BOOL fKeyBoardLight)
{
    if (strTCT.fHandsetPresent) 
    {
        if(VS_TRUE == fKeyBoardLight)
        {
            
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_ON,0xffffff);
            CTOS_BackLightSetEx(d_BKLIT_LCD,d_ON,0xffffff);
        }
        else
        {
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_OFF,100);
            CTOS_BackLightSetEx(d_BKLIT_LCD,d_OFF,3000);
        }

    }
    else
    {
        if(VS_TRUE == fKeyBoardLight)
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_ON,0xffffff);
        else
            CTOS_BackLightSetEx(d_BKLIT_KBD,d_OFF,100);
    }
}

void vduiPowerOff(void)
{
    BYTE block[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    USHORT ya,yb,xa,xb;
    unsigned char c;
        
    //vduiClearBelow(1);
    CTOS_LCDTClearDisplay(); /*BDO: Clear window -- sidumili*/
	
    vduiDisplayStringCenter(4,"POWER OFF TERMINAL?");
	//gcitra-0728
    
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");
	//gcitra-0728
    c=WaitKey(60);
    
    if(c!=d_KBD_ENTER)
    {
            return;
    }    
    
    for(ya =1; ya<5; ya++)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,ya);
        CTOS_LCDTClear2EOL();
    }
    for(yb=8; yb>4; yb--)
    {
        CTOS_Delay(100);
        CTOS_LCDTGotoXY(1,yb);
        CTOS_LCDTClear2EOL();
    }
    CTOS_LCDTPrintXY(1,4,"----------------");
    for(xa=1; xa<8; xa++)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xa,4," ");
    }
    for(xb=16; xb>7; xb--)
    {
        CTOS_Delay(25);
        CTOS_LCDTPrintXY(xb,4," ");
    }
            
    CTOS_LCDGShowPic(58, 6, block, 0, 6);
    CTOS_Delay(250);
    CTOS_LCDTGotoXY(7,4);
    CTOS_LCDTClear2EOL();
    CTOS_Delay(250);

    CTOS_PowerOff();
}

void vduiDisplayStringCenter(unsigned char  y,unsigned char *sBuf)
{
	//1027
	//setLCDPrint27(y, DISPLAY_POSITION_CENTER,sBuf);	
	//setLCDPrint(y,DISPLAY_POSITION_CENTER,sBuf);
	BYTE szTitle[20+1];
    BYTE szDisMsg[200];

	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);

	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
    strcat(szDisMsg, sBuf);
	
	usCTOSS_LCDDisplay(szDisMsg);
	

}

void vduiClearBelow(int line)
{
	int i = 0,
		inNumOfLine = 8;

	return; // uneeded for android
	
	/* BDOCLG-00005: should clear the rest of the line even for V3 terminals - start -- jzg */
	//inTCTRead(1);
	if((strTCT.byTerminalType % 2) == 0)
		inNumOfLine = 16;

	for(i=line; i<=inNumOfLine; i++)
		clearLine(i);
	/* BDOCLG-00005: should clear the rest of the line even for V3 terminals - end -- jzg */
}

void vduiWarningSound(void)
{
	return; // uneeded for android
	
    CTOS_LEDSet(d_LED1, d_ON);
    CTOS_LEDSet(d_LED2, d_ON);
    CTOS_LEDSet(d_LED3, d_ON);
    
    CTOS_Beep();
    CTOS_Delay(300);
    CTOS_Beep();
    
    CTOS_LEDSet(d_LED1, d_OFF);
    CTOS_LEDSet(d_LED2, d_OFF);
    CTOS_LEDSet(d_LED3, d_OFF);
}


void vdDisplayErrorMsg(int inColumn, int inRow,  char *msg)
{
	int inRowtmp;
	BYTE szTitle[20+1];
    BYTE szDisMsg[200];
	
    #if 0
    //if ((strTCT.byTerminalType % 2) == 0)
    //inRowtmp = V3_ERROR_LINE_ROW;
    //else
    //inRowtmp = inRow;
    
    //clearLine(inRowtmp);
    
    //CTOS_LCDTPrintXY(inColumn, inRowtmp, "                                        ");
    //CTOS_LCDTPrintXY(inColumn, inRowtmp, msg);
    memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle);
    
    memset(szDisMsg, 0x00, sizeof(szDisMsg));
    strcpy(szDisMsg, szTitle);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, msg);
    
    usCTOSS_LCDDisplay(szDisMsg);
    CTOS_Beep();
    CTOS_Delay(1500);
    //clearLine(inRowtmp);
    #else

	vdTrimSpaces(msg); // trim spaces
	
    CTOS_Beep();
    inDisplayMessageBoxWithButtonConfirmation(1,8,"",msg,"","",MSG_TYPE_ERROR, BUTTON_TYPE_NONE_OK);
	//CTOS_Delay(1000);

	DisplayStatusLine("");
    #endif
}

#if 0
//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated start 3 of 5
void vdDisplayErrorMsgResp (int inColumn, int inColumn2, int inColumn3, int inRow, int inRow2, int inRow3,  char *msg, char *msg2, char *msg3)
{
    BYTE szTitle[20+1];
    BYTE szDisMsg[200];
#if 0
	CTOS_LCDTPrintXY(inColumn, inRow, "                                        ");
		CTOS_LCDTPrintXY(inColumn2, inRow2, "                                        ");
		CTOS_LCDTPrintXY(inColumn3, inRow3, "                                        ");
    CTOS_LCDTPrintXY(inColumn, inRow, msg);
		CTOS_LCDTPrintXY(inColumn2, inRow2, msg2);
		CTOS_LCDTPrintXY(inColumn3, inRow3, msg3);
#else
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
    strcat(szDisMsg, msg);
	strcat(szDisMsg, "\n");
    strcat(szDisMsg, msg2);
	strcat(szDisMsg, "\n");
    strcat(szDisMsg, msg3);
	usCTOSS_LCDDisplay(szDisMsg);
#endif
    CTOS_Beep();
    CTOS_Delay(2000);
}
//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated end 3 of 5
#endif

void vdDisplayErrorMsgResp2 (char *msg, char *msg2, char *msg3)
{
	//BYTE szTitle[20+1];
    //BYTE szDisMsg[200];
   //CTOS_LCDTClearDisplay();

	vdDebug_LogPrintf("--vdDisplayErrorMsgResp2--");
	vdDebug_LogPrintf("Len=[%d], msg=[%s]", strlen(msg), msg);
	vdDebug_LogPrintf("Len=[%d], msg2=[%s]", strlen(msg2), msg2);
	vdDebug_LogPrintf("Len=[%d], msg=[%s]", strlen(msg3), msg3);

 #if 0
 if ((strTCT.byTerminalType % 2) == 1) 
 {
      vduiDisplayStringCenter(3,msg);
      vduiDisplayStringCenter(4,msg2);
      vduiDisplayStringCenter(5,msg3);
 
 }
 else 
 {
      vduiDisplayStringCenter(6, msg);
      vduiDisplayStringCenter(7, msg2);
      vduiDisplayStringCenter(8, msg3);
 }
 memset(szTitle, 0x00, sizeof(szTitle));
 szGetTransTitle(srTransRec.byTransType, szTitle);
 
 memset(szDisMsg, 0x00, sizeof(szDisMsg));
 strcpy(szDisMsg, szTitle);
 strcat(szDisMsg, "|");
 strcat(szDisMsg, msg);
 strcat(szDisMsg, "\n");
 strcat(szDisMsg, msg2);
 strcat(szDisMsg, "\n");
 strcat(szDisMsg, msg3);
 usCTOSS_LCDDisplay(szDisMsg);
 #else

 #ifdef ANDROID_NEW_UI
	//vdDisplayMessageBox(1, 8, msg, msg2, msg3, MSG_TYPE_INFO);
	//CTOS_Beep();
	//CTOS_Delay(1500);
	
    inDisplayMessageBoxWithButtonConfirmation(1,8,msg,msg2,msg3," ",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
 	CTOS_Beep();
    CTOS_Delay(1000);
	DisplayStatusLine("");
	
 #endif
 
 
 #endif
   //CTOS_Beep();
   //CTOS_Delay(1500);
   //CTOS_LCDTClearDisplay();
}

void vdDisplayErrorMsgResp2Ex(char *msg, char *msg2, char *msg3)
{
   //CTOS_LCDTClearDisplay();
   //vdClearBelowLine(2);
   BYTE szTitle[20+1];
   BYTE szDisMsg[200];

#if 0   
   if ((strTCT.byTerminalType % 2) == 1) 
   {
		vduiDisplayStringCenter(3,msg);
		vduiDisplayStringCenter(4,msg2);
		vduiDisplayStringCenter(5,msg3);
			
   }
   else 
   {
      vduiDisplayStringCenter(6, msg);
      vduiDisplayStringCenter(7, msg2);
      vduiDisplayStringCenter(8, msg3);
   }

	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, szTitle);
	strcat(szDisMsg, "|");
    strcat(szDisMsg, msg);
	strcat(szDisMsg, "\n");
    strcat(szDisMsg, msg2);
	strcat(szDisMsg, "\n");
    strcat(szDisMsg, msg3);
	usCTOSS_LCDDisplay(szDisMsg);
#else
	vdDisplayMessageBox(1, 8, msg, msg2, msg3, MSG_TYPE_INFO);
	CTOS_Beep();
	CTOS_Delay(1500);
	
	
#endif
   //CTOS_Beep();
   //CTOS_Delay(1500);
   //vdClearBelowLine(2);
}

/* functions for loyalty - Meena 15/01/2012 - start*/
#if 0
short vduiAskConfirmContinue(int inDisplay)
{
    unsigned char key;
	BYTE szTitle[20+1];
    BYTE szDisMsg[250];
  
    //vduiClearBelow(1);
    #if 0
    CTOS_LCDTClearDisplay();/*BDO: Clear window -- sidumili*/
    vduiDisplayStringCenter(3,"ARE YOU SURE");
    vduiDisplayStringCenter(4,"YOU WANT TO");
	if (inDisplay == 1)		
    	vduiDisplayStringCenter(5,"CLEAR BATCH?");
	else if (inDisplay == 2)	
    	vduiDisplayStringCenter(5,"DELETE REVERSAL?");
	else		
		vduiDisplayStringCenter(5,"CONTINUE?");
	//gcitra-0728
    //CTOS_LCDTPrintXY(1,7,"NO[X]   YES[OK] ");
    vduiDisplayStringCenter(7,"NO[X]   YES[OK] ");
	//gcitra-0728
	#endif
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	memset(szDisMsg,0x00,sizeof(szDisMsg));
	if (inDisplay == 1)		
	{
		strcpy(szDisMsg, "CLEAR BATCH");
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "ARE YOU SURE | YOU WANT TO | CLEAR BATCH?");
	}
	else if (inDisplay == 2)
	{
		strcpy(szDisMsg, "DELETE REVERSAL");
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "ARE YOU SURE | YOU WANT TO | DELETE REVERSAL?");
	}
	else
	{
		strcpy(szDisMsg, szTitle);
		strcat(szDisMsg, "|");
		strcat(szDisMsg, "ARE YOU SURE | YOU WANT TO | CONTINUE?");
	}
        
    while(1)
    {
        //key = struiGetchWithTimeOut();
        key=usCTOSS_Confirm(szDisMsg);
        if (key==d_OK)
            return d_OK;
        else if (key==d_USER_CANCEL)
            return -1;
		else if (key==0xFF)
			return -1;		
        else
            vduiWarningSound();
    }
    
}
#endif

BYTE struiGetchWithTimeOut(void)
{
    unsigned char c;
    BOOL isKey;
    CTOS_TimeOutSet(TIMER_ID_3,3000);
    
    while(1)//loop for time out
    {
        CTOS_KBDInKey(&isKey);
        if (isKey){ //If isKey is TRUE, represent key be pressed //
            vduiLightOn();
            //Get a key from keyboard //
            CTOS_KBDGet(&c);
            return c;   
        }
        else if (CTOS_TimeOutCheck(TIMER_ID_3) == d_YES)
        {      
            return d_KBD_CANCEL;
        }
    }
}

/* functions for loyalty - Meena 15/01/2012 - End*/

short inuiAskSettlement(void)
{
    unsigned char key;
    while(1) 
    {
        vduiClearBelow(2);
        vduiDisplayStringCenter(2,"DAILY SETTLEMENT");
        vduiDisplayStringCenter(3,"NOTIFICATION");

        vduiDisplayStringCenter(5,"PERFORM");
        vduiDisplayStringCenter(6,"SETTLEMENT?");
        vduiDisplayStringCenter(8,"NO[X] YES[OK]");

        CTOS_KBDGet(&key);
        if(key==d_KBD_ENTER)
            return d_OK;
        else if(key==d_KBD_CANCEL)
            return d_KBD_CANCEL;
        else if(key==d_KBD_F1)
            vduiPowerOff();
    }
        
}

void vduiDisplaySignalStrengthBatteryCapacity(void)
{
    
    BYTE bCapacity, msg2[50];
    USHORT dwRet;
    short insign;
    
    
    if(GPRSCONNETSTATUS== d_OK && strTCT.inMainLine == GPRS_MODE)
    {
        insign=incommSignal();
        if(insign==-1)
        {
            CTOS_LCDTPrintXY (9,1, "SIGNAL:NA");
        }
        else
        {           
            if(insign/6 == 0)
                CTOS_LCDTPrintXY (9,1, "NO SIGNAL");
            else if(insign/6 == 1)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:l____"); 
            }
            else if(insign/6 == 2)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:ll___"); 
            }
            else if(insign/6 == 3)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:lll__"); 
            }
            else if(insign/6 == 4)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:llll_"); 
            }
            else if(insign/6 == 5)
            {                               
               CTOS_LCDTPrintXY (9,1, "S:lllll"); 
            }
            
        }
    }
    
    dwRet= CTOS_BatteryGetCapacityByIC(&bCapacity);  
    if(dwRet==d_OK)
    {
        sprintf(msg2, "B:%d%% ", bCapacity);
        CTOS_LCDTPrintXY (3,1, msg2);
    }
                
}

void vdSetErrorMessage(char *szMessage)
{
    int inErrLen=0;

    inErrLen = strlen(szMessage);
    memset(szErrorMessage,0x00,sizeof(szErrorMessage));
	memset(szErrorMessage1,0x00,sizeof(szErrorMessage1));
    
    if (inErrLen > 0)
    {
        if (inErrLen > ERRORLEN)
            inErrLen = ERRORLEN;
        
        memcpy(szErrorMessage,szMessage,inErrLen);
    }
}

void vdSetErrorMessages(char *szMessage, char *szMessage1)
{
    int inErrLen=0, inErrLen1=0;

    inErrLen = strlen(szMessage);
	inErrLen1 = strlen(szMessage1);

    memset(szErrorMessage,0x00,sizeof(szErrorMessage));
	memset(szErrorMessage1,0x00,sizeof(szErrorMessage1));
    
    if (inErrLen1 > 0)
    {
            inErrLen = ERRORLEN;
		    inErrLen1 = ERRORLEN1;
        
        memcpy(szErrorMessage,szMessage,inErrLen);
		memcpy(szErrorMessage1,szMessage1,inErrLen1);
    }
}


int inGetErrorMessage(char *szMessage)
{
    int inErrLen=0;

    inErrLen = strlen(szErrorMessage);

    if (inErrLen > 0)
    {       
        memcpy(szMessage,szErrorMessage,inErrLen);
    }
    
    return inErrLen;
}

int inGetErrorMessages(char *szMessage, char *szMessage1)
{
    int inErrLen=0, inErrLen1=0;

    inErrLen = strlen(szErrorMessage);
	inErrLen1 = strlen (szErrorMessage1);
      
        memcpy(szMessage,szErrorMessage,inErrLen);
		memcpy(szMessage1,szErrorMessage1,inErrLen1);

    return inErrLen1;
}

//gcitra
void setLCDPrint27(int line,int position, char *pbBuf)
{
    int iInitX = 0;
    int lens = 0;

		CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);

    switch(position)
    {
        case DISPLAY_POSITION_LEFT:
            CTOS_LCDTPrintXY(1, line, pbBuf);
            break;
        case DISPLAY_POSITION_CENTER:
            lens = strlen(pbBuf);
            iInitX = (20- lens) / 2 + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
        case DISPLAY_POSITION_RIGHT:
            lens = strlen(pbBuf);
            iInitX = 20- lens + 1;
            CTOS_LCDTPrintXY(iInitX, line, pbBuf);
            break;
    }

		
		CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
}

//gcitra


//sidumili: display message
void vdDisplayMessage(char *szLine1Msg, char *szLine2Msg, char *szLine3Msg)
{
#if 0
	CTOS_LCDTClearDisplay();
		vduiClearBelow(2);
		vduiDisplayStringCenter(4, szLine1Msg);
		vduiDisplayStringCenter(5, szLine2Msg);
		vduiDisplayStringCenter(6, szLine3Msg);
		CTOS_Beep(); /*BDO: Added BEEP -- sidumili*/
		WaitKey(1);
#else
	//tine: for android terminal display
	 BYTE szTitle[20+1];
	 BYTE szDisMsg[200];
	//CTOS_LCDTClearDisplay();


	 memset(szTitle, 0x00, sizeof(szTitle));
	 szGetTransTitle(srTransRec.byTransType, szTitle);
	 
	 memset(szDisMsg, 0x00, sizeof(szDisMsg));
	 strcpy(szDisMsg, szTitle);
	 strcat(szDisMsg, "|");
	 strcat(szDisMsg, szLine1Msg);
	 usCTOSS_LCDDisplay(szDisMsg);
	
#endif
}
//sidumili

//sidumili: confirmation
short vduiAskEnterToConfirm(void)
{
    unsigned char key;
  
    
    CTOS_LCDTPrintXY(1,8,"CONFIRM?NO[X]YES[OK]");
        
    while(1)
    {
        key = struiGetchWithTimeOut();
        if (key==d_KBD_ENTER)
            return d_OK;
        else if (key==d_KBD_CANCEL){
					
						//sidumili: disconnect communication when USER PRESS CANCEL KEY
						if (strCPT.inCommunicationMode == DIAL_UP_MODE){
										inCTOS_inDisconnect();
						}
						//sidumili: disconnect communication when USER PRESS CANCEL KEY
					
            return -1;
        	}
        else
            vduiWarningSound();
    }
    
}
//sidumili:

#if 0
int inDisplayDCCRateScreen(void)
{
	char szTemp[MAX_CHAR_PER_LINE+1];
	char szTemp1[MAX_CHAR_PER_LINE+1];
	char szTemp2[MAX_CHAR_PER_LINE+1];
	BYTE szAmtBuff[20+1], szCurAmtBuff[20+1];
	BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
	int iLine,inLength=0;
	BYTE key=0;
	float inMarkup = 0;

	//#define RATE_RESPONSE_FULL "\x01\x70\x37\x31\x30\x30\x39\x63\x66\x34\x63\x32\x64\x64\x34\x38\x66\x34\x63\x32\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x30\x20\x20\x20\x20\x20\x20\x20\x20\x31\x31\x31\x32\x33\x34\x35\x36\x37\x38\x39\x31\x30\x30\x33\x36\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x34\x37\x32\x38\x30\x30\x31\x38\x34\x38\x39\x34\x33\x32\x30\x32\x30\x31\x32\x30\x39\x32\x32\x31\x32\x32\x38\x31\x38\x33\x35\x36\x30\x33\x36\x56\x53\x41\x64\x64\x37\x35\x34\x30\x38\x34\x39\x35\x35\x36\x30\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x32\x20\x20\x20\x20\x20\x20\x20\x20\x30\x30\x30\x30\x30\x31\x41\x55\x44\x34\x38\x33\x2E\x30\x32\x20\x20\x20\x20\x20\x30\x33\x36"
	
	//inUnPackIsoFunc61(&srTransRec,RATE_RESPONSE_FULL);

	vdDebug_LogPrintf("--inDisplayDCCRateScreen--");
	
	CTOS_LCDTClearDisplay();
	
	iLine = ((strTCT.byTerminalType%2)?3:4);
	
	wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBaseAmt,szAmtBuff); 
	sprintf(szCurAmtBuff,"(1)%s %s", srTransRec.szDCCLocalSymbol, szAmtBuff);
	setLCDPrint(2, DISPLAY_POSITION_RIGHT, szCurAmtBuff);

	memset(szAmtBuff,0x00,sizeof(szAmtBuff));
	memset(szCurAmtBuff,0x00,sizeof(szCurAmtBuff));
	memset(szBaseAmt,0x00,sizeof(szBaseAmt));

	inCSTReadHostID(srTransRec.szDCCCur);


    if(strTCT.fFormatDCCAmount == TRUE)
    	vdDCCModifyAmount(srTransRec.szDCCCurAmt,szAmtBuff);
	else
		vdCTOS_FormatAmount(strCST.szAmountFormat, srTransRec.szDCCCurAmt,szAmtBuff);
	
	
	sprintf(szCurAmtBuff,"(2)%s %s",srTransRec.szDCCCurSymbol, szAmtBuff);// Wait for strCST for foreign currency
	setLCDPrint((strTCT.byTerminalType%2)?3:4, DISPLAY_POSITION_RIGHT, szCurAmtBuff);
	
	inLength=strlen(srTransRec.szDCCFXRate)-srTransRec.inDCCFXRateMU;
	memset(szTemp,0x00,sizeof(szTemp));
	memcpy(szTemp,srTransRec.szDCCFXRate,inLength);
	memcpy(&szTemp[inLength],".",1);
	memcpy(&szTemp[inLength+1],&srTransRec.szDCCFXRate[inLength],srTransRec.inDCCFXRateMU);
		
	setLCDPrint((strTCT.byTerminalType%2)?5:7, DISPLAY_POSITION_LEFT, "Exchange Rate:");
	setLCDPrint((strTCT.byTerminalType%2)?6:8, DISPLAY_POSITION_RIGHT,szTemp);	

	memset(szTemp,0x00,sizeof(szTemp));
	memset(szTemp1,0x00,sizeof(szTemp1));


	inMarkup = atof(srTransRec.szDCCMarkupPer);
	sprintf(szTemp,"%.2f",inMarkup);
	sprintf(szTemp1,"%20.20s",szTemp);
	//sprintf(szTemp1,"%s",szTemp);
	sprintf(szTemp2,"Markup:%s",szTemp1);
	strcat(szTemp2,"%");
	
	setLCDPrint((strTCT.byTerminalType%2)?7:10,DISPLAY_POSITION_LEFT,szTemp2);

	srTransRec.fDCC = VS_FALSE;
	
	
	while(1)
	{
		key=WaitKey(inGetIdleTimeOut(TRUE));

       	if(key == d_KBD_1)
       	{
			return d_OK;
       	}
       	else if(key == d_KBD_2)
       	{	
			srTransRec.fDCC = VS_TRUE;			
		
			memset(srTransRec.szDCCLocalAmount,0x00,sizeof(srTransRec.szDCCLocalAmount));
			memcpy(srTransRec.szDCCLocalAmount, srTransRec.szTotalAmount,sizeof(srTransRec.szTotalAmount));

			return d_OK;
		}
        else if(key == d_KBD_CANCEL)
        {
			return FAIL;
        } 
		else if(key == 0xFF)
		{
			return FAIL;
		}

	}

	
	
}
#else
int inDisplayDCCRateScreen(void)
{
	VS_BOOL fDisplayForExRate = inFLGGet("fForExRate");
	VS_BOOL fDisplayMarkup = inFLGGet("fDCCMarkUp");
	char szTemp[MAX_CHAR_PER_LINE+1];
	char szTemp1[MAX_CHAR_PER_LINE+1];
	char szTemp2[MAX_CHAR_PER_LINE+1];
	BYTE szAmtBuff[20+1] = {0};
	BYTE szForeignAmtBuff[20+1] = {0};
	BYTE szCurAmtBuff[20+1] = {0};
	BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
	int iLine,inLength=0;
	BYTE key=0;
	float inMarkup = 0;
	CHAR szTitle[40 + 1] = {0};
	BYTE szDisplayBuf[1024 + 1] = {0};
	BYTE szConvRate[100 + 1] = {0};
	char szExchangeRate[MAX_CHAR_PER_LINE+1];
	char szMarkUp[MAX_CHAR_PER_LINE+1];
	
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitleForAndroid(srTransRec.byTransType, szTitle);
	
	//#define RATE_RESPONSE_FULL "\x01\x70\x37\x31\x30\x30\x39\x63\x66\x34\x63\x32\x64\x64\x34\x38\x66\x34\x63\x32\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x30\x20\x20\x20\x20\x20\x20\x20\x20\x31\x31\x31\x32\x33\x34\x35\x36\x37\x38\x39\x31\x30\x30\x33\x36\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x34\x37\x32\x38\x30\x30\x31\x38\x34\x38\x39\x34\x33\x32\x30\x32\x30\x31\x32\x30\x39\x32\x32\x31\x32\x32\x38\x31\x38\x33\x35\x36\x30\x33\x36\x56\x53\x41\x64\x64\x37\x35\x34\x30\x38\x34\x39\x35\x35\x36\x30\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x32\x20\x20\x20\x20\x20\x20\x20\x20\x30\x30\x30\x30\x30\x31\x41\x55\x44\x34\x38\x33\x2E\x30\x32\x20\x20\x20\x20\x20\x30\x33\x36"
	
	//inUnPackIsoFunc61(&srTransRec,RATE_RESPONSE_FULL);

	vdDebug_LogPrintf("--inDisplayDCCRateScreen--");
	vdDebug_LogPrintf("strTCT.fFormatDCCAmount=[%d]", strTCT.fFormatDCCAmount);
	vdDebug_LogPrintf("srTransRec.szDCCCur=[%s]", srTransRec.szDCCCur);
	vdDebug_LogPrintf("srTransRec.szDCCCurAmt=[%s]", srTransRec.szDCCCurAmt);

	strGBLVar.fGBLvDCC = TRUE;
	
	CTOS_LCDTClearDisplay();
	
	memset(szAmtBuff,0x00,sizeof(szAmtBuff));
	memset(szCurAmtBuff,0x00,sizeof(szCurAmtBuff));
	memset(szBaseAmt,0x00,sizeof(szBaseAmt));
	memset(szForeignAmtBuff, 0x00, sizeof(szForeignAmtBuff));
	
	// local
	wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBaseAmt,szAmtBuff); 
	
	// foreign
	inCSTReadHostID(srTransRec.szDCCCur);
    if(strTCT.fFormatDCCAmount == TRUE)
    	vdDCCModifyAmount(srTransRec.szDCCCurAmt,szForeignAmtBuff);
	else
		vdCTOS_FormatAmount(strCST.szAmountFormat, srTransRec.szDCCCurAmt,szForeignAmtBuff);
	
	// Exchange Rate
	inLength=strlen(srTransRec.szDCCFXRate)-srTransRec.inDCCFXRateMU;
	memset(szTemp,0x00,sizeof(szTemp));
	memset(szExchangeRate, 0x00, sizeof(szExchangeRate));
	memcpy(szTemp,srTransRec.szDCCFXRate,inLength);
	memcpy(&szTemp[inLength],".",1);
	memcpy(&szTemp[inLength+1],&srTransRec.szDCCFXRate[inLength],srTransRec.inDCCFXRateMU);
	strcpy(szExchangeRate, szTemp);

	// MarkUp
	memset(szTemp,0x00,sizeof(szTemp));
	memset(szTemp1,0x00,sizeof(szTemp1));
	memset(szMarkUp, 0x00, sizeof(szMarkUp));
	inMarkup = atof(srTransRec.szDCCMarkupPer);
	sprintf(szTemp,"%.2f",inMarkup);
	sprintf(szTemp1,"Markup: %s",szTemp);
	strcpy(szMarkUp, szTemp1);
	strcat(szMarkUp, "%");
	
	srTransRec.fDCC = VS_FALSE;

	memset(szDisplayBuf, 0x00, sizeof(szDisplayBuf));
	strcpy(szDisplayBuf, szTitle);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, "Select currency");
	strcat(szDisplayBuf, "|");

	sprintf(szConvRate, "%s = %s %s", szExchangeRate, szAmtBuff, srTransRec.szDCCLocalSymbol);
	
	if (!fDisplayForExRate)
		memset(szConvRate, 0x00, sizeof(szConvRate));	

	if (!fDisplayMarkup)
		memset(szMarkUp, 0x00, sizeof(szMarkUp));
	
	strcat(szDisplayBuf, szConvRate);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, srTransRec.szDCCLocalSymbol);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, srTransRec.szDCCCurSymbol);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, (fDisplayMarkup ? "Markup": " "));
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szAmtBuff);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, szForeignAmtBuff);
	strcat(szDisplayBuf, "|");
	strcat(szDisplayBuf, (fDisplayMarkup ? szMarkUp : " "));

	vdDebug_LogPrintf("usCTOSS_ConfirmDCC szDisMsg[%s]", szDisplayBuf);	
	
	key = usCTOSS_ConfirmDCC(szDisplayBuf);
	vdDebug_LogPrintf("usCTOSS_ConfirmDCC, strGBLVar.szGBLvSelectedValue=[%s]", strGBLVar.szGBLvSelectedValue);

	if (key == d_OK)
	{
		if (strcmp(strGBLVar.szGBLvSelectedValue, "PHP") == 0)
		{
			return d_OK;
		}
		else
		{
			srTransRec.fDCC = VS_TRUE;			
		
			memset(srTransRec.szDCCLocalAmount,0x00,sizeof(srTransRec.szDCCLocalAmount));
			memcpy(srTransRec.szDCCLocalAmount, srTransRec.szTotalAmount,sizeof(srTransRec.szTotalAmount));

			return d_OK;
		}
	}
	else
	{
		return d_NO;
	}
	
}

#endif

int inDisplayRateHostError(void)
{
	int iLine=0;
	BYTE key=0;

	CTOS_LCDTClearDisplay();
	setLCDPrint27((strTCT.byTerminalType%2)?2:3,DISPLAY_POSITION_CENTER,"RATE HOST ERROR");
	setLCDPrint((strTCT.byTerminalType%2)?4:5,DISPLAY_POSITION_LEFT,"PROCEED TO?");
	setLCDPrint((strTCT.byTerminalType%2)?5:6,DISPLAY_POSITION_LEFT,"BDO HOST");
	setLCDPrint((strTCT.byTerminalType%2)?6:7,DISPLAY_POSITION_RIGHT,"(1) YES");
	setLCDPrint((strTCT.byTerminalType%2)?8:9,DISPLAY_POSITION_RIGHT,"(2) NO");

	while(1)
	{
		key=WaitKey(inGetIdleTimeOut(TRUE));

       	if(key == d_KBD_1)
       	{
			return d_OK;
       	}
       	else if(key == d_KBD_2)
      	{	
			return FAIL;
			
		}
        else if(key == d_KBD_CANCEL)
        {
			return FAIL;
        } 
		else if(key == 0xFF)
		{
			return FAIL;
		}

	}
	
}

void vdDisplayErrorMsgResp3 (char *msg, char *msg2, char *msg3, char *msg4)
{
   CTOS_LCDTClearDisplay();

   if ((strTCT.byTerminalType % 2) == 1) 
   {
		vduiDisplayStringCenter(3,msg);
		vduiDisplayStringCenter(4,msg2);
		vduiDisplayStringCenter(5,msg3);
		vduiDisplayStringCenter(6,msg4);
			
   }
   else 
   {
      vduiDisplayStringCenter(6, msg);
      vduiDisplayStringCenter(7, msg2);
      vduiDisplayStringCenter(8, msg3);
	  vduiDisplayStringCenter(9,msg4);
	  
   }
	 
   CTOS_Beep();
   CTOS_Delay(1500);
   CTOS_LCDTClearDisplay();
}

int vdDispTransTitleAndCardType(BYTE byTransType)
{
    BYTE szTitle[16+1];
    BYTE szTitleDisplay[MAX_CHAR_PER_LINE+1], szAmtBuff[20+1], szCurAmtBuff[20+1];
    int iInitX = 1;
		int inCardLabellen, inCardDispStart, inMaxDisplen;
		char szDisplayCardLable [MAX_CHAR_PER_LINE+1];
		char szVoidCurrSymbol [10+1];

//issue-00229: do not display amount on TC upload
    //SIT

    if (inGetATPBinRouteFlag() == TRUE)		
		return d_OK;

	if(srTransRec.byPackType == TC_UPLOAD || fAdviceTras == TRUE)
		return d_OK;


		/* BDOCLG-00318: Fix for garbage display problem - start -- jzg */
		//inTCTRead(1);
		//if(((strTCT.fFleetGetLiters == TRUE) || (strTCT.fGetDescriptorCode == TRUE)) && (srTransRec.fFleetCard == TRUE))
		//	CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
		//else
			//CTOS_LCDFontSelectMode(d_FONT_FNT_MODE);
		/* BDOCLG-00318: Fix for garbage display problem - end -- jzg */
   
    memset(szTitle, 0x00, sizeof(szTitle));
	if(byTransType == VOID)
		strcpy(szTitle,"VOID");
	else		
		szGetTransTitle(byTransType, szTitle);

	iInitX = 40;
    memset(szTitleDisplay, 0x00, sizeof(szTitleDisplay));
    memset(szTitleDisplay, 0x20, MAX_CHAR_PER_LINE);
    memcpy(&szTitleDisplay[0], szTitle, strlen(szTitle));
	memset(szDisplayCardLable, 0x00, sizeof(szDisplayCardLable));

	if(strcmp(srTransRec.szCardLable,"CITI MASTER")==0)
		memcpy(&szDisplayCardLable[0],"MASTERCARD",10);
	else if(strcmp(srTransRec.szCardLable,"CITI VISA")==0)
		memcpy(&szDisplayCardLable[0],"VISA",4);
	else
		memcpy(&szDisplayCardLable[0],srTransRec.szCardLable,strlen(srTransRec.szCardLable));
		
    inCardLabellen = strlen(szDisplayCardLable);
		
   
   if ((srTransRec.byTransType == SALE) ||(srTransRec.byTransType == PRE_AUTH) || (srTransRec.byTransType == CASH_ADVANCE) || (srTransRec.byTransType == VOID) || (srTransRec.byTransType == BIN_VER))
   {
          
		BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
		{
	        inCardDispStart = iInitX  - inCardLabellen * 2;
				
			CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
			CTOS_LCDTSetReverse(TRUE);
			CTOS_LCDTPrintXY(1,1,szTitleDisplay);
			if ((inGetATPBinRouteFlag() != TRUE) && (srTransRec.byTransType != BIN_VER)	)
				   CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);
			
			CTOS_LCDTSetReverse(FALSE);
		}
#if 0
    if (srTransRec.byTransType != BIN_VER)
    {
       CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
       wub_hex_2_str(srTransRec.szTotalAmount, szBaseAmt, 6); 
       vdCTOS_FormatAmount(strCST.szAmountFormat, szBaseAmt,szAmtBuff); // patrick fix case #229
       
       setLCDPrint(3, DISPLAY_POSITION_LEFT, "TOTAL:");
       
       
       if (srTransRec.byTransType == VOID)   
       {
          memset(szVoidCurrSymbol,0,sizeof(szVoidCurrSymbol));
          strcpy(szVoidCurrSymbol,strCST.szCurSymbol); 
          strcat(szVoidCurrSymbol,"-");
          sprintf(szCurAmtBuff,"%s%s",szVoidCurrSymbol, szAmtBuff);
          setLCDPrint(5, DISPLAY_POSITION_CENTER, szCurAmtBuff);
       }
       else
       {
          sprintf(szCurAmtBuff,"%s%s",strCST.szCurSymbol, szAmtBuff);
          //issue-00371
          //inCTOS_DisplayCurrencyAmount(srTransRec.szTotalAmount, 5, DISPLAY_POSITION_CENTER);	 
          setLCDPrint(5, DISPLAY_POSITION_CENTER, szCurAmtBuff);
       }

    }
#endif
	 }
#if 0
   else if(srTransRec.byTransType == BIN_VER)
   {
      inCardDispStart = iInitX  - inCardLabellen * 2;
      CTOS_LCDFontSelectMode(d_FONT_TTF_MODE);
      //memcpy(&szTitleDisplay[inCardDispStart], szDisplayCardLable, inCardLabellen);
      CTOS_LCDTSetReverse(TRUE);
      //CTOS_LCDTPrintAligned(1, szTitle, d_LCD_ALIGNLEFT);
      //CTOS_LCDTPrintAligned(1, szTitleDisplay, d_LCD_ALIGNLEFT);
	  
	  CTOS_LCDTPrintXY(1,1,szTitleDisplay);
	  if (inGetATPBinRouteFlag() == TRUE)	
      	CTOS_LCDTPrintAligned(1, szDisplayCardLable, d_LCD_ALIGNRIGHT);
      CTOS_LCDTSetReverse(FALSE);
   }
#endif
	 else
	 	CTOS_LCDFontSelectMode(d_FONT_TTF_MODE); 
}

#if 0
int inReportSelecion(void)
{
    char szChoiceMsg[100];
    int bHeaderAttr = 0x01+0x04, key=0;
    char szHeaderString2[30 + 1];
    BYTE szDisMsg[100];

	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
	memset(szHeaderString2, 0x00, sizeof(szHeaderString2));
	strcpy(szHeaderString2, "REPORT");

    strcat(szChoiceMsg,"SUMMARY \n");
    strcat(szChoiceMsg,"DETAIL \n");
	strcat(szChoiceMsg,"HOST INFO \n");
	strcat(szChoiceMsg,"CRC \n");
	strcat(szChoiceMsg,"RS232 \n");
	strcat(szChoiceMsg,"IP \n");
	strcat(szChoiceMsg,"ISO LOG REPORT \n");
	strcat(szChoiceMsg, "EMV TAGS DATA");
	
	
    key = MenuDisplay(szHeaderString2,strlen(szHeaderString2), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    //key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);	
    //key = MenuDisplay(0,0, bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    if (key == 0xFF)
    {
        //strcpy(szDisMsg, szHeaderString2);
        //strcat(szDisMsg, "|");
        //strcat(szDisMsg, "TIME OUT");
        //usCTOSS_LCDDisplay(szDisMsg);
        //CTOS_Beep();
        //CTOS_Delay(1500);

		vdDisplayMessageBox(1, 8, "", "TIME OUT", "", MSG_TYPE_TIMEOUT);
		CTOS_Beep();
		CTOS_Delay(1000);
    }

    if (key == d_KBD_CANCEL)
    {
        //strcpy(szDisMsg, szHeaderString2);
        //strcat(szDisMsg, "|");
        //strcat(szDisMsg, "USER CANCEL");
        //usCTOSS_LCDDisplay(szDisMsg);
        //CTOS_Beep();
        //CTOS_Delay(1500);

		vdDisplayMessageBox(1, 8, "", "USER CANCEL", "", MSG_TYPE_INFO);
		CTOS_Beep();
		CTOS_Delay(1000);
    }

	if (key > 0)
	{
		if (key == 1)   //SUMMARY
		{
			//return d_OK;
			inCTOS_PRINT_SUMMARY_SELECTION();
		}
				
		if (key == 2)   //DETAIL
		{
			inCTOS_PRINT_DETAIL_SELECTION();
		}

		if (key == 3)   //HOST INFO
		{
			vdCTOS_HostInfo();
		}

		if (key == 4)   //CRC
		{
			vdCTOS_PrintCRC();
		}

		if (key == 5)   //RS232
		{
			vdCTOS_PrintRS232Report();
		}

		if (key == 6)   //IP
		{
			vdCTOS_PrintIP();
		}

		if (key == 7)   //ISO LOG REPORT
		{
			vdPrintISOLog();
		}
        if (key == 8)   //EMV TAGS DATA
        {
            vdPrintEMVTags();
        }
	}
	
    return 0;
}
#else
int inReportSelecion(void)
{
    char szChoiceMsg[100];
    int bHeaderAttr = 0x01+0x04, key=0;
    char szHeaderString2[30 + 1];
    BYTE szDisMsg[100];
	BYTE szMessage[2000 + 1] = {0};
	
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
	#if 0
	strcat(szChoiceMsg,"SUMMARY|");
    strcat(szChoiceMsg,"DETAIL|");
	strcat(szChoiceMsg,"HOST INFO|");
	strcat(szChoiceMsg,"CRC|");
	strcat(szChoiceMsg,"RS232|");
	strcat(szChoiceMsg,"IP|");
	strcat(szChoiceMsg,"ISO LOG REPORT|");
	strcat(szChoiceMsg, "EMV TAGS DATA");
	#else
	strcat(szChoiceMsg,"Summary|");
    strcat(szChoiceMsg,"Detail|");
	strcat(szChoiceMsg,"Host Info|");
	strcat(szChoiceMsg,"CRC|");
	strcat(szChoiceMsg,"IP");
	#endif
	vdDebug_LogPrintf("szChoiceMsg=[%s]", szChoiceMsg);
	
    memset(szMessage, 0x00, sizeof(szMessage));
	sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_VIEW, MENU_2_COL_COUNT, "Report", "Select a report to print");
	strcat(szMessage, szChoiceMsg);
	
	key = inMenuSelection(szMessage);	
    vdDebug_LogPrintf("inMenuSelection=[%d]", key);
	
    if (key == 0xFF || key == d_TIMEOUT)
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

	key = inGetMenuKeySelected(szChoiceMsg, strGBLVar.szGBLvProcessDesc);
    vdDebug_LogPrintf("inGetMenuKeySelected=[%d]", key);

	memset(strGBLVar.szGBLvMenuTitle, 0x00, sizeof(strGBLVar.szGBLvMenuTitle));
	memset(strGBLVar.szGBLvPrintTitle, 0x00, sizeof(strGBLVar.szGBLvPrintTitle));
	if (key > 0)
	{
		//vdDisplayMessageBox(1, 8, "", "", "", MSG_TYPE_NONE);
		//CTOS_Delay(100);
		
		if (key == 1)   //SUMMARY
		{
			strcpy(strGBLVar.szGBLvMenuTitle, "Summary Report");
			strcpy(strGBLVar.szGBLvPrintTitle, "Printing Summary");
			inCTOS_PRINT_SUMMARY_SELECTION();
		}
				
		if (key == 2)   //DETAIL
		{
			if (strTCT.fHotelSetup)
			{
				memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
				strcat(szChoiceMsg,"Detail|");
    			strcat(szChoiceMsg,"Card Verify|");
				vdDebug_LogPrintf("szChoiceMsg=[%s]", szChoiceMsg);

				memset(szMessage, 0x00, sizeof(szMessage));
				sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_VIEW, MENU_1_COL_COUNT, "Detail Report", "Select report");
				strcat(szMessage, szChoiceMsg);
				
				key = inMenuSelection(szMessage);	
			    vdDebug_LogPrintf("inMenuSelection=[%d]", key);
				if (key == 0xFF || key == d_TIMEOUT)
			    {
					CTOS_Beep();
					return d_NO;
			    }

			    else if (key == d_KBD_CANCEL || key == d_USER_CANCEL)
			    {	
					CTOS_Beep();
					return d_NO;
			    }

				key = inGetMenuKeySelected(szChoiceMsg, strGBLVar.szGBLvProcessDesc);
			    vdDebug_LogPrintf("inGetMenuKeySelected=[%d]", key);

				memset(strGBLVar.szGBLvMenuTitle, 0x00, sizeof(strGBLVar.szGBLvMenuTitle));
				memset(strGBLVar.szGBLvPrintTitle, 0x00, sizeof(strGBLVar.szGBLvPrintTitle));

				if (key > 0)
				{
					if (key == 1) // DETAIL
					{
						strcpy(strGBLVar.szGBLvMenuTitle, "Detail Report");
						inCTOS_PRINT_DETAIL_SELECTION();
					}

					if (key == 2) // Card Verify
					{
						strcpy(strGBLVar.szGBLvMenuTitle, "Card Verify");
						vdCTOS_PrintPreAuthReport();
					}
				}
				
			}
			else
			{
				strcpy(strGBLVar.szGBLvMenuTitle, "Detail Report");
				inCTOS_PRINT_DETAIL_SELECTION();
			}		
		}

		if (key == 3)   //HOST INFO
		{
			strcpy(strGBLVar.szGBLvMenuTitle, "Host Info Report");
			vdCTOS_HostInfo();
		}

		if (key == 4)   //CRC
		{
			strcpy(strGBLVar.szGBLvMenuTitle, "CRC Report");
			vdCTOS_PrintCRC();
		}

		#if 0
		if (key == 5)   //RS232
		{
			strcpy(strGBLVar.szGBLvMenuTitle, "RS232 Report");
			vdCTOS_PrintRS232Report();
		}
		#endif

		if (key == 5)   //IP
		{
			strcpy(strGBLVar.szGBLvMenuTitle, "IP Report");
			vdCTOS_PrintIP();
		}

		#if 0
		if (key == 6)   //ISO LOG REPORT
		{
			strcpy(strGBLVar.szGBLvMenuTitle, "ISO Log Report");
			vdPrintISOLog();
		}
        if (key == 7)   //EMV TAGS DATA
        {
        	strcpy(strGBLVar.szGBLvMenuTitle, "EMV Tags Report");
            vdPrintEMVTags();
        }
		#endif
	}
	else
	{
		return d_NO;
	}
	
    return 0;
}

#endif

int inSettingsSelection(void)
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
    //strcat(szChoiceMsg,"DETAIL \n");
    //strcat(szChoiceMsg,"HOST INFO \n");

    key = MenuDisplay(szHeaderString2,strlen(szHeaderString2), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    //key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    //key = MenuDisplay(0,0, bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    if (key == 0xFF)
    {
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

    if (key == d_KBD_CANCEL)
    {
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

    if (key > 0)
    {
        if (key == 1)   //CLEAR BATCH
        {
            //return d_OK;
            inCTOS_ManualSettle();
        }

        if (key == 2)
        {

        }

        if (key == 3)
        {

        }
    }

    return 0;
}

int inReprintSelection(void)
{
    char szChoiceMsg[100];
    int bHeaderAttr = 0x01+0x04, key=0;
    char szHeaderString2[30 + 1];
    BYTE szDisMsg[100];
	BYTE szMessage[2000 + 1] = {0};

    //CTOS_LCDTClearDisplay();
	
    memset(szDisMsg, 0x00, sizeof(szDisMsg));
    memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
    memset(szHeaderString2, 0x00, sizeof(szHeaderString2));
    strcpy(szHeaderString2, "REPRINT");

	#if 0
    strcat(szChoiceMsg,"LAST RECEIPT \n");
    strcat(szChoiceMsg,"ANY RECEIPT \n");
    strcat(szChoiceMsg,"LAST SETTLE \n");

    key = MenuDisplay(szHeaderString2,strlen(szHeaderString2), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
   	#else

		strcat(szChoiceMsg,"Last receipt|");
    	strcat(szChoiceMsg,"Any receipt|");
   		strcat(szChoiceMsg,"Last settlement");
	
		vdDebug_LogPrintf("szChoiceMsg=[%s]", szChoiceMsg);
	
		memset(szMessage, 0x00, sizeof(szMessage));
		sprintf(szMessage, "%d|%d|%s|%s^", MENU_TEXT_VIEW, MENU_1_COL_COUNT, "Reprint", "What should be reprinted?");
		strcat(szMessage, szChoiceMsg);

		key = inMenuSelection(szMessage);	
		vdDebug_LogPrintf("inMenuSelection=[%d]", key);
	
	#endif

    if (key == 0xFF || key == d_TIMEOUT)
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

	key = inGetMenuKeySelected(szChoiceMsg, strGBLVar.szGBLvProcessDesc);
    vdDebug_LogPrintf("inGetMenuKeySelected=[%d]", key);

	memset(strGBLVar.szGBLvMenuTitle, 0x00, sizeof(strGBLVar.szGBLvMenuTitle));

	if (key > 0)
	{
		if (key == 1)   //LAST RECEIPT
	    {
	    	strcpy(strGBLVar.szGBLvMenuTitle, "Last Receipt");
	        inCTOS_REPRINT_LAST();
	    }

	    if (key == 2)   //ANY RECEIPT
	    {
	    	strcpy(strGBLVar.szGBLvMenuTitle, "Any Receipt");
			inCTOS_REPRINT_ANY();
	    }

	    if (key == 3)   //LAST SETTLE
	    {
	    	strcpy(strGBLVar.szGBLvMenuTitle, "Reprint Last Settlement");
	        inCTOS_REPRINTF_LAST_SETTLEMENT();
	    }
	}
    else
    {
    	return d_NO;
    }

    return 0;
}

USHORT CTOS_LCDTClearDisplay(void)
{
	vdDebug_LogPrintf("CTOS_LCDTClearDisplay overload...");
	return d_OK;
}

void vdDisplayProcessingReceiving(void)
{
    
}

void DisplayStatusLine(char *szDisplay) 
{
 	BYTE szTitle[25+1];
	BYTE szDisMsg[200];

	vdDebug_LogPrintf("--DisplayStatusLine--");
	
	#if 0
	memset(szTitle, 0x00, sizeof(szTitle));
    szGetTransTitle(srTransRec.byTransType, szTitle); 

    strcpy(szDisMsg, szTitle);
    strcat(szDisMsg, "|");
    strcat(szDisMsg, szDisplay);
    usCTOSS_LCDDisplay(szDisMsg);
	#else
		vdDisplayMessageStatusBox(1, 8, "", "", MSG_TYPE_NONE);
	#endif
     
}

void vdDisplayMessageBox(int inColumn, int inRow,  char *msg, char *msg2, char *msg3, int msgType)
{
	BYTE szDisMsg[512 + 1] = {0};
	BYTE szMsgType[40 + 1] = {0};
	//int inCtr = get_env_int("READCARDFAILEDCTR");
	//int inLimit = get_env_int("READCARDFAILEDLIMIT");

	//vdDebug_LogPrintf("--vdDisplayMessageBox--inCtr[%d], inLimit[%d]", inCtr, inLimit);

	//if((strcmp("READ CARD FAILED",msg2)==0) && (inCtr >=inLimit))
	      //inDisplayMessageBoxWithButtonReboot(1,8,"APPLICATION UPDATE","PLEASE PRESS OK","AND WAIT", MSG_TYPE_INFO);
	      	
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

	vdDebug_LogPrintf("vdDisplayMessageBox msg2[%s]", msg2);

        /*if(strcmp("READ CARD FAILED",msg2)==0)
        {
             //strcat(szDisMsg, "|");
             //strcat(szDisMsg, "READ CARD FAILED");
             put_env_int("READCARDFAILEDCTR",inCtr+1);
        }*/

	vdDebug_LogPrintf("szDisMsg=[%s], msgType=[%d]", szDisMsg, msgType);
	usCTOSS_DisplayBox(szDisMsg);      
}

void vdDisplayMessageStatusBox(int inColumn, int inRow,  char *msg, char *msg2, int msgType)
{
	BYTE szDisMsg[528 + 1] = {0};
	BYTE szMsgType[40 + 1] = {0};

	vdDebug_LogPrintf("--vdDisplayMessageStatusBox--");
	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, msg);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, msg2);
	strcat(szDisMsg, "|");

	memset(szMsgType, 0x00, sizeof(szMsgType));
	inGetBoxMessageType(msgType, szMsgType);
	strcat(szDisMsg, szMsgType);

	vdDebug_LogPrintf("szDisMsg=[%s], msgType=[%d]", szDisMsg, msgType);
	
	usCTOSS_DisplayStatusBox(szDisMsg);
}


int inGetBoxMessageType(int msgType, char* szMsgType)
{
	vdDebug_LogPrintf("--inGetBoxMessageType--");
	
	switch (msgType)
	{
		case MSG_TYPE_SUCCESS:
			strcpy(szMsgType, "success");
		break;
		case MSG_TYPE_ERROR:
			strcpy(szMsgType, "error");
		break;
		case MSG_TYPE_WARNING:
			strcpy(szMsgType, "warning");
		break;
		case MSG_TYPE_INFO:
			strcpy(szMsgType, "info");
		break;
		case MSG_TYPE_QUESTION:
			strcpy(szMsgType, "question");
		break;
		case MSG_TYPE_PRINT:
			strcpy(szMsgType, "print");
		break;			
		case MSG_TYPE_PROCESS:
			strcpy(szMsgType, "process");
		break;
		case MSG_TYPE_SEND:
			strcpy(szMsgType, "send");
		break;	
		case MSG_TYPE_RECEIVE:
			strcpy(szMsgType, "receive");
		break;
		case MSG_TYPE_WAIT:
			strcpy(szMsgType, "wait");
		break;
		case MSG_TYPE_NONE:
			strcpy(szMsgType, "none");
		break;
		case MSG_TYPE_TIMEOUT:
			strcpy(szMsgType, "timeout");
		break;
		case MSG_TYPE_REMOVE:
			strcpy(szMsgType, "remove");
		break;
		default:
			strcpy(szMsgType, "none");
		break;	
	}

	vdDebug_LogPrintf("szMsgType=[%s]", szMsgType);
	
	return d_OK;
}

// sidumili: for display message with button
int inDisplayMessageBoxWithButton(int inColumn, int inRow,  char *msg, char *msg2, char *msg3, int msgType)
{
	BYTE szDisMsg[200 + 1] = {0};
	BYTE szMsgType[40 + 1] = {0};
	int inRet = d_NO;

	vdDebug_LogPrintf("--inDisplayMessageBoxWithButton--");
	
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
	
	inRet = usCTOSS_ConfirmOK(szDisMsg);

	return inRet;

}

#if 0
short vduiAskConfirmation(char *szHeaderString)
{
    char szChoiceMsg[30 + 1];
    int bHeaderAttr = 0x01+0x04, key=0;
    char szHeaderString2[30 + 1];	

    CTOS_LCDTClearDisplay();
	memset(szChoiceMsg, 0x00, sizeof(szChoiceMsg));
	memset(szHeaderString2, 0x00, sizeof(szHeaderString2));

  //#00136 - No header in Delete Batch & Clear Reversal
    if(strcmp(szHeaderString, "DELETE BATCH?") == 0)	
    		strcpy(szHeaderString2, "DELETE BATCH?");	
    else if(strcmp(szHeaderString, "CLEAR REVERSAL?") == 0)	
    		strcpy(szHeaderString2, "CLEAR REVERSAL?");	
    else if(strcmp(szHeaderString, "RESTART?") == 0)	
    		strcpy(szHeaderString2, "RESTART?");	
    else if(strcmp(szHeaderString, "GOT EMAIL ADD?") == 0)	
    		strcpy(szHeaderString2, "GOT EMAIL ADD?");
	else if(strcmp(szHeaderString, "DELETE ERM BATCH?") == 0)	
    		strcpy(szHeaderString2, "DELETE ERM BATCH?");	
		
    strcat(szChoiceMsg,"YES \n");
    strcat(szChoiceMsg,"NO");
    key = MenuDisplay(szHeaderString2,strlen(szHeaderString2), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    //key = MenuDisplay(szHeaderString,strlen(szHeaderString), bHeaderAttr, 1, 1, szChoiceMsg, TRUE);	
    //key = MenuDisplay(0,0, bHeaderAttr, 1, 1, szChoiceMsg, TRUE);
    
	if (key > 0)
	{
		if (key == 1) 
		{
			return d_OK;
		}
				
		if (key == 2 || key == d_KBD_CANCEL)
		{
			return -1;
		}	
	}
	
    return -1;
}
#else
// for new UI -- sidumili
short vduiAskConfirmation(char *szHeaderString)
{
	BYTE szDisMsg[40 + 1] = {0};
	int key = d_NO;
	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));

	if(strcmp(szHeaderString, "DELETE BATCH?") == 0)
	{
		sprintf(szDisMsg, "batch for %s?", strHDT.szHostLabel);
		key = inDisplayMessageBoxWithButtonConfirmation(1,8,"Are you sure you want to clear",szDisMsg,"","Clear Batch",MSG_TYPE_QUESTION, BUTTON_TYPE_NO_YES);
	}
	else if(strcmp(szHeaderString, "CLEAR REVERSAL?") == 0)
	{
		sprintf(szDisMsg, "reversal for %s?", strHDT.szHostLabel);
		key = inDisplayMessageBoxWithButtonConfirmation(1,8,"Are you sure you want to clear",szDisMsg,"","Clear Reversal",MSG_TYPE_QUESTION, BUTTON_TYPE_NO_YES);
	}
		
	if (key == 0xFF || key == d_TIMEOUT)
	{
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(1000);
		return d_NO;
	}

	else if (key == d_KBD_CANCEL || key == d_USER_CANCEL)
	{	
		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_TIMEOUT, BUTTON_TYPE_NONE_OK);
		CTOS_Beep();
		CTOS_Delay(1000);
		return d_NO;
	}

	return d_OK;
}

#endif

// sidumili: for display message with button and pass button label(setText)
int inDisplayMessageBoxWithButtonConfirmation(int inColumn, int inRow,  char *msg, char *msg2, char *msg3, char *msg4, int msgType, int btnType)
{
	BYTE szDisMsg[512 + 1] = {0};
	BYTE szMsgType[40 + 1] = {0};
	int inRet = d_NO;

	vdDebug_LogPrintf("--inDisplayMessageBoxWithButtonConfirmation--");
	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	strcpy(szDisMsg, msg);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, msg2);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, msg3);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, msg4);
	strcat(szDisMsg, "|");

	memset(szMsgType, 0x00, sizeof(szMsgType));
	inGetBoxMessageType(msgType, szMsgType);
	strcat(szDisMsg, szMsgType);

	memset(szMsgType,0x00, sizeof(szMsgType));
	inGetBoxButtonType(btnType, szMsgType);
	strcat(szDisMsg, "|");
	strcat(szDisMsg, szMsgType);
		
	vdDebug_LogPrintf("szDisMsg=[%s], msgType=[%d], btnType=[%d]", szDisMsg, msgType, btnType);
	
	inRet = usCTOSS_ConfirmOKCancel(szDisMsg);
	
	return inRet;

}

// sidumili: Get button label
int inGetBoxButtonType(int btnType, char* szMsgType)
{
	vdDebug_LogPrintf("--inGetBoxButtonType--");
	
	switch (btnType)
	{
		case BUTTON_TYPE_CANCEL_OK:
			strcpy(szMsgType, "CANCEL|OK");
		break;
		case BUTTON_TYPE_NO_YES:
			strcpy(szMsgType, "NO|YES");
		break;
		case BUTTON_TYPE_CANCEL_CONFIRM:
			strcpy(szMsgType, "CANCEL|CONFIRM");
		break;
		case BUTTON_TYPE_CANCEL_LOGIN:
			strcpy(szMsgType, "CANCEL|LOGIN");
			break;
		case BUTTON_TYPE_CANCEL_LOGOUT:
			strcpy(szMsgType, "CANCEL|LOGOUT");
			break;
		case BUTTON_TYPE_CANCEL_SET:
			strcpy(szMsgType, "CANCEL|SET");
			break;	
		case BUTTON_TYPE_CANCEL_TRYAGAIN:
			strcpy(szMsgType, "CANCEL|TRY AGAIN");
			break;
		case BUTTON_TYPE_CANCEL_ENTER:
			strcpy(szMsgType, "CANCEL|ENTER");
			break;
		case BUTTON_TYPE_CANCEL_PROCEED:
			strcpy(szMsgType, "CANCEL|PROCEED");
			break;
		case BUTTON_TYPE_CANCEL_UPLOAD:
			strcpy(szMsgType, "CANCEL|UPLOAD");
			break;	
		case BUTTON_TYPE_CANCEL_PRINT_UPLOAD:
			strcpy(szMsgType, "CANCEL|PRINT/UPLOAD");
			break;	
		case BUTTON_TYPE_CANCEL_PRINT:
			strcpy(szMsgType, "CANCEL|PRINT");
			break;
		case BUTTON_TYPE_CANCEL_PROCEED_PRINT:
			strcpy(szMsgType, "CANCEL|PROCEED|PRINT");
			break;
		case BUTTON_TYPE_CANCEL_OK_PRINT:
			strcpy(szMsgType, "CANCEL|OK|PRINT");
			break;	
		case BUTTON_TYPE_NONE_FINISH:
			strcpy(szMsgType, "NONE|FINISH");
			break;		
		case BUTTON_TYPE_NONE_OK:
			strcpy(szMsgType, "NONE|OK");
			break;
		case BUTTON_TYPE_NONE_BACK_TO_MAIN:
			strcpy(szMsgType, "NONE|BACK TO MAIN MENU");
			break;
		default:
			strcpy(szMsgType, "CANCEL|OK");
		break;	
	}

	vdDebug_LogPrintf("szMsgType=[%s]", szMsgType);
	
	return d_OK;
}

void vdDisplayCheck(void)
{
	BYTE bySC_status;
	int inRowtmp,inRow=8;
	BYTE szTitle[20+1];
	BYTE szDisMsg[256];
	BOOL fDisplayBox = FALSE; //sidumili: variable to display box once and avoid flicker
	
	memset(szDisMsg, 0x00, sizeof(szDisMsg));
	memset(szTitle, 0x00, sizeof(szTitle));
	szGetTransTitle(srTransRec.byTransType, szTitle);
	
	while(1)
	{
		CTOS_SCStatus(d_SC_USER, &bySC_status);
		if(bySC_status & d_MK_SC_PRESENT)
		{
			if (!fDisplayBox)
			{
				vdDisplayTxnFinishUI();
				fDisplayBox = TRUE;
			}

			CTOS_Beep();
			CTOS_Delay(300);
			CTOS_Beep();
			continue;
			
		}	
		break;
	}
}

void szGetTransTitleForAndroid(BYTE byTransType, BYTE *szTitle)
{    
    int i;
    szTitle[0] = 0x00;
    
    //vdDebug_LogPrintf("**szGetTransTitle START byTransType[%d]Orig[%d]**", byTransType, srTransRec.byOrgTransType);

//smac
	if (fSMACTRAN){
		if (byTransType == SMAC_ACTIVATION)
			strcpy(szTitle, "SMAC Logon");
		if (byTransType == SALE_OFFLINE)		
			strcpy(szTitle, "Award Points");
		if (byTransType == SMAC_BALANCE)	
			strcpy(szTitle, "Pts Inquiry");
		if (byTransType == SALE)		
			strcpy(szTitle, "Redemption");

    	i = strlen(szTitle);
    	szTitle[i]=0x00;
    	return ;
	}
		
//smac

	if ((fInstApp == TRUE) && (byTransType == SALE)){
		strcpy(szTitle, "Installment");
		return;
	}

    switch(byTransType)
    {
        case SALE:
            strcpy(szTitle, "Card Sale");
            break;
        case PRE_AUTH:
			//0826
            //strcpy(szTitle, "PRE AUTH");
            strcpy(szTitle, "Card Verify");
			//0826
            break;
        case PRE_COMP:
            strcpy(szTitle, "Auth Comp");
            break;
        case REFUND:
            strcpy(szTitle, "Refund");
            break;
        case VOID:
            if(REFUND == srTransRec.byOrgTransType)
                strcpy(szTitle, "Void Refund");
            else if(srTransRec.byOrgTransType == SALE_OFFLINE)
            {
				if(memcmp(srTransRec.szAuthCode,"Y1",2) == 0)
				{
                    strcpy(szTitle, "Void Offline");					
				}
                else
                {
                    if(strTCT.fCheckout == 1)	
                        strcpy(szTitle, "Void Checkout");
                    else
                        strcpy(szTitle, "Void Completion");			
                }
            }
			else if(srTransRec.byOrgTransType == SALE_TIP && (srTransRec.byPackType == OFFLINE_VOID || byPackTypeBeforeDCCLog == OFFLINE_VOID))
			{
				 if(strTCT.fCheckout == 1)	
                    strcpy(szTitle, "Void Checkout");
                 else
                    strcpy(szTitle, "Void Completion");			
			}
			else if(srTransRec.byOrgTransType == CASH_ADVANCE)
				strcpy(szTitle, "Cash Adv Void");
			else	
                strcpy(szTitle, "Void");
            break;
        case SALE_TIP:
            strcpy(szTitle, "Tip Adjust");
            break;
			
        case SALE_OFFLINE:
        if(memcmp(srTransRec.szAuthCode,"Y1",2) == 0)
        {
            strcpy(szTitle, "Sale");        
        }
		else
		{
            if(strTCT.fCheckout == 1)	
                strcpy(szTitle, "Checkout");
            else
                strcpy(szTitle, "Completion");
		}
        break;
		
        case SALE_ADJUST: 
            strcpy(szTitle, "Adjust");
            break;
        case SETTLE:
            strcpy(szTitle, "Settle");
            break;
        case SIGN_ON:
            strcpy(szTitle, "Sign On");
            break;
        case BATCH_REVIEW:
            strcpy(szTitle, "Batch Review");
            break;
        case BATCH_TOTAL:
            strcpy(szTitle, "Batch Total");
            break;
        case REPRINT_ANY:
            strcpy(szTitle, "Reprint Receipt");
            break;
		//TINE:  04JUN2019
		case MANUAL_SETTLE:
			strcpy(szTitle, "Clear Batch");
			break;

		//gcitra
		case BIN_VER:
			strcpy(szTitle, "BIN Check");
			break;
		case CASH_LOYALTY:
			strcpy(szTitle, "Reward Inquiry");
			break;	
		case POS_AUTO_REPORT:
			strcpy(szTitle, "POS Auto Report");
			break;	
		case CASH_ADVANCE:
			strcpy(szTitle, "Cash Adv"); //aaronnino for BDOCLG ver 9.0 fix on issue #00216 Cash advance txn title display should be CASH ADV instead of CASH ADVANCE
			break;	
		case BALANCE_INQUIRY:
			strcpy(szTitle, "Balance Inquiry"); //BDO-00143: Changed to BAL INQ -- jzg
			break;			
		//gcitra

		/* BDO CLG: Fleet card support - start -- jzg */
		case FLEET_SALE:
			strcpy(szTitle, "PTT Sale");
			break;
		/* BDO CLG: Fleet card support - end -- jzg */

		case RELOAD:
			strcpy(szTitle, "Reload");
			break;

		case SMAC_BALANCE:
			strcpy(szTitle, "Balance"); 
			break;

		case PAYMENT:
			strcpy(szTitle, strHDT.szHostLabel); 
			break;	

		case RETRIEVE:
			strcpy(szTitle, "Retrieve"); 
			break;	

		case SETUP:
			strcpy(szTitle, "Setup"); 
			break;
		case CTMS_UPDATE:
			strcpy(szTitle, "CTMS Update"); 
			break;

		case CLEAR_BATCH:
			strcpy(szTitle, "Clear Batch"); 
			break;

		case CLEAR_REVERSAL:
			strcpy(szTitle, "Clear Reversal"); 
			break;

		//SMAC
#if 0
		case SMAC_ACTIVATION:	
			strcpy(szTitle, "SMAC LOGON");
			break;
		case SMAC_AWARD:		
			strcpy(szTitle, "AWARD POINTS");
			break;
		case SMAC_BALANCE:	
			strcpy(szTitle, "POINTS INQUIRY");
		//SMAC
#endif		
        default:
            strcpy(szTitle, "");
            break;
    }
    i = strlen(szTitle);
    szTitle[i]=0x00;
    return ;
}

int inDisplayDCCRateScreenEx(void)
{
	VS_BOOL fDisplayForExRate = inFLGGet("fForExRate");
	VS_BOOL fDisplayMarkup = inFLGGet("fDCCMarkUp");
	char szTemp[MAX_CHAR_PER_LINE+1];
	char szTemp1[MAX_CHAR_PER_LINE+1];
	char szTemp2[MAX_CHAR_PER_LINE+1];
	BYTE szAmtBuff[20+1], szCurAmtBuff[20+1];
	BYTE szBaseAmt[AMT_ASC_SIZE + 1] = {0};
	int iLine,inLength=0;
	BYTE key=0;
	float inMarkup = 0;

	//#define RATE_RESPONSE_FULL "\x01\x70\x37\x31\x30\x30\x39\x63\x66\x34\x63\x32\x64\x64\x34\x38\x66\x34\x63\x32\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x30\x20\x20\x20\x20\x20\x20\x20\x20\x31\x31\x31\x32\x33\x34\x35\x36\x37\x38\x39\x31\x30\x30\x33\x36\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x34\x37\x32\x38\x30\x30\x31\x38\x34\x38\x39\x34\x33\x32\x30\x32\x30\x31\x32\x30\x39\x32\x32\x31\x32\x32\x38\x31\x38\x33\x35\x36\x30\x33\x36\x56\x53\x41\x64\x64\x37\x35\x34\x30\x38\x34\x39\x35\x35\x36\x30\x30\x38\x34\x35\x37\x32\x35\x32\x38\x30\x30\x31\x33\x36\x30\x35\x30\x32\x20\x20\x20\x20\x20\x20\x20\x20\x30\x30\x30\x30\x30\x31\x41\x55\x44\x34\x38\x33\x2E\x30\x32\x20\x20\x20\x20\x20\x30\x33\x36"
	
	//inUnPackIsoFunc61(&srTransRec,RATE_RESPONSE_FULL);
	
	CTOS_LCDTClearDisplay();
	
	iLine = ((strTCT.byTerminalType%2)?3:4);
	
	wub_hex_2_str(srTransRec.szDCCLocalAmount, szBaseAmt, 6); 
	vdCTOS_FormatAmount("NN,NNN,NNN,NNn.nn", szBaseAmt,szAmtBuff); 
	sprintf(szCurAmtBuff,"(1)%s %s", srTransRec.szDCCLocalSymbol, szAmtBuff);
	setLCDPrint(2, DISPLAY_POSITION_RIGHT, szCurAmtBuff);

	memset(szAmtBuff,0x00,sizeof(szAmtBuff));
	memset(szCurAmtBuff,0x00,sizeof(szCurAmtBuff));
	memset(szBaseAmt,0x00,sizeof(szBaseAmt));

	inCSTReadHostID(srTransRec.szDCCCur);


    if(strTCT.fFormatDCCAmount == TRUE)
    	vdDCCModifyAmount(srTransRec.szDCCCurAmt,szAmtBuff);
	else
		vdCTOS_FormatAmount(strCST.szAmountFormat, srTransRec.szDCCCurAmt,szAmtBuff);
	
	
	sprintf(szCurAmtBuff,"(2)%s %s",srTransRec.szDCCCurSymbol, szAmtBuff);// Wait for strCST for foreign currency
	setLCDPrint((strTCT.byTerminalType%2)?3:4, DISPLAY_POSITION_RIGHT, szCurAmtBuff);

	if(fDisplayForExRate)
	{
		inLength=strlen(srTransRec.szDCCFXRate)-srTransRec.inDCCFXRateMU;
		memset(szTemp,0x00,sizeof(szTemp));
		memcpy(szTemp,srTransRec.szDCCFXRate,inLength);
		memcpy(&szTemp[inLength],".",1);
		memcpy(&szTemp[inLength+1],&srTransRec.szDCCFXRate[inLength],srTransRec.inDCCFXRateMU);
			
		setLCDPrint((strTCT.byTerminalType%2)?5:7, DISPLAY_POSITION_LEFT, "Exchange Rate:");
		setLCDPrint((strTCT.byTerminalType%2)?6:8, DISPLAY_POSITION_RIGHT,szTemp);	
	}

	if(fDisplayMarkup)
	{
		memset(szTemp,0x00,sizeof(szTemp));
		memset(szTemp1,0x00,sizeof(szTemp1));
		
		inMarkup = atof(srTransRec.szDCCMarkupPer);
		sprintf(szTemp,"%.2f",inMarkup);
		sprintf(szTemp1,"%20.20s",szTemp);
		//sprintf(szTemp1,"%s",szTemp);
		sprintf(szTemp2,"Markup:%s",szTemp1);
		strcat(szTemp2,"%");
		
		setLCDPrint((strTCT.byTerminalType%2)?7:10,DISPLAY_POSITION_LEFT,szTemp2);
	}

	while(1)
	{
		key=WaitKey(inGetIdleTimeOut(TRUE));

       	if(key == d_KBD_1)
       	{
			//srTransRec.fDCC = VS_FALSE;
			return FAIL;
       	}
       	else if(key == d_KBD_2)
       	{	
			return VS_CONTINUE;
		}

	}

	
	
}

