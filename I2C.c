#include "I2C.h"

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------

/*
static WORD _wI2CRxCounter = 0;
static WORD _wI2CTxCounter = 0;
static BYTE* _bpI2CRxPtr;
static BYTE* _bpI2CTxPtr;
static BYTE _bI2CFlags = 0;
WORD _wI2CRxIndex=0;
*/

I2C_DRIVER_CONTROL i2cDriverControl;
BOOL SDA_B_IN;

//------------------------------------------------------------------------------
// Local functions prototypes
//------------------------------------------------------------------------------
void vfnI2CBitDly(void);
void vfnI2C_SCLHigh(void);
void vfnI2C_SCLLow(void);
void vfnI2C_StartBit(void);
void vfnI2C_StopBit(void);

void I2CDriverControl_SetRxCounter(WORD _wI2CRxCounter){
    
    if(_wI2CRxCounter > I2C_DRIVER_MAX_PAGING_DATA_SIZE)
        return;
    
    i2cDriverControl._wI2CRxCounter = _wI2CRxCounter; 
}

void I2CDriverControl_SetTxCounter(WORD _wI2CTxCounter){
    
    if(_wI2CTxCounter > I2C_DRIVER_MAX_PAGING_DATA_SIZE)
        return;
    
    i2cDriverControl._wI2CTxCounter = _wI2CTxCounter; 
}

WORD I2CDriverControl_GetRxCounter( void ){
    
    return i2cDriverControl._wI2CRxCounter;    
}

WORD I2CDriverControl_GetTxCounter( void ){

    return i2cDriverControl._wI2CTxCounter;
}

void I2CDriverControl_SetRxIndex(WORD _wI2CRxIndex){
    
    if(_wI2CRxIndex > I2C_DRIVER_MAX_PAGING_DATA_SIZE)
        return;
    
    i2cDriverControl._wI2CRxIndex = _wI2CRxIndex; 
}

void I2CDriverControl_SetTxIndex(WORD _wI2CTxIndex){
    
    if(_wI2CTxIndex > I2C_DRIVER_MAX_PAGING_DATA_SIZE)
        return;
    
    i2cDriverControl._wI2CTxIndex = _wI2CTxIndex; 
}

WORD I2CDriverControl_GetRxIndex(void){
    
    return i2cDriverControl._wI2CRxIndex;
}

WORD I2CDriverControl_GetTxIndex(void){
    
    return i2cDriverControl._wI2CTxIndex;
}

void I2CDriverControl_SetRxBuffer(BYTE * buffer, WORD _wI2CRxCounter){

    i2cDriverControl._bpI2CRxBuffer = buffer;
    i2cDriverControl._wI2CRxCounter = _wI2CRxCounter;
}

void I2CDriverControl_PutRxBufferByIndex( BYTE data, WORD _wI2CRxIndex ){
    
    if(_wI2CRxIndex > I2C_DRIVER_MAX_PAGING_DATA_SIZE)
        return;
    
    * (i2cDriverControl._bpI2CRxBuffer + _wI2CRxIndex) = data;
}

void I2CDriverControl_SetTxBuffer( BYTE * buffer, WORD _wI2CTxCounter ){
    
    if(_wI2CTxCounter > I2C_DRIVER_MAX_PAGING_DATA_SIZE)
        return;
    
    memset(i2cDriverControl._bpI2CTxBuffer, 0, I2C_DRIVER_MAX_PAGING_DATA_SIZE);
    memcpy(i2cDriverControl._bpI2CTxBuffer, buffer, _wI2CTxCounter);
    i2cDriverControl._wI2CTxCounter = _wI2CTxCounter;
}

BYTE I2CDriverControl_GetTxBufferByIndex(WORD _wI2CTxIndex){

    if(_wI2CTxIndex > I2C_DRIVER_MAX_PAGING_DATA_SIZE){
     
        print_error("Out of index");
        return 0;
    }
    
    return i2cDriverControl._bpI2CTxBuffer[_wI2CTxIndex];
}

void I2CDriverControl_ClearFlags( void ){
    
    i2cDriverControl._bI2CFlags = 0;
}

void I2CDriverControl_SetFlagsByIndex( eI2CFlags _bI2CFlags, BOOL state){
        
    if(state = FALSE)
        i2cDriverControl._bI2CFlags &= ((~1) << _bI2CFlags );    
    
    else if(state = TRUE)
        i2cDriverControl._bI2CFlags |= ((1) << _bI2CFlags );    
}

BOOL I2CDriverControl_GetFlagsByIndex(eI2CFlags _bI2CFlags){
    
    return i2cDriverControl._bI2CFlags & ((1) <<  _bI2CFlags);
}

BYTE I2CDriverControl_GetFlags( void ){
    
    return i2cDriverControl._bI2CFlags;
}

void I2CDriverControl_SetTxRxStatus(TXRX_STATUS status){
    
    i2cDriverControl.txrxStatus = status;    
}

TXRX_STATUS I2CDriverControl_GetTxRxStatus( void ){
    
    return i2cDriverControl.txrxStatus;
}

//------------------------------------------------------------------------------
// Functions Declarations
//------------------------------------------------------------------------------
/*!
	\fn
	\param
	\return
	\brief
*/
void vfnI2CInit(void)
{
    SCL_B_CONFIG;                   // Configura SCL como Salida
    SDA_B_CONFIG_IN;                // Configura SDA como Entrada
    vfnI2C_SCLLow();                // Pone SCL en Bajo
    SDA_B_HI;                       // Pone SDA en Alto
    vfnRTCChipReset();              // Regresa la memoria a estado conocido
}

void vfnI2C_StartBit(void){
    SDA_B_CONFIG_IN;                // Configura SDA como Entrada
    vfnI2C_SCLHigh();               // Pone SCL en Alto
    SDA_B_CONFIG_OUT;               // Configura SDA como Salida
    SDA_B_LO;                       // Pone SDA en Bajo
    vfnI2CBitDly();
    vfnI2C_SCLLow();                // Pone SCL en Bajo
}

void vfnI2C_StopBit(void){
    vfnI2C_SCLLow();                // Pone SCL en Bajo
    SDA_B_CONFIG_OUT;                   // Configura SDA como Salida
    SDA_B_LO;                       // Pone SDA en Bajo
    vfnI2CBitDly();
    vfnI2C_SCLHigh();               // Pone SCL en Alto
    SDA_B_CONFIG_IN;                // Configura SDA como Entrada
}

BYTE bfnI2CError(void)
{
    /*if(_bI2CFlags&(1<<I2C_NO_ACK_ERROR))
    {
        _bI2CFlags &= ((~1)<<I2C_NO_ACK_ERROR);
        return(TRUE);
    }
    return(FALSE);*/
    return I2CDriverControl_GetFlags( );
}

BYTE bfnIsI2CBusy(void)
{
    if( I2CDriverControl_GetRxCounter( ) ||
        I2CDriverControl_GetTxCounter( ) )
    {
        return(TRUE);
    }
    
    return(FALSE);
}

BYTE bfnI2CTxRxBuffer(BYTE* bpTxPtr, WORD wTxSize, BYTE* bpRxPtr, WORD wRxSize, BYTE bStart, BYTE bStopFlag)
{
    BYTE bError = TRUE;
    
    //! _bI2CFlags &= ((~1)<<I2C_NO_ACK_ERROR);
    
    
    if(bfnIsI2CBusy())
    {
        //I2CDriverControl_SetFlagsByIndex(I2C_NO_ACK_ERROR, FALSE);
        return(FALSE);
    }
    
    if(wTxSize)
    {        
        //!_wI2CTxCounter = wTxSize;
        //!_bpI2CTxPtr = bpTxPtr;
        print_debug("Packet Size: %d, Packet Data: ", wTxSize);
        print_buffer(bpTxPtr, wTxSize);
        I2CDriverControl_SetTxBuffer(bpTxPtr, wTxSize);
        
        bError = FALSE;
    }
    
    if(wRxSize)
    {
        //! Invoke an Event
        //! _wI2CRxCounter = wRxSize;
        //! _wI2CRxIndex=0;
        //! _bpI2CRxPtr = bpRxPtr;
        print_debug("Reception Packet Size: %d ", wRxSize);
        I2CDriverControl_SetRxBuffer(bpRxPtr ,wRxSize);
        bError = FALSE;
    }
    
    if(!bError)
    {
            if(bStart)
            {
            //Start bit...
                vfnI2C_StartBit();
            }
            //Enable IIC Event...
            vfnEventEnable(I2C_DRIVER_EVENT);
            vfnEventPost(I2C_DRIVER_EVENT);
            I2CDriverControl_ClearFlags();
            //! _bI2CFlags = 0;
            if(bStopFlag)
            {
                //!_bI2CFlags |= (1<<I2C_GEN_STOP);
                I2CDriverControl_SetFlagsByIndex( I2C_GEN_STOP, TRUE);
            }
            I2CDriverControl_SetTxRxStatus(I2C_DRIVER_INITIALIZED);
    }
    return(TRUE);
}

void vfnI2CDriver(void)
{
    BYTE bI2CDriverStop = FALSE;
    BYTE bI2CData;
    BYTE bCounter;
    BYTE bAck;    
    WORD _wI2CTxCounter;
    WORD _wI2CTxIndex;
    WORD _wI2CRxCounter;
    WORD _wI2CRxIndex;
    
    _wI2CTxCounter  = I2CDriverControl_GetTxCounter();
    _wI2CTxIndex    = I2CDriverControl_GetTxIndex();
    _wI2CRxCounter  = I2CDriverControl_GetRxCounter();
    _wI2CRxIndex    = I2CDriverControl_GetRxIndex();
    
    if( _wI2CTxCounter )
    {
        //Write next byte
        //!D2_LED=~D2_LED;
        //!CLRWDT();
        bI2CData = I2CDriverControl_GetTxBufferByIndex( _wI2CTxIndex );
        _wI2CTxIndex++;
        
        bCounter = 8;
        bAck = FALSE;           
        SDA_B_CONFIG_OUT;

        vfnI2CBitDly();
        do
        {
            if(bI2CData&(0x80))
            {
                SDA_B_HI;
            }
            else
            {
                SDA_B_LO;
            }

            vfnI2CBitDly();
            vfnI2C_SCLHigh();
            bI2CData <<= 1;
            vfnI2C_SCLLow();

        } while(--bCounter);

        _wI2CTxCounter--;

        //Ack?
        SDA_B_CONFIG_IN;            //Se configura como entrada
        vfnI2CBitDly();
        vfnI2C_SCLHigh();

        if(!SDA_B_IN)
        {
            bAck = TRUE;
        }

        vfnI2C_SCLLow();
        
        if(!bAck)
        {
            //No ack... Generate error!
            //! _bI2CFlags = (1<<I2C_NO_ACK_ERROR);
            I2CDriverControl_SetFlagsByIndex( I2C_NO_ACK_ERROR, TRUE );
            bI2CDriverStop = TRUE;
            _wI2CTxCounter = 0;
            _wI2CRxCounter = 0;
        }

        SDA_B_CONFIG_OUT;               //Se configura como Salida
    }
    
    else
    {
            if(_wI2CRxCounter)
            {
                    //Read next byte
                //!D2_LED=~D2_LED;
                //!CLRWDT();
                    register BYTE bI2CData = 0;
                    register BYTE bCounter = 8;
                    SDA_B_CONFIG_IN;
                    do
                    {
                            vfnI2CBitDly();
                            vfnI2C_SCLHigh();
                            bI2CData <<= 1;
                            if(SDA_B_IN)
                            {
                                    bI2CData |= 1;
                            }
                            vfnI2C_SCLLow();
                    }while(--bCounter);
                    
                    I2CDriverControl_PutRxBufferByIndex(bI2CData, _wI2CRxIndex);
                    _wI2CRxIndex++;
                    //!_bpI2CRxPtr[_wI2CRxIndex++] = bI2CData;
                    
                    if(_wI2CRxCounter--)
                    {
                            //Send Ack...
                            SDA_B_CONFIG_OUT;
                            vfnI2C_SCLLow();
                            SDA_B_LO;
                    }
                    vfnI2CBitDly();
                    vfnI2C_SCLHigh();
                    vfnI2C_SCLLow();
            }
            else
            {
                    //! if(_bI2CFlags&(1<<I2C_GEN_STOP))
                if(I2CDriverControl_GetFlagsByIndex(I2C_GEN_STOP))
                    {
                        vfnI2C_StopBit();
                    }
                    I2CDriverControl_ClearFlags();
                    bI2CDriverStop = TRUE;
            }
    }
    if(bI2CDriverStop)
    {
            vfnI2C_StopBit();
            //IIC driver disable...
            I2CDriverControl_SetTxRxStatus(I2C_DRIVER_ENDED);
            vfnEventClear(I2C_DRIVER_EVENT);
            vfnEventDisable(I2C_DRIVER_EVENT);
    }
    
    I2CDriverControl_SetTxCounter(_wI2CTxCounter);
    I2CDriverControl_SetRxCounter(_wI2CRxCounter);
    I2CDriverControl_SetTxIndex(_wI2CTxIndex);
    I2CDriverControl_SetRxIndex(_wI2CRxIndex);
}



/*!
	\fn
	\param
	\return
	\brief
*/
void vfnI2CBitDly(void)
{
    #if (I2C_BIT_DELAY != 0)
            WORD wDly = I2C_BIT_DELAY;
            while(--wDly);
    #else
            return;
    #endif
}
/*********************************************************************/
/*!
	\fn
	\param
	\return
	\brief
*/
/*********************************************************************/
void vfnI2C_SCLHigh(void)
{
    SCL_B_HI;
    vfnI2CBitDly();
}
/*********************************************************************/
/*!
	\fn
	\param
	\return
	\brief
*/
/*********************************************************************/
void vfnI2C_SCLLow(void)
{
    SCL_B_LO;
    vfnI2CBitDly();
}

/*********************************************************************/
/*!
	\fn		void vfnRTCChipReset(void)
	\param	none
	\return	void
	\brief 	Send chip reset to RTC
*/
/*********************************************************************/
void vfnRTCChipReset(void)
{
    volatile BYTE bCounter = 9;
    //Start bit...
    vfnI2C_StartBit();
    //Send nine "1"
    do
    {
            SDA_B_HI;
            vfnI2CBitDly();
            vfnI2C_SCLHigh();
            if(bCounter > 1)
            {
                    vfnI2C_SCLLow();
            }
    }while(--bCounter);

    //Start bit...
    vfnI2C_StartBit();


    //Stop bit...

    vfnI2C_StopBit();
}
//------------------------------------------------------------------------------









