#include "mem24_1025_I2C.h"
#include "Utility.h"


BYTE *bBufferIICRead;

BYTE bfnIIC_MEM24_1025_Write(BYTE* bpData, WORD wAddress, WORD wDataSize){
    
    WORD iwAddress;
    inverted_memcpy((BYTE *) &iwAddress,(BYTE *) &wAddress, sizeof(iwAddress));
    
    print_debug("wAddress: %04X", iwAddress);
    print_debug("Data: ");
    print_buffer(bpData, wDataSize);
}

BYTE bfnIIC_MEM24_1025_Read(WORD wAddress, WORD wDataSize){
    
    WORD iwAddress;
    inverted_memcpy((BYTE *) &iwAddress,(BYTE *) &wAddress, sizeof(iwAddress));
    
    print_debug("wAddress: %04X", iwAddress);
    print_debug("wDataSize: %d", wDataSize);
}

void vfnIIC_MEM24_1025Driver(void){
    
}

BYTE bfnIICIsBusy(void){
    return FALSE;
}
