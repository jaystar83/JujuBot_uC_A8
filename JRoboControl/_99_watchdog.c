/*
 * _99_Watchdog.c
 *
 * Created: 04.04.2021 21:15:53
 *  Author: jayst
 */ 

#include "_99_watchdog.h"

watchdogData WD_Data[WATCHDOG_ELEMENTS] = {0};

/*
	uint8_t WD_No;
	uint8_t WD_Activation;
	uint8_t TickLimit;
	uint8_t TickCnt;
	uint8_t WD_Allert;
	uint8_t WD_Reset;

*/

uint8_t watchdogInit(uint8_t TickLimit, watchdogCtrl* WDCtrlPtr)
{
	watchdogCtrl* wdCtrlTemp = WDCtrlPtr;
	
	for(uint8_t i=0; i<WATCHDOG_ELEMENTS; i++)
	{
		if(WD_Data[i].WD_No == 0)
		{
			WD_Data[i].WD_No = i+1;
			WD_Data[i].TickLimit = TickLimit;

			wdCtrlTemp->WD_Activation = &WD_Data[i].WD_Activation;
			wdCtrlTemp->WD_Allert = &WD_Data[i].WD_Allert;
			wdCtrlTemp->WD_Reset = &WD_Data[i].WD_Reset;
			
			return TRUE; 		
		}
	}
	return FALSE;
}

void watchdog()
{
	for(uint8_t i=0; i<WATCHDOG_ELEMENTS; i++)
	{
		if(WD_Data[i].WD_Reset)
		{
			WD_Data[i].WD_Allert = FALSE;
			WD_Data[i].TickCnt = 0;
			WD_Data[i].WD_Reset = FALSE;
		}
		else if(WD_Data[i].WD_Activation)
		{
			if(WD_Data[i].TickCnt++ > WD_Data[i].TickLimit)
			{
				WD_Data[i].WD_Allert = TRUE;
				WD_Data[i].WD_Activation = FALSE;
			}
		}

	}

}
