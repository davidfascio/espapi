#include "mem24_1025_I2C.h"

BYTE mem24_buffer[0xFFFF];

BYTE *bBufferIICRead;
BOOL * notification;

BYTE bfnIIC_MEM24_1025_Write(BYTE* bpData, WORD wAddress, WORD wDataSize){
    
    WORD iwAddress;
    inverted_memcpy((BYTE *) &iwAddress,(BYTE *) &wAddress, sizeof(iwAddress));
    
    print_debug("wAddress: %04X", iwAddress);
    print_debug("Data: ");
    print_buffer(bpData, wDataSize);
    memcpy(mem24_buffer + wAddress, bpData, wDataSize);
    
}

BYTE bfnIIC_MEM24_1025_Read(WORD wAddress, WORD wDataSize, BOOL * isWaitingForResponse){
    
    WORD iwAddress;
    inverted_memcpy((BYTE *) &iwAddress,(BYTE *) &wAddress, sizeof(iwAddress));
    
    print_debug("wAddress: %04X", iwAddress);
    print_debug("wDataSize: %d", wDataSize);
    
    memcpy(bBufferIICRead, mem24_buffer + wAddress,  wDataSize);
    notification = isWaitingForResponse;
    vfnOneShotReload(TIME_OUT_METER_RESPONSE_ONESHOT, _1_MSEC_);
            
}

void vfnIIC_MEM24_1025Driver(void){
    
}

BYTE bfnIICIsBusy(void){
    return FALSE;
}

void bfnIIC_MEM24_1025_Notification(void){
    
    * notification = FALSE;
}