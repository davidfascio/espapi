/* 
 * File:   System.h
 * Author: fascio
 *
 * Created on 1 de abril de 2016, 10:18
 */

#ifndef __SYSTEM_H__
#define	__SYSTEM_H__

//******************************************************************************
// Includes
//******************************************************************************
#include "GenericTypeDefs.h"
#include "ESPCommons.h"


//******************************************************************************
// Defines
//******************************************************************************

//******************************************************************************
// Status Command
//******************************************************************************
#define RESET_START                                                 0xF0
#define BACKUP_BUSY                                                 0xF4

#define SUCCESS_CMD                                                 0x01
#define PERMIT_SENT                                                 0x02
#define FRAGDATA_ARRIVED                                            0x03
#define NEW_DEV_ADDED                                               0x04
#define B_IDLE_STATE                                                0x05
#define ALARM_CONFIGURE_SENT                                        0x06
#define ALARM_OFF                                                   0x07
#define COOR_BUSY                                                   0x08
#define FIRMWARE_INVALID                                            0x0C

#define ANNCE_MAC                                                   0x23

#define ERROR_CODE                                                  0x80
#define BAD_CRC_CMD                                                 0x81
#define MALFORMED_FRAME                                             0x82
#define NO_FOUND_CMD                                                0x83
#define MTR_NO_RSPD                                                 0x85
#define FIRMWARE_NO_UPLOADED                                        0x88
#define METER_ALREADY_EXIST                                         0x8A
#define NO_FRAGDATA_ARRIVED                                         0x8B
#define CAB_WITHOUT_METERS                                          0x8C
#define METER_NO_SAVE_IN_DB                                         0x8D
#define PARAMETERS_NO_SAVE                                          0x8E

//******************************************************************************
// SYSTEM Vartypes
//******************************************************************************
typedef union {
    struct{        
        unsigned fix        :5;        
        unsigned function   :5;
        unsigned product    :6;
    };    
    
    struct {
        unsigned version :16;
    };
} SYSTEM_FIRMWARE_VERSION;

void System_SetStatus(BYTE status);
BYTE System_GetStatus(void);
WORD System_GetFirmwareVersion(void);

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************

BYTE bfnReadStatusCoordinator(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnVersionCoordinatorLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

BYTE bfnResetCoordinatorLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

#endif	/* __SYSTEM_H__ */
