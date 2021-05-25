/*
 * timerConfig.h
 *
 * Created: 26.04.2016 08:03:06
 *  Author: jaystar
 */ 

/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "_99_defines.h"

/***********************************************************************
	ICNLUDES
***********************************************************************/
#if( SW_TEST_ACTIVE )
	#include "_test_avrio.h"
#else
	#include <avr/io.h>
#endif


#ifndef TIMERCONFIG_H_
#define TIMERCONFIG_H_ 1

unsigned char setPreScaler(uint16_t, unsigned char*);

/////////////////////////////////////////////////////////////////////
////	8-Bit Timer/Counter	/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

/////	TIMER 0	////////////////////////////////////////////////////
// TCCR0 - Timer/Counter Control Register für Timer 0 (8-Bit)
void configTimer0(uint8_t Mode, uint16_t PreScale, uint8_t OCR);

/////	TIMER 2	////////////////////////////////////////////////////		
// TCCR2 - Timer/Counter Control Register für Timer 2 (8-Bit)
void configTimer2(uint8_t Mode, uint16_t PreScale, uint8_t OCR) ;
	
/////////////////////////////////////////////////////////////////////////
/////	16-Bit Timer/Counter	/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

// TCCR1 - Timer/Counter Control Register für Timer 1 (16-Bit)
void configTimer1(uint8_t Mode, uint16_t PreScale, uint16_t OCR);

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
unsigned char setPreScaler(uint16_t preScaler, unsigned char* TCCR);

#endif /* TIMERCONFIG_H_ */