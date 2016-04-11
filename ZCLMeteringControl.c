//******************************************************************************
//* File: ZCLMeteringControl.c
//


//******************************************************************************
// Includes
//******************************************************************************
#include "ZCLMeteringControl.h"

//******************************************************************************
// ZCL METERING CONTROL Variables
//******************************************************************************
WORD _wTimeSendAutomaticReadings=_600_SECONDS;

//******************************************************************************
// ZCLMeteringControl Function Prototypes
//******************************************************************************
BYTE ZCLMeteringControl_SetAutomaticReadingTime(WORD automaticReadingTime){
    
    _wTimeSendAutomaticReadings = automaticReadingTime;
    print_debug("Automatic Reading Time: %d",  _wTimeSendAutomaticReadings);
    return SUCCESS_CMD;    
}

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************
BYTE bfnReadFrecuencyAutoReadingsLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize){
 
    BYTE * dataResponse_ptr = dataResponse;
    
    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    memcpy(dataResponse_ptr,(BYTE *)  &_wTimeSendAutomaticReadings, sizeof (_wTimeSendAutomaticReadings));
    dataResponse_ptr += sizeof (_wTimeSendAutomaticReadings);

    * dataResponseSize = dataResponse_ptr - dataResponse;
    
    return ESP_DOES_NOT_WAIT_ANSWER;
    
}

BYTE bfnWriteFrecuencyAutoReadingsLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize){
    
    WORD automaticReadingTime;
    BYTE error_code;
    BYTE * dataResponse_ptr = dataResponse;

    
    if(dataRequestSize != sizeof(automaticReadingTime))
        return ESP_DATA_SIZE_ERROR_CODE;

    memcpy((BYTE *) &automaticReadingTime, dataRequest, sizeof(automaticReadingTime));
    
    error_code = ZCLMeteringControl_SetAutomaticReadingTime(automaticReadingTime);
    
    * dataResponse_ptr = error_code;
    * dataResponseSize = sizeof (error_code);

    return ESP_DOES_NOT_WAIT_ANSWER;    
}
