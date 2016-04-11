#include "ComSerialInterface.h"
#include "EventsEngine.h"


HANDLE ComSerialInterface_OpenHandler(  LPCSTR  portName, 
                                        DWORD   baudRate,
                                        DWORD   byteSize,
                                        DWORD   stopBits,
                                        BYTE    parity  )
{
    LPCSTR portname = portName;
    DWORD accessdirection = GENERIC_READ | GENERIC_WRITE;
    DCB dcbSerialParams= {0};
    
    COMMTIMEOUTS timeouts = {0};
    
    /*
     * WINBASEAPI HANDLE WINAPI CreateFileA (   LPCSTR lpFileName, 
     *                                          DWORD dwDesiredAccess, 
     *                                          DWORD dwShareMode, 
     *                                          LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
     *                                          DWORD dwCreationDisposition, 
     *                                          DWORD dwFlagsAndAttributes, 
     *                                          HANDLE hTemplateFile);
     */
    
    HANDLE hSerial = CreateFile(   portname,
                                    accessdirection,
                                    0,
                                    0,
                                    OPEN_EXISTING,
                                    0,
                                    0   );
    
    if(hSerial == INVALID_HANDLE_VALUE){
        
        return NULL; //-1;
    }
    
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    
    if(!GetCommState(hSerial, &dcbSerialParams)){
        
        return NULL; //-2;
    }
    
    dcbSerialParams.BaudRate = baudRate;    // 19200;
    dcbSerialParams.ByteSize = byteSize;    // 8;
    dcbSerialParams.StopBits = stopBits;    // ONESTOPBIT;
    dcbSerialParams.Parity   = parity;      // EVENPARITY;
    
    if(!SetCommState(hSerial, &dcbSerialParams)){
        
        return NULL;    // -3;
    }
    
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
    if(!SetCommTimeouts(hSerial, &timeouts)){
        
        return NULL; // -4;
        
    }

    return hSerial;
}

void ComSerialInterface_CloseHandler( HANDLE hSerial){
    
    CloseHandle(hSerial);
}

DWORD ComSerialInterface_ReadPort( HANDLE hSerial, BYTE * buffer, DWORD buffersize){
    
    DWORD dwBytesRead = 0;
    
    if(!ReadFile(hSerial, buffer, buffersize, &dwBytesRead, NULL)){
        
        return 0;
    }
    
    return dwBytesRead;
}

DWORD ComSerialInterface_WritePort( HANDLE hSerial, BYTE * buffer, DWORD buffersize){
    
    DWORD dwBytesRead = 0;
    
    if(!WriteFile(hSerial, buffer, buffersize, &dwBytesRead, NULL)){
        
        return 0;
    }
    
    return dwBytesRead;
}


HANDLE ComSerialInterfaceHandler = NULL;


void ComSerialInterface_PrintData(BYTE * frame, WORD frameLen){
    
    BYTE index;
    BYTE * frame_ptr = frame;
    
    
    for(index = 0; index < frameLen; index++)
        print_message("%02X ", *frame_ptr++);
    
    
}
DWORD ComSerialInterface_WriteData(BYTE * data, DWORD dataLen){
    
    if(ComSerialInterfaceHandler == NULL){
        
        print_error("Serial Port is closed");
        return 0;
    }
    
    return ComSerialInterface_WritePort(ComSerialInterfaceHandler, data, dataLen);
}

DWORD ComSerialInterface_ReadData(BYTE * data, DWORD dataLen){
    
    if(ComSerialInterfaceHandler == NULL){
    
        //printf("Serial Port is closed\n");
        return 0;
    }
    
    return ComSerialInterface_ReadPort(ComSerialInterfaceHandler, data, dataLen);
}


#define COM_SERIAL_INTERFACE_BUFFER_SIZE                                    (500)
#define COM_SERIAL_INTERFACE_BUFFER_WINDOW_SIZE                              (1)

BYTE ComSerialInterfaceBuffer[COM_SERIAL_INTERFACE_BUFFER_SIZE];
BYTE ComSerialInterfaceBufferUsed = 0;

void ComSerialInterface_Init(void){
    
    ComSerialInterface_Setup( COM_SERIAL_INTERFACE_HANDLER_115200_BAUDRATE, 
                              COM_SERIAL_INTERFACE_HANDLER_BYTE_8_SIZE, 
                              COM_SERIAL_INTERFACE_HANDLER_ONE_STOP_BIT, 
                              COM_SERIAL_INTERFACE_HANDLER_NO_PARITY);
    


    
    //ComSerialInterface_CleanBuffer();
    //memset(ComSerialInterfaceBuffer, 0 , sizeof(ComSerialInterfaceBuffer));
}

VOID ComSerialInterface_Setup(DWORD   baudRate, DWORD   byteSize, DWORD   stopBits, BYTE    parity  ){
    ComSerialInterface_CloseHandler(ComSerialInterfaceHandler);
    ComSerialInterfaceHandler = ComSerialInterface_OpenHandler( COM_SERIAL_INTERFACE_HANDLER_PORT,
                                                                baudRate,
                                                                byteSize,
                                                                stopBits,
                                                                parity);
    
    ComSerialInterface_CleanBuffer();    
}

void ComSerialInterface_CleanBuffer(void){
    
    ComSerialInterfaceBufferUsed = 0;
    memset(ComSerialInterfaceBuffer, 0 , sizeof(ComSerialInterfaceBuffer));
}

void ComSerialInterface_SendData(BYTE* frame, BYTE frameLen){ 
    
    print_info("Sent Data: ");
    ComSerialInterface_PrintData(frame, frameLen);
    ComSerialInterface_WriteData(frame, frameLen);
}

void ComSerialInterface_Check(void){
    
    BYTE * ComSerialInterfaceBuffer_ptr = ComSerialInterfaceBuffer + ComSerialInterfaceBufferUsed;
    BYTE arrivedBytes;
    
    arrivedBytes = ComSerialInterface_ReadData(ComSerialInterfaceBuffer_ptr, COM_SERIAL_INTERFACE_BUFFER_WINDOW_SIZE);
        
    if(arrivedBytes){
        
        ComSerialInterfaceBufferUsed += arrivedBytes;
        vfnOneShotReload(WAIT_FRAME_UART_ONESHOT, _1_MSEC_);
    }
}

BYTE * ComSerialInterface_GetBuffer( void){
    
    return ComSerialInterfaceBuffer;
    
}

WORD ComSerialInterface_GetBufferSize(void){
    
    return ComSerialInterfaceBufferUsed;
}

void ComSerialInterface_FillBuffer(BYTE * buffer, WORD bufferSize){
    
    memcpy(ComSerialInterfaceBuffer, buffer, bufferSize);
    ComSerialInterfaceBufferUsed = bufferSize;
            vfnOneShotReload(WAIT_FRAME_UART_ONESHOT, _1_MSEC_);
}