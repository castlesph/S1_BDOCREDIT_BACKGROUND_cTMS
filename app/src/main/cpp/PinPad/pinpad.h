
#ifndef ___PINPAD_H___
#define	___PINPAD_H___

#ifdef	__cplusplus
extern "C" {
#endif

    int inInitializePinPad(void);
    void TEST_Write3DES_Plaintext(void);
    void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine);
    //void inCTOS_DisplayCurrencyAmount(BYTE *szAmount, int inLine, int inDisplayBalancePos);  //existing code for V3 version12
    void OnGetPINDigit(BYTE NoDigits);
    void OnGetPINCancel(void);
    void OnGetPINBackspace(BYTE NoDigits);
    int inGetIPPPin(void);
    int inIPPGetMAC(BYTE *szDataIn, int inLengthIn, BYTE *szInitialVector, BYTE *szMAC);
    int inCalculateMAC(BYTE *szDataIn, int inLengthIn, BYTE *szMAC);
	int inCheckKeys(USHORT ushKeySet, USHORT ushKeyIndex);

	int inCTOS_KMS2PINGetExDukpt(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, USHORT pinBypassAllow);
    USHORT inCTOS_KMS2PINGetEx3Des(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, USHORT pinBypassAllow);
	int CTOSS_Load_3DES_TMK_PlaintextNewPAN(char *szKeyData);

    USHORT usCheckSMACKeys(USHORT ushKeySet, USHORT ushKeyIndex);

	// define for TMK keyset / index
	#define EFTSEC_KEYSET 		0xC000
	#define EFTSEC_KEYINDEX 	0x0004

	#define DEBIT_KEYSET 		0xC300
	#define DEBIT_KEYINDEX 		0x0001

	#define CUP_KEYSET 			0xC100
	#define CUP_KEYINDEX 		0x0001

	#define CUPUSD_KEYSET 		0xC200
	#define CUPUSD_KEYINDEX 	0x0001

	#define DEBIT2_KEYSET 		0x0001
	#define DEBIT2_KEYINDEX 	0x0001
	
#ifdef	__cplusplus
}
#endif

#endif	/* ___PINPAD_H___ */

