/* 
 * File:   ZDOControl.h
 * Author: fascio
 *
 * Created on 4 de abril de 2016, 13:38
 */

#ifndef __ZDO_CONTROL_H__
#define	__ZDO_CONTROL_H__

//******************************************************************************
// Includes
//******************************************************************************
#include "ESPCommons.h"
#include "System.h"
#include "SystemLog.h"

//******************************************************************************
// ZDOControl Function Prototypes
//******************************************************************************
BYTE ZDOControl_SetPANID(WORD panid);
BYTE ZDOControl_SetLocalPermitJoin(BYTE permit);

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************

BYTE bfnReadChannelLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnReadPANIDLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnWritePANIDLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnWritePermitJoinLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

#endif	/* __ZDO_CONTROL_H__ */

