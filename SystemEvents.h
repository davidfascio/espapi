/* 
 * File:   SystemEvents.h
 * Author: fascio
 *
 * Created on 17 de diciembre de 2015, 13:32
 */

#ifndef SYSTEMEVENTS_H
#define	SYSTEMEVENTS_H

#include <stdio.h>

/*****************************************************************************
  Constants and Enumerations
 *****************************************************************************/
/* None */
#define _1_MSEC_                                             1
#define _25_MSEC_                                            25
#define _500_MSEC_                                          50
#define _100_MSEC_                                          10
#define _200_MSEC_                                          20
#define _300_MSEC_                                          30
#define _900_MSEC_                                          90
#define _1000_MSEC_                                        100
#define _2200_MSEC_                                        220
#define _2000_MSEC_                                        200
#define _3000_MSEC_                                        300
#define _5000_MSEC_                                        500
#define _10000_MSEC_                                      1000
#define _15000_MSEC_                                      1500
#define _20000_MSEC_                                      2000
#define _30000_MSEC_                                      3000
#define _60000_MSEC_                                      6000
#define _1_HOUR_                                           360
#define _1_MIN_20_SEG_                                       80

	/*
		\def	PERIODIC_TIMER_PERIOD
		\brief	Specified the software timers base period in msecs
	*/
	#define PERIODIC_TIMER_PERIOD	(1)
	/*
		\enum	ePeriodicTimers
		\brief	Periodic timers IDs
	*/
	typedef enum
	{
            LED_TOGGLE_MAIN_PERTASK = 0,
            GO_TO_READ_MTR_PERTASK,
            GO_TO_SEND_READ_PERTASK,
            //DPY_SHOW_MAIN,
            //ANNCE_ACC_TIME,
            MAX_PERIODIC_TIMERS
	}ePeriodicTimers;
	/*
		\enum	eEvents
		\brief	Events IDs
	*/
	typedef enum
	{
		TB_EVENT = 0,
                SELF_CLEARING_EVENTS_LIST_END,
                MAIN_TASK_TEST = SELF_CLEARING_EVENTS_LIST_END,
                I2C_DRIVER_EVENT,                                        //Siempre va la maquina de estados despues de events list
                IIC_EVENT,
                BACKUP_START_EVENT,
                BACKUP_EVENT,
                BUFFER_EVENT,
                RLY_BACKUP_EVENT,
                MAC_BACKUP_EVENT,
                TIME_BACKUP_EVENT,                
                MAX_EVENTS
	}eEvents;
	
	#define USE_ONESHOTS	(1)
	/*
		\enum	eOneShots
		\brief	Oneshot Timers IDs
	*/
        typedef enum
        {
            WAIT_FRAME_UART_ONESHOT = 0,
            LEAVE_NWK_ONESHOT,
            TIME_OUT_METER_TO_ASSOC,
            TIME_OUT_METER_RESPONSE_ONESHOT,
            TIME_OUT_METER_STABILIZE_ONESHOT,
            TIME_OUT_IIC_PAGE_ONESHOT,
            WAIT_ANNCE_FIRST_ONESHOT,
            SEND_REQ_TIME_SYNC_ONESHOT,
            SEND_ALARM_STATUS_ONESHOT,
            REPEAT_ALARM_ONESHOT,
            LED_TOGGLE_NWK_ONESHOT,
            SEND_READING_ONESHOT,
            SEND_SINGLE_READING_ONESHOT,
            ANNCE_BUTTON_ONESHOT,
            PERMIT_MODE_ONESHOT,
            DEL_ALL_DATA_ONESHOT,
            SEND_BAPTISM_FRAME_ONESHOT,
            SEND_BAPTISM2_FRAME_ONESHOT,
            SEND_IPD_MSG_ONESHOT,
            TIMEOUT_RELAY_ONESHOT,
            TIMEOUT_FRAG_ANSI_ONESHOT,
            TIMEOUT_FRAG_TO_IPD_ONESHOT,
            TIMEOUT_FRAG_TO_COOR_ONESHOT,
            TIMEOUT_ASSOCIATE_BOTON_ONESHOT,
            CONTINUE_READING_PLC_ONESHOT,
            FRAGDATA_TIMEOUT_ONESHOT,
            TIME_OUT_RST_COOR_ONESHOT,        
            WAIT_FRAME_UART_TESTING_ONESHOT,
            TIME_OUT_UART_TESTING_RESPONSE_ONESHOT,
            TIME_OUT_ZIGBEE_NETWORK_CONNECTION_ONE_SHOT,
            MAX_ONESHOTS
        }eOneShots;

        #ifndef ZERO
#define ZERO 0
#endif

#endif	/* SYSTEMEVENTS_H */
