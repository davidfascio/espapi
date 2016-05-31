/* 
 * File:   mem24_1025_I2C.h
 * Author: fascio
 *
 * Created on 1 de abril de 2016, 13:53
 */

#ifndef MEM24_1025_I2C_H
#define	MEM24_1025_I2C_H

//******************************************************************************
//* Includes
//******************************************************************************
#include "GenericTypeDefs.h"
#include "SystemLog.h"
#include "Utility.h"
#include "SystemEvents.h"
#include "EventsEngine.h"
#include "I2C.h"

//******************************************************************************
//* MEM24_1025_I2C Defines
//******************************************************************************
#define MAX_IIC_WRITE_PAGE_SIZE                                             (0x0080)
#define MAX_IIC_BUFFER_SIZE                                                 (512)
#define MEM24_1025_I2C_EMPTY_LOCATION_VALUE                                 (0xFF)

#define CHIP_SELECT_MEM                                                     (0xA4)         
#define MEM24_1025_I2C_RETRIES                                              (10)
#define MEM24_1025_I2C_SELECT_ADDRESS_FRAME_SIZE                            (3)

//
// Error Codes
#define MEM24_1025_I2C_API_IS_BUSY_ERROR_CODE                               (0)

//******************************************************************************
//* MEM24_1025_I2C Vartypes
//******************************************************************************

typedef struct {        
    
    WORD wAddress;
    WORD _wIICTxSize;
    WORD _wIICTxSizeMAX;
    WORD _wIICRxSize;
    BYTE * receptionBuffer;
    WORD _wIICDataIndex;
    WORD _wIICDataIndex_Div;
    WORD _wIICDataIndex_Res;
    BYTE _bPacketPageSend;
    WORD _wPagingAddress;
    BYTE gbaIICDataBuffer[MAX_IIC_BUFFER_SIZE];
    BYTE retriesCount;
    BOOL * notification;
    BYTE error_code;
    
} MEM24_1025_I2C_CONTROL, * MEM24_1025_I2C_CONTROL_PTR;

typedef void (* UpdateCallBack_Function)(void);

//******************************************************************************
//* MEM24_1025_I2C CONTROL Funtions
//******************************************************************************
void MEM24_1025_I2C_SetAddress(WORD _wAddress);
WORD MEM24_1025_I2C_GetAddress(void);

void MEM24_1025_I2C_SetTxSize(WORD _wIICTxSize);
WORD MEM24_1025_I2C_GetTxSize(void);

void MEM24_1025_I2C_SetTxSizeMAX(WORD _wIICTxSizeMAX);
WORD MEM24_1025_I2C_GetTxSizeMAX(void);

void MEM24_1025_I2C_SetRxSize(WORD _wIICRxSize);
WORD MEM24_1025_I2C_GetRxSize(void);

void MEM24_1025_I2C_SetReceptionBuffer(BYTE * receptionBuffer, WORD _wIICRxSize);
BYTE * MEM24_1025_I2C_GetReceptionBuffer(void);

void MEM24_1025_I2C_SetDataIndex(WORD _wIICDataIndex);
WORD MEM24_1025_I2C_GetDataIndex(void);

void MEM24_1025_I2C_SetDataIndexDiv(WORD _wIICDataIndex_Div);
WORD MEM24_1025_I2C_GetDataIndexDiv(void);

void MEM24_1025_I2C_SetDataIndexRes(WORD _wIICDataIndex_Res);
WORD MEM24_1025_I2C_GetDataIndexRes(void);

void MEM24_1025_I2C_SetPacketPageSend(BYTE _bPacketPageSend);
BYTE MEM24_1025_I2C_GetPacketPageSend(void);

void MEM24_1025_I2C_SetPagingAddress(WORD _wPagingAddress);
WORD MEM24_1025_I2C_GetPagingAddress(void);

void MEM24_1025_I2C_ClearDataBuffer(void);
void MEM24_1025_I2C_SetDataBuffer( BYTE * data, WORD dataSize);
BYTE * MEM24_1025_I2C_GetDataBuffer(void);

void MEM24_1025_I2C_SetRetriesCount(BYTE retriesCount);
BYTE MEM24_1025_I2C_GetRetriesCount(void);

void MEM24_1025_I2C_SetupNotification(BOOL * notification);
void MEM24_1025_I2C_SetNotification(BOOL notification);
BOOL MEM24_1025_I2C_GetNotification(void);

void MEM24_1025_I2C_Clear( void );

//******************************************************************************
//* MEM24_1025_I2C API Function
//******************************************************************************
BOOL MEM24_1025_I2C_IsAPIBusy(void);
void MEM24_1025_I2C_SetAPIBusy(BOOL state);

BYTE API_MEM24_1025_I2C_Write(BYTE * bpData, WORD wAddress, WORD wDataSize);
BYTE API_MEM24_1025_I2C_Read(WORD wAddress, BYTE * receptionBuffer, WORD wDataSize, BOOL * isWaitingForResponse);
void MEM24_1025_I2C_ResetRetriesCounter(void);
void MEM24_1025_I2C_ErrorProcess(void);

void MEM24_1025_I2C_SendTxRxDataProcess(BYTE * txData,
        WORD txDataSize,
        BYTE * rxData,
        WORD rxDataSize,
        BOOL isFirstFrame,
        BOOL isLastFrame,
        UpdateCallBack_Function updateCallBackFunction,
        BYTE nextState);

WORD MEM24_1025_I2C_GetReadAddressByPacketPageSend(void);
WORD MEM24_1025_I2C_GetWriteAddressByPacketPageSend(void);
WORD MEM24_1025_I2C_BuildAddress(BYTE chipSelect, WORD wAddress, BYTE * buffer);

//******************************************************************************
//* Updating Function
//******************************************************************************
void MEM24_1025_I2C_UpdateTxPagingValues(void);
void MEM24_1025_I2C_UpdateDataIndex(void);


BYTE bfnIICIsBusy(void);

//******************************************************************************
//* MEM24_1025_I2C STATE MACHINE
//******************************************************************************

void vfnIIC_MEM24_1025Driver(void);
void MEM24_1025_I2C_SetStateMachine(BYTE actualState, BYTE nextState);


#endif	/* MEM24_1025_I2C_H */

