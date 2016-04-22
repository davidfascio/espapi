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
/*-- Defines -----------------------------------------------------------------*/
#define MAX_SIZE_BUFFER 3000
#define MAX_IIC_WRITE_PAGE_SIZE                 (0x0080)
#define MAX_IIC_BUFFER_SIZE                     (512)

#define MEM24_1025_IIC_EMPTY_LOCATION_VALUE                               (0xFF)


#define CHIP_SELECT_MEM                         (0xA4)         
#define MEM24_1025_IIC_RETRIES                                              (10)

// Error Codes
#define MEM24_1025_IIC_API_IS_BUSY_ERROR_CODE                               (0)

typedef struct{
    
    WORD _wIICDataIndex;
    WORD _wIICTxSize;    
    WORD _wIICTxSizeMAX;    
    WORD _wIICDataIndex_Div;
    WORD _wIICDataIndex_Res;
    BYTE _bPacketPageSend;

    WORD wAddress;
    BYTE gbaIICDataBuffer[MAX_IIC_BUFFER_SIZE];
    BYTE bIICDataPageWrite[MAX_IIC_WRITE_PAGE_SIZE];

    BYTE retriesCount;
}MEM24_1025_IIC_CONTROL, * MEM24_1025_IIC_CONTROL_PTR;

void MEM24_1025_I2C_Clear(MEM24_1025_IIC_CONTROL_PTR mem24_1025_control);
void MEM24_1025_IIC_ClearDataBuffer(MEM24_1025_IIC_CONTROL_PTR mem24_1025_control);

//******************************************************************************
//* MEM24_1025_I2C API Function
//******************************************************************************
BYTE bfnIIC_MEM24_1025_Write(BYTE* bpData, WORD wAddress, WORD wDataSize);
BYTE bfnIIC_MEM24_1025_Read(WORD wAddress, WORD wDataSize, BOOL * isWaitingForResponse);
void MEM24_1025_I2C_ResetRetriesCounter(void);
void MEM24_1025_I2C_ErrorProcess(void);

void vfnIIC_MEM24_1025Driver(void);
void MEM24_1025_I2C_SetStateMachine(BYTE actualState, BYTE nextState);
WORD MEM24_1025_I2C_GetAddressByPacketPageSend(void);
void MEM24_1025_I2C_UpdateTxPagingValues(WORD wAddress);
BYTE bfnIICIsBusy(void);

void bfnIIC_MEM24_1025_Notification(void);

extern BYTE *bBufferIICRead;
#endif	/* MEM24_1025_I2C_H */

