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
BYTE bfnBuffer_Table_Meter (WORD quantityOfItems, DATA_BASE_HANDLER_LIST_TYPE tableListType);

//******************************************************************************
// ESPMeteringTable Function Prototypes
//******************************************************************************
INT16 ESPMeteringTable_FindMeterTableIndexBySerialNumber(BYTE * serialNumber);
INT16 ESPMeteringTable_FindAvailableMeterTableIndex(void);
INT16 ESPMeteringTable_SetupMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem);
INT16 ESPMeteringTable_AddNewMeterTableItem(MTR_LIST_PTR meterItem);
Meter_Eneri_PTR ESPMeteringTable_GetMeterTableByIndex(INT16 index);
INT16 ESPMeteringTable_UpdateMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem);
BYTE ESPMeteringTable_SaveMeterTableItem(MTR_LIST_PTR meterItem);

INT16 ESPMeteringTable_FindDeviceTableIndexByMACAddress(BYTE * macAddress);
INT16 ESPMeteringTable_FindAvailableDeviceTableIndex(void);
Device_Eneri_PTR ESPMeteringTable_GetDeviceTableByIndex(INT16 index);
INT16 ESPMeteringTable_SetupDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem);
INT16 ESPMeteringTable_AddNewDeviceTableItem(DEV_LIST_PTR deviceItem);
INT16 ESPMeteringTable_UpdateDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem);
BYTE ESPMeteringTable_SaveDeviceTableItem(DEV_LIST_PTR deviceItem);

INT16 ESPMeteringTable_AddNewReadingTableItemByIndex(INT16 index, READING_LIST_PTR readingItem);
BYTE ESPMeteringTable_SaveReadingTableItem(READING_LIST_PTR readingItem);

BYTE API_ESPMeteringTable_SaveTable(BYTE bTableType, BYTE *vptrTableStructure );
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
