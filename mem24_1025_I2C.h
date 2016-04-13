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

//******************************************************************************
//* MEM24_1025_I2C Defines
//******************************************************************************
/*-- Defines -----------------------------------------------------------------*/
#define MAX_SIZE_BUFFER 3000
#define MAX_IIC_WRITE_PAGE_SIZE                 (0x0080)
#define MAX_IIC_BUFFER_SIZE                     (512)

#define MEM24_1025_IIC_EMPTY_LOCATION_VALUE                               (0xFF)

// Error Codes
#define MEM24_1025_IIC_API_IS_BUSY_ERROR_CODE                               (1)

typedef struct{
    
    WORD _wIICDataIndex;
    WORD _wIICTxSizeMAX;    
    WORD _wIICDataIndex_Div;
    WORD _wIICDataIndex_Res;
    BYTE _bPacketPageSend;

    WORD wAddress;
    BYTE gbaIICDataBuffer[MAX_IIC_BUFFER_SIZE];
    BYTE bIICDataPageWrite[MAX_IIC_WRITE_PAGE_SIZE];

    BYTE retriesCount;
}MEM24_1025_IIC_CONTROL, * MEM24_1025_IIC_CONTROL_PTR;

void MEM24_1025_IIC_ClearDataBuffer(MEM24_1025_IIC_CONTROL_PTR mem24_1025_control);

//******************************************************************************
//* MEM24_1025_I2C API Function
//******************************************************************************
BYTE bfnIIC_MEM24_1025_Write(BYTE* bpData, WORD wAddress, WORD wDataSize);
BYTE bfnIIC_MEM24_1025_Read(WORD wAddress, WORD wDataSize, BOOL * isWaitingForResponse);
void vfnIIC_MEM24_1025Driver(void);
BYTE bfnIICIsBusy(void);

void bfnIIC_MEM24_1025_Notification(void);

extern BYTE *bBufferIICRead;
#endif	/* MEM24_1025_I2C_H */

