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


typedef enum{
    METER_TABLE_LIST,
    READING_TABLE_LIST,
    DEVICE_TABLE_LIST
}ESP_METERING_TABLE_LIST_TYPE;

typedef struct{
    WORD startItem;
    WORD quantityOfItems;
    ESP_METERING_TABLE_LIST_TYPE tableListType;
}ESP_METERING_TABLE_QUERY, * ESP_METERING_TABLE_QUERY_PTR;

void ESPMeteringTable_SetupQuery(ESP_METERING_TABLE_QUERY_PTR query, WORD startItem, WORD quantityOfItems, ESP_METERING_TABLE_LIST_TYPE tableListType);
BYTE bfnBuffer_Table_Meter (WORD quantityOfItems, ESP_METERING_TABLE_LIST_TYPE tableListType);

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************

BYTE bfnReadMTRSTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

BYTE bfnReadDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

BYTE bfnDelMTRMetersTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

BYTE bfnDelMTRDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void ESPMeteringTable_SetStateMachine(BYTE actualState, BYTE nextState);

#endif	/* __ESP_METERING_TABLE_H__ */

