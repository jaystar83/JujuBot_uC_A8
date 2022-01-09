/*
 * CFile1.c
 *
 * Created: 06.03.2021 19:33:08
 *  Author: jayst
 */ 

#include "auxiliaryFunctions.h"


/***********************************************************************
	LOCAL VARS
***********************************************************************/
uint8_t DebounceCycleCnt[DebounceBttns] = {0};
	
debounceData_t debounce_PortD_Pin5;// = {PORTD, 5, DebounceCycleLimit};
//debounce_PortD_Pin5.PortByte = PORTD;
//debounce_PortD_Pin5.PinNo = 5;
//debounce_PortD_Pin5.DebounceCycles = DebounceCycleLimit;
debounceData_t debounce_PortD_Pin6;
//debounce_PortD_Pin6.PortByte = PORTD;
//debounce_PortD_Pin6.PinNo = 6;
//debounce_PortD_Pin6.DebounceCycles = DebounceCycleLimit;
debounceData_t debounce_PortD_Pin7;
//debounce_PortD_Pin7.PortByte = PORTD;
//debounce_PortD_Pin7.PinNo = 7;
//debounce_PortD_Pin7.DebounceCycles = DebounceCycleLimit;

/***********************************************************************
	IMPLEMENTATION
***********************************************************************/
void EA_Init()
{
//***	DIG-I/O	*****************************************************
	//	DDRB	|= 1 << 5;	// Pin 5 von Port B als Ausgang# verwenden	-> (Board LED)

	DDRC	|= 1 << 0;	// Pin 0 von Port C als Ausgang verwenden	-> (Green LED)
	DDRC	|= 1 << 1;	// Pin 1 von Port C als Ausgang verwenden
	DDRC	|= 1 << 2;	// Pin 2 von Port C als Ausgang verwenden
	DDRC	|= 1 << 3;	// Pin 3 von Port C als Ausgang verwenden

	DDRD	&= ~(1 << 5);	// Pin 5 von Port D als Eingang verwenden (Green Button -> Start)
	DDRD	&= ~(1 << 6);	// Pin 6 von Port D als Eingang verwenden (Black Button -> Set)
	DDRD	&= ~(1 << 7);	// Pin 7 von Port D als Eingang verwenden (Switch: Auto <-> RemoteCtrl
	PORTD	= 0;
	PORTD	|= (1 << 5);	// Pull-Up Widerstand aktiviert
	PORTD	|= (1 << 6);	// Pull-Up Widerstand aktiviert
	PORTD	|= (1 << 7);	// Pull-Up Widerstand aktiviert
}

uint8_t debouncer(uint8_t PortByte, uint8_t PinNo, uint8_t DebounceCycles, uint8_t* DebounceCycleCntr)
{
	if( (*DebounceCycleCntr) == 0 ) 	// Entprell-Cnt = 0 -> Überwachung, ob etwas an PinX anliegt
	{
		if(PortByte & PinNo)		// Wenn etwas anliegt -> erhöhe Entprell-Cnt
		(*DebounceCycleCntr)++;
		return 0;
	}
	else
	{
		if( *DebounceCycleCntr < DebounceCycles)
		(*DebounceCycleCntr)++;
		else
		{
			if( !(PortByte & PinNo) )
			{
				(*DebounceCycleCntr) = 0;
				return 0;
			}
		}
		
		return 1;
	}
}


// Flankenauswertung eines Tasters -> muss in Zyklus mit mehreren ms (zB. 100 ausgewertet werden)
uint8_t r_trig(uint8_t PortByte, uint8_t PinNo, uint8_t Out )
{
	if( (Out == 0) && (PortByte & (1<<PinNo)) )
	{
		return 1;
	}
	else if( !( PortByte & (1<<PinNo) ) )
	{
		return 0;
	}
	else
	return 0;
	// INFO: es ist möglich, dass beim Lösen des Tasters auch Pulse entstehen -> Möglichkeit aber sehr gering, wenn Cyclus-Zeit hoch
}

// Button Click Counter ////////////////////////////////////////////////////////////////////////
uint8_t bttnClickCounter(uint8_t Trigger, uint8_t *CickCnt,  uint8_t *CycleCnt, uint8_t CyclLimit )
{
	///// Trigger activates Cycle Cnt	/////
	if (Trigger)
	{
		if(*CycleCnt >= CyclLimit)
		{
			*CycleCnt = 0;
			*CickCnt = 0;
		}
		(*CickCnt)++;
	}		
	
	///// shows ClickCounter finished	/////
	if(*CycleCnt >= CyclLimit)
		return 1;
	else
	{
		if(*CickCnt > 0)
			(*CycleCnt)++; 
			
		return 0;
	}	
	
}

// exponetial function
int32_t expoInt32(int32_t base, int8_t expon)
{
	if(expon >= 0)
	{
		int32_t ret	= 1;

		for(int i=0; i<expon; i++)
		{
			ret*= base;
		}
		return ret;
	}
	else
	{
		return  FALSE;
	}
}