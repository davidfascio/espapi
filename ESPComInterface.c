//******************************************************************************
// Includes
//******************************************************************************
#include "ESPComInterface.h"


//******************************************************************************
// Variables
//******************************************************************************
INPUT_BUFFER_T ESPComInterfaceBuffer;

//******************************************************************************
// ESP_COM_INTERFACE Function
//******************************************************************************

void ESPComInterfaceResponse_Setup(ESP_COM_INTERFACE_RESPONSE_PTR responseControl, BYTE invokedFunctionCode, BYTE * data, WORD dataSize){
    
    ESPComInterfaceResponse_Clear(responseControl);
    ESPComInterfaceResponse_SetInvokedFunctionCode(responseControl, invokedFunctionCode);
    ESPComInterfaceResponse_SetData(responseControl, data, dataSize);    
}

void ESPComInterfaceResponse_Clear(ESP_COM_INTERFACE_RESPONSE_PTR responseControl){
    
    if(responseControl == NULL)
        return;
    
    memset(responseControl, 0, sizeof(ESP_COM_INTERFACE_RESPONSE));
}

void ESPComInterfaceResponse_SetInvokedFunctionCode(ESP_COM_INTERFACE_RESPONSE_PTR responseControl, BYTE invokedFunctionCode){

    if(responseControl == NULL)
        return;
    
    responseControl->invokedFunctionCode = invokedFunctionCode;
}

void ESPComInterfaceResponse_SetData(ESP_COM_INTERFACE_RESPONSE_PTR responseControl, BYTE * data, WORD dataSize){
    
    if(responseControl == NULL)
        return;
    
    if(dataSize > ESP_COM_INTERFACE_RECEIVED_DATA_MAX_SIZE)
        return;
    
    memset(responseControl->data, 0, ESP_COM_INTERFACE_RECEIVED_DATA_MAX_SIZE);
    memcpy(responseControl->data, data, dataSize);
    responseControl->dataSize = dataSize; 
}

BYTE ESPComInterfaceResponse_GetInvokedFunctionCode(ESP_COM_INTERFACE_RESPONSE_PTR responseControl){

    return responseControl->invokedFunctionCode;
}

WORD ESPComInterfaceResponse_GetData(ESP_COM_INTERFACE_RESPONSE_PTR responseControl, BYTE * data, WORD  dataSize){
    
    if(responseControl->dataSize > dataSize)
        return 0;
    
    memset(data, 0, dataSize);
    memcpy(data, responseControl->data, responseControl->dataSize);
    
    return responseControl->dataSize;
}


void ESPComInterfaceResponse_Print(ESP_COM_INTERFACE_RESPONSE_PTR responseControl){
    
    print_log("InvokedFunctionCode %02X", responseControl->invokedFunctionCode);
    print_log("ReceivedData: ");
    print_buffer(responseControl->data, responseControl->dataSize);    
}

void ESPComInterface_Setup(BYTE one_shot_event, BYTE time_delay) {

    //! vfnUART1Init()
}

//******************************************************************************
//                      E S P  C O M  P R O T O C O L
//******************************************************************************
//    WORD startOfHeader;
//    WORD frameSize;
//    BYTE macAddress[ESP_COM_INTERFACE_MAC_ADDRESS_SIZE];   
//    BYTE functionCode;
//    BYTE dataReceived[ESP_COM_INTERFACE_DATA_RECEIVED_MAX_SIZE];
//    WORD dataReceivedSize;
//    WORD crcValue;
//    WORD endOfHeader;

BOOL ESPComInterface_BuildFrame(WORD startOfHeader, BYTE * macAddress, BYTE functionCode,
        BYTE * data, WORD dataSize, WORD endOfHeader, BYTE * frame, WORD * frameLen) {

    BYTE * frame_ptr = frame;
    WORD frameSize;
    WORD crcFrame;

    inverted_memcpy(frame_ptr, (BYTE *) & startOfHeader, ESP_COM_INTERFACE_START_OF_HEADER_SIZE);
    frame_ptr += ESP_COM_INTERFACE_START_OF_HEADER_SIZE;

    frameSize = ESP_COM_INTERFACE_MAC_ADDRESS_SIZE + ESP_COM_INTERFACE_FUNCTION_CODE_HEADER_SIZE + dataSize;

    memcpy(frame_ptr, (BYTE *) & frameSize, ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE);
    frame_ptr += ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE;

    memcpy(frame_ptr, macAddress, ESP_COM_INTERFACE_MAC_ADDRESS_SIZE);
    frame_ptr += ESP_COM_INTERFACE_MAC_ADDRESS_SIZE;

    * frame_ptr = functionCode;
    frame_ptr += ESP_COM_INTERFACE_FUNCTION_CODE_HEADER_SIZE;

    memcpy(frame_ptr, data, dataSize);
    frame_ptr += dataSize;

    crcFrame = wfnCRC16(frame + ESP_COM_INTERFACE_START_OF_HEADER_SIZE + ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE,
            frame_ptr - (frame + ESP_COM_INTERFACE_START_OF_HEADER_SIZE + ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE));

    memcpy(frame_ptr, (BYTE *) & crcFrame, ESP_COM_INTERFACE_CRC_HEADER_SIZE);
    frame_ptr += ESP_COM_INTERFACE_CRC_HEADER_SIZE;

    inverted_memcpy(frame_ptr, (BYTE *) & endOfHeader, ESP_COM_INTERFACE_END_OF_HEADER_SIZE);
    frame_ptr += ESP_COM_INTERFACE_END_OF_HEADER_SIZE;

    *frameLen = (frame_ptr - frame);

    return TRUE;
}

void ESPComInterface_SendCustomFrame(WORD startOfHeader, BYTE * macAddress, BYTE functionCode,
        BYTE * data, WORD dataSize, WORD endOfHeader) {

    BYTE frame[ESP_COM_INTERFACE_FRAME_MAX_SIZE];
    WORD frameLen;
    BOOL frameBuilt;

    //! frameLen Validation

    frameBuilt = ESPComInterface_BuildFrame(startOfHeader, macAddress, functionCode,
            data, dataSize, endOfHeader, frame, &frameLen);

    if (frameBuilt == FALSE) {

        print_error("It could not build ESPCom frame");
        return;
    }

    //! ESPComInterface_WriteData(frame, frameLen);      
    print_info("Sending Data: ");
    print_buffer(frame, frameLen);
    ComSerialInterface_WriteData(frame, frameLen);
}

void ESPComInterface_SendFrame(BYTE functionCode, BYTE * data, WORD dataSize) {

    ESPComInterface_SendCustomFrame(ESP_COM_INTERFACE_START_OF_HEADER, macLongAddrByte, functionCode, data, dataSize, ESP_COM_INTERFACE_END_OF_HEADER);
}

BYTE ESPComInterface_ReceivedHandler(BYTE * buffer, WORD bufferSize, ESP_COM_INTERFACE_RESPONSE_PTR responseControl) {

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

    inverted_memcpy((BYTE *) & startOfHeader, buffer_ptr, ESP_COM_INTERFACE_START_OF_HEADER_SIZE);

    if (startOfHeader != ESP_COM_INTERFACE_START_OF_HEADER)
        return ESP_COM_INTERFACE_START_OF_HEADER_ERROR_CODE;

    buffer_ptr += ESP_COM_INTERFACE_START_OF_HEADER_SIZE;


    //**************************************************************************
    // Frame Size Validation

    memcpy((BYTE *) & frameSize, buffer_ptr, ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE);

    if (frameSize > (bufferSize - (buffer_ptr - buffer)))
        return ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_ERROR_CODE;

    buffer_ptr += ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE;


    //**************************************************************************
    // CRC Validation

    crcValue = wfnCRC16(buffer_ptr, frameSize);

    if (memcmp((BYTE *) & crcValue, buffer_ptr + frameSize, ESP_COM_INTERFACE_CRC_HEADER_SIZE))
        return ESP_COM_INTERFACE_CRC_ERROR_CODE;


    //**************************************************************************
    // MAC Address Validation

    inverted_memcpy(macAddress, buffer_ptr, ESP_COM_INTERFACE_MAC_ADDRESS_SIZE);

    if (memcmp(macLongAddrByteInverse, macAddress, ESP_COM_INTERFACE_MAC_ADDRESS_SIZE))
        return ESP_COM_INTERFACE_MAC_ADDRESS_ERROR_CODE;

    buffer_ptr += ESP_COM_INTERFACE_MAC_ADDRESS_SIZE;


    //**************************************************************************
    // Function Code Validation

    functionCode = *buffer_ptr;
    buffer_ptr += ESP_COM_INTERFACE_FUNCTION_CODE_HEADER_SIZE;


    //**************************************************************************
    // Received Data Validation

    receivedDataSize = frameSize - ESP_COM_INTERFACE_MAC_ADDRESS_SIZE - ESP_COM_INTERFACE_FUNCTION_CODE_HEADER_SIZE;

    if (receivedDataSize > ESP_COM_INTERFACE_RECEIVED_DATA_MAX_SIZE) 
        return ESP_COM_INTERFACE_RECEIVED_DATA_ERROR_CODE;
    
    if (receivedDataSize != 0) {

        memcpy(receivedData, buffer_ptr, receivedDataSize);
        buffer_ptr += receivedDataSize;
    }


    //**************************************************************************
    //  End of Header Validation

    buffer_ptr += ESP_COM_INTERFACE_CRC_HEADER_SIZE;

    inverted_memcpy((BYTE *) & endOfHeader, buffer_ptr, ESP_COM_INTERFACE_END_OF_HEADER_SIZE);

    if (endOfHeader != ESP_COM_INTERFACE_END_OF_HEADER)
        return ESP_COM_INTERFACE_END_OF_HEADER_ERROR_CODE;


    ESPComInterfaceResponse_Setup(responseControl, functionCode, receivedData, receivedDataSize);
    
    return ESP_COM_INTERFACE_NO_ERROR_CODE;
}