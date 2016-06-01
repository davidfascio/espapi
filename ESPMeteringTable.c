//******************************************************************************
// Includes
//******************************************************************************
#include "ESPMeteringTable.h"

//******************************************************************************
// Variables
//******************************************************************************
DBMS_HANDLER_RECORD_QUERY espMeteringTableRecordQuery;

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void ESPMeteringTable_IdleState(void);
void ESPMeteringTable_SelectQueryState(void);
void ESPMeteringTable_EndState(void);

typedef enum {
    ESP_METERING_TABLE_IDLE_STATE = 0,
    ESP_METERING_TABLE_SELECT_QUERY_STATE,
    ESP_METERING_TABLE_END_STATE
} ESPMeteringTableDriverStates;

void (*const ESPMeteringTablerDriverFunctionStates[])(void) =
{
    ESPMeteringTable_IdleState,
    ESPMeteringTable_SelectQueryState,    
    ESPMeteringTable_EndState    
};

sSM ESPMeteringTableSM = {  ESP_METERING_TABLE_IDLE_STATE,               /* bActualState     */
                            ESP_METERING_TABLE_IDLE_STATE,               /* bNextState       */
                            ESP_METERING_TABLE_IDLE_STATE,               /* bPrevState       */
                            ESP_METERING_TABLE_IDLE_STATE};              /* bErrorState      */

//******************************************************************************
// DataBaseHandler Function Prototypes
//******************************************************************************
BYTE ESPMeteringTable_SelectQuery(WORD quantityOfRecords, DBMS_HANDLER_TABLE_ID tableId) {

    switch (tableId) {

        case METER_TABLE_ID:
        case READING_TABLE_ID:
        case DEVICE_TABLE_ID:
            break;

        default:
            return FALSE;
    }
    
    DBMSHandler_SetupRecordQuery(&espMeteringTableRecordQuery,tableId, 0, quantityOfRecords);
    ESPMeteringTable_SetStateMachine(ESP_METERING_TABLE_SELECT_QUERY_STATE, ESP_METERING_TABLE_END_STATE);
    
    return TRUE;
}

//******************************************************************************
// DataBaseHandler Function Prototypes
//******************************************************************************

INT16 ESPMeteringTable_GetRecordIndex(DBMS_HANDLER_TABLE_ID tableId){
    
    INT16 indexRecord;
    WORD indexRecordAddress;
    
    indexRecordAddress = DBMSHandler_GetTableAddressByTableId(tableId);
    
    if(indexRecordAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_ADDRESS_NOT_FOUND_ERROR_CODE;
    
    DBMSHandler_ReadRecordByAddress(tableId, indexRecordAddress, (BYTE *) &indexRecord, sizeof(indexRecord) );
    
    return indexRecord;
}

INT16 ESPMeteringTable_SetRecordIndex(DBMS_HANDLER_TABLE_ID tableId, INT16 meterIndexRecord){
    
    WORD indexRecordAddress;
    
    indexRecordAddress = DBMSHandler_GetTableAddressByTableId(tableId);
    
    if(indexRecordAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_ADDRESS_NOT_FOUND_ERROR_CODE;
    
    DBMSHandler_WriteRecordByAddress(tableId, indexRecordAddress, (BYTE *) &meterIndexRecord, sizeof(meterIndexRecord) );
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 ESPMeteringTable_GetMeterIndex(void){
    
    return ESPMeteringTable_GetRecordIndex(METER_INDEX_TABLE_ID);
}

INT16 ESPMeteringTable_SetMeterIndex(INT16 meterIndexRecord){
    
    return ESPMeteringTable_SetRecordIndex(METER_INDEX_TABLE_ID, meterIndexRecord);
}

INT16 ESPMeteringTable_GetDeviceIndex(void){
    
    return ESPMeteringTable_GetRecordIndex(DEVICE_INDEX_TABLE_ID);
}

INT16 ESPMeteringTable_SetDeviceIndex(INT16 deviceIndexRecord){
    
    return ESPMeteringTable_SetRecordIndex(DEVICE_INDEX_TABLE_ID, deviceIndexRecord);
}

INT16 ESPMeteringTable_FindMeterTableIndexByRecordIndex(INT16 recordIndex){
	
    MTR_LIST meter;
    WORD meterAddress;    
    INT16 index = 0;
    INT16 recordIndexCounter = -1;

    meterAddress = DBMSHandler_GetTableAddressByTableId(METER_TABLE_ID);
    
    while(index < NUM_MAX_METERS) {

        DBMSHandler_ReadRecordByAddress(METER_TABLE_ID, meterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
        
        if(meter.Type)         
            recordIndexCounter++;     
        
        if(recordIndexCounter == recordIndex)
            break;        

        meterAddress += sizeof(MTR_LIST);
        index++;
    }
	
    if(index == NUM_MAX_METERS)
	return ESP_METERING_TABLE_NUM_MAX_METERS_REACHED_ERROR_CODE;
		
    return index;
}

INT16 ESPMeteringTable_FindMeterTableIndexBySerialNumber(BYTE * serialNumber){
	
    MTR_LIST meter; 
    WORD meterAddress;    
    INT16 index = 0;

    meterAddress = DBMSHandler_GetTableAddressByTableId(METER_TABLE_ID);
    
    while(index < NUM_MAX_METERS){

        DBMSHandler_ReadRecordByAddress(METER_TABLE_ID, meterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
        
        if(!memcmp(meter.Serial_Num, serialNumber, ID_METER_SIZE))
            break;
        
        meterAddress += sizeof(MTR_LIST);
        index++;
    }

    if(index == NUM_MAX_METERS)
        return ESP_METERING_TABLE_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE;

    return index;
}

INT16 ESPMeteringTable_FindAvailableMeterTableIndex(void){
	
    MTR_LIST meter;
    WORD meterAddress;    
    INT16 index = 0;

    meterAddress = DBMSHandler_GetTableAddressByTableId(METER_TABLE_ID);
    
    while(index < NUM_MAX_METERS) {

        DBMSHandler_ReadRecordByAddress(METER_TABLE_ID, meterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
        
        if(!meter.Type)
            break;

        meterAddress += sizeof(MTR_LIST);
        index++;
    }
	
    if(index == NUM_MAX_METERS)
	return ESP_METERING_TABLE_NUM_MAX_METERS_REACHED_ERROR_CODE;
		
    return index;
}

INT16 ESPMeteringTable_InsertMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem){
	
    WORD meterAddress = ESPMeteringTable_GetMeterTableAddressByIndex(index);
	
    if (meterAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;
	
    DBMSHandler_WriteRecordByAddress(METER_TABLE_ID, meterAddress, (BYTE *) meterItem, sizeof(MTR_LIST));
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 ESPMeteringTable_InsertMeterTableItem(MTR_LIST_PTR meterItem){
	
    INT16 index;

    index = ESPMeteringTable_FindAvailableMeterTableIndex();	

    if(index == ESP_METERING_TABLE_NUM_MAX_METERS_REACHED_ERROR_CODE)	
        return index;
    
    print_info("Adding New Meter Table");
    return ESPMeteringTable_InsertMeterTableItemByIndex(index, meterItem);
}

WORD ESPMeteringTable_GetMeterTableAddressByIndex(INT16 index){
	
    if (index >= NUM_MAX_METERS)
	return DBMS_HANDLER_NULL_ADDRESS;
	
    return DBMSHandler_GetTableIndexAddressByTableId(METER_TABLE_ID, index);    
}

INT16 ESPMeteringTable_UpdateMeterTableItemByIndex(INT16 index, MTR_LIST_PTR meterItem){
	
    WORD foundMeterAddress;
    MTR_LIST meter;
    
    foundMeterAddress = ESPMeteringTable_GetMeterTableAddressByIndex(index);

    if(foundMeterAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_ReadRecordByAddress(METER_TABLE_ID, foundMeterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
    
    if( memcmp((BYTE *) &meter, meterItem, sizeof(MTR_LIST))){		
        
        print_info("Updating Meter Table");
        return ESPMeteringTable_InsertMeterTableItemByIndex(index, meterItem);
    }

    print_info("Meter Table Index did not change");
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 API_ESPMeteringTable_SelectMeterTableItemByRecordIndex(INT16 recordIndex, DBMS_HANDLER_RECORD_QUERY_PTR recordQuery ){
    
    INT16 locationIndex;		    
    INT16 meterIndex; 
    WORD  meterAddress;
    
    meterIndex = ESPMeteringTable_GetMeterIndex();  
    
    if(recordIndex > meterIndex)
        return ESP_METERING_TABLE_RECORD_INDEX_OUT_OF_RANGE_ERROR_CODE;
    
    locationIndex = ESPMeteringTable_FindMeterTableIndexByRecordIndex(recordIndex);
        
    if( locationIndex == ESP_METERING_TABLE_NUM_MAX_METERS_REACHED_ERROR_CODE )
        return locationIndex;  
    
    meterAddress = ESPMeteringTable_GetMeterTableAddressByIndex(locationIndex);
	
    if (meterAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;
	   
    DBMSHandler_SetRecordQueryResponseSize(recordQuery, sizeof(MTR_LIST));
    
    DBMSHandler_ReadRecordByAddress( METER_TABLE_ID, meterAddress, 
                            (BYTE *) DBMSHandler_GetRecordQueryResponse(recordQuery), 
                            DBMSHandler_GetRecordQueryResponseSize(recordQuery));
            
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 API_ESPMeteringTable_InsertMeterTableItem(MTR_LIST_PTR meterItem){
	
    INT16 index;		
    INT16 error_code;
    INT16 meterIndex; 

    index = ESPMeteringTable_FindMeterTableIndexBySerialNumber(meterItem->Serial_Num);
    meterIndex = ESPMeteringTable_GetMeterIndex();
    
    if(index == ESP_METERING_TABLE_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE){		

        error_code = ESPMeteringTable_InsertMeterTableItem(meterItem);

        if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
            return error_code;        
        
        meterIndex++;
        ESPMeteringTable_SetMeterIndex(meterIndex);
        
        return ESP_METERING_TABLE_NEW_MTR_ADD;
    }

    error_code = ESPMeteringTable_UpdateMeterTableItemByIndex(index, meterItem);

    if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
        return error_code;

    return ESP_METERING_TABLE_UPDATE_MTR_ADD;	
}

INT16 ESPMeteringTable_DeleteMeterTableItemByIndex(INT16 index){
    
    WORD meterAddress = ESPMeteringTable_GetMeterTableAddressByIndex(index);
	
    if (meterAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_DeleteRecordByAddress(METER_TABLE_ID, meterAddress, sizeof(MTR_LIST));
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 API_ESPMeteringTable_DropMeterTable(void){
    
    INT16 index;
        
    for(index = 0; index < NUM_MAX_METERS; index++ )
        ESPMeteringTable_DeleteMeterTableItemByIndex(index);
    
    return ESPMeteringTable_SetMeterIndex(0);
}

INT16 ESPMeteringTable_FindDeviceTableIndexByRecordIndex(INT16 recordIndex){
    
    DEV_LIST device;
    WORD deviceAddress;    
    INT16 index = 0;
    INT16 recordIndexCounter = -1;

    deviceAddress = DBMSHandler_GetTableAddressByTableId(DEVICE_TABLE_ID);
    
    while(index < NUM_MAX_NODES) {

        DBMSHandler_ReadRecordByAddress(DEVICE_TABLE_ID, deviceAddress, (BYTE *) &device, sizeof(DEV_LIST));
        
        if(device.Type)         
            recordIndexCounter++;     
        
        if(recordIndexCounter == recordIndex)
            break;        

        deviceAddress += sizeof(DEV_LIST);
        index++;
    }
	
    if(index == NUM_MAX_NODES)
	return ESP_METERING_TABLE_NUM_MAX_NODES_REACHED_ERROR_CODE;
		
    return index;
}

INT16 ESPMeteringTable_FindDeviceTableIndexByMACAddress(BYTE * macAddress){
	
    DEV_LIST device;
    WORD deviceAddress;
    INT16 index = 0;
	
    deviceAddress = DBMSHandler_GetTableAddressByTableId(DEVICE_TABLE_ID);
    
    while(index < NUM_MAX_NODES){
        
        DBMSHandler_ReadRecordByAddress(DEVICE_TABLE_ID, deviceAddress, (BYTE *) &device, sizeof(DEV_LIST));
		
        if(!memcmp(device.MAC, macAddress, MAC_SIZE))
            break;
		
        deviceAddress += sizeof(DEV_LIST);
        index++;
    }
	
    if(index == NUM_MAX_NODES)
        return ESP_METERING_TABLE_MAC_ADDRESS_NOT_FOUND_ERROR_CODE;
		
    return index;
}

INT16 ESPMeteringTable_FindAvailableDeviceTableIndex(void){
	
    DEV_LIST device;
    WORD deviceAddress;
    INT16 index = 0;
	
    deviceAddress = DBMSHandler_GetTableAddressByTableId(DEVICE_TABLE_ID);
    
    while(index < NUM_MAX_NODES){
		
        DBMSHandler_ReadRecordByAddress(DEVICE_TABLE_ID, deviceAddress, (BYTE *) &device, sizeof(DEV_LIST));
        
        if(!device.Type)
            break;
			
        deviceAddress += sizeof(DEV_LIST);
        index++;
    }
	
    if(index == NUM_MAX_NODES)
        return ESP_METERING_TABLE_NUM_MAX_NODES_REACHED_ERROR_CODE;
		
    return index;
}

WORD ESPMeteringTable_GetDeviceTableAddressByIndex(INT16 index){
	
    if (index >= NUM_MAX_NODES)
            return DBMS_HANDLER_NULL_ADDRESS;

    return DBMSHandler_GetTableIndexAddressByTableId(DEVICE_TABLE_ID, index);
}

INT16 ESPMeteringTable_InsertDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem){
	
	WORD deviceAddress = ESPMeteringTable_GetDeviceTableAddressByIndex(index);
	
	if (deviceAddress == DBMS_HANDLER_NULL_ADDRESS)
		return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;
	
        DBMSHandler_WriteRecordByAddress(DEVICE_TABLE_ID, deviceAddress, (BYTE *) deviceItem, sizeof(DEV_LIST));
	return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 ESPMeteringTable_InsertDeviceTableItem(DEV_LIST_PTR deviceItem){
	
	INT16 index;
	
	index = ESPMeteringTable_FindAvailableDeviceTableIndex();	
	
	if(index == ESP_METERING_TABLE_NUM_MAX_NODES_REACHED_ERROR_CODE)	
            return index;
		
        print_info("Adding New Device Table");
	return ESPMeteringTable_InsertDeviceTableItemByIndex(index, deviceItem);
}


INT16 ESPMeteringTable_UpdateDeviceTableItemByIndex(INT16 index, DEV_LIST_PTR deviceItem){
	
    WORD foundDeviceAddress;
    DEV_LIST device;

    foundDeviceAddress = ESPMeteringTable_GetDeviceTableAddressByIndex(index);

    if(foundDeviceAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_ReadRecordByAddress(DEVICE_TABLE_ID, foundDeviceAddress, (BYTE *) &device, sizeof(DEV_LIST));

    if( memcmp( (BYTE *) &device, deviceItem, sizeof(DEV_LIST))){		

        return ESPMeteringTable_InsertDeviceTableItemByIndex(index, deviceItem);
    }

    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 API_ESPMeteringTable_SelectDeviceTableItemByRecordIndex(INT16 recordIndex, DBMS_HANDLER_RECORD_QUERY_PTR recordQuery){
    
    INT16 locationIndex;		    
    INT16 deviceIndex; 
    WORD  deviceAddress;
    
    deviceIndex = ESPMeteringTable_GetDeviceIndex();  
    
    if(recordIndex > deviceIndex)
        return ESP_METERING_TABLE_RECORD_INDEX_OUT_OF_RANGE_ERROR_CODE;
    
    locationIndex = ESPMeteringTable_FindDeviceTableIndexByRecordIndex(recordIndex);
        
    if( locationIndex == ESP_METERING_TABLE_NUM_MAX_NODES_REACHED_ERROR_CODE )
        return locationIndex;  
    
    deviceAddress = ESPMeteringTable_GetDeviceTableAddressByIndex(locationIndex);
	
    if (deviceAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;
	     
    DBMSHandler_SetRecordQueryResponseSize(recordQuery, sizeof(DEV_LIST));
    
    DBMSHandler_ReadRecordByAddress( DEVICE_TABLE_ID, deviceAddress, 
                            (BYTE *) DBMSHandler_GetRecordQueryResponse(recordQuery), 
                            DBMSHandler_GetRecordQueryResponseSize(recordQuery));
        
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 API_ESPMeteringTable_InsertDeviceTableItem(DEV_LIST_PTR deviceItem){
	
    INT16 index;		
    INT16 error_code;
    INT16 deviceIndex;

    index = ESPMeteringTable_FindDeviceTableIndexByMACAddress(deviceItem->MAC);
    deviceIndex = ESPMeteringTable_GetDeviceIndex();

    if(index == ESP_METERING_TABLE_MAC_ADDRESS_NOT_FOUND_ERROR_CODE){		

        error_code = ESPMeteringTable_InsertDeviceTableItem(deviceItem);

        if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
            return error_code;

        deviceIndex++;
        ESPMeteringTable_SetDeviceIndex(deviceIndex);
        
        return ESP_METERING_TABLE_NEW_DEV_ADD;
    }

    error_code = ESPMeteringTable_UpdateDeviceTableItemByIndex(index, deviceItem);

    if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
        return error_code;
	
    return ESP_METERING_TABLE_UPDATE_DEV_ADD;	
}

INT16 ESPMeteringTable_DeleteDeviceTableItemByIndex(INT16 index){
    
    WORD deviceAddress = ESPMeteringTable_GetDeviceTableAddressByIndex(index);
	
    if (deviceAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_DeleteRecordByAddress(DEVICE_TABLE_ID, deviceAddress, sizeof(DEV_LIST));
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 API_ESPMeteringTable_DropDeviceTable(void){
    
    INT16 index;
        
    for(index = 0; index < NUM_MAX_NODES; index++ )
        ESPMeteringTable_DeleteDeviceTableItemByIndex(index);
    
    return ESPMeteringTable_SetDeviceIndex(0);
}

WORD ESPMeteringTable_GetReadingTableAddressByIndex(INT16 index){
    
    if (index >= NUM_MAX_METERS) 
            return DBMS_HANDLER_NULL_ADDRESS;

    return DBMSHandler_GetTableIndexAddressByTableId(READING_TABLE_ID, index);    
}

INT16 ESPMeteringTable_InsertReadingTableItemByIndex(INT16 index, READING_LIST_PTR readingItem){
	
    WORD readingAddress;

    readingAddress = ESPMeteringTable_GetReadingTableAddressByIndex(index);

    if (readingAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_WriteRecordByAddress(READING_TABLE_ID, readingAddress, (BYTE *) readingItem, sizeof(READING_LIST));

    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 API_ESPMeteringTable_SelectReadingTableItemByRecordIndex(INT16 recordIndex, DBMS_HANDLER_RECORD_QUERY_PTR recordQuery){
    
    INT16 locationIndex;		    
    INT16 readingIndex; 
    WORD  readingAddress;
    
    readingIndex = ESPMeteringTable_GetMeterIndex();  
    
    if(recordIndex > readingIndex)
        return ESP_METERING_TABLE_RECORD_INDEX_OUT_OF_RANGE_ERROR_CODE;
    
    locationIndex = ESPMeteringTable_FindMeterTableIndexByRecordIndex(recordIndex);
        
    if( locationIndex == ESP_METERING_TABLE_NUM_MAX_METERS_REACHED_ERROR_CODE )
        return locationIndex;  
    
    readingAddress = ESPMeteringTable_GetReadingTableAddressByIndex(locationIndex);
	
    if (readingAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;
	   
    DBMSHandler_SetRecordQueryResponseSize(recordQuery, sizeof(READING_LIST));
    
    DBMSHandler_ReadRecordByAddress( READING_TABLE_ID, readingAddress, 
                            (BYTE *) DBMSHandler_GetRecordQueryResponse(recordQuery), 
                            DBMSHandler_GetRecordQueryResponseSize(recordQuery));
        
    return ESP_METERING_TABLE_NO_ERROR_CODE;    
}

INT16 API_ESPMeteringTable_InsertReadingTableItem(READING_LIST_PTR readingItem){
	
    INT16 index;		
    INT16 error_code;

    index = ESPMeteringTable_FindMeterTableIndexBySerialNumber(readingItem->Serial_Num);

    if(index == ESP_METERING_TABLE_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE){		

        return index;
    }

    error_code = ESPMeteringTable_InsertReadingTableItemByIndex(index, readingItem);

    if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
        return error_code;

    return ESP_METERING_TABLE_NEW_READING_ADD;	
}

INT16 API_ESPMeteringTable_Init(void){

    WORD deviceIndexKey;
    WORD meterIndexKey;
    INT16 error_code;   
    
    
    error_code = DBMSHandler_GetRecord(DEVICE_INDEX_TABLE_KEY_ID, (BYTE *) &deviceIndexKey, sizeof(deviceIndexKey));
       
    if(error_code != DBMS_HANDLER_NO_ERROR_CODE)
        exit(0);
    
    if (deviceIndexKey != ESP_METERING_TABLE_KEY_WORD){
        
        deviceIndexKey = ESP_METERING_TABLE_KEY_WORD;
        API_ESPMeteringTable_DropDeviceTable();
        
        DBMSHandler_SetRecord(DEVICE_INDEX_TABLE_KEY_ID, (BYTE *) &deviceIndexKey, sizeof(deviceIndexKey));
    }
    
    error_code = DBMSHandler_GetRecord(METER_INDEX_TABLE_KEY_ID, (BYTE *) &meterIndexKey, sizeof(meterIndexKey));
    
    if(error_code != DBMS_HANDLER_NO_ERROR_CODE)
        exit(0);
    
    if (meterIndexKey != ESP_METERING_TABLE_KEY_WORD){
        
        meterIndexKey = ESP_METERING_TABLE_KEY_WORD;
        API_ESPMeteringTable_DropMeterTable();
        
        DBMSHandler_SetRecord(METER_INDEX_TABLE_KEY_ID, (BYTE *) &meterIndexKey, sizeof(meterIndexKey));
    }
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************
BYTE bfnReadMTRSTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    INT16 meterIndex;    
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    meterIndex = ESPMeteringTable_GetMeterIndex();

    if (meterIndex > 0) {

        * pagingDataResponseSize = (meterIndex * sizeof (MTR_LIST));
        
        ESPMeteringTable_SelectQuery(meterIndex, METER_TABLE_ID);
        
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnReadDevicesTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    WORD deviceIndex;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    deviceIndex = ESPMeteringTable_GetDeviceIndex();

    if (deviceIndex > 0) {

        * pagingDataResponseSize = (deviceIndex * sizeof (DEV_LIST));
        ESPMeteringTable_SelectQuery(deviceIndex, DEVICE_TABLE_ID);
        
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    WORD readingIndex;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    readingIndex = ESPMeteringTable_GetMeterIndex();

    if (readingIndex > 0) {

        * pagingDataResponseSize = (readingIndex * sizeof (READING_LIST));
        
        ESPMeteringTable_SelectQuery(readingIndex, READING_TABLE_ID);
        
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnDelMTRMetersTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    BYTE * dataResponse_ptr = dataResponse;

    * dataResponse_ptr = SUCCESS_CMD;
    * dataResponseSize = sizeof (BYTE);
    
    API_ESPMeteringTable_DropMeterTable();

    return ESP_DOES_NOT_WAIT_ANSWER;
}

BYTE bfnDelMTRDevicesTable(BYTE * dataRequest, WORD dataRequestSize, BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    BYTE * dataResponse_ptr = dataResponse;
    
    * dataResponse_ptr = SUCCESS_CMD;
    * dataResponseSize = sizeof (BYTE);

    API_ESPMeteringTable_DropDeviceTable();
    
    return ESP_DOES_NOT_WAIT_ANSWER;
}

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void ESPMeteringTableDriver(void){
    
    ESPMeteringTablerDriverFunctionStates[ESPMeteringTableSM.bActualState]();
}

void ESPMeteringTable_SetStateMachine(BYTE actualState, BYTE nextState) {

    ESPMeteringTableSM.bActualState = actualState;
    ESPMeteringTableSM.bNextState = nextState;
    
    vfnEventEnable(BUFFER_EVENT);
    vfnEventPost(BUFFER_EVENT);
}

void ESPMeteringTable_ErrorProcess(void){
    
    DBMSHandler_ClearRecordQuery(&espMeteringTableRecordQuery);
    ESPMeteringTable_SetStateMachine(ESP_METERING_TABLE_END_STATE, ESP_METERING_TABLE_END_STATE);    
}

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void ESPMeteringTable_IdleState(void){
    
}

void ESPMeteringTable_SelectQueryState(void){
    
    BYTE bStatusData;    
    
    WORD quantityOfRecords = DBMSHandler_GetQuantityOfRecordQueries(&espMeteringTableRecordQuery);
    WORD startItem = DBMSHandler_GetStartRecordQueryIndex(&espMeteringTableRecordQuery);
    DBMS_HANDLER_TABLE_ID tableId = DBMSHandler_GetTableIdRecordQuery(&espMeteringTableRecordQuery);
    BOOL isWaitingForQueryResponse = DBMSHandler_IsWaitingForRecordQueryResponse(&espMeteringTableRecordQuery);
    
    if(isWaitingForQueryResponse){
    
        return;
    }
    
    if(DBMSHandler_GetRecordQueryResponseSize(&espMeteringTableRecordQuery)){
            
        startItem++;
        quantityOfRecords--;
        
        ESPComInterface_SendFrame(  0, 
                                    DBMSHandler_GetRecordQueryResponse(&espMeteringTableRecordQuery), 
                                    DBMSHandler_GetRecordQueryResponseSize(&espMeteringTableRecordQuery), 
                                    TRUE, 
                                    FALSE, 
                                    quantityOfRecords == 0 );
        
        DBMSHandler_SetupRecordQuery(&espMeteringTableRecordQuery, tableId, startItem, quantityOfRecords);           
    }
    
    if( quantityOfRecords == 0){
     
        print_warm("quantityOfRecords = 0");
        ESPMeteringTable_ErrorProcess();
        return;
    }
        
    switch(tableId){
        
        case METER_TABLE_ID:
            
            bStatusData = API_ESPMeteringTable_SelectMeterTableItemByRecordIndex(startItem, &espMeteringTableRecordQuery);
            break;
        
        case DEVICE_TABLE_ID:
            
            bStatusData = API_ESPMeteringTable_SelectDeviceTableItemByRecordIndex(startItem, &espMeteringTableRecordQuery);
            break;
            
        case READING_TABLE_ID:            
            
            bStatusData = API_ESPMeteringTable_SelectReadingTableItemByRecordIndex(startItem, &espMeteringTableRecordQuery);
            break;
            
        default:
            print_error("NO VALID TABLE_LIST_TYPE");
            ESPMeteringTable_ErrorProcess();
            return;
    }
    
    if(bStatusData != ESP_METERING_TABLE_NO_ERROR_CODE) {
        
        print_error("There is not a index inside DataBase");
        ESPMeteringTable_ErrorProcess();
        return;
    }   
}

void ESPMeteringTable_EndState(void){
    
    vfnEventClear(BUFFER_EVENT);                                        
    vfnEventDisable(BUFFER_EVENT);
}
