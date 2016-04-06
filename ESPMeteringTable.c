#include "ESPMeteringTable.h"

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

sSM _tBufferSM = {  _BUFFER_METER_IDLE_STATE,               /* bActualState     */
                    _BUFFER_METER_IDLE_STATE,               /* bNextState       */
                    _BUFFER_METER_IDLE_STATE,               /* bPrevState       */
                    _BUFFER_METER_IDLE_STATE};              /* bErrorState      */

void ESPMeteringTable_SetupQuery(ESP_METERING_TABLE_QUERY_PTR query, WORD startItem, WORD quantityOfItems, ESP_METERING_TABLE_LIST_TYPE tableListType) {

    if(query == NULL)
        return;
    
    query->startItem = startItem;
    query->quantityOfItems = quantityOfItems;
    query->tableListType = tableListType;
    
    return;
}

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

BYTE bfnReadMTRSTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize) {

    WORD wIndexData;
    WORD wDataSize;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    wIndexData = wfnIndexConsutl(METERSTABLE);

    if (wIndexData) {

        wDataSize = (wIndexData * sizeof (MTR_LIST));
        memcpy(dataResponse_ptr, (BYTE *) & wDataSize, sizeof (WORD));
        dataResponse_ptr += sizeof (WORD);

        //! bfnBuffer_Table_Meter(wIndexData, sizeof(MTR_LIST), &bpOutputData[0]); 
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnReadDevicesTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize) {

    WORD wIndexData;
    WORD wDataSize;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    wIndexData = wfnIndexConsutl(DEVICESTABLE);

    if (wIndexData) {

        wDataSize = (wIndexData * sizeof (DEV_LIST));
        memcpy(dataResponse_ptr, (BYTE *) & wDataSize, sizeof (WORD));
        dataResponse_ptr += sizeof (WORD);



        //! bfnBuffer_Table_Meter(wIndexData, sizeof(READING_LIST), &bpOutputData[0]); 
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize) {

    WORD wIndexData;
    WORD wDataSize;
    BYTE * dataResponse_ptr = dataResponse;
    BYTE answer_code = ESP_DOES_NOT_WAIT_ANSWER;

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    wIndexData = wfnIndexConsutl(METERSTABLE);

    if (wIndexData) {

        wDataSize = (wIndexData * sizeof (READING_LIST));
        memcpy(dataResponse_ptr, (BYTE *) & wDataSize, sizeof (WORD));
        dataResponse_ptr += sizeof (WORD);



        // bfnBuffer_Table_Meter(wIndexData, sizeof(READING_LIST), &bpOutputData[0]); 
        answer_code = WAIT_ANSWER;
    }

    * dataResponseSize = dataResponse_ptr - dataResponse;
    return answer_code;
}

BYTE bfnDelMTRMetersTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize) {

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
        BYTE * dataResponse, WORD * dataResponseSize) {

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

void ESPMeteringTable_SetStateMachine(BYTE actualState, BYTE nextState) {

    _tBufferSM.bActualState = actualState;
    _tBufferSM.bNextState = nextState;
    
    vfnEventEnable(BUFFER_EVENT);
    vfnEventPost(BUFFER_EVENT);
}

void vfnBufferIdleState(void){
    
}

void vfnBufferConsultTableState(void){
    
}

void vfnBufferTransmitTableState(void){
    
}

void vfnBufferEndState(void){
    
}