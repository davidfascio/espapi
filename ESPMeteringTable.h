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
#include "SystemEvents.h"
#include "System.h"

#include "ESPCommons.h"
#include "DBMSHandler.h"

//******************************************************************************
// ESP_METERING_TABLE Defines
//******************************************************************************
#define ESP_METERING_TABLE_NEW_DEV_ADD                                      (1)
#define ESP_METERING_TABLE_UPDATE_DEV_ADD                                   (2)
#define ESP_METERING_TABLE_NEW_MTR_ADD                                      (3)
#define ESP_METERING_TABLE_UPDATE_MTR_ADD                                   (4)
#define ESP_METERING_TABLE_NEW_READING_ADD                                  (5)

#define ESP_METERING_TABLE_KEY_WORD                                     (0x1234)

#define ESP_METERING_TABLE_NO_ERROR_CODE                                    (0)
#define ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE                           (-1)
#define ESP_METERING_TABLE_MAC_ADDRESS_NOT_FOUND_ERROR_CODE                 (-2)
#define ESP_METERING_TABLE_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE               (-3)
#define ESP_METERING_TABLE_NUM_MAX_METERS_REACHED_ERROR_CODE                (-4)
#define ESP_METERING_TABLE_NUM_MAX_NODES_REACHED_ERROR_CODE                 (-5)
#define ESP_METERING_TABLE_ADDRESS_NOT_FOUND_ERROR_CODE                     (-6)
#define ESP_METERING_TABLE_RECORD_INDEX_OUT_OF_RANGE_ERROR_CODE             (-7)
#define ESP_METERING_TABLE_NO_DATA_MATCH_ERROR_CODE                         (-8)

//******************************************************************************
// ESP_METERING_TABLE DataTypes
//******************************************************************************


//******************************************************************************
// ESPMeteringTable Function Prototypes
//******************************************************************************
BYTE ESPMeteringTable_SelectQuery (WORD quantityOfItems, DBMS_HANDLER_TABLE_ID tableListType);

//******************************************************************************
// ESPMeteringTable Function Prototypes
//******************************************************************************

INT16 ESPMeteringTable_GetRecordIndex(DBMS_HANDLER_TABLE_ID tableId);
INT16 ESPMeteringTable_SetRecordIndex(DBMS_HANDLER_TABLE_ID tableId, INT16 meterIndexRecord);
INT16 ESPMeteringTable_GetMeterIndex(void);
INT16 ESPMeteringTable_SetMeterIndex(INT16 meterIndexRecord);
INT16 ESPMeteringTable_GetDeviceIndex(void);
INT16 ESPMeteringTable_SetDeviceIndex(INT16 deviceIndexRecord);


INT16 ESPMeteringTable_FindMeterTableIndexByRecordIndex(INT16 recordIndex);
INT16 ESPMeteringTable_FindMeterTableIndexBySerialNumber(BYTE * serialNumber);
INT16 ESPMeteringTable_FindAvailableMeterTableIndex(void);
INT16 ESPMeteringTable_InsertMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem);
INT16 ESPMeteringTable_InsertMeterTableItem(MTR_LIST_PTR meterItem);
WORD  ESPMeteringTable_GetMeterTableAddressByIndex(INT16 index);
INT16 ESPMeteringTable_UpdateMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem);
INT16 API_ESPMeteringTable_SelectMeterTableItemByRecordIndex(INT16 recordIndex, DBMS_HANDLER_RECORD_QUERY_PTR recordQuery );
INT16 API_ESPMeteringTable_InsertMeterTableItem(MTR_LIST_PTR meterItem);
INT16 ESPMeteringTable_DeleteMeterTableItemByIndex(INT16 index);
INT16 API_ESPMeteringTable_DropMeterTable(void);


INT16 ESPMeteringTable_FindDeviceTableIndexByRecordIndex(INT16 recordIndex);
INT16 ESPMeteringTable_FindDeviceTableIndexByMACAddress(BYTE * macAddress);
INT16 ESPMeteringTable_FindAvailableDeviceTableIndex(void);
INT16 ESPMeteringTable_InsertDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem);
INT16 ESPMeteringTable_InsertDeviceTableItem(DEV_LIST_PTR deviceItem);
WORD  ESPMeteringTable_GetDeviceTableAddressByIndex(INT16 index);
INT16 ESPMeteringTable_UpdateDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem);
INT16 API_ESPMeteringTable_SelectDeviceTableItemByRecordIndex(INT16 recordIndex, DBMS_HANDLER_RECORD_QUERY_PTR recordQuery);
INT16 API_ESPMeteringTable_InsertDeviceTableItem(DEV_LIST_PTR deviceItem);
INT16 ESPMeteringTable_DeleteDeviceTableItemByIndex(INT16 index);
INT16 API_ESPMeteringTable_DropDeviceTable(void);

WORD  ESPMeteringTable_GetReadingTableAddressByIndex(INT16 index);
INT16 ESPMeteringTable_InsertReadingTableItemByIndex(INT16 index, READING_LIST_PTR readingItem);
INT16 API_ESPMeteringTable_SelectReadingTableItemByRecordIndex(INT16 recordIndex, DBMS_HANDLER_RECORD_QUERY_PTR recordQuery);
INT16 API_ESPMeteringTable_InsertReadingTableItem(READING_LIST_PTR readingItem);

INT16 API_ESPMeteringTable_Init(void);

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************

BYTE bfnReadMTRSTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);
BYTE bfnReadDevicesTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);
BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);
BYTE bfnDelMTRMetersTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);
BYTE bfnDelMTRDevicesTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void ESPMeteringTableDriver(void);
void ESPMeteringTable_SetStateMachine(BYTE actualState, BYTE nextState);
void ESPMeteringTable_ErrorProcess(void);

#endif	/* __ESP_METERING_TABLE_H__ */
