/* 
 * File:   ESPMeteringCommons.h
 * Author: fascio
 *
 * Created on 1 de abril de 2016, 13:43
 */

#ifndef __ESP_METERING_COMMONS_H__
#define	__ESP_METERING_COMMONS_H__

#include "Utility.h"
#define ZERO	0

/*Defines structures table*/
#define ID_METER_SIZE                                                 16
//#define MAC_SIZE                                                       8
#define SERIAL_SIZE                                                    6
#define TYPE_SIZE                                                      1
#define SIGNATURE_SIZE_MTR                                             1
#define SHORT_SIZE                                                     2
#define NUM_MAX_METERS_cab                                            24
#define NUM_MAX_BYTES_Buffer                                          62
#define CRC_bytes                                                      2
#define Address_Size                                                   2
#define FRAME_REST                                                    42
#define METER_NAME_SIZE                            (ID_METER_SIZE+TYPE_SIZE+MAC_SIZE+SIGNATURE_SIZE_MTR+MAC_SIZE+Address_Size+CRC_bytes)
#define METER_NAME_SIZE_FRAG                       (ID_METER_SIZE+TYPE_SIZE+MAC_SIZE+SIGNATURE_SIZE_MTR)
#define Buffer_Lenght_Single_reading               (ID_METER_SIZE+NUM_MAX_BYTES_Buffer+CRC_bytes)
#define BUFFER_LENGHT_SINGLE_FW                    (ID_METER_SIZE+SHORT_SIZE+CRC_bytes)
#define SIGNATURE_MTR                                               0x70
#define METERSTABLE                                                 0x00
#define DEVICESTABLE                                                0x01
#define DEVICESTABLE_SHORT                                          0xF1
#define READINGSTABLE                                               0x02
#define DELETEALL                                                   0x03
#define ALLDATADEV                                                  0x04
#define ALLDATAMTR                                                  0x05
#define ALLDATAREADINGS                                             0x07
#define METER                                                       0x06
#define DEVICE                                                      0x08
#define ALLDATAREADINGS                                             0x07
#define UNKNOWN                                                     0x01
#define FIRST                                                       0x02
#define Lenght_Med_Read                            (ID_METER_SIZE + NUM_MAX_BYTES_Buffer + CRC_bytes)
#define Buffer_Lenght_Meter_Info                   (METER_NAME_SIZE_FRAG+NUM_MAX_BYTES_Buffer+CRC_bytes)
#define Buffer_Lenght_Single_Meter                 (METER_NAME_SIZE_FRAG+MAC_SIZE)
#define Pos_Lenght_1                               (Buffer_Lenght_Meter_Info*NUM_MAX_METERS_cab)+38
#define Pos_Lenght_2                               (Buffer_Lenght_Meter_Info*NUM_MAX_METERS_cab)+39
#define OFSSET_MAC                                                  34
#define OFSET_METER_NAME                                            26

#if Funcionalidad_Num >= 4
#define NUM_MAX_METERS                                              540 // Modificacion Display por Medidor, se seleccionaron 160 medidores y 140 nodos.
#define NUM_MAX_NODES                                               45 // Nota: La modificacion display por gabinete se seleccionaron 230 medidores y 62 nodos.
#else
#define NUM_MAX_METERS                                              540
#define NUM_MAX_NODES                                               45
#endif
#define MAX_IPD_UART_BYTES                                          160

#define LENGTH_DEVICE_TABLE                        (NUM_MAX_NODES*Buffer_Lenght_MAC_Info)

/*Memory MAP I2C*/
#define TIME_AUTO_READINGS_ADD                                      0x0010
#define MAC_ADD_BACK_UP                                             0x0020
#define CABINET_ADD_BACKUP                                          0x0030
#define RLY_STATE_ADD                                               0x0040
#define SHORT_ADD_BACK_UP                                           0x0070
#define NODES_INDEX_ADD                                             0x0072
#define NODES_INDEX_BCKUP                                           0x0074
#define METERS_INDEX_ADD                                            0x0076
#define METERS_INDEX_BCKUP                                          0x0078
#define CAB_READ_Meter_ADD                                          0x0080
#define CAB_READ_Readings_ADD                                       0x3900
#define CAB_READ_Meter_MAC_SHORT                                    0xB600

typedef enum {
    DBMS_HANDLER_NO_TABLE_ID,
    AUTO_READING_TIME_TABLE_ID,            
    DEVICE_INDEX_TABLE_ID,
    METER_INDEX_TABLE_ID,
    DEVICE_TABLE_ID,
    METER_TABLE_ID,    
    READING_TABLE_ID    
} DBMS_HANDLER_TABLE_ID;

/*****************************************************************************
   Data Structures
 *****************************************************************************/

//**********************************************************************
// Vartypes
//**********************************************************************

/*typedef struct _Meter_Eneri
{
    BYTE Serial_Num[ID_METER_SIZE];
    BYTE Type;
    BYTE MACAdd_Display[MAC_SIZE];
    BYTE Signature;
    BYTE MAC_Gabinet[MAC_SIZE];
    BYTE Address[SHORT_SIZE];
    BYTE CRC[CRC_bytes];
} Meter_Eneri, * Meter_Eneri_PTR;*/

/*typedef struct _MAC_Short_Type
{
    BYTE Short_Add[SHORT_SIZE];
    BYTE MAC[MAC_SIZE];
    BYTE Type;
    BYTE SerialNumber[SERIAL_SIZE];
    BYTE Address[SHORT_SIZE];
    BYTE CRC[CRC_bytes];
} MAC_Short_Type;*/

typedef struct __attribute__((packed,aligned(1))) _DEV_LIST
{
    WORD Short_Add;
    BYTE MAC[MAC_SIZE];
    BYTE Type;
    BYTE SerialNumber[SERIAL_SIZE];
} DEV_LIST, * DEV_LIST_PTR;

typedef struct __attribute__((packed,aligned(1))) _MTR_LIST
{
    BYTE Serial_Num[ID_METER_SIZE];
    BYTE Type;
    BYTE MACAdd_Display[MAC_SIZE];
    BYTE Signature;
    BYTE MAC_Cabinet[MAC_SIZE];
} MTR_LIST, * MTR_LIST_PTR;

typedef struct __attribute__((packed,aligned(1))) _READING_LIST
{
    BYTE Serial_Num[ID_METER_SIZE];
    //BYTE Reading[NUM_MAX_BYTES_Buffer];
    Data_Readings Reading;
    WORD CRC;
} READING_LIST, * READING_LIST_PTR;

typedef struct __attribute__((packed,aligned(1))) {	
    MTR_LIST meterItem;
    WORD Address;
    WORD CRC;    
} Meter_Eneri, * Meter_Eneri_PTR;

typedef struct __attribute__((packed,aligned(1))){	
    DEV_LIST deviceItem;
    WORD Address;
    WORD CRC;    
} Device_Eneri, * Device_Eneri_PTR;

#define Buffer_Lenght_MAC_Info                     sizeof(Device_Eneri)

#endif	/* __ESP_METERING_COMMONS_H__ */
