/*
 * servoCtrl.h
 *
 * Created: 20.03.2021 16:15:48
 *  Author: jayst
 */ 

/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "defines.h"

/***********************************************************************
	INCLUDES
***********************************************************************/
#if SW_TEST_ACTIVE
	#include "_test_avrio.h"
	#include "_test_util_delay.h"
#else
	#include <avr/io.h> // AVR types
	#include <util/delay.h>
#endif

#include "defines.h"
#include "pca9685.h"

#ifndef servoMotionCtrl_H_
#define servoMotionCtrl_H_

/***********************************************************************
	DEFINES
***********************************************************************/
#define SPEED_MAX 40
#define ACC_DEC_MAX 5

#define MOTION_INIT 0
#define GO_TO_MAX 1
#define GO_TO_MIN  2
#define GO_TO_REVERT  3
#define EMERGENCY_STOP  0xFE
#define MOTION_FINISHED  0xFF


/***********************************************************************
	FUNCTIONS
***********************************************************************/
uint8_t ServoCtrlInit(long FCPU, long TwiBaud, uint8_t TwePrescaler, uint8_t PCA9685Adress);

// Runs in 1ms Task to check if Servo needs to run
void SercoCtrl_CheckMoveRequest();

// Runs in Idle Task to ctrl servo in 50us steps
void ServoCtrl(uint8_t isrCnt);

#endif /* servoMotionCtrl_H_ */
