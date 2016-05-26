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

INT16 ESPMeteringTable_GetIndexRecord(DBMS_HANDLER_TABLE_ID tableId){
    
    INT16 indexRecord;
    WORD indexRecordAddress;
    
    indexRecordAddress = DBMSHandler_GetTableAddressByTableId(tableId);
    
    if(indexRecordAddress == DBMS_HANDLER_NOT_INIT)
        return ESP_METERING_TABLE_ADDRESS_NOT_FOUND_ERROR_CODE;
    
    DBMSHandler_ReadRecord(tableId, indexRecordAddress, (BYTE *) &indexRecord, sizeof(indexRecord) );
    
    return indexRecord;
}

INT16 ESPMeteringTable_SetIndexRecord(DBMS_HANDLER_TABLE_ID tableId, INT16 meterIndexRecord){
    
    WORD indexRecordAddress;
    
    indexRecordAddress = DBMSHandler_GetTableAddressByTableId(tableId);
    
    if(indexRecordAddress == DBMS_HANDLER_NOT_INIT)
        return ESP_METERING_TABLE_ADDRESS_NOT_FOUND_ERROR_CODE;
    
    DBMSHandler_WriteRecord(tableId, indexRecordAddress, (BYTE *) &meterIndexRecord, sizeof(meterIndexRecord) );
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 ESPMeteringTable_GetMeterIndex(void){
    
    return ESPMeteringTable_GetIndexRecord(METER_INDEX_TABLE_ID);
}

INT16 ESPMeteringTable_SetMeterIndex(INT16 meterIndexRecord){
    
    return ESPMeteringTable_SetIndexRecord(METER_INDEX_TABLE_ID, meterIndexRecord);
}

INT16 ESPMeteringTable_GetDeviceIndex(void){
    
    return ESPMeteringTable_GetIndexRecord(DEVICE_INDEX_TABLE_ID);
}

INT16 ESPMeteringTable_SetDeviceIndex(INT16 deviceIndexRecord){
    
    return ESPMeteringTable_SetIndexRecord(DEVICE_INDEX_TABLE_ID, deviceIndexRecord);
}

INT16 ESPMeteringTable_FindMeterTableIndexBySerialNumber(BYTE * serialNumber){
	
    MTR_LIST meter; 
    WORD meterAddress;    
    INT16 index = 0;

    meterAddress = DBMSHandler_GetTableAddressByTableId(METER_TABLE_ID);
    
    while(index < NUM_MAX_METERS){

        DBMSHandler_ReadRecord(METER_TABLE_ID, meterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
        
        if(!memcmp(meter.Serial_Num, serialNumber, ID_METER_SIZE))
            break;
        
        meterAddress += sizeof(MTR_LIST);
        index++;
    }

    if(index == NUM_MAX_METERS)
        return DATA_BASE_HANDLER_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE;

    return index;
}

INT16 ESPMeteringTable_FindAvailableMeterTableIndex(void){
	
    MTR_LIST meter;
    WORD meterAddress;    
    INT16 index = 0;

    meterAddress = DBMSHandler_GetTableAddressByTableId(METER_TABLE_ID);
    
    while(index < NUM_MAX_METERS) {

        DBMSHandler_ReadRecord(METER_TABLE_ID, meterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
        
        if(!meter.Type)
            break;

        meterAddress += sizeof(MTR_LIST);
        index++;
    }
	
    if(index == NUM_MAX_METERS)
	return DATA_BASE_HANDLER_NUM_MAX_METERS_REACHED_ERROR_CODE;
		
    return index;
}

INT16 ESPMeteringTable_SetupMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem){
	
    WORD meterAddress = ESPMeteringTable_GetMeterTableAddressByIndex(index);
	
    if (meterAddress == DBMS_HANDLER_NOT_INIT)
        return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;
	
    // Clear Meter_Eneri Struct
    /*
        memset(meter_ptr, 0, sizeof(Meter_Eneri));

        memcpy((BYTE *) &meter_ptr->meterItem, meterItem, sizeof(MTR_LIST)); 		
        meter_ptr->Address = (CAB_READ_Meter_ADD + (index * sizeof(Meter_Eneri)));	
        meter_ptr->CRC = wfnCRC16( (BYTE *) meter_ptr, sizeof(Meter_Eneri) - sizeof(WORD));	
        //error_code = API_MEM24_1025_I2C_Write( (BYTE *) meter_ptr, meter_ptr->Address, sizeof(Meter_Eneri));	
        MEM_EEPROM_Write( meter_ptr->Address, (BYTE *) meter_ptr,  sizeof(Meter_Eneri));	
    */
    
    DBMSHandler_WriteRecord(METER_TABLE_ID, meterAddress, (BYTE *) &meterItem, sizeof(MTR_LIST));
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

WORD ESPMeteringTable_GetMeterTableAddressByIndex(INT16 index){
	
    if (index >= NUM_MAX_METERS)
	return DBMS_HANDLER_NOT_INIT;
	
    return DBMSHandler_GetTableIndexAddressByTableId(METER_TABLE_ID, index);    
}

INT16 ESPMeteringTable_UpdateMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem){
	
    WORD foundMeterAddress;
    MTR_LIST meter;
    
    foundMeterAddress = ESPMeteringTable_GetMeterTableAddressByIndex(index);

    if(foundMeterAddress == DBMS_HANDLER_NOT_INIT)
        return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_ReadRecord(METER_TABLE_ID, foundMeterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
    
    if( memcmp((BYTE *) &meter, meterItem, sizeof(MTR_LIST))){		
        
        print_info("Updating Meter Table");
        return ESPMeteringTable_SetupMeterTableItemByIndex(index, meterItem);
    }

    print_info("Meter Table Index did not change");
    return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

BYTE ESPMeteringTable_SaveMeterTableItem(MTR_LIST_PTR meterItem){
	
    INT16 index;		
    INT16 error_code;
    INT16 meterIndex; 

    index = ESPMeteringTable_FindMeterTableIndexBySerialNumber(meterItem->Serial_Num);
    meterIndex = ESPMeteringTable_GetMeterIndex();
    
    if(index == DATA_BASE_HANDLER_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE){		

        error_code = ESPMeteringTable_AddNewMeterTableItem(meterItem);

        if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
            return 0;        
        
        meterIndex++;
        ESPMeteringTable_SetMeterIndex(meterIndex);
        
        return NEW_MTR_ADD;
    }

    error_code = ESPMeteringTable_UpdateMeterTableItemByIndex(index, meterItem);

    if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
        return 0;

    return NO_NEW_MTR_ADD;	
}

INT16 ESPMeteringTable_FindDeviceTableIndexByMACAddress(BYTE * macAddress){
	
    //Device_Eneri_PTR device_ptr = tsDevice;	
    DEV_LIST device;
    WORD deviceAddress;
    INT16 index = 0;
	
    deviceAddress = DBMSHandler_GetTableAddressByTableId(DEVICE_TABLE_ID);
    
    while(index < NUM_MAX_NODES){
        
        DBMSHandler_ReadRecord(DEVICE_TABLE_ID, deviceAddress, (BYTE *) &device, sizeof(DEV_LIST));
		
        if(!memcmp(device.MAC, macAddress, MAC_SIZE))
            break;
		
        deviceAddress += sizeof(DEV_LIST);
        index++;
    }
	
    if(index == NUM_MAX_NODES)
        return DATA_BASE_HANDLER_MAC_ADDRESS_NOT_FOUND_ERROR_CODE;
		
    return index;
}

INT16 ESPMeteringTable_FindAvailableDeviceTableIndex(void){
	
    DEV_LIST device;
    WORD deviceAddress;
    INT16 index = 0;
	
    deviceAddress = DBMSHandler_GetTableAddressByTableId(DEVICE_TABLE_ID);
    
    while(index < NUM_MAX_NODES){
		
        DBMSHandler_ReadRecord(DEVICE_TABLE_ID, deviceAddress, (BYTE *) &device, sizeof(DEV_LIST));
        
        if(!device.Type)
            break;
			
        deviceAddress += sizeof(DEV_LIST);
        index++;
    }
	
    if(index == NUM_MAX_NODES)
        return DATA_BASE_HANDLER_NUM_MAX_NODES_REACHED_ERROR_CODE;
		
    return index;
}

WORD ESPMeteringTable_GetDeviceTableAddressByIndex(INT16 index){
	
    if (index >= NUM_MAX_NODES)
            return DBMS_HANDLER_NOT_INIT;

    return DBMSHandler_GetTableIndexAddressByTableId(DEVICE_TABLE_ID, index);
}

INT16 ESPMeteringTable_SetupDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem){
	
	WORD deviceAddress = ESPMeteringTable_GetDeviceTableAddressByIndex(index);
	
	if (deviceAddress == DBMS_HANDLER_NOT_INIT)
		return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;
	
        /*
	// Clear Meter_Eneri Struct
	memset(device_ptr, 0, sizeof(Device_Eneri));
		
	memcpy( (BYTE *) &device_ptr->deviceItem, deviceItem, sizeof(DEV_LIST)); 		
	device_ptr->Address = (CAB_READ_Meter_MAC_SHORT + (index * sizeof(Device_Eneri)));	
	device_ptr->CRC = wfnCRC16( (BYTE *) device_ptr, sizeof(Device_Eneri) - sizeof(WORD));	
	//!API_MEM24_1025_I2C_Write( (BYTE *) device_ptr, device_ptr->Address, sizeof(Device_Eneri));	
	MEM_EEPROM_Write(  device_ptr->Address, (BYTE *) device_ptr, sizeof(Device_Eneri));	
        */
        DBMSHandler_WriteRecord(DEVICE_TABLE_ID, deviceAddress, (BYTE *) deviceItem, sizeof(DEV_LIST));
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
	
    WORD foundDeviceAddress;
    DEV_LIST device;

    foundDeviceAddress = ESPMeteringTable_GetDeviceTableAddressByIndex(index);

    if(foundDeviceAddress == DBMS_HANDLER_NOT_INIT)
        return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_ReadRecord(DEVICE_TABLE_ID, foundDeviceAddress, (BYTE *) &device, sizeof(DEV_LIST));

    if( memcmp( (BYTE *) &device, deviceItem, sizeof(DEV_LIST))){		

        return ESPMeteringTable_SetupDeviceTableItemByIndex(index, deviceItem);
    }

    return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

BYTE ESPMeteringTable_SaveDeviceTableItem(DEV_LIST_PTR deviceItem){
	
    INT16 index;		
    INT16 error_code;
    INT16 deviceIndex;

    index = ESPMeteringTable_FindDeviceTableIndexByMACAddress(deviceItem->MAC);
    deviceIndex = ESPMeteringTable_GetDeviceIndex();

    if(index == DATA_BASE_HANDLER_MAC_ADDRESS_NOT_FOUND_ERROR_CODE){		

        error_code = ESPMeteringTable_AddNewDeviceTableItem(deviceItem);

        if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
            return 0;

        deviceIndex++;
        ESPMeteringTable_SetDeviceIndex(deviceIndex);
        
        return 1;
    }

    error_code = ESPMeteringTable_UpdateDeviceTableItemByIndex(index, deviceItem);

    if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
        return 0;

    // NOTE: It used to call vfnSaveIndexDev();		
    return 1;	
}

WORD ESPMeteringTable_GetReadingTableAddressByIndex(INT16 index){
    
    if (index >= NUM_MAX_METERS)
            return DBMS_HANDLER_NOT_INIT;

    return DBMSHandler_GetTableIndexAddressByTableId(READING_TABLE_ID, index);    
}

INT16 ESPMeteringTable_AddNewReadingTableItemByIndex(INT16 index, READING_LIST_PTR readingItem){
	
    WORD readingAddress;

    readingAddress = ESPMeteringTable_GetReadingTableAddressByIndex(index);

    if (readingAddress == DBMS_HANDLER_NOT_INIT)
        return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_WriteRecord(READING_TABLE_ID, readingAddress, (BYTE *) readingItem, sizeof(READING_LIST));

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

// Not long support
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
