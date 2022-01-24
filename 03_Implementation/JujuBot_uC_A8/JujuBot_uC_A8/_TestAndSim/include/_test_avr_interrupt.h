/*
 * _test_avr_interrupt.h
 *
 * Created: 05.06.2021 19:50:48
 *  Author: jaystar
 */ 


#ifndef _TEST_AVR_INTERRUPT_H_
#define _TEST_AVR_INTERRUPT_H_

#define ISR(isv) 	\
	void isv(void)	

#ifndef TIMER0_COMPA_vect
	#define TIMER0_COMPA_vect	isr_t0_compA
#endif
void isr_t0_compA(void);

#ifndef TIMER2_COMPA_vect 
	#define TIMER2_COMPA_vect	isr_t2_compA
#endif
void isr_t2_compA(void);

//void sei(void);
void cli(void);
#endif /* _TEST_AVR_INTERRUPT_H_ */