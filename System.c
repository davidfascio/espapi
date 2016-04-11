//******************************************************************************
//* File: System.c
//

//******************************************************************************
// Includes
//******************************************************************************
#include "System.h"
#include "SystemLog.h"

//******************************************************************************
// Varibles
//******************************************************************************
BYTE SystemStatus = RESET_START;

//******************************************************************************
// ESP_API Function 
//******************************************************************************

void System_SetStatus(BYTE status) {

    SystemStatus = status;
}

BYTE System_GetStatus(void) {

    return SystemStatus;
}

WORD System_GetFirmwareVersion(void) {

    SYSTEM_FIRMWARE_VERSION wFirmwareVersion;

    wFirmwareVersion.product = Producto_Num;
    wFirmwareVersion.function = Funcionalidad_Num;
    wFirmwareVersion.fix = Fix_Num;
    
    return wFirmwareVersion.version;
}

BYTE bfnReadStatusCoordinator(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    * dataResponse = System_GetStatus();
    * dataResponseSize = sizeof (BYTE);

    return ESP_DOES_NOT_WAIT_ANSWER;
}

BYTE bfnVersionCoordinatorLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize) {

    WORD wFirmwareVersion;
    BYTE * dataResponse_ptr = dataResponse;

    wFirmwareVersion = System_GetFirmwareVersion();

    * dataResponse_ptr = SUCCESS_CMD;
    dataResponse_ptr += sizeof (BYTE);

    memcpy(dataResponse_ptr,(BYTE *)  &wFirmwareVersion, sizeof (wFirmwareVersion));
    dataResponse_ptr += sizeof (wFirmwareVersion);

    * dataResponseSize = dataResponse_ptr - dataResponse;
    
    return ESP_DOES_NOT_WAIT_ANSWER;
}

BYTE bfnResetCoordinatorLocal(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize, WORD * pagingDataResponseSize){
    
    BYTE * dataResponse_ptr = dataResponse;
    
    * dataResponse_ptr = SUCCESS_CMD;
    * dataResponseSize = sizeof (BYTE);
    
    print_debug("SYSTEM RESET");
    
    return ESP_DOES_NOT_WAIT_ANSWER;
}
