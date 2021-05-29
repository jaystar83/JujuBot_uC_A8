/*
 * task100ms.c
 *
 * Created: 12.03.2021 20:12:49
 *  Author: jayst
 */ 

#include "_task100ms.h"
#include "serial.h"

uint8_t blinkCnt[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t _100msCompare = 0;

//char message[] = {'0','1','2','3','4','5','6','7','8','9','\r','\n'};
unsigned char message[] = {0,0,0,0,0,0,0,0,0,0,'\r','\n'};

char ErrMessage[]		= {'C','0','1',',','0','1',',','N','O','K','\r','\n','\0'}; // Cxy,ab,NOK\r\n\0 = 13 Byte
char SuccessMessage[]	= {'C','0','1',',','O','K','\r','\n','\0'}; // Cxy,OK\r\n\0 = 9 Byte

uint8_t _1sCnt = 0;

void task100ms(uint8_t _100msCntInput)
{
	if(_100msCompare != _100msCntInput)
	{
		_100msCompare = _100msCntInput;

//		blink1s(0, PORTB, PORTB5);
			
		_1sCnt++;
		if(_1sCnt >= 10)
		{
			TOOGLE_BIT(PORTC,PORTC0);	// -> (Green LED)
//			TOOGLE_BIT(PORTB,PORTB5);	// -> (Board LED)

			_1sCnt = 0;
			for(uint8_t i = 0; i < 10; i++)
			message[i]='0' + i;

//			uart_putd(message, 12);							
		}

//		uart_puts(ErrMessage);
//		uart_puts(SuccessMessage);

	}
}


void blink1s(uint8_t blinkCntNo, uint8_t Port, uint8_t Pin)
{
		blinkCnt[blinkCntNo]++;
		if(blinkCnt[blinkCntNo] >= 10)
		{
			blinkCnt[blinkCntNo] = 0;
			TOOGLE_BIT(Port, Pin); //!!!	Funktioniert nicht	!!! ???
		}
	
}

