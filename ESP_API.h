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
#include "ESPComInterface.h"
#include "SystemLog.h"

//******************************************************************************
// Defines
//******************************************************************************
#define ESP_API_NO_INVOKED_FUNCTION_CODE                                    (0)
#define ESP_API_FUNCTION_LIST_NULL      {ESP_API_NO_INVOKED_FUNCTION_CODE, NULL}

//******************************************************************************
// ESP_COM_INTERFACE Data types
//******************************************************************************
typedef BYTE(* ESP_API_FunctionCallBack)(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

typedef struct {
    BYTE functionCode;
    ESP_API_FunctionCallBack functionCallBack;
} ESP_API_FUNCTION_LIST, * ESP_API_FUNCTION_LIST_PTR;

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************
void ESP_API_ResponseProcess(BYTE functionCode, BYTE status_code);
void ESP_API_ReceivedHandler(void);

#endif	/* __ESP_API_H__ */
