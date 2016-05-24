//******************************************************************************
//* File ESPComInterface.c
//


//******************************************************************************
// Includes
//******************************************************************************
#include "ESPComInterface.h"

//******************************************************************************
// Variables
//******************************************************************************
WORD ESPComInterfaceLastCRCFrame = 0xFFFF;

//******************************************************************************
// ESP_COM_INTERFACE Function
//******************************************************************************
void ESPComInterface_SetLastCRCFrame(WORD crcFrame){
    
    ESPComInterfaceLastCRCFrame = crcFrame;
}

WORD ESPComInterface_GetLastCRCFrame(void){
    
    return ESPComInterfaceLastCRCFrame;
}

void ESPComInterfaceRequest_Setup(ESP_COM_INTERFACE_REQUEST_PTR requestControl, BYTE invokedFunctionCode, BYTE * data, WORD dataSize){
    
    ESPComInterfaceRequest_Clear(requestControl);
    ESPComInterfaceRequest_SetInvokedFunctionCode(requestControl, invokedFunctionCode);
    ESPComInterfaceRequest_SetData(requestControl, data, dataSize);    
}

void ESPComInterfaceRequest_Clear(ESP_COM_INTERFACE_REQUEST_PTR requestControl){
    
    if(requestControl == NULL)
        return;
    
    memset(requestControl, 0, sizeof(ESP_COM_INTERFACE_REQUEST));
}

void ESPComInterfaceResponse_Clear(ESP_COM_INTERFACE_RESPONSE_PTR responseControl){
    
    if(responseControl == NULL)
        return;
    
    memset(responseControl, 0, sizeof(ESP_COM_INTERFACE_RESPONSE));
}

void ESPComInterfaceRequest_SetInvokedFunctionCode(ESP_COM_INTERFACE_REQUEST_PTR requestControl, BYTE invokedFunctionCode){

    if(requestControl == NULL)
        return;
    
    requestControl->invokedFunctionCode = invokedFunctionCode;
}

void ESPComInterfaceRequest_SetData(ESP_COM_INTERFACE_REQUEST_PTR requestControl, BYTE * data, WORD dataSize){
    
    if(requestControl == NULL)
        return;
    
    if(dataSize > ESP_COM_INTERFACE_RECEIVED_DATA_MAX_SIZE)
        return;
    
    memset(requestControl->data, 0, ESP_COM_INTERFACE_RECEIVED_DATA_MAX_SIZE);
    memcpy(requestControl->data, data, dataSize);
    requestControl->dataSize = dataSize; 
}

BYTE ESPComInterfaceRequest_GetInvokedFunctionCode(ESP_COM_INTERFACE_REQUEST_PTR requestControl){

    return requestControl->invokedFunctionCode;
}

WORD ESPComInterfaceRequest_GetData(ESP_COM_INTERFACE_REQUEST_PTR requestControl, BYTE * data, WORD  dataSize){
    
    if(requestControl->dataSize > dataSize)
        return 0;
    
    memset(data, 0, dataSize);
    memcpy(data, requestControl->data, requestControl->dataSize);
    
    return requestControl->dataSize;
}


void ESPComInterfaceRequest_Print(ESP_COM_INTERFACE_REQUEST_PTR requestControl){
    
    print_log("InvokedFunctionCode %02X", requestControl->invokedFunctionCode);
    print_log("ReceivedData (%d Bytes): ", requestControl->dataSize);
    print_buffer(requestControl->data, requestControl->dataSize);    
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
        BYTE * data, WORD dataSize, WORD endOfHeader, BOOL isPagingFrame, BOOL isFirstPagingFrame, 
        BOOL isLastPagingFrame, WORD pagingDataSize, BYTE * frame, WORD * frameLen) {

    BYTE * frame_ptr = frame;
    WORD frameSize;
    WORD crcFrame;
    WORD lastCRCFrame = ESPComInterface_GetLastCRCFrame();

    if((!isPagingFrame)|| (isPagingFrame && isFirstPagingFrame)){
        
        inverted_memcpy(frame_ptr, (BYTE *) & startOfHeader, ESP_COM_INTERFACE_START_OF_HEADER_SIZE);
        frame_ptr += ESP_COM_INTERFACE_START_OF_HEADER_SIZE;

        frameSize = ESP_COM_INTERFACE_MAC_ADDRESS_SIZE + ESP_COM_INTERFACE_FUNCTION_CODE_HEADER_SIZE + dataSize + pagingDataSize;

        memcpy(frame_ptr, (BYTE *) & frameSize, ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE);
        frame_ptr += ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE;

        memcpy(frame_ptr, macAddress, ESP_COM_INTERFACE_MAC_ADDRESS_SIZE);
        frame_ptr += ESP_COM_INTERFACE_MAC_ADDRESS_SIZE;

        * frame_ptr = functionCode;
        frame_ptr += ESP_COM_INTERFACE_FUNCTION_CODE_HEADER_SIZE;
    }
    
    memcpy(frame_ptr, data, dataSize);
    frame_ptr += dataSize;

    if ((!isPagingFrame)|| (isPagingFrame && isFirstPagingFrame)){
     
        crcFrame = wfnCRC_CALC(frame + ESP_COM_INTERFACE_START_OF_HEADER_SIZE + ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE,
        frame_ptr - (frame + ESP_COM_INTERFACE_START_OF_HEADER_SIZE + ESP_COM_INTERFACE_FRAME_SIZE_OF_HEADER_SIZE), 0xFFFF);
        
    } else{
        
        crcFrame = wfnCRC_CALC(data, dataSize, lastCRCFrame);
    }
    
    ESPComInterface_SetLastCRCFrame(crcFrame);
    
    if((!isPagingFrame) || (isPagingFrame && isLastPagingFrame)){
        
        memcpy(frame_ptr, (BYTE *) & crcFrame, ESP_COM_INTERFACE_CRC_HEADER_SIZE);
        frame_ptr += ESP_COM_INTERFACE_CRC_HEADER_SIZE;

        inverted_memcpy(frame_ptr, (BYTE *) & endOfHeader, ESP_COM_INTERFACE_END_OF_HEADER_SIZE);
        frame_ptr += ESP_COM_INTERFACE_END_OF_HEADER_SIZE;
    }

    *frameLen = (frame_ptr - frame);

    return TRUE;
}

void ESPComInterface_SendCustomFrame(WORD startOfHeader, BYTE * macAddress, BYTE functionCode,
        BYTE * data, WORD dataSize, WORD endOfHeader, 
        BOOL isPagingFrame, BOOL isFirstPagingFrame, BOOL isLastPagingFrame, WORD pagingDataSize) {

    BYTE frame[ESP_COM_INTERFACE_FRAME_MAX_SIZE];
    WORD frameLen;
    BOOL frameBuilt;

    //! frameLen Validation

    frameBuilt = ESPComInterface_BuildFrame(startOfHeader, macAddress, functionCode,
            data, dataSize, endOfHeader, isPagingFrame, isFirstPagingFrame, isLastPagingFrame, pagingDataSize, frame, &frameLen);

    if (frameBuilt == FALSE) {

        print_error("It could not build ESPCom frame");
        return;
    }

    //! ESPComInterface_WriteData(frame, frameLen);      
    print_info("Sending Data (%d Bytes): ", frameLen);
    print_buffer(frame, frameLen);
    ComSerialInterface_WriteData(frame, frameLen);
}

void ESPComInterface_SendFrame(BYTE functionCode, BYTE * data, WORD dataSize, BOOL isPagingFrame, BOOL isFirstPagingFrame, BOOL isLastPagingFrame, WORD pagingDataSize) {

    ESPComInterface_SendCustomFrame(ESP_COM_INTERFACE_START_OF_HEADER, macLongAddrByte, functionCode, data, dataSize, ESP_COM_INTERFACE_END_OF_HEADER,
            isPagingFrame, isFirstPagingFrame, isLastPagingFrame, pagingDataSize);
}

BYTE ESPComInterface_ReceivedHandler(BYTE * buffer, WORD bufferSize, ESP_COM_INTERFACE_REQUEST_PTR requestControl) {

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


    ESPComInterfaceRequest_Setup(requestControl, functionCode, receivedData, receivedDataSize);
    
    return ESP_COM_INTERFACE_NO_ERROR_CODE;
}
