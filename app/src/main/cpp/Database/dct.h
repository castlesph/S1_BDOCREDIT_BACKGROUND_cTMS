#define IPP_TDES_KEY_SIZE 16
#define IPP_DISPLAY_SIZE 25
#define IPP_PIN_BLOCK 8

typedef struct
{
	int HDTid;
	char szPINKey[IPP_TDES_KEY_SIZE+1];
	char szMACKey[IPP_TDES_KEY_SIZE+1];
	int inMinPINDigit;
	int inMaxPINDigit;
	unsigned short usKeySet;
	short usKeyIndex;
	char szDisplayLine1[IPP_DISPLAY_SIZE + 1];
	int szDisplayLine2[IPP_DISPLAY_SIZE + 1];
	int szDisplayProcessing[IPP_DISPLAY_SIZE + 1];
	//gcitra
    uint8_t fDebitLogonRequired;
	//gcitra
	
	int inMMTid;
}STRUCT_DCT;

STRUCT_DCT strDCT;

int inDCTRead(int inHDTid,int inMITid);
	int inDCTSave(int inHDTid,int inMITid);
int inDCTNumRecord(void);


