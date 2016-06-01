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
#include "MEMEEPROM.h"
#include "DBMSHandler.h"

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
    NULL, // vfnI2CDriver, //vfnI2CDriver,                               //  I2C_DRIVER_EVENT
    NULL, //vfnIIC_MEM24_1025Driver,                            //  IIC_EVENT,
    NULL, //vfnBackUpStartDriver,                               //  BACKUP_START_EVENT
    NULL, //vfnBackUpDriver,                            //  BACKUP_EVENT
    ESPMeteringTableDriver, //vfnRLYBackUpStartDriver,  //  BUFFER_EVENT
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
    
    //! bfnIIC_MEM24_1025_Notification();
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
WORD demo_mtr_index = 1;

BYTE demo_meter_serial_number[20];
BYTE demo_meter_type = 0x81;
BYTE demo_meter_mac_display[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
BYTE demo_meter_signature = 'p';

        

void FillDemoDevices(void);

void FillDemoDevices(void){
    
    #ifndef DATA_BASE_TEST
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
    
    
    memcpy( (BYTE *) &demo_reading.Reading, (BYTE *) &dataReading, sizeof(demo_reading.Reading));
    crcFrame = wfnCRC_CALC((BYTE *) &demo_reading.Reading, sizeof(demo_reading.Reading), 0xFFFF);
    
    inverted_memcpy((BYTE *) &demo_reading.CRC, (BYTE *) &crcFrame, 2);
    
    while(demo_dev_index  < 10){
        
        
        memcpy(&demo_dev.Short_Add, demo_dev_default_short_addr, sizeof(demo_dev_default_short_addr));
        
        
        inverted_memcpy(demo_dev.MAC, demo_dev_default_mac, sizeof(demo_dev_default_mac));
        
        demo_dev.Type = demo_dev_default_type;
        memcpy(demo_dev.SerialNumber, demo_dev_default_serial_number, sizeof(demo_dev_default_serial_number));

        //! bfnSaveData(DEVICESTABLE, (BYTE *) &demo_dev);
        //! API_ESPMeteringTable_SaveTable(DEVICESTABLE, (BYTE *) &demo_dev);
        API_ESPMeteringTable_InsertDeviceTableItem((DEV_LIST_PTR) &demo_dev );
        
        memcpy(demo_mtr.MACAdd_Display, demo_meter_mac_display, sizeof(demo_meter_mac_display));
        inverted_memcpy(demo_mtr.MAC_Cabinet, demo_dev_default_mac, sizeof(demo_dev_default_mac));
        demo_mtr.Type = demo_meter_type;
        demo_mtr.Signature = demo_meter_signature;
        
        
        
        
        for(index = 0; index < 10; index++){
            
            sprintf(demo_meter_serial_number, "%016d", demo_mtr_index);
            inverted_memcpy(demo_mtr.Serial_Num, demo_meter_serial_number, 16);
            //! bfnSaveData (METERSTABLE, (BYTE *) &demo_mtr);
            if(demo_mtr_index == 395)
                print("here");
            
            //! API_ESPMeteringTable_SaveTable(METERSTABLE, (BYTE *) &demo_mtr);
            API_ESPMeteringTable_InsertMeterTableItem( (MTR_LIST_PTR) &demo_mtr);
            
            inverted_memcpy(demo_reading.Serial_Num , demo_meter_serial_number, 16);
            
            //! bfnSaveData(READINGSTABLE, (BYTE *) &demo_reading);
            //! API_ESPMeteringTable_SaveTable(READINGSTABLE, (BYTE *) &demo_reading);
            API_ESPMeteringTable_InsertReadingTableItem( (READING_LIST_PTR) &demo_reading);
            
            
            demo_mtr_index++;
            demo_mtr.Signature++;
        }
        
        demo_dev_default_short_addr[0]++;
        demo_dev_default_mac[7]++;
        demo_dev_index++;
    }
#endif
}

extern Meter_Eneri    tsMeter[];
int main(int argc, char** argv) {
    
    print_log("ESP API Demo");
    
    
    
    BYTE buffer[500];
    WORD bufferSize = sizeof(buffer);
    WORD address= 0x00b6;
    BOOL notification;
    WORD index;
    WORD metersTableAddress;
    
    vfnEventsEngineInit();
    vfnEventEnable(TB_EVENT);
    
    // Com Serial Interface Initialization
    ComSerialInterface_Init();
    
    vfnPeriodicTimerEnable(LED_TOGGLE_MAIN_PERTASK);
    //vfnPeriodicTimerEnable(GO_TO_READ_MTR_PERTASK);
    
    //ComSerialInterface_FillBuffer(buffer,  bufferSize);
    //!FillDemoDevices();
    //for(index = 0; index < 500; index++)
    //    buffer[index] = index;
    
    //bfnIIC_MEM24_1025_Write(buffer,address,bufferSize);
    //API_MEM24_1025_I2C_Read(address, buffer, 450, &notification);
    
    /*DEV_LIST device_d;
    memcpy(device_d.SerialNumber,"000001",6);
    device_d.Short_Add = 0x0001;
    device_d.Type = 1;
    memcpy(device_d.MAC, macLongAddrByte, 8);
            
    API_DataBaseHandler_SaveTable(DEVICESTABLE, (BYTE*) &device_d);*/
    
    INT16 error_code = DBMSHandler_Init();    
    
    if(error_code != DBMS_HANDLER_NO_ERROR_CODE)
        exit(0);
    
    API_ESPMeteringTable_Init();
    API_ESPMeteringTable_Init();
    println("");
    
    
    /*BYTE originalString[] = "Hola Mundo!";
    WORD originalStringSize = sizeof(originalString);
    
    BYTE flushString[50];
    memset(flushString, 0, 50);
    
    MEM_EEPROM_Init();
    MEM_EEPROM_Write(0x0000, originalString,originalStringSize );
    MEM_EEPROM_Read(0x0000, flushString, originalStringSize );
    print_debug("%s", flushString);*/
    
    save_devices();
/*
    MTR_LIST meter_d;
    memset(meter_d.MACAdd_Display, 0xFF, 8);
    memcpy(meter_d.MAC_Cabinet, macLongAddrByte, 8);
    memset(meter_d.Serial_Num, '0', 16);
    meter_d.Signature = 'p';
    meter_d.Type = 0x81;
    
    //!Meter_Eneri_PTR meter_ptr = tsMeter;    
    //!memcpy((BYTE* ) &meter_ptr->meterItem,(BYTE* ) &meter_d, sizeof(MTR_LIST));
    
    READING_LIST reading_d;
    memset(reading_d.Serial_Num, '0', 16);
    memset(&reading_d.Reading, 0xFF, sizeof(Data_Readings));
    reading_d.Reading.CURRENT_A_Add = 30;
    reading_d.Reading.VOLTAGE_A_Add = 12000;
    reading_d.Reading.ENERGY_ACT_A_Add = 2000;
    reading_d.Reading.TIME_STAMP_Add = 0;
*/
    
    //!API_DataBaseHandler_SaveTable(METERSTABLE, (BYTE*) &meter_d);
    //!API_DataBaseHandler_SaveTable(READINGSTABLE, (BYTE*) &reading_d);
    
    while(TRUE){
        vfnEventsEngine();
        vfnTimer();
        ComSerialInterface_Check();
        
        if( notification ){
            print_info("Notify: Buffer has changed: ");
            print_buffer(buffer, bufferSize);
            notification = FALSE;
        }
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
