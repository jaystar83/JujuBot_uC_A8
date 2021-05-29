/*
 * _timerConfig.c
 *
 * Created: 07.03.2021 16:51:40
 *  Author: jayst
 */ 

#include "timerConfig.h"

/////////////////////////////////////////////////////////////////////
////	8-Bit Timer/Counter	/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

/////	TIMER 0	////////////////////////////////////////////////////
// TCCR0 - Timer/Counter Control Register für Timer 0 (8-Bit)
void configTimer0(uint8_t Mode, uint16_t PreScale, uint8_t OCR) 
{	TCCR0A |= (0<<COM0A1);	// Bit 7
	TCCR0A |= (0<<COM0A0);	// Bit 6
	TCCR0A |= (0<<COM0B1);	// Bit 5
	TCCR0A |= (0<<COM0B0);	// Bit 4
	// TCCR1A;	// Bit 3
	// TCCR1A;	// Bit 2
	TCCR0A |= (0<<WGM01);	// Bit 1 - Waveform Generator Mode bits 0..2
	TCCR0A |= (0<<WGM00);	// Bit 0 - 000 - Normal, 001 - PWM PC, 010 - CTC, 011- PWM Fast, 100 Reserved
	
	TCCR0B |= (0<<FOC0A);	// Bit 7
	TCCR0B |= (0<<FOC0B);	// Bit 6
	//	TCCR1B				// Bit 5 - nicht belegt
	//	TCCR0B 				// Bit 4 - nicht belegt
	TCCR0B |= (0<<WGM02);	// Bit 3 - 101 - PWM Fast PC, 110 - Reserved, 111 - PWM Fast
	TCCR0B |= (0<<CS02);		// Bit 2 - Bit2-1 CSS Clock Select Bits- geben die Quelle und den Prescaler für die Timer
	TCCR0B |= (0<<CS01);		// Bit 1 - 000 - Stop, 001 - CPU-Takt, 010 - CPU/8, 011 - CPU/64, 100 - CPU/256, 101 - CPU/1024
	TCCR0B |= (0<<CS00);		// Bit 0 - 110 - Pin TO - fallende Flanke, 111 - Pin TO - steigende Flanke
	
	// TCNT0 - Timer/Counter Daten Register Timer 0
	// 8Bit Zähl-Register 0...255

	// OCR0 - Output Compare Register
	// 8Bit-Zahl 0...255 -> 125 ticks = 124!!!

	// TIMSK - Timer/Counter Interrupt Mask Register

	// TIFR - Timer/Counter Interrupt Flag Register

	unsigned char tempTCCR = 0;
	switch(Mode)
	{
	// Timer Off	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case 0:	
		{	TCCR0B |= (0<<CS02);		
			TCCR0B |= (0<<CS01);		
			TCCR0B |= (0<<CS00);
		
			OCR0A = OCR;
			TIMSK0 &= (0 << OCIE0A);    //ReSet the ISR COMPA vect
			break;	
		}
	// Normal Mode	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case 1:	
		{	setPreScaler(PreScale, &tempTCCR);
			TCCR0B |= tempTCCR;
			break;
		}
	// CTC Mode		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case 2:	
		{	setPreScaler(PreScale, &tempTCCR);
			TCCR0B |= tempTCCR;
		
			// Set the Timer Mode to CTC
			TCCR0A |= (1 << WGM01);

			// Set the value that you want to count to
			OCR0A = OCR;
			// OCR0A = 0xF9; //(249 = 250 - 1 -> 250 Zyklen) -> 16Mhz / 64 = 250kHz / 250 OCRA = 1kHz
			// OCR0A = 0x7C; //(124 = 125 - 1 -> 125 Zyklen)

			TIMSK0 |= (1 << OCIE0A);    //Set the ISR COMPA vect
			break;
		}
	}
	
};

/////	TIMER 2	////////////////////////////////////////////////////		
// TCCR2 - Timer/Counter Control Register für Timer 2 (8-Bit)
void configTimer2(uint8_t Mode, uint16_t PreScale, uint8_t OCR) 
{	TCCR2A |= (0<<COM2A1);	// Bit 7
	TCCR2A |= (0<<COM2A0);	// Bit 6
	TCCR2A |= (0<<COM2B1);	// Bit 5
	TCCR2A |= (0<<COM2B0);	// Bit 4
	// TCCR1A;	// Bit 3
	// TCCR1A;	// Bit 2
	TCCR2A |= (0<<WGM21);	// Bit 1 - Waveform Generator Mode bits 0..2
	TCCR2A |= (0<<WGM20);	// Bit 0 - 000 - Normal, 001 - PWM PC, 010 - CTC, 011- PWM Fast, 100 Reserved
	
	TCCR2B |= (0<<FOC2A);	// Bit 7
	TCCR2B |= (0<<FOC2B);	// Bit 6
	//	TCCR2B				// Bit 5 - nicht belegt
	//	TCCR2B 				// Bit 4 - nicht belegt
	TCCR2B |= (0<<WGM22);	// Bit 3 - 101 - PWM Fast PC, 110 - Reserved, 111 - PWM Fast
	TCCR2B |= (0<<CS22);		// Bit 2 - Bit2-1 CSS Clock Select Bits- geben die Quelle und den Prescaler für die Timer
	TCCR2B |= (0<<CS21);		// Bit 1 - 000 - Stop, 001 - CPU-Takt, 010 - CPU/8, 011 - CPU/64, 100 - CPU/256, 101 - CPU/1024
	TCCR2B |= (0<<CS20);		// Bit 0 - 110 - Pin TO - fallende Flanke, 111 - Pin TO - steigende Flanke
	
	// TCNT2 - Timer/Counter Daten Register Timer 0
	// 8Bit Zähl-Register 0...255

	// OCR2 - Output Compare Register
	// 8Bit-Zahl 0...255 -> 125 ticks = 124!!!

	// TIMSK - Timer/Counter Interrupt Mask Register

	// TIFR - Timer/Counter Interrupt Flag Register

	unsigned char tempTCCR = 0;
	switch(Mode)
	{
	// Timer Off	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case 0:	
		{	TCCR2B |= (0<<CS22);		
			TCCR2B |= (0<<CS21);		
			TCCR2B |= (0<<CS20);
		
			OCR2A = OCR;
			TIMSK2 &= (0 << OCIE2A);    //ReSet the ISR COMPA vect
			break;	
		}
	// Normal Mode	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case 1:	
		{	setPreScaler(PreScale, &tempTCCR);
			TCCR2B |= tempTCCR;
			break;
		}
	// CTC Mode		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case 2:	
		{	setPreScaler(PreScale, &tempTCCR);
			TCCR2B |= tempTCCR;
		
			// Set the Timer Mode to CTC
			TCCR2A |= (1 << WGM21);

			// Set the value that you want to count to
			OCR2A = OCR;
			// OCR0A = 0xF9; //(249 = 250 - 1 -> 250 Zyklen) -> 16Mhz / 64 = 250kHz / 250 OCRA = 1kHz
			// OCR0A = 0x7C; //(124 = 125 - 1 -> 125 Zyklen)

			TIMSK2 |= (1 << OCIE2A);    //Set the ISR COMPA vect
			break;
		}
	}
	
};
	
/////////////////////////////////////////////////////////////////////////
/////	16-Bit Timer/Counter	/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// TCCR1 - Timer/Counter Control Register für Timer 1 (16-Bit)
void configTimer1(uint8_t Mode, uint16_t PreScale, uint16_t OCR)
{	// TCCR1x - Timer/Counter Control Register für Timer 1 (16-Bit)
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	// TIMSK1 - Timer/Counter Interrupt Mask Register
	TIMSK1 = 0;
	
	unsigned char tempTCCR = 0;
	switch(Mode)
	{
		// Timer Off	//////////////////////////////////////////////////
		case 0:
		{	TCCR1B |= (0<<CS02);
			TCCR1B |= (0<<CS01);
			TCCR1B |= (0<<CS00);
			
			OCR1A = OCR;
			TIMSK1 &= (0 << OCIE1A);    //ReSet the ISR COMPA vect
			break;
		}
		// Normal Mode	//////////////////////////////////////////////////
		case 1:
		{	setPreScaler(PreScale, &tempTCCR);
			TCCR1B |= tempTCCR;
			break;
		}
		// CTC Mode		//////////////////////////////////////////////////
		case 2:
		{	setPreScaler(PreScale, &tempTCCR);
			TCCR1B |= tempTCCR;
			
			// Set the Timer Mode to CTC
			TCCR1B |= (1 << WGM12);

			// Set the value that you want to count to
			OCR1A = OCR;
			// OCR0A = 0xF9; //(249 = 250 - 1 -> 250 Zyklen) -> 16Mhz / 64 = 250kHz / 250 OCRA = 1kHz
			// OCR0A = 0x7C; //(124 = 125 - 1 -> 125 Zyklen)

			TIMSK1 |= (1 << OCIE1A);    //Set the ISR COMPA vect
			break;
		}
	}
	
}


// TCCR1x - Timer/Counter Control Register für Timer 1 (16-Bit)
/*	TCCR1A |= (0<<COM1A1);	// Bit 7 OutputMode Kanal A -> 00 - disable, 01 - toggle  OC1A
	TCCR1A |= (0<<COM1A0);	// Bit 6 10 - reset OC1A, 11 - set OC1A; ! PWM-Mode siehe bsp. Kanal B
	TCCR1A |= (0<<COM1B1);	// Bit 5 OutputMode Kanal B (als PWM) ->  00 disable, 
	TCCR1A |= (0<<COM1B0);	// Bit 4 01 + WGM13:0=14/15 toggle OC1A, 10 - clear OC1A/B, 11 - set OC1A/B
	// TCCR1A;	// Bit 3	// nicht beleget 
	// TCCR1A;	// Bit 2	// nicht belegt
	TCCR1A |= (0<<WGM11);	// Bit 1 WGM10:3 0000 - normal, 0001 - 8bit PWM, 0010 - 9bit PWM, 0011 - 10bit PW CTCM
	TCCR1A |= (0<<WGM10);	// Bit 0 0100 - CTC auf OCR1A, 0101 - fast PWM 8bit, 0110 9bit fPWM, 0111 10bit fPWM	

	TCCR1B |= (0<<ICNC1);	// Bit 7
	TCCR1B |= (0<<ICES1);	// Bit 6
	//	TCCR1B	// Bit 5
	TCCR1B |= (0<<WGM13);	// Bit 4 (siehe TCCR1A) 1100 - CTC auf ICR1, 1101 - reserved, 1110 - fPWM auf OCR1A,
	TCCR1B |= (0<<WGM12);	// Bit 3				1111 - fPWM auf ICR1
	TCCR1B |= (0<<CS02);	// Bit 2 - Bit2-1 CSS Clock Select Bits- geben die Quelle und den Prescaler für die Timer
	TCCR1B |= (0<<CS01);	// Bit 1 - 000 - Stop, 001 - CPU-Takt, 010 - CPU/8, 011 - CPU/64, 100 - CPU/256, 101 - CPU/1024
	TCCR1B |= (0<<CS00);	// Bit 0 - 110 - Pin TO - fallende Flanke, 111 - Pin TO - steigende Flanke

	TCCR1C |= (0<<FOC1A);	// Bit 7
	TCCR1C |= (0<<FOC1B);	// Bit 6
	//	TCCR1C	// Bit 5
	//	TCCR1C 	// Bit 4
	//	TCCR1C	// Bit 3
	//	TCCR1C 	// Bit 2 
	//	TCCR1C	// Bit 1
	//	TCCR1C 	// Bit 0

	// TIMSK1 - Timer/Counter Interrupt Mask Register
	TIMSK1	// Bit 7
	TIMSK1	// Bit 6
	TIMSK1 |= (0<<ICIE1)  // Bit 5	1 -> Input Capture Interrupt EIN -> wenn globale Interrupts EIN
	TIMSK1	// Bit 4
	TIMSK1	// Bit 3
	TIMSK1 |= (0<<OCIE1B) // Bit 2	1 -> Output Compare B Interrupt EIN -> wenn globale Interrupts EIN
	TIMSK1 |= (0<<OCIE1A) // Bit 1	1 -> Output Compare A Interrupt EIN -> wenn globale Interrupts EIN
	TIMSK1 |= (0<<TOIE1)  // Bit 0	1 -> Overflow Interrupt EIN -> wenn globale Interrupts EIN

	// TCNT1 - Timer/Counter 16bit Daten Register Timer 1 - contains TCNT1L & TCNT1H
	// 16Bit Zähl-Register 0...1023

	// OCR1A - Output Compare Register A (16bit) - contains OCR1AL & OCR1AL
	// 16Bit-Zahl 0...1023 -> 1000 ticks = 999!!!
	// OCR1B - Output Compare Register B (16bit) - contains OCR1AL & OCR1AL
	// 16Bit-Zahl 0...1023 -> 1000 ticks = 999!!!
	// -> auf welchem Register der Interrupt ausgelöst wird im TIMSK1 gesetzt

	// TIFR1 - Timer/Counter Interrupt Flag Register
	TIFR1	// Bit 7
	TIFR1	// Bit 6
	TIFR1 	// Bit 5 - ICF1 - 1 bei CaptureEvent an ICP1-Pin, wenn in WGM13:0 entspr. konfiguriert, wird zurück gesetzt wenn ISR ausgeführt wurde
	TIFR1	// Bit 4
	TIFR1	// Bit 3
	TIFR1	// Bit 2 - OCF1B - 1 wenn CounterValue (TCNT1) Wert des OCR1B , wird zurück gesetzt wenn Compare Match B ISR ausgeführt wurde
	TIFR1	// Bit 1 - OCF1A - 1 wenn CounterValue (TCNT1) Wert des OCR1A , wird zurück gesetzt wenn Compare Match A ISR ausgeführt wurde
	TIFR1	// Bit 0 - TOV1 - 1 bei TimerOverflow, abhängig von WGM13:0, wird zurück gesetzt, wenn ISR ausgeführt wurde
*/

///////////////////////////////////////////////////////////////////////////////////////
///// Funktionen	///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
unsigned char setPreScaler(uint16_t preScaler, unsigned char* TCCR)
{
	// 000 - Stop, 001 - CPU-Takt, 010 - CPU/8, 011 - CPU/64, 100 - CPU/256, 101 - CPU/1024
	// 110 - Pin TO - fallende Flanke, 111 - Pin TO - steigende Flanke
	// 16Mhz / 64 = 250kHz
	// 16Mhz / 256 = 62.5kHz
	// 16Mhz / 1024 = 15.625 kHz
		 
	switch(preScaler)
	{	case 1:		// CPU-Takt
			*TCCR |= (0<<CS02);
			*TCCR |= (0<<CS01);
			*TCCR |= (1<<CS00);
			return(1);	// beendet Funtkion -> kein "break" nötig um switch ... case ... zu beenden
			
		case 8:		// CPU-Takt / 8
			*TCCR |= (0<<CS02);
			*TCCR |= (1<<CS01);
			*TCCR |= (0<<CS00);
			return(1);	// beendet Funtkion -> kein "break" nötig um switch ... case ... zu beenden
			
		case 64:	// CPU-Takt / 64
			*TCCR |= (0<<CS02);
			*TCCR |= (1<<CS01);
			*TCCR |= (1<<CS00);
			return(1);	// beendet Funtkion -> kein "break" nötig um switch ... case ... zu beenden

		case 256:	// CPU-Takt / 256
			*TCCR |= (1<<CS02);
			*TCCR |= (0<<CS01);
			*TCCR |= (0<<CS00);
			return(1);	// beendet Funtkion -> kein "break" nötig um switch ... case ... zu beenden

		case 1024:	// CPU-Takt /1024
			*TCCR |= (1<<CS02);
			*TCCR |= (0<<CS01);
			*TCCR |= (1<<CS00);
			return(1);	// beendet Funtkion -> kein "break" nötig um switch ... case ... zu beenden

		default:
			return(0);	// beendet Funtkion -> kein "break" nötig um switch ... case ... zu beenden
	}
}