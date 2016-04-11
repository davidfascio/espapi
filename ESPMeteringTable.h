/* 
 * File:   ESPMeteringTable.h
 * Author: fascio
 *
 * Created on 1 de abril de 2016, 13:07
 */

#ifndef __ESP_METERING_TABLE_H__
#define	__ESP_METERING_TABLE_H__

//******************************************************************************
// Includes
//******************************************************************************
#include "GenericTypeDefs.h"
#include "System.h"
#include "Data_Base_Handler.h"
#include "ESPCommons.h"

//******************************************************************************
// ESPMeteringTable Function Prototypes
//******************************************************************************
BYTE bfnBuffer_Table_Meter (WORD quantityOfItems, ESP_METERING_TABLE_LIST_TYPE tableListType);

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************

BYTE bfnReadMTRSTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnReadDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnDelMTRMetersTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnDelMTRDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);


//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************
void vfn_tBufferLocalDriver(void);
void ESPMeteringTable_SetStateMachine(BYTE actualState, BYTE nextState);
void ESPMeteringTable_ErrorProcess(void);

#endif	/* __ESP_METERING_TABLE_H__ */
