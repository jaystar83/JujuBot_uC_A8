/*
 * _01_servoCtrl.h
 *
 * Created: 20.03.2021 16:15:48
 *  Author: jayst
 */ 

/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "_99_defines.h"

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

#include "_99_defines.h"
#include "_00_pca9685.h"

#ifndef _01_SERVOCTRL_H_
#define _01_SERVOCTRL_H_

/***********************************************************************
	DEFINES
***********************************************************************/
#define SPEED_MAX 255

#define MOTION_INIT 0
#define GO_TO_MAX 1
#define GO_TO_MIN  2
#define GO_TO_REVERT  3
#define EMERGENCY_STOP  0xFE
#define MOTION_FINISHED  0xFF

typedef struct {
	uint8_t servoNo;
	uint8_t moveRequest;
	
	uint16_t targetPosition;
	uint8_t targetSpeed;
	uint8_t acceleration;
	uint8_t deceleration;
	
	uint16_t currentPosition;
	uint8_t currentSpeed;
	
	uint8_t startSpeed;	// necessary for position manipulation with accelaration/decelaration  
	
	//// Data for ServoCtrlGoTo	////
	uint8_t servoCtrlGoTo_State;	// 0: Init, 1: to Max, 2: to Min, 3: Emergeny stop,  other -> End

	//// Data for speed ctrl	////
	uint16_t stepDelayTicksCnt;		// counts ticks until next step is released
	uint16_t stepDelayTicksToggle;	// compare value to recognize next Tick
	uint8_t speedCtrlInitDone;		// if FALSE -> init required			
	
}servoData;

//uint8_t servoCtrlInit(servoData* ServoDataPtr, long FCPU, long TwiBaud, uint8_t TwePrescaler, uint8_t PCA9685Adress);
uint8_t ServoCtrlInit(long FCPU, long TwiBaud, uint8_t TwePrescaler, uint8_t PCA9685Adress);

//uint8_t servoCtrlGoTo(servoData* ServoDataPtr);
//uint8_t servoCtrlGoTo_2(servoData* ServoDataPtr, uint8_t isrCnt);
void SercoCtrl_CheckMoveRequest();

void ServoCtrl(uint8_t isrCnt);

////	Getter / Setter	////

uint8_t ServoData_GetServoNo(uint8_t ServoNo);
uint8_t ServoData_GetMoveRequestStatus(uint8_t ServoNo);

uint16_t ServoData_GetTargetPosition(uint8_t ServoNo);
uint8_t ServoData_GetTargetSpeed(uint8_t ServoNo);
uint8_t ServoData_GetAcceleration(uint8_t ServoNo);
uint8_t ServoData_GetDecelaration(uint8_t ServoNo);

uint16_t ServoData_GetCurrentPosition(uint8_t ServoNo);
uint8_t ServoData_GetCurrentSpeed(uint8_t ServoNo);

uint8_t ServoData_SetMoveRequestStatus(uint8_t ServoNo, uint8_t MoveReq);
uint8_t ServoData_SetTargetPosition(uint8_t ServoNo, uint16_t TargetPosi);
uint8_t ServoData_SetTargetSpeed(uint8_t ServoNo, uint8_t TargetSpeed);
uint8_t ServoData_SetAcceleration(uint8_t ServoNo, uint8_t Acceleration);
uint8_t ServoData_SetDeceleratiom(uint8_t ServoNo, uint8_t Deceleration);


#endif /* _01_SERVOCTRL_H_ */
