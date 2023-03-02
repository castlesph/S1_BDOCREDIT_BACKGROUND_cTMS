#ifndef __CTOSS_TMS_H__
#define __CTOSS_TMS_H__

int inCTOSS_SettlementCheckTMSDownloadRequest(void);

int inCTOSS_CheckIfPendingTMSDownload(void);
int inCTOSS_TMSDownloadRequest(void);
int inCTOSS_TMSChkBatchEmptyProcess(void);
int inCTOSS_TMSChkBatchEmpty(void);
int inCTOSS_TMSDownloadRequest(void);
void inCTOSS_TMS_USBUpgrade(void);
int inCTOSS_TMSPreConfig2(int inComType);
int inCTOSS_ADLSettlementCheckTMSDownloadRequest(void);
int inCTOS_CTMSUPDATE(void);
//int inCallJAVA_CTMSUPDATE(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen);
int inCTOS_CTMS_Connect(int index);

#endif //end __CTOSS_TMS_H__
