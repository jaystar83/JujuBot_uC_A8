/*
 * _CommunicationCtrl.h
 *
 * Created: 20.03.2021 15:54:38
 *  Author: jayst
 */ 


/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "defines.h"
#include "watchdog.h"


#if( SW_TEST_ACTIVE )
	#include "_test_avrio.h"
#else
	#include <avr/io.h>
#endif

#ifndef _COMMUNICATIONCTRL_H_
#define _COMMUNICATIONCTRL_H_ 1

uint8_t CommunicationCtrlInit();
uint8_t CommunicationCtrl();


#endif /* _COMMUNICATIONCTRL_H_ */
