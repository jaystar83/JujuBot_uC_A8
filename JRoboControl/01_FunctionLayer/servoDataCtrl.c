/*
 * servoDataCtrl.c
 *
 * Created: 07.06.2021 21:53:47
 *  Author: jayst
 */ 



/***********************************************************************
	ICNLUDES
***********************************************************************/
#include "servoDataCtrl.h"
#include "auxiliaryFunctions.h"

/***********************************************************************
	LOCAL VARS
***********************************************************************/
servoData servoArr[SERVO_CNT] = {0};

/***********************************************************************
	GLOBAL FUNCTIONS IMPLEMENTATION
***********************************************************************/
//uint8_t servoCtrlInit(uint8_t ServoNo, long FCPU, long TwiBaud, uint8_t TwePrescaler, uint8_t PCA9685Adress)
uint8_t ServoCtrlInit(long FCPU, long TwiBaud, uint8_t TwePrescaler, uint8_t PCA9685Adress)
{
	for(uint8_t i=0; i<SERVO_CNT; i++)
	{		
		servoArr[i].servoNo = i;
		servoArr[i].moveRequest = FALSE;
		
		servoArr[i].targetSpeed = 0;
		servoArr[i].acceleration = 0;
		servoArr[i].deceleration = 0;
		servoArr[i].currentSpeed = 0;
		
		servoArr[i].servoMotionCtrl_State= 0;	// 0: Init, 1: to Max, 2: to Min, 3: Emergeny stop,  other -> End
	}
	servoArr[0].targetPosition = SERVO_0_INIT;
	servoArr[0].currentPosition = SERVO_0_INIT;

	servoArr[1].targetPosition = SERVO_1_INIT;
	servoArr[1].currentPosition = SERVO_1_INIT;

	servoArr[2].targetPosition = SERVO_2_INIT;
	servoArr[2].currentPosition = SERVO_2_INIT;

	servoArr[3].targetPosition = SERVO_3_INIT;
	servoArr[3].currentPosition = SERVO_3_INIT;
	
	servoArr[4].targetPosition = SERVO_4_INIT;
	servoArr[4].currentPosition = SERVO_4_INIT;
	
	servoArr[5].targetPosition = SERVO_5_INIT;
	servoArr[5].currentPosition = SERVO_5_INIT;

	#if( SERVEO_CTRL_BOARD_AVAILABLE )
		if( !(InitPCA9685(SERVO_MODE, FCPU, TwiBaud, TwePrescaler, PCA9685Adress)) )
			return 0;

		if (!PCA9685_Sleep(0) )
			return 0;

		for(uint8_t i=0; i<SERVO_CNT; i++)
		{
			Servo(servoArr[i].servoNo, servoArr[i].targetPosition);
		}
	#endif
	
	return 1;
}


/***********************************************************************
	GETTER / SETTER
***********************************************************************/
uint8_t ServoData_GetServoData(uint8_t ServoNo, servoData *ServoCtrlData)
{	
	ServoCtrlData->servoNo = servoArr[ServoNo].servoNo;
	ServoCtrlData->moveRequest = servoArr[ServoNo].moveRequest;

	ServoCtrlData->targetPosition = servoArr[ServoNo].targetPosition;
	ServoCtrlData->targetSpeed = servoArr[ServoNo].targetSpeed;
	ServoCtrlData->acceleration = servoArr[ServoNo].acceleration;
	ServoCtrlData->deceleration = servoArr[ServoNo].deceleration;

	ServoCtrlData->currentPosition = servoArr[ServoNo].currentPosition;
	ServoCtrlData->currentSpeed = servoArr[ServoNo].currentSpeed;

	//// Data for ServoCtrlGoTo	////
	ServoCtrlData->servoMotionCtrl_State = servoArr[ServoNo].servoMotionCtrl_State;
	ServoCtrlData->motionPhase = servoArr[ServoNo].motionPhase;

	//// Data for speed ctrl	////
	ServoCtrlData->speedDelayTicksCnt = servoArr[ServoNo].speedDelayTicksCnt;
	ServoCtrlData->speedDelayTicksToggle = servoArr[ServoNo].speedDelayTicksToggle;
	ServoCtrlData->speedCtrlInitDone = servoArr[ServoNo].speedCtrlInitDone;

	//// Data for step ctrl	////
	ServoCtrlData->stepDelayTicksCnt = servoArr[ServoNo].stepDelayTicksCnt;
	ServoCtrlData->stepDelayTicksToggle = servoArr[ServoNo].stepDelayTicksToggle;
	ServoCtrlData->stepCtrlInitDone = servoArr[ServoNo].stepCtrlInitDone;
	
	return TRUE;
}

uint8_t ServoData_GetMotionCtrlState(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].servoMotionCtrl_State;
}


uint8_t ServoData_GetServoNo(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].servoNo;
}

uint8_t ServoData_GetMoveRequest(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].moveRequest;
}

uint16_t ServoData_GetTargetPosition(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFFFF;
	else
		return servoArr[ServoNo].targetPosition;
}

uint8_t ServoData_GetTargetSpeed(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].targetSpeed;
}

uint8_t ServoData_GetAcceleration(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].acceleration;
}

uint8_t ServoData_GetDecelaration(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].deceleration;
}

uint16_t ServoData_GetCurrentPosition(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFFFF;
	else
		return servoArr[ServoNo].currentPosition;
}

uint8_t ServoData_GetCurrentSpeed(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].currentSpeed;
}

uint8_t ServoData_GetMotionPhase(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	
	return servoArr[ServoNo].motionPhase;
}


uint16_t ServoData_GetSpeedDelayTicksCnt(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFFFF;
	else
		return servoArr[ServoNo].speedDelayTicksCnt;
}

uint16_t ServoData_GetSpeedDelayTicksToggle(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFFFF;
	else
		return servoArr[ServoNo].speedDelayTicksToggle;
}

uint8_t ServoData_GetSpeedCtrlInitDone(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].speedCtrlInitDone;
}


uint16_t ServoData_GetStepDelayTicksCnt(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFFFF;
	else
		return servoArr[ServoNo].stepDelayTicksCnt;
}

uint16_t ServoData_GetStepDelayTicksToggle(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFFFF;
	else
		return servoArr[ServoNo].stepDelayTicksToggle;
}

uint8_t ServoData_GetStepCtrlInitDone(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].stepCtrlInitDone;
}



//// SETTER	////
uint8_t ServoData_SetMotionCtrlState(uint8_t ServoNo, uint8_t State)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].servoMotionCtrl_State = State;
		return TRUE;
	}
}


uint8_t ServoData_SetMoveRequest(uint8_t ServoNo, uint8_t MoveReq)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].moveRequest = MoveReq;
		return TRUE;
	}
}

uint8_t ServoData_SetTargetPosition(uint8_t ServoNo, uint16_t TargetPosi)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].targetPosition = TargetPosi;
		return TRUE;
	}
}

uint8_t ServoData_SetTargetSpeed(uint8_t ServoNo, uint8_t TargetSpeed)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].targetSpeed= TargetSpeed;
		return TRUE;
	}
}

uint8_t ServoData_SetAcceleration(uint8_t ServoNo, uint8_t Acceleration)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].acceleration = Acceleration;
		return TRUE;
	}
}

uint8_t ServoData_SetDeceleration(uint8_t ServoNo, uint8_t Deceleration)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].deceleration = Deceleration;
		return TRUE;
	}
}

uint8_t ServoData_SetCurrentSpeed(uint8_t ServoNo, uint8_t CurrentSpeed)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].currentSpeed = CurrentSpeed;
		return TRUE;
	}
}


uint8_t ServoData_SetMotionPhase(uint8_t ServoNo, uint8_t MotionPhase)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].motionPhase = MotionPhase;
		return TRUE;
	}
}


uint8_t ServoData_SetSpeedDelayTicksCnt(uint8_t ServoNo, uint16_t SpeedDelayTicksCnt)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].speedDelayTicksCnt = SpeedDelayTicksCnt;
		return TRUE;
	}
}

uint8_t ServoData_SetSpeedDelayTicksToggle(uint8_t ServoNo, uint16_t SpeedDelayTicksToggle)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].speedDelayTicksToggle = SpeedDelayTicksToggle;
		return TRUE;
	}
}

uint8_t ServoData_SetSpeedCtrlInitDone(uint8_t ServoNo, uint8_t SpeedCtrlInitDone)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].speedCtrlInitDone = SpeedCtrlInitDone;
		return TRUE;
	}
}


uint8_t ServoData_SetStepDelayTicksCnt(uint8_t ServoNo, uint16_t StepDelayTicksCnt)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].stepDelayTicksCnt = StepDelayTicksCnt;
		return TRUE;
	}
}

uint8_t ServoData_SetStepDelayTicksToggle(uint8_t ServoNo, uint16_t StepDelayTicksToggle)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].speedDelayTicksToggle = StepDelayTicksToggle;
		return TRUE;
	}
}

uint8_t ServoData_SetStepCtrlInitDone(uint8_t ServoNo, uint8_t StepCtrlInitDone)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].stepCtrlInitDone = StepCtrlInitDone;
		return TRUE;
	}
}

////	Postion control	////
uint8_t ServoData_IncrementPoistion(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
	return FALSE;
	else
	{
		servoArr[ServoNo].currentPosition++;
		return TRUE;
	}
}

uint8_t ServoData_DecrementPoistion(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
	return FALSE;
	else
	{
		servoArr[ServoNo].currentPosition--;
		return TRUE;
	}
}
