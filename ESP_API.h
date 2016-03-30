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
// ESP_COM_INTERFACE Data types
//******************************************************************************
typedef BYTE (* ESP_API_FunctionCallBack )( BYTE * inputBuffer, WORD inputBufferSize, 
                                            BYTE * outputBuffer, WORD * outputBufferSize);

typedef struct {
    BYTE functionCode;
    ESP_API_FunctionCallBack functionCallBack;
} ESP_API_FUNCTION_LIST, * ESP_API_FUNCTION_LIST_PTR;


void ESP_API_ReceivedHandler(void);

#endif	/* __ESP_API_H__ */
