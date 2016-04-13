#include "mem24_1025_I2C.h"

BYTE mem24_buffer[0xFFFF];

//******************************************************************************
//* MEM24_1025_I2C Temporal Variables
//******************************************************************************
BYTE *bBufferIICRead;
BOOL * notification;

//******************************************************************************
//* MEM24_1025_I2C STATE MACHINE
//******************************************************************************
void vfnIICIdleState(void);
void vfnIICAddressToWriteState(void);
void vfnIICWriteDataState(void);
void vfnIICWriteDataCheckPageState(void);
void vfnIICAddressToReadState(void);
void vfnIICCMDToReadState(void);
void vfnIICReadDataState(void);
void vfnIICEndState(void);

typedef enum
{
    _IIC_IDLE_STATE = 0,
    _IIC_WRITE_ADD_STATE,
    _IIC_WRITE_DATA_STATE,
    _IIC_WRITE_DATA_CHECK_PAGE_STATE,
    _IIC_READ_ADD_STATE,
    _IIC_READ_CMD_STATE,
    _IIC_READ_DATA_STATE,
    _IIC_END_STATE
}MEM24_1025_IIC_Driver_States;

void (*const vfnaIICDriverState[])(void) =
{
    vfnIICIdleState,
    vfnIICAddressToWriteState,
    vfnIICWriteDataState,
    vfnIICWriteDataCheckPageState,
    vfnIICAddressToReadState,
    vfnIICCMDToReadState,
    vfnIICReadDataState,
    vfnIICEndState
};

sSM MEM24_1025_IIC_SM = { _IIC_IDLE_STATE,      /*  bActualState    */
                          _IIC_IDLE_STATE,      /*  bNextState      */
                          _IIC_IDLE_STATE,      /*  bPrevState      */
                          _IIC_IDLE_STATE};     /*  bErrorState     */


MEM24_1025_IIC_CONTROL mem241025IICControl;

//******************************************************************************
//* MEM24_1025_I2C API Function
//******************************************************************************
BOOL MEM24_1025_IIC_IsAPIBusy(void){
    
    return sFlags.MEM_Busy;
}

void MEM24_1025_IIC_SetAPIBusy(BOOL state){
    
    sFlags.MEM_Busy = state;
}

BOOL MEM24_1025_IIC_IsAPIError(void){
    
    return sFlags.IIC_Error;
}

void MEM24_1025_IIC_SetAPIError(BOOL state){
    
    sFlags.IIC_Error = state;
}

void MEM24_1025_IIC_ClearDataBuffer(MEM24_1025_IIC_CONTROL_PTR mem24_1025_control){
    
    if(mem24_1025_control == NULL)
        return;
    
    memset(mem24_1025_control->gbaIICDataBuffer, MEM24_1025_IIC_EMPTY_LOCATION_VALUE, MAX_IIC_BUFFER_SIZE);
}

void MEM24_1025_IIC_SetDataBuffer(MEM24_1025_IIC_CONTROL_PTR mem24_1025_control, BYTE * data, WORD dataSize){
    
    if(mem24_1025_control == NULL)
        return;
    
    if(dataSize > MAX_IIC_BUFFER_SIZE)
        return;
    
    memcpy(mem24_1025_control->gbaIICDataBuffer, data, dataSize);
}

BYTE bfnIIC_MEM24_1025_Write(BYTE* bpData, WORD wAddress, WORD wDataSize){
    
    WORD iwAddress;
    inverted_memcpy((BYTE *) &iwAddress,(BYTE *) &wAddress, sizeof(iwAddress));
    
    print_debug("wAddress: %04X", iwAddress);
    print_debug("Data: ");
    print_buffer(bpData, wDataSize);
    memcpy(mem24_buffer + wAddress, bpData, wDataSize);
    
    if(MEM24_1025_IIC_IsAPIBusy())
        return MEM24_1025_IIC_API_IS_BUSY_ERROR_CODE;
    
    mem241025IICControl._wIICDataIndex = 0;
    mem241025IICControl._wIICTxSizeMAX = wDataSize;
    
    mem241025IICControl._wIICDataIndex_Div = 0;
    
    MEM24_1025_IIC_SetAPIBusy(TRUE);
    MEM24_1025_IIC_SetAPIError(FALSE);

    // If error exists generate ErrorProcess 
    MEM24_1025_IIC_ClearDataBuffer(&mem241025IICControl);
    MEM24_1025_IIC_SetDataBuffer(&mem241025IICControl, bpData, wDataSize);
    
    
    
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

//******************************************************************************
//* MEM24_1025_I2C STATE MACHINE
//******************************************************************************
void vfnIIC_MEM24_1025Driver(void){
    
    vfnaIICDriverState[MEM24_1025_IIC_SM.bActualState]();    
}

void vfnIICIdleState(void)
{
    
}

BYTE bfnIICIsBusy(void){
    return FALSE;
}

void bfnIIC_MEM24_1025_Notification(void){
    
    * notification = FALSE;
}