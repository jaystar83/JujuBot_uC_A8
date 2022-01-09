/*
 * _task1ms.c
 *
 * Created: 12.03.2021 21:43:57
 *  Author: jayst
 */ 

#include "_task1ms.h"

#include "auxiliaryFunctions.h"
//#include "servoMotionCtrl.h"
//#include "serial.h"

uint8_t msToggleCompare = 0;

void task1ms(uint8_t MsToggle)
{
	if(msToggleCompare != MsToggle)
	{
		msToggleCompare = MsToggle;
				
//		SercoCtrl_CheckMoveRequest();
		
/*		servoCtrlGoTo(ServoDataPtr);
		ServoDataPtr++;
		servoCtrlGoTo(ServoDataPtr);		
*/
	}
}
