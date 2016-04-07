#include "ZDOControl.h"

BYTE ZDOControl_SetPANID(WORD panid){
    
    print_debug("New PANID: %04X", panid);
    return SUCCESS_CMD;
}

BYTE ZDOControl_SetLocalPermitJoin(BYTE permit){
 
    print_debug("LocalPermitJoin: %d", permit);
    return SUCCESS_CMD;
}

BYTE bfnReadChannelLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize){
    
    BYTE currentChannel;
    BYTE * dataResponse_ptr = dataResponse;

    currentChannel = 14;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    memcpy(dataResponse_ptr,(BYTE *)  &currentChannel, sizeof (currentChannel));
    dataResponse_ptr += sizeof (currentChannel);

    * dataResponseSize = dataResponse_ptr - dataResponse;
    
    return ESP_DOES_NOT_WAIT_ANSWER;
}

BYTE bfnReadPANIDLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize){
    
    WORD currentPANID;
    BYTE * dataResponse_ptr = dataResponse;

    currentPANID = 0x1234;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    memcpy(dataResponse_ptr,(BYTE *)  &currentPANID, sizeof (currentPANID));
    dataResponse_ptr += sizeof (currentPANID);

    * dataResponseSize = dataResponse_ptr - dataResponse;
    
    return ESP_DOES_NOT_WAIT_ANSWER;
}

BYTE bfnWritePANIDLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize){
    
    WORD currentPANID;
    BYTE error_code;
    BYTE * dataResponse_ptr = dataResponse;

    if(dataRequestSize != sizeof(currentPANID))
        return ESP_DATA_SIZE_ERROR_CODE;
    
    memcpy((BYTE *) &currentPANID, dataRequest, dataRequestSize);
    
    error_code = ZDOControl_SetPANID(currentPANID);

    * dataResponse_ptr = error_code;
    * dataResponseSize = sizeof (BYTE);   
    
    return ESP_DOES_NOT_WAIT_ANSWER;
}

BYTE bfnWritePermitJoinLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize){
    
    BYTE permit;
    BYTE error_code;
    BYTE * dataResponse_ptr = dataResponse;

    if(dataRequestSize != sizeof(permit))
        return ESP_DATA_SIZE_ERROR_CODE;
    
    memcpy((BYTE *) &permit, dataRequest, dataRequestSize);
    
    error_code = ZDOControl_SetLocalPermitJoin(permit);

    * dataResponse_ptr = error_code;
    * dataResponseSize = sizeof (BYTE);   
    
    return ESP_DOES_NOT_WAIT_ANSWER;
}