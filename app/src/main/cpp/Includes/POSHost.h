
#ifndef ___POS_HOST___
#define ___POS_HOST___




int inCTOS_GetTxnBaseAmount(void);
//int inCTOS_GetTxnTipAmount(void);  // code cleanup
int inCTOS_UpdateTxnTotalAmount(void);
int inCTOS_GetInvoice(void);
int inCTOS_CustComputeAndDispTotal(void);

int inCTOS_DisplayResponse(TRANS_DATA_TABLE *srTransPara);
int inCTOS_BatchSearch(void);

int inCTOS_GeneralGetInvoice(void);
int inCTOS_CheckTipadjust(void);
int inCTOS_CheckVOID(void);
VS_BOOL fAmountLessThanFloorLimit(void) ;


int inCTOS_VoidSelectHost(void);
int inCTOS_ChkBatchEmpty(void);
int inCTOS_ConfirmInvAmt(void);
int inCTOS_GetTipAfjustAmount(void);
int inCTOS_GetOffApproveNO(void);
int inCTOS_SettlementSelectAndLoadHost(void);
int inCTOS_LoadCDTandIIT(void);
int inCTOS_DisplayBatchRecordDetail(int inType, int inNavRecType);
int inCTOS_SettlementClearBathAndAccum(BOOL fManualSettlement);

int inCTOS_DisplayBatchTotal(BOOL fConfirm);

int inCTOSS_DeleteAdviceByINV(BYTE *szInvoiceNo);
int inCTOSS_BatchCheckDuplicateInvoice(void);

extern void vdCTOSS_DisplayAmount(USHORT usX,USHORT usY,char * szCurSymbol,char * szAmount);

//1022
int inCTOS_INSTGetTxnBaseAmount(void);
//1022

int inCTOS_VoidSelectHostNoPreConnect(void); 


int inComputePercent (long lnPercent, char *szCalcAmt, long lnBaseAmt, int inFractionalSize);
int inCTOS_ChkBatchEmpty2();
BYTE InputStringAlpha2(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS);
BYTE InputString2(USHORT usX, USHORT usY, BYTE bInputMode,  BYTE bShowAttr, BYTE *pbaStr, USHORT *usStrLen, USHORT usMinLen, USHORT usTimeOutMS);

// sidumili: added function
int inCTOS_ValidateTrxnAmount(void);

int inNSRFlag(void);
int inCTOSS_CheckNSR(int flag);
extern unsigned long wub_str_2_doble(unsigned char *str);
int inCTOS_GetTxnBaseAmountWithMenu(void);
int inCTOS_ChkBatchEmpty_AllHosts(void);

int inCTOS_ChkBatchNotEmpty();

int inCTOS_ChkifPreAuthExists(void);
int inCTOS_DisplaySettleBatchTotal(BYTE byTransType, BOOL fConfirm);
int inCTOS_ChkSettleBatchEmpty();

int vdCTOS_Ping(void);
int vdCTOS_TelNet(void);

#endif //end ___POS_HOST___

