/*
 * servoCtrl.c
 *
 * Created: 20.03.2021 16:16:04
 *  Author: jayst
 to do:

- stepCtrl_3:
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
#include "servoMotionCtrl.h"
#include "servoDataCtrl.h"
#include "auxiliaryFunctions.h"

/***********************************************************************
	LOCAL VARS
***********************************************************************/
// Vorgabe der Tick-Zahl {0, 800 -> 40ms, ...                                                                                  1,1ms, 1,1ms, 1.05ms, 1.05ms, 1ms};
uint16_t veloTicksDefault[] = {0,800,400,267,200,160,133,114,100,89,80,73,67,62,57,53,50,47,44,42,40,38,36,35,33,32,31,30,29,28,27,26,25,24,24,23,22,22,21,21,20};

uint16_t stepsToStop[5][40] = {	{1,4,8,13,20,29,39,50,64,79,95,113,133,154,176,200,226,254,283,313,345,379,414,450,489,529,570,613,658,703,751,800,851,904,958,1013,1070,1129,1189,1250},
								{1,2,3,5,8,12,16,20,26,32,38,45,53,62,71,80,91,102,113,125,138,152,166,180,196,212,228,245,263,282,301,320,341,362,383,405,428,452,476,500},
								{1,1,2,3,4,6,8,10,13,16,19,23,27,31,36,40,46,51,57,63,69,76,83,90,98,106,114,123,132,141,151,160,171,181,192,203,214,226,238,250},
								{1,1,1,1,2,3,4,4,6,7,8,9,11,13,15,16,19,21,23,25,28,31,34,36,40,43,46,49,53,57,61,64,69,73,77,81,86,91,96,100},
								{1,1,1,1,1,2,2,2,3,4,4,5,6,7,8,8,10,11,12,13,14,16,17,18,20,22,23,25,27,29,31,32,35,37,39,41,43,46,48,50}
							  };

// Counter of 50us ticks to increment / decrement speed
// st/ms^2 :                    0, 0.0004, 0,001, 0.002, 0.005, 0.01
uint16_t accDecSpeedIncTickCnt[] = {0, 1250, 500, 250, 100, 50};

uint8_t EmergencyStop_Request = 0;
/***********************************************************************
	LOCAL FUNCTIONS DEFINITION
***********************************************************************/
uint8_t stepCtrl(uint8_t ServoNo, uint8_t Ticks);
//uint8_t stepCtrl_Old(uint8_t ServoNo, uint8_t ServoSpeed, uint8_t Ticks);	

uint8_t speedControl(uint8_t ServoNo, uint8_t Ticks);

uint8_t servoCtrlGoTo(uint8_t ServoNo, uint8_t Ticks);
uint8_t servoCtrlGoTo_3(uint8_t ServoNo, uint8_t Ticks);

uint8_t motionInit(uint8_t ServoNo);
uint8_t motionCtrl(uint8_t ServoNo, uint8_t Ticks);


/***********************************************************************
	GLOBAL FUNCTIONS IMPLEMENTATION
***********************************************************************/
// Runs in 1ms Task to check if Servo needs to run
void SercoCtrl_CheckMoveRequest()
{
	for(uint8_t i=0; i<SERVO_CNT; i++)
	{
		if( ServoData_GetCurrentPosition(i) != ServoData_GetTargetPosition(i) )
			ServoData_SetMoveRequest(i, TRUE);
	}
}

// Runs in Idle Task to ctrl servo in 50us steps
void ServoCtrl(uint8_t IsrCnt)
{
	for(uint8_t i=0; i<SERVO_CNT; i++)
	{
		if( ServoData_GetMoveRequest(i) )
//			servoCtrlGoTo(i, IsrCnt);
			servoCtrlGoTo_3(i, IsrCnt);
	}
}

/***********************************************************************
	LOCAL FUNCTIONS IMPLEMENTATION
***********************************************************************/
////////////////////////////////////////////////////////////////////////////////////
uint8_t servoCtrlGoTo(uint8_t ServoNo, uint8_t Ticks)
{
	if( ServoData_GetCurrentPosition(ServoNo) > ServoData_GetTargetPosition(ServoNo) )
	{
		ServoData_SetCurrentSpeed(ServoNo, ServoData_GetTargetSpeed(ServoNo));
//		if( stepCtrl_Old(ServoNo, ServoData_GetCurrentSpeed(ServoNo), Ticks) )
		if( stepCtrl(ServoNo, Ticks) )
		{
			ServoData_DecrementPoistion(ServoNo);
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
			Servo(ServoNo, ServoData_GetCurrentPosition(ServoNo));
			#endif
		}
		
		return TRUE;
	}
	else if( ServoData_GetCurrentPosition(ServoNo) < ServoData_GetTargetPosition(ServoNo) )
	{
		ServoData_SetCurrentSpeed(ServoNo, ServoData_GetTargetSpeed(ServoNo));
//		if( stepCtrl_Old(ServoNo, ServoData_GetCurrentSpeed(ServoNo), Ticks) )
		if( stepCtrl(ServoNo, Ticks) )
		{
			ServoData_IncrementPoistion(ServoNo);
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
			Servo(ServoNo, ServoData_GetCurrentPosition(ServoNo));
			#endif
		}
		
		return TRUE;
	}
	
	ServoData_SetCurrentSpeed(ServoNo, 0);
	ServoData_SetMoveRequest(ServoNo, FALSE);
	ServoData_SetSpeedCtrlInitDone(ServoNo, FALSE);
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////
uint8_t servoCtrlGoTo_3(uint8_t ServoNo, uint8_t Ticks)
{
	if(EmergencyStop_Request)
		ServoData_SetMotionCtrlState(ServoNo, EMERGENCY_STOP);

	// 0: Init, 1: to Max, 2: to Min, 3: Stop due to reversal of movement, 0xFE: Emergeny stop,  other: End		
	switch( ServoData_GetMotionCtrlState(ServoNo) )
	{
	////	Init	////////////////////////////////////////////////////////////////
	case MOTION_INIT:
		ServoData_SetMotionCtrlState( ServoNo, motionInit(ServoNo) );
		break;

	////	Go to MAX	////////////////////////////////////////////////////////////
	case GO_TO_MAX:
	/*
		w = alpha * t + w_0
		s = alpha/2 * t^2 + w_0 * t + s_0
		s = s(alpha,t) + s(w_0,t) + s_0
	*/	
		if(ServoData_GetCurrentPosition(ServoNo) == ServoData_GetTargetPosition(ServoNo) )
		{
			ServoData_SetMotionCtrlState(ServoNo, MOTION_FINISHED );
		}
		else
		{
			// checking acc, position to start dec and calculating dec, if received value not valid
			ServoData_SetMotionCtrlState(ServoNo, motionCtrl(ServoNo, Ticks) );

			// calculating speed depending on motionPhase -> accelaration / constant / decelaration 
			ServoData_SetCurrentSpeed(ServoNo, speedControl(ServoNo, Ticks) );		
		
			// waiting for next Step depwnding on calculated speed
			if( stepCtrl(ServoNo, Ticks) )
			{
				ServoData_IncrementPoistion(ServoNo);
				#if( SERVEO_CTRL_BOARD_AVAILABLE )
					Servo(ServoNo, ServoData_GetCurrentPosition(ServoNo) );
				#endif
			}
		}
		break;
		
	////	Go to MIN	////////////////////////////////////////////////////////////
	case GO_TO_MIN:
	
		if(ServoData_GetCurrentPosition(ServoNo) == ServoData_GetTargetPosition(ServoNo) )
		{
			ServoData_SetMotionCtrlState(ServoNo, MOTION_FINISHED );
		}
		else
		{
			// checking acc, position to start dec, calculating dec, if received value not valid
			ServoData_SetMotionCtrlState(ServoNo, motionCtrl(ServoNo, Ticks) );

			// calculating speed depending on acc / decelaration
			ServoData_SetCurrentSpeed(ServoNo, speedControl(ServoNo, Ticks));
		
			// waiting for next Step depwnding on calculated speed
			if( stepCtrl(ServoNo, Ticks) )
			{
				ServoData_DecrementPoistion(ServoNo);
				#if( SERVEO_CTRL_BOARD_AVAILABLE )
				Servo(ServoNo, ServoData_GetCurrentPosition(ServoNo) );
				#endif
			}
		}
		break;

	////	EMERGENCY STOP	////////////////////////////////////////////////////////
	case EMERGENCY_STOP:
		ServoData_SetCurrentSpeed(ServoNo, 0);
		ServoData_SetSpeedCtrlInitDone(ServoNo, FALSE);
		ServoData_SetStepCtrlInitDone(ServoNo, FALSE);
		ServoData_SetMotionCtrlState(ServoNo, MOTION_INIT);
		ServoData_SetMotionPhase(ServoNo, MoPha_OFF);
		break;

	////	END		////////////////////////////////////////////////////////////////
	default:
		ServoData_SetCurrentSpeed(ServoNo, 0);
		ServoData_SetMoveRequest(ServoNo, FALSE);
		ServoData_SetSpeedCtrlInitDone(ServoNo, FALSE);
		ServoData_SetStepCtrlInitDone(ServoNo, FALSE);
		ServoData_SetMotionCtrlState(ServoNo, MOTION_INIT);
		ServoData_SetMotionPhase(ServoNo, MoPha_OFF);
		return FALSE;
	}

	return TRUE;

}

////////////////////////////////////////////////////////////////////////////////////
uint8_t motionInit(uint8_t ServoNo)
{
	ServoData_SetStepCtrlInitDone( ServoNo, FALSE );
	ServoData_SetSpeedCtrlInitDone( ServoNo, FALSE );
	
	// Checking servoCtrlData
	if( ServoData_GetAcceleration( ServoNo ) > ACC_DEC_MAX )
		ServoData_SetAcceleration( ServoNo, ACC_DEC_MAX );
		
	if( ServoData_GetDecelaration( ServoNo ) > ACC_DEC_MAX )
		ServoData_SetDeceleration( ServoNo, ACC_DEC_MAX );

	if( ServoData_GetTargetSpeed( ServoNo ) > SPEED_MAX )
		ServoData_SetTargetSpeed( ServoNo, SPEED_MAX );

	uint8_t retVal = 0;
	
	// 	Checking the moving direction
	if( ServoData_GetCurrentPosition(ServoNo) < ServoData_GetTargetPosition(ServoNo) )
	{
		retVal = GO_TO_MAX;											////	Go to MAX
		ServoData_SetMotionPhase(ServoNo, MoPha_AccToTargetSpeed);	// Starting with acceleration phase
	}
	else if( ServoData_GetCurrentPosition(ServoNo) > ServoData_GetTargetPosition(ServoNo) )
	{
		retVal = GO_TO_MIN;											////	Go to MIN
		ServoData_SetMotionPhase(ServoNo, MoPha_AccToTargetSpeed);	// Starting with acceleration phase
	}
	else
		retVal = MOTION_FINISHED;	////	END
				
	return retVal;
}

uint8_t motionCtrl(uint8_t ServoNo, uint8_t Ticks)
{
	// 1. Checking declaration and breaking distance
	uint8_t RetVal = 0;

	servoData tmpServoData;
	ServoData_GetServoData(ServoNo, &tmpServoData);
	
	uint16_t stepsForStop = stepsToStop[ ServoData_GetDecelaration(ServoNo)-1 ][ ServoData_GetCurrentSpeed(ServoNo)-1 ];
	
/*	uint16_t currentPosi = ServoData_GetCurrentPosition(ServoNo);
	uint16_t targetPosi = ServoData_GetTargetPosition(ServoNo);
	
	uint8_t currentSpeed = ServoData_GetCurrentSpeed(ServoNo);
	uint8_t targetSpeed = ServoData_GetTargetSpeed(ServoNo);
	
	uint16_t stepsForStop = stepsToStop[ ServoData_GetDecelaration(ServoNo)-1 ][ ServoData_GetCurrentSpeed(ServoNo)-1 ];
	uint8_t accelaration = ServoData_GetAcceleration(ServoNo);
	uint8_t decelaration = ServoData_GetDecelaration(ServoNo);
	
	uint8_t motionCtrlState = ServoData_GetMotionCtrlState( ServoNo );
	uint8_t motionPhase = ServoData_GetMotionPhase( ServoNo );
*/	
	switch( tmpServoData.motionPhase )
	{
	/***	Phase 1 - Accelaration	***/
	case MoPha_AccToTargetSpeed:
		/* 1) if Acc == 0 -> switch direct to Phase 2 without Acc-Phase */
		if( tmpServoData.acceleration == 0 )		// 
		{
			ServoData_SetMotionPhase( ServoNo, MoPha_UniformMoving );	// Uniform movement
			RetVal = tmpServoData.servoMotionCtrl_State;	// No change of motionCtrlState
		}
		/* 2) Moving direction to Servo Max Position */
		else if( GO_TO_MAX == tmpServoData.servoMotionCtrl_State )
		{	
			/* a) if the direction changes	*/
			if ( tmpServoData.currentPosition > tmpServoData.targetPosition ) 
			{
				if( (tmpServoData.deceleration != 0) && (tmpServoData.currentSpeed > 0) )
				{	
					ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase before changing direction
					RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State	
				}
				else // no decelaration -> directly go on into other direction
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
					RetVal = GO_TO_MIN;		// Changing direction
				}
			}
			/* b) if requiredBrakingDistance is reached before reaching targetSpeed */
			else if( (tmpServoData.deceleration != 0) && (tmpServoData.currentPosition >= tmpServoData.targetPosition - stepsForStop) )
			{
				ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase 
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}

		/* 3) Moving direction to Servo Min Position */
		else if( GO_TO_MIN == tmpServoData.servoMotionCtrl_State )
		{
			/* a) if the direction changes	*/
			if ( tmpServoData.currentPosition < tmpServoData.targetPosition )
			{
				if( tmpServoData.deceleration != 0 )
				{
					if ( tmpServoData.currentSpeed > 0)
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase before changing direction
						RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
					}
					else
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
						RetVal = GO_TO_MAX;		// Changing direction
					}
				}
				else // no decelaration -> directly go on into other direction
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
					RetVal = GO_TO_MAX;		// Changing direction
				}
			}
			/* b) if requiredBrakingDistance is reached before reaching targetSpeed */
			else if( (tmpServoData.deceleration != 0) && (tmpServoData.currentPosition <= tmpServoData.targetPosition + stepsForStop) )
			{
				ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}
		/* 4. independent of moving direction */
		else
		{			
			/* a) target speed is reached */
			if( tmpServoData.currentSpeed == tmpServoData.targetSpeed )
			{
				ServoData_SetMotionPhase(ServoNo, MoPha_UniformMoving);	// Uniform movement
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
			/* b) currentSpeed > targetSpeed */
			else if( tmpServoData.currentSpeed > tmpServoData.targetSpeed )
			{
				ServoData_SetMotionPhase(ServoNo, MoPha_DecToTargetSpeed);	// dec because of chmaged target speed
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
					
			}
			/* c) accelaration progress */
			else
			{
				// nicht nötig	ServoData_SetMotionPhase(ServoNo, MoPha_ACC);	// going on with ACC
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}
		
		break;
	
	/***	Phase 2 - Uniform movement	***/
	case MoPha_UniformMoving:
	
		/* 1) Moving direction to Servo Max Position */
		if( GO_TO_MAX == tmpServoData.servoMotionCtrl_State )
		{
			/* a) if the direction changes	*/
			if ( tmpServoData.currentPosition > tmpServoData.targetPosition )
			{
				if( tmpServoData.deceleration != 0 )
				{
					if ( tmpServoData.currentSpeed > 0)
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase before changing direction
						RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
					}
					else
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
						RetVal = GO_TO_MIN;		// Changing direction
					}
				}
				else // no decelaration -> directly go on into other direction
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
					RetVal = GO_TO_MIN;		// Changing direction
				}
			}
			/* b) if requiredBrakingDistance is reached before reaching targetSpeed */
			else if( (tmpServoData.deceleration != 0) && (tmpServoData.currentPosition >= tmpServoData.targetPosition - stepsForStop) )
			{
				ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}
		/* 2) Moving direction to Servo Min Position */
		else if( GO_TO_MIN == tmpServoData.servoMotionCtrl_State )
		{
			/* a) if the direction changes	*/
			if ( tmpServoData.currentPosition < tmpServoData.targetPosition )
			{
				if( tmpServoData.deceleration != 0 )
				{
					if ( tmpServoData.currentSpeed > 0)
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase before changing direction
						RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
					}
					else
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
						RetVal = GO_TO_MAX;		// Changing direction
					}
				}
				else // no decelaration -> directly go on into other direction
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
					RetVal = GO_TO_MAX;		// Changing direction
				}
			}
			/* b) if requiredBrakingDistance is reached before reaching targetSpeed */
			else if( (tmpServoData.deceleration != 0) && (tmpServoData.currentPosition <= tmpServoData.targetPosition + stepsForStop) )
			{
				ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}

		}
		/* 3. independent of moving direction */
		else
		{
			/* a) currentSpeed > targetSpeed */
			if( tmpServoData.currentSpeed > tmpServoData.targetSpeed )
			{
				ServoData_SetMotionPhase(ServoNo, MoPha_DecToTargetSpeed);	// dec because of chmaged target speed
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
			/* b) currentSpeed < targetSpeed */
			else if( tmpServoData.currentSpeed < tmpServoData.targetSpeed )
			{
				ServoData_SetMotionPhase(ServoNo, MoPha_AccToTargetSpeed);	// dec because of chmaged target speed
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State			
			}
			/* c) uniform moving in progress */
			else
			{
				// nicht nötig	ServoData_SetMotionPhase(ServoNo, MoPha_ACC);	// going on with ACC
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}
		break;

	/***	Phase 3 - Deceleration to target Speed	***/
	case MoPha_DecToTargetSpeed:
		/* 1) Moving direction to Servo Max Position */
		if( GO_TO_MAX == tmpServoData.servoMotionCtrl_State )
		{
			/* a) if the direction changes	*/
			if ( tmpServoData.currentPosition > tmpServoData.targetPosition )
			{
				if( (tmpServoData.deceleration != 0) && (tmpServoData.currentSpeed > 0) )
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase before changing direction
					RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
				}
				else // no decelaration -> directly go on into other direction
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
					RetVal = GO_TO_MIN;		// Changing direction
				}
			}
			/* b) if requiredBrakingDistance is reached before reaching targetSpeed */
			else if( (tmpServoData.deceleration != 0) && (tmpServoData.currentPosition >= tmpServoData.targetPosition - stepsForStop) )
			{
				ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}
		/* 2) Moving direction to Servo Min Position */
		else if( GO_TO_MIN == tmpServoData.servoMotionCtrl_State )
		{
			/* a) if the direction changes	*/
			if ( tmpServoData.currentPosition > tmpServoData.targetPosition )
			{
				if( (tmpServoData.deceleration != 0) && (tmpServoData.currentSpeed > 0) )
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase before changing direction
					RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
				}
				else // no decelaration -> directly go on into other direction
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
					RetVal = GO_TO_MAX;		// Changing direction
				}
			}
			/* b) if requiredBrakingDistance is reached before reaching targetSpeed */
			else if( (tmpServoData.deceleration != 0) && (tmpServoData.currentPosition >= tmpServoData.targetPosition - stepsForStop) )
			{
				ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}
		/* 3. independent of moving direction */
		else
		{
			/* a) currentSpeed < targetSpeed */
			if( tmpServoData.currentSpeed < tmpServoData.targetSpeed )
			{
				ServoData_SetMotionPhase(ServoNo, MoPha_AccToTargetSpeed);	// dec because of chmaged target speed
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
			/* b) currentSpeed > targetSpeed */
			else if( tmpServoData.currentSpeed > tmpServoData.targetSpeed)
			{
				// not needed ServoData_SetMotionPhase(ServoNo, MoPha_DecToTargetSpeed);	// dec because of chmaged target speed
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State			
			}
			/* c) uniform moving in progress */
			else
			{
				ServoData_SetMotionPhase(ServoNo, MoPha_UniformMoving);	// going on with uniform movement
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}
		break;
		
	/***	Phase 4 - Deceleration to Stop	***/
	case MoPha_DecToStop:
		/* 1) Moving direction to Servo Max Position */
		if( GO_TO_MAX == tmpServoData.servoMotionCtrl_State )
		{
			/* a) if the direction changes	*/
			if ( tmpServoData.currentPosition < tmpServoData.targetPosition )
			{
				if( tmpServoData.deceleration != 0 )
				{
					if ( tmpServoData.currentSpeed > 0)
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase before changing direction
						RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
					}
					else
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
						RetVal = GO_TO_MIN;		// Changing direction
					}
				}
				else // no decelaration -> directly go on into other direction
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
					RetVal = GO_TO_MIN;		// Changing direction
				}
			}
			/* b) if requiredBrakingDistance is reached before reaching targetSpeed */
			else if( (tmpServoData.deceleration != 0) && (tmpServoData.currentPosition <= tmpServoData.targetPosition + stepsForStop) )
			{
				ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}
		/* 2) Moving direction to Servo Min Position */
		else if( GO_TO_MIN == tmpServoData.servoMotionCtrl_State )
		{
			/* a) if the direction changes	*/
			if ( tmpServoData.currentPosition < tmpServoData.targetPosition )
			{
				if( tmpServoData.deceleration != 0 )
				{
					if ( tmpServoData.currentSpeed > 0)
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase before changing direction
						RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
					}
					else
					{
						ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
						RetVal = GO_TO_MAX;		// Changing direction
					}
				}
				else // no decelaration -> directly go on into other direction
				{
					ServoData_SetMotionPhase( ServoNo, MoPha_AccToTargetSpeed );	// Changing direction, starting with Acceleration Phase
					RetVal = GO_TO_MAX;		// Changing direction
				}
			}
			/* b) if requiredBrakingDistance is reached before reaching targetSpeed */
			else if( (tmpServoData.deceleration != 0) && (tmpServoData.currentPosition <= tmpServoData.targetPosition + stepsForStop) )
			{
				ServoData_SetMotionPhase( ServoNo, MoPha_DecToStop );	// Breaking Phase
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}
		/* 3. independent of moving direction */
		else
		{
			/* a) currentSpeed > targetSpeed */
			if( tmpServoData.currentSpeed > tmpServoData.targetSpeed )
			{
				ServoData_SetMotionPhase(ServoNo, MoPha_DecToTargetSpeed);	// dec because of chmaged target speed
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
			/* b) uniform moving in progress */
			else
			{
				// nicht nötig	ServoData_SetMotionPhase(ServoNo, MoPha_ACC);	// going on with ACC
				RetVal = tmpServoData.servoMotionCtrl_State;	// No change of tmpServoData.servoMotionCtrl_State
			}
		}	
		break;

	/***	Phase XX - Motion control OFF	***/
	default:
		ServoData_SetMotionPhase(ServoNo, MoPha_OFF);
	}

	return RetVal;
}

uint8_t speedControl(uint8_t ServoNo, uint8_t Ticks)
{
	servoData tmpServoData;
	ServoData_GetServoData(ServoNo, &tmpServoData);

	uint8_t retVal = tmpServoData.currentSpeed;

	// Init stepCtrl	////////////////////////////////////////////////////
	if( tmpServoData.speedCtrlInitDone == FALSE )
	{
		ServoData_SetSpeedCtrlInitDone(ServoNo, TRUE);
		ServoData_SetSpeedDelayTicksToggle(ServoNo, Ticks);
		ServoData_SetSpeedDelayTicksCnt(ServoNo, 0);
	}

	// Check if Ticks increased	////////////////////////////////////////////
	if( tmpServoData.speedDelayTicksToggle != Ticks )
	{
		// Ticks overflow
		if( tmpServoData.speedDelayTicksToggle > Ticks )
		{
			ServoData_SetSpeedDelayTicksCnt( ServoNo, tmpServoData.speedDelayTicksCnt + (0xFF - tmpServoData.speedDelayTicksToggle) + Ticks );
			//servoArr[ServoNo].stepDelayTicksCnt += (255 - servoArr[ServoNo].stepDelayTicksToggle) + Ticks;
		}
		// No overflowveloTicksDefault
		else
		{
			ServoData_SetSpeedDelayTicksCnt(ServoNo, tmpServoData.speedDelayTicksCnt +  Ticks -  tmpServoData.speedDelayTicksToggle);
			//servoArr[ServoNo].stepDelayTicksCnt += Ticks - servoArr[ServoNo].stepDelayTicksToggle;
		}
		ServoData_SetSpeedDelayTicksToggle(ServoNo, Ticks);
	}
	
	// veloTicksCntr reaches default value	////////////////////////////////////
	if( tmpServoData.speedDelayTicksCnt >=  veloTicksDefault[tmpServoData.currentSpeed] )
	{
		// Save passed Ticks for next cycle
		ServoData_SetSpeedDelayTicksCnt(ServoNo, tmpServoData.speedDelayTicksCnt - veloTicksDefault[tmpServoData.currentSpeed] );
		
		if( MoPha_AccToTargetSpeed == tmpServoData.motionPhase )
		{
			//ServoData_SetCurrentSpeed( ServoNo, ++(tmpServoData.currentSpeed) );
			retVal = ++(tmpServoData.currentSpeed);
		}
		else if( MoPha_DecToTargetSpeed == tmpServoData.motionPhase ||
				 MoPha_DecToStop == tmpServoData.motionPhase
			)
		{
			//ServoData_SetCurrentSpeed( ServoNo, --(tmpServoData.currentSpeed) );
			retVal = --(tmpServoData.currentSpeed);
		}
	}
	
	return retVal;
}

uint8_t stepCtrl(uint8_t ServoNo, uint8_t Ticks)
{
	// Init stepCtrl	////////////////////////////////////////////////////
	if( ServoData_GetStepCtrlInitDone(ServoNo) == FALSE )
	{
		ServoData_SetStepCtrlInitDone(ServoNo, TRUE);
		ServoData_SetStepDelayTicksToggle(ServoNo, Ticks);
		ServoData_SetStepDelayTicksCnt(ServoNo, 0);
	}

	// Check if Ticks increased	////////////////////////////////////////////
	if( ServoData_GetStepDelayTicksToggle(ServoNo) != Ticks )
	{
		// Ticks overflow
		if( ServoData_GetStepDelayTicksToggle(ServoNo) > Ticks )
		{
			ServoData_SetStepDelayTicksCnt(ServoNo, ServoData_GetStepDelayTicksCnt(ServoNo) + (255 - ServoData_GetStepDelayTicksToggle(ServoNo)) + Ticks );
			//servoArr[ServoNo].stepDelayTicksCnt += (255 - servoArr[ServoNo].stepDelayTicksToggle) + Ticks;

		}
		// No overflowveloTicksDefault
		else
		{
			ServoData_SetStepDelayTicksCnt(ServoNo, ServoData_GetStepDelayTicksCnt(ServoNo) + Ticks - ServoData_GetStepDelayTicksToggle(ServoNo) );
			//servoArr[ServoNo].stepDelayTicksCnt += Ticks - servoArr[ServoNo].stepDelayTicksToggle;
		}
		ServoData_SetStepDelayTicksToggle(ServoNo, Ticks);
	}
	
	// veloTicksCntr reaches default value	////////////////////////////////////
	if(ServoData_GetStepDelayTicksCnt(ServoNo) >=  veloTicksDefault[ServoData_GetCurrentSpeed(ServoNo)] )
	{
		// Save passed Ticks for next cycle
		ServoData_SetStepDelayTicksCnt(ServoNo, ServoData_GetStepDelayTicksCnt(ServoNo) - veloTicksDefault[ServoData_GetCurrentSpeed(ServoNo )] );
		return TRUE;
	}
	
	return FALSE;
}

/*
uint8_t stepCtrl_Old(uint8_t ServoNo, uint8_t ServoSpeed, uint8_t Ticks)
{
	// Init stepCtrl	////////////////////////////////////////////////////
	if(!servoArr[ServoNo].stepCtrlInitDone)
	{
		servoArr[ServoNo].stepCtrlInitDone = TRUE;
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
	
	// veloTicksCntr reaches default value	////////////////////////////////////
	if(servoArr[ServoNo].stepDelayTicksCnt >= veloTicksDefault[ServoSpeed])
	{
		// Save passed Ticks for next cycle
		servoArr[ServoNo].stepDelayTicksCnt -= veloTicksDefault[ServoSpeed];
		return TRUE;
	}
	
	return FALSE;
}
*/
