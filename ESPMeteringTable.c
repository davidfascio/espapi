#include "ESPMeteringTable.h"

BYTE bfnReadDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize){
    
    WORD wIndexData;
    WORD wDataSize;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;
    
    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof(BYTE);
    
    wIndexData = wfnIndexConsutl(DEVICESTABLE);
    
    if(wIndexData){
        
        wDataSize = (wIndexData * sizeof(DEV_LIST));
        memcpy(dataResponse_ptr, (BYTE *) &wDataSize, sizeof(WORD));
        dataResponse_ptr += sizeof(WORD);
        
        
        
        // bfnBuffer_Table_Meter(wIndexData, sizeof(READING_LIST), &bpOutputData[0]); 
        answer_code = WAIT_ANSWER;
    }
    
    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
    
    
}


BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize){
    
    WORD wIndexData;
    WORD wDataSize;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;
    
    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof(BYTE);
    
    wIndexData = wfnIndexConsutl(METERSTABLE);
    
    if(wIndexData){
        
        wDataSize = (wIndexData * sizeof(READING_LIST));
        memcpy(dataResponse_ptr, (BYTE *) &wDataSize, sizeof(WORD));
        dataResponse_ptr += sizeof(WORD);
        
        
        
        // bfnBuffer_Table_Meter(wIndexData, sizeof(READING_LIST), &bpOutputData[0]); 
        answer_code = WAIT_ANSWER;
    }
    
    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

