//******************************************************************************
// Includes
//******************************************************************************

#include "System.h"

//******************************************************************************
// Varibles
//******************************************************************************

BYTE SystemStatus = RESET_START;

//******************************************************************************
// ESP_API Function 
//******************************************************************************

void System_SetStatus(BYTE status){
    
    SystemStatus = status;
}

BYTE System_GetStatus(void){
    
    return SystemStatus;
}

BYTE bfnReadStatusCoordinator(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize){
    
    * dataResponse = System_GetStatus();
    * dataResponseSize = sizeof(BYTE);
}
