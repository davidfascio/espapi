/* 
 * File:   mem24_1025_I2C.h
 * Author: fascio
 *
 * Created on 1 de abril de 2016, 13:53
 */

#ifndef MEM24_1025_I2C_H
#define	MEM24_1025_I2C_H

#include "GenericTypeDefs.h"
#include "SystemLog.h"
#include "Utility.h"
#include "SystemEvents.h"
#include "EventsEngine.h"

/*-- Defines -----------------------------------------------------------------*/
#define MAX_SIZE_BUFFER 3000
//TODO: USER CODE

//void vfnIICDriver(void);
//
BYTE bfnIIC_MEM24_1025_Write(BYTE* bpData, WORD wAddress, WORD wDataSize);
BYTE bfnIIC_MEM24_1025_Read(WORD wAddress, WORD wDataSize, BOOL * isWaitingForResponse);
void vfnIIC_MEM24_1025Driver(void);
BYTE bfnIICIsBusy(void);

void bfnIIC_MEM24_1025_Notification(void);

extern BYTE *bBufferIICRead;
#endif	/* MEM24_1025_I2C_H */

