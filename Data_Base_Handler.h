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
#define DATA_BASE_HANDLER_QUERY_RESPONSE_MAX_SIZE                         (100)

#define DATA_BASE_TEST
//**********************************************************************
// Error Code
//**********************************************************************
#define DATA_BASE_HANDLER_NO_ERROR_CODE						(0)
#define DATA_BASE_HANDLER_NUM_MAX_METERS_REACHED_ERROR_CODE			(-1)
#define DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE				(-2)
#define DATA_BASE_HANDLER_MAC_ADDRESS_NOT_FOUND_ERROR_CODE			(-3)
#define DATA_BASE_HANDLER_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE                    (-4)
#define DATA_BASE_HANDLER_NUM_MAX_NODES_REACHED_ERROR_CODE			(-5)

typedef enum{
    METER_TABLE_LIST,
    READING_TABLE_LIST,
    DEVICE_TABLE_LIST
}DATA_BASE_HANDLER_LIST_TYPE;

typedef struct{
    WORD startItem;
    WORD quantityOfItems;
    DATA_BASE_HANDLER_LIST_TYPE tableListType;
    BYTE queryResponseBuffer[DATA_BASE_HANDLER_QUERY_RESPONSE_MAX_SIZE];
    WORD queryResponseBufferSize;
    BOOL isWaitingForQueryResponse;
}DATA_BASE_HANDLER_QUERY, * DATA_BASE_HANDLER_QUERY_PTR;

/*-- Local functions prototypes --------------------------------------------------------*/
void DataBaseHandler_SetupQuery(DATA_BASE_HANDLER_QUERY_PTR query, WORD startItem, WORD quantityOfItems, DATA_BASE_HANDLER_LIST_TYPE tableListType);
void DataBaseHandler_ClearQuery(DATA_BASE_HANDLER_QUERY_PTR query);
WORD DataBaseHandler_GetStartItem(DATA_BASE_HANDLER_QUERY_PTR query);
void DataBaseHandler_SetStartItem(DATA_BASE_HANDLER_QUERY_PTR query, WORD startItem);
WORD DataBaseHandler_GetQuantityOfItems(DATA_BASE_HANDLER_QUERY_PTR query);
void DataBaseHandler_SetQuantityOfItems(DATA_BASE_HANDLER_QUERY_PTR query, WORD quantityOfItems);
DATA_BASE_HANDLER_LIST_TYPE DataBaseHandler_GetTableListType(DATA_BASE_HANDLER_QUERY_PTR query);
BYTE * DataBaseHandler_GetQueryResponseBuffer(DATA_BASE_HANDLER_QUERY_PTR query);
WORD DataBaseHandler_GetQueryResponseBufferSize(DATA_BASE_HANDLER_QUERY_PTR query);
void DataBaseHandler_SetQueryResponseBufferSize(DATA_BASE_HANDLER_QUERY_PTR query, WORD queryResponseBufferSize);
BOOL DataBaseHandler_IsWaitingForQueryResponse(DATA_BASE_HANDLER_QUERY_PTR query);
void DataBaseHandler_SetWaitingForQueryResponse(DATA_BASE_HANDLER_QUERY_PTR query, BOOL isWaitingForQueryResponse);

/*-- Defines ---------------------------------------------------------------------------*/
#define NEW_MTR_ADD                 0x02
#define NO_NEW_MTR_ADD              0x03
/*-- Variables -------------------------------------------------------------------------*/

/*-- Functions -------------------------------------------------------------------------*/
BYTE bfnSaveData(BYTE bTableType,BYTE *vptrTableStructure);
BYTE bfnConsultData(BYTE bTableType, BYTE *bptrKeyID, WORD wAllDataIndexDev
        , WORD wAllDataIndexMtr, DATA_BASE_HANDLER_QUERY_PTR query);
void vfnInitDataBase(void);
BYTE bfnBackUp_Init (void);
WORD wfnIndexConsutl(BYTE bTableType);
void vfnBackUpStartDriver(void);
BYTE bfnDelAllData(BYTE bTableType,BYTE *bptrKeyID);
void vfnSaveIndexMet(void);



//******************************************************************************
//
//******************************************************************************

INT16 DataBaseHandler_FindMeterTableIndexBySerialNumber(BYTE * serialNumber);
INT16 DataBaseHandler_FindAvailableMeterTableIndex(void);
INT16 DataBaseHandler_SetupMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem);
INT16 DataBaseHandler_AddNewMeterTableItem(MTR_LIST_PTR meterItem);
Meter_Eneri_PTR DataBaseHandler_GetMeterTableByIndex(INT16 index);
INT16 DataBaseHandler_UpdateMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem);
BYTE DataBaseHandler_SaveMeterTableItem(MTR_LIST_PTR meterItem);

INT16 DataBaseHandler_FindDeviceTableIndexByMACAddress(BYTE * macAddress);
INT16 DataBaseHandler_FindAvailableDeviceTableIndex(void);
Device_Eneri_PTR DataBaseHandler_GetDeviceTableByIndex(INT16 index);
INT16 DataBaseHandler_SetupDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem);
INT16 DataBaseHandler_AddNewDeviceTableItem(DEV_LIST_PTR deviceItem);
INT16 DataBaseHandler_UpdateDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem);
BYTE DataBaseHandler_SaveDeviceTableItem(DEV_LIST_PTR deviceItem);

INT16 DataBaseHandler_AddNewReadingTableItemByIndex(INT16 index, READING_LIST_PTR readingItem);
BYTE DataBaseHandler_SaveReadingTableItem(READING_LIST_PTR readingItem);

BYTE API_DataBaseHandler_SaveTable(BYTE bTableType, BYTE *vptrTableStructure );

#endif
/****************************************************************************************/
/****************************************************************************************/




