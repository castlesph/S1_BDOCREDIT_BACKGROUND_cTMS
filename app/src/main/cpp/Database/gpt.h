#include<typedef.h>

typedef struct
{
	UINT 	GPTid;
	BYTE	szGPTAPN[30] ;
	BYTE	szGPTUserName[30] ;
	BYTE	szGPTPassword[30] ;
	BYTE	szGPTPriTxnHostIP[30];
	BYTE	szGPTSecTxnHostIP[30] ;
	BYTE	szGPTPriSettlementHostIP[30];
	BYTE	szGPTSecSettlementHostIP[30];
	int	inGPTPriTxnHostPortNum;
	int	inGPTSecTxnHostPortNum;
	int	inGPTPriSettlementHostPort;
	int	inGPTSecSettlementHostPort;
	BYTE	szGPTTMSHostIP[30];
	BYTE	szGPTTLSHostIP[30];
	int	inGPTTMSHostPortNum;
	int	inGPTTLSHostPortNum;

}STRUCT_GPT;

STRUCT_GPT strGPT;

int inGPTRead(int inSeekCnt);
int inGPTSave(int inSeekCnt);
int inGPTNumRecord(void);


