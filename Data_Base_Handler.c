/****************************************************************************************/
/*
 * File:        Data_Base_Handler.c
 * Author:      Jose Antonio Becerril Morales
 * version:     Intern Support
 * Processor:   PIC32MX360F512L
 * Complier:    C32(v2.02)
 * Company:     Eneri
 * Date:        Created on September 17,2013.
 * Comment:     Data Base Control
 */
/****************************************************************************************/
/*-- Includes --------------------------------------------------------------------------*/
#include "Data_Base_Handler.h"
/*-- Defines ---------------------------------------------------------------------------*/
/*Size of Meter ID*/
#define ID_MAX_SIZE            0x50
/*Address where Measurement Tables begin*/
#define INIT_ADD_METERS_TABLE  0x80
/*OFFSET = size of meters data, used to access and write*/
#define METERS_TABLE_OFFSET    0x24
/*Address where Measurement Tables end*/
#define END_ADD_METERS_TABLE   0x389C
/*Limited Searchs*/
#define SEARCHS_ALLOWED        0x0A
/*-- Types Definitions -----------------------------------------------------------------*/

/*Back Up State Machine*/
typedef enum
{
    _BACKUP_IDLE_STATE = 0,
    _BACKUP_DEVICES_INDEX_STATE,
    _BACKUP_DEV_ADD_STATE,
    _BACKUP_DEV_READ_STATE,
    _BACKUP_METERS_INDEX_STATE,
    _BACKUP_METERS_ADD_STATE,
    _BACKUP_WRITE_DEV_INDEX_STATE,
    _BACKUP_WRITE_MET_INDEX_STATE,
    _BACKUP_END_STATE,
} eBACKUPDriverStates;
/*-- Local functions prototypes --------------------------------------------------------*/
/*BackUP State machine*/
void vfnBKUPIdleState(void);
void vfnBKUPReadDevicesBKIndxState(void);
void vfnBKUPGetDevicesIndexState(void);
void vfnBKUPReadDevicesState(void);
void vfnBKUPReadMetersBKIndxState(void);
void vfnBKUPGetMetersIndexState(void);
void vfnBKUPWriteDeviceIndexState(void);
void vfnBKUPWriteMeterIndexState(void);
void vfnBKUPEndState(void);
/*End BackUP State machine*/

void vfnSaveIndexDev(void);
void vfnSaveIndexMet(void);
void vfnAllocMtr(WORD wIndex, BYTE bStatus);
void vfnAllocDev(WORD wIndex, BYTE bStatus);


void (*const vfnaBACKUPStartDriverState[]) (void) =
{
    vfnBKUPIdleState,
    vfnBKUPReadDevicesBKIndxState,
    vfnBKUPGetDevicesIndexState,
    vfnBKUPReadDevicesState,
    vfnBKUPReadMetersBKIndxState,
    vfnBKUPGetMetersIndexState,
    vfnBKUPWriteDeviceIndexState,
    vfnBKUPWriteMeterIndexState,
    vfnBKUPEndState,
};

sSM _tBackStartUpSM = {ZERO, ZERO, ZERO, ZERO};

/*-- Variables -------------------------------------------------------------------------*/
/*Empty Array*/
BYTE bEmpty[MAC_SIZE];
/*Pointers to Tables*/
Meter_Eneri *tsPtrMeter;
MAC_Short_Type *tsPtrDevice;
/*Devices Index*/
static WORD wDevicesIndex;
static WORD wDevicesIndxBckUp;       //Numero hasta donde hay medidores validos.
/*Meters Index*/
static WORD wMetersIndex;           //Numero de medidores valido.
static WORD wMetersIndxBckUp;       //Numero hasta donde hay medidores validos.
/*Tables declaration*/
Meter_Eneri    tsMeter[NUM_MAX_METERS];
MAC_Short_Type tsDevice[NUM_MAX_NODES];
Meter_Eneri    *apTemp;
READING_LIST   *apTemp3;
WORD wAddressGLOBAL;
BYTE bSpace;
BYTE bSpace1;
//BYTE tsReading[];
/****************************************************************************************/
/*
 * \brief    vfnBackUpStartDriver - State Machine Driver
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnBackUpStartDriver(void)
{
    vfnaBACKUPStartDriverState[_tBackStartUpSM.bActualState]();
}
/****************************************************************************************/
/*
 * \brief    bfnBackUp_Init - Initialize the Back Up Function,Which is to recover
 * \         information from external EEPROM
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
BYTE bfnBackUp_Init(void)
{   /*Asks if Back up is not being used*/
    if (sFlags.BACKUP_Busy)
    {
        return (FALSE);
    }
    sFlags.BACKUP_Busy = TRUE;
    /*Set Acutal and Next States*/
    _tBackStartUpSM.bActualState = _BACKUP_DEVICES_INDEX_STATE;
    _tBackStartUpSM.bNextState = _BACKUP_DEV_ADD_STATE;
    /*Start Finite state machine of BKUP MTR*/
    vfnEventEnable(BACKUP_START_EVENT);
    vfnEventPost(BACKUP_START_EVENT);
    /*Read Devices Index From EEPROM*/
    bBufferIICRead = (BYTE*) & wDevicesIndxBckUp;
    bfnIIC_MEM24_1025_Read(NODES_INDEX_BCKUP, Address_Size);
    return (TRUE);
}
/****************************************************************************************/
/*
 * \brief    vfnSaveIndexDev - Initialize a State Machine which saves DevicesIndex In EEPROM
 * \         information from external EEPROM
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnSaveIndexDev(void)
{
    if (sFlags.BACKUP_Busy)
    {
        return;
    }
    sFlags.BACKUP_Busy = TRUE;
    _tBackStartUpSM.bActualState = _BACKUP_WRITE_DEV_INDEX_STATE;
    _tBackStartUpSM.bNextState = _BACKUP_END_STATE;
    vfnEventEnable(BACKUP_START_EVENT);
    vfnEventPost(BACKUP_START_EVENT);
}
/****************************************************************************************/
/*
 * \brief    vfnSaveIndexDev - Initialize a State Machine which saves MetersIndex In EEPROM
 * \         information from external EEPROM
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnSaveIndexMet(void)
{
//    if (sFlags.BACKUP_Busy)
//    {
//        return;
//    }
//    sFlags.BACKUP_Busy = TRUE;
    _tBackStartUpSM.bActualState = _BACKUP_WRITE_MET_INDEX_STATE;
    _tBackStartUpSM.bNextState = _BACKUP_END_STATE;
    vfnEventEnable(BACKUP_START_EVENT);
    vfnEventPost(BACKUP_START_EVENT);
}

void vfnBKUPIdleState(void)
{

}

/****************************************************************************************/
/*
 * \brief    vfnBKUPReadDevicesBKIndxState - Recover Index Devices From EEPROM
 * \author   Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnBKUPReadDevicesBKIndxState(void){
    /*Asks if EEPROM is Busy*/
    if (!bfnIICIsBusy())
    {   /*Starts Reading*/
        bBufferIICRead = (BYTE*) & wDevicesIndex;
        bfnIIC_MEM24_1025_Read(NODES_INDEX_ADD, Address_Size);
        _tBackStartUpSM.bActualState = _BACKUP_DEV_ADD_STATE;
        _tBackStartUpSM.bNextState = _BACKUP_END_STATE;
    }
}
/****************************************************************************************/
/*
 * \brief    vfnBKUPGetDevicesIndexState - Recover Devices Table From EEPROM
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnBKUPGetDevicesIndexState(void)
{   /*Asks if EEPROM is Busy*/
    if (!bfnIICIsBusy())
    {   /*Validate a Coherent Value read from EEPROM Previously*/
        if (wDevicesIndxBckUp > 0 && wDevicesIndxBckUp < NUM_MAX_NODES)
        {   /*Starts Reading*/
            static WORD wIndexDevicesTemp=FALSE;
            if(wDevicesIndex>=NUM_MAX_NODES)wDevicesIndex=0;
            bBufferIICRead = (BYTE *) & tsDevice[wIndexDevicesTemp].Short_Add[0];
            if(bfnIIC_MEM24_1025_Read(CAB_READ_Meter_MAC_SHORT+(wIndexDevicesTemp*Buffer_Lenght_MAC_Info), Buffer_Lenght_MAC_Info)){
                wIndexDevicesTemp++;
            }
            if(wIndexDevicesTemp>=wDevicesIndxBckUp){
                wIndexDevicesTemp=0;
                _tBackStartUpSM.bActualState = _BACKUP_DEV_READ_STATE;
                _tBackStartUpSM.bNextState = _BACKUP_METERS_ADD_STATE;
            }
        }
        else
        {   /*If there is not a valid Device Index, starts reading MetersIndex*/
            wDevicesIndex = FALSE;
            bBufferIICRead = (BYTE*) & wMetersIndex;
            bfnIIC_MEM24_1025_Read(METERS_INDEX_ADD, Address_Size);
            _tBackStartUpSM.bActualState = _BACKUP_METERS_INDEX_STATE;
            _tBackStartUpSM.bNextState = _BACKUP_END_STATE;
        }
    }
}
/****************************************************************************************/
/*
 * \brief    vfnBKUPReadMetersBKIndxState - Recover MetersIndex From EEPROM
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnBKUPReadMetersBKIndxState(void)
{   /*Asks if EEPROM is Busy*/
    if (!bfnIICIsBusy())
    {   /*Starts Reading*/
        bBufferIICRead = (BYTE *) & wMetersIndxBckUp;
        bfnIIC_MEM24_1025_Read(METERS_INDEX_BCKUP, Address_Size);
        _tBackStartUpSM.bActualState = _BACKUP_METERS_ADD_STATE;
        _tBackStartUpSM.bNextState = _BACKUP_END_STATE;
    }
}
/****************************************************************************************/
/*
 * \brief    vfnBKUPReadDevicesState - Recover MetersIndex From EEPROM
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnBKUPReadDevicesState(void)
{   /*Asks If EEPROM is busy*/
    if (!bfnIICIsBusy())
    {   /*Starts reading*/
        bBufferIICRead = (BYTE *) & wMetersIndex;
        bfnIIC_MEM24_1025_Read(METERS_INDEX_ADD, Address_Size);
        _tBackStartUpSM.bActualState =  _BACKUP_METERS_INDEX_STATE;
        _tBackStartUpSM.bNextState = _BACKUP_END_STATE;
    }
}
/****************************************************************************************/
/*
 * \brief    vfnBKUPGetMetersIndexState - Recover Meters Table From EEPROM
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnBKUPGetMetersIndexState(void)
{   /*Asks If EEPROM is busy*/
    if (!bfnIICIsBusy())
    {   /*Validate a Coherent Value read from EEPROM Previously*/
        if (wMetersIndxBckUp > 0 && wMetersIndxBckUp < NUM_MAX_METERS)
        {   /*Starts reading*/
            static WORD wMeterIndexTemp=0;
            bBufferIICRead = (BYTE *) & tsMeter[wMeterIndexTemp];
            if(bfnIIC_MEM24_1025_Read(CAB_READ_Meter_ADD+(wMeterIndexTemp*METER_NAME_SIZE),METER_NAME_SIZE)){
                wMeterIndexTemp++;
            }
            if(wMeterIndexTemp>=wMetersIndxBckUp){
                wMeterIndexTemp=0;
                _tBackStartUpSM.bActualState = _BACKUP_END_STATE;
                _tBackStartUpSM.bNextState = ZERO;
            }
        }
        else
        {
            wMetersIndex = FALSE;
            /*End of Back Up*/
            _tBackStartUpSM.bActualState = _BACKUP_END_STATE;
            _tBackStartUpSM.bNextState = ZERO;
        }
    }
}
/****************************************************************************************/
/*
 * \brief    vfnBKUPWriteDeviceIndexState - Writes Index State in EEPROM
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnBKUPWriteDeviceIndexState(void)
{
    BYTE waDevicesIndex[4];
    /*Asks if EEPROM busy*/
    if (!bfnIICIsBusy())
    {   /*Back Up in order to separete HIGH and LOW significative bits*/
        waDevicesIndex[1] = (BYTE) ((wDevicesIndex & 0xFF00) >> 8);
        waDevicesIndex[0] = (BYTE) (wDevicesIndex & 0x00FF);
        waDevicesIndex[3] = (BYTE) ((wDevicesIndxBckUp & 0xFF00) >> 8);
        waDevicesIndex[2] = (BYTE) (wDevicesIndxBckUp & 0x00FF);
        if(bfnIIC_MEM24_1025_Write(&waDevicesIndex[0]
                , NODES_INDEX_ADD
                , Address_Size*2)){
            _tBackStartUpSM.bActualState = _BACKUP_END_STATE;
            _tBackStartUpSM.bNextState = ZERO;
        }
    }
}
/****************************************************************************************/
/*
 * \brief    vfnBKUPWriteMeterIndexState - Writes Index State in EEPROM
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnBKUPWriteMeterIndexState(void)
{
    BYTE waMetersIndex[4];
    /*Asks if EEPROM busy*/
    if (!bfnIICIsBusy())
    {   /*Back Up in order to separete HIGH and LOW significative bits*/
        waMetersIndex[1] = (BYTE) ((wMetersIndex & 0xFF00) >> 8);
        waMetersIndex[0] = (BYTE) (wMetersIndex  & 0x00FF);
        waMetersIndex[3] = (BYTE) ((wMetersIndxBckUp & 0xFF00) >> 8);
        waMetersIndex[2] = (BYTE) (wMetersIndxBckUp  & 0x00FF);
        if(bfnIIC_MEM24_1025_Write(&waMetersIndex[0]
                , METERS_INDEX_ADD
                , 4)){
            _tBackStartUpSM.bActualState = _BACKUP_END_STATE;
            _tBackStartUpSM.bNextState = ZERO;
        }
    }
}
/****************************************************************************************/
/*
 * \brief    vfnBKUPEndState - End of BackUp
 * \author   Jose Antonio Becerril Morales & Rodrigo Castro
 * \param    none
 * \return   none
 * \todo
 */
/****************************************************************************************/
void vfnBKUPEndState(void)
{    /*Asks if EEPROM busy*/
    if (!bfnIICIsBusy())
    {   /*End of Back Up*/
        sFlags.BACKUP_Busy = FALSE;
        vfnEventClear(BACKUP_START_EVENT);
        vfnEventDisable(BACKUP_START_EVENT);
    }
}

/*-- Functions Code --------------------------------------------------------------------*/
void vfnInitDataBase(void)
{
    /*Clear Buffer*/
    memset((BYTE*) & bEmpty[0], 0x00, MAC_SIZE);
}
/****************************************************************************************/
/*
 * \brief    bfnSaveData - Used to save specific devices or meters
 * \author   Jose Antonio Becerril Morales
 * \param    bTableType, it can be Meters or Devices tables
 * \return   TRUE-saved successful,FALSE-saved error
 * \todo
 */
/****************************************************************************************/
BYTE bfnSaveData(BYTE bTableType, BYTE *vptrTableStructure)
{   /*Asks if EEPROM is busy*/
    if (bfnIICIsBusy())
    {
        return (FALSE);
    }
    BYTE bCompare    = FALSE;               //Poner en variable static
    BYTE bChange     = FALSE;
    WORD wIndex      = FALSE;
    WORD wIndexBckUp = FALSE;
    BYTE bLocated    = FALSE;
    //Meter_Eneri    *apTemp;
    MAC_Short_Type   *apTemp2;
    //READING_LIST   *apTemp3;
    /*cases of what kind of table is going to be save or set as new*/
    switch (bTableType)
    {
        case METERSTABLE:
            /*Validate a Coherent Value read from EEPROM Previously*/
            if (wMetersIndex > 0 && wMetersIndex < NUM_MAX_METERS)
            {   /*Local pointer to Data*/
                apTemp = (Meter_Eneri*) vptrTableStructure;
                /*Seek for data already written*/
                while (wIndexBckUp < wMetersIndex)
                {   /*Asks for blank spaces*/
                    if(tsMeter[wIndex].Type)
                    {
                        bCompare = memcmp((const void *) & tsMeter[wIndex].Serial_Num[0]
                                , (const void *) & apTemp->Serial_Num[0]
                                , ID_METER_SIZE);
                        if (!bCompare)
                        {   /*Data Located*/
                            bLocated = TRUE;
                            break;
                        }
                        else{
                            wIndexBckUp++;
                        }
                    }
                    wIndex++;
                }
                /*If data Located*/
                if (bLocated)
                {   /*Asks if Any data has been changed in order to update it*/
                    bCompare = memcmp((const void *)&tsMeter[wIndex].MACAdd_Display[0]
                            , (const void *)&apTemp->MACAdd_Display[0]
                            , MAC_SIZE);
                    if (bCompare)
                    {
                        memcpy((void *) &tsMeter[wIndex].MACAdd_Display[0]
                                , (const void *) &apTemp->MACAdd_Display[0]
                                , MAC_SIZE);
                        bChange = TRUE;
                    }
                    bCompare = memcmp((const void *)&tsMeter[wIndex].Signature
                            , (const void *)&apTemp->Signature
                            , SIGNATURE_SIZE_MTR);
                    if (bCompare)
                    {
                        memcpy((void *) &tsMeter[wIndex].Signature
                                , (const void *) &apTemp->Signature
                                , SIGNATURE_SIZE_MTR);
                        bChange = TRUE;
                    }
                    bCompare = memcmp((const void *)&tsMeter[wIndex].MAC_Gabinet[0]
                            , (const void *)&apTemp->MAC_Gabinet[0]
                            , MAC_SIZE);
                    if (bCompare)
                    {
                        memcpy((void *) &tsMeter[wIndex].MAC_Gabinet[0]
                                , (const void *) &apTemp->MAC_Gabinet[0]
                                , MAC_SIZE);
                        bChange = TRUE;
                    }
                    if (bChange)
                    {   /*if any data has suffered a change, then update it*/
                        vfnAllocMtr(wIndex, ZERO);
                    }
                    return NO_NEW_MTR_ADD;
                }
                else
                {   /*In case of new data*/
                    wIndex = FALSE;
                    bLocated = FALSE;
                    bCompare = TRUE;
                    /*Locates free space in order to write new data*/
                    while (wIndex <= wMetersIndex)
                    {
                        if(!tsMeter[wIndex].Type)
                        {
                            bCompare=FALSE;
                        }
                        if (!bCompare)
                        {
                            bLocated = TRUE;
                            break;
                        }
                        wIndex++;
                    }
                    /*if a free space has been located*/
                    if (bLocated)
                    {
                        /*Data allocation in EEPROM*/
                        memcpy((void *) &tsMeter[wIndex].Serial_Num[0]
                                , (const void *) vptrTableStructure
                                , METER_NAME_SIZE - 4);
                        vfnAllocMtr(wIndex, UNKNOWN);
                        wMetersIndex++;
                        if(wMetersIndxBckUp <= wIndex)
                        {
                            wMetersIndxBckUp++;
                        }
                        //vfnSaveIndexMet();
                        return NEW_MTR_ADD;
                    }
                    else
                    {
                        return FALSE;
                    }
                }
            }/*Just for the very first Data to be allocated*/
            else if (!wMetersIndex)
            {   /*Data allocation in EEPROM*/
                memcpy((void *) &tsMeter[wMetersIndex].Serial_Num[0]
                        , (const void *) vptrTableStructure
                        , METER_NAME_SIZE - 4);
                vfnAllocMtr(wIndex, FIRST);
                wMetersIndex++;
                wMetersIndxBckUp = wMetersIndex;
                //vfnSaveIndexMet();
                return NEW_MTR_ADD;
            }
            else
            {
                return FALSE;
            }
            break;
        case DEVICESTABLE:
            /*Validate a Coherent Value read from EEPROM Previously*/
            if (wDevicesIndex > 0 && wDevicesIndex < NUM_MAX_NODES)
            {   /*Local pointer to Data*/
                apTemp2 = (MAC_Short_Type*) vptrTableStructure;
                /*Seek for data already written*/
                while (wIndexBckUp <=wDevicesIndex )
                {   /*Asks for blank spaces*/
                    if(tsDevice[wIndex].Type)
                    {
                        bCompare = memcmp((const void *) & tsDevice[wIndex].MAC[0]
                            , (const void *) & apTemp2->MAC[0]
                            , MAC_SIZE);
                        if (!bCompare)
                        {   /*Data Located*/
                            bLocated = TRUE;
                            break;
                        }
                        else{
                            wIndexBckUp++;
                        }
                    }
                    wIndex++;
                }
                /*If data Located*/
                if (bLocated)
                {   /*Asks if Any data has been changed in order to update it*/
                    bCompare = memcmp((const void *)&tsDevice[wIndex].Short_Add[0]
                            , (const void *)&apTemp2->Short_Add[0]
                            , SHORT_SIZE);
                    if (bCompare)
                    {
                        memcpy((void *) &tsDevice[wIndex].Short_Add[0]
                            , (const void *) &apTemp2->Short_Add[0]
                            , SHORT_SIZE);
                        vfnAllocDev(wIndex, ZERO);
                    }
                    else if((tsDevice[wIndex].Type&0xF0)!=(apTemp2->Type&0xF0)){
                        memcpy((void *) &tsDevice[wIndex].Short_Add[0]
                                , (const void *) &apTemp2->Short_Add[0]
                                , sizeof(DEV_LIST));
                        vfnAllocDev(wIndex, ZERO);
                    }
                    return TRUE;
                }
                else
                {   /*In case of new data*/
                    wIndex = FALSE;
                    bLocated = FALSE;
                    bCompare = TRUE;
                    /*Locates free space in order to write new data*/
                    while (wIndex <= wDevicesIndex)
                    {
                        if(!tsDevice[wIndex].Type)
                        {
                            bCompare=FALSE;
                        }
                        if (!bCompare)
                        {   /*Free Space Located*/
                            bLocated = TRUE;
                            break;
                        }
                        wIndex++;
                    }
                    /*if a free space has been located*/
                    if (bLocated)
                    {
                        /*Data allocation in EEPROM*/
                        memcpy((void *) &tsDevice[wIndex].Short_Add[0]
                                , (const void *) vptrTableStructure
                                , sizeof(DEV_LIST));
                        vfnAllocDev(wIndex, UNKNOWN);
                        wDevicesIndex++;
                        if(wDevicesIndxBckUp <= wIndex)
                        {
                            wDevicesIndxBckUp++;
                        }
                        vfnSaveIndexDev();
                        return TRUE;
                    }
                    else
                    {
                        return FALSE;
                    }
                }
            }/*Just for the very first Data to be allocated*/
            else if (!wDevicesIndex)
            {   /*Data allocation in EEPROM*/
                memcpy((void *) &tsDevice[wDevicesIndex].Short_Add[0]
                        , (const void *) vptrTableStructure
                        , sizeof(DEV_LIST));
                vfnAllocDev(wDevicesIndex, FIRST);
                wDevicesIndex++;
                wDevicesIndxBckUp = wDevicesIndex;
                vfnSaveIndexDev();
                return TRUE;
            }
            else
            {
                return FALSE;
            }
            break;
        case READINGSTABLE:
            /*Validate a Coherent Value read from EEPROM Previously*/
            if (wMetersIndex > 0 && wMetersIndex < NUM_MAX_METERS)
            {   /*Local pointer to Data*/
                apTemp3 = (READING_LIST*) vptrTableStructure;
                /*Seek data to be read*/
                while (wIndex <= (wMetersIndxBckUp - 1))//locates Mac
                {
                    bCompare = memcmp((const void *) & tsMeter[wIndex].Serial_Num[0]
                            , (const void *) & apTemp3->Serial_Num[0]
                            , ID_METER_SIZE);
                    if (!bCompare)
                    {   /*Data Located*/
                        bLocated = TRUE;
                        break;
                    }
                    wIndex++;
                }
                if (bLocated)
                {     /*Starts reading of Data*/
                     wAddressGLOBAL = (WORD)((wIndex*Buffer_Lenght_Single_reading)
                                  +  CAB_READ_Readings_ADD);
                     if(!bfnIIC_MEM24_1025_Write((BYTE*)apTemp3
                                            , wAddressGLOBAL
                                            , Buffer_Lenght_Single_reading)){
                         return FALSE;
                     }
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
            break;
    }
}
/****************************************************************************************/
/*
 * \brief    vfnAllocMtr - Used to allocate based in status
 * \author   Jose Antonio Becerril Morales
 * \param    wIndex in order to know which meter or device
 * \         bSize it can be Meter's or Device's size
 * \         bStatus Known or Unknown
 * \         wAdd address where begin to allocate in case First meter or device
 * \return   void
 * \todo
 */

/****************************************************************************************/
void vfnAllocMtr(WORD wIndex, BYTE bStatus)
{
    WORD wAddress = 0;
    /*bStatus Can be UNKNOWN(new infromation),FIRST(very first Data to be written)
     *and ZERO (Update Data)*/
    switch (bStatus)
    {
        case UNKNOWN:
            /*New EEPROM Address calculation*/
            wAddress = (tsMeter[wIndex-1].Address[0] << 8)
                     | (tsMeter[wIndex-1].Address[1]);
            wAddress += (WORD) METER_NAME_SIZE;
            tsMeter[wIndex].Address[0] = (BYTE) ((wAddress & 0xFF00) >> 8);
            tsMeter[wIndex].Address[1] = (BYTE)  (wAddress & 0x00FF);
            break;
        case FIRST:
            /*First Address Calculation*/
            tsMeter[wIndex].Address[0] = (BYTE) ((CAB_READ_Meter_ADD & 0xFF00) >> 8);
            tsMeter[wIndex].Address[1] = (BYTE) (CAB_READ_Meter_ADD & 0x00FF);
            wAddress = CAB_READ_Meter_ADD;
            break;
        case ZERO:
            /*Identifying Data address*/
            wAddress = (tsMeter[wIndex].Address[0] << 8) | (tsMeter[wIndex].Address[1]);
            break;
    }
    /*CRC Calculation*/
    WORD wCRCTemp = wfnCRC16(&tsMeter[wIndex].Serial_Num[0], METER_NAME_SIZE - CRC_bytes);
    tsMeter[wIndex].CRC[0] = (wCRCTemp & 0xFF00) >> 8;
    tsMeter[wIndex].CRC[1] = (wCRCTemp & 0x00FF);
    /*Start Writing*/
    //Aqui se deshabilitar las interrupciones
    bfnIIC_MEM24_1025_Write(&tsMeter[wIndex].Serial_Num[0]
            , wAddress
            , METER_NAME_SIZE);
}
/****************************************************************************************/
/*
 * \brief    vfnAllocDev - Used to allocate based in status
 * \author   Jose Antonio Becerril Morales
 * \param    wIndex in order to know which meter or device
 * \         bSize it can be Meter's or Device's size
 * \         bStatus Known or Unknown
 * \         wAdd address where begin to allocate in case First meter or device
 * \return   void
 * \todo
 */

/****************************************************************************************/
void vfnAllocDev(WORD wIndex, BYTE bStatus)
{
    WORD wAddress = 0;
    /*bStatus Can be UNKNOWN(new infromation),FIRST(very first Data to be written)
     *and ZERO (Update Data)*/
    switch (bStatus)
    {
        case UNKNOWN:
            /*New EEPROM Address calculation*/
            wAddress = (WORD)((tsDevice[wIndex-1].Address[0] << 8)
                     | (tsDevice[wIndex-1].Address[1]));
            wAddress += (WORD) Buffer_Lenght_MAC_Info;
            tsDevice[wIndex].Address[0] = (BYTE) ((wAddress & 0xFF00) >> 8);
            tsDevice[wIndex].Address[1] = (BYTE) (wAddress & 0x00FF);
            break;
        case FIRST:
            /*First Address Calculation*/
            tsDevice[wIndex].Address[0] = (BYTE) ((CAB_READ_Meter_MAC_SHORT & 0xFF00) >> 8);
            tsDevice[wIndex].Address[1] = (BYTE) (CAB_READ_Meter_MAC_SHORT & 0x00FF);
            wAddress = (tsDevice[wIndex].Address[0] << 8) | (tsDevice[wIndex].Address[1]);
            break;
        case ZERO:
            /*Identifying Data address*/
            wAddress = (tsDevice[wIndex].Address[0] << 8) | (tsDevice[wIndex].Address[1]);
            break;
    }
    /*CRC Calculation*/
    WORD wCRCTemp = wfnCRC16(&tsDevice[wIndex].Short_Add[0]
                           , Buffer_Lenght_MAC_Info - CRC_bytes);
    tsDevice[wIndex].CRC[0] = (wCRCTemp & 0xFF00) >> 8;
    tsDevice[wIndex].CRC[1] = (wCRCTemp & 0x00FF);
    /*Start Writing*/
    bfnIIC_MEM24_1025_Write(&tsDevice[wIndex].Short_Add[0]
            , wAddress
            , Buffer_Lenght_MAC_Info);
}

/****************************************************************************************/
/*
 * \brief    bfnConsultData - Used to consult specific devices or meters
 * \author   Jose Antonio Becerril Morales
 * \param    bTableType, can be Meters or Devices tables
 * \param    bptrKeyID, Data to identify
 * \param    bptrDataWrite, Where data found is goin to be allocated
 * \param    wAllDataIndexDev and wAllDataIndexMtr. Device or meter to consult
 * \return   TRUE-read successful,FALSE-read error
 * \todo
 */

/****************************************************************************************/
BYTE bfnConsultData(BYTE bTableType, BYTE *bptrKeyID, WORD wAllDataIndexDev
        , WORD wAllDataIndexMtr, BYTE *bptrDataWrite, WORD * bptrDataWriteSize)
{
    WORD wIndex      = FALSE;
    WORD wIndexBckUp = FALSE;
    BYTE bLocated    = FALSE;
    BYTE bCompare    = FALSE;
    BYTE *apTemp2;
    static WORD wIndexm=FALSE;
    bSpace          = FALSE;
    
    * bptrDataWriteSize = 0;
    
    switch (bTableType)
    {
        case METERSTABLE:
            /*If there is at least one meter*/
            if (wMetersIndex)
            {
                wIndex = FALSE;
                bLocated = FALSE;
                apTemp2 = bptrKeyID;
                /*Locates Data to Consult*/
                while (wIndexBckUp <= (wMetersIndex))
                {   /*Blank Spaces Filter*/
                    if(tsMeter[wIndex].Type)
                    {
                        bCompare = memcmp((const void *) & tsMeter[wIndex].Serial_Num[0]
                                , (const void *) & apTemp2[0]
                                , ID_METER_SIZE);
                        if (!bCompare)
                        {   /*Data Located*/
                            bLocated = TRUE;
                            break;
                        }
                        else{
                           wIndexBckUp++;
                       }
                    }
                    wIndex++;
                }
                if (bLocated)
                {   /*if located store data in a buffer defined by user*/
                    if(!bptrDataWrite){
                        return TRUE;
                    }
                    memcpy((void*) bptrDataWrite
                            , (const void*) &tsMeter[wIndex].Serial_Num[0]
                            , METER_NAME_SIZE - 4);
                    return TRUE;
                }
                else
                {   /*if not located*/
                    return FALSE;
                }
            }
            else
            {   /*There is no Meter*/
                return FALSE;
            }
            break;
        case DEVICESTABLE:
            /*If there is at least one device*/
            if (wDevicesIndex)
            {
                apTemp2 = bptrKeyID;
                /*Locates Data to Consult*/
                while (wIndex <= (wDevicesIndex))//Locates MAC
                {
                    bCompare = memcmp((BYTE *) & tsDevice[wIndex].MAC[0]
                            , (BYTE *) & apTemp2[0]
                            , MAC_SIZE);
                    if (!bCompare)
                    {   /*Data located*/
                        bLocated = TRUE;
                        break;
                    }
                    wIndex++;
                }
                if (bLocated)
                {    /*if located store data in a buffer defined by user*/
                    memcpy((void*) bptrDataWrite
                            , (const void*) &tsDevice[wIndex].Short_Add[0]
                            , Buffer_Lenght_MAC_Info - 4);
                    return TRUE;
                  }
                else
                {   /*if not located*/
                    return FALSE;
                }
            }
            else
            {   /*There is no Device*/
                return FALSE;
            }
            break;
        case DEVICESTABLE_SHORT:
            /*If there is at least one device*/
            if (wDevicesIndex)
            {
                apTemp2 = bptrKeyID;
                /*Locates Data to Consult*/
                while (wIndex <= (wDevicesIndex))//Locates MAC
                {
                    bCompare = memcmp((BYTE *) & tsDevice[wIndex].Short_Add[0]
                            , (BYTE *) & apTemp2[0]
                            , SHORT_SIZE);
                    if (!bCompare)
                    {   /*Data located*/
                        bLocated = TRUE;
                        break;
                    }
                    wIndex++;
                }
                if (bLocated)
                {    /*if located store data in a buffer defined by user*/
                    memcpy((void*) bptrDataWrite
                            , (const void*) &tsDevice[wIndex].Short_Add[0]
                            , Buffer_Lenght_MAC_Info - 4);
                    return TRUE;
                }
                else
                {   /*if not located*/
                    return FALSE;
                }
            }
            else
            {   /*There is no Device*/
                return FALSE;
            }
            break;
        case READINGSTABLE:
            /*Locates Data to Consult*/

            if(wIndexm >= (wMetersIndex)){
                wIndexm=FALSE;
                return FALSE;
            }
            while (wIndexm <= (wMetersIndex))//Locates MAC
            {   /*Blank Spaces Filter*/
                if(tsMeter[wIndexm].Type)
                {
                    bCompare = memcmp((BYTE *) & tsMeter[wIndexm].MAC_Gabinet
                            , (BYTE *) & bptrKeyID[0]
                            , MAC_SIZE);
                }
                if (!bCompare)
                {   /*Data Located*/
                    bLocated = TRUE;
                    break;
                }
                wIndexm++;
            }
            if (bLocated)
            {
                if(!bptrDataWrite){
                    return TRUE;
                }
                memcpy((void*) bptrDataWrite
                        , (const void*) &tsMeter[wIndexm++].Serial_Num[0]
                        , METER_NAME_SIZE_FRAG);
                return TRUE;
            }
            else{
                return FALSE;
            }
            break;
        case ALLDATADEV:
            /*Store data in a buffer defined by user*/
            if(tsDevice[wAllDataIndexDev].Type)
            {
                memcpy((void*) bptrDataWrite
                        , (const void*) &tsDevice[wAllDataIndexDev].Short_Add[0]
                        , Buffer_Lenght_MAC_Info - 4);
                * bptrDataWriteSize = Buffer_Lenght_MAC_Info - 4;
                return TRUE;
            }else
            {
                return FALSE;
            }
            break;
        case ALLDATAMTR:
            /*Store data in a buffer defined by user*/
             if(tsMeter[wAllDataIndexMtr].Type)
             {
                memcpy((void*) bptrDataWrite
                        , (const void*) &tsMeter[wAllDataIndexMtr].Serial_Num[0]
                        , METER_NAME_SIZE - 4);
                
                * bptrDataWriteSize = METER_NAME_SIZE - 4;
                return TRUE;
             }else
             {
                return FALSE;
             }
            break;
        case ALLDATAREADINGS:
            /*Blank Spaces Filter*/
            if(tsMeter[wAllDataIndexMtr].Type)
            {   /*Store data in a buffer defined by user*/
                bBufferIICRead = bptrDataWrite;
                wAddressGLOBAL = (WORD)(((wAllDataIndexMtr)*Buffer_Lenght_Single_reading) + CAB_READ_Readings_ADD);
                bfnIIC_MEM24_1025_Read(wAddressGLOBAL
                                    , (WORD)Buffer_Lenght_Single_reading);
                return TRUE;
            }else
            {
                return FALSE;
            }
            break;
    }
}
/****************************************************************************************/
/*
 * \brief    wfnIndexConsutl - Devices and Meters Index Consult
 * \author   Jose Antonio Becerril Morales
 * \param    bTableType, it can be Meters or Devices tables
 * \return   wDevicesIndex or wMetersIndex
 * \todo
 */

/****************************************************************************************/
WORD wfnIndexConsutl(BYTE bTableType)
{   /*Returns Index Value*/
    switch (bTableType)
    {
        case METERSTABLE:
            return wMetersIndex;
            break;
        case DEVICESTABLE:
            return wDevicesIndex;
            break;
    }
}
/****************************************************************************************/
/*
 * \brief    bfnDelAllData - Deletes devices and meters index, it means that there's no
 * \data stored
 * \author   Jose Antonio Becerril Morales
 * \param    bTableType, it can be Meters, Devices or delete all tables
 * \return   TRUE-erased successful ,FALSE-erased error
 * \todo
 */

/****************************************************************************************/
BYTE bfnDelAllData(BYTE bTableType,BYTE *bptrKeyID)
{
    WORD wIndex           = FALSE;
    WORD wIndexBckUp      = FALSE;
    BYTE bCompare         = FALSE;
    BYTE bLocated         = FALSE;
    WORD wAddress;

    switch (bTableType)
    {
        case METER:/*Delete a Data Meter Request*/
            /*Locates Data to Consult*/
            while (wIndex <= (wMetersIndxBckUp))//locates Serial
                {   /*Blank space filters*/
                    if(tsMeter[wIndex].Type)
                    {
                        bCompare = memcmp((const void *) & tsMeter[wIndex].Serial_Num[0]
                                , (const void *) & bptrKeyID[0]
                                , ID_METER_SIZE);
                        if (!bCompare)
                        {   /*Data Located*/
                            bLocated = TRUE;
                            break;
                        }
                     }
                     wIndex++;
                }
            if (bLocated)
            {   /*RAM and EEPROM erase*/
                 wAddress = (tsMeter[wIndex].Address[0] << 8)
                          | (tsMeter[wIndex].Address[1]);
                 memset((BYTE*)&tsMeter[wIndex].Serial_Num[0],FALSE,METER_NAME_SIZE);
                 if(!bfnIIC_MEM24_1025_Write((BYTE*)&tsMeter[wIndex].Serial_Num
                                        , wAddress
                                        , METER_NAME_SIZE)){
                     return FALSE;
                 }
                 /*Decrement number of meters*/
                 wMetersIndex--;
                 /*if wIndex is the las space*/
                 if(wIndex == (wMetersIndxBckUp-1))
                 {  /*Decremente number of spaces to read*/
                     wMetersIndxBckUp--;
                 }
                 /*if there is no more meter ensure reset*/
                 if(!wMetersIndex)
                 {
                     wMetersIndxBckUp = FALSE;
                 }
                 return TRUE;
                 //vfnSaveIndexMet();
            }
            break;
        case DEVICE:/*Delete a Data Device Request*/
            /*Locates Data to Consult*/
            while (wIndex <= (wDevicesIndxBckUp))//locates Serial
                {   /*Blank space filters*/
                    if(tsDevice[wIndex].Type)
                    {
                        bCompare = memcmp((const void *) & tsDevice[wIndex].MAC[0]
                                , (const void *) & bptrKeyID[0]
                                , MAC_SIZE);
                        if (!bCompare)
                        {   /*Data Located*/
                            bLocated = TRUE;
                            break;
                        }
                     }
                     wIndex++;
                }
            if (bLocated)
            {   /*RAM and EEPROM erase*/
                 wAddress = (tsDevice[wIndex].Address[0] << 8)
                          | (tsDevice[wIndex].Address[1]);
                 memset((BYTE*)&tsDevice[wIndex].Short_Add[0],FALSE,sizeof(MAC_Short_Type));
                 if(!bfnIIC_MEM24_1025_Write((BYTE*)&tsDevice[wIndex].Short_Add[0]
                                        , wAddress
                                        , sizeof(MAC_Short_Type))){
                     return FALSE;
                 }
                 /*Decrement number of devices*/
                 wDevicesIndex--;
                 /*if wIndex is the last space*/
                 if(wIndex == (wDevicesIndxBckUp-1))
                 {  /*Decremente number of spaces to read*/
                     wDevicesIndxBckUp--;
                 }
                 /*if there is no more meter ensure reset*/
                 if(!wDevicesIndex)
                 {
                     wDevicesIndxBckUp = FALSE;
                 }
                 vfnSaveIndexDev();
                 return TRUE;
            }
            break;
        case METERSTABLE:
            /*Default Reset in RAM and EEPROM*/
            wMetersIndex     = FALSE;
            wMetersIndxBckUp = FALSE;
            vfnSaveIndexMet();
            memset((BYTE *) & tsMeter[0].Serial_Num[0], 0x00
                    , METER_NAME_SIZE * NUM_MAX_METERS);
            return TRUE;
            break;
        case DEVICESTABLE:
            /*Default Reset in RAM and EEPROM*/
            wDevicesIndex = FALSE;
            wDevicesIndxBckUp = FALSE;
            vfnSaveIndexDev();
            memset((BYTE *) & tsDevice[0].Short_Add[0], 0x00
                    , Buffer_Lenght_MAC_Info * NUM_MAX_NODES);
            return TRUE;
            break;
        case DELETEALL:
            /*Default Reset in RAM and EEPROM*/
            wDevicesIndex = FALSE;
            memset((BYTE *) & tsDevice[0].Short_Add[0], 0x00
                    , Buffer_Lenght_MAC_Info * NUM_MAX_NODES);
            BYTE waDevicesIndex[2];
            waDevicesIndex[1] = (BYTE) ((wDevicesIndex & 0xFF00) >> 8);
            waDevicesIndex[0] = (BYTE) (wDevicesIndex & 0x00FF);
            wDevicesIndxBckUp = FALSE;
            if(!bfnIIC_MEM24_1025_Write(&waDevicesIndex[0]
                    , NODES_INDEX_ADD
                    , Address_Size)){
                return FALSE;
            }
            wMetersIndex = FALSE;
            wMetersIndxBckUp = FALSE;
            memset((BYTE *) & tsMeter[0].Serial_Num[0], 0x00
                    , METER_NAME_SIZE * NUM_MAX_METERS);
            vfnSaveIndexMet();
            return TRUE;
            break;
    }
    return FALSE;
}
/****************************************************************************************/
/*
 * \brief    vfnEnd - Used to end the process intializing variables
 * \author   Jose Antonio Becerril Morales
 * \param    void
 * \return   void
 * \todo
 */
/****************************************************************************************/
void vfnEnd(void)
{

}
/****************************************************************************************/
/****************************************************************************************/

