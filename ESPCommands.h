/* 
 * File:   ESPCommands.h
 * Author: fascio
 *
 * Created on 1 de abril de 2016, 10:42
 */

#ifndef __ESP_COMMANDS_H__
#define	__ESP_COMMANDS_H__

/*Local READ Command Types*/
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

#endif	/* __ESP_COMMANDS_H__ */
