#ifndef _DATABASEFUNC__H
#define	_DATABASEFUNC__H

#ifdef	__cplusplus
extern "C" {
#endif
#include "dct.h"

#include "prm.h" //Installment: Promo selection table header  -- jzg
#include "par.h" //POS auto report: added PAR header -- jzg
#include "pas.h" //Auto-settlement: added AST table -- jzg
#include "gpt.h" //BDO: [Select Telco Setting] -- sidumili
#include "flt.h" //Added FLT table -- jzg

//Added other database headers - start -- jzg
#include "bvt.h"
#include "clt.h"
//Added other database headers - end -- jzg


#include "../Includes/POSTypedef.h"

#include "../Includes/Trans.h"
//#include "../FileModule/myFileFunc.h"

//#include "../DataBase/DataBaseFunc.h"


//��ע�⣬��Ҫ�����ļ����Ĵ�Сд
#define DB_TERMINAL "/data/data/pub/TERMINAL.S3DB"
#define DB_BATCH	"/data/data/com.Source.S1_BDO.BDO/V5S_BDOCREDIT.S3DB"
#define DB_EMV	"/data/data/pub/EMV.S3DB"
#define DB_COM	"/data/data/pub/COM.S3DB"

#define DB_MULTIAP	"/data/data/pub/MULTIAP.S3DB"
#define DB_WAVE		"/data/data/pub/WAVE.S3DB"

#define DB_ERM	"/data/data/pub/ERM.S3DB"
#define DB_CTLS	"/data/data/pub/CTLS.S3DB"
#define DB_EFT_LIB "/data/data/pub/EFT.S3DB"

#define DB_MULTIAP_JOURNAL "/data/data/pub/MULTIAP.S3DB-journal"

#define DB_INVOICE "/data/data/pub/INVOICE.S3DB"

#define ECRISOLOG_FILE "/data/data/pub/ECRISO.LOG"

#define ISOLOG_FILE "/data/data/pub/ISO.LOG"
#define DB_EFTSEC "/data/data/pub/EFT.S3DB"

#define DB_BDOPAY	"/data/data/pub/BDOPAY.S3DB"




/* BDO CLG: Revised menu functions -- jzg */
#define FILENAME_SIZE 200



int inAIDNumRecord(void);
int inHDTNumRecord(void);
int inTCTNumRecord(void);
int inCDTNumRecord(void);
int inRDTNumRecord(void);
int inEMVNumRecord(void);
int inAIDNumRecord(void);
int inMSGNumRecord(void);
int inTLENumRecord(void);
int inIITNumRecord(void);
int inMITRead(int inSeekCnt);

int inCPTRead(int inSeekCnt);
int inCPTReadEx(int inSeekCnt);
int inCPTSave(int inSeekCnt);

int inPCTRead(int inSeekCnt);
int inPCTSave(int inSeekCnt);

int inHDTRead(int inSeekCnt);
int inHDTReadEx(int inSeekCnt);
int inHDTSave(int inSeekCnt);
int inCDTRead(int inSeekCnt);
int inCDTReadMulti(char *szPAN, int *inFindRecordNum);
int inCDTMAX(void);

int inEMVRead(int inSeekCnt);
int inAIDRead(int inSeekCnt);
int inAIDReadbyRID(int inSeekCnt, char * PRid);
int inTCTRead(int inSeekCnt);
int inTCTSave(int inSeekCnt);
int inTCPRead(int inSeekCnt);
int inTCPSave(int inSeekCnt);
int inMSGRead(int inSeekCnt);
int inTLERead(int inSeekCnt);
int inTLESave(int inSeekCnt);
int inIITRead(int inSeekCnt);
int inIITSave(int inSeekCnt);

int inCSTNumRecord(void);
int inCSTRead(int inSeekCnt);
int inCSTReadEx(int inSeekCnt);
int inCSTSave(int inSeekCnt);

int inMMTReadRecord(int inHDTid,int inMITid);
int inMMTReadRecordEx(int inHDTid,int inMITid);

int inMMTNumRecord(void);

int inMMTReadNumofRecords(int inSeekCnt,int *inFindRecordNum);

int inMMTSave(int inSeekCnt);

int inPITNumRecord(void);
int inPITRead(int inSeekCnt);
int inPITSave(int inSeekCnt);

int inWaveAIDRead(int inSeekCnt);
int inWaveAIDNumRecord(void);
int inWaveEMVNumRecord(void);
int inWaveEMVRead(int inSeekCnt);

//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated start 2 of 5
#if 0
int inMSGResponseCodeRead(char* szMsg, int inMsgIndex, int inHostIndex);
#else
int inMSGResponseCodeRead(char* szMsg, char* szMsg2, char* szMsg3, int inMsgIndex, int inHostIndex);
#endif
//aaronnino for BDOCLG ver 9.0 fix on issue #00124 Terminal display according to response codes was not updated end 2 of 5


int inDatabase_BatchDeleteHDTidMITid(void);
int inDatabase_BatchDelete(void);
int inDatabase_BatchInsert(TRANS_DATA_TABLE *transData);
int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType);
int inDatabase_BatchRead(TRANS_DATA_TABLE *transData, int inSeekCnt);
int inDatabase_BatchSave(TRANS_DATA_TABLE *transData, int inStoredType);
int inDatabase_BatchSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo);
int inDatabase_BatchCheckDuplicateInvoice(char *hexInvoiceNo);
int inBatchNumRecord(void);
int inDatabase_BatchReadByHostidAndMITid(TRANS_DATA_TABLE *transData,int inHDTid,int inMITid);
int inBatchByMerchandHost(int inNumber, int inHostIndex, int inMerchIndex, char *szBatchNo, int *inTranID);
int inDatabase_BatchReadByTransId(TRANS_DATA_TABLE *transData, int inTransDataid);


int inHDTReadHostName(char szHostName[][100], int inCPTID[]);

int inMultiAP_Database_BatchRead(TRANS_DATA_TABLE *transData);
int inMultiAP_Database_BatchUpdate(TRANS_DATA_TABLE *transData);
int inMultiAP_Database_BatchDelete(void);
int inMultiAP_Database_BatchInsert(TRANS_DATA_TABLE *transData);

int inMultiAP_Database_EMVTransferDataInit(void);
int inMultiAP_Database_EMVTransferDataWrite(USHORT usDataLen, BYTE *bEMVData);
int inMultiAP_Database_EMVTransferDataRead(USHORT *usDataLen, BYTE *bEMVData);

int inMultiAP_Database_COM_Read(void);
int inMultiAP_Database_COM_Save(void);
int inMultiAP_Database_COM_Clear(void);

//1027
int inCRCRead(int inSeekCnt);
//1027
int inMMTReadRecord_SettleAll(int inHDTid,int inMITid);

int inMMTReadRecord_Footer(void);

/*BDO PHASE 2: [Function to get the total record of CPT table, use for "Change Comm"] -- sidumili */ 
int inCPTNumRecord(void);
void vdSetJournalModeOff(void);

/* BDO: Removed CARD VER from batch review - start -- jzg */
int inBatchReviewNumRecord(void);
int inBatchReviewByMerchandHost(int inNumber, int inHostIndex, int inMerchIndex, char *szBatchNo, int *inTranID);
/* BDO: Removed CARD VER from batch review - end -- jzg */

/* BDO CLG: Revised menu functions -- jzg */
int inReadDynamicMenu(int inIndex);


/* BDO: Make sure we use the BDO Credit host details first -- jzg */
int inGetCreditHostIndex(void);
int inGetCreditHostIndexEx(void);

int inNMTReadNumofRecords(int *inFindRecordNum);

int inMustSettleNumRecord(void);
int inCheckHostEnable_Per_APPLICATION(char *szAPPNAME);

int inSetOfflineLabel(char *szButtonItemLabel);
int inNMTReadRecord(int NMTID);

int inBatchNumALLRecord(void);
int inDatabase_InvoiceNumInsert(TRANS_DATA_TABLE *transData);
int inDatabase_InvoiceNumSearch(TRANS_DATA_TABLE *transData, char *hexInvoiceNo);
int inDatabase_InvoiceNumDelete(int HDTid, int MITid);
int inCheckBatchEmtpy(void);

int inHDTSaveEDCSETTTING(int inSeekCnt);

int inInstallmentCDTReadMulti(char *szPAN, int *inFindRecordNum);
int inInstallmentCDTRead(int inSeekCnt);
int inHDTReadData(int inSeekCnt);
int inHDTReadDataEx(int inSeekCnt);
int inSetTipAllowedLabel(char *szButtonItemLabel);
int inDatabase_IPReportInsert(void);

int inFXTRead(int inSeekCnt);
int inCSTReadHostID(char *szCurCode);

int inMMTReadHostName(char szHostName[][400], int inCPTID[], int inMITid, int inIndicator);
int inSetPriSecConnection(char *szPriTxnHostIP, char *szSecTxnHostIP, int inPriPort, int inSecPort);

int inMMTResetReprintSettle(int inMITid);
int inMMTSetReprintSettle(int inMMTid);

int inMMTNumRecordwithBatch(int inMITid, int inHDTid[]);
int inSetMMTSettleStatusEmpty(int inMITid);
int inHDTReadOrderBySequence(int inHDTid[]);
int inMMTNumRecordwithSettleStatusSuccess(int inMITid, int inHDTid[]);
int inHDTReadHostID(char *szHostName, int inHDTid[]);

/*albert - start - 20161202 - Reprint of Detail Report for Last Settlement Report*/
int inDatabase_BackupDetailReport(int HDTid, int MITid);
int inDatabase_DeleteDetailReport(int HDTid, int MITid);
int inBatchByDetailReport(int inNumber, int inHostIndex, int inMerchIndex, int *inTranID);
int inDatabase_ReadDetailReport(TRANS_DATA_TABLE *transData, int inTransDataid);
int inBackupDetailReportNumRecord(void);
/*albert - end - 20161202 - Reprint of Detail Report for Last Settlement Report*/

int inEFTPubRead(int inSeekCnt);
int inEFTPubSave(int inSeekCnt);
int inHDTReadinSequence(int inSeekCnt);
int inHDTDCCSave(int inSeekCnt);
void vdIncDCCSTAN(TRANS_DATA_TABLE *srTransPara);
int inCSTReadData(int inSeekCnt);
int inDatabase_BatchDeleteTransType(BYTE *byTransType);
int inDCCMMTUpdate(void);
int inDatabase_inSMACFooterRead(char *szBuff, TRANS_DATA_TABLE *transData);
int inDatabase_inSMACFooterSave(char *szBuff, TRANS_DATA_TABLE *transData);
int inDatabase_SMACFooterDeleteAll(void);
int inDatabase_SMACFooterDeletebyTraceNum(TRANS_DATA_TABLE *transData);
int inDatabase_SMACFooterDelete(int HDTid, int MITid);

int inDatabase_TerminalOpenDatabase(void);
int inDatabase_TerminalCloseDatabase(void);
int inDatabase_TerminalOpenDatabaseEx(const char *szDBName);
int inCDTReadEx(int inSeekCnt);
int inIITReadEx(int inSeekCnt);
int inInstallmentCDTReadEx(int inSeekCnt);
int inCSTReadHostIDEx(char *szCurCode);
int inMMTSaveEx(int inSeekCnt);
int inTCTReadEx(int inSeekCnt);
int inPCTReadEx(int inSeekCnt);
int inTCPReadEx(int inSeekCnt);

int inMultiAP_Database_EMVTransferDataInitEx(void);
int inMultiAP_Database_EMVTransferDataWriteEx(USHORT usDataLen, BYTE *bEMVData);
int inMultiAP_Database_EMVTransferDataReadEx(USHORT *usDataLen, BYTE *bEMVData);

int inIITSaveEx(int inSeekCnt);
int inMultiAP_Database_BatchDeleteEx(void);
int inMultiAP_Database_EMVTransferDataInitEx(void);
int inMultiAP_Database_BatchInsertEx(TRANS_DATA_TABLE *transData);
int inHDTReadinSequenceEx(int inSeekCnt);
int inHDTSaveEx(int inSeekCnt);
int inCPT_Update(int inCommMode, int inIPHeader);
int inFLGGet(char *szFlag);
int inFLGRead(int inSeekCnt);
int inFLGSet(char *szFlag, BOOL fValue);
int inCheckHostBatchEmtpy(int inHDTid,int inMITid);
int inTCTReadLastInvoiceNo(int inSeekCnt);
int inReadDMTrx(int inMenu);
int inHDTReadQRMenu(char *szHostName, int inHDTid[]);
int inTMSEXRead(int inSeekCnt);
int inTMSEXSave(int inSeekCnt);

int inUpdateCommsMode(int inCommMode, int inIPHeader);

int inCheckAllBatchEmtpy(void);
int inPHQRRead(int inSeekCnt);

int inCPT_IPUpdate(char *szPriTxnHostIP, char *szSecTxnHostIP, char *szPriSettlementHostIP, char *szSecSettlementHostIP);
int inCPT_PortUpdate(int inPriTxnHostPortNum, int inSecTxnHostPortNum, int inPriSettlementHostPort, int inSecSettlementHostPort);

int inMMTReadPreAuthHostName(char szHostName[][400], int inCPTID[], int inMITid, int inIndicator);

int inBatchPreAuthNumRecord(void);

int inHDTUpdate(int inSeekCnt);

int inIITReadIssuerName(char szHostName[][100], int inCPTID[]);

int inSetMobileWalletLabel(char *szButtonItemLabel);
int inIMGRead(void);

int inPHQRIPUpdate(int inSeekCnt, char *szPriTxnHostIP, char *szSecTxnHostIP);
int inPHQRPortUpdate(int inSeekCnt, int inPriTxnHostPortNum, int inSecTxnHostPortNum);

int inBDOPayRead(int inSeekCnt);
int inBDOPayIPUpdate(int inSeekCnt, char *szPriTxnHostIP);
int inGetMenuID(int inSeekCnt);
int inGetDynamicMenuDetail(char* szButtonItemLabel);
char* getHostLabel(int inSeekCnt, char* szHostLabel);
int getHostIndex(char* szHostLabel);
int inReadIMGMenu(int pTxnTpe);
char* getImageFileName(char* szHostLabel, char* szFileName);
int getCTLSMode(int inSeekCnt);

int inMMTReadBDOHost(int inHDTid,int inMITid);
int inMMTBDOMustSettleSave(int inSeekCnt);
int inDatabase_OptOutbyTransTypeBatchUpdate(BYTE byTransType, char *hexInvoiceNo);
int inDatabase_OptOutCompleteBatchUpdate(char *hexInvoiceNo);

#endif	/* _DATABASEFUNC__H */

