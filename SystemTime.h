/* 
 * File:   SystemTime.h
 * Author: fascio
 *
 * Created on 4 de abril de 2016, 16:11
 */

#ifndef __SYSTEM_TIME_H__
#define	__SYSTEM_TIME_H__

#include "GenericTypeDefs.h"
#include "System.h"

#define ZIGBEE_EPOCH_TIME                                           (946684800)

typedef struct __attribute__((packed,aligned(1))){
    BYTE timeZone;
    DWORD time;    
}SYSTEM_TIME;

DWORD SystemTime_GetTime(void);
BYTE SystemTime_SetTime(SYSTEM_TIME time);

BYTE bfnReadTimeDateLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

BYTE bfnWriteTimeDateLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

#endif	/* __SYSTEM_TIME_H__ */

