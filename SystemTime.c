#include "SystemTime.h"
#include "SystemLog.h"

DWORD SystemTime_GetTime(void){
    time_t sec;
    sec = time(NULL);
    
    return sec - ZIGBEE_EPOCH_TIME;
}
BYTE SystemTime_SetTime(SYSTEM_TIME time){
    
    print_debug("TimeZone :d, Time: %d",  time.timeZone, time.time);
    return SUCCESS_CMD;
}

BYTE bfnReadTimeDateLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize){
    
    DWORD currentTime;
    BYTE * dataResponse_ptr = dataResponse;

    currentTime = SystemTime_GetTime();

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    memcpy(dataResponse_ptr,(BYTE *)  &currentTime, sizeof (currentTime));
    dataResponse_ptr += sizeof (currentTime);

    * dataResponseSize = dataResponse_ptr - dataResponse;
    
    return ESP_DOES_NOT_WAIT_ANSWER;    
}

BYTE bfnWriteTimeDateLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize){
    
    SYSTEM_TIME currentTime;
    BYTE error_code;
    BYTE * dataResponse_ptr = dataResponse;

    print("DATA_SIZE: %d", sizeof(currentTime));
    if(dataRequestSize != sizeof(currentTime))
        return ESP_DATA_SIZE_ERROR_CODE;

    memcpy((BYTE *) &currentTime, dataRequest, sizeof(currentTime));
    
    error_code = SystemTime_SetTime(currentTime);
    
    * dataResponse_ptr = error_code;
    * dataResponseSize = sizeof (error_code);

    return ESP_DOES_NOT_WAIT_ANSWER;
}
