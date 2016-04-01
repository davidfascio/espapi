/* 
 * File:   ESPMeteringTable.h
 * Author: fascio
 *
 * Created on 1 de abril de 2016, 13:07
 */


#ifndef __ESP_METERING_TABLE_H__
#define	__ESP_METERING_TABLE_H__

//******************************************************************************
// Includes
//******************************************************************************
#include "GenericTypeDefs.h"
#include "System.h"
#include "Data_Base_Handler.h"

BYTE bfnReadMTRReadingsTable(BYTE * dataRequest, WORD dataRequestSize,
        BYTE * dataResponse, WORD * dataResponseSize);

#endif	/* __ESP_METERING_TABLE_H__ */

