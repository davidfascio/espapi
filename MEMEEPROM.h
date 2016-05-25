#ifndef __MEM_EEPROM_H__
#define __MEM_EEPROM_H__

//**********************************************************************
//* MEM_EEPROM Includes
//**********************************************************************
#include "SystemLog.h"

//**********************************************************************
//* MEM_EEPROM Functions
//**********************************************************************
#define MEM_EEPROM_I2C_HARDWARE_MODULE                      (I2C2)
#define MEM_EEPROM_I2C_HARDWARE_CONFIGURATION               (I2C_ENABLE_HIGH_SPEED )
#define MEM_EEPROM_I2C_HARDWARE_CLOCK_FREQUENCY             (1000000)
#define MEM_EEPROM_I2C_HARDWARE_SLAVE_ADDRESS               (0x56)
#define MEM_EEPROM_HEADER_SIZE                              (3)
#define MEM_EEPROM_MAX_BUFFER_PAGE_SIZE                     (0x0080)
#define MEM_EEPROM_MAX_MEMORY_SIZE                          (0xFFFF)
//**********************************************************************
//* MEM_EEPROM Datatypes
//**********************************************************************


//**********************************************************************
//* MEM_EEPROM Functions
//**********************************************************************

void MEM_EEPROM_Init(void);
//WORD MEM_EEPROM_BuildHeader(WORD address, BYTE * buffer, WORD bufferSize);
void MEM_EEPROM_ReadPage(WORD address, BYTE * buffer, WORD bufferSize);
void MEM_EEPROM_Read(WORD address, BYTE * buffer, WORD bufferSize);
void MEM_EEPROM_WritePage( WORD address, BYTE * buffer, WORD bufferSize);
void MEM_EEPROM_Write(WORD address, BYTE * buffer, WORD bufferSize);

#endif /* __MEM_EEPROM_H__ */
