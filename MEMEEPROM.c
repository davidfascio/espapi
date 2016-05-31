#include "MEMEEPROM.h"

//**********************************************************************
//* MEM_EEPROM Variables
//**********************************************************************
/*
I2C_COM_INTERFACE_CONTROL eepromComInterfaceControl;
*/
static BYTE eepromMemory[0xFFFF];

//**********************************************************************
//* MEM_EEPROM Functions
//**********************************************************************
void MEM_EEPROM_Init(void) {

/*
    I2CComInterface_Init(&eepromComInterfaceControl,
                        MEM_EEPROM_I2C_HARDWARE_MODULE,
                        MEM_EEPROM_I2C_HARDWARE_CONFIGURATION,
                        MEM_EEPROM_I2C_HARDWARE_CLOCK_FREQUENCY,
                        MEM_EEPROM_I2C_HARDWARE_SLAVE_ADDRESS);
*/
    memset(eepromMemory, 0, sizeof(eepromMemory));
}

void MEM_EEPROM_ReadPage(WORD address, BYTE * buffer, WORD bufferSize) {

/*
    BYTE eepromHeader[MEM_EEPROM_HEADER_SIZE];
    WORD eepromHeaderSize;
*/

    print_info("Read Page Data into address: %04X, size: %d Bytes", address, bufferSize);
    memcpy(buffer, eepromMemory + address,  bufferSize);
    print_log("Received Data (%d Bytes): ", bufferSize);          
    print_buffer(buffer, bufferSize);
    
/*
    eepromHeaderSize = MEM_EEPROM_BuildHeader(address, eepromHeader, sizeof (eepromHeader));

    I2CComInterface_SendData(&eepromComInterfaceControl, eepromHeader, eepromHeaderSize, TRUE, FALSE);
    I2CComInterface_ReceiveData(&eepromComInterfaceControl, buffer, bufferSize);    
*/
}

void MEM_EEPROM_Read(WORD address, BYTE * buffer, WORD bufferSize) {

    WORD bufferPageSize = MEM_EEPROM_MAX_BUFFER_PAGE_SIZE;

    print_info("Read Data into address: %04X, Total Size: %d", address, bufferSize);

    while(bufferSize){

        if(bufferSize < bufferPageSize)
            bufferPageSize = bufferSize;

        MEM_EEPROM_ReadPage(address, buffer, bufferPageSize);

        bufferSize -= bufferPageSize;
        buffer += bufferPageSize;
        address += bufferPageSize;
    }
}

void MEM_EEPROM_WritePage(WORD address, BYTE * buffer, WORD bufferSize) {

/*
    BYTE eepromHeader[MEM_EEPROM_HEADER_SIZE];
    WORD eepromHeaderSize;  
*/

    print_info("Write Page Data into address: %04X, size: %d Bytes", address, bufferSize);
    print_log("Sent Data (%d Bytes): ", bufferSize);          
    print_buffer(buffer, bufferSize);
    memcpy(eepromMemory + address, buffer, bufferSize);
    
/*
    eepromHeaderSize = MEM_EEPROM_BuildHeader(address, eepromHeader, sizeof (eepromHeader));

    I2CComInterface_SendData(&eepromComInterfaceControl, eepromHeader, eepromHeaderSize, TRUE, FALSE);
    I2CComInterface_SendData(&eepromComInterfaceControl, buffer, bufferSize, FALSE, TRUE);
    I2CComInterface_AcknowledgePolling(&eepromComInterfaceControl);
*/
     
}

void MEM_EEPROM_Write(WORD address, BYTE * buffer, WORD bufferSize) {

    WORD bufferPageSize = MEM_EEPROM_MAX_BUFFER_PAGE_SIZE;

    print_info("Write Data into address: %04X, Total Size: %d Bytes", address, bufferSize);

    while(bufferSize){

        if(bufferSize < bufferPageSize){

            bufferPageSize = bufferSize;
        }
            
        MEM_EEPROM_WritePage(address, buffer, bufferPageSize);

        bufferSize -= bufferPageSize;
        buffer += bufferPageSize;
        address += bufferPageSize;
    }
}
