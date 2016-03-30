//******************************************************************************
// Includes
//******************************************************************************

#include "ESPComInterface.h"

//******************************************************************************
// ESP_COM_INTERFACE Function
//******************************************************************************

BYTE ESPComInterface_ReceivedHandler(BYTE * buffer, WORD bufferSize){
    
    BYTE * buffer_ptr = buffer;    
    WORD startOfHeader;
    WORD frameSize;
    BYTE macAddress[ESP_COM_INTERFACE_MAC_ADDRESS_SIZE];   
    BYTE functionCode;
    BYTE receivedData[ESP_COM_INTERFACE_RECEIVED_DATA_MAX_SIZE];
    WORD receivedDataSize;
    WORD crcValue;
    WORD endOfHeader;
    
    
    //**************************************************************************
    // Start Of Head Validation
    
    inverted_memcpy((BYTE *) &startOfHeader, buffer_ptr, ESP_COM_INTERFACE_START_OF_HEADER_SIZE);
    
    if(startOfHeader != ESP_COM_INTERFACE_START_OF_HEADER)
        return ESP_COM_INTERFACE_START_OF_HEADER_ERROR_CODE;
    
    buffer_ptr += ESP_COM_INTERFACE_START_OF_HEADER_SIZE;
    
    
    //**************************************************************************
    // Frame Size Validation
    
    memcpy((BYTE *) &frameSize, buffer_ptr, ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE);
    
    if(frameSize > (bufferSize - (buffer_ptr - buffer)))
        return ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_ERROR_CODE;
    
    buffer_ptr += ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE;
    
    
    //**************************************************************************
    // CRC Validation
    
    crcValue = wfnCRC16(buffer_ptr, frameSize );
    
    if(memcmp((BYTE *) &crcValue, buffer_ptr + frameSize, ESP_COM_INTERFACE_CRC_HEADER_SIZE))
        return  ESP_COM_INTERFACE_CRC_ERROR_CODE;
    
    
    //**************************************************************************
    // MAC Address Validation
    
    inverted_memcpy(macAddress, buffer_ptr, ESP_COM_INTERFACE_MAC_ADDRESS_SIZE);
    
    if(memcmp(macLongAddrByteInverse, macAddress, ESP_COM_INTERFACE_MAC_ADDRESS_SIZE))
        return ESP_COM_INTERFACE_MAC_ADDRESS_ERROR_CODE;
    
    buffer_ptr += ESP_COM_INTERFACE_MAC_ADDRESS_SIZE;
    
   
    //**************************************************************************
    // Function Code Validation
    
    functionCode = * buffer_ptr;
    buffer_ptr += ESP_COM_INTERFACE_FUNCTION_CODE_HEADER_SIZE;
    
    
    //**************************************************************************
    // Received Data Validation
    
    receivedDataSize = frameSize - ESP_COM_INTERFACE_MAC_ADDRESS_SIZE - ESP_COM_INTERFACE_FUNCTION_CODE_HEADER_SIZE;
    
    if(receivedDataSize != 0){
        
        memcpy(receivedData, buffer_ptr, receivedDataSize);
        buffer_ptr += receivedDataSize;
    }
    
    
    //**************************************************************************
    //  End of Header Validation
    
    buffer_ptr += ESP_COM_INTERFACE_CRC_HEADER_SIZE;    
    
    inverted_memcpy((BYTE *) &endOfHeader, buffer_ptr, ESP_COM_INTERFACE_END_OF_HEADER_SIZE);    
    
    if(endOfHeader != ESP_COM_INTERFACE_END_OF_HEADER)
        return ESP_COM_INTERFACE_END_OF_HEADER_ERROR_CODE;
    
    
    return ESP_COM_INTERFACE_NO_ERROR_CODE;
}