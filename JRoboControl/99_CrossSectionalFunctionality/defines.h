/*
 * defines.h
 *
 * Created: 21.03.2021 19:14:15
 *  Author: jayst
 */ 



#ifndef _DEFINES_H_
#define _DEFINES_H_

/***********************************************************************
	DEFINITIONS
***********************************************************************/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

#ifndef BAUD
	#define BAUD 115000UL
#endif
#ifndef SERIAL_MESSAGE_LENGTH
	#define SERIAL_MESSAGE_LENGTH 12
#endif
#ifndef SERIAL_RxISR_ENABLE
	#define SERIAL_RxISR_ENABLE TRUE
#endif

#ifndef CRC_CHECK_OFF
	#define CRC_CHECK_OFF
#endif

#ifndef TWI_BAUD
	#define TWI_BAUD 400000UL
#endif
#ifndef TWI_PRESCALER
	#define TWI_PRESCALER 1
#endif
#ifndef PCA9685_ADDR
	#define PCA9685_ADDR 0x40
#endif

#ifndef WATCHDOG_ELEMENTS
	#define WATCHDOG_ELEMENTS 3
#endif
#ifndef SERIAL_COM_WD_TICKS 
	#define SERIAL_COM_WD_TICKS 255
#endif

#ifndef SERVO_0_INIT
	#define SERVO_CNT 6
	
	#define SERVO_0_INIT 185	// (0x00D2) Head - middle position
	#define SERVO_0_MIN 110		// (0x006E) Head - left
	#define SERVO_0_MAX 310		// (0x0136)	Head - right
	#define SERVO_1_INIT 288	// (0x0120) Head - init / down
	#define SERVO_1_MIN 235		// (0x00EB) Head - up
	#define SERVO_1_MAX 288		// (0x0120) Head - down
	#define SERVO_2_INIT 210	// (0x00D2) Middle position
	#define SERVO_2_MIN 110		// (0x006E) left
	#define SERVO_2_MAX 310		// (0x0136)	right
	#define SERVO_3_INIT 210	// (0x00D2) Middle position
	#define SERVO_3_MIN 110		// (0x006E) left
	#define SERVO_3_MAX 310		// (0x0136)	right
	#define SERVO_4_INIT 210	// (0x00D2) Middle position
	#define SERVO_4_MIN 110		// (0x006E) left
	#define SERVO_4_MAX 310		// (0x0136)	right
	#define SERVO_5_INIT 210	// (0x00D2) Middle position
	#define SERVO_5_MIN 110		// (0x006E) left
	#define SERVO_5_MAX 310		// (0x0136)	right
#endif

#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif

/***	Dev output activation	***************************************/
#ifndef DEV_SERIAL_OUTPUT
	#define  DEV_SERIAL_OUTPUT TRUE
#endif

/***	Servo Cntrlr Board available	*******************************/
#ifndef SERVEO_CTRL_BOARD_AVAILABLE
	#define  SERVEO_CTRL_BOARD_AVAILABLE FALSE
#endif

/***********************************************************************
	TEST MODIFICATIONS
***********************************************************************/
#ifndef SW_TEST_ACTIVE
	#define  SW_TEST_ACTIVE FALSE
#endif	

#endif // _DEFINES_H_
