/*
 * JRoboControl.c
 *
 * Created: 06.03.2021 14:36:18
 * Author : jaystar
 * Version: 0.0.1_0002
 *
 * Beispiel für die 2. Version eines Programms, in der 3. Nebenversion und in der 5. Fehlerkorrektur, Build 0041:
 * 2.3.5_0041
 * | | |  '------ Buildnummer
 * | | '--------- Revisionsnummer
 * | '----------- Nebenversionsnummer
 * '------------- Hauptversionsnummer
 */ 


/***********************************************************************
	DEFINITIONS
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
	//#include <util/delay.h>
	//#include <stdint-gcc.h>
	#include <avr/interrupt.h>
#endif

#include "serial.h"

#include "servoCtrl.h"
#include "communicationCtrl.h"

#include "auxiliaryFunctions.h"
#include "timerConfig.h"
#include "watchdog.h"

#include "_task1ms.h"
#include "_task100ms.h"
#include "_taskIdle.h"

/***********************************************************************
	MACROS
***********************************************************************/

/***********************************************************************
	VARS
***********************************************************************/
// ISR VOLATILE	////////////////////////////////////////////////////////////////////
volatile uint8_t isrCnt0	= 0;
volatile uint8_t isrCnt2	= 0;

volatile uint8_t _1msCnt  = 0;
volatile uint8_t _10msCnt  = 0;
volatile uint8_t _100msCnt  = 0;
volatile uint8_t _1000msCnt  = 0;


/***********************************************************************
	MAIN
***********************************************************************/
int main(void)
{
/***********************************************************************
	INITIALIZATION
***********************************************************************/
//***	UART mit Clock-Frequenz und Baudrate initialisieren
	CommunicationCtrlInit();
	
//***	E/A-Initialisierung	*****************************************
	EA_Init();

//***	Servos	*****************************************************
//	servoCtrlInit(servo, F_CPU, TWI_BAUD, TWI_PRESCALER, PCA9685_ADDR);
	ServoCtrlInit(F_CPU, TWI_BAUD, TWI_PRESCALER, PCA9685_ADDR);
	
//***	Timer	*****************************************************
	//(Mode,PreScaler, OCR) Mode: 0-Timer OFF, 1-TIMER Überlauf, 2-CTC
	// PreScaler: 2, 8, 64, 256, 1024, OCR: 8bit / 16bit Compare-Wert
	configTimer0(2, 64, 250); // bei 16MHz = 0,5ms, 8Mhz = 0,2ms (Mode, PreScaler, OCR)
	configTimer2(2, 8, 100); // (Mode, PreScaler, OCR) -> bei 16MHz = 0.05ms 
	// f_Steper = 100 Hz = 0.01ms -> 1Revolution = 4Cycles -> (0.01/4) / 0.0005 = 5 => 5 TimerTicks = 1/4Step = CoilCtrlStepMaxSpeed

	sei();	// aktiviert ISR

	uart_puts("JRoboCtrl -V.0.0.1\r\n");

/***********************************************************************
	Application Code
***********************************************************************/
    while (1) 
    {
		taskIdle(isrCnt2);
		task1ms(_1msCnt);
		task100ms(_100msCnt);
	}
}

/***********************************************************************
	Timer0 Interrupt (1ms)
***********************************************************************/
ISR (TIMER0_COMPA_vect)  // timer0 CTC overflow interrupt
{
	isrCnt0++;
	_1msCnt++;
	
	watchdog();
	
	// Cntr für 10ms Tick
	if(_1msCnt >= 10)
	{
		_10msCnt++;
		_1msCnt = 0;
		
		// Cntr für 100ms Tick
		if(_10msCnt >= 10)
		{
			_100msCnt++;
			_10msCnt = 0;
			
			// Ctr für 1s Tick
			if(_100msCnt >= 10)
			{
				//TOOGLE_BIT(PORTB,PORTB5);
				_100msCnt = 0;
				_1000msCnt++;
			}
		}
	}
}

/***********************************************************************
	Timer2 Interrupt (0.05ms = 50us)
***********************************************************************/
ISR (TIMER2_COMPA_vect)  // timer2 CTC overflow interrupt
{
	isrCnt2++;	
}


/*
Servo Specs

Specifications
 Weight: 13.4 g
 Dimension: 22.5 x 12 x 35.5
 Stall torque: 1.8 kg·cm (4.8V ), 2.2 kg·cm (6 V)
 Operating speed: 0.1 s/60 degree (4.8 V), 0.08 s/60 degree (6 V)
 Operating voltage: 4.8 V - 6.0V
 Dead band width: 5 µs

*/
