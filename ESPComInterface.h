/* 
 * File:   ESPComInterface.h
 * Author: fascio
 *
 * Created on 29 de marzo de 2016, 11:13
 */

#ifndef __ESP_COM_INTERFACE_H__
#define	__ESP_COM_INTERFACE_H__

//******************************************************************************
// Includes
//******************************************************************************

#include "GenericTypeDefs.h"
#include "Utility.h"

//******************************************************************************
// ESP_COM_INTERFACE Defines
//******************************************************************************

#define ESP_COM_INTERFACE_MAC_ADDRESS_SIZE                           (8)
#define ESP_COM_INTERFACE_RECEIVED_DATA_MAX_SIZE                     (20)

#define ESP_COM_INTERFACE_START_OF_HEADER                            (0x55CC)
#define ESP_COM_INTERFACE_END_OF_HEADER                              (0x33CC)

//******************************************************************************
//                      E S P  C O M  P R O T O C O L
//******************************************************************************
//    WORD startOfHeader;
//    WORD frameSize;
//    BYTE macAddress[ESP_COM_INTERFACE_MAC_ADDRESS_SIZE];   
//    BYTE functionCode;
//    BYTE dataReceived[ESP_COM_INTERFACE_DATA_RECEIVED_MAX_SIZE];
//    WORD dataReceivedSize;
//    WORD crcValue;
//    WORD endOfHeader;


#define ESP_COM_INTERFACE_START_OF_HEADER_SIZE                       (2)
#define ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE                  (2)
#define ESP_COM_INTERFACE_FUNCTION_CODE_HEADER_SIZE                  (1)
#define ESP_COM_INTERFACE_CRC_HEADER_SIZE                            (2)
#define ESP_COM_INTERFACE_END_OF_HEADER_SIZE                         (2)

//******************************************************************************
// Error Code
//******************************************************************************

#define ESP_COM_INTERFACE_NO_ERROR_CODE                             (0)
#define ESP_COM_INTERFACE_START_OF_HEADER_ERROR_CODE                (-1)
#define ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_ERROR_CODE           (-2)
#define ESP_COM_INTERFACE_MAC_ADDRESS_ERROR_CODE                    (-3)
#define ESP_COM_INTERFACE_CRC_ERROR_CODE                            (-4)
#define ESP_COM_INTERFACE_END_OF_HEADER_ERROR_CODE                  (-5)
//******************************************************************************
// ESP_COM_INTERFACE Function Prototypes
//******************************************************************************

BYTE ESPComInterface_ReceivedHandler(BYTE * buffer, WORD bufferSize);

#endif	/* __ESP_COM_INTERFACE_H__ */

