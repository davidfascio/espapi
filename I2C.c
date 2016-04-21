#include "I2C.h"

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------
static WORD _wI2CRxCounter = 0;
static WORD _wI2CTxCounter = 0;
static BYTE* _bpI2CRxPtr;
static BYTE* _bpI2CTxPtr;
static BYTE _bI2CFlags = 0;
WORD _wI2CRxIndex=0;


typedef enum
{
    I2C_GEN_STOP = 0,
    I2C_NO_ACK_ERROR
}eI2CFlags;
//------------------------------------------------------------------------------
// Local functions prototypes
//------------------------------------------------------------------------------
void vfnI2CBitDly(void);
void vfnI2C_SCLHigh(void);
void vfnI2C_SCLLow(void);
void vfnI2C_StartBit(void);
void vfnI2C_StopBit(void);

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
    if(_bI2CFlags&(1<<I2C_NO_ACK_ERROR))
    {
        _bI2CFlags &= ((~1)<<I2C_NO_ACK_ERROR);
        return(TRUE);
    }
    return(FALSE);
}

BYTE bfnIsI2CBusy(void)
{
    if((_wI2CRxCounter)||(_wI2CTxCounter))
    {
            return(TRUE);
    }
    return(FALSE);
}

BYTE bfnI2CTxRxBuffer(BYTE* bpTxPtr, WORD wTxSize, BYTE* bpRxPtr, WORD wRxSize, BYTE bStart, BYTE bStopFlag)
{
    _bI2CFlags &= ((~1)<<I2C_NO_ACK_ERROR);
    BYTE bError = TRUE;
    if(bfnIsI2CBusy())
    {
            return(FALSE);
    }
    if(wTxSize)
    {
            _wI2CTxCounter = wTxSize;
            _bpI2CTxPtr = bpTxPtr;
            bError = FALSE;
    }
    if(wRxSize)
    {
            _wI2CRxCounter = wRxSize;
            _wI2CRxIndex=0;
            _bpI2CRxPtr = bpRxPtr;
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
            _bI2CFlags = 0;
            if(bStopFlag)
            {
                _bI2CFlags |= (1<<I2C_GEN_STOP);
            }
    }
    return(TRUE);
}

void vfnI2CDriver(void)
{
    BYTE bI2CDriverStop = FALSE;
    if(_wI2CTxCounter)
    {
            //Write next byte
            //!D2_LED=~D2_LED;
            //!CLRWDT();
            BYTE bI2CData = *_bpI2CTxPtr++;
            BYTE bCounter = 8;
            BYTE bAck = FALSE;
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
            }while(--bCounter);
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
                    _bI2CFlags = (1<<I2C_NO_ACK_ERROR);
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
                    _bpI2CRxPtr[_wI2CRxIndex++] = bI2CData;
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
                    if(_bI2CFlags&(1<<I2C_GEN_STOP))
                    {
                        vfnI2C_StopBit();
                    }
                    _bI2CFlags = 0;
                    bI2CDriverStop = TRUE;
            }
    }
    if(bI2CDriverStop)
    {
            vfnI2C_StopBit();
            //IIC driver disable...
            vfnEventClear(I2C_DRIVER_EVENT);
            vfnEventDisable(I2C_DRIVER_EVENT);
    }
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









