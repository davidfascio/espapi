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

WORD wfnCRC_CALC (BYTE *ptFRAME, WORD wSizeFrame, WORD wCRCStart);
BYTE wfnBCC_CALC (BYTE * frame, WORD frameLen, BYTE bccStart);
void inverted_memcpy(BYTE * data1, BYTE * data2, WORD data_size);
WORD wfnCRC16 ( BYTE* bpMsg, WORD wDataLen );

extern BYTE macLongAddrByteInverse[MAC_SIZE];
extern BYTE macLongAddrByte[MAC_SIZE];

#endif	/* __UTILITY_H__ */

