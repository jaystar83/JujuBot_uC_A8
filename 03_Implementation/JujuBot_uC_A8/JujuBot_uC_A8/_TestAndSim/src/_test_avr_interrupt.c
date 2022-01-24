/*
 * _test_avr_interrupt.c
 *
 * Created: 05.06.2021 23:04:42
 *  Author: jayst
 */ 

#include "defines.h"
#include "_test_avr_interrupt.h"

void isr_t0_compA(void)
{
	
};

/*
void sei()
 {
     ;
 }
 */
/*
hier muss nach aufruf von sei() ein threat f�r die timer ISR gestartet werden
-> die timer defines m�ssen in die avrio_test.h
-> diese m�ssen hier ausgewertet werden um timer ISR zu realisieren

cli() stoppt den threat
*/