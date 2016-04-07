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
void ESPMeteringTable_ClearQuery(ESP_METERING_TABLE_QUERY_PTR query);
WORD ESPMeteringTable_GetStartItem(ESP_METERING_TABLE_QUERY_PTR query);
void ESPMeteringTable_SetStartItem(ESP_METERING_TABLE_QUERY_PTR query, WORD startItem);
WORD ESPMeteringTable_GetQuantityOfItems(ESP_METERING_TABLE_QUERY_PTR query);
void ESPMeteringTable_SetQuantityOfItems(ESP_METERING_TABLE_QUERY_PTR query, WORD quantityOfItems);
ESP_METERING_TABLE_LIST_TYPE ESPMeteringTable_GetTableListType(ESP_METERING_TABLE_QUERY_PTR query);
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

