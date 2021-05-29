/*
 * _task1000ms.h
 *
 * Created: 11.08.2017 20:24:28
 *  Author: jayst
 */ 


#ifndef _TASK1000MS_H_
#define _TASK1000MS_H_

#include "_auxiliaryFunctions.h"
#include "_twiCtrl.h"

///////////////////////////////////////////////////////////////////////////////////////////
// 1000ms	///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//int8_t expo = 0;
//uint16_t readBusVoltage();

void _1sTask(uint8_t *_100msCycleCnt)
{
	if (*_100msCycleCnt == 10)
	{
		*_100msCycleCnt = 0;
//		uart_puts("1s Tick \r\n");
/*		if(PORTB & (1 << 5))
			PORTB &= ~(1 << 5);
		else
			PORTB |= (1 << 5);
*/			

		PORTB ^= 1 << 5;	// XOR Blink LED Pin5

		
/*		int32_t result = expoInt32(3,expo);
		uart_puts("result = ");
		uart_putc('0' + (char)(result/1000));
		uart_putc('0' + (char)((result%1000)/100));
		uart_putc('0' + (char)((result%100)/10));
		uart_putc('0' + (char)((result%10)));
		uart_puts("\r\n");
		expo++;
*/
/*		uint16_t busVoltage;
		busVoltage = readBusVoltage();
		uart_puts("Bus Voltage (Dig) = ");
		uart_putc('0' + (char)(busVoltage/1000));
		uart_putc('0' + (char)((busVoltage%1000)/100));
		uart_putc('0' + (char)((busVoltage%100)/10));
		uart_putc('0' + (char)((busVoltage%10)));
		uart_puts(" mV\r\n");
*/	}
	
}

/*uint16_t readBusVoltage()
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
		uart_puts("Byte 0x02 sent\r\n");
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
*/
#endif /* _TASK1000MS_H_ */
