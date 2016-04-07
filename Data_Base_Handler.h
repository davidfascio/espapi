/****************************************************************************************/
/*
 * File:        Data_Base_Handler.h
 * Author:      Jose Antonio Becerril Morales
 * version:     Intern Support
 * Processor:   PIC32MX360F512L
 * Complier:    C32(v2.02)
 * Company:     Eneri
 * Date:        Created on September 17,2013.
 * Comment:     Data Base Control
 */
/****************************************************************************************/
#ifndef DATA_BASE_HANDLER_H
#define DATA_BASE_HANDLER_H
/*-- Includes --------------------------------------------------------------------------*/
//!#include "HIL/mem24_1025_I2C.h"
//#include "ESPCommands.h"
#include "GenericTypeDefs.h"
#include "Utility.h"
#include "mem24_1025_I2C.h"
#include "ESPMeteringCommons.h"
#include "EventsEngine.h"
#include "SystemEvents.h"

/*-- Types Definitions -----------------------------------------------------------------*/

/*-- Local functions prototypes --------------------------------------------------------*/

/*-- Defines ---------------------------------------------------------------------------*/
#define NEW_MTR_ADD                 0x02
#define NO_NEW_MTR_ADD              0x03
/*-- Variables -------------------------------------------------------------------------*/

/*-- Functions -------------------------------------------------------------------------*/
BYTE bfnSaveData(BYTE bTableType,BYTE *vptrTableStructure);
BYTE bfnConsultData(BYTE bTableType, BYTE *bptrKeyID, WORD wAllDataIndexDev
        , WORD wAllDataIndexMtr, BYTE *bptrDataWrite, WORD * bptrDataWriteSize);
void vfnInitDataBase(void);
BYTE bfnBackUp_Init (void);
WORD wfnIndexConsutl(BYTE bTableType);
void vfnBackUpStartDriver(void);
BYTE bfnDelAllData(BYTE bTableType,BYTE *bptrKeyID);
void vfnSaveIndexMet(void);
#endif
/****************************************************************************************/
/****************************************************************************************/




