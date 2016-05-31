//******************************************************************************
// Includes
//******************************************************************************
#include "ESPMeteringTable.h"

//******************************************************************************
// Variables
//******************************************************************************
DBMS_HANDLER_RECORD_QUERY espMeteringTableQuery;

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
BYTE bfnBuffer_Table_Meter(WORD quantityOfItems, DBMS_HANDLER_TABLE_ID tableListType) {

    switch (tableListType) {

        case METER_TABLE_ID:
        case READING_TABLE_ID:
        case DEVICE_TABLE_ID:
            break;

        default:
            return FALSE;
    }
    
    //!DataBaseHandler_SetupQuery(&espMeteringTableQuery, 0, quantityOfItems, tableListType);
    DBMSHandler_SetupRecordQuery(&espMeteringTableQuery,tableListType, 0, quantityOfItems);
    ESPMeteringTable_SetStateMachine(_BUFFER_CONSULT_TABLE_STATE, _BUFFER_TRANSMIT_TABLE_STATE);
    
    return TRUE;
}

//******************************************************************************
// DataBaseHandler Function Prototypes
//******************************************************************************
INT16 ESPMeteringTable_GetRecord(DBMS_HANDLER_TABLE_ID tableId, BYTE * record, WORD recordSize ){
    
    WORD recordAddress;
    
    recordAddress = DBMSHandler_GetTableAddressByTableId(tableId);
    
    if(recordAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_ADDRESS_NOT_FOUND_ERROR_CODE;
    
    DBMSHandler_ReadRecord(tableId, recordAddress, record, recordSize );
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 ESPMeteringTable_SetRecord(DBMS_HANDLER_TABLE_ID tableId, BYTE * record, WORD recordSize){
    
    WORD recordAddress;
    
    recordAddress = DBMSHandler_GetTableAddressByTableId(tableId);
    
    if(recordAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_ADDRESS_NOT_FOUND_ERROR_CODE;
    
    DBMSHandler_WriteRecord(tableId, recordAddress, record, recordSize);
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;    
}


INT16 ESPMeteringTable_GetRecordIndex(DBMS_HANDLER_TABLE_ID tableId){
    
    INT16 indexRecord;
    WORD indexRecordAddress;
    
    indexRecordAddress = DBMSHandler_GetTableAddressByTableId(tableId);
    
    if(indexRecordAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_ADDRESS_NOT_FOUND_ERROR_CODE;
    
    DBMSHandler_ReadRecord(tableId, indexRecordAddress, (BYTE *) &indexRecord, sizeof(indexRecord) );
    
    return indexRecord;
}

INT16 ESPMeteringTable_SetRecordIndex(DBMS_HANDLER_TABLE_ID tableId, INT16 meterIndexRecord){
    
    WORD indexRecordAddress;
    
    indexRecordAddress = DBMSHandler_GetTableAddressByTableId(tableId);
    
    if(indexRecordAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_ADDRESS_NOT_FOUND_ERROR_CODE;
    
    DBMSHandler_WriteRecord(tableId, indexRecordAddress, (BYTE *) &meterIndexRecord, sizeof(meterIndexRecord) );
    
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

        DBMSHandler_ReadRecord(METER_TABLE_ID, meterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
        
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

        DBMSHandler_ReadRecord(METER_TABLE_ID, meterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
        
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

        DBMSHandler_ReadRecord(METER_TABLE_ID, meterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
        
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
	
    DBMSHandler_WriteRecord(METER_TABLE_ID, meterAddress, (BYTE *) meterItem, sizeof(MTR_LIST));
    
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

    DBMSHandler_ReadRecord(METER_TABLE_ID, foundMeterAddress, (BYTE *) &meter, sizeof(MTR_LIST));
    
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
    
    DBMSHandler_ReadRecord( METER_TABLE_ID, meterAddress, 
                            (BYTE *) DBMSHandler_GetRecordQueryResponse(recordQuery), 
                            DBMSHandler_GetRecordQueryResponseSize(recordQuery));
            
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

BYTE API_ESPMeteringTable_InsertMeterTableItem(MTR_LIST_PTR meterItem){
	
    INT16 index;		
    INT16 error_code;
    INT16 meterIndex; 

    index = ESPMeteringTable_FindMeterTableIndexBySerialNumber(meterItem->Serial_Num);
    meterIndex = ESPMeteringTable_GetMeterIndex();
    
    if(index == ESP_METERING_TABLE_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE){		

        error_code = ESPMeteringTable_InsertMeterTableItem(meterItem);

        if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
            return 0;        
        
        meterIndex++;
        ESPMeteringTable_SetMeterIndex(meterIndex);
        
        return NEW_MTR_ADD;
    }

    error_code = ESPMeteringTable_UpdateMeterTableItemByIndex(index, meterItem);

    if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
        return 0;

    return NO_NEW_MTR_ADD;	
}

INT16 ESPMeteringTable_DeleteMeterTableItemByIndex(INT16 index){
    
    WORD meterAddress = ESPMeteringTable_GetMeterTableAddressByIndex(index);
	
    if (meterAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_DeleteRecord(METER_TABLE_ID, meterAddress, sizeof(MTR_LIST));
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 API_ESPMeteringTable_DropMeterTable(void){
    
    INT16 index;
        
    for(index = 0; index < NUM_MAX_METERS; index++ )
        ESPMeteringTable_DeleteMeterTableItemByIndex(index);
    
    ESPMeteringTable_SetMeterIndex(0);
}

INT16 ESPMeteringTable_FindDeviceTableIndexByRecordIndex(INT16 recordIndex){
    
    DEV_LIST device;
    WORD deviceAddress;    
    INT16 index = 0;
    INT16 recordIndexCounter = -1;

    deviceAddress = DBMSHandler_GetTableAddressByTableId(DEVICE_TABLE_ID);
    
    while(index < NUM_MAX_NODES) {

        DBMSHandler_ReadRecord(DEVICE_TABLE_ID, deviceAddress, (BYTE *) &device, sizeof(DEV_LIST));
        
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
        
        DBMSHandler_ReadRecord(DEVICE_TABLE_ID, deviceAddress, (BYTE *) &device, sizeof(DEV_LIST));
		
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
		
        DBMSHandler_ReadRecord(DEVICE_TABLE_ID, deviceAddress, (BYTE *) &device, sizeof(DEV_LIST));
        
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
	
        DBMSHandler_WriteRecord(DEVICE_TABLE_ID, deviceAddress, (BYTE *) deviceItem, sizeof(DEV_LIST));
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

    DBMSHandler_ReadRecord(DEVICE_TABLE_ID, foundDeviceAddress, (BYTE *) &device, sizeof(DEV_LIST));

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
    
    DBMSHandler_ReadRecord( DEVICE_TABLE_ID, deviceAddress, 
                            (BYTE *) DBMSHandler_GetRecordQueryResponse(recordQuery), 
                            DBMSHandler_GetRecordQueryResponseSize(recordQuery));
        
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

BYTE API_ESPMeteringTable_InsertDeviceTableItem(DEV_LIST_PTR deviceItem){
	
    INT16 index;		
    INT16 error_code;
    INT16 deviceIndex;

    index = ESPMeteringTable_FindDeviceTableIndexByMACAddress(deviceItem->MAC);
    deviceIndex = ESPMeteringTable_GetDeviceIndex();

    if(index == ESP_METERING_TABLE_MAC_ADDRESS_NOT_FOUND_ERROR_CODE){		

        error_code = ESPMeteringTable_InsertDeviceTableItem(deviceItem);

        if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
            return 0;

        deviceIndex++;
        ESPMeteringTable_SetDeviceIndex(deviceIndex);
        
        return 1;
    }

    error_code = ESPMeteringTable_UpdateDeviceTableItemByIndex(index, deviceItem);

    if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
        return 0;
	
    return 1;	
}

INT16 ESPMeteringTable_DeleteDeviceTableItemByIndex(INT16 index){
    
    WORD deviceAddress = ESPMeteringTable_GetDeviceTableAddressByIndex(index);
	
    if (deviceAddress == DBMS_HANDLER_NULL_ADDRESS)
        return ESP_METERING_TABLE_NULL_STRUCT_ERROR_CODE;

    DBMSHandler_DeleteRecord(DEVICE_TABLE_ID, deviceAddress, sizeof(DEV_LIST));
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

INT16 API_ESPMeteringTable_DropDeviceTable(void){
    
    INT16 index;
        
    for(index = 0; index < NUM_MAX_NODES; index++ )
        ESPMeteringTable_DeleteDeviceTableItemByIndex(index);
    
    ESPMeteringTable_SetDeviceIndex(0);
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

    DBMSHandler_WriteRecord(READING_TABLE_ID, readingAddress, (BYTE *) readingItem, sizeof(READING_LIST));

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
    
    DBMSHandler_ReadRecord( READING_TABLE_ID, readingAddress, 
                            (BYTE *) DBMSHandler_GetRecordQueryResponse(recordQuery), 
                            DBMSHandler_GetRecordQueryResponseSize(recordQuery));
        
    return ESP_METERING_TABLE_NO_ERROR_CODE;    
}

BYTE API_ESPMeteringTable_InsertReadingTableItem(READING_LIST_PTR readingItem){
	
    INT16 index;		
    INT16 error_code;

    index = ESPMeteringTable_FindMeterTableIndexBySerialNumber(readingItem->Serial_Num);

    if(index == ESP_METERING_TABLE_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE){		

        return 0;
    }

    error_code = ESPMeteringTable_InsertReadingTableItemByIndex(index, readingItem);

    if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
        return 0;

    return 1;	
}

INT16 API_ESPMeteringTable_Init(void){

    WORD deviceIndexKey;
    WORD meterIndexKey;
    INT16 error_code;   
    
    
    error_code = ESPMeteringTable_GetRecord(DEVICE_INDEX_TABLE_KEY_ID, (BYTE *) &deviceIndexKey, sizeof(deviceIndexKey));
       
    if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
        exit(0);
    
    if (deviceIndexKey != ESP_METERING_TABLE_KEY_WORD){
        
        deviceIndexKey = ESP_METERING_TABLE_KEY_WORD;
        API_ESPMeteringTable_DropDeviceTable();
        
        ESPMeteringTable_SetRecord(DEVICE_INDEX_TABLE_KEY_ID, (BYTE *) &deviceIndexKey, sizeof(deviceIndexKey));
    }
    
    error_code = ESPMeteringTable_GetRecord(METER_INDEX_TABLE_KEY_ID, (BYTE *) &meterIndexKey, sizeof(meterIndexKey));
    
    if(error_code != ESP_METERING_TABLE_NO_ERROR_CODE)
        exit(0);
    
    if (meterIndexKey != ESP_METERING_TABLE_KEY_WORD){
        
        meterIndexKey = ESP_METERING_TABLE_KEY_WORD;
        API_ESPMeteringTable_DropMeterTable();
        
        ESPMeteringTable_SetRecord(METER_INDEX_TABLE_KEY_ID, (BYTE *) &meterIndexKey, sizeof(meterIndexKey));
    }
    
    return ESP_METERING_TABLE_NO_ERROR_CODE;
}

BYTE API_ESPMeteringTable_SaveTable(BYTE bTableType, BYTE *vptrTableStructure ) {			
	
    switch(bTableType){

            case METERSTABLE:			

                    return API_ESPMeteringTable_InsertMeterTableItem((MTR_LIST_PTR) vptrTableStructure);	

            case DEVICESTABLE:		

                    return API_ESPMeteringTable_InsertDeviceTableItem((DEV_LIST_PTR) vptrTableStructure);

            case READINGSTABLE:

                    return API_ESPMeteringTable_InsertReadingTableItem((READING_LIST_PTR) vptrTableStructure);			
    }

    return 0;
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
        
        bfnBuffer_Table_Meter(meterIndex, METER_TABLE_ID);
        
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
        bfnBuffer_Table_Meter(deviceIndex, DEVICE_TABLE_ID);
        
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
        
        bfnBuffer_Table_Meter(readingIndex, READING_TABLE_ID);
        
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
    
    DBMSHandler_ClearRecordQuery(&espMeteringTableQuery);
    ESPMeteringTable_SetStateMachine(_BUFFER_END_STATE, _BUFFER_END_STATE);    
}

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void vfnBufferIdleState(void){
    
}

void vfnBufferConsultTableState(void){
    
    BYTE bStatusData;    
    
    WORD quantityOfItems = DBMSHandler_GetQuantityOfRecordQueries(&espMeteringTableQuery);
    WORD startItem = DBMSHandler_GetStartRecordQueryIndex(&espMeteringTableQuery);
    DBMS_HANDLER_TABLE_ID tableListType = DBMSHandler_GetTableIdRecordQuery(&espMeteringTableQuery);
    BOOL isWaitingForQueryResponse = DBMSHandler_IsWaitingForRecordQueryResponse(&espMeteringTableQuery);
    
    if(isWaitingForQueryResponse){
    
        return;
    }
    
    if(DBMSHandler_GetRecordQueryResponseSize(&espMeteringTableQuery)){
            
        startItem++;
        quantityOfItems--;
        
        ESPComInterface_SendFrame(  0, 
                                    DBMSHandler_GetRecordQueryResponse(&espMeteringTableQuery), 
                                    DBMSHandler_GetRecordQueryResponseSize(&espMeteringTableQuery), 
                                    TRUE, 
                                    FALSE, 
                                    quantityOfItems == 0 );
        
        DBMSHandler_SetupRecordQuery(&espMeteringTableQuery, tableListType, startItem, quantityOfItems);           
    }
    
    if( quantityOfItems == 0){
     
        print_warm("quantityOfItems = 0");
        ESPMeteringTable_ErrorProcess();
        return;
    }
        
    switch(tableListType){
        
        case METER_TABLE_ID:
            
            bStatusData = API_ESPMeteringTable_SelectMeterTableItemByRecordIndex(startItem, &espMeteringTableQuery);
            break;
        
        case DEVICE_TABLE_ID:
            
            bStatusData = API_ESPMeteringTable_SelectDeviceTableItemByRecordIndex(startItem, &espMeteringTableQuery);
            break;
            
        case READING_TABLE_ID:            
            
            bStatusData = API_ESPMeteringTable_SelectReadingTableItemByRecordIndex(startItem, &espMeteringTableQuery);
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

void vfnBufferTransmitTableState(void){
    
}

void vfnBufferEndState(void){
    
    vfnEventClear(BUFFER_EVENT);                                        
    vfnEventDisable(BUFFER_EVENT);
}
