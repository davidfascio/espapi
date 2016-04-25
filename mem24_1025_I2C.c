//******************************************************************************
//* Includes
//******************************************************************************
#include "mem24_1025_I2C.h"

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


MEM24_1025_I2C_CONTROL mem241025IICControl;

//******************************************************************************
//* MEM24_1025_IIC_CONTROL Functions
//******************************************************************************

void MEM24_1025_I2C_SetAddress(WORD _wAddress){
 
    mem241025IICControl.wAddress = _wAddress;
}

WORD MEM24_1025_I2C_GetAddress(void){
 
    return mem241025IICControl.wAddress;
}

void MEM24_1025_I2C_SetTxSize(WORD _wIICTxSize){
    
    mem241025IICControl._wIICTxSize =  _wIICTxSize;
}

WORD MEM24_1025_I2C_GetTxSize(void){
    
    return mem241025IICControl._wIICTxSize;
}

void MEM24_1025_I2C_SetTxSizeMAX(WORD _wIICTxSizeMAX){
    
    mem241025IICControl._wIICTxSizeMAX = _wIICTxSizeMAX;
}

WORD MEM24_1025_I2C_GetTxSizeMAX(void){
    
    return mem241025IICControl._wIICTxSizeMAX;
}

void MEM24_1025_I2C_SetRxSize(WORD _wIICRxSize){
    
    mem241025IICControl._wIICRxSize = _wIICRxSize;
}

WORD MEM24_1025_I2C_GetRxSize(void){
    
    return mem241025IICControl._wIICRxSize;
}

void MEM24_1025_I2C_SetReceptionBuffer(BYTE * receptionBuffer, WORD _wIICRxSize){
    
    if(receptionBuffer == NULL)
        return;
    
    mem241025IICControl.receptionBuffer = receptionBuffer;
    MEM24_1025_I2C_SetRxSize(_wIICRxSize);
}

BYTE * MEM24_1025_I2C_GetReceptionBuffer(void){
    
    return mem241025IICControl.receptionBuffer;
}

void MEM24_1025_I2C_SetDataIndex(WORD _wIICDataIndex){

    mem241025IICControl._wIICDataIndex = _wIICDataIndex;
}

WORD MEM24_1025_I2C_GetDataIndex(void){
    
    return mem241025IICControl._wIICDataIndex;
}

void MEM24_1025_I2C_SetDataIndexDiv(WORD _wIICDataIndex_Div){

    mem241025IICControl._wIICDataIndex_Div = _wIICDataIndex_Div;
}

WORD MEM24_1025_I2C_GetDataIndexDiv(void){
    
    return mem241025IICControl._wIICDataIndex_Div;
}

void MEM24_1025_I2C_SetDataIndexRes(WORD _wIICDataIndex_Res){
    
    mem241025IICControl._wIICDataIndex_Res = _wIICDataIndex_Res;
}

WORD MEM24_1025_I2C_GetDataIndexRes(void){
    
    return mem241025IICControl._wIICDataIndex_Res;
}

void MEM24_1025_I2C_SetPacketPageSend(BYTE _bPacketPageSend){

    mem241025IICControl._bPacketPageSend = _bPacketPageSend;
}

BYTE MEM24_1025_I2C_GetPacketPageSend(void){
    
    return mem241025IICControl._bPacketPageSend;
}

void MEM24_1025_I2C_SetPagingAddress(WORD _wPagingAddress){
    
    mem241025IICControl._wPagingAddress = _wPagingAddress;
}

WORD MEM24_1025_I2C_GetPagingAddress(void){
    
    return mem241025IICControl._wPagingAddress;
}

void MEM24_1025_I2C_ClearDataBuffer(void){
    
    memset(mem241025IICControl.gbaIICDataBuffer, MEM24_1025_I2C_EMPTY_LOCATION_VALUE, MAX_IIC_BUFFER_SIZE);
}

void MEM24_1025_I2C_SetDataBuffer( BYTE * data, WORD dataSize){
    
    if(dataSize > MAX_IIC_BUFFER_SIZE)
        return;
    
    memcpy(mem241025IICControl.gbaIICDataBuffer, data, dataSize);
}

BYTE * MEM24_1025_I2C_GetDataBuffer(void){
    
    return mem241025IICControl.gbaIICDataBuffer;
}

void MEM24_1025_I2C_SetRetriesCount(BYTE retriesCount){
    
    mem241025IICControl.retriesCount = retriesCount;
}
BYTE MEM24_1025_I2C_GetRetriesCount(void){
    
    return mem241025IICControl.retriesCount;
}

void MEM24_1025_I2C_SetupNotification(BOOL * notification){
    
    mem241025IICControl.notification = notification;
}

void MEM24_1025_I2C_SetNotification(BOOL notification){
    
    * (mem241025IICControl.notification) = notification;
}

BOOL MEM24_1025_I2C_GetNotification(void){
    
    return *( mem241025IICControl.notification);
}

void MEM24_1025_I2C_Clear(void){
   
    memset(&mem241025IICControl, 0, sizeof(MEM24_1025_I2C_CONTROL));
}

//******************************************************************************
//* MEM24_1025_I2C API Function
//******************************************************************************

BOOL MEM24_1025_I2C_IsAPIBusy(void){
    
    return sFlags.MEM_Busy;
}

void MEM24_1025_I2C_SetAPIBusy(BOOL state){
    
    sFlags.MEM_Busy = state;
}

BYTE bfnIICIsBusy(void){
    return MEM24_1025_I2C_IsAPIBusy();
}

BYTE API_MEM24_1025_I2C_Write(BYTE* bpData, WORD wAddress, WORD wDataSize){
    
    WORD iwAddress;
    inverted_memcpy((BYTE *) &iwAddress,(BYTE *) &wAddress, sizeof(iwAddress));
    
    if(MEM24_1025_I2C_IsAPIBusy())
        return MEM24_1025_I2C_API_IS_BUSY_ERROR_CODE;
    
    print_debug("wAddress: %04X", iwAddress);
    print_debug("Data: ");
    print_buffer(bpData, wDataSize);
    //memcpy(mem24_buffer + iwAddress, bpData, wDataSize);
    
    mem241025IICControl._wIICDataIndex = 0;
    mem241025IICControl._wIICTxSizeMAX = wDataSize;    
    mem241025IICControl._wIICDataIndex_Div = 0;
    
    MEM24_1025_I2C_SetAPIBusy(TRUE);    

    // If error exists generate ErrorProcess 
    MEM24_1025_I2C_ClearDataBuffer();
    MEM24_1025_I2C_SetDataBuffer(bpData, wDataSize);
    
    mem241025IICControl.wAddress = iwAddress;
    
    MEM24_1025_I2C_SetStateMachine(_IIC_WRITE_ADD_STATE, _IIC_WRITE_DATA_CHECK_PAGE_STATE );
    
    mem241025IICControl._wIICDataIndex_Div = (mem241025IICControl.wAddress % MAX_IIC_WRITE_PAGE_SIZE);
    mem241025IICControl._wIICDataIndex_Div += mem241025IICControl._wIICTxSizeMAX;
    mem241025IICControl._wIICDataIndex_Res = (mem241025IICControl._wIICDataIndex_Div) % MAX_IIC_WRITE_PAGE_SIZE;
    mem241025IICControl._wIICDataIndex_Div /= MAX_IIC_WRITE_PAGE_SIZE;
    
    //! vfnRTCChipReset();
    MEM24_1025_I2C_ResetRetriesCounter();
    
    return TRUE;
}

BYTE API_MEM24_1025_I2C_Read(WORD wAddress, BYTE * receptionBuffer, WORD wDataSize, BOOL * isWaitingForResponse){
    
    WORD iwAddress;
    inverted_memcpy((BYTE *) &iwAddress,(BYTE *) &wAddress, sizeof(iwAddress));
    
    if(MEM24_1025_I2C_IsAPIBusy())
        return MEM24_1025_I2C_API_IS_BUSY_ERROR_CODE;
    
    MEM24_1025_I2C_SetAPIBusy(TRUE);
    
    print_debug("wAddress: %04X", iwAddress);
    print_debug("wDataSize: %d", wDataSize);
    
    
    //! mem241025IICControl.wAddress = iwAddress;
    MEM24_1025_I2C_SetAddress(iwAddress);
    
    //! mem241025IICControl._wIICRxSize = wDataSize;
    //! mem241025IICControl.receptionBuffer = receptionBuffer;
    MEM24_1025_I2C_SetReceptionBuffer(receptionBuffer, wDataSize);
     
    
    //! memcpy(bBufferIICRead, mem24_buffer + wAddress,  wDataSize);
    
    //! vfnRTCChipReset();
    
    MEM24_1025_I2C_SetStateMachine(_IIC_READ_ADD_STATE, _IIC_READ_ADD_STATE );
   
    //! notification = isWaitingForResponse;
    MEM24_1025_I2C_SetupNotification(isWaitingForResponse);    
   
    //! * notification = FALSE;
    MEM24_1025_I2C_SetNotification(FALSE);
    
    //! vfnOneShotReload(TIME_OUT_METER_RESPONSE_ONESHOT, _1_MSEC_);            
    return TRUE;
}

void MEM24_1025_I2C_ResetRetriesCounter(void){
    
    if(I2CDriverControl_GetTxRxStatus() != I2C_DRIVER_INITIALIZED){
     
        I2CDriverControl_SetTxRxStatus(I2C_DRIVER_STOPPED);
    }
    
    mem241025IICControl.retriesCount = 0;
}

void MEM24_1025_I2C_ErrorProcess(void){
    
    MEM24_1025_I2C_Clear();
    MEM24_1025_I2C_SetStateMachine(_IIC_END_STATE, _IIC_END_STATE);
}

void MEM24_1025_I2C_SendTxRxDataProcess(    BYTE * txData, 
                                            WORD txDataSize, 
                                            BYTE * rxData, 
                                            WORD rxDataSize, 
                                            BOOL isFirstFrame, 
                                            BOOL isLastFrame, 
                                            UpdateCallBack_Function updateCallBackFunction, 
                                            BYTE nextState ) {
    
    // IF TRANSMITION ENDED AND THERE IS NOT ACK ERROR CODE THEN
    if( (I2CDriverControl_GetTxRxStatus() == I2C_DRIVER_ENDED ) &&
       !(I2CDriverControl_GetFlagsByIndex( I2C_NO_ACK_ERROR )) ) {
        
        if( updateCallBackFunction != NULL ){
            
            updateCallBackFunction();            
        }
                    
        MEM24_1025_I2C_ResetRetriesCounter();        
        MEM24_1025_I2C_SetStateMachine(nextState, _IIC_END_STATE);
        
        return;     
    }
    
    //  IF TRANSMITION DIFFERENT TO INITIALIZED  AND RETRIES IS MINOR TO MAX NUMBER OF RETRIES THEN
    if(     (I2CDriverControl_GetTxRxStatus() != I2C_DRIVER_INITIALIZED ) &&
            (mem241025IICControl.retriesCount < MEM24_1025_I2C_RETRIES)){
    
        bfnI2CTxRxBuffer(txData, txDataSize, rxData, rxDataSize, isFirstFrame, isLastFrame);        
        mem241025IICControl.retriesCount++;
    }
    
    // OTHERWISE EXCECUTE AND NOTIFY ERROR PROCESS 
    if(mem241025IICControl.retriesCount == MEM24_1025_I2C_RETRIES){
     
        print_error("MaxRetriesCount Allowed");
        MEM24_1025_I2C_ErrorProcess();
    }
}

WORD MEM24_1025_I2C_GetWriteAddressByPacketPageSend(void){

    WORD address = mem241025IICControl.wAddress;
    
    if(mem241025IICControl._bPacketPageSend){
        
        address += ( (mem241025IICControl._bPacketPageSend * MAX_IIC_WRITE_PAGE_SIZE) - 
                     (address % MAX_IIC_WRITE_PAGE_SIZE ) );
    }
    
    return address;
}

WORD MEM24_1025_I2C_GetReadAddressByPacketPageSend(void){
    
    WORD address = mem241025IICControl.wAddress;
    address += ( mem241025IICControl._bPacketPageSend * MAX_IIC_WRITE_PAGE_SIZE);
    return address;
}

WORD MEM24_1025_I2C_BuildAddress(BYTE chipSelect, WORD wAddress, BYTE * buffer) {    
    
    BYTE * buffer_ptr = buffer;
    WORD bufferSize;
    
    memcpy(buffer_ptr, &chipSelect, sizeof(BYTE));
    buffer_ptr += sizeof(BYTE);
    
    memcpy(buffer_ptr, (BYTE *) &wAddress, sizeof(WORD));
    buffer_ptr += sizeof(WORD);
    
    bufferSize =  buffer_ptr - buffer;
    
    return bufferSize;
}

//******************************************************************************
//* Updating Function
//******************************************************************************

void MEM24_1025_I2C_UpdateTxPagingValues( void ){
    
    WORD wRestAdd;
    WORD wFaltAdd;
    
    wRestAdd = mem241025IICControl._wPagingAddress % MAX_IIC_WRITE_PAGE_SIZE;
    wFaltAdd = MAX_IIC_WRITE_PAGE_SIZE - wRestAdd;
    
    if(mem241025IICControl._wIICTxSizeMAX >= wFaltAdd){
        
        mem241025IICControl._wIICTxSize = wFaltAdd;
        mem241025IICControl._wIICTxSizeMAX -= wFaltAdd;
    } else {
        
        mem241025IICControl._wIICTxSize = mem241025IICControl._wIICTxSizeMAX;
    }
}

void MEM24_1025_I2C_UpdateDataIndex(void) {
    
    mem241025IICControl._wIICDataIndex += mem241025IICControl._wIICTxSize;
}

void MEM24_1025_I2C_UpdateReceptionNotification(void) {

    MEM24_1025_I2C_SetNotification(TRUE);
}

//******************************************************************************
//* MEM24_1025_I2C STATE MACHINE
//******************************************************************************

void vfnIIC_MEM24_1025Driver(void){
    
    vfnaIICDriverState[MEM24_1025_IIC_SM.bActualState]();    
}

void MEM24_1025_I2C_SetStateMachine(BYTE actualState, BYTE nextState){
    
    MEM24_1025_IIC_SM.bActualState = actualState;
    MEM24_1025_IIC_SM.bNextState = nextState;
    
    vfnEventEnable(IIC_EVENT);
    vfnEventPost(IIC_EVENT);
}

void vfnIICIdleState(void)
{
    // Do nothing    
}

void vfnIICAddressToWriteState(void) {    
        
    BYTE txData[MEM24_1025_I2C_SELECT_ADDRESS_FRAME_SIZE];  
    WORD wAddress;
    WORD txDataSize;
    
    wAddress = MEM24_1025_I2C_GetWriteAddressByPacketPageSend();
    MEM24_1025_I2C_SetPagingAddress(wAddress);    
    txDataSize = MEM24_1025_I2C_BuildAddress(CHIP_SELECT_MEM, wAddress, txData);
    
    MEM24_1025_I2C_SendTxRxDataProcess( txData,                                 /*  BYTE * txData       */                 
                                        txDataSize,                             /*  WORD txDataSize     */ 
                                        NULL,                                   /*  BYTE * rxData,      */
                                        0,                                      /*  WORD rxDataSize     */ 
                                        TRUE,                                   /*  BOOL isFirstFrame   */
                                        FALSE,                                  /*  BYTE isLastFrame    */
                                        MEM24_1025_I2C_UpdateTxPagingValues,    /*  UpdateCallBack_Function updateCallBackFunction  */
                                        _IIC_WRITE_DATA_STATE );                /*  BYTE nextState      */
}

void vfnIICWriteDataState(void) {        
 
    BYTE * txData = MEM24_1025_I2C_GetDataBuffer();
    WORD txDataSize;    
    
    txData += MEM24_1025_I2C_GetDataIndex();
    txDataSize = MEM24_1025_I2C_GetTxSize();
    
    MEM24_1025_I2C_SendTxRxDataProcess( txData, 
                                        txDataSize, 
                                        NULL,
                                        0, 
                                        FALSE, 
                                        TRUE, 
                                        MEM24_1025_I2C_UpdateDataIndex, 
                                        _IIC_WRITE_DATA_CHECK_PAGE_STATE );
}

void vfnIICWriteDataCheckPageState(void){
    
    BYTE _bPacketPageSend = MEM24_1025_I2C_GetPacketPageSend();
    WORD _wIICDataIndex_Div = MEM24_1025_I2C_GetDataIndexDiv();
        
    if(_bPacketPageSend < _wIICDataIndex_Div)
    {
       _bPacketPageSend++;
       MEM24_1025_I2C_SetStateMachine(_IIC_WRITE_ADD_STATE, _IIC_WRITE_DATA_CHECK_PAGE_STATE);       
       
    } else {
        
        MEM24_1025_I2C_SetStateMachine(_IIC_END_STATE, _IIC_END_STATE);       
    }  
    
    MEM24_1025_I2C_SetPacketPageSend(_bPacketPageSend);
}

void vfnIICAddressToReadState(void){
        
    BYTE txData[MEM24_1025_I2C_SELECT_ADDRESS_FRAME_SIZE];  
    WORD wAddress;
    WORD txDataSize;
    
    wAddress = MEM24_1025_I2C_GetReadAddressByPacketPageSend();
    txDataSize = MEM24_1025_I2C_BuildAddress(CHIP_SELECT_MEM, wAddress, txData);
    
    MEM24_1025_I2C_SendTxRxDataProcess( txData,                                 /*  BYTE * txData       */                 
                                        txDataSize,                             /*  WORD txDataSize     */ 
                                        NULL,                                   /*  BYTE * rxData,      */
                                        0,                                      /*  WORD rxDataSize     */ 
                                        TRUE,                                   /*  BOOL isFirstFrame   */
                                        FALSE,                                  /*  BOOL isLastFrame    */
                                        NULL,                                   /*  UpdateCallBack_Function updateCallBackFunction  */
                                        _IIC_READ_CMD_STATE );                  /*  BYTE nextState      */    
}

void vfnIICCMDToReadState(void){
    
    BYTE txData = CHIP_SELECT_MEM | 0x01;  
    
    MEM24_1025_I2C_SendTxRxDataProcess( &txData,                                /*  BYTE * txData       */                 
                                        sizeof(BYTE),                           /*  WORD txDataSize     */ 
                                        NULL,                                   /*  BYTE * rxData,      */
                                        0,                                      /*  WORD rxDataSize     */ 
                                        TRUE,                                   /*  BOOL isFirstFrame   */
                                        FALSE,                                  /*  BOOL isLastFrame    */
                                        NULL,                                   /*  UpdateCallBack_Function updateCallBackFunction  */
                                        _IIC_READ_DATA_STATE );                 /*  BYTE nextState      */    
}

void vfnIICReadDataState(void){    
    
    MEM24_1025_I2C_SendTxRxDataProcess( NULL,                                       /*  BYTE * txData       */                 
                                        0,                                          /*  WORD txDataSize     */ 
                                        MEM24_1025_I2C_GetReceptionBuffer(),        /*  BYTE * rxData,      */
                                        MEM24_1025_I2C_GetRxSize(),                 /*  WORD rxDataSize     */ 
                                        FALSE,                                      /*  BOOL isFirstFrame   */
                                        TRUE,                                       /*  BOOL isLastFrame    */
                                        MEM24_1025_I2C_UpdateReceptionNotification, /*  UpdateCallBack_Function updateCallBackFunction  */
                                        _IIC_END_STATE );                           /*  BYTE nextState      */    
}

void vfnIICEndState(void){
    // Get I2C Driver Status
    //! MEM24_1025_IIC_Clear();
    vfnEventClear(IIC_EVENT);
    vfnEventDisable(IIC_EVENT);
    MEM24_1025_I2C_SetAPIBusy(FALSE);
}
