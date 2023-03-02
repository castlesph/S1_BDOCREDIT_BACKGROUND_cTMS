/**
 * Name: sbdf_kms2.h
 * Purpose: KMS2 Saturn 1000 5891 chip (Secure Box) Define, for user and developer.
 * Auther: Aaron Chang
 * Create: 2016.12.30
 */

#ifndef __SBDF_KMS2_H__
#define __SBDF_KMS2_H__

//#include "../ctos/typedef.h"

//#include "temp.h"

/* Key Type */
#define SBKMS2_KEYTYPE_3DES							0x01
#define SBKMS2_KEYTYPE_3DES_DUKPT					0x02 	// ANS X9.24 - 2004
#define SBKMS2_KEYTYPE_AES							0x03
#define SBKMS2_KEYTYPE_RSA							0x04
#define SBKMS2_KEYTYPE_AES_DUKPT					0x07

/* Key Attribute */
#define SBKMS2_KEYATTRIBUTE_PIN							0x00000001
#define SBKMS2_KEYATTRIBUTE_MAC							0x00000002
#define SBKMS2_KEYATTRIBUTE_ENCRYPT						0x00000004
#define SBKMS2_KEYATTRIBUTE_DECRYPT						0x00000008
#define SBKMS2_KEYATTRIBUTE_KBPK						0x00000010 //for tr31
#define SBKMS2_KEYATTRIBUTE_KBEK						0x00000020 //for kbek
#define SBKMS2_KEYATTRIBUTE_MASTERKEY					0x00000040
#define SBKMS2_KEYATTRIBUTE_SK_ENCRYPT					0x00000080 //only with mk is work, like flag
#define SBKMS2_KEYATTRIBUTE_RSA_PUBLICKEYENCRYPT		0x00000100
#define SBKMS2_KEYATTRIBUTE_RSA_PRIVATEKEYENCRYPT		0x00000200
#define SBKMS2_KEYATTRIBUTE_KPK							0x00010000//define in sbesdf_kms2.h
#define SBKMS2_KEYATTRIBUTE_INTERMEDIATE				0x00020000//define in sbesdf_kms2.h

#define SBKMS2_KEYATTRIBUTE_VALUE_NOT_UNIQUE			0x10000000

/* MAC Method */
#define SBKMS2_MACMETHOD_CBC							0x00  	// Normal CBC MAC
#define SBKMS2_MACMETHOD_X9_19							0x01	// Retail MAC, done in one call
#define SBKMS2_MACMETHOD_X9_19_START					0x02	// Start for Retail MAC. Retail MAC is done in three or more calls
#define SBKMS2_MACMETHOD_X9_19_UPDATE					0x03	// Continue for Retail MAC. Retail MAC is done in three or more calls
#define SBKMS2_MACMETHOD_X9_19_FINAL					0x04	// End for Retail MAC. Retail MAC is done in three or more call

/* data encrypt/decrypt method */
#define SBKMS2_DATAENCRYPTCIPHERMETHOD_ECB				0x00
#define SBKMS2_DATAENCRYPTCIPHERMETHOD_CBC				0x01

/* RSA encrypt method */
#define SBKMS2_RSAPUBLICKEYENCRYPT						0x00
#define SBKMS2_RSAPRIVATEKEYENCRYPT						0x01

/* Key Write Mode */
#define SBKMS2_KEYPROTECTIONMODE_PLAINTEXT				0x00

/* PIN Block Type */
#define SBKMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_0				0x00
#define SBKMS2_PINBLOCKTYPE_ANSI_X9_8_ISO_4				0x01

/* PIN Protection Cipher Method */
#define SBKMS2_PINCIPHERMETHOD_ECB						0x00
#define SBKMS2_PINCIPHERMETHOD_CBC						0x01


#define SBKMS2_HASH_ALGORITHM_SHA1			0x00
#define SBKMS2_HASH_ALGORITHM_SHA256		0x01

//Get PIN Call Back Function.------------------------------------------------------------------------
#define d_KMS2_CALLBACKFUNCTION_CALL		0xA913
#define d_JUMP_NON							0x00
#define d_JUMP_TESTCANCEL					0x01
#define d_JUMP_ONGETPINDIGIT				0x02
#define d_JUMP_ONGETPINCANCEL				0x03
#define d_JUMP_ONGETPINBACKSPACE			0x04
#define d_JUMP_ONGETPINOTHERKEYS			0x05
//Get PIN Max timeout -------------------------------------------------------------------------------
#define d_GETPIN_MAX_TIMEOUT				900

#endif
