/* 
 * File:   DBMSHandler.h
 * Author: fascio
 *
 * Created on 25 de mayo de 2016, 09:03 AM
 */

#ifndef __DBMS_HANDLER_H__
#define	__DBMS_HANDLER_H__

//******************************************************************************
// DBMS_HANDLER Includes
//******************************************************************************
#include "MEMEEPROM.h"
#include "ESPMeteringCommons.h"

//******************************************************************************
// DBMS_HANDLER Defines
//******************************************************************************
#define DBMS_HANDLER_START_ADDRESS                                      (0x0000)
#define DBMS_HANDLER_NOT_INIT                                           (0xFFFF)
#define DBMS_HANDLER_SINGLE_RECORD                                          (1)

// DBMS_HANDLER Error Codes
#define DBMS_HANDLER_NO_ERROR_CODE                                          (0)
#define DBMS_HANDLER_MEMORY_OVERFLOW_ERROR_CODE                             (-1)
#define DBMS_HANDLER_NULL_PTR_ERROR_CODE                                    (-2)
#define DBMS_HANDLER_WRONG_RECORD_SIZE_ERROR_CODE                           (-3)
#define DBMS_HANDLER_RECORD_ADDRESS_OUT_OF_RANGE_ERROR_CODE                 (-4)


//******************************************************************************
// DBMS_HANDLER DataTypes
//******************************************************************************

typedef struct{
    DBMS_HANDLER_TABLE_ID tableId;
    WORD tableAddress;
    WORD recordSize;
    WORD quantityOfRecords;
}DBMS_HANDLER, * DBMS_HANDLER_PTR;

#define DBMS_HANDLER_NULL_TABLE                    {DBMS_HANDLER_NO_TABLE_ID, DBMS_HANDLER_NOT_INIT, DBMS_HANDLER_NOT_INIT, DBMS_HANDLER_NOT_INIT}

//******************************************************************************
// DBMS_HANDLER SET and GET Function Prototypes
//******************************************************************************

void DBMSHandler_SetTableId(DBMS_HANDLER_PTR dbmsItem, DBMS_HANDLER_TABLE_ID tableId);
DBMS_HANDLER_TABLE_ID DBMSHandler_GetTableId(DBMS_HANDLER_PTR dbmsItem);

void DBMSHandler_SetTableAddress(DBMS_HANDLER_PTR dbmsItem, WORD tableAddress);
WORD DBMSHandler_GetTableAddress(DBMS_HANDLER_PTR dbmsItem);
WORD * DBMSHandler_GetTableAddressPtr(DBMS_HANDLER_PTR dbmsItem);
WORD DBMSHandler_GetTableIndexAddress(DBMS_HANDLER_PTR dbmsItem, INT16 index);

void DBMSHandler_SetRecordSize(DBMS_HANDLER_PTR dbmsItem, WORD recordSize);
WORD DBMSHandler_GetRecordSize(DBMS_HANDLER_PTR dbmsItem);

void DBMSHandler_SetQuantityOfRecords(DBMS_HANDLER_PTR dbmsItem, WORD quantityOfRecords);
WORD DBMSHandler_GetQuantityOfRecords(DBMS_HANDLER_PTR dbmsItem);

//******************************************************************************
// DBMS_HANDLER Function Prototypes
//******************************************************************************
INT16 DBMSHandler_Init(void);
INT16 DBMSHandler_Alloc(WORD * location, WORD size);
INT16 DBMSHandler_Read(WORD src, BYTE * dest, WORD count);
INT16 DBMSHandler_Write(WORD dest, BYTE * src, WORD count);

DBMS_HANDLER_PTR DBMSHandler_GetDBMSItemByTableId(DBMS_HANDLER_TABLE_ID tableId);
WORD DBMSHandler_GetTableAddressByTableId(DBMS_HANDLER_TABLE_ID tableId);
WORD DBMSHandler_GetTableIndexAddressByTableId(DBMS_HANDLER_TABLE_ID tableId, INT16 index);
INT16 DBMSHandler_ValidateRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, WORD recordSize);
INT16 DBMSHandler_ReadRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, BYTE * record, WORD recordSize);
INT16 DBMSHandler_WriteRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, BYTE * record, WORD recordSize);
INT16 DBMSHandler_ClearRecord(DBMS_HANDLER_TABLE_ID tableId, WORD recordAddress, WORD recordSize);

#endif	/* __DBMS_HANDLER_H__ */

