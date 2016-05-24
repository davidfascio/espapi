//******************************************************************************
//* File DataBaseHandler.c
//

//******************************************************************************
// Includes
//******************************************************************************
#include "ESPMeteringTable.h"

//******************************************************************************
// Variables
//******************************************************************************
DATA_BASE_HANDLER_QUERY espMeteringTableQuery;

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void vfnBufferIdleState(void);
void vfnBufferConsultTableState(void);
void vfnBufferTransmitTableState(void);
void vfnBufferEndState(void);

typedef enum {
    _BUFFER_METER_IDLE_STATE = 0,
    _BUFFER_CONSULT_TABLE_STATE,
    _BUFFER_TRANSMIT_TABLE_STATE,
    _BUFFER_END_STATE
} eBUFFERDriverStates;

void (*const vfnaBufferDriverState[])(void) =
{
    vfnBufferIdleState
    ,vfnBufferConsultTableState
    ,vfnBufferTransmitTableState
    ,vfnBufferEndState    
};

sSM _tBufferSM = {  _BUFFER_METER_IDLE_STATE,               /* bActualState     */
                    _BUFFER_METER_IDLE_STATE,               /* bNextState       */
                    _BUFFER_METER_IDLE_STATE,               /* bPrevState       */
                    _BUFFER_METER_IDLE_STATE};              /* bErrorState      */

//******************************************************************************
// DataBaseHandler Function Prototypes
//******************************************************************************
BYTE bfnBuffer_Table_Meter(WORD quantityOfItems, DATA_BASE_HANDLER_LIST_TYPE tableListType) {

    switch (tableListType) {

        case METER_TABLE_LIST:
        case READING_TABLE_LIST:
        case DEVICE_TABLE_LIST:
            break;

        default:
            return FALSE;
    }
    
    DataBaseHandler_SetupQuery(&espMeteringTableQuery, 0, quantityOfItems, tableListType);
    ESPMeteringTable_SetStateMachine(_BUFFER_CONSULT_TABLE_STATE, _BUFFER_TRANSMIT_TABLE_STATE);
    
    return TRUE;
}

//******************************************************************************
// DataBaseHandler Function Prototypes
//******************************************************************************

INT16 ESPMeteringTable_FindMeterTableIndexBySerialNumber(BYTE * serialNumber){
	
    Meter_Eneri_PTR meter_ptr = tsMeter;	
    INT16 index = 0;

    while(index < NUM_MAX_METERS){

        if(!memcmp(meter_ptr->meterItem.Serial_Num, serialNumber, ID_METER_SIZE))
            break;

        meter_ptr++;
        index++;
    }

    if(index == NUM_MAX_METERS)
        return DATA_BASE_HANDLER_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE;

    return index;
}

INT16 ESPMeteringTable_FindAvailableMeterTableIndex(void){
	
    Meter_Eneri_PTR meter_ptr = tsMeter;	
    INT16 index = 0;

    while(index < NUM_MAX_METERS) {

        if(!meter_ptr->meterItem.Type)
            break;

        meter_ptr++;
        index++;
    }
	
    if(index == NUM_MAX_METERS)
	return DATA_BASE_HANDLER_NUM_MAX_METERS_REACHED_ERROR_CODE;
		
    return index;
}

INT16 ESPMeteringTable_SetupMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem){
	
    INT16 error_code;
    Meter_Eneri_PTR meter_ptr = ESPMeteringTable_GetMeterTableByIndex(index);
	
    if (meter_ptr == NULL)
        return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;
	
    // Clear Meter_Eneri Struct
    memset(meter_ptr, 0, sizeof(Meter_Eneri));

    memcpy((BYTE *) &meter_ptr->meterItem, meterItem, sizeof(MTR_LIST)); 		
    meter_ptr->Address = (CAB_READ_Meter_ADD + (index * sizeof(Meter_Eneri)));	
    meter_ptr->CRC = wfnCRC16( (BYTE *) meter_ptr, sizeof(Meter_Eneri) - sizeof(WORD));	
    //error_code = API_MEM24_1025_I2C_Write( (BYTE *) meter_ptr, meter_ptr->Address, sizeof(Meter_Eneri));	
    MEM_EEPROM_Write( meter_ptr->Address, (BYTE *) meter_ptr,  sizeof(Meter_Eneri));	
    //!print_error("ERROR CODE: %d", error_code);
    
    return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

INT16 ESPMeteringTable_AddNewMeterTableItem(MTR_LIST_PTR meterItem){
	
    INT16 index;

    index = ESPMeteringTable_FindAvailableMeterTableIndex();	

    if(index == DATA_BASE_HANDLER_NUM_MAX_METERS_REACHED_ERROR_CODE)	
        return DATA_BASE_HANDLER_NUM_MAX_METERS_REACHED_ERROR_CODE;
    
    print_info("Adding New Meter Table");
    return ESPMeteringTable_SetupMeterTableItemByIndex(index, meterItem);
}

Meter_Eneri_PTR ESPMeteringTable_GetMeterTableByIndex(INT16 index){
	
    if (index >= NUM_MAX_METERS)
	return NULL;
	
    return (tsMeter + index);
}

INT16 ESPMeteringTable_UpdateMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem){
	
    Meter_Eneri_PTR foundMeter;
    
    foundMeter = ESPMeteringTable_GetMeterTableByIndex(index);

    if(foundMeter == NULL)
        return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;

    if( memcmp((BYTE *) &foundMeter->meterItem, meterItem, sizeof(MTR_LIST))){		
        
        print_info("Updating Meter Table");
        return ESPMeteringTable_SetupMeterTableItemByIndex(index, meterItem);
    }

    print_info("Meter Table Index did not change");
    return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

BYTE ESPMeteringTable_SaveMeterTableItem(MTR_LIST_PTR meterItem){
	
    INT16 index;		
    INT16 error_code;

    index = ESPMeteringTable_FindMeterTableIndexBySerialNumber(meterItem->Serial_Num);

    if(index == DATA_BASE_HANDLER_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE){		

        error_code = ESPMeteringTable_AddNewMeterTableItem(meterItem);

        if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
            return 0;

        wMetersIndex++;
        return NEW_MTR_ADD;
    }

    error_code = ESPMeteringTable_UpdateMeterTableItemByIndex(index, meterItem);

    if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
        return 0;

    return NO_NEW_MTR_ADD;	
}

INT16 ESPMeteringTable_FindDeviceTableIndexByMACAddress(BYTE * macAddress){
	
    Device_Eneri_PTR device_ptr = tsDevice;	
    INT16 index = 0;
	
    while(index < NUM_MAX_NODES){
		
        if(!memcmp(device_ptr->deviceItem.MAC, macAddress, MAC_SIZE))
            break;
		
        device_ptr++;
        index++;
    }
	
    if(index == NUM_MAX_NODES)
        return DATA_BASE_HANDLER_MAC_ADDRESS_NOT_FOUND_ERROR_CODE;
		
    return index;
}

INT16 ESPMeteringTable_FindAvailableDeviceTableIndex(void){
	
    Device_Eneri_PTR device_ptr = tsDevice;	
    INT16 index = 0;
	
    while(index < NUM_MAX_NODES){
		
        if(!device_ptr->deviceItem.Type)
            break;
			
        device_ptr++;
        index++;
    }
	
    if(index == NUM_MAX_NODES)
        return DATA_BASE_HANDLER_NUM_MAX_NODES_REACHED_ERROR_CODE;
		
    return index;
}

Device_Eneri_PTR ESPMeteringTable_GetDeviceTableByIndex(INT16 index){
	
	if (index >= NUM_MAX_NODES)
		return NULL;
	
	return (tsDevice + index);
}

INT16 ESPMeteringTable_SetupDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem){
	
	Device_Eneri_PTR device_ptr = ESPMeteringTable_GetDeviceTableByIndex(index);
	
	if (device_ptr == NULL)
		return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;
	
	// Clear Meter_Eneri Struct
	memset(device_ptr, 0, sizeof(Device_Eneri));
		
	memcpy( (BYTE *) &device_ptr->deviceItem, deviceItem, sizeof(DEV_LIST)); 		
	device_ptr->Address = (CAB_READ_Meter_MAC_SHORT + (index * sizeof(Device_Eneri)));	
	device_ptr->CRC = wfnCRC16( (BYTE *) device_ptr, sizeof(Device_Eneri) - sizeof(WORD));	
	//!API_MEM24_1025_I2C_Write( (BYTE *) device_ptr, device_ptr->Address, sizeof(Device_Eneri));	
	MEM_EEPROM_Write(  device_ptr->Address, (BYTE *) device_ptr, sizeof(Device_Eneri));	
	return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

INT16 ESPMeteringTable_AddNewDeviceTableItem(DEV_LIST_PTR deviceItem){
	
	INT16 index;
	
	index = ESPMeteringTable_FindAvailableDeviceTableIndex();	
	
	if(index == DATA_BASE_HANDLER_NUM_MAX_NODES_REACHED_ERROR_CODE)	
		return index;
		
        print_info("Adding New Device Table");
	return ESPMeteringTable_SetupDeviceTableItemByIndex(index, deviceItem);
}


INT16 ESPMeteringTable_UpdateDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem){
	
	Device_Eneri_PTR foundDevice;	
		
	foundDevice = ESPMeteringTable_GetDeviceTableByIndex(index);
	
	if(foundDevice == NULL)
		return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;
	
	if( memcmp( (BYTE *) &foundDevice->deviceItem, deviceItem, sizeof(DEV_LIST))){		
		
		return ESPMeteringTable_SetupDeviceTableItemByIndex(index, deviceItem);
	}
	
	return DATA_BASE_HANDLER_NO_ERROR_CODE;
}


BYTE ESPMeteringTable_SaveDeviceTableItem(DEV_LIST_PTR deviceItem){
	
	INT16 index;		
	INT16 error_code;
	
	index = ESPMeteringTable_FindDeviceTableIndexByMACAddress(deviceItem->MAC);
			
	if(index == DATA_BASE_HANDLER_MAC_ADDRESS_NOT_FOUND_ERROR_CODE){		
		
		error_code = ESPMeteringTable_AddNewDeviceTableItem(deviceItem);
		
		if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
			return 0;
		
		// NOTE: It used to call vfnSaveIndexDev();
                wDevicesIndex++;
		return 1;
	}
	
	error_code = ESPMeteringTable_UpdateDeviceTableItemByIndex(index, deviceItem);
	
	if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
		return 0;
	
	// NOTE: It used to call vfnSaveIndexDev();		
	return 1;	
}

INT16 ESPMeteringTable_AddNewReadingTableItemByIndex(INT16 index, READING_LIST_PTR readingItem){
	
	WORD address;
	
	address = (CAB_READ_Readings_ADD + (index * sizeof(READING_LIST)));	
	
	//! API_MEM24_1025_I2C_Write( (BYTE *) readingItem, address, sizeof(READING_LIST));	
        MEM_EEPROM_Write(  address, (BYTE *) readingItem, sizeof(READING_LIST));	
	
	return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

BYTE ESPMeteringTable_SaveReadingTableItem(READING_LIST_PTR readingItem){
	
	INT16 index;		
	INT16 error_code;
	
	index = ESPMeteringTable_FindMeterTableIndexBySerialNumber(readingItem->Serial_Num);
			
	if(index == DATA_BASE_HANDLER_MAC_ADDRESS_NOT_FOUND_ERROR_CODE){		
	
		return 0;
	}
	
	error_code = ESPMeteringTable_AddNewReadingTableItemByIndex(index, readingItem);
	
	if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
		return 0;
	
		
	return 1;	
}

BYTE API_ESPMeteringTable_SaveTable(BYTE bTableType, BYTE *vptrTableStructure ) {			
	
    switch(bTableType){

            case METERSTABLE:			

                    return ESPMeteringTable_SaveMeterTableItem((MTR_LIST_PTR) vptrTableStructure);	

            case DEVICESTABLE:		

                    return ESPMeteringTable_SaveDeviceTableItem((DEV_LIST_PTR) vptrTableStructure);

            case READINGSTABLE:

                    return ESPMeteringTable_SaveReadingTableItem((READING_LIST_PTR) vptrTableStructure);			
    }

    return 0;
}

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************
BYTE bfnReadMTRSTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    WORD wIndexData;    
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    wIndexData = wfnIndexConsutl(METERSTABLE);

    if (wIndexData) {

        * pagingDataResponseSize = (wIndexData * sizeof (MTR_LIST));
        
        //! bfnBuffer_Table_Meter(wIndexData, sizeof(MTR_LIST), &bpOutputData[0]); 
        bfnBuffer_Table_Meter(wIndexData, METER_TABLE_LIST);
        
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnReadDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    WORD wIndexData;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    wIndexData = wfnIndexConsutl(DEVICESTABLE);

    if (wIndexData) {

        * pagingDataResponseSize = (wIndexData * sizeof (DEV_LIST));
        bfnBuffer_Table_Meter(wIndexData, DEVICE_TABLE_LIST);
        
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    WORD wIndexData;
    WORD wDataSize;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    wIndexData = wfnIndexConsutl(METERSTABLE);

    if (wIndexData) {

        * pagingDataResponseSize = (wIndexData * sizeof (READING_LIST));
        
        //! bfnBuffer_Table_Meter(wIndexData, sizeof(MTR_LIST), &bpOutputData[0]); 
        bfnBuffer_Table_Meter(wIndexData, READING_TABLE_LIST);
        
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnDelMTRMetersTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    //! It needs to implement a better command process
    //! also the response needs to be performed

    if (bfnDelAllData(METERSTABLE, ZERO)) {

        * dataResponse_ptr = SUCCESS_CMD;
    } else {

        * dataResponse_ptr = COOR_BUSY;
    }

    * dataResponseSize = sizeof (BYTE);

    return ESP_DOES_NOT_WAIT_ANSWER;
}

BYTE bfnDelMTRDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    //! It needs to implement a better command process
    //! also the response needs to be performed

    if (bfnDelAllData(DEVICESTABLE, ZERO)) {

        * dataResponse_ptr = SUCCESS_CMD;
    } else {

        * dataResponse_ptr = COOR_BUSY;
    }

    * dataResponseSize = sizeof (BYTE);

    return ESP_DOES_NOT_WAIT_ANSWER;

}


//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void vfn_tBufferLocalDriver(void){
    
    vfnaBufferDriverState[_tBufferSM.bActualState]();
}

void ESPMeteringTable_SetStateMachine(BYTE actualState, BYTE nextState) {

    _tBufferSM.bActualState = actualState;
    _tBufferSM.bNextState = nextState;
    
    vfnEventEnable(BUFFER_EVENT);
    vfnEventPost(BUFFER_EVENT);
}

void ESPMeteringTable_ErrorProcess(void){
    
    DataBaseHandler_ClearQuery(&espMeteringTableQuery);
    ESPMeteringTable_SetStateMachine(_BUFFER_END_STATE, _BUFFER_END_STATE);    
}

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void vfnBufferIdleState(void){
    
}

void vfnBufferConsultTableState(void){
    
    BYTE bStatusData;    
    
    WORD quantityOfItems = DataBaseHandler_GetQuantityOfItems(&espMeteringTableQuery);
    WORD startItem = DataBaseHandler_GetStartItem(&espMeteringTableQuery); 
    DATA_BASE_HANDLER_LIST_TYPE tableListType = DataBaseHandler_GetTableListType(&espMeteringTableQuery);
    BOOL isWaitingForQueryResponse = DataBaseHandler_IsWaitingForQueryResponse(&espMeteringTableQuery);
    
    if(isWaitingForQueryResponse){
    
        return;
    }
    
    if(DataBaseHandler_GetQueryResponseBufferSize(&espMeteringTableQuery)){
            
        startItem++;
        quantityOfItems--;
        
        //!print_info("Paging Data: ");
        //!print_buffer(DataBaseHandler_GetQueryResponseBuffer(&espMeteringTableQuery), DataBaseHandler_GetQueryResponseBufferSize(&espMeteringTableQuery));
        ESPComInterface_SendFrame(0, DataBaseHandler_GetQueryResponseBuffer(&espMeteringTableQuery), DataBaseHandler_GetQueryResponseBufferSize(&espMeteringTableQuery), TRUE, FALSE, quantityOfItems == 0 );
        
        DataBaseHandler_SetupQuery(&espMeteringTableQuery,startItem,quantityOfItems,tableListType);                
    }
    
    if( quantityOfItems == 0){
     
        print_warm("quantityOfItems = 0");
        ESPMeteringTable_ErrorProcess();
        return;
    }
    
    /*if( startItem >= NUM_MAX_METERS){
     
        print_error("startItem >= NUM_MAX_METERS");
        ESPMeteringTable_ErrorProcess();
        return;
    }*/
    
    switch(tableListType){
        
        case METER_TABLE_LIST:
            bStatusData = bfnConsultData(ALLDATAMTR, ZERO, ZERO , startItem, &espMeteringTableQuery);
            break;
        
        case DEVICE_TABLE_LIST:
            
            bStatusData = bfnConsultData(ALLDATADEV, ZERO, startItem, ZERO , &espMeteringTableQuery);
            break;
            
        case READING_TABLE_LIST:
            
            bStatusData = bfnConsultData(ALLDATAREADINGS, ZERO, ZERO, startItem , &espMeteringTableQuery);
            break;
            
        default:
            print_error("NO VALID TABLE_LIST_TYPE");
            ESPMeteringTable_ErrorProcess();
            return;
    }
    
    if(!bStatusData) {
        
        print_error("There is not a index inside DataBase");
        ESPMeteringTable_ErrorProcess();
        return;
    }   
}

void vfnBufferTransmitTableState(void){
    
}

void vfnBufferEndState(void){
    
    vfnEventClear(BUFFER_EVENT);                                        
    vfnEventDisable(BUFFER_EVENT);
}
