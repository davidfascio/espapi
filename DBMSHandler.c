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
        DBMS_HANDLER_NOT_INIT,          /*  WORD tableAddress       */
        sizeof(WORD),                   /*  WORD recordSize         */
        DBMS_HANDLER_SINGLE_RECORD      /*  WORD quatityOfRecords   */
    },   
    
    //******************************************************************************
    // DBMS_HANDLER DEVICE INDEX TABLE
    //******************************************************************************
    {
        DEVICE_INDEX_TABLE_ID,          /*  BYTE tableId            */
        DBMS_HANDLER_NOT_INIT,          /*  WORD tableAddress       */
        sizeof(INT16),                   /*  WORD recordSize         */
        DBMS_HANDLER_SINGLE_RECORD      /*  WORD quatityOfRecords   */
    },
    
    //******************************************************************************
    // DBMS_HANDLER METER INDEX TABLE
    //******************************************************************************
    {
        METER_INDEX_TABLE_ID,           /*  BYTE tableId            */
        DBMS_HANDLER_NOT_INIT,          /*  WORD tableAddress       */
        sizeof(INT16),                   /*  WORD recordSize         */
        DBMS_HANDLER_SINGLE_RECORD      /*  WORD quatityOfRecords   */
    },
    
    //******************************************************************************
    // DBMS_HANDLER DEVICES TABLE
    //******************************************************************************
    {
        DEVICE_TABLE_ID,                /*  BYTE tableId            */
        DBMS_HANDLER_NOT_INIT,          /*  WORD tableAddress       */
        sizeof(DEV_LIST),               /*  WORD recordSize         */
        NUM_MAX_NODES                   /*  WORD quatityOfRecords   */
    },
        
    //******************************************************************************
    // DBMS_HANDLER METERS TABLE
    //******************************************************************************
    {
        METER_TABLE_ID,                 /*  BYTE tableId            */
        DBMS_HANDLER_NOT_INIT,          /*  WORD tableAddress       */
        sizeof(MTR_LIST),               /*  WORD recordSize         */
        NUM_MAX_METERS                  /*  WORD quatityOfRecords   */
    },

    //******************************************************************************
    // DBMS_HANDLER READING TABLE
    //******************************************************************************
    {
        READING_TABLE_ID,               /*  BYTE tableId            */
        DBMS_HANDLER_NOT_INIT,          /*  WORD tableAddress       */
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
    
    if(quatityOfRecords < index)
        return DBMS_HANDLER_NOT_INIT;
    
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
// DBMS_HANDLER Function Prototypes
//******************************************************************************

INT16 DBMSHandler_Init(void) {

    INT16 error_code;
    DBMS_HANDLER_PTR dbmsHandler_ptr = dbmsHandlerControl;

    MEM_EEPROM_Init();

    while (DBMSHandler_GetTableId(dbmsHandler_ptr) != DBMS_HANDLER_NO_TABLE_ID) {

        error_code = DBMSHandler_Alloc( DBMSHandler_GetTableAddressPtr(dbmsHandler_ptr),
                                        DBMSHandler_GetRecordSize(dbmsHandler_ptr) *
                                        DBMSHandler_GetQuantityOfRecords(dbmsHandler_ptr));

        if (error_code != DBMS_HANDLER_NO_ERROR_CODE)
            return error_code;

        dbmsHandler_ptr++;
    }

    return DBMS_HANDLER_NO_ERROR_CODE;
}

INT16 DBMSHandler_Alloc(WORD * location, WORD size) {

    if ((DBMSHandler_MemoryLocation + size) > MEM_EEPROM_MAX_MEMORY_SIZE) {

        print_error("DBMS_HANDLER_MEMORY_OVERFLOW_ERROR_CODE");
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
        return DBMS_HANDLER_NOT_INIT;
    
    return DBMSHandler_GetTableAddress(dbmsItem);
}

WORD DBMSHandler_GetTableIndexAddressByTableId(DBMS_HANDLER_TABLE_ID tableId, INT16 index){
    
    DBMS_HANDLER_PTR dbmsItem;
        
    dbmsItem = DBMSHandler_GetDBMSItemByTableId(tableId);
    
    if(dbmsItem == NULL)
        return DBMS_HANDLER_NOT_INIT;
    
    return DBMSHandler_GetTableIndexAddress(dbmsItem, index);
}

INT16 DBMSHandler_ValidateRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, WORD recordSize){
    
    DBMS_HANDLER_PTR dbmsItem;
    WORD minTableAddress;
    WORD maxTableAddress;
    
    dbmsItem = DBMSHandler_GetDBMSItemByTableId(tableId);
    
    if(dbmsItem == NULL)
        return DBMS_HANDLER_NULL_PTR_ERROR_CODE;
    
    if(recordSize > DBMSHandler_GetRecordSize(dbmsItem))
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

INT16 DBMSHandler_ClearRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, WORD recordSize){
    
    BYTE * dummyRecord;
    INT16 error_code;
    
    dummyRecord = (BYTE *) calloc(recordSize, sizeof(BYTE));
    
    if(dummyRecord == NULL)
        return DBMS_HANDLER_NULL_PTR_ERROR_CODE;
    
    error_code = DBMSHandler_WriteRecord(tableId, recordAddress, dummyRecord, recordSize);    
    
    free(dummyRecord);
    
    return error_code;
}
