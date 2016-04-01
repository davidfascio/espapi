#include "mem24_1025_I2C.h"


BYTE *bBufferIICRead;

BYTE bfnIIC_MEM24_1025_Write(BYTE* bpData, WORD wAddress, WORD wDataSize){
    
    print_log("wAddress: %04X", wAddress);
    print_log("Data: ");
    print_buffer(bpData, wDataSize);
}

BYTE bfnIIC_MEM24_1025_Read(WORD wAddress, WORD wDataSize){
    
    print_log("wAddress: %04X", wAddress);
    print_log("wDataSize: %d", wDataSize);
}

void vfnIIC_MEM24_1025Driver(void){
    
}

BYTE bfnIICIsBusy(void){
    return FALSE;
}
