/*
 * _99_watchdog.h
 *
 * Created: 04.04.2021 21:16:30
 *  Author: jayst
 */ 

#include "_99_defines.h"
#if( SW_TEST_ACTIVE )
	#include "_test_avrio.h"
#else
	#include <avr/io.h>
#endif


#ifndef _99_WATCHDOG_H_
#define _99_WATCHDOG_H_

typedef struct {
	uint8_t WD_No;
	uint8_t WD_Activation;
	uint8_t TickLimit;
	uint8_t TickCnt;
	uint8_t WD_Allert;
	uint8_t WD_Reset;
}watchdogData;

typedef struct {
	uint8_t* WD_Activation;
	uint8_t* WD_Allert;
	uint8_t* WD_Reset;
}watchdogCtrl;

uint8_t watchdogInit(uint8_t TickLimit, watchdogCtrl* WDCtrlPtr);

void watchdog();




#endif /* _99_WATCHDOG_H_ */
