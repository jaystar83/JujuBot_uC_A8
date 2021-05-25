/*
 * _auxiliaryFunctions.h
 *
 * Created: 11.08.2017 11:53:10
 *  Author: jayst
 */ 


/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "_99_defines.h"


#ifndef _AUXILIARYFUNCTIONS_H_
#define _AUXILIARYFUNCTIONS_H_

/***********************************************************************
	INCLUDES
***********************************************************************/
#if( SW_TEST_ACTIVE )
	#include "_test_avrio.h"
#else
	#include <avr/io.h>
#endif


/***********************************************************************
	MACROS
***********************************************************************/
#define SET_BIT(PORT, BITNUM)	((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOOGLE_BIT(PORT,BITNUM) ((PORT) ^= (1<<(BITNUM)))

/***********************************************************************
	DEFINITIONS
***********************************************************************/

typedef struct  
{
	uint8_t PortByte;
	uint8_t PinNo;
	uint8_t DebounceCycles;
}debounceData_t;
/***********************************************************************
	GLOBAL VARIABLES
***********************************************************************/
#define DebounceCycleLimit 5
#define DebounceBttns 3

/***********************************************************************
	PROTYPS
***********************************************************************/

void EA_Init();
// Entprelken
uint8_t debouncer(uint8_t PortByte, uint8_t PinNo, uint8_t DebounceCycles, uint8_t* DebounceCycleCntr);
// Flankenauswertung eines Tasters -> muss in Zyklus mit mehreren ms (zB. 100 ausgewertet werden)
uint8_t r_trig(uint8_t PortByte, uint8_t PinNo, uint8_t Out );
// Button Click Counter ////////////////////////////////////////////////////////////////////////
uint8_t bttnClickCounter(uint8_t Trigger, uint8_t *CickCnt,  uint8_t *CycleCnt, uint8_t CyclLimit );
// exponetial function
int32_t expoInt32(int32_t base, int8_t expon);

#endif /* _AUXILIARYFUNCTIONS_H_ */
