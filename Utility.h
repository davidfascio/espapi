/* 
 * File:   Utility.h
 * Author: fascio
 *
 * Created on 16 de diciembre de 2015, 13:44
 */

#ifndef __UTILITY_H__
#define	__UTILITY_H__

#include "GenericTypeDefs.h"

#define MAC_SIZE                                             (8)
#define SHORT_MAC_SIZE                                       (4)
#define UTILITY_FRAME_BCC_START_INDEX                                       (1)

typedef struct {
    
    	BYTE bActualState;
    	BYTE bNextState;
    	BYTE bPrevState;
    	BYTE bErrorState;
} sSM;

typedef struct INPUT_BUFFER_TAG
{
    BYTE buffer[240];
    BYTE used;
}INPUT_BUFFER_T;

typedef struct __attribute__((packed,aligned(1))) _Data_Readings
{
    DWORD   ENERGY_ACT_C_Add;
    DWORD   ENERGY_ACT_B_Add;
    DWORD   ENERGY_ACT_A_Add;
    DWORD   POWER_ACT_SYSTEM_Add;
    DWORD   FLAGS_Add_LWEND;
    WORD    DELTA_REAC_Add;
    WORD    DELTA_ACT_Add;
    WORD    INDEX_CON_Add;
    DWORD   ROLLER_DEMAND_Add;
    WORD    POWER_FACTOR_Add;
    WORD    CURRENT_C_Add;
    WORD    CURRENT_B_Add;
    WORD    CURRENT_A_Add;
    WORD    VOLTAGE_C_Add;
    WORD    VOLTAGE_B_Add;
    WORD    VOLTAGE_A_Add;
    WORD    FRECUENCY_Add;
    DWORD   TIME_STAMP_Add;
    DWORD   ENERGY_NEG_ACT_TOTAL_Add;
    DWORD   ENERGY_NEG_REACT_TOTAL_Add;
    DWORD   ENERGY_REACT_TOTAL_Add;
}Data_Readings , * Data_Readings_Ptr;

WORD wfnCRC_CALC (BYTE *ptFRAME, WORD wSizeFrame, WORD wCRCStart);
BYTE wfnBCC_CALC (BYTE * frame, WORD frameLen, BYTE bccStart);
void inverted_memcpy(BYTE * data1, BYTE * data2, WORD data_size);
WORD wfnCRC16 ( BYTE* bpMsg, WORD wDataLen );

extern BYTE macLongAddrByteInverse[MAC_SIZE];
extern BYTE macLongAddrByte[MAC_SIZE];

#endif	/* __UTILITY_H__ */

