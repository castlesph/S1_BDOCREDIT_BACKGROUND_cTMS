#ifndef _LOCALFUNC__H
#define	_LOCALFUNC__H

#ifdef	__cplusplus
extern "C" {
#endif
#define DB_LOCALAP	"/data/data/com.Source.S1_BDO.BDO/LOCALAP.S3DB"

int inLocalAP_Database_BatchRead(TRANS_DATA_TABLE *transData);
int inLocalAP_Database_BatchUpdate(TRANS_DATA_TABLE *transData);
int inLocalAP_Database_BatchDelete(void);
int inLocalAP_Database_BatchInsert(TRANS_DATA_TABLE *transData);

#endif	/* _LOCALFUNC__H */

