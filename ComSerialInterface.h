/* 
 * File:   ComSerialInterface.h
 * Author: fascio
 *
 * Created on 4 de enero de 2016, 12:35
 */

#ifndef __COM_SERIAL_INTERFACE_H__
#define	__COM_SERIAL_INTERFACE_H__

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "SystemLog.h"


#define COM_SERIAL_INTERFACE_HANDLER_PORT                       "\\\\.\\COM62" //"\\\\.\\COM30"
#define COM_SERIAL_INTERFACE_HANDLER_9600_BAUDRATE                (9600) 
#define COM_SERIAL_INTERFACE_HANDLER_BYTE_7_SIZE                  (7) 
#define COM_SERIAL_INTERFACE_HANDLER_ONE_STOP_BIT                (ONESTOPBIT)
#define COM_SERIAL_INTERFACE_HANDLER_EVEN_PARITY                  (EVENPARITY) 

#define COM_SERIAL_INTERFACE_HANDLER_19200_BAUDRATE               (19200) 
#define COM_SERIAL_INTERFACE_HANDLER_BYTE_8_SIZE                  (8)
#define COM_SERIAL_INTERFACE_HANDLER_NO_PARITY                    (NOPARITY)

#define COM_SERIAL_INTERFACE_HANDLER_115200_BAUDRATE               (115200) 


HANDLE ComSerialInterface_OpenHandler(  LPCSTR  portName, 
                                        DWORD   baudRate,
                                        DWORD   byteSize,
                                        DWORD   stopBits,
                                        BYTE    parity  );

void ComSerialInterface_CloseHandler( HANDLE hSerial);
DWORD ComSerialInterface_ReadPort( HANDLE hSerial, BYTE * buffer, DWORD buffersize);
DWORD ComSerialInterface_WritePort( HANDLE hSerial, BYTE * buffer, DWORD buffersize);
void ComSerialInterface_CleanBuffer(void);

void ComSerialInterface_Init(void);
VOID ComSerialInterface_Setup(DWORD   baudRate, DWORD   byteSize, DWORD   stopBits, BYTE    parity  );
DWORD ComSerialInterface_WriteData(BYTE * data, DWORD dataLen);
DWORD ComSerialInterface_ReadData(BYTE * data, DWORD dataLen);
void ComSerialInterface_SendData(BYTE* frame, BYTE frameLen);
void ComSerialInterface_Check(void);

void ComSerialInterface_PrintData(BYTE * frame, WORD frameLen);
BYTE * ComSerialInterface_GetBuffer( void);    
WORD ComSerialInterface_GetBufferSize(void);

#endif	/* __COM_SERIAL_INTERFACE_H__ */

