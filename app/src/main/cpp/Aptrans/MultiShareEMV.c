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

#include "MultiApTrans.h"
#include "..\Database\DatabaseFunc.h"

//#include "..\Includes\POSTypedef.h"
#include "..\Includes\Wub_lib.h"
#include "..\Includes\postrans.h"

#include "..\Debug\Debug.h"

#include "MultiShareEMV.h"
#include "../Ui/Display.h"


#define EMV_DATA_POOL_MAX   2048
static BYTE szEMVDataPool[2048];
static USHORT usEMVDataTotalLen;

/* ==========================================================================
 * FUNCTION NAME: TermDataGet
 * DESCRIPTION:               
 * RETURN:        
 * ========================================================================== */   
short usCTOSS_GetEMVDataPoolTagData(USHORT usTag, BYTE *buf)
{
    USHORT i, usLen;
    BYTE byTag[2];
	
    byTag[0] = (BYTE)(usTag / 256);
    byTag[1] = (BYTE)(usTag % 256);
	
    i = 0;
 
    while (i < usEMVDataTotalLen)
    {
        usLen = szEMVDataPool[i + 2] * 256 + szEMVDataPool[i + 3];
        if (szEMVDataPool[i] == byTag[0] && szEMVDataPool[i + 1] == byTag[1])
        {
            wub_memcpy(buf, &szEMVDataPool[i + 4], usLen);
            return usLen;
        }
        i += 4 + usLen;
    }
	
    return 0;
}

/***************
 *
 *Remove the data from the terminal
 *[in] tag
 *
 **************/
void vdCTOSS_EMVDataPoolTagRemove(WORD tag)
{
    BYTE byTag[2];
    USHORT i, j, len;

    i = j = 0;

    byTag[0] = (BYTE)(tag / 256);
    byTag[1] = (BYTE)(tag % 256);
    while (i < usEMVDataTotalLen)
    {
        len = 4 + szEMVDataPool[i + 2] * 256 + szEMVDataPool[i + 3];
        if (byTag[0] == szEMVDataPool[i] && byTag[1] == szEMVDataPool[i + 1])
        {
            i += len;
            continue;
        } else
        {
            wub_memcpy(&szEMVDataPool[j], &szEMVDataPool[i], len);
            i += len;
            j += len;
        }
    }

    usEMVDataTotalLen = j;
}

/* ==========================================================================
 * FUNCTION NAME: ShowAPList
 * DESCRIPTION: Add tag data to EMV data global pool 
 * RETURN:        
 * ========================================================================== */
USHORT usCTOSS_AddOrUpdateTagToEMVDataPool(USHORT usTag, USHORT usLen, BYTE *ucValue)
{
    USHORT i, usTmpLen;
    BYTE byTag[2],key;
    
    
    byTag[0] = (BYTE)(usTag / 256);
    byTag[1] = (BYTE)(usTag % 256);

    i = 0;
    while (i < usEMVDataTotalLen)
    {
        usTmpLen = szEMVDataPool[i + 2] * 256 + szEMVDataPool[i + 3];
        if (szEMVDataPool[i] == byTag[0] && szEMVDataPool[i + 1] == byTag[1])
        {
            if (usTmpLen == usLen)
            {
                wub_memcpy(&szEMVDataPool[i + 4], ucValue, usLen);
                return d_OK;
            }
            else
                vdCTOSS_EMVDataPoolTagRemove(usTag);
        }
        i += 4 + usTmpLen;
    }
    
    //Add new
    if (usEMVDataTotalLen + 2 + 2 + usLen >= EMV_DATA_POOL_MAX)
    {
        return EMV_POOL_FULL;
    }
    
    szEMVDataPool[usEMVDataTotalLen ++] = (BYTE)(usTag / 256);
    szEMVDataPool[usEMVDataTotalLen ++] = (BYTE)(usTag % 256);
    szEMVDataPool[usEMVDataTotalLen ++] = (BYTE)(usLen / 256);
    szEMVDataPool[usEMVDataTotalLen ++] = (BYTE)(usLen % 256);
    wub_memcpy(&szEMVDataPool[usEMVDataTotalLen], ucValue, usLen);
    usEMVDataTotalLen += usLen;
    return d_OK;
}

/*
USHORT usCTOSS_FindTagFromDataPackage(USHORT tag, BYTE *value, USHORT *length, const BYTE *buffer, USHORT bufferlen)
{
    BYTE *ptr = NULL;
    BYTE bTagBuf[4];
    USHORT bufLen;
    USHORT len = 0;
    USHORT bytesRead;
    USHORT usTagLen;
    
    if ((tag & 0x1F00) == 0x1F00)
    {
        usTagLen = 2;
        bTagBuf[0] = tag >> 8;
        bTagBuf[1] = tag & 0x00FF;
    }
    else
    {
        usTagLen = 1;
        bTagBuf[0] = tag;
    }
    
    *length = len;
    
    ptr = (BYTE *) & buffer[0];
    bufLen = bufferlen;

    bytesRead = 0;

    while(bytesRead < bufLen)
    {
        if(0 == memcmp(bTagBuf, &ptr[bytesRead], usTagLen))
        {
            bytesRead += usTagLen;
            len = ptr[bytesRead++];
            memcpy(value, &ptr[bytesRead], len);
            *length = len;

            break;
        }
        else
        {   
            if ((ptr[bytesRead] & 0x1F) == 0x1F)
                bytesRead += 2;
            else
                bytesRead ++;
            len = ptr[bytesRead++];
            bytesRead += len;
        }
     
    }
    
    if(*length > 0)
        return d_OK;

    return d_NO;
}
*/
USHORT usCTOSS_FindTagFromDataPackage(unsigned int tag, BYTE *value, USHORT *length, const BYTE *buffer, USHORT bufferlen)
{
    BYTE *ptr = NULL;
    BYTE bTagBuf[4];
    USHORT bufLen;
    USHORT len = 0;
    USHORT bytesRead;
    USHORT usTagLen;

	if ((tag & 0xDF8000) == 0xDF8000)
	{
		usTagLen = 3;
		bTagBuf[0] = tag >> 16;
		bTagBuf[1] = tag >> 8;
		bTagBuf[2] = tag & 0x0000FF;
	}
    //else if ((tag & 0x9F00) == 0x9F00)
    else if ((tag & 0x1F00) == 0x1F00)
    {
        usTagLen = 2;
        bTagBuf[0] = tag >> 8;
        bTagBuf[1] = tag & 0x00FF;
    }
    else
    {
        usTagLen = 1;
        bTagBuf[0] = tag;
    }
    
    *length = len;
    
    ptr = (BYTE *) & buffer[0];
    bufLen = bufferlen;

    bytesRead = 0;

    while(bytesRead < bufLen)
    {
        if(0 == memcmp(bTagBuf, &ptr[bytesRead], usTagLen))
        {
            bytesRead += usTagLen;
            len = ptr[bytesRead++];
            memcpy(value, &ptr[bytesRead], len);
            *length = len;

            break;
        }
        else
        {   
            if (((ptr[bytesRead] & 0xDF) == 0xDF) && ((ptr[bytesRead+1] & 0x80) == 0x80))
                bytesRead += 3;
//            else if ((ptr[bytesRead] & 0x9F) == 0x9F)
			else if ((ptr[bytesRead] & 0x1F) == 0x1F)
                bytesRead += 2;
            else
                bytesRead ++;
            len = ptr[bytesRead++];
            bytesRead += len;
        }
     
    }
    
    if(*length > 0)
        return d_OK;

    return d_NO;
}

BYTE* ptCTOSS_FindTagAddr(BYTE *bFindTag, BYTE *bInString, USHORT usInStringLen)
{
    BYTE bBuf[5];
    USHORT usOffset = 0;
    
    while(1)
    {
        if(0 == memcmp(bFindTag, &bInString[usOffset], SHARE_EMV_DEFINE_TAGS_LEN))
        {
            return &bInString[usOffset];
        }
        else
        {
            usOffset += SHARE_EMV_DEFINE_TAGS_LEN;
            memcpy(bBuf, &bInString[usOffset], SHARE_EMV_DEFINE_LEN);
            usOffset += SHARE_EMV_DEFINE_LEN;

            bBuf[SHARE_EMV_DEFINE_LEN] = 0x00;
            usOffset += atoi(bBuf);
        }

        if(usOffset >= usInStringLen)
        {
            return NULL;
        }
    }
}

USHORT usCTOSS_PackTagLenValue(BYTE *bDataBuf, BYTE *bTag, USHORT usTagValueLen, BYTE *bTagValue)
{
    BYTE bBuf[8];
    USHORT usInLen = 0;
    
    memcpy(bDataBuf, bTag, SHARE_EMV_DEFINE_TAGS_LEN);
    usInLen = SHARE_EMV_DEFINE_TAGS_LEN;

    memset(bBuf, 0x00, sizeof(bBuf));
    sprintf(bBuf, "%03d", usTagValueLen);
    memcpy(&bDataBuf[usInLen], bBuf, SHARE_EMV_DEFINE_LEN);
    usInLen += SHARE_EMV_DEFINE_LEN;

    memcpy(&bDataBuf[usInLen], bTagValue, usTagValueLen);
    usInLen += usTagValueLen;

    return usInLen;
}

USHORT usCTOSS_GetTagLenValue(BYTE *bDataBuf, USHORT usDataBufLen, BYTE *bTag, USHORT *usTagValueLen, BYTE *bTagValue)
{
    BYTE bBuf[8];
    BYTE *ptr = NULL;
    USHORT usDataLen = 0;

    ptr = ptCTOSS_FindTagAddr(bTag, bDataBuf, usDataBufLen);
    if(NULL == ptr)
    {
        *usTagValueLen = usDataLen;
        return usDataLen;
    }
    else
    {
        memcpy(bBuf, ptr+SHARE_EMV_DEFINE_TAGS_LEN, SHARE_EMV_DEFINE_LEN);
        bBuf[SHARE_EMV_DEFINE_LEN] = 0x00;
        usDataLen = atoi(bBuf);

        memcpy(bTagValue, ptr+SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN, usDataLen);
        *usTagValueLen = usDataLen;
    }

    return usDataLen;
}



USHORT usCTOSS_GetCardPresent(void)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_GetCardPresent, bInBuf, usInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = FALSE;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
        }
    }
    else
    {
        usResult = FALSE;
    }

    return usResult;
}

USHORT usCTOSS_EMVInitialize(void)
{
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;

    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Initialize, bInBuf, usInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = d_NO;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
        }
    }
    
    return usResult;
}

#if 1
USHORT usCTOSS_EMV_TxnAppSelect(IN BYTE* pPreferAID, IN USHORT usPreferAIDLen, OUT BYTE* pSelectedAID, OUT USHORT* pSelectedAIDLen, OUT BYTE *pSelectedAppLabel, OUT USHORT *pSelectedAppLabelLen)
{
	BYTE bTagStringHex[256];
    BYTE bInBuf[2048];
		BYTE bIntmpBuf[512];
    BYTE bOutBuf[2048];
    BYTE *ptr = NULL;
    USHORT usTagStringLen = 0;
    USHORT usInLen = 0,ustmpInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    USHORT usDataLen = 0;
	USHORT usFallback = 0;
	
	EMV_AID_ALLLIST psrCandidateList;
	EMV_AID_LIST_STRUCT psrAIDList;
	BYTE bAppNum = 0;
	int i = 0, isVisa = 0, isMaster = 0, isCUP = 0;
	EMV_AID_ALLLIST psrTempCandidateList;
	char szPIN[12+1];


	vdDebug_LogPrintf("saturn usCTOSS_EMV_TxnAppSelect test 1");

	memset(szPIN, 0x00, sizeof(szPIN));

    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
		ustmpInLen = 0;
    memset(bIntmpBuf, 0x00, sizeof(bIntmpBuf));
    //Prefer AID
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_PREFER_AID, usPreferAIDLen, pPreferAID);
		memcpy(bIntmpBuf,bInBuf,usInLen);
		ustmpInLen = usInLen;

	//check Non EMV card flag
	usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_CHK_NON_EMVCARD, 1, "0");
	memcpy(&bIntmpBuf[ustmpInLen],bInBuf,usInLen);
	ustmpInLen += usInLen;

	 //Select APP
	usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
//	if (!strTCT.fAllowVisaCUPAIDSelect || !strTCT.fAllowMasterCUPAIDSelect)
    		usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_SELECT_APP, 1, "1");
//	else
//    		usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_SELECT_APP, 1, "0");
	
	memcpy(&bIntmpBuf[ustmpInLen],bInBuf,usInLen);
	ustmpInLen += usInLen;
		
    usTagStringLen = strlen(GET_EMV_TAG_AFTER_SELECT_APP);
    wub_str_2_hex(GET_EMV_TAG_AFTER_SELECT_APP, bTagStringHex, usTagStringLen);
    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    //TagString
    //usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_GET_MULTI_TAG, usTagStringLen, bTagStringHex);
     usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_GET_MULTI_TAG, (usTagStringLen/2), bTagStringHex);
		memcpy(&bIntmpBuf[ustmpInLen],bInBuf,usInLen);
		ustmpInLen += usInLen;

		vdPCIDebug_HexPrintf("saturn TxnAppSelect",bIntmpBuf,ustmpInLen);
    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_App_SelectEx, bIntmpBuf, ustmpInLen, bOutBuf, &usOutLen);

    vdDebug_LogPrintf("saturn inMultiAP_RunIPCCmdTypes result %d", usResult);
	if(d_OK == usResult)
    {
        //status
        vdDebug_LogPrintf("usOutLen=[%d],bOutBuf=[%s]",usOutLen,bOutBuf);
        if (memcmp(bOutBuf,SHARE_EMV_SUB_IPC,3) == 0)
		{
			if (memcmp(&bOutBuf[3],SHARE_EMV_SELECTAPPLIST,1) == 0)
			{
				memset(&psrAIDList,0x00,sizeof(EMV_AID_LIST_STRUCT));
				memset(&psrCandidateList,0x00,sizeof(EMV_AID_ALLLIST));
				memset(&psrTempCandidateList,0x00,sizeof(EMV_AID_ALLLIST));
				memcpy(&psrCandidateList,&bOutBuf[4],sizeof(EMV_AID_ALLLIST));

				//up to this point, we don't have PAN yet, loop once to check is it is Visa UnionPay card or MasterCard UnionPay card
				bAppNum = psrCandidateList.countAID;
				for(i = 0 ; i < bAppNum ; i++)
				{
					vdPCIDebug_HexPrintf("AID", psrCandidateList.arrAIDList[i].baAID, psrCandidateList.arrAIDList[i].baAIDLen);
				
					if (memcmp(VISA_RID, psrCandidateList.arrAIDList[i].baAID, 5) == 0)
					{
				        vdDebug_LogPrintf("got visa");
						isVisa = 1;
					}
					else if (memcmp(MASTERCARD_RID, psrCandidateList.arrAIDList[i].baAID, 5) == 0)
					{
				        vdDebug_LogPrintf("got master");
						isMaster = 1;
					}
					else if (memcmp(UNIONPAY_RID, psrCandidateList.arrAIDList[i].baAID, 5) == 0)
					{
				        vdDebug_LogPrintf("got unionpay");
						isCUP = 1;
					}
				}
#if 0
		        vdDebug_LogPrintf("strTCT.fAllowVisaCUPAIDSelect=%d, strTCT.fAllowMasterCUPAIDSelect=%d", strTCT.fAllowVisaCUPAIDSelect, strTCT.fAllowMasterCUPAIDSelect);
				if ((!strTCT.fAllowVisaCUPAIDSelect && isVisa && isCUP)	||	// UnionPay not allowed, suppress UnionPay AID
					(!strTCT.fAllowMasterCUPAIDSelect && isMaster && isCUP))
				{
					for(i = 0 ; i < bAppNum ; i++)
					{
						if (memcmp(UNIONPAY_RID, psrCandidateList.arrAIDList[i].baAID, 5) == 0)
						{
					        vdDebug_LogPrintf("exclude unionpay");
						}
						else
						{
							memcpy(&psrTempCandidateList.arrAIDList[psrTempCandidateList.countAID], &psrCandidateList.arrAIDList[i],sizeof(EMV_AID_LIST_STRUCT));
							psrTempCandidateList.countAID++;
						}
					}
					psrTempCandidateList.currentIndex= psrCandidateList.currentIndex;
					memset(&psrCandidateList,0x00,sizeof(EMV_AID_ALLLIST));
					memcpy(&psrCandidateList,&psrTempCandidateList,sizeof(EMV_AID_ALLLIST));
				}
#endif
				usResult = OnAPPLISTEX(&psrCandidateList,&psrAIDList);
				vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);
				
				usInLen = 0;
    			memset(bInBuf, 0x00, sizeof(bInBuf));
				strcpy(bInBuf,SHARE_EMV_SUB_IPC);
				strcat(bInBuf,SHARE_EMV_SELECTAPPLIST);
				usInLen = strlen(bInBuf);
				memcpy(&bInBuf[usInLen],&psrAIDList,sizeof(EMV_AID_LIST_STRUCT));
				usInLen += sizeof(EMV_AID_ALLLIST);
				vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

				memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    			usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_App_SelectEx, bInBuf, usInLen, bOutBuf, &usOutLen);

    			vdDebug_LogPrintf("saturn usCTOSS_EMV_TxnAppSelect after inMultiAP_RunIPCCmdTypes 1 %d", usResult);
				if(d_OK != usResult)
					return usResult;
			}

			/*handle if check Non EMV in share EMV*/
			if (memcmp(&bOutBuf[3], SHARE_EMV_NONEMVCARD, 1) == 0)
			{
				
    			vdDebug_LogPrintf("saturn usCTOSS_EMV_TxnAppSelect non emv card");
				usResult = usCTOSS_NonEMVCardProcess();
				vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);
				
				usInLen = 0;
    			memset(bInBuf, 0x00, sizeof(bInBuf));
				strcpy(bInBuf,SHARE_EMV_SUB_IPC);
				strcat(bInBuf,SHARE_EMV_NONEMVCARD);
				usInLen = strlen(bInBuf);
				if (d_OK == usResult)
					memcpy(&bInBuf[usInLen], "\x00",sizeof(usResult));
				else
					memcpy(&bInBuf[usInLen], "\x01",sizeof(usResult));
				
				usInLen += 1;
				vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

				memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    			usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_App_Select, bInBuf, usInLen, bOutBuf, &usOutLen);
    			vdDebug_LogPrintf("saturn usCTOSS_EMV_TxnAppSelect after inMultiAP_RunIPCCmdTypes 2 %d", usResult);

				if(d_OK != usResult)
					return usResult;
			}
		}
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = d_NO;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
            vdDebug_LogPrintf("saturn pSelectedAID usResult[%d]", usResult);
			if(usResult != 0)
			{
				//VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - start -- jzg
				if (usResult == 12) //JCB: If application is blocked deny the transaction -- jzg
				{
					vdDebug_LogPrintf("saturn card blocked usResult[%d]", usResult);
					return EMV_CARD_BLOCKED;
				}
				//VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - end -- jzg

				//EMV: If AID not found display "TRANS NOT ALLOWED" - start -- jzg
				if (usResult == 2)
				{
					vdDebug_LogPrintf("saturn trans not allowed usResult[%d]", usResult);
					return EMV_TRANS_NOT_ALLOWED;
				}
				//EMV: If AID not found display "TRANS NOT ALLOWED" - end -- jzg
				
				usFallback = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN + 1];
				if(usFallback == 1)//fallback
				{
					vdDebug_LogPrintf("saturn fallback usResult[%d]", usResult);
					return EMV_FALLBACK;
					
				}
				
				vdDebug_LogPrintf("return, fallback usResult[%d]", usResult);
			}
            else
            {
                //pSelectedAID
                usCTOSS_GetTagLenValue(bOutBuf, usOutLen, SHARE_EMV_SELECTED_AID, &usDataLen, pSelectedAID);
                *pSelectedAIDLen =  usDataLen; 
                
                //pSelectedAppLabel
                usCTOSS_GetTagLenValue(bOutBuf, usOutLen, SHARE_EMV_SELECTED_APP_LAB, &usDataLen, pSelectedAppLabel);
                *pSelectedAppLabelLen =  usDataLen; 
            }
        }
    }

	
	vdDebug_LogPrintf("saturn usCTOSS_EMV_TxnAppSelect return %d", usResult);
    return usResult;
}
#else

USHORT usCTOSS_EMV_TxnAppSelect(IN BYTE* pPreferAID, IN USHORT usPreferAIDLen, OUT BYTE* pSelectedAID, OUT USHORT* pSelectedAIDLen, OUT BYTE *pSelectedAppLabel, OUT USHORT *pSelectedAppLabelLen)
{
	BYTE bTagStringHex[256];
    BYTE bInBuf[512];
		BYTE bIntmpBuf[512];
    BYTE bOutBuf[2048];
    BYTE *ptr = NULL;
    USHORT usTagStringLen = 0;
    USHORT usInLen = 0,ustmpInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    USHORT usDataLen = 0;
	USHORT usFallback = 0;

    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
		ustmpInLen = 0;
    memset(bIntmpBuf, 0x00, sizeof(bIntmpBuf));
    //Prefer AID
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_PREFER_AID, usPreferAIDLen, pPreferAID);
		memcpy(bIntmpBuf,bInBuf,usInLen);
		ustmpInLen = usInLen;

		
    usTagStringLen = strlen(GET_EMV_TAG_AFTER_SELECT_APP);
    wub_str_2_hex(GET_EMV_TAG_AFTER_SELECT_APP, bTagStringHex, usTagStringLen);
    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    //TagString
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_GET_MULTI_TAG, usTagStringLen, bTagStringHex);
		memcpy(&bIntmpBuf[ustmpInLen],bInBuf,usInLen);
		ustmpInLen += usInLen;

		vdPCIDebug_HexPrintf("TxnAppSelect",bIntmpBuf,ustmpInLen);
    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_App_SelectEx, bIntmpBuf, ustmpInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = d_NO;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
            vdDebug_LogPrintf("pSelectedAID usResult[%d]", usResult);
			if(usResult != 0)
			{
				//VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - start -- jzg
				if (usResult == 12) //JCB: If application is blocked deny the transaction -- jzg
				{
					vdDebug_LogPrintf("card blocked usResult[%d]", usResult);
					return EMV_CARD_BLOCKED;
				}
				//VISA: Testcase 29 - should display "CARD BLOCKED" instead of doing fallback - end -- jzg

				//EMV: If AID not found display "TRANS NOT ALLOWED" - start -- jzg
				if (usResult == 2)
				{
					vdDebug_LogPrintf("trans not allowed usResult[%d]", usResult);
					return EMV_TRANS_NOT_ALLOWED;
				}
				//EMV: If AID not found display "TRANS NOT ALLOWED" - end -- jzg
				
				usFallback = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN + 1];
				if(usFallback == 1)//fallback
				{
					vdDebug_LogPrintf("fallback usResult[%d]", usResult);
					return EMV_FALLBACK;
					
				}
				
				vdDebug_LogPrintf("return, fallback usResult[%d]", usResult);
			}
            else
            {
                //pSelectedAID
                usCTOSS_GetTagLenValue(bOutBuf, usOutLen, SHARE_EMV_SELECTED_AID, &usDataLen, pSelectedAID);
                *pSelectedAIDLen =  usDataLen; 
                
                //pSelectedAppLabel
                usCTOSS_GetTagLenValue(bOutBuf, usOutLen, SHARE_EMV_SELECTED_APP_LAB, &usDataLen, pSelectedAppLabel);
                *pSelectedAppLabelLen =  usDataLen; 
            }
        }
    }
    
    return usResult;
}
#endif

USHORT usCTOSS_EMV_TxnPerform(void)
{
    BYTE bTagStringHex[256];
    BYTE bInBuf[512];
    BYTE bOutBuf[2048];
    BYTE *ptr = NULL;
    USHORT usTagStringLen = 0;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult,ustmpResult;
    USHORT usDataLen = 0;

	//tine: android
	int inReEnterPin = 0;
	int inPinDigit = 0;
    BYTE szTitle[20+1];
    BYTE szDisMsg[200];

    BOOL fPINCancelled=VS_FALSE;
	
	usTagStringLen = strlen(GET_EMV_TAG_AFTER_1STAC);
    wub_str_2_hex(GET_EMV_TAG_AFTER_1STAC, bTagStringHex, usTagStringLen);

    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    //TagString
    //tine:  android - removed
    //usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_GET_MULTI_TAG, usTagStringLen, bTagStringHex);

	usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_GET_MULTI_TAG, (usTagStringLen/2), bTagStringHex);
	
    memset(bOutBuf, 0x00, sizeof(bOutBuf));    

	//test
	vdDebug_LogPrintf("saturn usCTOSS_EMV_TxnPerform start");
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);
	//usResult = inMultiAP_RunIPCCmdTypesEx("SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);

	vdDebug_LogPrintf("saturn usCTOSS_EMV_TxnPerform end %s %d",bOutBuf, usResult);


	
	vdDebug_LogPrintf("saturn bOutBuf %s", bOutBuf);
    if(d_OK == usResult)
    {
        //status    
        vdDebug_LogPrintf("saturn bOutBuf %s", bOutBuf);
		//test remove
		//strcpy(bOutBuf,SHARE_EMV_SUB_IPC);
		//bOutBuf[3] = SHARE_EMV_ONLINEPIN;
		
		//
		if (memcmp(bOutBuf,SHARE_EMV_SUB_IPC,3) == 0)
		{
		    
			if (memcmp(&bOutBuf[3],SHARE_EMV_ONLINEPIN,1) == 0)
			{
#if 0
			    usResult = inGetIPPPin();
				CTOS_LCDTClearDisplay();// clear the PIN entry screen.
#else
                vdDebug_LogPrintf("saturn before pin");
				srTransRec.fEMVPINEntered = TRUE;
				srTransRec.fPINByPassAllow = TRUE;
				usResult = inGetIPPPinEx();
				vdDebug_LogPrintf("saturn after pin, usResult=[%d]", usResult);

#endif
                #if 0
				if(usResult != d_NO && usResult != d_KMS2_GET_PIN_ABORT)
					vdDispTitleAndCardProcessing();					
                #else
				if(usResult != d_OK)
				{
					if (usResult == 1)
						fPINCancelled=VS_TRUE;
				}
				#endif
				vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);

				/* CUP: set srTransRec.fEMVPIN if PIN Bypass - start -- jzg */
				if (usResult == d_KMS2_GET_PIN_NULL_PIN)
					srTransRec.fEMVPIN = FALSE;
				else
					srTransRec.fEMVPIN = TRUE;

				vdDebug_LogPrintf("saturn emvpin %d", srTransRec.fEMVPIN);
				/* CUP: set srTransRec.fEMVPIN if PIN Bypass - end -- jzg */
				
				usInLen = 0;
    			memset(bInBuf, 0x00, sizeof(bInBuf));
				strcpy(bInBuf,SHARE_EMV_SUB_IPC);
				strcat(bInBuf,SHARE_EMV_ONLINEPIN);
				usInLen = strlen(bInBuf);
				bInBuf[usInLen] = (unsigned char)(usResult		& 0xFF);
		   		bInBuf[usInLen+1] = (unsigned char)(usResult >>  8 & 0xFF);
				usInLen += 2;
				vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

				memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    			usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);

				if(d_OK != usResult)
					return usResult;
			}

			if (memcmp(&bOutBuf[3],SHARE_EMV_PINVERIFYOK,1) == 0)
			{		
			    
				usInLen = 0;
				memset(bInBuf, 0x00, sizeof(bInBuf));
				strcpy(bInBuf,SHARE_EMV_SUB_IPC);
				strcat(bInBuf,SHARE_EMV_PINVERIFYOK);
				usInLen = strlen(bInBuf);
				  
				vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

				memset(szDisMsg,0,sizeof(szDisMsg));
				strcpy(szDisMsg, " ");
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "VERIFY PIN OK");
				//usCTOSS_LCDDisplay(szDisMsg);
				inDisplayMessageBoxWithButtonConfirmation(1,8,"","VERIFY PIN OK","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
				CTOS_Delay(700);
			  
				memset(bOutBuf, 0x00, sizeof(bOutBuf));	 
				usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);
			  
				if(d_OK != usResult)
					return usResult;
			}
			
			if (memcmp(&bOutBuf[3],SHARE_EMV_PINBLOCKED,1) == 0)
			{				
				usInLen = 0;
				memset(bInBuf, 0x00, sizeof(bInBuf));
				strcpy(bInBuf,SHARE_EMV_SUB_IPC);
				strcat(bInBuf,SHARE_EMV_PINBLOCKED);
				usInLen = strlen(bInBuf);
				
				vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);
			
				memset(szDisMsg,0,sizeof(szDisMsg));
				strcpy(szDisMsg, " ");
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "PIN BLOCKED");
				//usCTOSS_LCDDisplay(szDisMsg);	
				inDisplayMessageBoxWithButtonConfirmation(1,8,"","PIN BLOCKED","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
				CTOS_Delay(700);
			
				memset(bOutBuf, 0x00, sizeof(bOutBuf));    
				usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);
			
				if(d_OK != usResult)
					return usResult;
			}

			if (memcmp(&bOutBuf[3],SHARE_EMV_WRONGPIN,1) == 0)
			{				
				usInLen = 0;
				memset(bInBuf, 0x00, sizeof(bInBuf));
				strcpy(bInBuf,SHARE_EMV_SUB_IPC);
				strcat(bInBuf,SHARE_EMV_WRONGPIN);
				usInLen = strlen(bInBuf);
				
				vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);
			
				memset(szDisMsg,0,sizeof(szDisMsg));
				strcpy(szDisMsg, " ");
				strcat(szDisMsg, "|");
				strcat(szDisMsg, "WRONG PIN");
				//usCTOSS_LCDDisplay(szDisMsg);	
				inDisplayMessageBoxWithButtonConfirmation(1,8,"","WRONG PIN","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
				CTOS_Beep();
				CTOS_Delay(700);
			
				memset(bOutBuf, 0x00, sizeof(bOutBuf));    
				usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);
			
				if(d_OK != usResult)
					return usResult;
			}

			  if (memcmp(&bOutBuf[3],SHARE_EMV_ShOWPINDIGIT,1) == 0)
			  {
				  vdDebug_LogPrintf("SHARE_EMV_ShOWPINDIGIT [%d]", atoi(&bOutBuf[4]));
				   srTransRec.fEMVPINEntered = TRUE;
				  inPinDigit = atoi(&bOutBuf[4]);
				  OnShowPinDigit(atoi(&bOutBuf[4]),inReEnterPin);

				  vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);
				  usInLen = 0;
				  memset(bInBuf, 0x00, sizeof(bInBuf));
				  strcpy(bInBuf,SHARE_EMV_SUB_IPC);
				  strcat(bInBuf,SHARE_EMV_ShOWPINDIGIT);
				  usInLen = strlen(bInBuf);
				
				  vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

				  memset(bOutBuf, 0x00, sizeof(bOutBuf));	 
				  usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);

				  if(d_OK != usResult)
					  return usResult;

					 if (memcmp(&bOutBuf[3],SHARE_EMV_SHOWVIRTUALPIN,1) == 0)
					  {
						  vdDebug_LogPrintf("SHARE_EMV_SHOWVIRTUALPIN");
						  USHORT usPINPadButtonNum;
						  BYTE bPINPadButtonNum[6];
						  BYTE bPINPadButtonInfo[1024];	  
						  BYTE bPINPadButtonInfoStr[1024];
						  USHORT usPINPadButtonInfoLen;
						  
						  BYTE usPINPadButtonInfoLenStr[10];
						  
						  

						  //usCTOSS_BackToProgress(" ");
						
						  usResult = OnShowVirtualPIN(&usPINPadButtonNum, bPINPadButtonInfo, &usPINPadButtonInfoLen);

						  vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);
						  usInLen = 0;
						  memset(bInBuf, 0x00, sizeof(bInBuf));
						  strcpy(bInBuf,SHARE_EMV_SUB_IPC);
						  strcat(bInBuf,SHARE_EMV_SHOWVIRTUALPIN);
						  
						  memset(bPINPadButtonNum, 0x00, sizeof(bPINPadButtonNum));
						  sprintf(bPINPadButtonNum, "%02d", usPINPadButtonNum);
						  strcat(bInBuf,bPINPadButtonNum);
						    
						  //usInLen =usInLen + strlen(bPINPadButtonNum);

						  memset(bPINPadButtonInfoStr, 0x00, sizeof(bPINPadButtonInfoStr));			  
						  memset(usPINPadButtonInfoLenStr, 0x00, sizeof(usPINPadButtonInfoLenStr));			  
						  
						  wub_hex_2_str(bPINPadButtonInfo,bPINPadButtonInfoStr,usPINPadButtonInfoLen);
						  vdDebug_LogPrintf("usPINPadButtonInfoLen=[%d],bPINPadButtonInfoStr=[%s]",usPINPadButtonInfoLen,bPINPadButtonInfoStr);

						  sprintf(usPINPadButtonInfoLenStr, "%06d", usPINPadButtonInfoLen);

						  strcat(bInBuf,usPINPadButtonInfoLenStr);
						  strcat(bInBuf,bPINPadButtonInfoStr);
			  
						  usInLen = strlen(bInBuf);
						  vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

						  memset(bOutBuf, 0x00, sizeof(bOutBuf));	 
						  usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);
						  if(d_OK != usResult)
							  return usResult;
						  while (1)
						  {
						  	if(d_OK == usResult)
						    {
						        //status 
						        vdDebug_LogPrintf("usOutLen=[%d],bOutBuf=[%s]",usOutLen,bOutBuf);
								if (memcmp(bOutBuf,SHARE_EMV_SUB_IPC,3) == 0)
								{
									  if (memcmp(&bOutBuf[3],SHARE_EMV_ShOWPINDIGIT,1) == 0)
									  {
										  vdDebug_LogPrintf("SHARE_EMV_ShOWPINDIGIT [%d]", atoi(&bOutBuf[4]));
										  
										  inPinDigit = atoi(&bOutBuf[4]);
										  OnShowPinDigit(atoi(&bOutBuf[4]),inReEnterPin);

										  vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);
										  usInLen = 0;
										  memset(bInBuf, 0x00, sizeof(bInBuf));
										  strcpy(bInBuf,SHARE_EMV_SUB_IPC);
										  strcat(bInBuf,SHARE_EMV_ShOWPINDIGIT);
										  usInLen = strlen(bInBuf);
										
										  vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

										  memset(bOutBuf, 0x00, sizeof(bOutBuf));	 
										  usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);

										  if(d_OK != usResult)
											  return usResult;

										  if (d_OK == usResult)
										  	continue;
									  }
									
								}

								vdDebug_LogPrintf("usOutLen=[%d],bOutBuf=[%s]",usOutLen,bOutBuf);
								if (memcmp(&bOutBuf[3],SHARE_EMV_SHOWVIRTUALPIN,1) == 0)
								 {
									 vdDebug_LogPrintf("SHARE_EMV_SHOWVIRTUALPIN");
									 USHORT usPINPadButtonNum;
									 BYTE bPINPadButtonNum[6];
									 BYTE bPINPadButtonInfo[1024];	 
									 BYTE bPINPadButtonInfoStr[1024];
									 USHORT usPINPadButtonInfoLen;
									 
									 BYTE usPINPadButtonInfoLenStr[10];
									 
									 //usCTOSS_BackToProgress(" ");
								   
									 usResult = OnShowVirtualPIN(&usPINPadButtonNum, bPINPadButtonInfo, &usPINPadButtonInfoLen);
								
									 vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);
									 usInLen = 0;
									 memset(bInBuf, 0x00, sizeof(bInBuf));
									 strcpy(bInBuf,SHARE_EMV_SUB_IPC);
									 strcat(bInBuf,SHARE_EMV_SHOWVIRTUALPIN);
									 
									 memset(bPINPadButtonNum, 0x00, sizeof(bPINPadButtonNum));
									 sprintf(bPINPadButtonNum, "%02d", usPINPadButtonNum);
									 strcat(bInBuf,bPINPadButtonNum);
									   
									 //usInLen =usInLen + strlen(bPINPadButtonNum);
								
									 memset(bPINPadButtonInfoStr, 0x00, sizeof(bPINPadButtonInfoStr));			 
									 memset(usPINPadButtonInfoLenStr, 0x00, sizeof(usPINPadButtonInfoLenStr));			 
									 
									 wub_hex_2_str(bPINPadButtonInfo,bPINPadButtonInfoStr,usPINPadButtonInfoLen);
									 vdDebug_LogPrintf("usPINPadButtonInfoLen=[%d],bPINPadButtonInfoStr=[%s]",usPINPadButtonInfoLen,bPINPadButtonInfoStr);
								
									 sprintf(usPINPadButtonInfoLenStr, "%06d", usPINPadButtonInfoLen);
								
									 strcat(bInBuf,usPINPadButtonInfoLenStr);
									 strcat(bInBuf,bPINPadButtonInfoStr);
								
									 usInLen = strlen(bInBuf);
									 vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);
								
									 memset(bOutBuf, 0x00, sizeof(bOutBuf));	
									 usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);
									 if(d_OK != usResult)
										 return usResult;

									 if (d_OK == usResult)
										  	continue;
							    }

								vdDebug_LogPrintf("usOutLen=[%d],bOutBuf=[%s]",usOutLen,bOutBuf);
								if (memcmp(bOutBuf,SHARE_EMV_SUB_IPC,3) == 0)
								{
									  if (memcmp(&bOutBuf[3],SHARE_EMV_GETPINDONE,1) == 0)
									  {
										  vdDebug_LogPrintf("SHARE_EMV_GETPINDONE [%d]", atoi(&bOutBuf[4]));
										  
										  usResult = OnGetPINDone();

										  vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);
										  usInLen = 0;
										  memset(bInBuf, 0x00, sizeof(bInBuf));
										  strcpy(bInBuf,SHARE_EMV_SUB_IPC);
										  strcat(bInBuf,SHARE_EMV_GETPINDONE);
										  usInLen = strlen(bInBuf);
										
										  vdDebug_LogPrintf("usInLen=[%d],str=[%s],inPinDigit=[%d],fOfflinePinbypass=[%d]",usInLen,bInBuf,inPinDigit,strTCT.fOfflinePinbypass);
										  if (strTCT.fOfflinePinbypass == 1)
										  {
										  	if (inPinDigit == 0)
										  	{
                                            	strcpy(szDisMsg, " ");
                                                strcat(szDisMsg, "|");
                                                strcat(szDisMsg, "PIN Bypass");
                                                srTransRec.fEMVPINBYPASS = TRUE;
                                                //usCTOSS_LCDDisplay(szDisMsg);  
                                                inDisplayMessageBoxWithButtonConfirmation(1,8,"","PIN Bypass","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
                                                CTOS_Beep();
                                                CTOS_Delay(700);
										  	}
										  }

										  memset(bOutBuf, 0x00, sizeof(bOutBuf));	 
										  usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);

											vdDebug_LogPrintf("SHARE_EMV_GETPINDONE CHECK usOutLen=[%d],bOutBuf=[%s], usResult[%d]", usOutLen,bOutBuf, usResult);

											if (memcmp(&bOutBuf[0],"IPC1",4) == 0)
											{
												srTransRec.fEMVPINEntered = TRUE;
												//usResult = inGetIPPPin();
												usResult = GetPIN_With_3DESDUKPTEx();
												vdDebug_LogPrintf("usResult=[%d],usResult=[%x]",usResult,usResult);

												/* CUP: set srTransRec.fEMVPIN if PIN Bypass - start -- jzg */
												if (usResult == d_KMS2_GET_PIN_NULL_PIN)
												{
												srTransRec.fEMVPIN = FALSE;
												}
												else
												srTransRec.fEMVPIN = TRUE;
												/* CUP: set srTransRec.fEMVPIN if PIN Bypass - end -- jzg */

												usInLen = 0;
												memset(bInBuf, 0x00, sizeof(bInBuf));
												strcpy(bInBuf,SHARE_EMV_SUB_IPC);
												strcat(bInBuf,SHARE_EMV_ONLINEPIN);
												usInLen = strlen(bInBuf);
												bInBuf[usInLen] = (unsigned char)(usResult		& 0xFF);
												bInBuf[usInLen+1] = (unsigned char)(usResult >>  8 & 0xFF);
												usInLen += 2;
												vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);

												memset(bOutBuf, 0x00, sizeof(bOutBuf));    
												usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);

												if(d_OK != usResult)
												return usResult;
											}

										  if (memcmp(&bOutBuf[0],"IPC9",4) == 0)
										  { 			  
											  usInLen = 0;
											  memset(bInBuf, 0x00, sizeof(bInBuf));
											  strcpy(bInBuf,SHARE_EMV_SUB_IPC);
											  strcat(bInBuf,SHARE_EMV_PINVERIFYOK);
											  usInLen = strlen(bInBuf);
											  
											  vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);
										  
											  CTOS_Beep();
											  strcpy(szDisMsg, " ");
											  strcat(szDisMsg, "|");
											  strcat(szDisMsg, "VERIFY PIN OK");
											  //usCTOSS_LCDDisplay(szDisMsg);   
											  inDisplayMessageBoxWithButtonConfirmation(1,8,"","VERIFY PIN OK","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
											  CTOS_Delay(700);
										  
											  memset(bOutBuf, 0x00, sizeof(bOutBuf));	 
											  usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);
										  
											  if(d_OK != usResult)
												  return usResult;
										  }

										  if (memcmp(&bOutBuf[0],"IPC8",4) == 0)										  
										  { 			  
											  usInLen = 0;
											  memset(bInBuf, 0x00, sizeof(bInBuf));
											  strcpy(bInBuf,SHARE_EMV_SUB_IPC);
											  strcat(bInBuf,SHARE_EMV_PINBLOCKED);
											  usInLen = strlen(bInBuf);
											  
											  vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);
										  
											  memset(szDisMsg,0,sizeof(szDisMsg));
											  strcpy(szDisMsg, " ");
											  strcat(szDisMsg, "|");
											  strcat(szDisMsg, "PIN BLOCKED");
											  //usCTOSS_LCDDisplay(szDisMsg);   
											  inDisplayMessageBoxWithButtonConfirmation(1,8,"","PIN BLOCKED","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
											  CTOS_Delay(700);
										  
											  memset(bOutBuf, 0x00, sizeof(bOutBuf));	 
											  usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);
										  
											  if(d_OK != usResult)
												  return usResult;
										  }

										  if (memcmp(&bOutBuf[0],"IPCA",4) == 0)										  
										  { 			  
											  usInLen = 0;
											  memset(bInBuf, 0x00, sizeof(bInBuf));
											  strcpy(bInBuf,SHARE_EMV_SUB_IPC);
											  strcat(bInBuf,SHARE_EMV_WRONGPIN);
											  usInLen = strlen(bInBuf);
											  
											  vdDebug_LogPrintf("usInLen=[%d],str=[%s]",usInLen,bInBuf);
										  
											  memset(szDisMsg,0,sizeof(szDisMsg));
											  strcpy(szDisMsg, " ");
											  strcat(szDisMsg, "|");
											  strcat(szDisMsg, "WRONG PIN");
											  //usCTOSS_LCDDisplay(szDisMsg);   
											  inDisplayMessageBoxWithButtonConfirmation(1,8,"","WRONG PIN","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
											  CTOS_Beep();
											  CTOS_Delay(700);
										  
											  memset(bOutBuf, 0x00, sizeof(bOutBuf));	 
											  usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_PerformEx, bInBuf, usInLen, bOutBuf, &usOutLen);
												vdDebug_LogPrintf("333 usOutLen=[%d],bOutBuf=[%s]",usOutLen,bOutBuf);
										  	  if (memcmp(&bOutBuf[3],SHARE_EMV_ShOWPINDIGIT,1) == 0)
											  {
											  		inReEnterPin = 1;
													continue;
											  }
											  if(d_OK != usResult)
												  return usResult;
										  }

										  if (memcmp(&bOutBuf[3],SHARE_EMV_ShOWPINDIGIT,1) == 0)
										  {
												inReEnterPin = 1;
												continue;
										   }

										  if(d_OK != usResult)
											  return usResult;

										  if (d_OK == usResult)
										  {
										  		inReEnterPin = 0;
										  		vdDebug_LogPrintf("end enter PIN ");
										  	return usResult;
										  }

									  }
									
								}


						        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
						        if(NULL == ptr)
						        {
						            usResult = d_NO;
						        }
						        else
						        {
						            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
						        }

								return usResult;
						    }
							else
							{
								return usResult;
							}
						  }
					  }

			}			

			
		}
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = d_NO;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
			vdDebug_LogPrintf("ptr[%d]",usResult);
        }

		ustmpResult=0;
		//full status
		usCTOSS_GetTagLenValue(bOutBuf, usOutLen, SHARE_EMV_FULL_RESP_STATUS, &usDataLen, &ustmpResult);
        vdDebug_LogPrintf("saturn - LogTest - ustmpResult=[%d]",ustmpResult);
		if(ustmpResult==10512 || ustmpResult==10511)//for PIN TIMEOUT and PIN CANCEL
		{
			vdSetErrorMessage("ABORTED");
		}
    }

	if(fPINCancelled == VS_TRUE)
		return PIN_CANCELLED;
	
    return usResult;
}

USHORT usCTOSS_EMV_PowerOffICC()
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	memset(bOutBuf, 0x00, sizeof(bOutBuf));    
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_PowerOffICC, bInBuf, usInLen, bOutBuf, &usOutLen);
	if(d_OK == usResult)
	{
		//status
		ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
		if(NULL == ptr)
		{
			usResult = d_NO;
		}
		else
		{
			usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
		}
	}
	
	return usResult;
}


USHORT usCTOSS_EMV_PowerOnICC()
{
	BYTE bInBuf[40];
	BYTE bOutBuf[40];
	BYTE *ptr = NULL;
	USHORT usInLen = 0;
	USHORT usOutLen = 0;
	USHORT usResult;

	memset(bOutBuf, 0x00, sizeof(bOutBuf));    
	usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_PowerOnICC, bInBuf, usInLen, bOutBuf, &usOutLen);
	if(d_OK == usResult)
	{
		//status
		ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
		if(NULL == ptr)
		{
			usResult = d_NO;
		}
		else
		{
			usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
		}
	}
	
	return usResult;
}

USHORT usCTOSS_EMV_TxnCompletion(IN EMV_ONLINE_RESPONSE_DATA* pOnlineResponseData)
{
    BYTE bInBuf[1024];
    BYTE bOutBuf[512];
    BYTE bBuf[10];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
		BYTE bIntmpBuf[1024];
		USHORT ustmpInLen = 0;
		BYTE bTagStringHex[256];
		USHORT usTagStringLen = 0;

    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    //EMV_ONLINE_RESPONSE_DATA
    //bAction
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_2ND_GEN_AC_ACTION, 1, (BYTE*)&(pOnlineResponseData->bAction));

    //pAuthorizationCode
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_8A_RESP_CODE, strlen(pOnlineResponseData->pAuthorizationCode), (BYTE*)(pOnlineResponseData->pAuthorizationCode));

    //pIssuerAuthenticationData
    if(pOnlineResponseData->IssuerAuthenticationDataLen > 0)
    {
        usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_ARPC, (pOnlineResponseData->IssuerAuthenticationDataLen), (BYTE*)(pOnlineResponseData->pIssuerAuthenticationData));
    }

    //pIssuerScript
    if(pOnlineResponseData->IssuerScriptLen > 0)
    {
        usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_ISSUE_SCRIPT, (pOnlineResponseData->IssuerScriptLen), (BYTE*)(pOnlineResponseData->pIssuerScript));
    }
		memcpy(bIntmpBuf,bInBuf,usInLen);
		ustmpInLen = usInLen;

		
    usTagStringLen = strlen(GET_EMV_TAG_AFTER_2NDAC);
    wub_str_2_hex(GET_EMV_TAG_AFTER_2NDAC, bTagStringHex, usTagStringLen);

    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    //TagString
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_GET_MULTI_TAG, usTagStringLen, bTagStringHex);

		memcpy(&bIntmpBuf[ustmpInLen],bInBuf,usInLen);
		ustmpInLen += usInLen;
		vdPCIDebug_HexPrintf("TxnAppSelect",bIntmpBuf,ustmpInLen);

    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_Txn_CompletionEx, bIntmpBuf, ustmpInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = d_NO;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
        }
    }
    
    return usResult;
}


USHORT usCTOSS_EMV_DataGet(IN USHORT usTag, INOUT USHORT *pLen, OUT BYTE *pValue)
{
    BYTE bInBuf[256];
    BYTE bOutBuf[256];
    BYTE bBuf[10];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    USHORT usTagLocal;
    USHORT usDataLen = 0;

    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    //usTag
    memset(bBuf, 0x00, sizeof(bBuf));
    usTagLocal = usTag;
    memcpy(bBuf, (BYTE*)&usTagLocal, sizeof(USHORT));
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_TAG, sizeof(USHORT), bBuf);
    
    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_GetOneData, bInBuf, usInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = d_NO;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];

            //pValue
            usCTOSS_GetTagLenValue(bOutBuf, usOutLen, SHARE_EMV_TAG_VALUE, &usDataLen, pValue);
            *pLen =  usDataLen; 

            
            vdDebug_LogPrintf("usCTOSS_EMV_DataGet usResult[%d] usTagLen[%d]",usResult, usDataLen);
        }
    }

    return usResult;
}


USHORT usCTOSS_EMV_DataSet(IN USHORT usTag, IN USHORT usLen, IN BYTE *pValue)
{
    BYTE bInBuf[256];
    BYTE bOutBuf[256];
    BYTE bBuf[10];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    USHORT usTagLocal;

    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    //usTag
    memset(bBuf, 0x00, sizeof(bBuf));
    usTagLocal = usTag;
    memcpy(bBuf, (BYTE*)&usTagLocal, sizeof(USHORT));
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_TAG, sizeof(USHORT), bBuf);

    //pValue
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_TAG_VALUE, usLen, pValue);

    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_SetOneData, bInBuf, usInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = d_NO;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
        }
    }
    
    return usResult;
}

USHORT usCTOSS_EMV_MultiDataGet(IN BYTE *pTagString, INOUT USHORT *pLen, OUT BYTE *pValue)
{
    BYTE bTagStringHex[256];
    BYTE bInBuf[256];
    BYTE bOutBuf[2048];
    BYTE *ptr = NULL;
    USHORT usTagStringLen = 0;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult;
    USHORT usDataLen = 0;

    inMultiAP_Database_EMVTransferDataInit();

    usTagStringLen = strlen(pTagString);
    wub_str_2_hex(pTagString, bTagStringHex, usTagStringLen);

    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    //TagString
    usInLen += usCTOSS_PackTagLenValue(&bInBuf[usInLen], SHARE_EMV_GET_MULTI_TAG, usTagStringLen, bTagStringHex);
    
    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_GetPackageData, bInBuf, usInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = d_NO;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];

            //pValue
            inMultiAP_Database_EMVTransferDataRead(&usDataLen, pValue);
            *pLen =  usDataLen; 
        }
    }

    return usResult;
}


USHORT usCTOSS_EMV_MultiDataSet(IN USHORT usLen, IN BYTE *pValue)
{
    BYTE bInBuf[2048];
    BYTE bOutBuf[64];
    BYTE *ptr = NULL;
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
    USHORT usResult = 0;

    inDatabase_TerminalOpenDatabaseEx(DB_EMV);   
    inMultiAP_Database_EMVTransferDataInitEx();
    
    usInLen = 0;
    memset(bInBuf, 0x00, sizeof(bInBuf));
    //Tags TLV data
    inMultiAP_Database_EMVTransferDataWriteEx(usLen, pValue);
    inDatabase_TerminalCloseDatabase();
#if 0
    memset(bOutBuf, 0x00, sizeof(bOutBuf));    
    usResult = inMultiAP_RunIPCCmdTypes("com.Source.SHARLS_EMV.SHARLS_EMV", d_IPC_CMD_EMV_SetPackageData, bInBuf, usInLen, bOutBuf, &usOutLen);
    if(d_OK == usResult)
    {
        //status
        ptr = ptCTOSS_FindTagAddr(SHARE_EMV_RESP_STATU, bOutBuf, usOutLen);
        if(NULL == ptr)
        {
            usResult = d_NO;
        }
        else
        {
            usResult = ptr[SHARE_EMV_DEFINE_TAGS_LEN+SHARE_EMV_DEFINE_LEN];
        }
    }

    return usResult;
#endif		
}

USHORT OnAPPLISTEX(EMV_AID_ALLLIST *psrCandidateList,EMV_AID_LIST_STRUCT *psrAIDList)
{
    BYTE sHeaderString[17];
    char aplist[20][17];
    int i;
    BYTE bAppNum = 0;
    BYTE key;
    BYTE bHeaderAttr = 0x01+0x04, iCol = 1;
    BYTE  x = 1;
    char szHeaderString[50] = "SELECT APP";
    char szMitMenu[1024];
    int inLoop = 0;
    short shMinLen = 1;
    short shMaxLen = 20;
    BYTE Bret;
    BOOL fSort = FALSE;

    bAppNum = psrCandidateList->countAID;

    vdDebug_LogPrintf("Enter OnAPPLISTEX,bAppNum=[%d]",bAppNum);
#if 0
    if (strTCT.TaxiMode)
    {
        for(i = 0 ; i < bAppNum ; i++)
        {
            DebugAddINT("index", i);
            DebugAddHEX("AppLabel", psrCandidateList->arrAIDList[i].cAppLabel, 17);
            //DebugAddHEX("AID", pstAppListExData[i].baAID, pstAppListExData[i].baAIDLen);

            memcpy(aplist[i], psrCandidateList->arrAIDList[i].cAppLabel, 17);

            if(0 == memcmp(aplist[i], "NETS", 4))
            {
                memset(&psrCandidateList->arrAIDList[i], 0x00, sizeof(EMV_AID_LIST_STRUCT));
                psrCandidateList->countAID--;
                bAppNum = psrCandidateList->countAID;
                fSort = TRUE;
                break;
            }
        }
    }
#endif
    /*only one App left*/
    if (bAppNum == 1)
    {
        //*pbAppSelectedIndex = psrSuppAID[0].inPreIdx;
        memcpy(psrAIDList,&psrCandidateList->arrAIDList[0],sizeof(EMV_AID_LIST_STRUCT));
        //vdDebug_LogPrintf("*pbAppSelectedIndex[%d]", *pbAppSelectedIndex);
        return d_EMVAPLIB_OK;
    }

    if (fSort)
    {
        EMV_AID_LIST_STRUCT arrTempAIDList[10];

        i++;
        memset(arrTempAIDList, 0x00, sizeof(arrTempAIDList));
        memcpy(arrTempAIDList, &psrCandidateList->arrAIDList[i], (10-i)*sizeof(EMV_AID_LIST_STRUCT));
        memcpy(&psrCandidateList->arrAIDList[i-1], arrTempAIDList, (10-i)*sizeof(EMV_AID_LIST_STRUCT));
        memset(&psrCandidateList->arrAIDList[9], 0x00, sizeof(EMV_AID_LIST_STRUCT));
    }

#if 1
    memset(aplist, 0, sizeof(aplist));
    memset(szMitMenu, 0x00, sizeof(szMitMenu));
    CTOS_KBDBufFlush();//cleare key buffer

#if 0	//sample - PMPC is malaysia requirement
    if (!strTCT.fAllowVisaCUPAIDSelect && !strTCT.fAllowMasterCUPAIDSelect)
	{
		strcat((char *)szMitMenu, "PMPC");
	    strcat((char *)szMitMenu, (char *)" \n");
	}
#endif

    for(i = 0 ; i < bAppNum ; i++)
    {
        vdDebug_LogPrintf("index=[%d]",i);
        vdDebug_LogPrintf("bExcludedAID=[%d]",psrCandidateList->arrAIDList[i].bExcludedAID);
        vdDebug_LogPrintf("AppLabel=[%s]",psrCandidateList->arrAIDList[i].cAppLabel);
        vdPCIDebug_HexPrintf("AID", psrCandidateList->arrAIDList[i].baAID, psrCandidateList->arrAIDList[i].baAIDLen);
        strcat((char *)szMitMenu, psrCandidateList->arrAIDList[i].cAppLabel);
        if(i + 1 != bAppNum)
            strcat((char *)szMitMenu, (char *)" \n");
    }

    key = MenuDisplay(szHeaderString, strlen(szHeaderString), bHeaderAttr, iCol, x, szMitMenu, TRUE);

    if (key == 0xFF)
    {
        vdDisplayErrorMsg(1, 8, "WRONG INPUT!!!");
        return -1;
    }

    if(key > 0)
    {
        if(d_KBD_CANCEL == key)
            return -1;

        vdDebug_LogPrintf("key[%d]-------", key);
#if 0	//sample - PMPC is malaysia requirement
        if (!strTCT.fAllowVisaCUPAIDSelect && !strTCT.fAllowMasterCUPAIDSelect)
		{
			if (key > 1)
				memcpy(psrAIDList,&psrCandidateList->arrAIDList[key-2],sizeof(EMV_AID_LIST_STRUCT));//PMPC
		}
		else
#endif
        memcpy(psrAIDList,&psrCandidateList->arrAIDList[key-1],sizeof(EMV_AID_LIST_STRUCT));

        vdDebug_LogPrintf("bExcludedAID=[%d]",psrAIDList->bExcludedAID);
        vdDebug_LogPrintf("currentIndex=[%d]",psrAIDList->currentIndex);
        vdDebug_LogPrintf("cAppLabel=[%s]",psrAIDList->cAppLabel);
        vdPCIDebug_HexPrintf("baAID", psrAIDList->baAID, psrAIDList->baAIDLen);
    }
#endif

    return d_EMVAPLIB_OK;
}

/*Please add you own function here*/
USHORT usCTOSS_NonEMVCardProcess(void)
{
    return d_OK;
}




