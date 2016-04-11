/* 
 * File:   GenericTypeDefs.h
 * Author: fascio
 *
 * Created on 16 de diciembre de 2015, 11:04
 */

#ifndef __GENERIC_TYPE_DEFS_H__
#define	__GENERIC_TYPE_DEFS_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

//typedef enum _BOOL { FALSE = 0, TRUE } BOOL;    /* Undefined size */
//typedef enum _BIT { CLEAR = 0, SET } BIT;

//typedef unsigned char           BYTE;                           /* 8-bit unsigned  */
//typedef unsigned short int      WORD;                           /* 16-bit unsigned */
//typedef unsigned long           DWORD;                          /* 32-bit unsigned */

typedef struct _LONG_ADDR
{
    BYTE v[8];
} LONG_ADDR;

#endif	/* __GENERIC_TYPE_DEFS_H__ */
