/* 
 * File:   ZDOControl.h
 * Author: fascio
 *
 * Created on 4 de abril de 2016, 13:38
 */

#ifndef __ZDO_CONTROL_H__
#define	__ZDO_CONTROL_H__

#include "ESPCommons.h"
#include "System.h"
#include "SystemLog.h"

BYTE ZDOControl_SetPANID(WORD panid);
//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************

BYTE bfnReadChannelLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

BYTE bfnReadPANIDLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

BYTE bfnWritePANIDLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);


#endif	/* __ZDO_CONTROL_H__ */
