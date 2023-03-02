//
// Created by patri on 6/3/2018.
//

#include "patrick-lib.h"
#include <sqlite3.h>
#include "Database/DatabaseFunc.h"
#include "Debug/debug.h"
#include "Aptrans/MultiAptrans.h"
#include "Ctls/POSWave.h"
#include "Includes/POSSale.h"
#include "Ctls/POSCtls.h"
#include "Includes/POSVoid.h"
#include "Includes/POSSettlement.h"
#include "Ui/Display.h"
#include "Includes/POSAuth.h"
#include "Includes/POSInstallment.h"
#include "Includes/POSOffline.h"
#include "Includes/POSTipAdjust.h"
#include "Print/Print.h"
#include "Includes/POSTrans.h"
#include "Includes/CfgExpress.h"
#include "Includes/POSSetting.h"
#include "Includes/POSBalanceInq.h"
#include "Functionslist/POSFunctionsList.h"
#include "Includes/wub_lib.h"
#include "Includes/POSMain.h"
#include "UIapi.h"
#include <ctosapi.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <dirent.h>
#include <vegaapi.h>
#include <android_jni_log.h>
#include <KMS2Client.h>
#include "../TMS/TMS.h"

sqlite3 * db;
sqlite3_stmt *stmt;

extern JavaVM *jvm;
extern jclass activityClass;
extern jobject activityObj;
extern JNIEnv *g_env;


JavaVM *g_JavaVM;
extern jobject g_callback_obj_ctos;
extern jmethodID  g_callback_mid_bring2Front;

extern BOOL fEntryCardfromIDLE;
extern BOOL fIdleInsert;
extern BOOL fIdleSwipe;

jmethodID g_callback_mid_lcd;
jmethodID  g_callback_mid_clear;
jmethodID  g_callback_mid_aligned;

jobject g_callback_obj_input;
jmethodID  g_callback_mid_showkeypad;
jmethodID  g_callback_mid_keypadmsg;
jmethodID  g_callback_mid_keypaddone;
jmethodID  g_callback_mid_InputAmountEx;

JavaVM *g_vm;

int ing_KeyPressed = 0;
int printcopies_cntr = 0;

//settlement host
int inSettlementHost = 0;

extern VS_BOOL fPreConnectEx;

//ecr
jclass ecrClass;
jobject ecrObj;
JavaVM *ecrjvm;

JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1InitWaveData(JNIEnv *env, jobject instance) {
    // TODO

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;
	int inRet;

    //inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    //inTCTSave(1);
    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    vdDebug_LogPrintf("saturn initwavedata vdCTOS_InitWaveData");

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);


    vdDebug_LogPrintf("saturn inCTOSS_ProcessCfgExpress");
    inCTOSS_ProcessCfgExpress();
	
	// debug only to be remove
	inTCTRead(1);
	vdDebug_LogPrintf("InitWaveData->strTCT.szSuperPW=[%s], strTCT.szSystemPW=[%s], strTCT.szEngineerPW=[%s], strTCT.szFunKeyPW=[%s], strTCT.szPMpassword=[%s], strTCT.szBInRoutePW=[%s]", strTCT.szSuperPW, strTCT.szSystemPW, strTCT.szEngineerPW, strTCT.szFunKeyPW, strTCT.szPMpassword, strTCT.szBInRoutePW);

    //TINE -- added as per "Way to migrate..." document item 19)
    vdCTOS_InitWaveData();


    //inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.S1_BDOINST.BDOINST", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);
    //inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.S1_CLGDEBIT.CLGDEBIT", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);
    //inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_EMV.SHARLS_EMV", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);
    //inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_COM.SHARLS_COM", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);

	vdDebug_LogPrintf("strTCT.fFirstInit=[%d]",strTCT.fFirstInit);
    srTransRec.byEntryMode = 0;
	if(strTCT.fFirstInit == 1)
	{
		strTCT.fFirstInit = 0;
		inTCTSave(1);
	}

	vdSetECRTransactionFlg(0);
    put_env_int("ECRPROCESS",0);
	put_env_int("APP_STARTED", 0);
	
    //release memory
    if (cls != NULL)
        (*env)->DeleteLocalRef(env, cls);
    return 0;
}

JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Main(JNIEnv *env, jobject instance) {
	// TODO

	int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

	inTCTRead(1);
	//strTCT.byRS232DebugPort = 8;
	inTCTSave(1);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

	jint rs = (*env)->GetJavaVM(env, &jvm);

	jclass cls = (*env)->GetObjectClass(env, instance);
	activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
	activityObj = (*env)->NewGlobalRef(env, instance);
	
	//vdCTOS_InitWaveData();//GLADTEST

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

int ePad_SignatureCaptureLibEex(DISPLAY_REC *szDisplayRec)
{
    unsigned char uszBuffer[100+1];
    int inRet = 0;

    vdDebug_LogPrintf("=====ePad_SignatureCaptureLibEex=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

	if (strlen(szDisplayRec)>0)
        strcpy(uszBuffer, szDisplayRec);

    //jstring jstr = (*env)->NewStringUTF(env, "NA");
    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "ePad_SignatureCaptureLibEex_Java", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "ePad_SignatureCaptureLibEex_Java");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
    {
        vdDebug_LogPrintf("get nothing...");
    }

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end ePad_SignatureCaptureLibEex");

    //if(0 == strcmp(uszBuffer, "OK"))
    //    return d_OK;
    //else
    //    return d_NO;

	return d_OK;
}

int inCallJAVA_usCARDENTRY2(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[524+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_usCARDENTRY2=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    //else
    //pbDispMsg = "Tine: UserConfirm activity";

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", "This this Pass in string data to Java");

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "usCARDENTRY2_Java", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "usCARDENTRY2_Java");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)

    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);
    //(*jvm)->DetachCurrentThread(jvm);

    vdDebug_LogPrintf("end inCallJAVA_usCARDENTRY2");
    return d_OK;
}

#if 0
JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_xxx1Sale(JNIEnv *env, jobject instance) {
    // TODO

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    DISPLAY_REC szDisplayRec;

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    ePad_SignatureCaptureLibEex(&szDisplayRec);
    ushCTOS_ePadPrintSignature();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 0;
    /*Here do test init, load TCT PRM*/
    strTCT.fShareComEnable = 1;
    strTCT.byERMMode  = 0;

    inRetVal = inTCTSave(1);
    vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);


//    jint rs = (*env)->GetJavaVM(env, &jvm);

//    jclass cls = (*env)->GetObjectClass(env, instance);
//    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
//    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOSS_Sale.............");
    inCTOS_WAVE_SALE();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}
#endif

JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Sale(JNIEnv *env, jobject instance, bool isEcr) {
    // TODO

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

	vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Sale--");
	
    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d], isEcr=[%d]", inRetVal, strTCT.fDemo, isEcr);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 0;
    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;
    //strTCT.byERMMode  = 0;
	fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR
	
    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOSS_Sale.............");

	if(isEcr == TRUE)
    {
		fEntryCardfromIDLE=VS_FALSE;
		vdSetECRTransactionFlg(VS_TRUE);
    }
	
    inCTOS_WAVE_SALE();
	//inCallJAVA_GetMenu();
	
    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Void(JNIEnv *env, jobject instance) {
    // TODO

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

	//vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Void--");
	
    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 0;
    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;	
	fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);


    vdDebug_LogPrintf("inCTOS_VOID.............");
    inCTOS_VOID();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Sale1(JNIEnv *env, jobject instance) {
    // TODO

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

	//vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Sale1--");
	
    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 1;
    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;
	
	fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    inCTOS_SALE();
    return 0;
}

JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Settle(JNIEnv *env, jobject instance) {
    // TODO

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

	//vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Settle--");
	
    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;
    //strTCT.fDemo = 0;

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

	vdDebug_LogPrintf("before ctos settlement");
    inCTOS_SETTLEMENT();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}



JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inDisplayMsg1(JNIEnv *env, jobject instance) {


    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];

    inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    inTCTSave(1);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inDisplayMsg");
    inDisplayMsg1();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);


    return 0;
}

JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inDisplayImage(JNIEnv *env, jobject instance) {


    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];

    inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    inTCTSave(1);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inDisplayImage");
    inDisplayImage();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);


    return 0;
}


JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inDisplayMsg(JNIEnv *env, jobject instance) {


    inDisplayMsg1();
    return 0;
}

int inCallJAVA_DisplayMultipleMessage(char *pszMsg)
{
    unsigned char uszBuffer[100+1];
    int inRet = 0;

    vdDebug_LogPrintf("=====inCallJAVA_DisplayMultipleMessage=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("jstring[%s]", "This comes from inCallJAVA_DisplayMultipleMessage.");

    jstring jstr = (*env)->NewStringUTF(env, pszMsg);
    vdDebug_LogPrintf("jstring[%s][%s]", "This this Pass in string data to Java", pszMsg);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "displayMultipleMsg", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "displayMultipleMsg");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        vdDebug_LogPrintf("strcpy");

        memset(uszBuffer, 0x00, sizeof(uszBuffer));
        strcpy(uszBuffer, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
    {
        vdDebug_LogPrintf("get nothing...");
    }

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_DisplayMultipleMessage");

    if(0 == strcmp(uszBuffer, "OK"))
        return d_OK;
    else
        return d_NO;
}


int inCallJAVA_DisplayImage(int x, int y, char *pszMsg_Img)
{
    unsigned char uszBuffer[100+1];
    int inRet = 0;

    vdDebug_LogPrintf("=====inCallJAVA_DisplayImage=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("jstring[%s]", "This comes from inCallJAVA_DisplayImage.");

    jstring jstr = (*env)->NewStringUTF(env, pszMsg_Img);
    vdDebug_LogPrintf("jstring[%s][%s]", "This this Pass in string data to Java", pszMsg_Img);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "displayImage", "(IILjava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "inCallJAVA_DisplayImage");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, x, y, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        vdDebug_LogPrintf("strcpy");

        memset(uszBuffer, 0x00, sizeof(uszBuffer));
        strcpy(uszBuffer, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
    {
        vdDebug_LogPrintf("get nothing...");
    }

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_DisplayImage");

    if(0 == strcmp(uszBuffer, "OK"))
        return d_OK;
    else
        return d_NO;
}

int inCallJAVA_GetAmountString(BYTE *pbDispMsg, BYTE *pbAmtStr, BYTE *pbAmtLen)
{
    unsigned char uszBuffer[528+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_GetAmountString=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
    	strcpy(uszBuffer, " ");

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", "This this Pass in string data to Java");

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetAmountString", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "GetAmountString");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbAmtLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbAmtStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbAmtLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_GetAmountString");
    return d_OK;
}


int inCallJAVA_inCTOSS_WaveGetCardFields(BYTE *pbDispMsg, BYTE *pbPanStr, BYTE *pbPanLen)
{
    unsigned char uszBuffer[1024+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_inCTOSS_WaveGetCardFields=====");


    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    //else
    //pbDispMsg = "Tine: UserConfirm activity";

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", "This this Pass in string data to Java");

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetPanString", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "GetPanString");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);


    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbPanLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbPanStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbPanLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_inCTOSS_WaveGetCardFields");
    return d_OK;
}


int inCallJAVA_UserConfirmMenu(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[d_MAX_IPC_BUFFER];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_UserConfirmMenu=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
    //else
		//pbDispMsg = "Tine: UserConfirm activity";
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserConfirmMenu", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserConfirmMenu");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserConfirmMenu");
    return d_OK;
}

int inCallJAVA_ConfirmYesNo(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[250+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_ConfirmYesNo=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
    //else
		//pbDispMsg = "Tine: UserConfirm activity";
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "ConfirmYesNo", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "ConfirmYesNo");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_ConfirmYesNo");
    return d_OK;
}

int inCallJAVA_BatchReviewUI(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[250+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_BatchReviewUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "BatchReviewUI", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "BatchReviewUI");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_BatchReviewUI");
    return d_OK;
}


int inCallJAVA_UserConfirmOKMenu(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[d_MAX_IPC_BUFFER];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_UserConfirmOKMenu=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    //else
    //pbDispMsg = "Tine: UserConfirm activity";

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserConfirmOKMenu", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserConfirmOKMenu");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserConfirmOKMenu");
    return d_OK;
}

int inCallJAVA_UserConfirmCard(BYTE *pbCardDisplay, BYTE *pbOutBuf, BYTE *pbLen)
{
    unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_UserConfirmCard=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbCardDisplay)>0)
        strcpy(uszBuffer, pbCardDisplay);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserConfirmCard", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserConfirmCard");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    vdDebug_LogPrintf("jstring[%s]", "CallObjectMethod");
    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutBuf, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserConfirmCard");
    return d_OK;
}

int inCallJAVA_UserCancelUI(BYTE *pbCancelDisplay, BYTE *pbOutBuf, BYTE *pbLen)
{
    unsigned char uszBuffer[140+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_UserCancelUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbCancelDisplay)>0)
        strcpy(uszBuffer, pbCancelDisplay);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserCancelUI", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserCancelUI");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutBuf, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserCancelUI");
    return d_OK;
}


int inCallJAVA_UserInputString(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[528+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_UserInputString=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("pbDispMsg[%s]", pbDispMsg);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
        strcpy(uszBuffer, "PASS IN MSG");

	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserInputString", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserInputString");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserInputString");
    return d_OK;
}

int inCallJAVA_S1InputString(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[1024+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_S1InputString=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("pbDispMsg[%s]", pbDispMsg);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
        strcpy(uszBuffer, "PASS IN MSG");


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "InputStringUI", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "InputStringUI");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_S1InputString");
    return d_OK;
}

int inCallJAVA_S1InputStringAlpha(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("saturn =====inCallJAVA_S1InputStringAlpha=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("saturn pbDispMsg[%s]", pbDispMsg);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
        strcpy(uszBuffer, "PASS IN MSG");


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "InputStringAlpha", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("saturn jstring[%s]", "InputStringAlpha");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_S1InputStringAlpha");
    return d_OK;
}


int inCallJAVA_EnterAnyNum(char *pbNumLen, char *pbaNum)
{
    unsigned char uszBuffer[100+1];
    int inRet = 0;

    char baPINBlk[8];

    vdDebug_LogPrintf("=====inCallJAVA_EnterAnyNum=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("jstring[%s]", "This comes from CallJavaForNumString.");

    jstring jstr = (*env)->NewStringUTF(env, "This this Pass in string data to Java");
    vdDebug_LogPrintf("jstring[%s]", "This this Pass in string data to Java");

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "getAnyNumStr", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "getAnyNumStr");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbNumLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbaNum, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);


        vdDebug_LogPrintf("Copy data back");
        if (0 == strcmp(str, "BYPASS"))
            *pbNumLen = 0;
    }
    else
        *pbNumLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_EnterAnyNum");
    return d_OK;
}

int inCallJAVA_DOptionMenuDisplay(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[100+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_DOptionMenuDisplay=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("pbDispMsg[%s]", pbDispMsg);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
        strcpy(uszBuffer, "PASS IN MSG");

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "DOptionMenuDisplay", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "DOptionMenuDisplay");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_DOptionMenuDisplay");
    return d_OK;
}

INT inCallJAVA_DPopupMenuDisplay(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[9999+1];
    int inRet = 0;

    memset(uszBuffer, 0, sizeof(uszBuffer));

    vdDebug_LogPrintf("=====inCallJAVA_DPopupMenuDisplay=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("pbDispMsg[%s]", pbDispMsg);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
        strcpy(uszBuffer, "PASS IN MSG");

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "DPopupMenuDisplay", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "DPopupMenuDisplay");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_DPopupMenuDisplay");
    return d_OK;
}

/*below is the new way to display UI*/

/*
USHORT CTOS_LCDTClearDisplay(void)
{
    JNIEnv *tenv;
    LOGD("---JNI AP CB--- CTOS_LCDTClearDisplay\n");
    if((*g_JavaVM)->AttachCurrentThread(g_JavaVM, &tenv, NULL) != JNI_OK)
    {
        // attachCurrentThread() failed.
    }
    else {
        (*tenv)->CallLongMethod(tenv, g_callback_obj_ctos, g_callback_mid_clear);
    }
    return 0;
}
*/

#if 0
USHORT CTOS_LCDTPrintXY(USHORT usX, USHORT usY, UCHAR* pbBuf)
{
    JNIEnv *tenv;
    jshort jsusX;
    jshort jsusY;

    LOGD("---JNI AP CB--- CTOS_LCDTPrintXY\n");
    if((*g_JavaVM)->AttachCurrentThread(g_JavaVM, &tenv, NULL) != JNI_OK)
    {
        // attachCurrentThread() failed.
    }
    else {
        jsusX = (jshort) usX;
        jsusY = (jshort) usY;
        jstring str = (*tenv)->NewStringUTF(tenv, (char*)pbBuf);
        (*tenv)->CallLongMethod(tenv, g_callback_obj_ctos, g_callback_mid_lcd, jsusX, jsusY, str);
	 (*tenv)->DeleteLocalRef(tenv, str);
    }



    return 0;
}
#endif

//Tine:  24Apr2019
int inCallJAVA_CTOS_LCDTPrintXY(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[100+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_CTOS_LCDTPrintXY=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "LCDDisplay", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "LCDDisplay");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_CTOS_LCDTPrintXY");
    return d_OK;
}


USHORT CTOS_LCDTPrintAligned(USHORT usY, UCHAR* pbBuf, BYTE bMode)
{
	vdDebug_LogPrintf("CTOS_LCDTPrintAligned, overload...");
	return d_OK;
	
    JNIEnv *tenv;
    jshort jsusY;
    jbyte jbbMode;

    LOGD("---JNI AP CB--- CTOS_LCDTPrintAligned\n");
    if((*g_JavaVM)->AttachCurrentThread(g_JavaVM, &tenv, NULL) != JNI_OK)
    {
        // attachCurrentThread() failed.
    }
    else {
        jsusY = (jshort) usY;
        jbbMode = (jbyte) bMode;
        jstring str = (*tenv)->NewStringUTF(tenv, (char*)pbBuf);
        (*tenv)->CallLongMethod(tenv, g_callback_obj_ctos, g_callback_mid_aligned, jsusY, str, jbbMode);
    }
    return 0;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    g_vm = vm;
    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR; // JNI version not supported.
    }

    return JNI_VERSION_1_6;
}

JNIEXPORT jlong JNICALL Java_com_Source_S1_1BDO_BDO_Main_JNI_1Lib_REG_1CB_1CTOS(
        JNIEnv *env, jobject obj, jobject instance)
{
    int Rtn = 0;

    if(g_callback_obj_ctos != NULL)
        (*env)->DeleteGlobalRef(env, g_callback_obj_ctos);

    (*env)->GetJavaVM(env, &g_JavaVM);
    g_callback_obj_ctos = (*env)->NewGlobalRef(env,instance);
    jclass clz = (*env)->GetObjectClass(env,g_callback_obj_ctos);

    if(clz == NULL)
    {
        //failed to find class
    }

    g_callback_mid_lcd = (*env)->GetMethodID(env, clz, "CTOS_LCDTPrintXY", "(SSLjava/lang/String;)J");
    g_callback_mid_aligned = (*env)->GetMethodID(env, clz, "CTOS_LCDTPrintAligned", "(SLjava/lang/String;B)J");
    g_callback_mid_clear = (*env)->GetMethodID(env, clz, "CTOS_LCDTClearDisplay", "()J");
    g_callback_mid_bring2Front = (*env)->GetMethodID(env, clz, "CTOS_bring2Front", "()J");

    //g_callback_mid_pin_result = env->GetMethodID(clz, "CTOS_PINGetResult", "([B)J");

    JavaVM *javaVM = g_vm;
    jint res = (*javaVM)->GetEnv(javaVM, (void **) &env, JNI_VERSION_1_6);
    if (res != JNI_OK) {
        res = (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);
        if (JNI_OK != res) {
            LOGE("Failed to AttachCurrentThread, ErrorCode = %d", res);

        }
    }

    if(clz != NULL)
        (*env)->DeleteLocalRef(env, clz);

    return Rtn;
}


int ShowVirtualKeypPad(OUT USHORT *pusKeyPadButtonNum, OUT BYTE *pbKeyPadButtonInfo, OUT USHORT *pusKeyPadButtonInfoLen)
{
    JNIEnv* env = NULL;

    LOGD("---JNI AP CB--- ShowVirtualKeypPad\n");

    //inCallJAVA_GetAmountString(pusKeyPadButtonNum, pbKeyPadButtonInfo, pusKeyPadButtonInfoLen);
    //return d_OK;

    if((*g_JavaVM)->AttachCurrentThread(g_JavaVM, &env, NULL) != JNI_OK)
    {
        // attachCurrentThread() failed.
    }
    else {

    }

    int buffsize = 1024;
    int i = 0;

    BYTE baInputbuffer[1024];
    BYTE baOutputbuffer[1024];

    //jbyte *by = (jbyte*)baInputbuffer;

    jbyteArray jarray = (*env)->NewByteArray(env, buffsize);

    (*env)->SetByteArrayRegion(env, jarray, 0, buffsize, (jbyte*)baInputbuffer);

    LOGE("buffsize= %d", buffsize);

    (*env)->CallLongMethod(env, g_callback_obj_input, g_callback_mid_showkeypad, jarray);

    //jsize len = env->GetArrayLength (jarray);
    //unsigned char* buf = new unsigned char[len];

    (*env)->GetByteArrayRegion (env, jarray, 0, buffsize, (jbyte*)(baOutputbuffer));

    *pusKeyPadButtonNum = baOutputbuffer[0];

    LOGE("baOutputbuffer[0]= %02X", baOutputbuffer[0]);
    LOGE("baOutputbuffer[1]= %02X", baOutputbuffer[1]);
    LOGE("baOutputbuffer[2]= %02X", baOutputbuffer[2]);

    *pusKeyPadButtonInfoLen = 0;
    *pusKeyPadButtonInfoLen += (baOutputbuffer[1]*256);
    *pusKeyPadButtonInfoLen += (baOutputbuffer[2]);

    LOGE("pusPINPadButtonNum= %d", *pusKeyPadButtonNum);
    LOGE("pusPINPadButtonInfoLen= %d", *pusKeyPadButtonInfoLen);


    //memcpy(pbKeyPadButtonInfo, &baOutputbuffer[3], *pusKeyPadButtonInfoLen);

    return 0;
}


int GetKeyPadDone(void)
{
    JNIEnv *tenv;
    jshort jsusX;
    jshort jsusY;

    LOGD("---JNI AP CB--- GetKeyPadDone\n");
    if((*g_JavaVM)->AttachCurrentThread(g_JavaVM, &tenv, NULL) != JNI_OK)
    {
        // attachCurrentThread() failed.
    }
    else {
        (*tenv)->CallLongMethod(tenv, g_callback_obj_input, g_callback_mid_keypaddone);
    }
    return 0;

}


int ShowKeyPadMsg(BYTE digitsNum, BYTE bPINType, BYTE bRemainingCounter)
{
    JNIEnv *tenv;
    jbyte digits;


    LOGD("---JNI AP CB--- ShowKeyPadMsg\n");
    if((*g_JavaVM)->AttachCurrentThread(g_JavaVM, &tenv, NULL) != JNI_OK)
    {
        // attachCurrentThread() failed.
    }
    else {
        digits = (jbyte) digitsNum;
        (*tenv)->CallLongMethod(g_callback_obj_input, g_callback_mid_keypadmsg, digits, bPINType, bRemainingCounter);
    }
    return 0;

}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Trans_AmountEntryActivity_vdCTOSS_1FormatAmountUI(
        JNIEnv *env, jobject instance, jstring szFmt_, jstring szInAmt_) {
    const char *szFmt = (*env)->GetStringUTFChars(env, szFmt_, 0);
    const char *szInAmt = (*env)->GetStringUTFChars(env, szInAmt_, 0);
    char temp[24 + 1];

    // TODO
    memset(temp, 0, sizeof(temp));
    vdCTOSS_FormatAmount(szFmt, szInAmt, temp);

    (*env)->ReleaseStringUTFChars(env, szFmt_, szFmt);
    (*env)->ReleaseStringUTFChars(env, szInAmt_, szInAmt);

    return (*env)->NewStringUTF(env, temp);
    //return env->NewStringUTF(temp);

}

int inCallJAVA_UserConfirmMenuInvandAmt(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_UserConfirmMenu=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
    //else
		//pbDispMsg = "Tine: UserConfirm activity";
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserConfirmMenuInvandAmt", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserConfirmMenuInvandAmt");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserConfirmMenu");
    return d_OK;
}



int inCallJAVA_DisplayUI(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[100+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_DisplayUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "DisplayUI", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "DisplayUI");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_DisplayUI");
    return d_OK;
}

int inCallJAVA_LCDDisplay(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[528];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_LCDDisplay=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "LCDDisplay", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_LCDDisplay");
    return d_OK;
}

int inCallJAVA_usCARDENTRY(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_usCARDENTRY=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "usCARDENTRY", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "usCARDENTRY");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_usCARDENTRY");
    return d_OK;
}

int inCallJAVA_onFailedCtlsFallback(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[128+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_onFailedCtlsFallback=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "onFailedCtlsFallback", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "onFailedCtlsFallback");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_onFailedCtlsFallback");
    return d_OK;
}


int inCallJAVA_PrintReceiptUI(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[128+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_PrintReceiptUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "ReceiptOnScreen", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "ReceiptOnScreen");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

	// Check printer status here -- sidumili
	vdDebug_LogPrintf("pbOutStr=[%s]", pbOutStr);
	if (strlen(pbOutStr) > 0)
	{
		if ((strcmp(pbOutStr, "TIME OUT") == 0) || (strcmp(pbOutStr, "CANCEL") == 0))
			return d_NO;
	}
	
	//printCheckPaperS1();
	
    vdDebug_LogPrintf("end inCallJAVA_PrintReceiptUI");

    return d_OK;
}

int inCallJAVA_PrintFirstReceiptUI(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[128+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_PrintFirstReceiptUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "FirstReceiptOnScreen", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "FirstReceiptOnScreen");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

	// Check printer status here -- sidumili
	vdDebug_LogPrintf("pbOutStr=[%s]", pbOutStr);
	if (strlen(pbOutStr) > 0)
	{
		if ((strcmp(pbOutStr, "TIME OUT") == 0) || (strcmp(pbOutStr, "CANCEL") == 0))
			return d_NO;
	}
	
	//printCheckPaperS1();
	
    vdDebug_LogPrintf("end inCallJAVA_PrintFirstReceiptUI");

    return d_OK;
}

int inCallJAVA_EliteReceiptUI(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[128+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_EliteReceiptUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "EliteReceiptOnScreen", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "EliteReceiptOnScreen");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

	// Check printer status here -- sidumili
	vdDebug_LogPrintf("pbOutStr=[%s]", pbOutStr);
	if (strlen(pbOutStr) > 0)
	{
		if ((strcmp(pbOutStr, "TIME OUT") == 0) || (strcmp(pbOutStr, "CANCEL") == 0))
			return d_NO;
	}
	
	//printCheckPaperS1();
	
    vdDebug_LogPrintf("end inCallJAVA_EliteReceiptUI");

    return d_OK;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1REPORTS(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];

    inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    inTCTSave(1);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inReportSelecion");
	DisplayStatusLine("");
    inReportSelecion();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1PREAUTH(JNIEnv *env, jobject instance) {

    // TODO

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 0;
    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);


    vdDebug_LogPrintf("inCTOS_PREAUTH.............");
    inCTOS_PREAUTH();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;

}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1INSTALLMENT(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("saturn inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 0;
    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);


    vdDebug_LogPrintf("saturn inCTOS_INSTALLMENT.............");
    inCTOS_INSTALLMENT();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;

}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1SALE_1OFFLINE(JNIEnv *env,
                                                                     jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 0;
    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);


    vdDebug_LogPrintf("inCTOS_SALE_OFFLINE.............");
    inCTOS_SALE_OFFLINE();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1TIP_1ADJUST(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 0;
    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);


    vdDebug_LogPrintf("inCTOS_TIPADJUST.............");
    inCTOS_TIPADJUST();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1REPRINT(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];

    inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    inTCTSave(1);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inReprintSelection");
    inReprintSelection();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1SETTINGS(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];

    inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    inTCTSave(1);

    if (activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if (activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inSettingsSelection");
    inSettingsSelection();

    //release memory
    if (cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Trans_GetCardOptionsActivity_inCTOSS_1WaveGetCardFields(JNIEnv *env,
                                                                                    jobject instance) {

    // TODO

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("WaveGetCardFields");
    inCTOS_WaveGetCardFields();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;

}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Trans_CardEntry_CTOSS_1MSRRead_1inJava(JNIEnv *env, jobject instance) {

    // TODO
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES_50], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES ;
    usTk2Len = TRACK_II_BYTES_50 ;
    usTk3Len = TRACK_III_BYTES ;
    int byMSR_status= d_NO;
    int shReturn= d_NO;

    vdDebug_LogPrintf("MSRRead");
    byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
    if (byMSR_status == 0) {
        CTOS_Beep();
        shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf,
                                                   usTk3Len);
    }

    if(shReturn == 0) {
        vdDebug_LogPrintf("on MSR NewStringUTF");
        return (*env)->NewStringUTF(env, "MSR_OK");
    } else {
        return (*env)->NewStringUTF(env, "MSR_NO");
    }

}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Trans_CardEntry_CTOSS_1SCStatus_1inJava(JNIEnv *env, jobject instance) {

    // TODO
    int bySC_status = d_NO;
    int shReturn= d_NO;
    CTOS_SCStatus(d_SC_USER, &bySC_status);
    if(bySC_status & d_MK_SC_PRESENT) {
        vdDebug_LogPrintf("chip entry");
        //inCTOSS_CLMCancelTransaction();
        vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
        //shReturn = inCTOS_EMVCardReadProcess();
        vdDebug_LogPrintf("on ICC NewStringUTF");
        CTOS_Beep();
        return (*env)->NewStringUTF(env, "ICC_OK");
    } else {
        return (*env)->NewStringUTF(env, "ICC_NO");
    }
}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Trans_CardEntry_usCTOSS_1CtlsV3Trans_1inJava(JNIEnv *env,
                                                                         jobject instance,
                                                                         jstring szTotalAmount_,
                                                                         jstring szOtherAmt_,
                                                                         jstring szTransType_,
                                                                         jstring szCatgCode_,
                                                                         jstring szCurrCode_) {
    const char *szTotalAmount = (*env)->GetStringUTFChars(env, szTotalAmount_, 0);
    const char *szOtherAmt = (*env)->GetStringUTFChars(env, szOtherAmt_, 0);
    const char *szTransType = (*env)->GetStringUTFChars(env, szTransType_, 0);
    const char *szCatgCode = (*env)->GetStringUTFChars(env, szCatgCode_, 0);
    const char *szCurrCode = (*env)->GetStringUTFChars(env, szCurrCode_, 0);

    // TODO
    ULONG ulAPRtn;
    int inRet2=0;
    EMVCL_RC_DATA_EX stRCDataEx;

    ulAPRtn = usCTOSS_CtlsV3Trans(szTotalAmount,szOtherAmt,szTransType,szCatgCode,szCurrCode,&stRCDataEx);
    if (ulAPRtn == d_OK) {
        vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
        inRet2 = inCTOSS_V3AnalyzeTransaction(&stRCDataEx);
        vdDebug_LogPrintf("inCTOSS_V3AnalyzeTransaction -END-, inRet2 = [%d]", inRet2);
    } else if (ulAPRtn == d_NO) {
        vdDebug_LogPrintf("ulAPRtn = d_NO");
        return (*env)->NewStringUTF(env, "WAVE_NO");
    }

    if (inRet2 != d_OK)
    {
        return (*env)->NewStringUTF(env, "CTLS_ANALYZE_ERROR");
    }

    inRet2 = inCTOS_LoadCDTIndex();
    if (d_OK != inRet2)
    {
        if(CTLS_V3_SHARECTLS != inCTOSS_GetCtlsMode() && CTLS_V3_INT_SHARECTLS != inCTOSS_GetCtlsMode())
            inCTOSS_CLMCancelTransaction();

        if(inRet2 == CTLS_EXPIRED_CARD)
            return (*env)->NewStringUTF(env, "CTLS_EXPIRED_CARD");
        else
            return (*env)->NewStringUTF(env, "USER_ABORT");
    }

    (*env)->ReleaseStringUTFChars(env, szTotalAmount_, szTotalAmount);
    (*env)->ReleaseStringUTFChars(env, szOtherAmt_, szOtherAmt);
    (*env)->ReleaseStringUTFChars(env, szTransType_, szTransType);
    (*env)->ReleaseStringUTFChars(env, szCatgCode_, szCatgCode);
    (*env)->ReleaseStringUTFChars(env, szCurrCode_, szCurrCode);

    if (inRet2 == d_OK) {
        vdCTOS_SetTransEntryMode(CARD_ENTRY_WAVE);
        return (*env)->NewStringUTF(env, "WAVE_OK");
    }
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1BUTTONCANCEL(JNIEnv *env, jobject instance) {

    // TODO
    vdDebug_LogPrintf("inCTOSS_BUTTONCANCEL.............");
    CTOS_KBDBufPut('C'); // patrick hit cancel 20190406
    ing_KeyPressed = 'C';
	vdDebug_LogPrintf("ing_KeyPressed = [%d]", ing_KeyPressed);
    return 0;

}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1CARDENTRYTIMEOUT(JNIEnv *env,
                                                                        jobject instance) {

    // TODO
    vdDebug_LogPrintf("inCTOSS_CARDENTRYTIMEOUT.............");
    CTOS_KBDBufPut('T'); // patrick hit cancel 20190406
    ing_KeyPressed = 'T';
    vdDebug_LogPrintf("ing_KeyPressed = [%d]", ing_KeyPressed);
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Balance(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("SATURN inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 0;
    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;
    //strTCT.byERMMode  = 0;
	
    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("saturn inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("saturn inCTOS_BALANCE_INQUIRY.............");
    inCTOS_BALANCE_INQUIRY();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

int inCallJAVA_GetPinUI(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[100+1];
    int inRet = 0;


    vdDebug_LogPrintf("SATURN =====inCallJAVA_GetPinUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    vdDebug_LogPrintf("SATURN jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetPinUI", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("SATURN jstring[%s]", "GetPinUI");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("SATURN end inCallJAVA_GetPinUI");
    return d_OK;
}

JNIEXPORT jbyteArray JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1GetBuffer(JNIEnv *env, jobject instance) {

	int size = 720*1280*4 + 54;
	BYTE* buffer = NULL;

	vdDebug_LogPrintf("SATURN CTOS_GETGBBuffer");
	while(1)
	{
		buffer = CTOS_GETGBBuffer();

		//vdDebug_LogPrintf("SATURN %s", buffer );
		if(buffer == NULL)
			continue;
		break;
	}
	jbyteArray array;
	array = (*env)->NewByteArray(env, size);
	(*env)->SetByteArrayRegion(env, array, 0, size, (jbyte*)buffer);
	buffer = NULL;
	return array;

}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1SETUP(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];

    //inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    //inTCTSave(1);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inSetupSelection");

	vdCTOS_SetTransType(SETUP);
	inRetVal = inCTOS_GetTxnPassword();
    if(d_OK != inRetVal)
		return inRetVal;
	
    //inRetVal = inSetupSelection();
    inRetVal = vdDisplaySetup();
    //inSettingsSelection();
    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    if(inRetVal == 3)
        return inRetVal;
    else
        return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Batch_1Total(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];

    inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    inTCTSave(1);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_BATCH_TOTAL()");
    inCTOS_BATCH_TOTAL();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;

}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Batch_1Review(JNIEnv *env,
                                                                     jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];

    inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    inTCTSave(1);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_BATCH_REVIEW()");
    inCTOS_BATCH_REVIEW();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Settle_1Selection(JNIEnv *env,
                                                                         jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;

    char cwd[1024];

    inTCTRead(1);
    //strTCT.byRS232DebugPort = 8;
    inTCTSave(1);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_Settle_Selection()");
    inCTOS_Settle_Selection();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1TerminalStartUp(JNIEnv *env,
                                                                       jobject instance) {

    // TODO
    BYTE bInBuf[40];
    BYTE bOutBuf[40];
    USHORT usInLen = 0;
    USHORT usOutLen = 0;


    char cwd[1024];
    int inRet=0;

    vdDebug_LogPrintf("saturn TerminalStartUp");

	inTCTRead(1);
	inFLGRead(1);
	
    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);
	
    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);


    vdDebug_LogPrintf("saturn terminal start up inCTOSS_ProcessCfgExpress");
	//CTOS_Delay(10000);
    inCTOSS_ProcessCfgExpress();
    //vdDebug_LogPrintf("saturn vdCTOS_InitWaveData");
    //vdCTOS_InitWaveData();

	vdDebug_LogPrintf("saturn inCTOSS_ProcessCfgExpress");

	// debug only to be remove
	//inTCTRead(1);
	//vdDebug_LogPrintf("TerminalStartUp->strTCT.szSuperPW=[%s], strTCT.szSystemPW=[%s], strTCT.szEngineerPW=[%s], strTCT.szFunKeyPW=[%s], strTCT.szPMpassword=[%s], strTCT.szBInRoutePW=[%s]", strTCT.szSuperPW, strTCT.szSystemPW, strTCT.szEngineerPW, strTCT.szFunKeyPW, strTCT.szPMpassword, strTCT.szBInRoutePW);

    inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.S1_BDOINST.BDOINST", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);
	vdDebug_LogPrintf("com.Source.S1_BDOINST.BDOINST, inRet=", inRet);
	
    inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.S1_CLGDEBIT.CLGDEBIT", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);
	vdDebug_LogPrintf("com.Source.S1_CLGDEBIT.CLGDEBIT, inRet=", inRet);
	
	inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.S1_QRPAY.QRPAY", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);
	vdDebug_LogPrintf("com.Source.S1_QRPAY.QRPAY, inRet=", inRet);
	
    inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_EMV.SHARLS_EMV", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);
	vdDebug_LogPrintf("com.Source.SHARLS_EMV.SHARLS_EMV, inRet=", inRet);
	
    inRet = inMultiAP_RunIPCCmdTypesEx("com.Source.SHARLS_COM.SHARLS_COM", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);
	vdDebug_LogPrintf("com.Source.SHARLS_COM.SHARLS_COM, inRet=", inRet);

	vdDebug_LogPrintf("TerminalStartUp, strTCT.fECR:%d", strTCT.fECR);
    if(strTCT.fECR)
    {
        if(inCallJAVA_checkAppRunning("com.persistent.app") == 0)
        {
            vdDebug_LogPrintf("1.Sharls ECR");
            inRet = inMultiAP_RunIPCCmdTypesEx("com.persistent.app", 0x99, bInBuf, usInLen, bOutBuf, &usOutLen);//Add to support ecr
            vdDebug_LogPrintf("com.persistent.app(SHARLS_ECR), inRet=", inRet);
        }
        else
        {
        	vdDebug_LogPrintf("2.Sharls ECR");
        }   
    }

    vdSetECRTransactionFlg(0);
    put_env_int("ECRPROCESS",0);
    put_env_int("APP_STARTED", 0);
	
    srTransRec.byEntryMode = 0;
    inCTLOS_Getpowrfail();//FOR TESTING
    inSettlementHost = 0;
    vdRemoveCard();//FOR TESTING

	inPrinterConfig(1);

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

	vdDebug_LogPrintf("inCTOSS_TerminalStartUp exit");
	
    return 0;

}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1MANUALENTRY(JNIEnv *env, jobject instance) {

    // TODO
    vdDebug_LogPrintf("saturn inCTOSS_MANUALENTRY............");
    CTOS_KBDBufPut('M');
    ing_KeyPressed = 'M';
    vdDebug_LogPrintf("ing_KeyPressed = [%d]", ing_KeyPressed);
    return 0;

}


int inCallJAVA_GetExpiryDate(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[1024+1];
    int inRet = 0;


    vdDebug_LogPrintf("saturn =====inCallJAVA_GetExpiryDate=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("SATURN jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("saturn pbDispMsg[%s]", pbDispMsg);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
        strcpy(uszBuffer, "PASS IN MSG");


    vdDebug_LogPrintf("saturn uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("saturn jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("saturn jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetExpiryDate", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("saturn test jstring[%s]", "GetExpiryDate");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);


    vdDebug_LogPrintf("saturn inCallJAVA_GetExpiryDate test");

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_GetExpiryDate");
    return d_OK;
}

/***Start Offline Pin***/
jobject g_callback_obj_emv;
jmethodID g_callback_mid_pinpad;
jmethodID g_callback_mid_pindone;
jmethodID g_callback_mid_pindigi;




int ShowVirtualPIN(OUT USHORT *pusPINPadButtonNum, OUT BYTE *pbPINPadButtonInfo, OUT USHORT *pusPINPadButtonInfoLen)
{
    BYTE baInputbuffer[1024];
    BYTE baOutputbuffer[1024];

    JNIEnv* env = NULL;
    if((*g_JavaVM)->AttachCurrentThread(g_JavaVM, &env, NULL) != JNI_OK)
    {
        // attachCurrentThread() failed.
    }
    else {

    }

    int buffsize = 1024;
    int i = 0;

    //jbyte *by = (jbyte*)baInputbuffer;

    jbyteArray jarray = (*env)->NewByteArray(env, buffsize);

    (*env)->SetByteArrayRegion(env, jarray, 0, buffsize, (jbyte*)baInputbuffer);

    LOGE("buffsize= %d", buffsize);

    (*env)->CallLongMethod(env, g_callback_obj_emv, g_callback_mid_pinpad, jarray);

    //jsize len = env->GetArrayLength (jarray);
    //unsigned char* buf = new unsigned char[len];

    (*env)->GetByteArrayRegion (env, jarray, 0, buffsize, (jbyte*)(baOutputbuffer));

    *pusPINPadButtonNum = baOutputbuffer[0];

    LOGE("baOutputbuffer[0]= %02X", baOutputbuffer[0]);
    LOGE("baOutputbuffer[1]= %02X", baOutputbuffer[1]);
    LOGE("baOutputbuffer[2]= %02X", baOutputbuffer[2]);

    *pusPINPadButtonInfoLen = 0;
    *pusPINPadButtonInfoLen += (baOutputbuffer[1]*256);
    *pusPINPadButtonInfoLen += (baOutputbuffer[2]);

    LOGE("pusPINPadButtonNum= %d", *pusPINPadButtonNum);
    LOGE("pusPINPadButtonInfoLen= %d", *pusPINPadButtonInfoLen);


    memcpy(pbPINPadButtonInfo, &baOutputbuffer[3], *pusPINPadButtonInfoLen);

    return 0;
}



int GetPINDone(void)
{
    JNIEnv *tenv;
    jshort jsusX;
    jshort jsusY;

    //LOGD("---JNI AP CB--- GetPINDone\n");
    if((*g_JavaVM)->AttachCurrentThread(g_JavaVM, &tenv, NULL) != JNI_OK)
    {
        // attachCurrentThread() failed.
    }
    else {
        (*tenv)->CallLongMethod(tenv,g_callback_obj_emv, g_callback_mid_pindone);
    }
    return 0;

}


int ShowPINDigit(BYTE digitsNum, BYTE bPINType, BYTE bRemainingCounter)
{
    JNIEnv *tenv;
    jbyte digits;


    //LOGD("---JNI AP CB--- ShowPINDigit\n");
    if((*g_JavaVM)->AttachCurrentThread(g_JavaVM, &tenv, NULL) != JNI_OK)
    {
        // attachCurrentThread() failed.
    }
    else {
        digits = (jbyte) digitsNum;
        (*tenv)->CallLongMethod(tenv, g_callback_obj_emv, g_callback_mid_pindigi, digits, bPINType, bRemainingCounter);
    }
    return 0;

}

JNIEXPORT jlong JNICALL
Java_com_Source_S1_1BDO_BDO_Main_JNI_1offlinepin_REG_1CB_1EMV(JNIEnv *env,jobject obj,  jobject instance) {

    // TODO
    int Rtn = 0;

    if(g_callback_obj_emv != NULL)
        (*env)->DeleteGlobalRef(env, g_callback_obj_emv);

    (*env)->GetJavaVM(env, &g_JavaVM);
    g_callback_obj_emv = (*env)->NewGlobalRef(env,instance);;
    jclass clz = (*env)->GetObjectClass(env, g_callback_obj_emv);

    LOGE("REG_1CB_1EMV");

    if(clz == NULL)
    {
        //failed to find class
    }

    LOGE("ShowVirtualPIN");
    LOGE("ShowPINDigit");
    LOGE("GetPINDone");

    g_callback_mid_pinpad = (*env)->GetMethodID(env,clz, "ShowVirtualPIN", "([B)J");
    g_callback_mid_pindigi = (*env)->GetMethodID(env, clz, "ShowPINDigit", "(BBB)J");
    g_callback_mid_pindone = (*env)->GetMethodID(env, clz, "GetPINDone", "()J");



    JavaVM *javaVM = g_vm;
    jint res = (*javaVM)->GetEnv(javaVM,(void **) &env, JNI_VERSION_1_6);
    if (res != JNI_OK) {
        res = (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);
        if (JNI_OK != res) {
            LOGE("Failed to AttachCurrentThread, ErrorCode = %d", res);

        }
    }

    if(clz != NULL)
        (*env)->DeleteLocalRef(env, clz);

    return Rtn;

}

/***End Offline Pin***/

/* Analyze a string for str1,str2 */
int SplitString(char *str, char *str1, char *str2, char c)
{
    int i,j;

    for (j=0,i=0;str[i]!='\0'&&str[i]!=c;j++,i++)
        str1[j]=str[i];
    str1[j]='\0';

    if (str[i]=='\0') str2[0]='\0';
    else
    {
        for (j=0,i++;str[i]!='\0';j++,i++)
            str2[j]=str[i];
        str2[j]='\0';
    }

    return strlen(str2);
}

int inCallJAVA_inGetPIN_With_3DESDUKPTEx(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, BYTE* szIndex, USHORT pinBypassAllow)
{
    unsigned char uszpInData[70+1];
    unsigned char PinBlockKsn[20+1];
    int inRet = 0;

    vdDebug_LogPrintf("saturn inCallJAVA_inGetPIN_With_3DESDUKPTEx Start KeySet[%d] KeyIndex[%d]", KeySet, KeyIndex);

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("saturn jint[%d] *env[%x]", rs, *env);


    memset(uszpInData, 0x00, sizeof(uszpInData));
    vdDebug_LogPrintf("saturn strlen(pInData)[%d]", strlen(pInData));


    if (strlen(pInData)>0)
        strcpy(uszpInData, pInData);

    vdDebug_LogPrintf("saturn uszpInData[%s]", uszpInData);

    jstring Inputstr = (*env)->NewStringUTF(env, uszpInData);
    vdDebug_LogPrintf("saturn jstring[%s]", uszpInData);

    vdDebug_LogPrintf("saturn jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetPIN_With_3DESDUKPTEx", "(IILjava/lang/String;I)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "GetIPPPinEx");

    jobject Result = (*env)->CallObjectMethod(env, activityObj, methodID, KeySet, KeyIndex, Inputstr, pinBypassAllow);

    jbyte* Outstr = NULL;


    if(NULL != Result)
    {
        Outstr = (*env)->GetStringUTFChars(env,(jstring) Result, NULL);
    }
    else
    {
        vdDebug_LogPrintf("Result is NULL");
    }


    if (Outstr!=NULL)
    {

        memset(uszpInData, 0x00, sizeof(uszpInData));

        vdDebug_LogPrintf("%s", Outstr);
        SplitString(Outstr, szIndex, PinBlockKsn, '*');
        vdDebug_LogPrintf("szIndex[%s]", szIndex);
        vdDebug_LogPrintf("PinBlockKsn[%s]", PinBlockKsn);
        SplitString(PinBlockKsn, szPINBlock, szKSN, '|');
        vdDebug_LogPrintf("szPINBlock[%s]", szPINBlock);
        vdDebug_LogPrintf("szKSN[%s]", szKSN);
        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, Result, Outstr);
        if(strlen(szPINBlock) == 0)
            inRet = d_NO;
        else
            inRet = d_OK;
    }
    else {
        *szPINBlock = 0;
        *szKSN = 0;
        vdDebug_LogPrintf("PINBYPASS");
        inRet = d_NO;
    }

    (*env)->DeleteLocalRef(env, Inputstr);
    (*env)->DeleteLocalRef(env, Result);

    vdDebug_LogPrintf("end inCallJAVA_inGetPIN_With_3DESDUKPTEx[%d]", inRet);
    return inRet;
}



int inCallJAVA_inGetIPPPinEx(USHORT KeySet,  USHORT KeyIndex,  BYTE* pInData, BYTE* szPINBlock, BYTE* szKSN, BYTE* szIndex, USHORT pinBypassAllow)
{
    unsigned char uszpInData[20+1];
    unsigned char PinBlockKsn[20+1];
    unsigned char szPinBlockTemp[100+1];
    int inRet = 0;

    vdDebug_LogPrintf("inCallJAVA_inGetIPPPinEx Start KeySet[%d] KeyIndex[%d] pinBypassAllow[%d]", KeySet, KeyIndex, pinBypassAllow);

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    memset(uszpInData, 0x00, sizeof(uszpInData));
    vdDebug_LogPrintf("strlen(pInData)[%d]", strlen(pInData));


    if (strlen(pInData)>0)
        strcpy(uszpInData, pInData);

    vdDebug_LogPrintf("uszpInData[%s]", uszpInData);

    jstring Inputstr = (*env)->NewStringUTF(env, uszpInData);
    vdDebug_LogPrintf("jstring[%s]", uszpInData);

    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetIPPPinEx", "(IILjava/lang/String;I)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "GetIPPPinEx");

    jobject Result = (*env)->CallObjectMethod(env, activityObj, methodID, KeySet, KeyIndex, Inputstr, pinBypassAllow);

    jbyte* Outstr = NULL;


    if(NULL != Result)
    {
        Outstr = (*env)->GetStringUTFChars(env,(jstring) Result, NULL);
    }
    else
    {
        vdDebug_LogPrintf("Result is NULL");
    }


    if (Outstr!=NULL)
    {

        memset(uszpInData, 0x00, sizeof(uszpInData));

        vdDebug_LogPrintf("%s", Outstr);
        SplitString(Outstr, szIndex, PinBlockKsn, '*');
        vdDebug_LogPrintf("szIndex[%s]", szIndex);
        vdDebug_LogPrintf("PinBlockKsn[%s]", PinBlockKsn);
        //vdDebug_LogPrintf("AAA before splitstring srTransRec.CDTid[%d], srTransRec.CDTid2[%d]", srTransRec.CDTid, srTransRec.CDTid2);
        memset(szPinBlockTemp, 0x00, sizeof(szPinBlockTemp));
        SplitString(PinBlockKsn, szPinBlockTemp, szKSN, '|');
        memcpy(szPINBlock, szPinBlockTemp, 8);
        //SplitString(PinBlockKsn, szPINBlock, szKSN, '|');
        //vdDebug_LogPrintf("AAA after splitstring srTransRec.CDTid[%d], srTransRec.CDTid2[%d]", srTransRec.CDTid, srTransRec.CDTid2);
        //srTransRec.CDTid = srTransRec.CDTid2; //AAA fix on CDTid wrong value.
        vdDebug_LogPrintf("szPINBlock[%s]", szPINBlock);
        PackEx(szPINBlock,16,srTransRec.szPINBlock);
        vdDebug_LogPrintf("szKSN[%s]", szKSN);
        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, Result, Outstr);
        if(strlen(szPINBlock) == 0)
        {
            vdDebug_LogPrintf("PINBYPASS");
            inRet = d_NO;
        }
        else
            inRet = d_OK;

    }
    else {
        *szPINBlock = 0;
        *szKSN = 0;
        inRet = d_NO;
        vdDebug_LogPrintf("Outstr is NULL");
    }

    (*env)->DeleteLocalRef(env, Inputstr);
    (*env)->DeleteLocalRef(env, Result);

    vdDebug_LogPrintf("end inCallJAVA_inGetIPPPinEx [%d]", inRet);
    return inRet;
}


JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_AppStart(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;

	put_env_int("APP_STARTED", 1);

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inRetVat = [%d]", inRetVal);
    inRetVal = inFLGRead(1);
    vdDebug_LogPrintf("inFLGRead: inRetVal = [%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    vdDebug_LogPrintf("inCTOS_APP_START");
    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("SATURN inCTOS_APP_START.............");

    inRetVal = inCTOS_APP_START();

	put_env_int("APP_STARTED", 0);
	
    vdDebug_LogPrintf("SATURN inCTOS_APP_END............. inRetVal=[%d]", inRetVal);
    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return inRetVal;

}

int inCallJAVA_usEditDatabase(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[128+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_usEditDatabase=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "usEditDatabase", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "usEditDatabase");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_usEditDatabase");
    return d_OK;
}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_CTOSS_1MSRRead_1inJava(JNIEnv *env,
                                                                     jobject instance) {

    // TODO
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES;
    usTk2Len = TRACK_II_BYTES;
    usTk3Len = TRACK_III_BYTES;

    int byMSR_status = d_NO;
    int shReturn = d_NO;

    int bySC_status = d_NO;
    DWORD dwWait = 0, dwWakeup = 0;

    vdSetFirstIdleKey(0x00);
    dwWait = d_EVENT_KBD | d_EVENT_MSR | d_EVENT_SC;
    CTOS_SystemWait(20, dwWait, &dwWakeup);
    if ((dwWakeup & d_EVENT_MSR) == d_EVENT_MSR) {
        vdDebug_LogPrintf("MSR Reading...");
    }

	//vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_CTOSS_1MSRRead_1inJava--");
	
    //vdDebug_LogPrintf("MSRRead");
    byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
    //CTOS_Delay(1000);
    if (byMSR_status == 0) {
        CTOS_Beep();
        fEntryCardfromIDLE = TRUE;
        fIdleSwipe = TRUE;
        //usCTOSS_LCDDisplay("SALE|PROCESSING...");

		#ifdef ANDROID_NEW_UI
        	//vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
		#endif
		
        shReturn = shCTOS_SetMagstripCardTrackData(szTk1Buf, usTk1Len, szTk2Buf, usTk2Len, szTk3Buf,                                                  usTk3Len);

        //vdDebug_LogPrintf("shCTOS_SetMagstripCardTrackData 2 = [%d]", shReturn);

        if (shReturn == INVALID_CARD) {
            CTOS_KBDBufFlush();
            vduiClearBelow(1);
            vdDisplayErrorMsg(1, 8, "READ CARD FAILED");
			usCTOSS_LCDDisplay(" ");
            return (*env)->NewStringUTF(env, "INVALID_CARD");
        }

        if (d_OK != inCTOS_LoadCDTIndex()) {
			vdCTOS_ResetMagstripCardData();
            CTOS_KBDBufFlush();
			usCTOSS_LCDDisplay(" ");
            return (*env)->NewStringUTF(env, "INVALID_CARD");
        }

        if (d_OK != inCTOS_CheckEMVFallback()) {
            vdCTOS_ResetMagstripCardData();
            return (*env)->NewStringUTF(env, "INSERT_ONLY");
        }

        //vdDebug_LogPrintf("on MSR NewStringUTF");
        return (*env)->NewStringUTF(env, "MSR_OK");
    }
    //else {
    //    return (*env)->NewStringUTF(env, "MSR_NO");
    //}
    
    //vdDebug_LogPrintf("ICCRead");
    CTOS_SCStatus(d_SC_USER, &bySC_status);
    //CTOS_Delay(1000);
    if (bySC_status & d_MK_SC_PRESENT) {
        CTOS_Beep();
        fEntryCardfromIDLE = TRUE;
        fIdleInsert = TRUE;
        //usCTOSS_LCDDisplay("SALE|PROCESSING...");

		#ifdef ANDROID_NEW_UI
        	//vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
		#endif
        //vdDebug_LogPrintf("chip entry");
        //inCTOSS_CLMCancelTransaction();
        vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
        //shReturn = inCTOS_EMVCardReadProcess();
        //vdDebug_LogPrintf("on ICC NewStringUTF");

        return (*env)->NewStringUTF(env, "ICC_OK");
    }
    //else {
    //    return (*env)->NewStringUTF(env, "ICC_NO");
    //}

    return (*env)->NewStringUTF(env, "MSR_ICC_NO");
}

JNIEXPORT jint 
JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Idle_1Sale(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

	//vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Idle_1Sale--");

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    //strTCT.byRS232DebugPort = 8;
    //strTCT.fDemo= 1;
    /*Here do test init, load TCT PRM*/
    //strTCT.fShareComEnable = 1;
    //strTCT.byERMMode = 0;

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

	put_env_int("APP_STARTED", 1);
    fEntryCardfromIDLE = TRUE;
    vdDebug_LogPrintf("srTransRec.byEntryMode = [%d]", srTransRec.byEntryMode);
    vdDebug_LogPrintf("srTransRec.szPAN = [%d]", strlen(srTransRec.szPAN));
    vdDebug_LogPrintf("fidleinsert = [%d]", fIdleInsert);
    vdDebug_LogPrintf("fidleswipe = [%d]", fIdleSwipe);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);


    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("SATURN inCTOSS_Sale.............");


    inCTOS_WAVE_SALE();

	put_env_int("APP_STARTED", 0);
    vdCTOS_ResetMagstripCardData();
    vdCTOS_TransEndReset();
    vdRemoveCard();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

#define d_EXTERNAL_PINPAD           0

BYTE g_bPINType;
BYTE g_bRemainingCounter;
BOOL ispinpadalone=true;


USHORT OnShowVirtualPIN(OUT USHORT *pusPINPadButtonNum, OUT BYTE *pbPINPadButtonInfo, OUT USHORT *pusPINPadButtonInfoLen)
{
	vdDebug_LogPrintf((BYTE*)"OnShowVirtualPIN() is triggered -->");
	USHORT usPINPadButtonNum;
	BYTE bPINPadButtonInfo[1024];
	USHORT usPINPadButtonInfoLen;

	ShowVirtualPIN(&usPINPadButtonNum, bPINPadButtonInfo, &usPINPadButtonInfoLen);

	*pusPINPadButtonNum= usPINPadButtonNum;
	*pusPINPadButtonInfoLen = usPINPadButtonInfoLen;
	memcpy(pbPINPadButtonInfo, bPINPadButtonInfo, usPINPadButtonInfoLen);

    DebugAddINT((BYTE*)"  pusPINPadButtonNum",  *pusPINPadButtonNum);
    DebugAddINT((BYTE*)"  pusPINPadButtonInfoLen", *pusPINPadButtonInfoLen);
    DebugAddHEX((BYTE*)"  pbPINPadButtonInfo", pbPINPadButtonInfo, *pusPINPadButtonInfoLen);
	return 0;
}

USHORT OnGetPINDone(void)
{
	vdDebug_LogPrintf((BYTE*)"OnGetPINDone() is triggered -->");
	if(ispinpadalone == true)
	{
		GetPINDone();
	}
	//CTOS_LCDTPrintXY(1, 3, (BYTE*)"Get PIN Done");

	return 0;
}

void OnShowPinDigit(IN BYTE bDigits,int reEnterPin)
{
	BYTE baStr[21];
	vdDebug_LogPrintf((BYTE*)"OnShowPinDigit() is triggered -->");
    DebugAddINT((BYTE*)"  bDigits", bDigits);

    DebugAddINT((BYTE*)"  ispinpadalone", ispinpadalone);
	if(ispinpadalone == true)
	{
		ShowPINDigit(bDigits, 0x1, reEnterPin/*0xF*/);
	}
	else
	{
		memset(baStr, 0x20, 20);
		baStr[20] = 0;

		memset(baStr, '*', bDigits);
		CTOS_LCDTPrintXY(1, 2, baStr);
	}
}

int inCallJAVA_BackToProgress(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[100+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====BackToProgress=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "BackToProgress", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "BackToProgress");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end BackToProgress");
    return d_OK;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_idleAmountEntry(JNIEnv *env, jobject instance) {

    // TODO
    int inRetVal = 0;

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead: inRetVal = [%d]", inRetVal);
    inRetVal = inFLGRead(1);
    vdDebug_LogPrintf("inFLGRead: inRetVal = [%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    vdDebug_LogPrintf("idleAmountEntry");
    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("SATURN inCTOS_IdleAmountEntry.............");
    //usCTOSS_LCDDisplay(" ");  //clear android screen
    //inRetVal = inCTOS_IdleAmountEntry();
    inRetVal = inStartTrans();

    vdDebug_LogPrintf("SATURN inCTOS_IdleAmountEntry - END............. inRetVal=[%d]", inRetVal);

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return inRetVal;

}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_posMain_1init(JNIEnv *env, jobject instance) {

    // TODO
    inDatabase_TerminalOpenDatabase();
    inTCTReadEx(1);
    inCPTReadEx(1);
    inPCTReadEx(1);
    inCSTReadEx(1);
    //inTCPRead(1);
    inHDTReadEx(1);
    inCSTReadEx(strHDT.inCurrencyIdx);
    inDatabase_TerminalCloseDatabase();

}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_idleEMVFallback(JNIEnv *env, jobject instance) {

    // TODO
    USHORT usTk1Len, usTk2Len, usTk3Len;
    BYTE szTk1Buf[TRACK_I_BYTES], szTk2Buf[TRACK_II_BYTES], szTk3Buf[TRACK_III_BYTES];
    usTk1Len = TRACK_I_BYTES;
    usTk2Len = TRACK_II_BYTES;
    usTk3Len = TRACK_III_BYTES;

    int byMSR_status = d_NO;
    int bySC_status = d_NO;
    int inKey = 0;
    BYTE szDisMsg[200];

    ing_KeyPressed = 0;

    strcpy(szDisMsg, "SALE");
    strcat(szDisMsg, "|");
    strcat(szDisMsg, "PLEASE INSERT CARD");
    strcat(szDisMsg, "|");
    strcat(szDisMsg, "CARD ENTRY");
    strcat(szDisMsg, "|");
    strcat(szDisMsg, "0");

    inKey = usCARDENTRY(szDisMsg);

    while(1) {

        byMSR_status = CTOS_MSRRead(szTk1Buf, &usTk1Len, szTk2Buf, &usTk2Len, szTk3Buf, &usTk3Len);
        if (byMSR_status == 0)
            vdCTOS_ResetMagstripCardData();

        CTOS_SCStatus(d_SC_USER, &bySC_status);
        //CTOS_Delay(1000);
        if (bySC_status & d_MK_SC_PRESENT) {
            CTOS_Beep();
            fEntryCardfromIDLE = TRUE;
            fIdleInsert = TRUE;
            //usCTOSS_LCDDisplay("SALE|PROCESSING...");

			#ifdef ANDROID_NEW_UI
            	//vdDisplayMessageStatusBox(1, 8, "PROCESSING...", MSG_PLS_WAIT, MSG_TYPE_PROCESS);
			#endif
			
            vdDebug_LogPrintf("chip entry");
            //inCTOSS_CLMCancelTransaction();
            vdCTOS_SetTransEntryMode(CARD_ENTRY_ICC);
            //shReturn = inCTOS_EMVCardReadProcess();
            vdDebug_LogPrintf("on ICC NewStringUTF");

            return (*env)->NewStringUTF(env, "ICC_OK");
        }

        if (ing_KeyPressed == 'C') {
            CTOS_KBDBufPut('C');
            vdDebug_LogPrintf("putchar C");
            //usCTOSS_LCDDisplay("SALE|USER CANCEL");
            inDisplayMessageBoxWithButtonConfirmation(1,8,"","User cancel","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
            CTOS_Beep();
            CTOS_Delay(1500);
            vdCTOS_TransEndReset();
            return (*env)->NewStringUTF(env, "TXN_ABORT");
        } else if (ing_KeyPressed == 'T') {
            CTOS_KBDBufPut('T');
            //usCTOSS_LCDDisplay("SALE|TIME OUT");
            inDisplayMessageBoxWithButtonConfirmation(1,8,"","Timeout","","",MSG_TYPE_INFO, BUTTON_TYPE_NONE_OK);
            CTOS_Beep();
            CTOS_Delay(1500);
            vdCTOS_TransEndReset();
            return (*env)->NewStringUTF(env, "TXN_ABORT");
        }
    }

}

int inCallJAVA_GetWifiInfo(BYTE *pbOutStr)
{
	
    unsigned char uszBuffer[528];
    int inRet = 0;


    vdDebug_LogPrintf("saturn =====inCallJAVA_GetWifiInfo=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    strcpy(uszBuffer, "test");

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetWIFISettings", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("saturn wifi settings %s", str);
        //*pbOutLen = strlen(str);
        //vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);
        (*env)->ReleaseStringUTFChars(env, result, str);



    }
    //else
        //*pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("saturn end inCallJAVA_GetWifiInfo");
    return d_OK;
}

int inCallJAVA_usGetConnectionStatus(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)

{
    unsigned char uszBuffer[128+1];
    int inRet = 0;


    vdDebug_LogPrintf("saturn =====inCallJAVA_usGetSerialNumber=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    memset(uszBuffer, 0x00, sizeof(uszBuffer));
    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);


    //vdDebug_LogPrintf("saturn uszBuffer[%s]", uszBuffer);
	//strcpy(uszBuffer, "WIFI");

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("saturn jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("saturn jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "fGetConnectStatus", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("saturn jstring[%s]", "fGetConnectStatus");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("saturn end fGetConnectStatus");
    return d_OK;
}

int inCallJAVA_ConfirmDetails(BYTE *pbCardDisplay, BYTE *pbOutBuf, BYTE *pbLen)
{
    unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_ConfirmDetails=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbCardDisplay)>0)
        strcpy(uszBuffer, pbCardDisplay);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserConfirmDetails", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserConfirmDetails");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    vdDebug_LogPrintf("jstring[%s]", "CallObjectMethod");
    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutBuf, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_ConfirmDetails");
    return d_OK;
}


int inCallJAVA_GetAmountStringWithMenu(BYTE *pbDispMsg, BYTE *pbAmtStr, BYTE *pbAmtLen)
{
    unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_GetAmountStringWithMenu=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
    	strcpy(uszBuffer, " ");

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", "This this Pass in string data to Java");

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetAmountStringWithMenu", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "GetAmountStringWithMenu");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbAmtLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbAmtStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbAmtLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_GetAmountStringWithMenu");
    return d_OK;
}


JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Trans_AmountEntryWithMenu_vdCTOSS_1FormatAmountUI(
        JNIEnv *env, jobject instance, jstring szFmt_, jstring szInAmt_) {
        
    const char *szFmt = (*env)->GetStringUTFChars(env, szFmt_, 0);
    const char *szInAmt = (*env)->GetStringUTFChars(env, szInAmt_, 0);
    char temp[24 + 1];

    // TODO
    memset(temp, 0, sizeof(temp));
    vdCTOSS_FormatAmount(szFmt, szInAmt, temp);

    (*env)->ReleaseStringUTFChars(env, szFmt_, szFmt);
    (*env)->ReleaseStringUTFChars(env, szInAmt_, szInAmt);

    return (*env)->NewStringUTF(env, temp);
}

int inCallJAVA_usGetSerialNumber(BYTE *pbOutStr, BYTE *pbOutLen)

{
    unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("saturn =====inCallJAVA_usGetSerialNumber=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);



    //if (strlen(pbDispMsg)>0)
    //    strcpy(uszBuffer, pbDispMsg);


    vdDebug_LogPrintf("saturn uszBuffer[%s]", uszBuffer);
	strcpy(uszBuffer, "test");

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetSerialNumber", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "GetSerialNumber");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_usGetSerialNumber");
    return d_OK;
}

int inCallJAVA_DisplayBox(BYTE *pbDispMsg, BYTE *pbOutStr_dispbox, BYTE *pbOutLen)
{
    unsigned char uszBuffer[512 + 1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_DisplayBox=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "DisplayBox", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr_dispbox, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_DisplayBox");
    return d_OK;
}

int inCallJAVA_DisplayStatusBox(BYTE *pbDispMsg, BYTE *pbOutStr_dsbox, BYTE *pbOutLen)
{
    unsigned char uszBuffer[528 + 1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_DisplayStatusBox=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "DisplayStatusBox", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr_dsbox, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_DisplayStatusBox");
    return d_OK;
}

int inCallJAVA_CTMSUPDATE(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[1024+1];
    int inRet = 0;

    memset(uszBuffer, 0x00, sizeof(uszBuffer));

    vdDebug_LogPrintf("=====inCallJAVA_CTMSUPDATE=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "CTMSUPDATE", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "CTMSUPDATE");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_CTMSUPDATE");
    return d_OK;
}


JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1Get1stInitFlag(JNIEnv *env, jobject thiz) {
    // TODO: implement inCTOSS_Get1stInitFlag()
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1GetDLNotFinishedFlag(JNIEnv *env,
                                                                            jobject thiz) {
    // TODO: implement inCTOSS_GetDLNotFinishedFlag()
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_Activity_1ctms_1update_inCTOSS_1BackupDataScript(JNIEnv *env,
                                                                                  jobject thiz) {
    // TODO: implement inCTOSS_BackupDataScript()
    
		vdDebug_LogPrintf("inCTOSS_BackupDataScript.............");
		char szOutPrmFile[100+1];
	
		memset(szOutPrmFile, 0, sizeof(szOutPrmFile));
	//	  inCTOSS_BackupDataScript("/data/data/pub/com.Source.S1_UOB.UOB.tbd", szOutPrmFile);
	//	  inCTOSS_BackupDataScript("/data/data/com.Source.S1_UOB.UOB/com.Source.S1_UOB.UOB.tbd", szOutPrmFile);
		inCTOSS_BackupDataScript("/data/data/com.Source.S1_BDO.BDO/com.Source.S1_BDO.BDO.tbd", szOutPrmFile);
		return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_Activity_1ctms_1update_inCTOSS_1SetDownloadedNotFinishedFlag(
        JNIEnv *env, jobject thiz, jint value) {
    // TODO: implement inCTOSS_SetDownloadedNotFinishedFlag()
    vdDebug_LogPrintf("inCTOSS_SetDownloadedNotFinishedFlag,byDLNotFinished[%d],value[%d].............",strTCT.byDLNotFinished, value);
    strTCT.byDLNotFinished = value;

    if(value == 0)
        strTCT.fFirstInit = 1;

    inTCTSave(1);
    return 0;
}

// sidumili: added for edit host info
int inCallJAVA_EditInfoListViewUI(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[9999+1];
    int inRet = 0;

	vdDebug_LogPrintf("=====inCallJAVA_EditInfoListViewUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "EditInfoListViewUI", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "EditInfoListViewUI");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_EditInfoListViewUI");
	
    return d_OK;
}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_GetAppHeaderDetails(JNIEnv *env, jobject instance, jint inMenu) {

    // TODO
    char szHeader[9000+1];
    int inRet, inSeek, inLoop;
	char reg_status[2] = {0};
    char idleMode[2] = {0};
	char szMenuImageItems[9000+1] = {0};
	
	vdDebug_LogPrintf("--GetAppHeaderDetails--");
		
    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    inTCTRead(1);
	inCPTRead(1);
    //strTCT.fShareComEnable = 1;
    inRet = inTCTSave(1);
    vdDebug_LogPrintf("inRet = [%d]", inRet);

    fEntryCardfromIDLE = FALSE;

	inMMTReadRecord(1,1);
	
	inNMTReadRecord(1);

	inGetTransImageList(szMenuImageItems, inMenu, "^");
	vdDebug_LogPrintf("inGetTransImageList,szMenuImageItems[%s]", szMenuImageItems);

	strcpy(reg_status, "1");
	strcpy(idleMode, "1");

    memset(szHeader, 0x00, sizeof(szHeader));
    strcpy(szHeader, strTCT.szAppVersionHeader);
    strcat(szHeader, " \n");
    strcat(szHeader, strSingleNMT.szMerchName);
    strcat(szHeader, " \n");
    strcat(szHeader, &strMMT[0].szMID);
    strcat(szHeader, " \n");
    strcat(szHeader, &strMMT[0].szTID);
    strcat(szHeader, " \n");
    strcat(szHeader, reg_status);
    strcat(szHeader, " \n");
    strcat(szHeader, idleMode);
	strcat(szHeader, " \n");
	strcat(szHeader, (strCPT.inCommunicationMode == GPRS_MODE ? "GPRS" : "WIFI"));
	strcat(szHeader, " \n");
	strcat(szHeader, szMenuImageItems);

	vdDebug_LogPrintf("szHeader=[%s]", szHeader);
	
    //remove to fix hang
    vdRemoveCard();

    return (*env)->NewStringUTF(env, szHeader);

}

int inCallJAVA_UserConfirmMenu2(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[d_MAX_IPC_BUFFER];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_UserConfirmMenu2=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
    //else
		//pbDispMsg = "Tine: UserConfirm activity";
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserConfirmMenu2", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserConfirmMenu2");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserConfirmMenu2");
    return d_OK;
}

int inCallJAVA_UserConfirmOKCancelMenu(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[512+1];
    int inRet = 0;

	vdDebug_LogPrintf("=====inCallJAVA_UserConfirmOKCancelMenu=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserConfirmOKCancelMenu", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserConfirmOKCancelMenu");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserConfirmOKCancelMenu");
    return d_OK;
}

int inCallJAVA_GetMobileInfo(BYTE *pbOutStr)
{

    unsigned char uszBuffer[528];
    int inRet = 0;


    vdDebug_LogPrintf("saturn =====inCallJAVA_GetMobileInfo=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    strcpy(uszBuffer, "test");

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetMOBILESettings", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("saturn mobile settings %s", str);
        //*pbOutLen = strlen(str);
        //vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);
        (*env)->ReleaseStringUTFChars(env, result, str);



    }
    //else
        //*pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("saturn end inCallJAVA_GetMobileInfo");
    return d_OK;
}

int inCallJAVA_ConfirmTotal(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[d_MAX_IPC_BUFFER];
    int inRet = 0;


    vdDebug_LogPrintf("saturn =====inCallJAVA_ConfirmTotal=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


  
    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);

	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "ConfirmTotal", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "ConfirmTotal");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_ConfirmTotal");
    return d_OK;
}

int inCallJAVA_getAppPackageInfo(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
	unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_getAppPackageInfo=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("pbDispMsg[%s]", pbDispMsg);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
        strcpy(uszBuffer, "PASS IN MSG");


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "AppPackageInfo", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "AppPackageInfo");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_getAppPackageInfo");
    return d_OK;
}

int inCallJAVA_DisplayApprovedUI(BYTE *pbDispMsg, BYTE *pbOutStr_dispbox, BYTE *pbOutLen)
{
    unsigned char uszBuffer[1024+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_DisplayApprovedUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "DisplayApprovedUI", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr_dispbox, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_DisplayApprovedUI");
    return d_OK;
}

int inCallJAVA_PromptMessageUI(BYTE *pbDispMsg, BYTE *pbOutStr_dispbox, BYTE *pbOutLen)
{
    unsigned char uszBuffer[528];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_PromptMessageUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "PromptMessageUI", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr_dispbox, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_PromptMessageUI");
    return d_OK;
}


JNIEXPORT 
jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1INSTALLMENT(JNIEnv *env, jobject instance) {
    // TODO: implement inCTOS_INSTALLMENT()

	int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

	//vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1INSTALLMENT--");
	
    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
	fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR
	
    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_INSTALLMENT.............");
    inCTOS_INSTALLMENT();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT
jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1LogOn(JNIEnv *env, jobject instance) {
    // TODO: implement inCTOSS_LogOn()

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    //vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1INSTALLMENT--");

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_LOGON.............");
    inCTOS_LOGON();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT 
jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1QRPay(JNIEnv *env, jobject instance) {
    // TODO: implement inCTOS_QRPay()

	int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

	//vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1INSTALLMENT--");
	
    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
	fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR
	
    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inQRPAY.............");
    inQRPAY();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT
jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1Retrieve(JNIEnv *env, jobject instance) {
    // TODO: implement inCTOS_Retrieve()

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    //vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1INSTALLMENT--");

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_Retrieve.............");
    inCTOS_Retrieve();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

int inCallJAVA_MenuSelection(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[9000+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_MenuSelection=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    //else
    //pbDispMsg = "Tine: UserConfirm activity";

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "MenuSelection", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "inCallJAVA_MenuSelection");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_MenuSelection");
    return d_OK;
}

JNIEXPORT
jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1BDOPayMenu(JNIEnv *env, jobject instance) {
    // TODO: implement inCTOS_BDOPayMenu()

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    //vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1INSTALLMENT--");

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inBDOPayMenu.............");
    inBDOPayMenu();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT
jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1BINCHECK(JNIEnv *env, jobject instance) {
    // TODO: implement inCTOS_BINCHECK()

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    //vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1INSTALLMENT--");

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_BINCHECK.............");
    inCTOS_BINCHECK();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

int inCallJAVA_TransDetailListViewUI(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[250+1];
    int inRet = 0;

	vdDebug_LogPrintf("=====inCallJAVA_TransDetailListViewUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "TransDetailListViewUI", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "TransDetailListViewUI");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_TransDetailListViewUI");
    return d_OK;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1HistoryMenu(JNIEnv *env, jobject instance) {
    // TODO: implement inCTOS_HistoryMenu()

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    //inRetVal = inTCTRead(1);
    //vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_SelectHistoryMenu.............");
	//DisplayStatusLine("");
    inCTOS_SelectHistoryMenu();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1MANAGEMENT(JNIEnv *env, jobject instance) {
    // TODO: implement inCTOSS_MANAGEMENT()

	int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_Management.............");
    inCTOS_Management();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

int inCallJAVA_SingleRecordDataListViewUI(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[250+1];
    int inRet = 0;

	vdDebug_LogPrintf("=====inCallJAVA_SingleRecordDataListViewUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "SingleRecordDataListViewUI", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "SingleRecordDataListViewUI");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_SingleRecordDataListViewUI");
    return d_OK;
}


JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1ECR_1Menu(JNIEnv *env, jobject instance,
                                                                 jint inECRTxnType) {
// TODO: implement inCTOSS_ECR_Menu()

int inRetVal = 0, inRet = 0;
BYTE outbuf[d_MAX_IPC_BUFFER];
USHORT out_len = 0;
char cwd[1024];

if(activityClass != NULL)
  (*env)->DeleteGlobalRef(env, activityClass);
if(activityObj != NULL)
  (*env)->DeleteGlobalRef(env, activityObj);

jint rs = (*env)->GetJavaVM(env, &jvm);

jclass cls = (*env)->GetObjectClass(env, instance);
activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
activityObj = (*env)->NewGlobalRef(env, instance);

fEntryCardfromIDLE=VS_FALSE;
vdSetECRTransactionFlg(VS_TRUE);

inRetVal = inTCTRead(1);
vdDebug_LogPrintf("AAA>>...[inECRTxnType[%d]", inECRTxnType);

switch (inECRTxnType)
{
     case 1:
          inCTOS_WAVE_SALE();
     	  break;

     case 2:
          inCTOS_VOID();
          break;
}

inCallJAVA_GetMenu();

vdSetECRTransactionFlg(0);
put_env_int("ECRPROCESS",0);
put_env_int("APP_STARTED", 0);

//release memory
if(cls != NULL)
  (*env)->DeleteLocalRef(env, cls);

return 0;
}

int inCallJAVA_GetMenu()
{
    vdDebug_LogPrintf("=====inCallJAVA_GetMenu=====");
    JNIEnv *env;

    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, clazz, "JNIGetMenu", "()V");

    (*env)->CallVoidMethod(env, activityObj, methodID);

    vdDebug_LogPrintf("=====inCallJAVA_GetMenu END=====");
    return d_OK;
}

JNIEXPORT jint
JNICALL
Java_com_Source_S1_1BDO_BDO_EcrCommandReceiver_vdEcrSetupInstance(JNIEnv *env, jobject instance) {
    vdDebug_LogPrintf("=====vdEcrSetupInstance=====");
    if(ecrClass != NULL)
        (*env)->DeleteGlobalRef(env, ecrClass);
    if(ecrObj != NULL)
        (*env)->DeleteGlobalRef(env, ecrObj);

    jint rs = (*env)->GetJavaVM(env, &ecrjvm);
    jclass cls = (*env)->GetObjectClass(env, instance);
    ecrClass = (jclass) (*env)->NewGlobalRef(env, cls);
    ecrObj = (*env)->NewGlobalRef(env, instance);
    vdDebug_LogPrintf("=====vdEcrSetupInstance exit=====");
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

int inCallJAVA_checkAppRunning(char *pAppame)
{
    unsigned char uszBuffer[500+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_checkAppRunning=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);

    if (strlen(pAppame)>0)
        strcpy(uszBuffer, pAppame);
    else
        strcpy(uszBuffer, "PASS IN MSG");

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "checkAppRunning", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "checkAppRunning");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)

    if(result != NULL)
        str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        if(memcmp(str, "1", 1) == 0)
            inRet = 1;
        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);


    }
    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end checkAppRunning", inRet);

//	ProcessingUI();
    return inRet;
}

int inCallJAVA_SendEcrResponse()
{
    vdDebug_LogPrintf("AAA<<>>> =====inCallJAVA_SendEcrResponse=====");
    JNIEnv *env;
    if(FALSE)
    {
        vdDebug_LogPrintf("=====Test code, don't send ecr resp=====");
        return d_OK;
    }

    jint rs = (*ecrjvm)->AttachCurrentThread(ecrjvm, &env, NULL);
    jstring jstr = (*env)->NewStringUTF(env, "dummy");

    jmethodID methodID = (*env)->GetMethodID(env, ecrClass, "sendBackEcrResponse", "(Ljava/lang/String;)V");

    (*env)->CallVoidMethod(env, ecrObj, methodID, jstr);
    vdDebug_LogPrintf("Clear buff");
    (*env)->DeleteLocalRef(env, jstr);

    vdDebug_LogPrintf("=====inCallJAVA_SendEcrResponse exit=====");
    return d_OK;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1TIPADJUST(JNIEnv *env, jobject instance) {
    // TODO: implement inCTOSS_TIPADJUST()
    // TODO: implement inCTOS_QRPay()

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    //vdDebug_LogPrintf("--Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1INSTALLMENT--");

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    vdDebug_LogPrintf("inCTOS_TIPADJUST.............");
    inCTOS_TIPADJUST();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

// Tip adjust confirmation -- sidumili
int inCallJAVA_UserConfirmTipAdjust(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_UserConfirmTipAdjust=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
    //else
		//pbDispMsg = "Tine: UserConfirm activity";
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserConfirmTipAdjust", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserConfirmTipAdjust");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserConfirmTipAdjust");
    return d_OK;
}

// Tip adjust approve ui -- sidumili
int inCallJAVA_DisplayTipAdjustApprovedUI(BYTE *pbDispMsg, BYTE *pbOutStr_dispbox, BYTE *pbOutLen)
{
    unsigned char uszBuffer[528];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_DisplayTipAdjustApprovedUI=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "DisplayTipAdjustApprovedUI", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr_dispbox, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_DisplayTipAdjustApprovedUI");
    return d_OK;
}

// for DCC -- sidumili
int inCallJAVA_UserConfirmDCC(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[1024+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_UserConfirmDCC=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
    //else
		//pbDispMsg = "Tine: UserConfirm activity";
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "UserConfirmDCC", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "UserConfirmDCC");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_UserConfirmDCC");
    return d_OK;
}

int inCallJAVA_InputQWERTY(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[512+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_InputQWERTY=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    vdDebug_LogPrintf("pbDispMsg[%s]", pbDispMsg);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
        strcpy(uszBuffer, "PASS IN MSG");


    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "InputQWERTYUI", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "InputQWERTYUI");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_InputQWERTY");
    return d_OK;
}

int inCallJAVA_Ping(BYTE *pbOutStr)
{
    unsigned char uszBuffer[528];
    int inRet = 0;

    vdDebug_LogPrintf("saturn =====inCallJAVA_GetPing=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    strcpy(uszBuffer, pbOutStr);

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "Ping", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("saturn wifi settings %s", str);
        strcpy(pbOutStr, str);
        (*env)->ReleaseStringUTFChars(env, result, str);
    }

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("saturn end inCallJAVA_GetPing");
    return d_OK;
}

int inCallJAVA_GetBatteryLevel(BYTE *pbOutStr)
{

    unsigned char uszBuffer[528];
    int inRet = 0;


    vdDebug_LogPrintf("saturn =====inCallJAVA_GetBatteryLevel=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);


    strcpy(uszBuffer, "test");

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetBatteryLevel", "(Ljava/lang/String;)Ljava/lang/String;");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL;
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL);
    if (str!=NULL)
    {
        vdDebug_LogPrintf("saturn wifi settings %s", str);
        //*pbOutLen = strlen(str);
        //vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);
        (*env)->ReleaseStringUTFChars(env, result, str);



    }
    //else
        //*pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    //(*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("saturn end inCallJAVA_GetBatteryLevel");
    return d_OK;
}


JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOS_1TransactionsMenu(JNIEnv *env, jobject instance, jint inMenu) {
    // TODO: implement inCTOS_HistoryMenu()

    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    //inRetVal = inTCTSave(1);
    //vdDebug_LogPrintf("inTCTSave inRetVal[%d]", inRetVal);

    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
    fPreConnectEx = FALSE;// hardcode as false - have to be change later if terminal will support ECR
    vdDebug_LogPrintf("inCTOS_SelectTransactionsMenu.............");
	//DisplayStatusLine("");
    inCTOS_SelectTransactionsMenu();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1CTMSDownloadRequest(JNIEnv *env,
                                                                           jobject instance) {
    // TODO: implement inCTOSS_CTMSDownloadRequest()

	// TODO
    int inRetVal = 0;
    BYTE outbuf[d_MAX_IPC_BUFFER];
    USHORT out_len = 0;
    EMVCL_RC_DATA_EX stRCDataEx;

    char cwd[1024];

    inRetVal = inTCTRead(1);
    vdDebug_LogPrintf("inTCTRead inRetVal[%d] strTCT.fDemo [%d]", inRetVal, strTCT.fDemo);
   
    if(activityClass != NULL)
        (*env)->DeleteGlobalRef(env, activityClass);
    if(activityObj != NULL)
        (*env)->DeleteGlobalRef(env, activityObj);

    jint rs = (*env)->GetJavaVM(env, &jvm);

    jclass cls = (*env)->GetObjectClass(env, instance);
    activityClass = (jclass) (*env)->NewGlobalRef(env, cls);
    activityObj = (*env)->NewGlobalRef(env, instance);


    vdDebug_LogPrintf("inCTOSS_TMSDownloadRequest.............");
	DisplayStatusLine("");
    inCTOSS_TMSDownloadRequest();

    //release memory
    if(cls != NULL)
        (*env)->DeleteLocalRef(env, cls);

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_Activity_1ctms_1update_inCTOSS_1GetEnvInt(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jstring sztag) {
    // TODO: implement inCTOSS_GetEnvInt()
    int ret = 0;

    const char *szFmt = (*env)->GetStringUTFChars(env, sztag, 0);

    ret = get_env_int(szFmt);
    vdDebug_LogPrintf("inCTOSS_1GetEnvInt[%d].............", ret);

    return ret;
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_Activity_1ctms_1update_inCTOSS_1SetEnvInt(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jstring sztag,
                                                                           jint invalue) {
    // TODO: implement inCTOSS_SetEnvInt()
    int     ret = -1;
    char    buf[64];
	const char *szFmt = (*env)->GetStringUTFChars(env, sztag, 0);

    memset (buf, 0, sizeof (buf));
    sprintf(buf, "%d", invalue);
    ret = inCTOSS_PutEnvDB (szFmt, buf);

    vdDebug_LogPrintf("inCTOSS_SetEnvInt [%s]=[%d] ret[%d]", szFmt, invalue, ret);
}

JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1CreatefsdataFolder(JNIEnv *env,
                                                                          jobject thiz) {
    // TODO: implement inCTOSS_CreatefsdataFolder()
#define d_FILE_EXIST 0x01

    BYTE pbaFileName[20] = {"PatTesting.txt"};
    BYTE bStorageType = 0;
    ULONG ulFileSize = 0, ulHandle;
    int ret = 0;

    /* Get the file size with specific file name.
     * If Get file size > 0, the file is already existed      */
    ret = CTOS_FileGetSize(pbaFileName,&ulFileSize);

    if (ulFileSize > 0)
        return d_FILE_EXIST;

    if ((ret != d_OK) && (ret != d_FS_FILE_NOT_FOUND))
        return ret;

    /* Open a file and return a number called a file handle.
     * If the specified file name does not exist , it will be created first. */
    ret =CTOS_FileOpen(pbaFileName , bStorageType , &ulHandle);

    if (ret == d_OK)
        CTOS_FileClose(ulHandle);
}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Trans_TipEntryActivity_vdCTOSS_1FormatAmountUI(JNIEnv *env,
                                                                           jobject thiz,
                                                                           jstring szFmt_,
                                                                           jstring szInAmt_) {
    // TODO: implement vdCTOSS_FormatAmountUI()
    const char *szFmt = (*env)->GetStringUTFChars(env, szFmt_, 0);
    const char *szInAmt = (*env)->GetStringUTFChars(env, szInAmt_, 0);
    char temp[24 + 1];

    // TODO
    memset(temp, 0, sizeof(temp));
    vdCTOSS_FormatAmount(szFmt, szInAmt, temp);

    (*env)->ReleaseStringUTFChars(env, szFmt_, szFmt);
    (*env)->ReleaseStringUTFChars(env, szInAmt_, szInAmt);

    return (*env)->NewStringUTF(env, temp);
}

int inCallJAVA_GetTipString(BYTE *pbDispMsg, BYTE *pbAmtStr, BYTE *pbAmtLen)
{
    unsigned char uszBuffer[528+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_GetTipString=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);

    if (strlen(pbDispMsg)>0)
        strcpy(uszBuffer, pbDispMsg);
    else
    	strcpy(uszBuffer, " ");

    vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", "This this Pass in string data to Java");

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

//	jmethodID methodID = (*env)->GetMethodID(env, clazz, "rndGetNumStr", "(Ljava/lang/String;)Ljava/lang/String;");
    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "GetTipString", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "GetTipString");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);
    //rs = (*jvm)->DetachCurrentThread(jvm);

    jbyte* str = NULL; //(*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); // should be released but what a heck, it's a tutorial :)
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbAmtLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbAmtStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbAmtLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_GetTipString");
    return d_OK;
}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Splash_getMerchantInfo(JNIEnv *env, jobject instance) {
    // TODO: implement getMerchantInfo()
    char szHeader[9000+1];
	
	vdDebug_LogPrintf("--getMerchantInfo--");

	inCPTRead(1);
    inTCTRead(1);
	
	inMMTReadRecord(1,1);

	inNMTReadRecord(1);

    memset(szHeader, 0x00, sizeof(szHeader));
    strcpy(szHeader, strTCT.szAppVersionHeader);
    strcat(szHeader, " \n");
    strcat(szHeader, strSingleNMT.szMerchName);
    strcat(szHeader, " \n");
    strcat(szHeader, &strMMT[0].szMID);
    strcat(szHeader, " \n");
    strcat(szHeader, &strMMT[0].szTID);
    strcat(szHeader, " \n");
	strcat(szHeader, strTCT.szAppVersionHeader);
    strcat(szHeader, " \n");
	strcat(szHeader, (strCPT.inCommunicationMode == GPRS_MODE ? "GPRS" : "WIFI"));
	strcat(szHeader, " \n");

	vdDebug_LogPrintf("Splash, szHeader=[%s]", szHeader);
	
    return (*env)->NewStringUTF(env, szHeader);
}

JNIEXPORT jstring JNICALL
Java_com_Source_S1_1BDO_BDO_Trans_LockScreen_getMerchantInfo(JNIEnv *env, jobject thiz) {
    // TODO: implement getMerchantInfo()

	char szHeader[9000+1];
	
	vdDebug_LogPrintf("--getMerchantInfo--");

	inCPTRead(1);
    inTCTRead(1);
	
	inMMTReadRecord(1,1);

	inNMTReadRecord(1);

    memset(szHeader, 0x00, sizeof(szHeader));
    strcpy(szHeader, strTCT.szAppVersionHeader);
    strcat(szHeader, " \n");
    strcat(szHeader, strSingleNMT.szMerchName);
    strcat(szHeader, " \n");
    strcat(szHeader, &strMMT[0].szMID);
    strcat(szHeader, " \n");
    strcat(szHeader, &strMMT[0].szTID);
    strcat(szHeader, " \n");
	strcat(szHeader, strTCT.szAppVersionHeader);
    strcat(szHeader, " \n");
	strcat(szHeader, (strCPT.inCommunicationMode == GPRS_MODE ? "GPRS" : "WIFI"));
	strcat(szHeader, " \n");

	vdDebug_LogPrintf("Splash, szHeader=[%s]", szHeader);
	
    return (*env)->NewStringUTF(env, szHeader);
}


JNIEXPORT jint JNICALL
Java_com_Source_S1_1BDO_BDO_Main_MainActivity_inCTOSS_1GetEnvInt(JNIEnv *env, jobject thiz,
                                                                 jstring sztag) {
    // TODO: implement inCTOSS_GetEnvInt()
    int ret = 0;

    const char *szFmt = (*env)->GetStringUTFChars(env, sztag, 0);

    ret = get_env_int(szFmt);
    vdDebug_LogPrintf("inCTOSS_1GetEnvInt[%d].............", ret);

    return ret;
}

int inCallJAVA_PrintAnimation(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[1024+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_PrintAnimation=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
    //else
		//pbDispMsg = "Tine: UserConfirm activity";
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "PrintAnimation", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "PrintAnimation");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_PrintAnimation");
    return d_OK;
}

int inCallJAVA_PrintAnimation2(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[1024+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_PrintAnimation2=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
    //else
		//pbDispMsg = "Tine: UserConfirm activity";
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "PrintAnimation2", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "PrintAnimation2");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_PrintAnimation2");
    return d_OK;
}

int inCallJAVA_PrintAnimation3(BYTE *pbDispMsg, BYTE *pbOutStr, BYTE *pbOutLen)
{
    unsigned char uszBuffer[1024+1];
    int inRet = 0;


    vdDebug_LogPrintf("=====inCallJAVA_PrintAnimation3=====");

    JNIEnv *env;
    jint rs = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
    // Use the env pointer...
    vdDebug_LogPrintf("jint[%d] *env[%x]", rs, *env);
  
  if (strlen(pbDispMsg)>0)
	  strcpy(uszBuffer, pbDispMsg);
    //else
		//pbDispMsg = "Tine: UserConfirm activity";
	
	vdDebug_LogPrintf("uszBuffer[%s]", uszBuffer);

    jstring jstr = (*env)->NewStringUTF(env, uszBuffer);
    vdDebug_LogPrintf("jstring[%s]", uszBuffer);

    jclass clazz = (*env)->FindClass(env, "com/Source/S1_BDO/BDO/Main/MainActivity");
    vdDebug_LogPrintf("jstring[%s]", "com/Source/S1_BDO/BDO/Main/MainActivity");

    jmethodID methodID = (*env)->GetMethodID(env, activityClass, "PrintAnimation3", "(Ljava/lang/String;)Ljava/lang/String;");

    vdDebug_LogPrintf("jstring[%s]", "PrintAnimation3");

    jobject result = (*env)->CallObjectMethod(env, activityObj, methodID, jstr);

    jbyte* str = NULL; 
    str = (*env)->GetStringUTFChars(env,(jstring) result, NULL); 
    if (str!=NULL)
    {
        vdDebug_LogPrintf("%s", str);
        *pbOutLen = strlen(str);
        vdDebug_LogPrintf("strcpy");
        strcpy(pbOutStr, str);

        vdDebug_LogPrintf("ReleaseStringUTFChars");
        (*env)->ReleaseStringUTFChars(env, result, str);

    }
    else
        *pbOutLen = 0;

    (*env)->DeleteLocalRef(env, jstr);
    (*env)->DeleteLocalRef(env, clazz);
    (*env)->DeleteLocalRef(env, result);

    vdDebug_LogPrintf("end inCallJAVA_PrintAnimation3");
    return d_OK;
}
