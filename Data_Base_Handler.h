/****************************************************************************************/
/*
 * File:        Data_Base_Handler.h
 * Author:      Jose Antonio Becerril Morales
 * version:     Intern Support
 * Processor:   PIC32MX360F512L
 * Complier:    C32(v2.02)
 * Company:     Eneri
 * Date:        Created on September 17,2013.
 * Comment:     Data Base Control
 */
/****************************************************************************************/
#ifndef DATA_BASE_HANDLER_H
#define DATA_BASE_HANDLER_H
/*-- Includes --------------------------------------------------------------------------*/
//!#include "HIL/mem24_1025_I2C.h"
//#include "ESPCommands.h"
#include "GenericTypeDefs.h"
#include "Utility.h"
#include "mem24_1025_I2C.h"
#include "ESPMeteringCommons.h"
#include "EventsEngine.h"
#include "SystemEvents.h"

/*-- Types Definitions -----------------------------------------------------------------*/
#define ESP_METERING_TABLE_QUERY_RESPONSE_MAX_SIZE                         (100)

typedef enum{
    METER_TABLE_LIST,
    READING_TABLE_LIST,
    DEVICE_TABLE_LIST
}ESP_METERING_TABLE_LIST_TYPE;

typedef struct{
    WORD startItem;
    WORD quantityOfItems;
    ESP_METERING_TABLE_LIST_TYPE tableListType;
    BYTE queryResponseBuffer[ESP_METERING_TABLE_QUERY_RESPONSE_MAX_SIZE];
    WORD queryResponseBufferSize;
    BOOL isWaitingForQueryResponse;
}ESP_METERING_TABLE_QUERY, * ESP_METERING_TABLE_QUERY_PTR;

/*-- Local functions prototypes --------------------------------------------------------*/
void ESPMeteringTable_SetupQuery(ESP_METERING_TABLE_QUERY_PTR query, WORD startItem, WORD quantityOfItems, ESP_METERING_TABLE_LIST_TYPE tableListType);
void ESPMeteringTable_ClearQuery(ESP_METERING_TABLE_QUERY_PTR query);
WORD ESPMeteringTable_GetStartItem(ESP_METERING_TABLE_QUERY_PTR query);
void ESPMeteringTable_SetStartItem(ESP_METERING_TABLE_QUERY_PTR query, WORD startItem);
WORD ESPMeteringTable_GetQuantityOfItems(ESP_METERING_TABLE_QUERY_PTR query);
void ESPMeteringTable_SetQuantityOfItems(ESP_METERING_TABLE_QUERY_PTR query, WORD quantityOfItems);
ESP_METERING_TABLE_LIST_TYPE ESPMeteringTable_GetTableListType(ESP_METERING_TABLE_QUERY_PTR query);
BYTE * ESPMeteringTable_GetQueryResponseBuffer(ESP_METERING_TABLE_QUERY_PTR query);
WORD ESPMeteringTable_GetQueryResponseBufferSize(ESP_METERING_TABLE_QUERY_PTR query);
void ESPMeteringTable_SetQueryResponseBufferSize(ESP_METERING_TABLE_QUERY_PTR query, WORD queryResponseBufferSize);
BOOL ESPMeteringTable_IsWaitingForQueryResponse(ESP_METERING_TABLE_QUERY_PTR query);
void ESPMeteringTable_SetWaitingForQueryResponse(ESP_METERING_TABLE_QUERY_PTR query, BOOL isWaitingForQueryResponse);

/*-- Defines ---------------------------------------------------------------------------*/
#define NEW_MTR_ADD                 0x02
#define NO_NEW_MTR_ADD              0x03
/*-- Variables -------------------------------------------------------------------------*/

/*-- Functions -------------------------------------------------------------------------*/
BYTE bfnSaveData(BYTE bTableType,BYTE *vptrTableStructure);
BYTE bfnConsultData(BYTE bTableType, BYTE *bptrKeyID, WORD wAllDataIndexDev
        , WORD wAllDataIndexMtr, ESP_METERING_TABLE_QUERY_PTR query);
void vfnInitDataBase(void);
BYTE bfnBackUp_Init (void);
WORD wfnIndexConsutl(BYTE bTableType);
void vfnBackUpStartDriver(void);
BYTE bfnDelAllData(BYTE bTableType,BYTE *bptrKeyID);
void vfnSaveIndexMet(void);
#endif
/****************************************************************************************/
/****************************************************************************************/




