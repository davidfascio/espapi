#include "ESP_API.h"


ESP_API_FUNCTION_LIST espAPIFunctionList[] = {
    
    //***********************************************************************************************************
    //* ESP API SYSTEM CALLBACK FUNCTIONS
    //***********************************************************************************************************
    
    {   READMODE_STATUS_LOCAL,                  /*  functionCode                */
        bfnReadStatusCoordinator },             /*  functionCallBack            */
        
    {   READMODE_VERSION_COOR_LOCAL,            /*  functionCode                */
        bfnVersionCoordinatorLocal },           /*  functionCallBack            */
    
    {   RESET_COOR_LCL,                         /*  functionCode                */
        bfnResetCoordinatorLocal },             /*  functionCallBack            */
    
        
    //***********************************************************************************************************
    //* ESP API SYSTEM CALLBACK FUNCTIONS
    //***********************************************************************************************************
    
    {   READMODE_TIME_DATE_LOCAL,               /*  functionCode                */
        bfnReadTimeDateLocal },                 /*  functionCallBack            */
    
    {   WRITE_MODE_UPDATE_TIME_DATE,               /*  functionCode                */
        bfnWriteTimeDateLocal },                 /*  functionCallBack            */
    //***********************************************************************************************************
    //* ESP API ZDO CONTROL CALLBACK FUNCTIONS
    //***********************************************************************************************************
    {   READMODE_CHANNEL_COOR_LOCAL,            /*  functionCode                */
        bfnReadChannelLocal },                  /*  functionCallBack            */
        
    {   READMODE_PAN_ID_COOR_LOCAL,             /*  functionCode                */
        bfnReadPANIDLocal},                     /*  functionCallBack            */
        
    {   WRITE_MODE_UPDATE_PAN_ID,               /*  functionCode                */
        bfnWritePANIDLocal},                    /*  functionCallBack            */
        
    //***********************************************************************************************************
    //* ESP API METERING TABLE CALLBACK FUNCTIONS
    //***********************************************************************************************************
    
    {   BUFFERMODE_READ_DEVICES_ZBEE_BUFFER,
        bfnReadDevicesTable},
        
    {   BUFFERMODE_READ_ALL_MTR_READINGS_BUFFER,/*  functionCode                */
        bfnReadMTRReadingsTable},               /*  functionCallBack            */
    
    
        
    ESP_API_FUNCTION_LIST_NULL
};

ESP_API_FUNCTION_LIST_PTR ESP_API_GetESPAPIFunctionList(BYTE functionCode) {

    ESP_API_FUNCTION_LIST_PTR espAPIFunctionList_ptr = espAPIFunctionList;

    while (espAPIFunctionList_ptr->functionCode != ESP_API_NO_INVOKED_FUNCTION_CODE) {

        if (espAPIFunctionList_ptr->functionCode == functionCode)
            break;

        espAPIFunctionList_ptr++;
    }

    if (espAPIFunctionList_ptr->functionCode == ESP_API_NO_INVOKED_FUNCTION_CODE)
        return NULL;

    return espAPIFunctionList_ptr;
}

BYTE ESP_API_ResponseProcess(ESP_COM_INTERFACE_REQUEST_PTR requestControl) {
    
    ESP_API_FUNCTION_LIST_PTR espAPIFunctionList_ptr;
    ESP_COM_INTERFACE_RESPONSE response;
    BYTE error_code;
    
    if(requestControl == NULL)
        return ESP_API_REQUEST_NULL_ERROR_CODE;
    
    espAPIFunctionList_ptr = ESP_API_GetESPAPIFunctionList(requestControl->invokedFunctionCode);

    if(espAPIFunctionList_ptr == NULL)
        return ESP_API_FUNCTION_LIST_IS_NOT_FOUND_ERROR_CODE;
    
    response.invokedFunctionCode = requestControl->invokedFunctionCode;
    error_code = espAPIFunctionList_ptr->functionCallBack(requestControl->data, requestControl->dataSize, response.data, &response.dataSize );
    
    switch(error_code){
    
        case ESP_DOES_NOT_WAIT_ANSWER:
            ESPComInterface_SendFrame(response.invokedFunctionCode, response.data, response.dataSize);
            break;
            
        case WAIT_ANSWER:
            // IMPORTANT Implement Paging
            break;
            
        /*case NO_SEND_ANSWER:
            print_error("DONT_SEND_ANSWER");
            break;
            
        case ESP_DATA_SIZE_ERROR_CODE:    
            print_error("ESP_DATA_SIZE_ERROR_CODE");
            break;*/
        
        default:
            break;
        
    }
    
    return error_code;
}

void ESP_API_ReceivedHandler(void) {

    BYTE * buffer_ptr;
    WORD bufferSize;
    BYTE error_code;
    ESP_COM_INTERFACE_REQUEST requestControl;

    //! buffer_ptr = ESPComInterface_GetBuffer();
    //! bufferSize = ESPComInterface_GetBufferSize();

    buffer_ptr = ComSerialInterface_GetBuffer();
    bufferSize = ComSerialInterface_GetBufferSize();

    if ((bufferSize == 0) || (buffer_ptr == NULL))
        return;

    print_log("Arrived Data: ");
    print_buffer(buffer_ptr, bufferSize);

    error_code = ESPComInterface_ReceivedHandler(buffer_ptr, bufferSize, &requestControl);

    switch (error_code) {

        case ESP_COM_INTERFACE_NO_ERROR_CODE:

            //! TODO            
            ESPComInterfaceRequest_Print(&requestControl);
            
            error_code = ESP_API_ResponseProcess(&requestControl);
            
            if (error_code != ESP_API_NO_ERROR_CODE){
                
                print_error("ESP_API_ResponseProcess Error Code: %04d", error_code);                
            } else {
                
                print_log("ESP_API_ResponseProcess Successfully");
            }
            
            break;

        case ESP_COM_INTERFACE_START_OF_HEADER_ERROR_CODE:
        case ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_ERROR_CODE:
        case ESP_COM_INTERFACE_MAC_ADDRESS_ERROR_CODE:
        case ESP_COM_INTERFACE_CRC_ERROR_CODE:
        case ESP_COM_INTERFACE_END_OF_HEADER_ERROR_CODE:

            //! debug
            print_error("ESPComInterface_ReceivedHandler: %04d", error_code);
            break;

        default:
            break;
    }

    //!ESPComInterface_CleanBuffer();
    ComSerialInterface_CleanBuffer();
}

