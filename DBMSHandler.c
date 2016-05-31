//******************************************************************************
// DBMS_HANDLER Includes
//******************************************************************************
#include "DBMSHandler.h"

//******************************************************************************
// DBMS_HANDLER Variables
//******************************************************************************
DBMS_HANDLER dbmsHandlerControl[] ={

    //******************************************************************************
    // DBMS_HANDLER AUTO_READING_TIME TABLE
    //******************************************************************************
    {
        AUTO_READING_TIME_TABLE_ID,     /*  BYTE tableId            */
        DBMS_HANDLER_NULL_ADDRESS,          /*  WORD tableAddress       */
        sizeof(WORD),                   /*  WORD recordSize         */
        DBMS_HANDLER_SINGLE_RECORD      /*  WORD quatityOfRecords   */
    },   
        
    //******************************************************************************
    // DBMS_HANDLER DEVICES TABLE
    //******************************************************************************
    {
        DEVICE_TABLE_ID,                /*  BYTE tableId            */
        DBMS_HANDLER_NULL_ADDRESS,          /*  WORD tableAddress       */
        sizeof(DEV_LIST),               /*  WORD recordSize         */
        NUM_MAX_NODES                   /*  WORD quatityOfRecords   */
    },
    
    //******************************************************************************
    // DBMS_HANDLER DEVICE INDEX TABLE KEY
    //******************************************************************************
    {
        DEVICE_INDEX_TABLE_KEY_ID,          /*  BYTE tableId            */
        DBMS_HANDLER_NULL_ADDRESS,          /*  WORD tableAddress       */
        sizeof(WORD),                   /*  WORD recordSize         */
        DBMS_HANDLER_SINGLE_RECORD      /*  WORD quatityOfRecords   */
    },
        
    //******************************************************************************
    // DBMS_HANDLER DEVICE INDEX TABLE
    //******************************************************************************
    {
        DEVICE_INDEX_TABLE_ID,          /*  BYTE tableId            */
        DBMS_HANDLER_NULL_ADDRESS,          /*  WORD tableAddress       */
        sizeof(INT16),                   /*  WORD recordSize         */
        DBMS_HANDLER_SINGLE_RECORD      /*  WORD quatityOfRecords   */
    },
            
    //******************************************************************************
    // DBMS_HANDLER METERS TABLE
    //******************************************************************************
    {
        METER_TABLE_ID,                 /*  BYTE tableId            */
        DBMS_HANDLER_NULL_ADDRESS,          /*  WORD tableAddress       */
        sizeof(MTR_LIST),               /*  WORD recordSize         */
        NUM_MAX_METERS                  /*  WORD quatityOfRecords   */
    },

    //******************************************************************************
    // DBMS_HANDLER METER INDEX TABLE KEY
    //******************************************************************************
    {
        METER_INDEX_TABLE_KEY_ID,          /*  BYTE tableId            */
        DBMS_HANDLER_NULL_ADDRESS,          /*  WORD tableAddress       */
        sizeof(WORD),                   /*  WORD recordSize         */
        DBMS_HANDLER_SINGLE_RECORD      /*  WORD quatityOfRecords   */
    },
    
    //******************************************************************************
    // DBMS_HANDLER METER INDEX TABLE
    //******************************************************************************
    {
        METER_INDEX_TABLE_ID,           /*  BYTE tableId            */
        DBMS_HANDLER_NULL_ADDRESS,          /*  WORD tableAddress       */
        sizeof(INT16),                   /*  WORD recordSize         */
        DBMS_HANDLER_SINGLE_RECORD      /*  WORD quatityOfRecords   */
    },
    
    //******************************************************************************
    // DBMS_HANDLER READING TABLE
    //******************************************************************************
    {
        READING_TABLE_ID,               /*  BYTE tableId            */
        DBMS_HANDLER_NULL_ADDRESS,          /*  WORD tableAddress       */
        sizeof(READING_LIST),           /*  WORD recordSize         */
        NUM_MAX_METERS                  /*  WORD quatityOfRecords   */
    },
    
    //******************************************************************************
    // DBMS_HANDLER NULL TABLE
    //******************************************************************************
    DBMS_HANDLER_NULL_TABLE
};

WORD DBMSHandler_MemoryLocation = DBMS_HANDLER_START_ADDRESS;

//******************************************************************************
// DBMS_HANDLER SET and GET Function Prototypes
//******************************************************************************

void DBMSHandler_SetTableId(DBMS_HANDLER_PTR dbmsItem, DBMS_HANDLER_TABLE_ID tableId) {

    if (dbmsItem == NULL)
        return;

    dbmsItem->tableId = tableId;
}

DBMS_HANDLER_TABLE_ID DBMSHandler_GetTableId(DBMS_HANDLER_PTR dbmsItem) {

    return dbmsItem->tableId;
}

void DBMSHandler_SetTableAddress(DBMS_HANDLER_PTR dbmsItem, WORD tableAddress) {

    if (dbmsItem == NULL)
        return;

    dbmsItem->tableAddress = tableAddress;
}

WORD DBMSHandler_GetTableAddress(DBMS_HANDLER_PTR dbmsItem) {

    return dbmsItem->tableAddress;
}

WORD * DBMSHandler_GetTableAddressPtr(DBMS_HANDLER_PTR dbmsItem) {

    return &dbmsItem->tableAddress;
}

WORD DBMSHandler_GetTableIndexAddress(DBMS_HANDLER_PTR dbmsItem, INT16 index){
    
    WORD address;
    WORD recordSize;
    WORD quatityOfRecords;
    
    quatityOfRecords = DBMSHandler_GetQuantityOfRecords(dbmsItem);
    
    if((quatityOfRecords < index) || (index < 0))
        return DBMS_HANDLER_NULL_ADDRESS;
    
    address = DBMSHandler_GetTableAddress(dbmsItem);
    recordSize = DBMSHandler_GetRecordSize(dbmsItem);
    address += (index * recordSize);
    
    return address;
}

void DBMSHandler_SetRecordSize(DBMS_HANDLER_PTR dbmsItem, WORD recordSize) {

    if (dbmsItem == NULL)
        return;

    dbmsItem->recordSize = recordSize;
}

WORD DBMSHandler_GetRecordSize(DBMS_HANDLER_PTR dbmsItem) {

    return dbmsItem->recordSize;
}

void DBMSHandler_SetQuantityOfRecords(DBMS_HANDLER_PTR dbmsItem, WORD quantityOfRecords) {

    if (dbmsItem == NULL)
        return;

    dbmsItem->quantityOfRecords = quantityOfRecords;
}

WORD DBMSHandler_GetQuantityOfRecords(DBMS_HANDLER_PTR dbmsItem) {

    return dbmsItem->quantityOfRecords;
}

//******************************************************************************
// DBMS_HANDLER_RECORD_QUERY SET and GET Function Prototypes
//******************************************************************************

void DBMSHandler_SetupRecordQuery(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery, DBMS_HANDLER_TABLE_ID tableIdRecord, WORD startRecordIdex, WORD quantityOfRecords){

    if(recordQuery == NULL)
        return;
    
    DBMSHandler_ClearRecordQuery(recordQuery);    
    
    DBMSHandler_SetStartRecordQueryIndex(recordQuery, startRecordIdex);
    DBMSHandler_SetQuantityOfRecordQueries(recordQuery, quantityOfRecords);
    DBMSHandler_SetTableIdRecordQuery(recordQuery, tableIdRecord);
}

void DBMSHandler_ClearRecordQuery(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery){
    
    if(recordQuery == NULL)
        return;
    
    memset(recordQuery, 0, sizeof(DBMS_HANDLER_RECORD_QUERY));
}

DBMS_HANDLER_TABLE_ID DBMSHandler_GetTableIdRecordQuery(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery){
    
    return recordQuery->tableIdRecord;
}

void DBMSHandler_SetTableIdRecordQuery(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery, DBMS_HANDLER_TABLE_ID tableIdRecord){
    
    if(recordQuery == NULL)
        return;
    
    recordQuery->tableIdRecord = tableIdRecord;
}

WORD DBMSHandler_GetStartRecordQueryIndex(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery){

    return recordQuery->startRecordIdex;
}

void DBMSHandler_SetStartRecordQueryIndex(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery, WORD startRecordIdex){
    
    if(recordQuery == NULL)
        return;
    
    recordQuery->startRecordIdex = startRecordIdex;
}

WORD DBMSHandler_GetQuantityOfRecordQueries(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery){
    
    return recordQuery->quantityOfRecords;
}

void DBMSHandler_SetQuantityOfRecordQueries(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery, WORD quantityOfRecords){
    
    if(recordQuery == NULL)
        return;
    
    recordQuery->quantityOfRecords = quantityOfRecords;    
}

BYTE * DBMSHandler_GetRecordQueryResponse(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery){
    
    return recordQuery->recordQueryResponse;
}

WORD DBMSHandler_GetRecordQueryResponseSize(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery){
    
    return recordQuery->recordQueryResponseSize;
}

void DBMSHandler_SetRecordQueryResponseSize(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery, WORD recordQueryResponseSize){
    
    if(recordQuery == NULL)
        return;
    
    recordQuery->recordQueryResponseSize = recordQueryResponseSize;
}

BOOL DBMSHandler_IsWaitingForRecordQueryResponse(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery){
    
    return recordQuery->isWaitingForRecordQueryResponse;    
}

void DBMSHandler_SetWaitingForRecordQueryResponse(DBMS_HANDLER_RECORD_QUERY_PTR recordQuery, BOOL isWaitingForRecordQueryResponse){
    
    if(recordQuery == NULL)
        return;
    
    recordQuery->isWaitingForRecordQueryResponse = isWaitingForRecordQueryResponse;
}

//******************************************************************************
// DBMS_HANDLER Function Prototypes
//******************************************************************************

INT16 DBMSHandler_Init(void) {

    INT16 error_code;
    DBMS_HANDLER_PTR dbmsHandler_ptr = dbmsHandlerControl;
    DWORD size;
    
    MEM_EEPROM_Init();

    while (DBMSHandler_GetTableId(dbmsHandler_ptr) != DBMS_HANDLER_NO_TABLE_ID) {

        size = DBMSHandler_GetRecordSize(dbmsHandler_ptr) * DBMSHandler_GetQuantityOfRecords(dbmsHandler_ptr);
        error_code = DBMSHandler_CreateTable(   DBMSHandler_GetTableAddressPtr(dbmsHandler_ptr),
                                                size);

        if (error_code != DBMS_HANDLER_NO_ERROR_CODE)
            break;

        dbmsHandler_ptr++;
    }

    print_info("DBMS assignation Free space size %d Bytes", DBMS_HANDLER_END_ADDRESS - DBMSHandler_MemoryLocation);
    return error_code;
}

INT16 DBMSHandler_CreateTable(WORD * location, DWORD size) {
    
    DWORD memoryLocation = DBMSHandler_MemoryLocation;

    if ((memoryLocation + size) > DBMS_HANDLER_END_ADDRESS) {

        print_error("DBMS_HANDLER_MEMORY_OVERFLOW_ERROR_CODE");
        print_error("DBMS could not assign into Location: %04X Size %d Bytes", memoryLocation, size);
        return DBMS_HANDLER_MEMORY_OVERFLOW_ERROR_CODE;
    }

    * location = DBMSHandler_MemoryLocation;
    DBMSHandler_MemoryLocation += size;

    print_info("New DBMS assignation into Location: %04X Size %d Bytes", * location, size);
    return DBMS_HANDLER_NO_ERROR_CODE;
}

INT16 DBMSHandler_Read(WORD src, BYTE * dest, WORD count) {

    MEM_EEPROM_Read(src, dest, count);
}

INT16 DBMSHandler_Write(WORD dest, BYTE * src, WORD count) {

    MEM_EEPROM_Write(dest, src, count);
}

DBMS_HANDLER_PTR DBMSHandler_GetDBMSItemByTableId(DBMS_HANDLER_TABLE_ID tableId){
    
    DBMS_HANDLER_PTR dbmsItem = dbmsHandlerControl;
    
    while(DBMSHandler_GetTableId(dbmsItem) != DBMS_HANDLER_NO_TABLE_ID){
        
        if(DBMSHandler_GetTableId(dbmsItem) == tableId)
            break;
        
        dbmsItem++;
    }
    
    if(DBMSHandler_GetTableId(dbmsItem) == DBMS_HANDLER_NO_TABLE_ID)
        return NULL;
    
    return dbmsItem;    
}

WORD DBMSHandler_GetTableAddressByTableId(DBMS_HANDLER_TABLE_ID tableId){

    DBMS_HANDLER_PTR dbmsItem;
        
    dbmsItem = DBMSHandler_GetDBMSItemByTableId(tableId);
    
    if(dbmsItem == NULL)
        return DBMS_HANDLER_NULL_ADDRESS;
    
    return DBMSHandler_GetTableAddress(dbmsItem);
}

WORD DBMSHandler_GetTableIndexAddressByTableId(DBMS_HANDLER_TABLE_ID tableId, INT16 index){
    
    DBMS_HANDLER_PTR dbmsItem;
        
    dbmsItem = DBMSHandler_GetDBMSItemByTableId(tableId);
    
    if(dbmsItem == NULL)
        return DBMS_HANDLER_NULL_ADDRESS;
    
    return DBMSHandler_GetTableIndexAddress(dbmsItem, index);
}

INT16 DBMSHandler_ValidateRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, WORD recordSize){
    
    DBMS_HANDLER_PTR dbmsItem;
    WORD minTableAddress;
    WORD maxTableAddress;
    
    dbmsItem = DBMSHandler_GetDBMSItemByTableId(tableId);
    
    if(dbmsItem == NULL)
        return DBMS_HANDLER_NULL_PTR_ERROR_CODE;
    
    if(recordSize != DBMSHandler_GetRecordSize(dbmsItem))
        return DBMS_HANDLER_WRONG_RECORD_SIZE_ERROR_CODE;
    
    minTableAddress = DBMSHandler_GetTableAddress(dbmsItem);
    maxTableAddress = DBMSHandler_GetTableAddress(dbmsItem) + 
                     ( DBMSHandler_GetRecordSize(dbmsItem) * ( DBMSHandler_GetQuantityOfRecords(dbmsItem) - 1 ) );
    
    if((minTableAddress > recordAddress) && (maxTableAddress < recordSize))
        return DBMS_HANDLER_RECORD_ADDRESS_OUT_OF_RANGE_ERROR_CODE;
    
    return DBMS_HANDLER_NO_ERROR_CODE;
}

INT16 DBMSHandler_ReadRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, BYTE * record, WORD recordSize){
    
    INT16 error_code;
    
    error_code = DBMSHandler_ValidateRecord(tableId, recordAddress, recordSize);
    
    if(error_code != DBMS_HANDLER_NO_ERROR_CODE)
        return error_code;
            
    return DBMSHandler_Read(recordAddress, record, recordSize);
}

INT16 DBMSHandler_WriteRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, BYTE * record, WORD recordSize){
    
    INT16 error_code;
    
    error_code = DBMSHandler_ValidateRecord(tableId, recordAddress, recordSize);
    
    if(error_code != DBMS_HANDLER_NO_ERROR_CODE)
        return error_code;
            
    return DBMSHandler_Write(recordAddress, record, recordSize);
}

INT16 DBMSHandler_DeleteRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, WORD recordSize){
    
    BYTE * dummyRecord;
    INT16 error_code;
    
    dummyRecord = (BYTE *) calloc(recordSize, sizeof(BYTE));
    
    if(dummyRecord == NULL)
        return DBMS_HANDLER_NULL_PTR_ERROR_CODE;
    
    error_code = DBMSHandler_WriteRecord(tableId, recordAddress, dummyRecord, recordSize);    
    
    free(dummyRecord);
    
    return error_code;
}
