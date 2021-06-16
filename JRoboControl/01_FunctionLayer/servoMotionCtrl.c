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

// Counter of 50us ticks to increment / decrement speed
// st/ms^2 :                    0, 0.0004, 0,001, 0.002, 0.005, 0.01
uint16_t accDecSpeedIncTickCnt[] = {0, 1250, 500, 250, 100, 50};

uint8_t EmergencyStop_Request = 0;
/***********************************************************************
	LOCAL FUNCTIONS DEFINITION
***********************************************************************/
uint8_t stepCtrl(uint8_t ServoNo, uint8_t ServoSpeed);
//uint8_t stepCtrl_Old(uint8_t ServoNo, uint8_t ServoSpeed, uint8_t Ticks);	

uint8_t speedControl(uint8_t ServoNo, uint8_t Ticks);

uint8_t servoCtrlGoTo(uint8_t ServoNo, uint8_t Ticks);

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
			servoCtrlGoTo(i, IsrCnt);
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
/*
	- neue Funktion *_3
	- bewegungsrichtung muss gespeichert werden
	- bewegungsumkehr erst nach Verzögerung auf 0
*/

uint8_t motionInit(uint8_t ServoNo)
{
	ServoData_SetStepCtrlInitDone(ServoNo, FALSE);
		
	if( ServoData_GetCurrentPosition(ServoNo) < ServoData_GetTargetPosition(ServoNo) )
		return GO_TO_MAX;			////	Go to MAX
			
	else if( ServoData_GetCurrentPosition(ServoNo) > ServoData_GetTargetPosition(ServoNo) )
		return GO_TO_MIN;			////	Go to MIN
			
	return MOTION_FINISHED;			////	END

}

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
		/*
		1. Beschleunigen
			1.1 prüfen, ob Distanz für Bremsvorgang erreicht ist
			  -> Ja: Bremsvorgang einleiten
			  -> Nein: Prüfen ob targetSpeed erreicht
			    -> Ja: Beschleunigung Ende - Gleichförmige Bewegung starten
				-> Nein: Beschleunigung fortsetzen
		2. Gleichförmige Bewegung
			2.1 Prüfen ob Distanz für Bremsvorgang erreicht ist
			  -> Ja: Bremsvorgang einleiten
			  -> Nein: Gleichförmige Bewegung fortsetzen
		3. Bremsvorgang
			3.1 Prüfen ob Zielposition erreicht ist
			  -> Ja: Bewegung beendet
			  -> Nein: Bremsvorgang fortsetzen
		*/
//		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		
		// checking acc, position to start dec, calculating dec, if received value not valid
		ServoData_SetMotionCtrlState(ServoNo, motionCtrl(ServoNo, Ticks) );

		// calculating speed depending on acc / decelaration 
		ServoData_SetCurrentSpeed(ServoNo, speedControl(ServoNo, Ticks));		
		
		// waiting for next Step depwnding on calculated speed
		if( stepCtrl(ServoNo, Ticks) )
		{
			ServoData_IncrementPoistion(ServoNo);
			#if( SERVEO_CTRL_BOARD_AVAILABLE )
				Servo(ServoNo, ServoData_GetCurrentPosition(ServoNo) );
			#endif
		}
	
		break;
		
	////	Go to MIN	////////////////////////////////////////////////////////////
	case GO_TO_MIN:
	
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
	
		break;

	////	STOP due to reversal of movement	////////////////////////////////////
	case GO_TO_REVERT:
	
		break;

	////	EMERGENCY STOP	////////////////////////////////////////////////////////
	case EMERGENCY_STOP:
	
		break;

	////	END		////////////////////////////////////////////////////////////////
	default:
		ServoData_SetCurrentSpeed(ServoNo, 0);
		ServoData_SetMoveRequest(ServoNo, FALSE);
		ServoData_SetSpeedCtrlInitDone(ServoNo, FALSE);
		ServoData_SetStepCtrlInitDone(ServoNo, FALSE);
		ServoData_SetMotionCtrlState(ServoNo, MOTION_INIT);
		
		return FALSE;
	}

	return TRUE;

	//////////////////////////////////////////////////////////////////////////////////
	// BACKUP
	////
/*
	if(servoArr[ServoNo].currentPosition > servoArr[ServoNo].targetPosition)
	{
		servoArr[ServoNo].currentSpeed = servoArr[ServoNo].targetSpeed;
		if( stepCtrl(ServoNo, Ticks) )
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
		if( stepCtrl(ServoNo, Ticks) )
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
	servoArr[ServoNo].stepCtrlInitDone = FALSE;
	return FALSE;
*/
}

////////////////////////////////////////////////////////////////////////////////////
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
			ServoData_SetStepDelayTicksCnt(ServoNo, ServoData_GetStepDelayTicksCnt(ServoNo) + 255 - ServoData_GetStepDelayTicksToggle(ServoNo) );
		// No overflowveloTicksDefault
		else
			ServoData_SetStepDelayTicksCnt(ServoNo, Ticks - ServoData_GetStepDelayTicksCnt(ServoNo) );
		
		ServoData_SetStepDelayTicksToggle(ServoNo, Ticks);
	}
	
	// veloTicksCntr reaches default value	////////////////////////////////////
	if(ServoData_GetStepDelayTicksCnt(ServoNo) >=  veloTicksDefault[ServoData_GetCurrentSpeed(ServoNo)] )
	{
		// Save passed Ticks for next cycle
		ServoData_SetStepDelayTicksCnt(ServoNo, ServoData_GetStepDelayTicksCnt(ServoNo) - veloTicksDefault[ServoData_GetCurrentSpeed(ServoNo)] );
		return TRUE;
	}
	
	return FALSE;
}

uint8_t speedControl(uint8_t ServoNo, uint8_t Ticks)
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
			ServoData_SetStepDelayTicksCnt(ServoNo, ServoData_GetStepDelayTicksCnt(ServoNo) + 255 - ServoData_GetStepDelayTicksToggle(ServoNo) );
		// No overflowveloTicksDefault
		else
			ServoData_SetStepDelayTicksCnt(ServoNo, Ticks - ServoData_GetStepDelayTicksCnt(ServoNo) );
		
		ServoData_SetStepDelayTicksToggle(ServoNo, Ticks);
	}
	
	// veloTicksCntr reaches default value	////////////////////////////////////
	if(ServoData_GetStepDelayTicksCnt(ServoNo) >=  veloTicksDefault[ServoData_GetCurrentSpeed(ServoNo)] )
	{
		// Save passed Ticks for next cycle
		ServoData_SetStepDelayTicksCnt(ServoNo, ServoData_GetStepDelayTicksCnt(ServoNo) - veloTicksDefault[ServoData_GetCurrentSpeed(ServoNo)] );
		return TRUE;
	}
	
	return FALSE;
}