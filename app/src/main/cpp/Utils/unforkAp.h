#define UNFORKAPP "/data/data/com.Source.S1_VisaMaster.VisaMaster/unforkap.ini"
#define AMT "/data/data/com.Source.S1_VisaMaster.VisaMaster/amt"
#ifdef	__cplusplus
extern "C" {
#endif

void vdNotLoadUnusedApps(void);
char CheckUnloadApp( char * apname );
    
#ifdef	__cplusplus
}
#endif