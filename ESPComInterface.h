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
#include "SystemLog.h"
#include "ComSerialInterface.h"

//******************************************************************************
// ESP_COM_INTERFACE Defines
//******************************************************************************

#define ESP_COM_INTERFACE_MAC_ADDRESS_SIZE                           (8)
#define ESP_COM_INTERFACE_RECEIVED_DATA_MAX_SIZE                     (50)
#define ESP_COM_INTERFACE_FRAME_MAX_SIZE                            (255)

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
#define ESP_COM_INTERFACE_START_OF_HEADER_ERROR_CODE                (1)
#define ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_ERROR_CODE           (2)
#define ESP_COM_INTERFACE_MAC_ADDRESS_ERROR_CODE                    (3)
#define ESP_COM_INTERFACE_RECEIVED_DATA_ERROR_CODE                  (4)
#define ESP_COM_INTERFACE_CRC_ERROR_CODE                            (5)
#define ESP_COM_INTERFACE_END_OF_HEADER_ERROR_CODE                  (6)

//******************************************************************************
// ESP_COM_INTERFACE DataTypes
//******************************************************************************
typedef struct {
    BYTE invokedFunctionCode;
    BYTE data[ESP_COM_INTERFACE_RECEIVED_DATA_MAX_SIZE];
    WORD dataSize;
} ESP_COM_INTERFACE_RESPONSE, * ESP_COM_INTERFACE_RESPONSE_PTR;

void ESPComInterfaceResponse_Setup(ESP_COM_INTERFACE_RESPONSE_PTR responseControl, BYTE invokedFunctionCode, BYTE * data, WORD dataSize);
void ESPComInterfaceResponse_Clear(ESP_COM_INTERFACE_RESPONSE_PTR responseControl);

void ESPComInterfaceResponse_SetInvokedFunctionCode(ESP_COM_INTERFACE_RESPONSE_PTR responseControl, BYTE invokedFunctionCode);
void ESPComInterfaceResponse_SetData(ESP_COM_INTERFACE_RESPONSE_PTR responseControl, BYTE * data, WORD dataSize);

BYTE ESPComInterfaceResponse_GetInvokedFunctionCode(ESP_COM_INTERFACE_RESPONSE_PTR responseControl);
WORD ESPComInterfaceResponse_GetData(ESP_COM_INTERFACE_RESPONSE_PTR responseControl, BYTE * data, WORD  dataSize);

void ESPComInterfaceResponse_Print(ESP_COM_INTERFACE_RESPONSE_PTR responseControl);
//******************************************************************************
// ESP_COM_INTERFACE Function Prototypes
//******************************************************************************

void ESPComInterface_Setup(BYTE one_shot_event, BYTE time_delay);

BOOL ESPComInterface_BuildFrame(WORD startOfHeader, BYTE * macAddress, BYTE functionCode, 
                                BYTE * data, WORD dataSize, WORD endOfHeader, BYTE * frame, WORD * frameLen);

void ESPComInterface_SendCustomFrame(WORD startOfHeader, BYTE * macAddress, BYTE functionCode,
        BYTE * data, WORD dataSize, WORD endOfHeader);

void ESPComInterface_SendFrame( BYTE functionCode, BYTE * data, WORD dataSize);

BYTE ESPComInterface_ReceivedHandler(BYTE * buffer, WORD bufferSize, ESP_COM_INTERFACE_RESPONSE_PTR responseControl);


#endif	/* __ESP_COM_INTERFACE_H__ */

