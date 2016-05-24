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
#include "MEMEEPROM.h"
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

BOOL temporal;

/*-- Variables -------------------------------------------------------------------------*/
/*Empty Array*/
BYTE bEmpty[MAC_SIZE];
/*Pointers to Tables*/
Meter_Eneri *tsPtrMeter;
Device_Eneri *tsPtrDevice;
/*Devices Index*/
static WORD wDevicesIndex;
static WORD wDevicesIndxBckUp;       //Numero hasta donde hay medidores validos.
/*Meters Index*/
static WORD wMetersIndex;           //Numero de medidores valido.
static WORD wMetersIndxBckUp;       //Numero hasta donde hay medidores validos.
/*Tables declaration*/
Meter_Eneri    tsMeter[NUM_MAX_METERS];
Device_Eneri tsDevice[NUM_MAX_NODES];
Meter_Eneri    *apTemp;
READING_LIST   *apTemp3;
WORD wAddressGLOBAL;
BYTE bSpace;
BYTE bSpace1;
//BYTE tsReading[];

void DataBaseHandler_SetupQuery(DATA_BASE_HANDLER_QUERY_PTR query, WORD startItem, WORD quantityOfItems, DATA_BASE_HANDLER_LIST_TYPE tableListType) {

    if(query == NULL)
        return;
    
    DataBaseHandler_ClearQuery(query);
    
    query->startItem = startItem;
    query->quantityOfItems = quantityOfItems;
    query->tableListType = tableListType;
    
    return;
}

void DataBaseHandler_ClearQuery(DATA_BASE_HANDLER_QUERY_PTR query){
    
    if(query == NULL)
        return;
    
    memset(query, 0, sizeof(DATA_BASE_HANDLER_QUERY));
    return;
}

WORD DataBaseHandler_GetStartItem(DATA_BASE_HANDLER_QUERY_PTR query){
    
    return query->startItem;
}

void DataBaseHandler_SetStartItem(DATA_BASE_HANDLER_QUERY_PTR query, WORD startItem){
    
    if(query == NULL)
        return;
    
    query->startItem = startItem;
    return;
}

WORD DataBaseHandler_GetQuantityOfItems(DATA_BASE_HANDLER_QUERY_PTR query){
    
    return query->quantityOfItems;
}

void DataBaseHandler_SetQuantityOfItems(DATA_BASE_HANDLER_QUERY_PTR query, WORD quantityOfItems){
    
    if(query == NULL)
        return;
    
    query->quantityOfItems = quantityOfItems;
    return;
}

DATA_BASE_HANDLER_LIST_TYPE DataBaseHandler_GetTableListType(DATA_BASE_HANDLER_QUERY_PTR query){
    
    return query->tableListType;
}

BYTE * DataBaseHandler_GetQueryResponseBuffer(DATA_BASE_HANDLER_QUERY_PTR query){
    
    return query->queryResponseBuffer;
}

WORD DataBaseHandler_GetQueryResponseBufferSize(DATA_BASE_HANDLER_QUERY_PTR query){
    
    return query->queryResponseBufferSize;
}

void DataBaseHandler_SetQueryResponseBufferSize(DATA_BASE_HANDLER_QUERY_PTR query, WORD queryResponseBufferSize){

    if(query == NULL)
        return;
    
    query->queryResponseBufferSize = queryResponseBufferSize;
    return;    
}

BOOL DataBaseHandler_IsWaitingForQueryResponse(DATA_BASE_HANDLER_QUERY_PTR query){
    
    return query->isWaitingForQueryResponse;
}

void DataBaseHandler_SetWaitingForQueryResponse(DATA_BASE_HANDLER_QUERY_PTR query, BOOL isWaitingForQueryResponse){
    
    if(query == NULL)
        return;
    
    query->isWaitingForQueryResponse = isWaitingForQueryResponse;
    return;
}


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
    //! bBufferIICRead = (BYTE*) & wDevicesIndxBckUp;
    API_MEM24_1025_I2C_Read(NODES_INDEX_BCKUP, (BYTE*) & wDevicesIndxBckUp, Address_Size, &temporal);
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
        //! bBufferIICRead = (BYTE*) & wDevicesIndex;
        API_MEM24_1025_I2C_Read(NODES_INDEX_ADD, (BYTE*) & wDevicesIndex, Address_Size, &temporal);
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
{   
    #ifndef DATA_BASE_TEST
    /*Asks if EEPROM is Busy*/
    if (!bfnIICIsBusy())
    {   /*Validate a Coherent Value read from EEPROM Previously*/
        if (wDevicesIndxBckUp > 0 && wDevicesIndxBckUp < NUM_MAX_NODES)
        {   /*Starts Reading*/
            static WORD wIndexDevicesTemp=FALSE;
            if(wDevicesIndex>=NUM_MAX_NODES)wDevicesIndex=0;
            //! bBufferIICRead = (BYTE *) & tsDevice[wIndexDevicesTemp].Short_Add[0];
            if(API_MEM24_1025_I2C_Read(CAB_READ_Meter_MAC_SHORT+(wIndexDevicesTemp*Buffer_Lenght_MAC_Info), (BYTE *) & tsDevice[wIndexDevicesTemp].Short_Add[0],
                    Buffer_Lenght_MAC_Info, &temporal)){
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
            //! bBufferIICRead = (BYTE*) & wMetersIndex;
            API_MEM24_1025_I2C_Read(METERS_INDEX_ADD, (BYTE*) & wMetersIndex, Address_Size, &temporal);
            _tBackStartUpSM.bActualState = _BACKUP_METERS_INDEX_STATE;
            _tBackStartUpSM.bNextState = _BACKUP_END_STATE;
        }
    }
    #endif
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
        //! bBufferIICRead = (BYTE *) & wMetersIndxBckUp;
        API_MEM24_1025_I2C_Read(METERS_INDEX_BCKUP, (BYTE *) & wMetersIndxBckUp, Address_Size, &temporal);
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
        //bBufferIICRead = (BYTE *) & wMetersIndex;
        API_MEM24_1025_I2C_Read(METERS_INDEX_ADD, (BYTE *) & wMetersIndex, Address_Size,&temporal );
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
            //bBufferIICRead = (BYTE *) & tsMeter[wMeterIndexTemp];
            if(API_MEM24_1025_I2C_Read(CAB_READ_Meter_ADD+(wMeterIndexTemp*METER_NAME_SIZE), (BYTE *) & tsMeter[wMeterIndexTemp], METER_NAME_SIZE, &temporal)){
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
        if(API_MEM24_1025_I2C_Write(&waDevicesIndex[0]
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
        if(API_MEM24_1025_I2C_Write(&waMetersIndex[0]
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
{   
    #ifndef DATA_BASE_TEST
    /*Asks if EEPROM is busy*/
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
                     if(!API_MEM24_1025_I2C_Write((BYTE*)apTemp3
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
    #else
    return TRUE;
    #endif
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
    #ifndef DATA_BASE_TEST
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
    API_MEM24_1025_I2C_Write(&tsMeter[wIndex].Serial_Num[0]
            , wAddress
            , METER_NAME_SIZE);
    #endif
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
    #ifndef DATA_BASE_TEST
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
    API_MEM24_1025_I2C_Write(&tsDevice[wIndex].Short_Add[0]
            , wAddress
            , Buffer_Lenght_MAC_Info);
    #endif // DATA_BASE_TEST
    
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
        , WORD wAllDataIndexMtr, DATA_BASE_HANDLER_QUERY_PTR query)
{
    #ifdef DATA_BASE_TEST
    WORD wIndex      = FALSE;
    WORD wIndexBckUp = FALSE;
    BYTE bLocated    = FALSE;
    BYTE bCompare    = FALSE;
    BYTE *apTemp2;
    static WORD wIndexm=FALSE;
    bSpace          = FALSE;
    
    BYTE * query_response_buffer_ptr =  DataBaseHandler_GetQueryResponseBuffer(query);
    //* bptrDataWriteSize = 0;
    
    
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
                    if(tsMeter[wIndex].meterItem.Type)
                    {
                        bCompare = memcmp((const void *) & tsMeter[wIndex].meterItem.Serial_Num[0]
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
                    if(!query_response_buffer_ptr){
                        return TRUE;
                    }
                    memcpy(query_response_buffer_ptr
                            , (const void*) &tsMeter[wIndex].meterItem.Serial_Num[0]
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
                    bCompare = memcmp((BYTE *) & tsDevice[wIndex].deviceItem.MAC[0]
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
                    memcpy(query_response_buffer_ptr
                            , (const void*) &tsDevice[wIndex].deviceItem.Short_Add
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
                    bCompare = memcmp((BYTE *) & tsDevice[wIndex].deviceItem.Short_Add
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
                    memcpy(query_response_buffer_ptr
                            , (const void*) &tsDevice[wIndex].deviceItem.Short_Add
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
                if(tsMeter[wIndexm].meterItem.Type)
                {
                    bCompare = memcmp((BYTE *) & tsMeter[wIndexm].meterItem.MAC_Cabinet
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
                if(!query_response_buffer_ptr){
                    return TRUE;
                }
                memcpy(query_response_buffer_ptr
                        , (const void*) &tsMeter[wIndexm++].meterItem.Serial_Num[0]
                        , METER_NAME_SIZE_FRAG);
                return TRUE;
            }
            else{
                return FALSE;
            }
            break;
        case ALLDATADEV:
            /*Store data in a buffer defined by user*/
            if(tsDevice[wAllDataIndexDev].deviceItem.Type)
            {
                memcpy( query_response_buffer_ptr
                        , (const void*) &tsDevice[wAllDataIndexDev].deviceItem.Short_Add
                        , Buffer_Lenght_MAC_Info - 4);
                
                DataBaseHandler_SetQueryResponseBufferSize(query, Buffer_Lenght_MAC_Info - 4);
                
                
                return TRUE;
            }else
            {
                return FALSE;
            }
            break;
        case ALLDATAMTR:
            /*Store data in a buffer defined by user*/
             if(tsMeter[wAllDataIndexMtr].meterItem.Type)
             {
                memcpy(query_response_buffer_ptr
                        , (const void*) &tsMeter[wAllDataIndexMtr].meterItem.Serial_Num[0]
                        , METER_NAME_SIZE - 4);
                
                DataBaseHandler_SetQueryResponseBufferSize(query, METER_NAME_SIZE - 4);
                return TRUE;
             }else
             {
                return FALSE;
             }
            break;
        case ALLDATAREADINGS:
            /*Blank Spaces Filter*/
            if(tsMeter[wAllDataIndexMtr].meterItem.Type)
            {   /*Store data in a buffer defined by user*/
                //! bBufferIICRead = query_response_buffer_ptr;
                wAddressGLOBAL = (WORD)(((wAllDataIndexMtr)*Buffer_Lenght_Single_reading) + CAB_READ_Readings_ADD);
                //! API_MEM24_1025_I2C_Read(wAddressGLOBAL, query_response_buffer_ptr, (WORD)Buffer_Lenght_Single_reading, &query->isWaitingForQueryResponse);
                MEM_EEPROM_Read(wAddressGLOBAL, query_response_buffer_ptr, (WORD)Buffer_Lenght_Single_reading);                
                DataBaseHandler_SetQueryResponseBufferSize(query, Buffer_Lenght_Single_reading);
                DataBaseHandler_SetWaitingForQueryResponse(query, FALSE);
                
                return TRUE;
            }else
            {
                return FALSE;
            }
            break;
    }
    #else
    return TRUE;
    #endif // DATA_BASE_TEST
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
    #ifndef DATA_BASE_TEST
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
                 if(!API_MEM24_1025_I2C_Write((BYTE*)&tsMeter[wIndex].Serial_Num
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
                 if(!API_MEM24_1025_I2C_Write((BYTE*)&tsDevice[wIndex].Short_Add[0]
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
            if(!API_MEM24_1025_I2C_Write(&waDevicesIndex[0]
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
    #else
    return TRUE;
    #endif // DATA_BASE_TEST
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

WORD DataBaseHandler_FindMeterTableIndexBySerialNumber(BYTE * serialNumber){
	
    Meter_Eneri_PTR meter_ptr = tsMeter;	
    WORD index = 0;

    while(index < NUM_MAX_METERS){

        if(!memcmp(meter_ptr->meterItem.Serial_Num, serialNumber, ID_METER_SIZE))
            break;

        meter_ptr++;
        index++;
    }

    if(index == NUM_MAX_METERS)
        return DATA_BASE_HANDLER_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE;

    return index;
}

WORD DataBaseHandler_FindAvailableMeterTableIndex(void){
	
    Meter_Eneri_PTR meter_ptr = tsMeter;	
    WORD index = 0;

    while(index < NUM_MAX_METERS) {

        if(!meter_ptr->meterItem.Type)
            break;

        meter_ptr++;
        index++;
    }
	
    if(index == NUM_MAX_METERS)
	return DATA_BASE_HANDLER_NUM_MAX_METERS_REACHED_ERROR_CODE;
		
    return index;
}

BYTE DataBaseHandler_SetupMeterTableItemByIndex(WORD index, MTR_LIST_PTR meterItem){
	
    BYTE error_code;
    Meter_Eneri_PTR meter_ptr = DataBaseHandler_GetMeterTableByIndex(index);
	
    if (meter_ptr == NULL)
        return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;
	
    // Clear Meter_Eneri Struct
    memset(meter_ptr, 0, sizeof(Meter_Eneri));

    memcpy((BYTE *) &meter_ptr->meterItem, meterItem, sizeof(MTR_LIST)); 		
    meter_ptr->Address = (CAB_READ_Meter_ADD + (index * sizeof(Meter_Eneri)));	
    meter_ptr->CRC = wfnCRC16( (BYTE *) meter_ptr, sizeof(Meter_Eneri) - sizeof(WORD));	
    //error_code = API_MEM24_1025_I2C_Write( (BYTE *) meter_ptr, meter_ptr->Address, sizeof(Meter_Eneri));	
    MEM_EEPROM_Write( meter_ptr->Address, (BYTE *) meter_ptr,  sizeof(Meter_Eneri));	
    //!print_error("ERROR CODE: %d", error_code);
    
    return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

BYTE DataBaseHandler_AddNewMeterTableItem(MTR_LIST_PTR meterItem){
	
    WORD index;

    index = DataBaseHandler_FindAvailableMeterTableIndex();	

    if(index == DATA_BASE_HANDLER_NUM_MAX_METERS_REACHED_ERROR_CODE)	
        return DATA_BASE_HANDLER_NUM_MAX_METERS_REACHED_ERROR_CODE;
    
    print_info("Adding New Meter Table");
    return DataBaseHandler_SetupMeterTableItemByIndex(index, meterItem);
    
    
}

Meter_Eneri_PTR DataBaseHandler_GetMeterTableByIndex(WORD index){
	
    if (index >= NUM_MAX_METERS)
	return NULL;
	
    return (tsMeter + index);
}

WORD DataBaseHandler_UpdateMeterTableItemByIndex(WORD index, MTR_LIST_PTR meterItem){
	
    Meter_Eneri_PTR foundMeter;
    
    foundMeter = DataBaseHandler_GetMeterTableByIndex(index);

    if(foundMeter == NULL)
        return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;

    if( memcmp((BYTE *) &foundMeter->meterItem, meterItem, sizeof(MTR_LIST))){		
        
        print_info("Updating Meter Table");
        return DataBaseHandler_SetupMeterTableItemByIndex(index, meterItem);
    }

    print_info("Meter Table Index did not change");
    return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

BYTE DataBaseHandler_SaveMeterTableItem(MTR_LIST_PTR meterItem){
	
    WORD index;		
    WORD error_code;

    index = DataBaseHandler_FindMeterTableIndexBySerialNumber(meterItem->Serial_Num);

    if(index == DATA_BASE_HANDLER_SERIAL_NUMBER_NOT_FOUND_ERROR_CODE){		

        error_code = DataBaseHandler_AddNewMeterTableItem(meterItem);

        if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
            return 0;

        wMetersIndex++;
        return NEW_MTR_ADD;
    }

    error_code = DataBaseHandler_UpdateMeterTableItemByIndex(index, meterItem);

    if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
        return 0;

    return NO_NEW_MTR_ADD;	
}

WORD DataBaseHandler_FindDeviceTableIndexByMACAddress(BYTE * macAddress){
	
    Device_Eneri_PTR device_ptr = tsDevice;	
    WORD index = 0;
	
    while(index < NUM_MAX_NODES){
		
        if(!memcmp(device_ptr->deviceItem.MAC, macAddress, MAC_SIZE))
            break;
		
        device_ptr++;
        index++;
    }
	
    if(index == NUM_MAX_NODES)
        return DATA_BASE_HANDLER_MAC_ADDRESS_NOT_FOUND_ERROR_CODE;
		
    return index;
}

WORD DataBaseHandler_FindAvailableDeviceTableIndex(void){
	
    Device_Eneri_PTR device_ptr = tsDevice;	
    WORD index = 0;
	
    while(index < NUM_MAX_NODES){
		
        if(!device_ptr->deviceItem.Type)
            break;
			
        device_ptr++;
        index++;
    }
	
    if(index == NUM_MAX_NODES)
        return DATA_BASE_HANDLER_NUM_MAX_NODES_REACHED_ERROR_CODE;
		
    return index;
}

Device_Eneri_PTR DataBaseHandler_GetDeviceTableByIndex(WORD index){
	
	if (index >= NUM_MAX_NODES)
		return NULL;
	
	return (tsDevice + index);
}

BYTE DataBaseHandler_SetupDeviceTableItemByIndex(WORD index, DEV_LIST_PTR deviceItem){
	
	Device_Eneri_PTR device_ptr = DataBaseHandler_GetDeviceTableByIndex(index);
	
	if (device_ptr == NULL)
		return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;
	
	// Clear Meter_Eneri Struct
	memset(device_ptr, 0, sizeof(Device_Eneri));
		
	memcpy( (BYTE *) &device_ptr->deviceItem, deviceItem, sizeof(DEV_LIST)); 		
	device_ptr->Address = (CAB_READ_Meter_MAC_SHORT + (index * sizeof(Device_Eneri)));	
	device_ptr->CRC = wfnCRC16( (BYTE *) device_ptr, sizeof(Device_Eneri) - sizeof(WORD));	
	//!API_MEM24_1025_I2C_Write( (BYTE *) device_ptr, device_ptr->Address, sizeof(Device_Eneri));	
	MEM_EEPROM_Write(  device_ptr->Address, (BYTE *) device_ptr, sizeof(Device_Eneri));	
	return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

BYTE DataBaseHandler_AddNewDeviceTableItem(DEV_LIST_PTR deviceItem){
	
	WORD index;
	
	index = DataBaseHandler_FindAvailableDeviceTableIndex();	
	
	if(index == DATA_BASE_HANDLER_NUM_MAX_NODES_REACHED_ERROR_CODE)	
		return index;
		
        
	return DataBaseHandler_SetupDeviceTableItemByIndex(index, deviceItem);
}


WORD DataBaseHandler_UpdateDeviceTableItemByIndex(WORD index, DEV_LIST_PTR deviceItem){
	
	Device_Eneri_PTR foundDevice;	
		
	foundDevice = DataBaseHandler_GetDeviceTableByIndex(index);
	
	if(foundDevice == NULL)
		return DATA_BASE_HANDLER_NULL_STRUCT_ERROR_CODE;
	
	if( memcmp( (BYTE *) &foundDevice->deviceItem, deviceItem, sizeof(DEV_LIST))){		
		
		return DataBaseHandler_SetupDeviceTableItemByIndex(index, deviceItem);
	}
	
	return DATA_BASE_HANDLER_NO_ERROR_CODE;
}


BYTE DataBaseHandler_SaveDeviceTableItem(DEV_LIST_PTR deviceItem){
	
	WORD index;		
	WORD error_code;
	
	index = DataBaseHandler_FindDeviceTableIndexByMACAddress(deviceItem->MAC);
			
	if(index == DATA_BASE_HANDLER_MAC_ADDRESS_NOT_FOUND_ERROR_CODE){		
		
		error_code = DataBaseHandler_AddNewDeviceTableItem(deviceItem);
		
		if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
			return 0;
		
		// NOTE: It used to call vfnSaveIndexDev();
                wDevicesIndex++;
		return 1;
	}
	
	error_code = DataBaseHandler_UpdateDeviceTableItemByIndex(index, deviceItem);
	
	if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
		return 0;
	
	// NOTE: It used to call vfnSaveIndexDev();		
	return 1;	
}

BYTE DataBaseHandler_AddNewReadingTableItemByIndex(WORD index, READING_LIST_PTR readingItem){
	
	WORD address;
	
	address = (CAB_READ_Readings_ADD + (index * sizeof(READING_LIST)));	
	
	//! API_MEM24_1025_I2C_Write( (BYTE *) readingItem, address, sizeof(READING_LIST));	
        MEM_EEPROM_Write(  address, (BYTE *) readingItem, sizeof(READING_LIST));	
	
	return DATA_BASE_HANDLER_NO_ERROR_CODE;
}

BYTE DataBaseHandler_SaveReadingTableItem(READING_LIST_PTR readingItem){
	
	WORD index;		
	WORD error_code;
	
	index = DataBaseHandler_FindMeterTableIndexBySerialNumber(readingItem->Serial_Num);
			
	if(index == DATA_BASE_HANDLER_MAC_ADDRESS_NOT_FOUND_ERROR_CODE){		
	
		return 0;
	}
	
	error_code = DataBaseHandler_AddNewReadingTableItemByIndex(index, readingItem);
	
	if(error_code != DATA_BASE_HANDLER_NO_ERROR_CODE)
		return 0;
	
		
	return 1;	
}

BYTE API_DataBaseHandler_SaveTable(BYTE bTableType, BYTE *vptrTableStructure ) {			
	
	switch(bTableType){
		
		case METERSTABLE:			
			
			return DataBaseHandler_SaveMeterTableItem((MTR_LIST_PTR) vptrTableStructure);	
			
		case DEVICESTABLE:		
		
			return DataBaseHandler_SaveDeviceTableItem((DEV_LIST_PTR) vptrTableStructure);
			
		case READINGSTABLE:
		
			return DataBaseHandler_SaveReadingTableItem((READING_LIST_PTR) vptrTableStructure);			
	}
	
	return 0;
}
