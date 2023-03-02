
//#include "../Includes/POSTypedef.h"

typedef struct
{
	uint32_t PASid;
	int inRetryTime;
	uint8_t fEnable;
	uint8_t fOnGoing;
	unsigned char szSTLTime1[8];
	unsigned char szSTLTime2[8];
	unsigned char szSTLTime3[8];
	unsigned char szWaitTime[8];
	uint8_t fFirstBootUp;
	unsigned char szlastSettleDate[8+1];
} STRUCT_PAS;

STRUCT_PAS strPAS;

int inPASRead(int inSeekCnt);
int inPASSave(int inSeekCnt);
int inPASNumRecord(void);


