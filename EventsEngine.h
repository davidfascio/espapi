/* 
 * File:   EventsEngine.h
 * Author: fascio
 *
 * Created on 17 de diciembre de 2015, 13:28
 */

#ifndef __EVENTS_ENGINE_H__
#define	__EVENTS_ENGINE_H__

#include "Utility.h"
#include "SystemEvents.h"

	void vfnEventsEngineInit(void);
	void vfnEventsEngine(void);


	typedef struct 
	{
		void (* const vfnpCB)(void);
		WORD wReloadValue;
	}sPeriodicTimers;
	
    extern const sPeriodicTimers taPeriodicTimers[];
    
    void vfnPeriodicTimerEnable(BYTE lbID);
    void vfnPeriodicTimerDisable(BYTE lbID);
    void vfnTimeBasedEventsEngine(void);
	typedef struct
	{
	  void (* const vfnpCB)(void);
	}sEvent;
	extern BYTE baEventFlags[];
	extern const sEvent taEvents[];
	extern const BYTE gbaBitMasks[];
	
	
	#define FAST_EVENT_SET(EventID)     (baEventFlags[0]|=(1<<EventID))
	void vfnEventPost(BYTE lbEventID);
	void vfnEventEnable(BYTE lbEventID);
	void vfnEventDisable(BYTE lbEventID);
	void vfnEventClear(BYTE lbEventID);
	#if (USE_ONESHOTS == 1)
	    typedef struct 
	    {
			void (* const vfnpCB)(void);
	    }sOneShotTimer;
		extern const sOneShotTimer taOneShotTimers[];
	    void vfnOneShotReload(BYTE lbID, WORD lbReloadValue);
	    void vfnOneShotDisable(BYTE lbID);  
	#endif

typedef union
{
    struct
    {
        BYTE BACKUP_Busy            : 1;        //16
        BYTE FragmentedBusy         : 1;        //15
        BYTE NewFrameMeterProcesed  : 1;        //14
        BYTE MeterRespond           : 1;        //13
        BYTE ModemRespond           : 1;        //12
        BYTE Time_Stabilize_Done    : 1;        //11
        BYTE MeterNoResponse        : 1;
        BYTE IIC_Busy               : 1;
        BYTE ADDDELMETER_Busy       : 1;        //8
        BYTE CMD_Meter_Busy         : 1;
        BYTE IIC_Error              : 1;
        BYTE MTR_TO_ASSOC           : 1;
        BYTE MEM_Busy               : 1;        //4
        BYTE Send_IPD_Busy          : 1;
        BYTE Send_PLC_Busy          : 1;
        BYTE DE_ASOCIATE_Busy       : 1;
        BYTE Associate_PLC_RUN      : 1;        //1
        BYTE SendReadingGo          : 1;
        BYTE SendSyncGo             : 1;
        BYTE MeterAnswerRequired   : 1;
        //-----
    };
    DWORD flags;
}Flags;

Flags sFlags;

#endif	/* __EVENTS_ENGINE_H__ */
