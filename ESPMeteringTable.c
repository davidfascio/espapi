#include "ESPMeteringTable.h"

BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize){
    
    BYTE * dataResponse_ptr = dataResponse;
    
    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof(BYTE);
    
    
    
    
    
}
