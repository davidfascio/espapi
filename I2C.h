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
#define SDAL          0 // ACK ERROR GEN                      
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
#define SDA_B_IN                            SDAL


/*Funtion Prototypes*/
void vfnI2CInit(void);
BYTE bfnI2CError(void);
BYTE bfnIsI2CBusy(void);
BYTE bfnI2CTxRxBuffer(BYTE* bpTxPtr, WORD wTxSize, BYTE* bpRxPtr, WORD wRxSize, BYTE bStart, BYTE bStopFlag);
void vfnI2CDriver(void);
void vfnRTCChipReset(void);

#endif	/* __I2C_H__ */

