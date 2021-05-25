/*
 * _pca9685.h
 *
 * Created: 11.02.2021 19:45:02
 *  Author: jayst
 */ 


#ifndef _PCA9685_H_
#define _PCA9685_H_

/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "_99_defines.h"

/***********************************************************************
	INCLUDEES
***********************************************************************/
#if( SW_TEST_ACTIVE )
	#include "_test_avrio.h"
	#include "_test_util_delay.h"
#else
	#include <avr/io.h> // AVR types
	#include <util/delay.h>
#endif

/******************************************************************************
 *							       USER SETTINGS 							  *
 *****************************************************************************/ 

/* - set the minimum and maximum trim values for the servo(s)
   - adjust these values if the servo(s) are either hitting 
     their end-stops, or are not able to rotate to the min or max ends of the 
     servo. */
   
#define SERVO_TRIM_MIN 110
#define SERVO_TRIM_MAX 590
//#define SERVO_TRIM_MIN 204
//#define SERVO_TRIM_MAX 408
 
 
/* PCA9685 Clock fequnecy
   If using an external clock change this value to match the clocks frequency 
   in Hz */
#define OSC_FREQ 25000000L

/****************************************************************************/ 

/* Register address and bit positions/masks for the devices internal registers */
#define MODE1 0x00
#define MODE1_ALLCALL_BIT 0
#define MODE1_ALLCALL_MASK ~(1 << MODE1_ALLCALL_BIT)
#define MODE1_SUB3_BIT 1
#define MODE1_SUB3_MASK ~(1 << MODE1_SUB3_BIT)
#define MODE1_SUB2_BIT 2
#define MODE1_SUB2_MASK ~(1 << MODE1_SUB2_BIT)
#define MODE1_SUB1_BIT 3
#define MODE1_SUB1_MASK ~(1 << MODE1_SUB1_BIT)
#define MODE1_SLEEP_BIT 4
#define MODE1_SLEEP_MASK ~(1 << MODE1_SLEEP_BIT)
#define MODE1_AI_BIT 5
#define MODE1_AI_MASK ~(1 << MODE1_AI_BIT)
#define MODE1_EXTCLK_BIT 6
#define MODE1_EXTCLK_MASK ~(1 << MODE1_EXTCLK_BIT)
#define MODE1_RESTART_BIT 7
#define MODE1_RESTART_MASK ~(1 << MODE1_RESTART_BIT)

#define MODE2 0x01
#define MODE2_OUTNE_BIT 0
#define MODE2_OUTNE_MASK ~(3 << MODE2_OUTNE_BIT)
#define MODE2_OUTDRV_BIT 2
#define MODE2_OUTDRV_MASK ~(1 << MODE2_OUTDRV_BIT)
#define MODE2_OCH_BIT 3
#define MODE2_OCH_MASK ~(1 << MODE2_OCH_BIT)
#define MODE2_INVRT_BIT 4
#define MODE2_INVRT_MASK ~(1 << MODE2_INVRT_BIT)
#define OCH_ACK 1
#define OCH_STOP 0
#define OUTDRV_TOTEM_POLE 1
#define OUTDRV_OPEN_DRAIN 1
#define OUTNE_LOW 0
#define OUTNE_HIGH 1
#define OUTNE_HIGH_IMPEDANCE 2

#define SUBADR1 0x02
#define SUBADR2 0x03
#define SUBADR3 0x04
#define ALLCALLADR 0x05
#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09
#define ALL_LED_ON_L 0xFA
#define ALL_LED_ON_H 0xFB
#define ALL_LED_OFF_L 0xFC
#define ALL_LED_OFF_H 0xFD
#define PRE_SCALE 0xFE

#define SERVO_FREQ 50
#define DEFAULT_MODE 0
#define SERVO_MODE 1


uint8_t InitPCA9685(uint8_t Mode, uint32_t Fcpu, uint32_t TwiBaud, uint8_t PreScaler, uint8_t SlaveAddr);
uint8_t PCA9685_Sleep(uint8_t Mode);
uint8_t SetPeriodFreq(uint32_t Freq);
uint8_t SetPreScaller(uint8_t Period);
uint8_t Servo(uint8_t Chan, uint16_t Pos);
uint8_t Output(uint8_t Chan, uint16_t On_Time, uint16_t Off_Time);
uint8_t OutputAll(uint16_t On_Time, uint16_t Off_Time);
uint8_t OutputOnTime(uint8_t Chan, uint16_t Time);
uint8_t OutputOffTime(uint8_t Chan, uint16_t Time);
uint8_t OutputNotEnableState(uint8_t State);
uint8_t OutputDrivers(uint8_t Mode);
uint8_t OCH(uint8_t Mode);
uint8_t Invert(uint8_t Mode);
uint8_t Enable_Sub1(uint8_t Mode);
uint8_t Enable_Sub2(uint8_t Mode);
uint8_t Enable_Sub3(uint8_t Mode);
uint8_t Enable_AllCall(uint8_t Mode);
uint8_t SetSubAddress(uint8_t SubAddress, uint8_t Address);
uint8_t SetAllCallAddress(uint8_t Address);
uint8_t ExtClk(void);

#endif /* _PCA9685_H_ */
