/* 
 * File:   I2C.h
 * Author: fascio
 *
 * Created on 20 de abril de 2016, 17:18
 */

#ifndef __I2C_H__
#define	__I2C_H__

#include "GenericTypeDefs.h"
#include "SystemLog.h"
#include "SystemEvents.h"

//#define I2C_BIT_DELAY                       (0x0007)
#define SCL                                 
#define SDA                                 
#define SDAL          1 // ACK ERROR GEN                      
#define SCLD                                
#define SDAD                                
#define SCLP                                
#define SDAP                                
#define WR_PROTECT_TRIS                     
#define WR_PROTECT                          

#define SDA_B_CONFIG_OUT                    
#define SCL_B_CONFIG                        
#define SDA_B_CONFIG_IN                     
#define SCL_B_CONFIG_IN                     
#define SDA_B_HI                            
#define SDA_B_LO                            
#define SCL_B_HI                            
#define SCL_B_LO                            
//#define SDA_B_IN                            SDAL
#define CLRWDT(x)

#define I2C_DRIVER_MAX_PAGING_DATA_SIZE                                     (128)

// Error Codes
#define I2C_DRIVER_CONTROL_API_IS_BUSY_ERROR_CODE                           (FALSE)

typedef enum
{
    I2C_GEN_STOP = 0,
    I2C_NO_ACK_ERROR
}eI2CFlags;

typedef enum{
    I2C_DRIVER_STOPPED,
    I2C_DRIVER_INITIALIZED,
    I2C_DRIVER_ENDED
} TXRX_STATUS;

typedef struct{
    
    WORD _wI2CRxCounter;
    WORD _wI2CTxCounter;
    WORD _wI2CRxIndex;  
    WORD _wI2CTxIndex;
    BYTE * _bpI2CRxBuffer;
    BYTE _bpI2CTxBuffer[I2C_DRIVER_MAX_PAGING_DATA_SIZE];
    BYTE _bI2CFlags;
    TXRX_STATUS txrxStatus;    
} I2C_DRIVER_CONTROL, * I2C_DRIVER_CONTROL_PTR;

void I2CDriverControl_SetRxCounter(WORD _wI2CRxCounter);
void I2CDriverControl_SetTxCounter(WORD _wI2CTxCounter);
WORD I2CDriverControl_GetRxCounter(void);
WORD I2CDriverControl_GetTxCounter(void);

void I2CDriverControl_SetRxIndex(WORD _wI2CRxIndex);
void I2CDriverControl_SetTxIndex(WORD _wI2CTxIndex);
WORD I2CDriverControl_GetRxIndex(void);
WORD I2CDriverControl_GetTxIndex(void);

void I2CDriverControl_SetRxBuffer(BYTE * buffer, WORD _wI2CRxCounter);
void I2CDriverControl_PutRxBufferByIndex( BYTE data, WORD _wI2CRxIndex );


void I2CDriverControl_SetTxBuffer(BYTE * buffer, WORD _wI2CTxCounter);
BYTE I2CDriverControl_GetTxBufferByIndex(WORD _wI2CTxIndex);

void I2CDriverControl_SetFlagsByIndex(eI2CFlags _bI2CFlags, BOOL state);
BOOL I2CDriverControl_GetFlagsByIndex(eI2CFlags _bI2CFlags);
void I2CDriverControl_ClearFlags( void );
BYTE I2CDriverControl_GetFlags(void);

void I2CDriverControl_SetTxRxStatus(TXRX_STATUS status);
TXRX_STATUS I2CDriverControl_GetTxRxStatus( void );


/*Funtion Prototypes*/
void vfnI2CInit(void);
BYTE bfnI2CError(void);
BYTE bfnIsI2CBusy(void);
BYTE bfnI2CTxRxBuffer(BYTE* bpTxPtr, WORD wTxSize, BYTE* bpRxPtr, WORD wRxSize, BYTE bStart, BYTE bStopFlag);
void vfnI2CDriver(void);
void vfnRTCChipReset(void);

extern BOOL SDA_B_IN;
#endif	/* __I2C_H__ */

