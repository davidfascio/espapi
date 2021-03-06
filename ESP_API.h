/* 
 * File:   ESP_API.h
 * Author: fascio
 *
 * Created on 29 de marzo de 2016, 12:28
 */

#ifndef __ESP_API_H__
#define	__ESP_API_H__

//******************************************************************************
// Includes
//******************************************************************************
#include "ComSerialInterface.h"
#include "ESPComInterface.h"
#include "ESPCommands.h"
#include "SystemLog.h"

#include "System.h"
#include "SystemTime.h"
#include "ESPMeteringTable.h"
#include "ZDOControl.h"
#include "ZCLMeteringControl.h"

//******************************************************************************
// Defines
//******************************************************************************

#define ESP_API_NO_INVOKED_FUNCTION_CODE                                    (0)
#define ESP_API_FUNCTION_LIST_NULL      {ESP_API_NO_INVOKED_FUNCTION_CODE, NULL}


// Error Codes
#define ESP_API_NO_ERROR_CODE                                               (0)
#define ESP_API_REQUEST_NULL_ERROR_CODE                                     (1)
#define ESP_API_FUNCTION_LIST_IS_NOT_FOUND_ERROR_CODE                       (2)

//******************************************************************************
// ESP_COM_INTERFACE Data types
//******************************************************************************

typedef BYTE(* ESP_API_FunctionCallBack)(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize);

typedef struct {
    BYTE functionCode;
    ESP_API_FunctionCallBack functionCallBack;
} ESP_API_FUNCTION_LIST, * ESP_API_FUNCTION_LIST_PTR;


//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************

ESP_API_FUNCTION_LIST_PTR ESP_API_GetESPAPIFunctionList(BYTE functionCode);
BYTE ESP_API_ResponseProcess(ESP_COM_INTERFACE_REQUEST_PTR requestControl);
void ESP_API_ReceivedHandler(void);

#endif	/* __ESP_API_H__ */
