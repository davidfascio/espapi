//******************************************************************************
//* File ESPMeteringTable.c
//

//******************************************************************************
// Includes
//******************************************************************************
#include "ESPMeteringTable.h"

//******************************************************************************
// Variables
//******************************************************************************
ESP_METERING_TABLE_QUERY espMeteringTableQuery;

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void vfnBufferIdleState(void);
void vfnBufferConsultTableState(void);
void vfnBufferTransmitTableState(void);
void vfnBufferEndState(void);

typedef enum {
    _BUFFER_METER_IDLE_STATE = 0,
    _BUFFER_CONSULT_TABLE_STATE,
    _BUFFER_TRANSMIT_TABLE_STATE,
    _BUFFER_END_STATE
} eBUFFERDriverStates;

void (*const vfnaBufferDriverState[])(void) =
{
    vfnBufferIdleState
    ,vfnBufferConsultTableState
    ,vfnBufferTransmitTableState
    ,vfnBufferEndState    
};

sSM _tBufferSM = {  _BUFFER_METER_IDLE_STATE,               /* bActualState     */
                    _BUFFER_METER_IDLE_STATE,               /* bNextState       */
                    _BUFFER_METER_IDLE_STATE,               /* bPrevState       */
                    _BUFFER_METER_IDLE_STATE};              /* bErrorState      */

//******************************************************************************
// ESPMeteringTable Function Prototypes
//******************************************************************************
BYTE bfnBuffer_Table_Meter(WORD quantityOfItems, ESP_METERING_TABLE_LIST_TYPE tableListType) {

    switch (tableListType) {

        case METER_TABLE_LIST:
        case READING_TABLE_LIST:
        case DEVICE_TABLE_LIST:
            break;

        default:
            return FALSE;
    }
    
    ESPMeteringTable_SetupQuery(&espMeteringTableQuery, 0, quantityOfItems, tableListType);
    ESPMeteringTable_SetStateMachine(_BUFFER_CONSULT_TABLE_STATE, _BUFFER_TRANSMIT_TABLE_STATE);
    
    return TRUE;
}

//******************************************************************************
// ESP_API Function Prototypes
//******************************************************************************
BYTE bfnReadMTRSTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    WORD wIndexData;    
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    wIndexData = wfnIndexConsutl(METERSTABLE);

    if (wIndexData) {

        * pagingDataResponseSize = (wIndexData * sizeof (MTR_LIST));
        
        //! bfnBuffer_Table_Meter(wIndexData, sizeof(MTR_LIST), &bpOutputData[0]); 
        bfnBuffer_Table_Meter(wIndexData, METER_TABLE_LIST);
        
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnReadDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    WORD wIndexData;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    wIndexData = wfnIndexConsutl(DEVICESTABLE);

    if (wIndexData) {

        * pagingDataResponseSize = (wIndexData * sizeof (DEV_LIST));
        bfnBuffer_Table_Meter(wIndexData, DEVICE_TABLE_LIST);
        
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    WORD wIndexData;
    WORD wDataSize;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    wIndexData = wfnIndexConsutl(METERSTABLE);

    if (wIndexData) {

        * pagingDataResponseSize = (wIndexData * sizeof (READING_LIST));
        
        //! bfnBuffer_Table_Meter(wIndexData, sizeof(MTR_LIST), &bpOutputData[0]); 
        bfnBuffer_Table_Meter(wIndexData, READING_TABLE_LIST);
        
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnDelMTRMetersTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    //! It needs to implement a better command process
    //! also the response needs to be performed

    if (bfnDelAllData(METERSTABLE, ZERO)) {

        * dataResponse_ptr = SUCCESS_CMD;
    } else {

        * dataResponse_ptr = COOR_BUSY;
    }

    * dataResponseSize = sizeof (BYTE);

    return ESP_DOES_NOT_WAIT_ANSWER;
}

BYTE bfnDelMTRDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    //! It needs to implement a better command process
    //! also the response needs to be performed

    if (bfnDelAllData(DEVICESTABLE, ZERO)) {

        * dataResponse_ptr = SUCCESS_CMD;
    } else {

        * dataResponse_ptr = COOR_BUSY;
    }

    * dataResponseSize = sizeof (BYTE);

    return ESP_DOES_NOT_WAIT_ANSWER;

}


//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void vfn_tBufferLocalDriver(void){
    
    vfnaBufferDriverState[_tBufferSM.bActualState]();
}

void ESPMeteringTable_SetStateMachine(BYTE actualState, BYTE nextState) {

    _tBufferSM.bActualState = actualState;
    _tBufferSM.bNextState = nextState;
    
    vfnEventEnable(BUFFER_EVENT);
    vfnEventPost(BUFFER_EVENT);
}

void ESPMeteringTable_ErrorProcess(void){
    
    ESPMeteringTable_ClearQuery(&espMeteringTableQuery);
    ESPMeteringTable_SetStateMachine(_BUFFER_END_STATE, _BUFFER_END_STATE);    
}

//******************************************************************************
//                  LOCAL ESP METERING TABLE STATE MACHINE
//******************************************************************************

void vfnBufferIdleState(void){
    
}

void vfnBufferConsultTableState(void){
    
    BYTE bStatusData;    
    
    WORD quantityOfItems = ESPMeteringTable_GetQuantityOfItems(&espMeteringTableQuery);
    WORD startItem = ESPMeteringTable_GetStartItem(&espMeteringTableQuery); 
    ESP_METERING_TABLE_LIST_TYPE tableListType = ESPMeteringTable_GetTableListType(&espMeteringTableQuery);
    BOOL isWaitingForQueryResponse = ESPMeteringTable_IsWaitingForQueryResponse(&espMeteringTableQuery);
    
    if(isWaitingForQueryResponse){
    
        return;
    }
    
    if(ESPMeteringTable_GetQueryResponseBufferSize(&espMeteringTableQuery)){
            
        startItem++;
        quantityOfItems--;
        
        //!print_info("Paging Data: ");
        //!print_buffer(ESPMeteringTable_GetQueryResponseBuffer(&espMeteringTableQuery), ESPMeteringTable_GetQueryResponseBufferSize(&espMeteringTableQuery));
        ESPComInterface_SendFrame(0, ESPMeteringTable_GetQueryResponseBuffer(&espMeteringTableQuery), ESPMeteringTable_GetQueryResponseBufferSize(&espMeteringTableQuery), TRUE, FALSE, quantityOfItems == 0 );
        
        ESPMeteringTable_SetupQuery(&espMeteringTableQuery,startItem,quantityOfItems,tableListType);
                
    }
    
    if( quantityOfItems == 0){
     
        print_warm("quantityOfItems = 0");
        ESPMeteringTable_ErrorProcess();
        return;
    }
    
    if( startItem >= NUM_MAX_METERS){
     
        print_error("startItem >= NUM_MAX_METERS");
        ESPMeteringTable_ErrorProcess();
        return;
    }
    
    switch(tableListType){
        
        case METER_TABLE_LIST:
            bStatusData = bfnConsultData(ALLDATAMTR, ZERO, ZERO , startItem, &espMeteringTableQuery);
            break;
        
        case DEVICE_TABLE_LIST:
            
            bStatusData = bfnConsultData(ALLDATADEV, ZERO, startItem, ZERO , &espMeteringTableQuery);
            break;
            
        case READING_TABLE_LIST:
            
            bStatusData = bfnConsultData(ALLDATAREADINGS, ZERO, ZERO, startItem , &espMeteringTableQuery);
            break;
            
        default:
            print_error("NO VALID TABLE_LIST_TYPE");
            ESPMeteringTable_ErrorProcess();
            return;
    }
    
    if(!bStatusData) {
        
        print_error("There is not a index inside DataBase");
        ESPMeteringTable_ErrorProcess();
        return;
    }   
}

void vfnBufferTransmitTableState(void){
    
}

void vfnBufferEndState(void){
    
    vfnEventClear(BUFFER_EVENT);                                        
    vfnEventDisable(BUFFER_EVENT);
}
