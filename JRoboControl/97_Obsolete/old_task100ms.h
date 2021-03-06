/*
 * _tasks.h
 *
 * Created: 11.08.2017 12:03:56
 *  Author: jayst
 */ 


#ifndef _TASK100MS_H_
#define _TASK100MS_H_

#define _BUTTONS 3

#include "_auxiliaryFunctions.h"
#include "_twiCtrl.h"

//	Buttons	/////////////////////////////////////////////////////////////////////////
uint8_t bttnVal[_BUTTONS] = {0, 0, 0};			// Button[x] -> debounced
uint8_t bttnClickCnt[_BUTTONS] = {0, 0, 0};		// Button[x] -> ClickCounter Value
uint8_t bttnCycleCnt[_BUTTONS] = {0, 0, 0};		// Button[x] -> Cycle cntr
uint8_t bttnCycleLimit[_BUTTONS] = {5, 5, 5};	// Button[x] -> Cycle cntr limit
uint8_t bttnTrigInstance[_BUTTONS] = {0, 0, 0};	// Button[x] -> R_Trig intancr var
uint8_t bttnTrig[_BUTTONS] = {0, 0, 0};			// Button[x] -> R_Trig intancr var

//	100ms Task	/////////////////////////////////////////////////////////////////////
uint8_t _0_5msCnt				= 0;
uint8_t _100msCnt				= 0; 
volatile uint8_t blinkMotCommand			= 0;		// glob. Variable um Blink-Takt von gelber und Roter LED zu steuern
volatile uint8_t nextStepDelayLimit		= 5;		// Wartezet bis nächsterProgrammschritt ausgeführt wird


//int8_t expo = 0;
uint16_t readBusVoltage();
uint8_t busVoltageAvergaeCnt	= 0;
uint16_t busVoltage;

///////////////////////////////////////////////////////////////////////////////////////////
// 100ms	///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void _100msTask(uint8_t *_0_5msCycleCnt, uint8_t *_100msCycleCnt, uint8_t Buttons, uint8_t *MotCommand)
{
if (*_0_5msCycleCnt >= 200){
//	Start	///////////////////////////////////////////////////////////////////////////////

// reset cycle cnt
*_0_5msCycleCnt = 0;
(*_100msCycleCnt)++;

// set buttons
for(int i=0; i<_BUTTONS; i++)
{
	bttnVal[i] = Buttons & ((1<<i) >> i);
	bttnTrig[i] = r_trig(Buttons, i, bttnTrigInstance[i]);
	
}
/*
bttn_0 = Buttons & (1<<0);
bttn_1 = (Buttons & (1<<1)) >> 1;
bttn_2 = (Buttons & (1<<2)) >> 2;

bttn_0_trig = r_trig(PORTD, 0, bttn_0_trig);
bttn_1_trig = r_trig(PORTD, 0, bttn_1_trig);
bttn_2_trig = r_trig(PORTD, 0, bttn_2_trig);
*/
if (bttnClickCounter(bttnTrig[0], &bttnClickCnt[0], &bttnCycleCnt[0], bttnCycleLimit[0]))
{
	switch(bttnCycleCnt[0])
	{
		case 1:
		{	
			if(*MotCommand == 1)
				*MotCommand = 2;	// Stop
			else
				*MotCommand = 1;	// Start
		}
		case 2:
		{
			;	
		}
	}
}
if (bttnClickCounter(bttnTrig[1], &bttnClickCnt[1], &bttnCycleCnt[1], bttnCycleLimit[1]))
{
	;
}
if (bttnClickCounter(bttnTrig[2], &bttnClickCnt[2], &bttnCycleCnt[2], bttnCycleLimit[2]))
{
	;
}

/*
if (busVoltageAvergaeCnt == 0)
{
	uint16_t busVoltTemp;
	busVoltTemp = readBusVoltage();
	uart_puts("DigVal  = ");
	uart_putc('0' + (char)(busVoltTemp/1000));
	uart_putc('0' + (char)((busVoltTemp%1000)/100));
	uart_putc('0' + (char)((busVoltTemp%100)/10));
	uart_putc('0' + (char)((busVoltTemp%10)));
	uart_puts(" - ");
	busVoltTemp = ((busVoltTemp)>>3)<<2;
	uart_puts("Bus Voltage Temp  = ");
	uart_putc('0' + (char)(busVoltTemp/1000));
	uart_putc('0' + (char)((busVoltTemp%1000)/100));
	uart_putc('0' + (char)((busVoltTemp%100)/10));
	uart_putc('0' + (char)((busVoltTemp%10)));
	uart_puts("mV - ");
	busVoltage = busVoltTemp;
	uart_puts("Bus Voltage (Dig) = ");
	uart_putc('0' + (char)(busVoltage/1000));
	uart_putc('0' + (char)((busVoltage%1000)/100));
	uart_putc('0' + (char)((busVoltage%100)/10));
	uart_putc('0' + (char)((busVoltage%10)));
	uart_puts(" mV\r\n");
}
else
{
	uint16_t busVoltTemp;
	busVoltTemp = readBusVoltage();
	uart_puts("DigVal  = ");
	uart_putc('0' + (char)(busVoltTemp/1000));
	uart_putc('0' + (char)((busVoltTemp%1000)/100));
	uart_putc('0' + (char)((busVoltTemp%100)/10));
	uart_putc('0' + (char)((busVoltTemp%10)));
	uart_puts(" - ");
	busVoltTemp = ((busVoltTemp)>>3)<<2;
	uart_puts("Bus Voltage Temp  = ");
	uart_putc('0' + (char)(busVoltTemp/1000));
	uart_putc('0' + (char)((busVoltTemp%1000)/100));
	uart_putc('0' + (char)((busVoltTemp%100)/10));
	uart_putc('0' + (char)((busVoltTemp%10)));
	uart_puts("mV - ");
	busVoltage = ( busVoltage + busVoltTemp) >> 1;
	uart_puts("Bus Voltage (Dig) = ");
	uart_putc('0' + (char)(busVoltage/1000));
	uart_putc('0' + (char)((busVoltage%1000)/100));
	uart_putc('0' + (char)((busVoltage%100)/10));
	uart_putc('0' + (char)((busVoltage%10)));
	uart_puts(" mV\r\n");
}
busVoltageAvergaeCnt++;

if (busVoltageAvergaeCnt>=10)
{	
	
	uart_puts("Bus Voltage (Dig) = ");
	uart_putc('0' + (char)(busVoltage/1000));
	uart_putc('0' + (char)((busVoltage%1000)/100));
	uart_putc('0' + (char)((busVoltage%100)/10));
	uart_putc('0' + (char)((busVoltage%10)));
	uart_puts(" mV\r\n");
	busVoltageAvergaeCnt = 0;
}
*/		

//	End	100ms Taks/////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////
}

uint16_t readBusVoltage()
{
	uint8_t busVoltageReg = 0x02;
	
	if (!TwiMStart(0x40, TWIM_WRITE))
	{
		TwiMStop();
		uart_puts("Could not start TWI Bus for WRITE\r\n");
	}
	else
	{
		//		  	for (i=0;i<8;i++)
		//		  	{
		//			TwiMWrite(busVoltageReg);
		//			uart_puts("Byte 0x02 sent");
		//		  	}
		if(!TwiMWrite(busVoltageReg))
		{
			TwiMStop();
			uart_puts("Could not send Byte 0x02 \r\n");
			return 0;
		}
		uart_puts("Byte 0x02 sent - ");
		TwiMStop();
		
		if (!TwiMStart (0x40, TWIM_READ))
		{
			TwiMStop ();
			uart_puts ("Could not start TWI Bus for READ\r\n");
		}
		else
		{
			uint16_t result = (TwiMReadAck() << 8) ;
			result += TwiMReadNack();
			//			result = (result>>3)*4;
			TwiMStop();
			
			return result;
		}

	}
	
	return 0;
}

#endif /* _TASKS_H_ */ 
