/* 
 * File:   ZCLMeteringControl.h
 * Author: fascio
 *
 * Created on 5 de abril de 2016, 10:48
 */

#ifndef __ZCL_METERING_CONTROL_H__
#define	__ZCL_METERING_CONTROL_H__

#include "System.h"
#include "ESPCommons.h"
#include "SystemLog.h"

#include "ZCLComProtocol.h"

#define _600_SECONDS                                        600

BYTE ZCLMeteringControl_SetAutomaticReadingTime(WORD automaticReadingTime);

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************
BYTE bfnReadFrecuencyAutoReadingsLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

BYTE bfnWriteFrecuencyAutoReadingsLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

#endif	/* __ZCL_METERING_CONTROL_H__ */

