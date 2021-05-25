/*
 * _task1ms.h
 *
 * Created: 12.03.2021 21:43:21
 *  Author: jayst
 */ 


/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "_99_defines.h"

/***********************************************************************
	ICNLUDES
***********************************************************************/
#include "_99_auxiliaryFunctions.h"
#include "_01_servoCtrl.h"

#if( SW_TEST_ACTIVE )
	#include "_test_avrio.h"
#else
	#include <avr/io.h>
#endif


#ifndef _TASK1MS_H_
#define _TASK1MS_H_

void task1ms(uint8_t MsToggle);




#endif /* _TASK1MS_H_ */
