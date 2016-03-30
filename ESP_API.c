#include "ESP_API.h"

void ESP_API_ReceivedHandler(void){
    
    BYTE * buffer_ptr;
    WORD bufferSize;
    
    //! buffer_ptr = ESPComInterface_GetBuffer();
    //! bufferSize = ESPComInterface_GetBufferSize();
    
    if( (bufferSize == 0) || (buffer_ptr == NULL))
        return;
    
    print_log("Arrived Data: ");
}
