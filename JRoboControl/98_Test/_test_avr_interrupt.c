/*
 * _test_avr_interrupt.c
 *
 * Created: 05.06.2021 23:04:42
 *  Author: jayst
 */ 

#include "defines.h"
#include "_test_avr_interrupt"

/*
hier muss nach aufruf von sei() ein threat für die timer ISR gestartet werden
-> die timer defines müssen in die avrio_test.h
-> diese müssen hier ausgewertet werden um timer ISR zu realisieren

cli() stoppt den threat
*/