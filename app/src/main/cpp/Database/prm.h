
//#include "../Includes/POSTypedef.h"


typedef struct
{
	int PRMid;
	int HDTid;
	char szPromoLabel[40+1];
	char szPromoCode[3+1];
	char szPrintPromoLabel[40+1];
	int inGroup;
	uint8_t fPRMEnable;
	uint8_t fInstEMVEnable;
	uint8_t fInstBINVerEnable;
}STRUCT_PRM;

STRUCT_PRM strPRM;
STRUCT_PRM strMultiPRM[20];

int inPRMReadbyHDTid(int inHDTid, int *inNumRecs);
int inPRMReadbyPRMid(int inPRMid);
int inPRMSave(int inSeekCnt);
int inPRMNumRecord(void);
int inPRMReadbyinInstGroup(int inGroup, int *inNumRecs);
int inPRMReadbyinInstGroupEx(int inGroup, int *inNumRecs);

