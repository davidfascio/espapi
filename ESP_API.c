#include "ESP_API.h"
#include "ComSerialInterface.h"

ESP_API_FUNCTION_LIST espAPIFunctionList[] = 
{ 
    /*  functionCode                */
    /*  functionCallBack            */
    
    ESP_API_FUNCTION_LIST_NULL
};


void ESP_API_ResponseProcess(BYTE functionCode, BYTE status_code){
        
    ESPComInterface_SendFrame(functionCode, &status_code, sizeof(BYTE));
}

void ESP_API_ReceivedHandler(void){
    
    BYTE * buffer_ptr;
    WORD bufferSize;
    BYTE error_code;
    ESP_COM_INTERFACE_RESPONSE responseControl;
    
    //! buffer_ptr = ESPComInterface_GetBuffer();
    //! bufferSize = ESPComInterface_GetBufferSize();
    
    buffer_ptr = ComSerialInterface_GetBuffer();
    bufferSize = ComSerialInterface_GetBufferSize();
    
    if( (bufferSize == 0) || (buffer_ptr == NULL))
        return;
    
    print_log("Arrived Data: ");
    print_buffer(buffer_ptr, bufferSize);
    
    error_code = ESPComInterface_ReceivedHandler(buffer_ptr, bufferSize, &responseControl);
    
    switch(error_code ) {
        
        case ESP_COM_INTERFACE_NO_ERROR_CODE:
            
            //! TODO            
            ESPComInterfaceResponse_Print(&responseControl);
            print_log("ESPComInterface_ReceivedHandler Successfully");
            ESP_API_ResponseProcess(responseControl.invokedFunctionCode, 0x01);
            break;
            
        case ESP_COM_INTERFACE_START_OF_HEADER_ERROR_CODE:
        case ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_ERROR_CODE:
        case ESP_COM_INTERFACE_MAC_ADDRESS_ERROR_CODE:
        case ESP_COM_INTERFACE_CRC_ERROR_CODE:
        case ESP_COM_INTERFACE_END_OF_HEADER_ERROR_CODE:
            
            //! debug
            print_error("ESPComInterface_ReceivedHandler: %d", error_code);
            break;
            
        default:
            break;
    }
    
    //!ESPComInterface_CleanBuffer();
    ComSerialInterface_CleanBuffer();
}

