/* 
 * File:   ESPCommands.h
 * Author: fascio
 *
 * Created on 1 de abril de 2016, 10:42
 */

#ifndef __ESP_COMMANDS_H__
#define	__ESP_COMMANDS_H__


//******************************************************************************
// Local READ Command Types
//******************************************************************************

#define READMODE_VERSION_COOR_LOCAL                                 0x40
#define ZBEE_GRAL_CMD                                               0x41
#define RESET_COOR_LCL                                              0x22
#define ALARM_SEND                                                  0x25
#define ALARM_BOOT_SEND                                             0x52
#define UPDATE_FRAME_IPD                                            0x26
#define SEND_FRAME_IPD                                              0x27
#define SYNC_REQUEST                                                0x28
#define ALARM_PROCESSED                                             0x29
#define FLAG_READING_CONFIGURE                                      0x2C
#define TIME_READING_CONFIGURE                                      0x2D
#define READ_ALARM_REMOTE                                           0x2A
#define READMODE_CHANNEL_COOR_LOCAL                                 0x70
#define READ_MODE_UPDATE_FRECUENCY_AUTOMATIC_READING                0x2F
#define READMODE_PAN_ID_COOR_LOCAL                                  0x61
#define READMODE_TIME_DATE_LOCAL                                    0x51
#define READMODE_ADC_STATE                                          0x1A
#define READMODE_RELAY_STATE                                        0x1B
#define READMODE_RELAY_SECURE_STATE                                 0x1C
#define READMODE_ADC_RELAY_SECURE_STATE                             0x1D
#define READMODE_STATUS_LOCAL                                       0x90
#define READ_MODE_UPDATE_RLY_REMOTE                                 0x36
#define READ_MODE_ANALOG_REMOTE                                     0x3A
#define READ_MODE_UPDATE_RLY_ANALOG_REMOTE                          0x3D

//******************************************************************************
// Local WRITE Command Types
//******************************************************************************
#define WRITE_MODE_RST_RED_COOR_LOCAL                               0x99
#define WRITE_MODE_RST_COOR_LOCAL                                   0x22
#define WRITE_MODE_DPY_BUFFER_LOAD_LOCAL                            0x36
#define WRITE_MODE_DPY_BUFFER_SEND_IPD                              0x37
#define WRITE_MODE_UPDATE_TIME_DATE                                 0x50
#define WRITE_MODE_UPDATE_RELAY_STATE                               0xA0
#define WRITE_MODE_UPDATE_RELAY_SECURE_STATE                        0xA1
#define WRITE_MODE_UPDATE_RELAY_GROUP_STATE                         0xA2
#define WRITE_MODE_DEL_INDEX_READINGS_TABLE                         0x78
#define WRITE_MODE_DEL_INDEX_ALL_TABLES                             0x3C
#define WRITE_MODE_UPDATE_FRECUENCY_AUTOMATIC_READINGS              0x2E
#define WRITE_MODE_UPDATE_PERMIT_JOIN_MODE                          0x20
#define WRITE_MODE_UPDATE_PERMIT_REMOTE                             0x21
#define WRITE_MODE_UPDATE_CONFIG_ALARM_REMOTE                       0x24            //Escribir alarma
#define WRITE_MODE_UPDATE_PAN_ID                                    0x60
#define WRITE_MODE_CHANNEL_COOR_LOCAL                               0x71
#define WRITE_MODE_UPDATE_RLY_REMOTE                                0x38
#define WRITE_MODE_UPDATE_RLY_SECURE_REMOTE                         0x39
#define WRITE_MODE_UPDATE_GROUP_RLY_REMOTE                          0x3B

//******************************************************************************
// Local Buffer Commands
//******************************************************************************

#define BUFFERMODE_READ_TEMPORAL_BUFFER                             0x33
#define BUFFERMODE_READ_DEVICES_ZBEE_BUFFER                         0x31
#define BUFFERMODE_READ_ALL_MTR_READINGS_BUFFER                     0x32
#define BUFFERMODE_READ_TEMPORAL_BUFFER                             0x33
#define BUFFERMODE_READ_ALL_MTR_MACS_BUFFER                         0x30
#define BUFFERMODE_READ_ALL_SHORTS_MACS_TYPES_BUFFER                0x3E
#define DEL_METERS_TABLE                                            0x34
#define DEL_DEVICES_TABLE                                           0x35
#define LEAVE_DELETE_DEVICE_CMD                                     0x07
#define DELETE_DEVICE_CMD                                           0x08
#define DEL_READINGS_TABLE                                          0x36


#endif	/* __ESP_COMMANDS_H__ */

