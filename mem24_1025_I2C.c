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

WORD MEM24_1025_I2C_GetAddressByPacketPageSend(void){

    WORD address = mem241025IICControl.wAddress;
    
    if(mem241025IICControl._bPacketPageSend){
        
        address += ( (mem241025IICControl._bPacketPageSend * MAX_IIC_WRITE_PAGE_SIZE) - 
                     (address % MAX_IIC_WRITE_PAGE_SIZE ) );
    }
    
    return address;
}

void MEM24_1025_I2C_UpdateTxPagingValues(WORD wAddress){
    
    WORD wRestAdd;
    WORD wFaltAdd;
    
    wRestAdd = wAddress % MAX_IIC_WRITE_PAGE_SIZE;
    wFaltAdd = MAX_IIC_WRITE_PAGE_SIZE - wRestAdd;
    
    if(mem241025IICControl._wIICTxSizeMAX >= wFaltAdd){
        
        mem241025IICControl._wIICTxSize = wFaltAdd;
        mem241025IICControl._wIICTxSizeMAX -= wFaltAdd;
    } else {
        
        mem241025IICControl._wIICTxSize = mem241025IICControl._wIICTxSizeMAX;
    }
}

#define MEM24_1025_I2C_SELECT_ADDRESS_FRAME_SIZE                            (3)

BYTE MEM24_1025_I2C_SendAddress(BYTE chipSelect, WORD wAddress){
    
    BYTE buffer[MEM24_1025_I2C_SELECT_ADDRESS_FRAME_SIZE];    
    BYTE * buffer_ptr = buffer;
    WORD bufferSize;
    
    memcpy(buffer_ptr, &chipSelect, sizeof(BYTE));
    buffer_ptr += sizeof(BYTE);
    
    memcpy(buffer_ptr, (BYTE *) &wAddress, sizeof(WORD));
    buffer_ptr += sizeof(WORD);
    
    bufferSize =  buffer_ptr - buffer;
    
    
    
    //return TRUE;
    return bfnI2CTxRxBuffer(buffer, bufferSize, NULL, 0, TRUE, FALSE);
}

BYTE MEM24_1025_I2C_SendData(BYTE * data, WORD dataSize){

    //! return bfnI2CTxRxBuffer(data, dataSize, NULL, 0, FALSE, TRUE);
    
    
    return bfnI2CTxRxBuffer(data, dataSize, NULL, 0, FALSE, TRUE);
}

BYTE bfnIIC_MEM24_1025_Write(BYTE* bpData, WORD wAddress, WORD wDataSize){
    
    WORD iwAddress;
    inverted_memcpy((BYTE *) &iwAddress,(BYTE *) &wAddress, sizeof(iwAddress));
    
    
    
    if(MEM24_1025_IIC_IsAPIBusy())
        return MEM24_1025_IIC_API_IS_BUSY_ERROR_CODE;
    
    print_debug("wAddress: %04X", iwAddress);
    print_debug("Data: ");
    print_buffer(bpData, wDataSize);
    memcpy(mem24_buffer + iwAddress, bpData, wDataSize);
    
    mem241025IICControl._wIICDataIndex = 0;
    mem241025IICControl._wIICTxSizeMAX = wDataSize;
    
    mem241025IICControl._wIICDataIndex_Div = 0;
    
    MEM24_1025_IIC_SetAPIBusy(TRUE);
    MEM24_1025_IIC_SetAPIError(FALSE);

    // If error exists generate ErrorProcess 
    MEM24_1025_IIC_ClearDataBuffer(&mem241025IICControl);
    MEM24_1025_IIC_SetDataBuffer(&mem241025IICControl, bpData, wDataSize);
    
    mem241025IICControl.wAddress = iwAddress;
    
    MEM24_1025_I2C_SetStateMachine(_IIC_WRITE_ADD_STATE, _IIC_WRITE_DATA_CHECK_PAGE_STATE );
    
    mem241025IICControl._wIICDataIndex_Div = (mem241025IICControl.wAddress % MAX_IIC_WRITE_PAGE_SIZE);
    mem241025IICControl._wIICDataIndex_Div += mem241025IICControl._wIICTxSizeMAX;
    mem241025IICControl._wIICDataIndex_Res = (mem241025IICControl._wIICDataIndex_Div) % MAX_IIC_WRITE_PAGE_SIZE;
    mem241025IICControl._wIICDataIndex_Div /= MAX_IIC_WRITE_PAGE_SIZE;
    
    //! vfnRTCChipReset();
    
    return TRUE;
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

void MEM24_1025_I2C_SetStateMachine(BYTE actualState, BYTE nextState){
    
    MEM24_1025_IIC_SM.bActualState = actualState;
    MEM24_1025_IIC_SM.bNextState = nextState;
    
    vfnEventEnable(IIC_EVENT);
    vfnEventPost(IIC_EVENT);
}

void vfnIIC_MEM24_1025Driver(void){
    
    vfnaIICDriverState[MEM24_1025_IIC_SM.bActualState]();    
}

void vfnIICIdleState(void)
{
    print_debug();
}

void vfnIICAddressToWriteState(void){
    
    WORD wAddress;
    BYTE error_code;
    
    //! if(MEM24_1025_IIC_IsAPIError())
    //!     return ERROR_CODE;
    
    
    wAddress = MEM24_1025_I2C_GetAddressByPacketPageSend();
    //!error_code = MEM24_1025_I2C_UpdateTxPagingValues(wAddress);
    
    
    //! if(error_code == NO_DATA_TO_TRANSMIT)
    //!     return ERROR_CODE
    
    error_code = MEM24_1025_I2C_SendAddress(CHIP_SELECT_MEM, wAddress);
    
     if(error_code == I2C_DRIVER_CONTROL_API_IS_BUSY_ERROR_CODE)
        return;
    
    MEM24_1025_I2C_UpdateTxPagingValues(wAddress);
    MEM24_1025_I2C_SetStateMachine(_IIC_WRITE_DATA_STATE, _IIC_WRITE_DATA_CHECK_PAGE_STATE);
}

void vfnIICWriteDataState(void){
    
    
    BYTE * data_ptr = mem241025IICControl.gbaIICDataBuffer;
    BYTE error_code;
    
    //! if bfnI2CError();
    // return ERROR_CODE_PROCESS
    
    data_ptr += mem241025IICControl._wIICDataIndex;
    
    error_code = MEM24_1025_I2C_SendData(data_ptr, mem241025IICControl._wIICTxSize);
    
    if(error_code == I2C_DRIVER_CONTROL_API_IS_BUSY_ERROR_CODE) // I2C_DRIVER_CONTROL_API_IS_BUSY_ERROR_CODE
        return;
    
    mem241025IICControl._wIICDataIndex += mem241025IICControl._wIICTxSize;
    
    MEM24_1025_I2C_SetStateMachine(_IIC_WRITE_DATA_CHECK_PAGE_STATE, _IIC_WRITE_DATA_CHECK_PAGE_STATE);
    //!print_debug();
    
}

void vfnIICWriteDataCheckPageState(void){
    
    //! if(bfnI2CError())
    // return ERROR_CODE_PROCESS
    
    //! bfnIsI2CBusy
    // return ERROR_CODE_PROCESS
    
    
    if(mem241025IICControl._bPacketPageSend < mem241025IICControl._wIICDataIndex_Div)
    {
       mem241025IICControl._bPacketPageSend++;
       MEM24_1025_I2C_SetStateMachine(_IIC_WRITE_ADD_STATE, _IIC_WRITE_DATA_CHECK_PAGE_STATE);       
    } else {
        
        MEM24_1025_I2C_SetStateMachine(_IIC_END_STATE, _IIC_END_STATE);       
    }
    //print_debug();
}

void vfnIICAddressToReadState(void){
    
    print_debug();
}

void vfnIICCMDToReadState(void){
    
    print_debug();
}

void vfnIICReadDataState(void){
    
    print_debug();
}

void vfnIICEndState(void){
    
    //! MEM24_1025_IIC_Clear();
    vfnEventClear(IIC_EVENT);
    vfnEventDisable(IIC_EVENT);
    MEM24_1025_IIC_SetAPIBusy(FALSE);
}

BYTE bfnIICIsBusy(void){
    return MEM24_1025_IIC_IsAPIBusy();
}

void bfnIIC_MEM24_1025_Notification(void){
    
    * notification = FALSE;
}