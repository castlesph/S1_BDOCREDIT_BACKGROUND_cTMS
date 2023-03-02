//nclude "../Includes/POSTypedef.h"


typedef struct
{
	uint32_t 	inPARid;
	unsigned char	szPARTPDU[5+1];
	unsigned char	szPARNII[4+1];
    unsigned char	szPARTID[10];
    unsigned char 	szPARMID[20];
	unsigned char	szPARTelNum1[30];
	unsigned char	szPARTelNum2[30];
	uint8_t      fPAREnable; 
	unsigned char       szPARVersion[11];
	unsigned char       szPARSerialNumber[14];

	unsigned char szPARLastLogonDate[11];

}STRUCT_PAR;

STRUCT_PAR strPAR;

int inPARRead(int inSeekCnt);
int inPARSave(int inSeekCnt);
int inPARNumRecord(void);


