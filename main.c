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
//!void vfnGO_TO_READ_MTR_PeriodTask(void);
void vfnTime_Out_Meter_Response_OneShot(void);
void vfnTime_Out_Meter_Stabilize_OneShot(void);
void vfnUART_Char_Received_OneShot(void);

const sEvent taEvents[] =
{
    vfnTimeBasedEventsEngine,
    NULL, //vfnMainTask,
    NULL, //vfnI2CDriver,
    NULL, //vfnIIC_MEM24_1025Driver,
    NULL,
    NULL, //vfnBackUpDriver,
    NULL //vfnRLYBackUpStartDriver,
    #if IT_HAS_SHELL
    Shell_Task,
    vfn_Zigbee_Network_Connection_Check,
    #endif
};

const sPeriodicTimers taPeriodicTimers[] =
{
    {vfnToogling, _500_MSEC_ },
    //{vfnLED_TOGGLE_NWK_ON_PeriodTask,_500_MSEC_},
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
    
    //!MeterControl_ExpireResponseTimeout();
    // For Debug proporse
    //MeterControl_SetDataAvailable(TRUE);    
}

void vfnUART_Char_Received_OneShot(void){
    
    ESP_API_ReceivedHandler();
}

void vfnTimer(void){
    
    //Sleep(10);
    usleep(100);
    /*clock_t now, then;
    long pause = CLOCKS_PER_SEC /1000;
    
    now = then = clock();
    
    while((now - then) < pause)
        now = clock();
    */
    FAST_EVENT_SET(TB_EVENT);
}

int main(int argc, char** argv) {
    
    print_log("ESP API Demo");
    vfnEventsEngineInit();
    vfnEventEnable(TB_EVENT);
    
    // Com Serial Interface Initialization
    ComSerialInterface_Init();
    
    vfnPeriodicTimerEnable(LED_TOGGLE_MAIN_PERTASK);
    
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
