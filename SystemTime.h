/* 
 * File:   SystemTime.h
 * Author: fascio
 *
 * Created on 4 de abril de 2016, 16:11
 */

#ifndef __SYSTEM_TIME_H__
#define	__SYSTEM_TIME_H__

//******************************************************************************
// Includes
//******************************************************************************
#include "GenericTypeDefs.h"
#include "System.h"
#include "SystemLog.h"

//******************************************************************************
// Defines
//******************************************************************************
#define ZIGBEE_EPOCH_TIME                                           (946684800)

//******************************************************************************
// SYSTEM TIME Vartypes
//******************************************************************************
typedef struct __attribute__((packed,aligned(1))){
    BYTE timeZone;
    DWORD time;    
}SYSTEM_TIME;

//******************************************************************************
// System Time Function Prototypes
//******************************************************************************
DWORD SystemTime_GetTime(void);
BYTE SystemTime_SetTime(SYSTEM_TIME time);

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************
BYTE bfnReadTimeDateLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnWriteTimeDateLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

#endif	/* __SYSTEM_TIME_H__ */
