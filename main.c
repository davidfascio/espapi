/* 
 * File:   main.c
 * Author: fascio
 *
 * Created on 29 de marzo de 2016, 11:12
 */

#include <stdio.h>
#include <stdlib.h>

#include "SystemEvents.h"
#include "EventsEngine.h"

#include "ESP_API.h"
#include "ComSerialInterface.h"

void vfnToogling (void);
void save_devices(void);
//!void vfnGO_TO_READ_MTR_PeriodTask(void);
void vfnTime_Out_Meter_Response_OneShot(void);
void vfnTime_Out_Meter_Stabilize_OneShot(void);
void vfnUART_Char_Received_OneShot(void);

const sEvent taEvents[] =
{
    vfnTimeBasedEventsEngine,                           //  TB_EVENT = 0
    NULL, //vfnMainTask,                                //  SELF_CLEARING_EVENTS_LIST_END, MAIN_TASK_TEST = SELF_CLEARING_EVENTS_LIST_END,
    vfnI2CDriver, //vfnI2CDriver,                               //  I2C_DRIVER_EVENT
    vfnIIC_MEM24_1025Driver,                            //  IIC_EVENT,
    vfnBackUpStartDriver,                               //  BACKUP_START_EVENT
    NULL, //vfnBackUpDriver,                            //  BACKUP_EVENT
    vfn_tBufferLocalDriver, //vfnRLYBackUpStartDriver,  //  BUFFER_EVENT
    NULL, //vfnRLYBackUpStartDriver                     //  RLY_BACKUP_EVENT
    NULL, //vfnMACBackUpStartDriver                     //  MAC_BACKUP_EVENT
    NULL //vfnTIMEBackUpStartDriver                     //  TIME_BACKUP_EVENT
};

const sPeriodicTimers taPeriodicTimers[] =
{
    {vfnToogling, _500_MSEC_ },
    {save_devices,_1_MSEC_},
    //!{vfnGO_TO_READ_MTR_PeriodTask, _10000_MSEC_ },
    {NULL,_1000_MSEC_ }
};

const sOneShotTimer taOneShotTimers[]=
{
    vfnUART_Char_Received_OneShot,
    NULL, //SE_SendNLMELeaveRequest,
    NULL, //vfnMETER_TO_ASSOC_OneShot,
    vfnTime_Out_Meter_Response_OneShot,
    vfnTime_Out_Meter_Stabilize_OneShot,
    NULL, //vfnTime_Out_IIC_Page_OneShot,
    NULL, //vfnSend_Annunce_OneShot,
    NULL, //vfnSendReqTimeSyncOneShot,
    NULL, //vfnSendReportAlarmOneShot,
    NULL, //vfnRepeatAlarmOneShot,
    NULL, //vfnLED_TOGGLE_NWK_ON_OneShot,
    NULL, //vfnSend_Reading_OneShot,
    NULL, //vfnSend_Single_Reading_OneShot,
    NULL, //vfnButtonAnncePressed,
    NULL, //vfnPermitModeOneShot,
    NULL, //vfnDelAllDataOneShot,
    NULL, //vfnSendBaptismOneShot,
    NULL, //vfnSendBaptism2OneShot,
    NULL, //vfnSendIPD_Message_OneShot,
    NULL, //vfnTimeoutRLYOneShot,
    NULL, //vfnSendFragANSIOneShot,
    NULL, //vfnSendFragToIPDOneShot,
    NULL, //vfnSendFragToCoorOneShot,
    NULL, //vfnTimeoutAssociateOneShot,
    NULL, //vfnContinueReadingPLCOneShot,
    NULL, //vfnFragdataBootTimeoutOneShot,
    NULL, //vfnCheckResetOneShot,
    NULL, //vfnUART_Char_Testing_Received_OneShot,
    NULL, //vfnTime_Out_UART_Testing_Response_OneShot,
    NULL //vfnTime_Out_Zigbee_Network_Connection_OneShot
};

void vfnTime_Out_Meter_Stabilize_OneShot(void){
    
    //!MeterControl_ExpireStabilizationTimeout();
}

void vfnTime_Out_Meter_Response_OneShot(void){
    
    bfnIIC_MEM24_1025_Notification();
    //!MeterControl_ExpireResponseTimeout();
    // For Debug proporse
    //MeterControl_SetDataAvailable(TRUE);    
}

void vfnUART_Char_Received_OneShot(void){
    
    ESP_API_ReceivedHandler();
}

void vfnTimer(void){
    
    //Sleep(10);
    //usleep(100);
    /*clock_t now, then;
    long pause = CLOCKS_PER_SEC /1000;
    
    now = then = clock();
    
    while((now - then) < pause)
        now = clock();
    */
    FAST_EVENT_SET(TB_EVENT);
}

BYTE demo_dev_default_mac[] ={0x24,0xda,0xb6,0x0a,0x01,0x09,0x00,0x01};
BYTE demo_dev_default_short_addr[] = {0x01, 0x00};
BYTE demo_dev_default_type = 1;
BYTE demo_dev_default_serial_number[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

BYTE demo_dev_index = 0;
BYTE demo_mtr_index = 1;

BYTE demo_meter_serial_number[20];
BYTE demo_meter_type = 0x81;
BYTE demo_meter_mac_display[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
BYTE demo_meter_signature = 'p';

        

void FillDemoDevices(void);

void FillDemoDevices(void){
    
    DEV_LIST demo_dev;
    MTR_LIST demo_mtr; 
    READING_LIST demo_reading;
    BYTE index;
    Data_Readings dataReading;
    WORD crcFrame;
    
    memset((BYTE *) &dataReading, 0x00, sizeof(Data_Readings));
    
    dataReading.CURRENT_A_Add = 30;
    dataReading.VOLTAGE_A_Add = 12000;
    dataReading.TIME_STAMP_Add = 0;
    dataReading.FRECUENCY_Add = 60;
    
    
    memcpy(demo_reading.Reading, (BYTE *) & dataReading, sizeof(demo_reading.Reading));
    crcFrame = wfnCRC_CALC(demo_reading.Reading, sizeof(demo_reading.Reading), 0xFFFF);
    
    inverted_memcpy((BYTE *) demo_reading.CRC, (BYTE *) &crcFrame, 2);
    
    if(demo_dev_index  < 1){
        
        
        memcpy(demo_dev.Short_Add, demo_dev_default_short_addr, sizeof(demo_dev_default_short_addr));
        
        
        inverted_memcpy(demo_dev.MAC, demo_dev_default_mac, sizeof(demo_dev_default_mac));
        
        demo_dev.Type = demo_dev_default_type;
        memcpy(demo_dev.SerialNumber, demo_dev_default_serial_number, sizeof(demo_dev_default_serial_number));

        bfnSaveData(DEVICESTABLE, (BYTE *) &demo_dev);
        
        memcpy(demo_mtr.MACAdd_Display, demo_meter_mac_display, sizeof(demo_meter_mac_display));
        inverted_memcpy(demo_mtr.MAC_Cabinet, demo_dev_default_mac, sizeof(demo_dev_default_mac));
        demo_mtr.Type = demo_meter_type;
        demo_mtr.Signature = demo_meter_signature;
        
        
        
        
        for(index = 0; index < 0; index++){
            
            sprintf(demo_meter_serial_number, "%016d", demo_mtr_index);
            inverted_memcpy(demo_mtr.Serial_Num, demo_meter_serial_number, 16);
            bfnSaveData (METERSTABLE, (BYTE *) &demo_mtr);
            
            inverted_memcpy(demo_reading.Serial_Num , demo_meter_serial_number, 16);
            
            bfnSaveData(READINGSTABLE, (BYTE *) &demo_reading);
            
            
            demo_mtr_index++;
            demo_mtr.Signature++;
        }
        
        demo_dev_default_short_addr[0]++;
        demo_dev_default_mac[7]++;
        demo_dev_index++;
    }
    
}
int main(int argc, char** argv) {
    
    print_log("ESP API Demo");
    
    
    
    BYTE buffer[500];
    WORD bufferSize = sizeof(buffer);
    WORD address= 0x00b6;
    
    vfnEventsEngineInit();
    vfnEventEnable(TB_EVENT);
    
    // Com Serial Interface Initialization
    ComSerialInterface_Init();
    
    vfnPeriodicTimerEnable(LED_TOGGLE_MAIN_PERTASK);
    vfnPeriodicTimerEnable(GO_TO_READ_MTR_PERTASK);
    
    //ComSerialInterface_FillBuffer(buffer,  bufferSize);
    //!FillDemoDevices();
    memset(buffer, '1',bufferSize );
    bfnIIC_MEM24_1025_Write(buffer,address,bufferSize);
    
    while(TRUE){
        vfnEventsEngine();
        vfnTimer();
        ComSerialInterface_Check();
    }

    return (EXIT_SUCCESS);
}

void vfnToogling (void)
{
    print_debug("TICK");
}

void save_devices(void){
    
    FillDemoDevices();
}
