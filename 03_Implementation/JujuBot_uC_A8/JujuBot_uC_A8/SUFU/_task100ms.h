/*
 * task100ms.h
 *
 * Created: 12.03.2021 20:13:07
 *  Author: jayst
 */ 


/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "defines.h"

/***********************************************************************
	ICNLUDES
***********************************************************************/
#include "auxiliaryFunctions.h"
#if( SW_TEST_ACTIVE )
	#include "_test_avrio.h"
#else
	#include <avr/io.h>
#endif


#ifndef TASK100MS_H_
#define TASK100MS_H_

void task100ms(uint8_t _100msCntInput);

void blink1s(uint8_t blinkCntNo, uint8_t Port, uint8_t Pin);


#endif /* TASK100MS_H_ */