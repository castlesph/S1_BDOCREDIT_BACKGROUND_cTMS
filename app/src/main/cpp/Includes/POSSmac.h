
#ifndef ___POS_SMAC___
#define ___POS_SMAC___

int inCTOS_SMACLOGON(void);
int inCTOS_SMACLogonFlowProcess(int inSelectMerchant);
int inCTOS_SelectSMACHost(void);
int inCTOS_SMACAward(void);
int inSMACSelectTender(void);
char* szCalculatePoints(void);
int inCTOS_SMACRedeemFlowProcess(void);
int inCTOS_SMACRedeem(void);
void vdTrimLeadSpaces(char* pchString);
int InSMACDisplayBalance(void);
int inCTOS_SMACBalanceInq(void);
int inCTOS_SMACBalanceInquiryFlowProcess(void);
int inCheckLogonResponse(void);
int inCheckNewSMACRateFromSettlement(void);
int inCheckSMAC(void);
int CheckIfSmacEnable(void);
int CheckIfSmacCard(void);
int CheckIfSmacAlreadyLogOn(void);
void vdUpdateSMACAmount(void); /*BDO: For SMAC which during void DE04 is the balance amount -- sidumili*/

int inCheckIfSMCardCDT(void);
int inCheckIfSMCardInq(void);
int inCheckIfSMCardTransPara(TRANS_DATA_TABLE *srTransPara);
int inCheckIfSMCardTransRec(void);
int CheckIfSmacEnableonIdleSale(void);
int inSMACAnalyzeField63(void);

#endif //end ___POS_SALE___
;
