/*=========================================*       
 *             I N C L U D E S             *     
 *=========================================*/ 
#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <EMVAPLib.h>
#include <EMVLib.h>


#include "../Includes/myEZLib.h"
#include "../Includes/wub_lib.h"
#include "../Includes/POSTypedef.h"

#include "../FileModule/myFileFunc.h"
#include "..\Includes\CTOSInput.h"
#include "../UI/Display.h"
#include "../Debug/Debug.h"


void vdRemoveCard(void)
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
            //clearLine(inRowtmp);
            //CTOS_LCDTPrintXY(5,inRowtmp,"PLEASE REMOVE CARD");

			if (!fDisplayBox)
			{
	    		//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Please remove card","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
	    		//fDisplayBox = TRUE;
			}

			//inDisplayMessageBoxWithButtonConfirmation(1,8,"","Please remove card","","",MSG_TYPE_WARNING, BUTTON_TYPE_NONE_OK);
			vdPromptMessageUI("Please remove card", "", "", MSG_TYPE_REMOVE);
			CTOS_Beep();
			CTOS_Delay(300);
			continue;
            
        }   
        break;
    }
}

short shChk_ExpireDate(void)
{
    BYTE     szYear[3];
    BYTE     szMonth[3];
    BYTE     szTemp[5];
    CTOS_RTC SetRTC;


    memset(szYear, 0x00, sizeof(szYear));
    memset(szMonth, 0x00, sizeof(szMonth));
    
    //Read the date and the time //
    CTOS_RTCGet(&SetRTC); 
    wub_hex_2_str(srTransRec.szExpireDate, szTemp, 2);    
    memcpy(szYear,&szTemp[0],2);   
    memcpy(szMonth,&szTemp[2],2);    

    
    vdDebug_LogPrintf("System Year[%d]-[%s] Month[%d]-[%s]",SetRTC.bYear ,szYear, SetRTC.bMonth, szMonth);
    /* modified as month and date copied wrongly - meena 08/01/2013*/
    if(SetRTC.bYear > atoi(szYear))
    {
        return CARD_EXPIRED;
    }
    else if(SetRTC.bYear == atoi(szYear))
    {
        if(SetRTC.bMonth > atoi(szMonth))
        {
            return CARD_EXPIRED;
        }
    }
    return d_OK;
}

short chk_luhn(BYTE *szPAN)
{
    int i ; 
    USHORT usSum , usWeight , usTmp ;
    BYTE byLen ;

    usSum = 0 ;
    usWeight = 2 ;
    byLen = strlen ( szPAN ) ;
       
    for ( i = byLen - 2 ; i >= 0 ; i -- )
    {
        usTmp = usWeight * ( szPAN[i] - '0' ) ;
        usSum += usTmp / 10 ;
        usSum += usTmp % 10 ;
        if ( usWeight == 2 )
            usWeight = 1 ;
        else
            usWeight = 2 ;
    }

    usTmp = szPAN[byLen - 1] - '0' ;

    if ( ( usTmp + usSum ) % 10 == 0 )
        return ( SUCCESS ) ;
    else
        return ( FAIL ) ;

}

void vdGetCardHolderName(BYTE *szTrack1, USHORT usTRack1Len)
{
    USHORT  usIndex ;
    BYTE    byNameFlag = 0;
    USHORT  usNameIndex = 0;
    
    for (usIndex = 0; usIndex < usTRack1Len;  usIndex++)
    {
        //1027
        if((szTrack1[usIndex] == '^') || (szTrack1[usIndex] == '>')) 
        {
            if(byNameFlag == 0)
            {
                byNameFlag = 1;
                continue;
            }
            else
                return;
        }
        
        if( byNameFlag == 1)
        {
            srTransRec.szCardholderName[usNameIndex] = szTrack1[usIndex];
            usNameIndex++;
        }
            
    }
}

short shAnalysisTrack2(BYTE *szTrack2, USHORT usTRack2Len) //Invalid card reading fix -- jzg
{
	USHORT  usIndex = 0;
	BYTE    byPanFlag = 0;
	BYTE    byExpFlag = 0;
	BYTE    bySerCodeFlag = 0;
	USHORT  usDataIndex  = 0;
	BYTE    szDataTmp[5] = {0};

	BYTE		bySeparatorFlag = 0; //Invalid card reading fix -- jzg


	vdDebug_LogPrintf("shAnalysisTrack2=%s", szTrack2);

	memset(srTransRec.szPAN, 0x00, sizeof(srTransRec.szPAN));

	byPanFlag = 1;
	for(usIndex = 0; usIndex < usTRack2Len; usIndex++)
	{
		if(byPanFlag)
		{
			if((szTrack2[usIndex] == 'D') || (szTrack2[usIndex] == '=')|| (szTrack2[usIndex] == ';'))
			{
		    
				byPanFlag = 0;
				byExpFlag = 1;
				bySerCodeFlag = 0;
				srTransRec.byPanLen = usDataIndex;
				usDataIndex = 0;
				bySeparatorFlag = 1;
				continue;
			}
			else
			{
				srTransRec.szPAN[usDataIndex] = szTrack2[usIndex];
				usDataIndex++;
				
			}
		}


		if(byExpFlag)
		{
			memset(szDataTmp, 0x00, sizeof(szDataTmp));
			wub_str_2_hex(&szTrack2[usIndex], szDataTmp, 4);
			srTransRec.szExpireDate[0] = szDataTmp[0];
			srTransRec.szExpireDate[1] = szDataTmp[1];
			usIndex = usIndex + 4;
			byPanFlag = 0;
			byExpFlag = 0;
			bySerCodeFlag = 1;
			
		}

		if(bySerCodeFlag)
		{
			memcpy(srTransRec.szServiceCode, &szTrack2[usIndex], 3);
			srTransRec.byPINEntryCapability = (atoi(srTransRec.szServiceCode) % 10); // ISO: Set PIN entry capability for later use -- jzg			
			return d_OK;
		}
	}

	//Invalid card reading fix -- jzg
	if (bySeparatorFlag != 1)
		return INVALID_CARD;
	else
		return d_OK;
}


//gcitra
void  vdAnalysisTrack1(BYTE *szTrack1, USHORT usTRack1Len)
{
    USHORT  usIndex = 0;
    BYTE    byPanFlag = 0;
    BYTE    byExpFlag = 0;
    BYTE    bySerCodeFlag = 0;
    USHORT  usDataIndex  = 0;
    BYTE    byNameFlag = 0;
    BYTE    szDataTmp[5] = {0};
    USHORT  usNameIndex = 0;


//	BYTE		bySeparatorFlag = 0;


	vdDebug_LogPrintf("inAnalysisTrack1=%s", szTrack1);

	
    byPanFlag = 1;
    for(usIndex = 0; usIndex < usTRack1Len; usIndex++)
    {
        if(byPanFlag)
        {
			if((szTrack1[usIndex] == '^') || (szTrack1[usIndex] == '>')) 
            {
                byPanFlag = 0;
		        byNameFlag = 1;
                byExpFlag = 0;
                bySerCodeFlag = 0;
                srTransRec.byPanLen = usDataIndex;
                usDataIndex = 0;
								//bySeparatorFlag = 1;
                continue;
            }
            else
            {
                if(szTrack1[usIndex] != 'B'){

                srTransRec.szPAN[usDataIndex] = szTrack1[usIndex];
                usDataIndex++;
                }
            }
        }


        if( byNameFlag == 1)
        {
            //1027
			if((szTrack1[usIndex] == '^') || (szTrack1[usIndex] == '>')) {
                byPanFlag = 0;
		  byNameFlag = 0;
                byExpFlag = 1;
                bySerCodeFlag = 0;

		  continue;
            }else{
            	srTransRec.szCardholderName[usNameIndex] = szTrack1[usIndex];
           	usNameIndex++;
	     }
	    
	     
        }

        
        if(byExpFlag)
        {
            memset(szDataTmp, 0x00, sizeof(szDataTmp));
            wub_str_2_hex(&szTrack1[usIndex], szDataTmp, 4);
            srTransRec.szExpireDate[0] = szDataTmp[0];
            srTransRec.szExpireDate[1] = szDataTmp[1];
            usIndex = usIndex + 4;
                byPanFlag = 0;
		  byNameFlag = 0;
                byExpFlag = 0;
                bySerCodeFlag = 1;

        }
        
        if(bySerCodeFlag)
        {
            memcpy(srTransRec.szServiceCode, &szTrack1[usIndex], 3);
            return;
        }
    }
  
	//if (bySeparatorFlag != 1)
	//	return INVALID_CARD;
	//else
	//	return d_OK;
	return;
}


//gcitra


