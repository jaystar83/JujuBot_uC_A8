/*
 * servoCtrl.c
 *
 * Created: 20.03.2021 16:16:04
 *  Author: jayst
 to do:

- speedCtrl_3:
	- mit Beschleunigung
	- es muss, wie auch vorher, geprüft werden, wann einen schritt weoter grschaltet werden darf
	- s = a/2 t^2 + v_0 * t + s_0
		- s_0 irrelevant
		- v_0 * t : geschwindigkeits steuerung wie gehabt
		- a/2 * t^2 : weg durch beschleunigung 
		- so lange bis v_0 + a*t == v_soll 
		- dann ohne beschleu. mit v_soll
		- a * t : linear -> zykl. addition bei gleicher cycle time -> array
		 
		
 */ 

/***********************************************************************
	ICNLUDES
***********************************************************************/
#include "servoCtrl.h"
#include "auxiliaryFunctions.h"

/***********************************************************************
	LOCAL VARS
***********************************************************************/
servoData servoArr[SERVO_CNT] = {0};

// Vorgabe der Tick-Zahl {0, 800 -> 40ms, ...                                                                                  1,1ms, 1,1ms, 1.05ms, 1.05ms, 1ms};
uint16_t veloTicksDefault[] = {0,800,400,267,200,160,133,114,100,89,80,73,67,62,57,53,50,47,44,42,40,38,36,35,33,32,31,30,29,28,27,26,25,24,24,23,22,22,21,21,20};

uint8_t EmergencyStop_Request = 0;
/***********************************************************************
	LOCAL FUNCTIONS DEFINITION
***********************************************************************/
uint8_t speedCtrl(uint8_t ServoNo, uint8_t ServoSpeed);
uint8_t speedCtrl_2(uint8_t ServoNo, uint8_t ServoSpeed, uint8_t Ticks);	
uint8_t servoCtrlGoTo_2(uint8_t ServoNo, uint8_t Ticks);
uint8_t motionCtrl(uint8_t ServoNo, uint8_t Ticks);

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
		
		servoArr[i].servoCtrlGoTo_State= 0;	// 0: Init, 1: to Max, 2: to Min, 3: Emergeny stop,  other -> End

		
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

// Runs in 1ms Task to check if Servo needs to run
void SercoCtrl_CheckMoveRequest()
{
	for(uint8_t i=0; i<SERVO_CNT; i++)
	{
		if(servoArr[i].currentPosition != servoArr[i].targetPosition)
			servoArr[i].moveRequest = TRUE;
	}
}

// Runs in Idle Task to ctrl servo in 50us steps
void ServoCtrl(uint8_t IsrCnt)
{
	for(uint8_t i=0; i<SERVO_CNT; i++)
	{
		if(servoArr[i].moveRequest)
			servoCtrlGoTo_2(i, IsrCnt);
	}
}

uint8_t ServoData_GetServoNo(uint8_t ServoNo)
{
	if(ServoNo >= SERVO_CNT)
		return 0xFF;
	else
		return servoArr[ServoNo].servoNo;
}

uint8_t ServoData_GetMoveRequestStatus(uint8_t ServoNo)
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

uint8_t ServoData_SetMoveRequestStatus(uint8_t ServoNo, uint8_t MoveReq)
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

uint8_t ServoData_SetDeceleratiom(uint8_t ServoNo, uint8_t Deceleration)
{
	if(ServoNo >= SERVO_CNT)
		return FALSE;
	else
	{
		servoArr[ServoNo].deceleration = Deceleration;
		return TRUE;
	}
}


/***********************************************************************
	LOCAL FUNCTIONS IMPLEMENTATION
***********************************************************************/
////////////////////////////////////////////////////////////////////////////////////
uint8_t servoCtrlGoTo(uint8_t ServoNo)
{
	if(servoArr[ServoNo].currentPosition > servoArr[ServoNo].targetPosition)
	{
		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		if( speedCtrl(ServoNo, servoArr[ServoNo].currentSpeed) )
		{
			servoArr[ServoNo].currentPosition--;
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
				Servo(ServoNo, servoArr[ServoNo].currentPosition);
			#endif
		}
		
		return 0;
	}
	else if(servoArr[ServoNo].currentPosition < servoArr[ServoNo].targetPosition)
	{
		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		if( speedCtrl(ServoNo,servoArr[ServoNo].currentSpeed) )
		{
			servoArr[ServoNo].currentPosition++;
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
				Servo(ServoNo, servoArr[ServoNo].currentPosition);
			#endif
		}
		
		return 0;
	}
	
	servoArr[ServoNo].currentSpeed = 0;
	return 1;
}

////////////////////////////////////////////////////////////////////////////////////
uint8_t servoCtrlGoTo_2(uint8_t ServoNo, uint8_t Ticks)
{
	if(servoArr[ServoNo].currentPosition > servoArr[ServoNo].targetPosition)
	{
		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		if( speedCtrl_2(ServoNo,servoArr[ServoNo].currentSpeed, Ticks) )
		{
			servoArr[ServoNo].currentPosition--;
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
			Servo(ServoNo, servoArr[ServoNo].currentPosition);
			#endif
		}
		
		return TRUE;
	}
	else if(servoArr[ServoNo].currentPosition < servoArr[ServoNo].targetPosition)
	{
		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		if( speedCtrl_2(ServoNo, servoArr[ServoNo].currentSpeed, Ticks) )
		{
			servoArr[ServoNo].currentPosition++;
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
			Servo(ServoNo, servoArr[ServoNo].currentPosition);
			#endif
		}
		
		return TRUE;
	}
	
	servoArr[ServoNo].currentSpeed = 0;
	servoArr[ServoNo].moveRequest = FALSE;
	servoArr[ServoNo].speedCtrlInitDone = FALSE;
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
/*
	- neue Funktion *_3
	- bewegungsrichtung muss gespeichert werden
	- bewegungsumkehr erst nach Verzögerung auf 0
*/
uint8_t servoCtrlGoTo_3(uint8_t ServoNo, uint8_t Ticks)
{
	// 0: Init, 1: to Max, 2: to Min, 3: Stop due to reversal of movement, 0xFE: Emergeny stop,  other: End
	if(EmergencyStop_Request)
		servoArr[ServoNo].servoCtrlGoTo_State = EMERGENCY_STOP;
		
	switch (servoArr[ServoNo].servoCtrlGoTo_State)
	{
	////	Init	////////////////////////////////////////////////////////////////
	case MOTION_INIT:
		servoArr[ServoNo].speedCtrlInitDone = FALSE;
		
		
		if(servoArr[ServoNo].currentPosition < servoArr[ServoNo].targetPosition)
			servoArr[ServoNo].servoCtrlGoTo_State = GO_TO_MAX;			// Go to MAX
			
		else if(servoArr[ServoNo].currentPosition > servoArr[ServoNo].targetPosition)
			servoArr[ServoNo].servoCtrlGoTo_State = GO_TO_MAX;			// Go to MIN
		
		else
			servoArr[ServoNo].servoCtrlGoTo_State = MOTION_FINISHED;	// END
		
		break;

	////	Go to MAX	////////////////////////////////////////////////////////////
	case GO_TO_MAX:
	/*
		w = alpha * t + w_0
		s = alpha/2 * t^2 + w_0 * t + s_0
		
		s = s(alpha,t) + s(w_0,t) + s_0
		
	*/
	
	
		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		servoArr[ServoNo].servoCtrlGoTo_State = motionCtrl(ServoNo, Ticks);
			
		if( speedCtrl_2(ServoNo, servoArr[ServoNo].startSpeed, Ticks) )
		{
			servoArr[ServoNo].currentPosition++;
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
				Servo(ServoNo, servoArr[ServoNo].currentPosition);
			#endif
		}
		break;

	////	Go to MIN	////////////////////////////////////////////////////////////
	case GO_TO_MIN:
	
		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		if( speedCtrl_2(ServoNo,servoArr[ServoNo].currentSpeed, Ticks) )
		{
			servoArr[ServoNo].currentPosition--;
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
			Servo(ServoNo, servoArr[ServoNo].currentPosition);
			#endif
		}
		break;

	////	STOP due to reversal of movement	////////////////////////////////////
	case GO_TO_REVERT:
	
		break;

	////	EMERGENCY STOP	////////////////////////////////////////////////////////
	case EMERGENCY_STOP:
	
		break;

	////	END		////////////////////////////////////////////////////////////////
	default:
		servoArr[ServoNo].currentSpeed = 0;
		servoArr[ServoNo].moveRequest = FALSE;
		servoArr[ServoNo].speedCtrlInitDone = FALSE;
		servoArr[ServoNo].servoCtrlGoTo_State = 0;
		return FALSE;
	}

	return TRUE;

	//////////////////////////////////////////////////////////////////////////////////
	// BACKUP
	////
	if(servoArr[ServoNo].currentPosition > servoArr[ServoNo].targetPosition)
	{
		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		if( speedCtrl_2(ServoNo,servoArr[ServoNo].currentSpeed, Ticks) )
		{
			servoArr[ServoNo].currentPosition--;
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
			Servo(ServoNo, servoArr[ServoNo].currentPosition);
			#endif
		}
		
		return TRUE;
	}
	else if(servoArr[ServoNo].currentPosition < servoArr[ServoNo].targetPosition)
	{
		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		if( speedCtrl_2(ServoNo, servoArr[ServoNo].currentSpeed, Ticks) )
		{
			servoArr[ServoNo].currentPosition++;
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
			Servo(ServoNo, servoArr[ServoNo].currentPosition);
			#endif
		}
		
		return TRUE;
	}
	
	servoArr[ServoNo].currentSpeed = 0;
	servoArr[ServoNo].moveRequest = FALSE;
	servoArr[ServoNo].speedCtrlInitDone = FALSE;
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////
uint8_t speedCtrl(uint8_t ServoNo, uint8_t ServoSpeed)
{
	if(	++(servoArr[ServoNo].stepDelayTicksCnt) >= (SPEED_MAX - ServoSpeed) )
	{
		servoArr[ServoNo].stepDelayTicksCnt = 0;
		return TRUE;
	}
	
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
uint8_t speedCtrl_2(uint8_t ServoNo, uint8_t ServoSpeed, uint8_t Ticks)
{
	// Init SpeedCtrl	////////////////////////////////////////////////////
	if(!servoArr[ServoNo].speedCtrlInitDone)
	{
		servoArr[ServoNo].speedCtrlInitDone = TRUE;
		servoArr[ServoNo].stepDelayTicksToggle = Ticks;
		servoArr[ServoNo].stepDelayTicksCnt = 0;
	}

	// Check if Ticks increased	////////////////////////////////////////////
	if(servoArr[ServoNo].stepDelayTicksToggle != Ticks)
	{
		// Ticks overflow
		if(servoArr[ServoNo].stepDelayTicksToggle > Ticks)
			servoArr[ServoNo].stepDelayTicksCnt += (255 - servoArr[ServoNo].stepDelayTicksToggle) + Ticks;
		// No overflow
		else
			servoArr[ServoNo].stepDelayTicksCnt += Ticks - servoArr[ServoNo].stepDelayTicksToggle;
		
		servoArr[ServoNo].stepDelayTicksToggle = Ticks;
	}
	
	// veloTicksCntr reachs default value	////////////////////////////////////
	if(servoArr[ServoNo].stepDelayTicksCnt >= veloTicksDefault[ServoSpeed])
	{
		// Save passed Ticks for next cycle
		servoArr[ServoNo].stepDelayTicksCnt -= veloTicksDefault[ServoSpeed];
		return TRUE;
	}
	
	return FALSE;
}
