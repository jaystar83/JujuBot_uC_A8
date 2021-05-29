/*
 * _taskIdle.h
 *
 * Created: 03.04.2021 23:10:36
 *  Author: jayst
 */ 


/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "defines.h"

/***********************************************************************
	ICNLUDES
***********************************************************************/
#if( SW_TEST_ACTIVE )
	#include "_test_avrio.h"
#else
	#include <avr/io.h>
#endif

#include "auxiliaryFunctions.h"
#include "servoCtrl.h"
#include "communicationCtrl.h"

#ifndef _TASKIDLE_H_
#define _TASKIDLE_H_

void taskIdle(uint8_t IsrTimer2Ticks);

#endif /* _TASKIDLE_H_ */
