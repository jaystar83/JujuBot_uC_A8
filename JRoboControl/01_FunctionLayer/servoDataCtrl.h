/*
 * servoDataCtrl.h
 *
 * Created: 07.06.2021 21:52:46
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


#ifndef SERVODATACTRL_H_
#define SERVODATACTRL_H_

/***********************************************************************
	DEFINES
***********************************************************************/
typedef struct {
	uint8_t servoNo;
	uint8_t moveRequest;
	
	uint16_t targetPosition;
	uint8_t targetSpeed;
	uint8_t acceleration;
	uint8_t deceleration;
	
	uint16_t currentPosition;
	uint8_t currentSpeed;
	
	//// Data for ServoCtrlGoTo	////
	uint8_t servoMotionCtrl_State;		// 0: Init, 1: to Max, 2: to Min, 3: Emergeny stop,  other -> End
	uint8_t motionPhase;				// 0: -, 1: Acceleration, 2: uniform movement, 3: decelaration

	//// Data for speed ctrl	////
	uint16_t speedDelayTicksCnt;		// counts ticks until next speed value is released
	uint16_t speedDelayTicksToggle;		// compare value to recognize next Tick
	uint8_t speedCtrlInitDone;			// if FALSE -> init required
	
	//// Data for step ctrl	////
	uint16_t stepDelayTicksCnt;		// counts ticks until next step is released
	uint16_t stepDelayTicksToggle;	// compare value to recognize next Tick
	uint8_t stepCtrlInitDone;		// if FALSE -> init required
	
}servoData;

#define MoPha_OFF				0	// Stop 
#define MoPha_AccToTargetSpeed	1	// Acceleration to target speed
#define MoPha_UniformMoving		2	// Uniform movement
#define MoPha_DecToStop			3	// Deceleration to Stop
#define MoPha_DecToTargetSpeed	4	// Deceleration to target speed


/***********************************************************************
	FUNCTIONS
***********************************************************************/
uint8_t ServoCtrlInit(long FCPU, long TwiBaud, uint8_t TwePrescaler, uint8_t PCA9685Adress);

////	Getter / Setter	////
uint8_t ServoData_GetMotionCtrlState(uint8_t ServoNo);

uint8_t ServoData_GetServoNo(uint8_t ServoNo);
uint8_t ServoData_GetMoveRequest(uint8_t ServoNo);

uint16_t ServoData_GetTargetPosition(uint8_t ServoNo);
uint8_t ServoData_GetTargetSpeed(uint8_t ServoNo);
uint8_t ServoData_GetAcceleration(uint8_t ServoNo);
uint8_t ServoData_GetDecelaration(uint8_t ServoNo);

uint16_t ServoData_GetCurrentPosition(uint8_t ServoNo);
uint8_t ServoData_GetCurrentSpeed(uint8_t ServoNo);

uint8_t ServoData_GetMotionPhase(uint8_t ServoNo);

uint16_t ServoData_GetSpeedDelayTicksCnt(uint8_t ServoNo);
uint16_t ServoData_GetSpeedDelayTicksToggle(uint8_t ServoNo);
uint8_t ServoData_GetSpeedCtrlInitDone(uint8_t ServoNo);

uint16_t ServoData_GetStepDelayTicksCnt(uint8_t ServoNo);
uint16_t ServoData_GetStepDelayTicksToggle(uint8_t ServoNo);
uint8_t ServoData_GetStepCtrlInitDone(uint8_t ServoNo);


uint8_t ServoData_SetMotionCtrlState(uint8_t ServoNo, uint8_t State);

uint8_t ServoData_SetMoveRequest(uint8_t ServoNo, uint8_t MoveReq);
uint8_t ServoData_SetTargetPosition(uint8_t ServoNo, uint16_t TargetPosi);
uint8_t ServoData_SetTargetSpeed(uint8_t ServoNo, uint8_t TargetSpeed);
uint8_t ServoData_SetAcceleration(uint8_t ServoNo, uint8_t Acceleration);
uint8_t ServoData_SetDeceleration(uint8_t ServoNo, uint8_t Deceleration);

uint8_t ServoData_SetCurrentSpeed(uint8_t ServoNo, uint8_t CurrentSpeed);

uint8_t ServoData_SetMotionPhase(uint8_t ServoNo, uint8_t MotionPhase);

uint8_t ServoData_SetSpeedDelayTicksCnt(uint8_t ServoNo, uint16_t SpeedDelayTicksCnt);
uint8_t ServoData_SetSpeedDelayTicksToggle(uint8_t ServoNo, uint16_t SpeedDelayTicksToggle);
uint8_t ServoData_SetSpeedCtrlInitDone(uint8_t ServoNo, uint8_t SpeedCtrlInitDone);

uint8_t ServoData_SetStepDelayTicksCnt(uint8_t ServoNo, uint16_t StepDelayTicksCnt);
uint8_t ServoData_SetStepDelayTicksToggle(uint8_t ServoNo, uint16_t StepDelayTicksToggle);
uint8_t ServoData_SetStepCtrlInitDone(uint8_t ServoNo, uint8_t StepCtrlInitDone);


uint8_t ServoData_IncrementPoistion(uint8_t ServoNo);
uint8_t ServoData_DecrementPoistion(uint8_t ServoNo);
#endif /* SERVODATACTRL_H_ */