//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "EventsEngine.h"
BYTE gbaBitMask[] = {1,2,4,8,16,32,64,128};
WORD waPeriodicTmrsCounters[MAX_PERIODIC_TIMERS];
BYTE baPeriodicTimersReadyList[((MAX_PERIODIC_TIMERS-1)/8)+1];  
BYTE baEventFlags[((MAX_EVENTS-1)/8)+1];
BYTE baEventsReadyList[((MAX_EVENTS-1)/8)+1];
#if (USE_ONESHOTS == 1)
	WORD waOneShotsCounters[MAX_ONESHOTS];
	BYTE baOneShotsReadyList[((MAX_ONESHOTS-1)/8)+1];
#endif
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
void vfnEventsEngineInit(void)
{
	BYTE lbIndex;
	//Periodic Timers
  	lbIndex = ((MAX_PERIODIC_TIMERS-1)/8);
  	do
  	{
		baPeriodicTimersReadyList[lbIndex] = 0;
	}while(lbIndex--);
    lbIndex = ((MAX_EVENTS-1)/8);
    do 
    {
      baEventFlags[lbIndex] = 0;
      baEventsReadyList[lbIndex] = 0;      
    }while(lbIndex--);   
	#if (USE_ONESHOTS == 1)
		//One Shot timers
		lbIndex = sizeof(baOneShotsReadyList) - 1;
		do
		{
			baOneShotsReadyList[lbIndex] = 0;
		}while(lbIndex--);
	#endif
}
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
void vfnEventsEngine(void)
{
	//Events
	register sEvent *ltpEvents = (sEvent *)&taEvents[MAX_EVENTS-1];
	register BYTE* lbpEventsFlags = &baEventFlags[(MAX_EVENTS-1)/8];
	register BYTE lbIndex = MAX_EVENTS-1;
	register BYTE* bapPtr = &baEventsReadyList[(MAX_EVENTS-1)/8];
	do
	{
		if((*bapPtr)&gbaBitMask[lbIndex%8])
		{
			if((*lbpEventsFlags)&gbaBitMask[lbIndex%8])
			{
				if(lbIndex < SELF_CLEARING_EVENTS_LIST_END)
				{
					//Event Acknowledge
					(*lbpEventsFlags) &= ~gbaBitMask[lbIndex%8];
				}
				//Event Callback
				ltpEvents->vfnpCB();
			}
		}
		if(!(lbIndex%8))
		{
			bapPtr--;
			lbpEventsFlags--;			
		}
		ltpEvents--;
	}while(lbIndex--);
}
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
void vfnPeriodicTimerEnable(BYTE bID)
{
	if(MAX_PERIODIC_TIMERS > bID)
	{
		baPeriodicTimersReadyList[(bID/8)] |= gbaBitMask[bID%8];
		waPeriodicTmrsCounters[bID] = taPeriodicTimers[bID].wReloadValue;		
	}
}
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
void vfnPeriodicTimerDisable(BYTE lbID)
{
	if(MAX_PERIODIC_TIMERS > lbID)
	{
		baPeriodicTimersReadyList[(lbID/8)] &= ~gbaBitMask[lbID%8];
	}
}
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
void vfnEventPost(BYTE bEventID)
{
	if(MAX_EVENTS > bEventID) 
	{
		BYTE bEventIndex = (BYTE)(bEventID/8);
		bEventID %= 8;
		baEventFlags[bEventIndex]|=gbaBitMask[bEventID];
	}
}
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
void vfnEventEnable(BYTE bEventID)
{
	if(MAX_EVENTS > bEventID) 
	{
		BYTE bEventIndex = (BYTE)(bEventID/8);
		bEventID %= 8;
		baEventsReadyList[bEventIndex]|=gbaBitMask[bEventID];
	}
}
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
void vfnEventDisable(BYTE lbEventID)
{
	if(MAX_EVENTS > lbEventID) 
	{
		BYTE lbEventIndex = (BYTE)(lbEventID/8);
		lbEventID %= 8;
		baEventsReadyList[lbEventIndex]&=~gbaBitMask[lbEventID];
	}
}
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
void vfnEventClear(BYTE lbEventID)
{
	if(MAX_EVENTS > lbEventID) 
	{
		BYTE lbEventIndex = (BYTE)(lbEventID/8);
		lbEventID %= 8;
		baEventFlags[lbEventIndex]&=~gbaBitMask[lbEventID];
	}

}
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
void vfnTimeBasedEventsEngine(void)
{
	register sPeriodicTimers *tpPeriodicTmr = (sPeriodicTimers *)&taPeriodicTimers[MAX_PERIODIC_TIMERS-1];
	register WORD* wapPtr = &waPeriodicTmrsCounters[MAX_PERIODIC_TIMERS-1];
	register BYTE lbIndex = MAX_PERIODIC_TIMERS-1;    
	register BYTE* bapPtr = &baPeriodicTimersReadyList[(MAX_PERIODIC_TIMERS-1)/8];
	do
	{
		if((*bapPtr)&gbaBitMask[lbIndex%8])
		{
			if(*wapPtr)
    		{
    		  *wapPtr = (WORD)(*wapPtr - 1);
    		}
    		else
    		{
				*wapPtr = tpPeriodicTmr->wReloadValue;
				tpPeriodicTmr->vfnpCB();
			}
		}
		wapPtr--;
		tpPeriodicTmr--;  
		if(!(lbIndex%8))
		{
			bapPtr--;		
		}
	}while(lbIndex--);
	#if (USE_ONESHOTS == 1)
	{
		register sOneShotTimer *tpOneShots = (sOneShotTimer *)&taOneShotTimers[MAX_ONESHOTS-1];
		lbIndex = MAX_ONESHOTS-1;
		wapPtr = &waOneShotsCounters[MAX_ONESHOTS-1];
		bapPtr = &baOneShotsReadyList[(MAX_ONESHOTS-1)/8];
		do
		{
			if((*bapPtr)&gbaBitMask[lbIndex%8])
			{
				if(*wapPtr)
				{
					*wapPtr = *wapPtr - 1;
				}
				else
				{
					(*bapPtr) &= ~gbaBitMask[lbIndex%8];
					tpOneShots->vfnpCB();          
				}
			}
			if(!(lbIndex%8))
			{
				bapPtr--;
			}
			wapPtr--;
			tpOneShots--;
		}while(lbIndex--);
	}
	#endif
}
#if (USE_ONESHOTS == 1)
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
	void vfnOneShotReload(BYTE lbID, WORD lbReloadValue)
	{
		if(MAX_ONESHOTS > lbID)
		{
			waOneShotsCounters[lbID] = lbReloadValue;
			baOneShotsReadyList[(lbID/8)] |= gbaBitMask[lbID%8];      
		}
	}
//------------------------------------------------------------------------------
/*!
	\fn     void vfnEventsEngine(void)
	\param  none
	\brief  
*/ 
//------------------------------------------------------------------------------
	void vfnOneShotDisable(BYTE lbID)
	{
		if(MAX_ONESHOTS > lbID)
		{
			baOneShotsReadyList[(lbID/8)] &= ~gbaBitMask[lbID%8];
		}
	}
#endif
//------------------------------------------------------------------------------


