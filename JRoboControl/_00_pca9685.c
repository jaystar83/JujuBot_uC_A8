/*
 * _pca9685.c
 *
 * Created: 11.02.2021 19:44:42
 *  Author: jayst
 */ 

#include "_00_pca9685.h"
#include "_00_twiCtrlMaster.h"

uint8_t _PCA9685Addr;
uint8_t _AutoIncrement(uint8_t Mode);



//uint8_t initPCA9685(uint8_t Mode, uint32_t Fcpu, uint32_t TwiBaud, uint8_t PreScaler)
//uint8_t initPCA9685(const PCA9685Data* Pca9685Data)
uint8_t InitPCA9685(uint8_t Mode, uint32_t Fcpu, uint32_t TwiBaud, uint8_t PreScaler, uint8_t SlaveAddr)
{
	//Wire.begin();
	_PCA9685Addr = SlaveAddr;
	
	if( !(InitTwiM(Fcpu, TwiBaud, PreScaler)) )
		return FALSE;
	
	/* If not already, put device to sleep. This is required to change the Prescaller value */
	PCA9685_Sleep(TRUE);

	
	/* Set register addressing mode to auto increment to speed up writing to 		 multiple registers */
	_AutoIncrement(TRUE);
	
	/* If the library has been initialised for driving servos set the period
	   frequency (prescaller) for 50Hz (2ms) period. */
	if(Mode == SERVO_MODE)
	{
		OutputOnTime(0, 0);
		OutputOffTime(0, 0);
		SetPeriodFreq(SERVO_FREQ);
		;
	}
	
	return TRUE;
}

/* Sets the sleep state of the device where:

   Mode is the required state. Valid values are:
		true 	(puts the device to sleep)
		false 	(brings the device out of sleep mode)
   
   If the device is woken up from a previously set sleep state then this function will wait 500us for the oscillator to stabilise. */
uint8_t PCA9685_Sleep(uint8_t Mode)
{
	uint8_t data;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE1, &data);

	if(status != TWIM_SUCCESS)
		return FALSE;
		
	data = (data & MODE1_SLEEP_MASK) | (Mode << MODE1_SLEEP_BIT);
	
	status = TwiMWriteByteInReg(_PCA9685Addr, MODE1, data);
//	I2CWriteReg(MODE1, Data);
	if(status != TWIM_SUCCESS)
		return FALSE;

	/* Check if device was put in to sleep mode and if so give the oscillator time to restart */
	if(data & ~MODE1_RESTART_MASK)
	{
		_delay_us(500);
		data |= (1 << MODE1_RESTART_BIT);
	}

	return TRUE;
}

/* Enables the devices register address auto increment mode */
uint8_t _AutoIncrement(uint8_t Mode)
{
	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE1, &data);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	data = (data & MODE1_AI_MASK) | (Mode << MODE1_AI_BIT);
	status = TwiMWriteByteInReg( _PCA9685Addr, MODE1, data);
	if( status != TWIM_SUCCESS )
		return FALSE;
		
	return TRUE;
}


/* Sets the period frequency (prescaller) in Hz where:
   Freq is the required frequency of on cycle. Valid values are:
		24 (24Hz) to 1526 (1.526KHz)
   
   These values are based on the devices internal 25MHz oscillator (default). 
   If using an external clock you will need to adjust the value for OSC_FREQ found in
   the HCPCA9685.h header file */
uint8_t SetPeriodFreq(uint32_t Freq)
{
	uint8_t prescaler = (OSC_FREQ / (4096L * Freq)) - 1;
	uint8_t status = TwiMWriteByteInReg(_PCA9685Addr, PRE_SCALE, prescaler );

	//Serial.println(Data);
	if(status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Sets the value for the prescaller where:
   Period is the required period for one cycle. Valid values are:
		0x03 to 0xFF 
   
   The prescaller value can be determined with the following formula:
   prescale value = (25MHz / (4096 * period frequency in Hz)) - 1 */
uint8_t SetPreScaller(uint8_t Period)
{
	uint8_t status = TwiMWriteByteInReg(_PCA9685Addr, PRE_SCALE, Period);
	if( status != TWIM_SUCCESS )
		return FALSE;
		
	return TRUE;
}


/* When the library has been initiliased to servo mode this function sets the
   on time for the servo where:
   
   Chan is the PWM output pin to change. Valid values are
		0 (PWM output pin 0) to 15 (PWM output pin 15)
		
   Pos is the required position of the servo. Valid values are
		0 (servo fully anti-clockwise) to 480 (servo fully clockwise)
		
	The maximum value is based in the default servo SERVO_TRIM_MAX and SERVO_TRIM_MIN trim settings found in the pca9685.h header file.
	These default to 590 for SERVO_TRIM_MAX and 110 for SERVO_TRIM_MIN.
	Thay can be adjusted to match the minimum and maximum end-stop positions for your servo(s). If these values are changed then the new max value for Pos can 
	be calculated as (SERVO_TRIM_MAX - SERVO_TRIM_MIN) */
uint8_t Servo(uint8_t Chan, uint16_t Pos)
{
	//unsigned int Offset = (unsigned int)Chan << 8;
	Pos += SERVO_TRIM_MIN;
	if (Pos > SERVO_TRIM_MAX)
		Pos = SERVO_TRIM_MAX;
  
	uint8_t status = Output(Chan, 0 /*Offset*/, Pos /*+ Offset*/);
	
	return status; // TRUE -> Success, FALSE -> Error
}

/* Sets the turn ON time and OFF time for one of the 15 PWM outputs where:
   
   Chan is the PWM output pin to change. Valid values are
		0 (PWM output pin 0) to 15 (PWM output pin 15)
		
   On_Time is the point at which to turn the output ON within the cycle period. Valid values are
		0 to 4095 (decimal)

    Off_Time is the point at which to turn the output OFF within the cycle period. Valid values are
		0 to 4095 (decimal)
*/
uint8_t Output(uint8_t Chan, uint16_t On_Time, uint16_t Off_Time)
{
	if(Chan > 15)
		Chan = 15;

	Chan = LED0_ON_L + (Chan << 2);
  
	uint8_t data[5] = {	Chan,
						On_Time & 0xFF,
						On_Time >> 8,
						Off_Time & 0xFF,
						Off_Time >> 8
					};

	uint8_t status = TwiMWriteData(_PCA9685Addr, data, 5);
	if( status != TWIM_SUCCESS)
		return FALSE;
		
	return TRUE;  	
}


/* Sets the turn ON time and OFF time for ALL of the 15 PWM outputs where:

   On_Time is the point at which to turn the outputs ON within the cycle period. Valid values are
		0 to 4095 (decimal)

    Off_Time is the point at which to turn the outputs OFF within the cycle period. Valid values are
		0 to 4095 (decimal) */
uint8_t OutputAll(uint16_t On_Time, uint16_t Off_Time)
{
	uint8_t data[5] = {	ALL_LED_ON_L,
						On_Time & 0xFF,
						On_Time >> 8,
						Off_Time & 0xFF,
						Off_Time >> 8
					};

	uint8_t status = TwiMWriteData(_PCA9685Addr, data, 5);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Sets the turn ON time for one of the 15 PWM outputs where:
   
   Chan is the PWM output pin to change. Valid values are
		0 (PWM output pin 0) to 15 (PWM output pin 15)
		
   Time is the point at which to turn the output on within the cycle period. Valid values are
		0 to 4095 (decimal) */
uint8_t OutputOnTime(uint8_t Chan, uint16_t Time)
{
	if(Chan > 15)
		Chan = 15;
	Chan = LED0_ON_L + (Chan << 2);

	uint8_t data[3] = {	Chan,
						Time & 0xFF,
						Time >> 8
					};

	uint8_t status = TwiMWriteData(_PCA9685Addr, data, 3);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Sets the turn OFF time for one of the 15 PWM outputs where:
   
   Chan is the PWM output channel pin to change. Valid values are
		0 (PWM output pin 0) to 15 (PWM output pin 15)
		
   Time is the point at which to turn the output off within the cycle period. Valid values are
		0 to 4095 (decimal) */
uint8_t OutputOffTime(uint8_t Chan, uint16_t Time)
{
	if(Chan > 15)
	Chan = 15;
	Chan = LED0_OFF_L + (Chan << 2);

	uint8_t data[3] = {	Chan,
		Time & 0xFF,
		Time >> 8
	};

	uint8_t status = TwiMWriteData(_PCA9685Addr, data, 3);
	if( status != TWIM_SUCCESS)
	return FALSE;
	
	return TRUE;
}

/* Sets the state of the 16 PWM outputs when the OE pin is driven high where:
	
	State is the required state of the output pins. Valid values are:
		OUTNE_LOW				Output pins are pulled low when OE = 1
		OUTNE_HIGH 				Output pins are pulled high when OE = 1
		OUTNE_HIGH_IMPEDANCE 	Output pins are high-impedance when OE = 1 */
uint8_t OutputNotEnableState(uint8_t State)
{
	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE2, &data);
	if (status != TWIM_SUCCESS)
		return FALSE;
	
	data = (data & MODE2_OUTNE_MASK) | (State << MODE2_OUTNE_BIT);
	status = TwiMWriteByteInReg( _PCA9685Addr, MODE2, data);
	if(status != TWIM_SUCCESS)
		return FALSE;
		
	return TRUE;
}

/* Sets the driver type for the PWM outputs where:

	Mode is the required type. Valid values are:
		OUTDRV_OPEN_DRAIN	The 16 PWM outputs are configured with an open-drain structure.
		OUTDRV_TOTEM_POLE	The 16 PWM outputs are configured with a totem pole structure. */
uint8_t OutputDrivers(uint8_t Mode)
{
	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE2, &data);
	if( status != TWIM_SUCCESS )
		return FALSE;
		
	data = (data & MODE2_OUTDRV_MASK) | (Mode << MODE2_OUTDRV_BIT);
	status = TwiMWriteByteInReg(_PCA9685Addr, MODE2, data);
	if( status != TWIM_SUCCESS)
		return FALSE;
		
	return TRUE;
}

/* Sets the point at which the 16 PWM outputs change state where:
	
	Mode is the required state. Valid values are:
		OCH_STOP		Outputs change on I2C STOP command.
		OCH_ACK			Outputs change on I2C ACK. */
uint8_t OCH(uint8_t Mode)
{
	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE2, &data);
	if( status != TWIM_SUCCESS)
		return FALSE;
		
	data = (data & MODE2_OCH_MASK) | (Mode << MODE2_OCH_BIT);
	status = TwiMWriteByteInReg(_PCA9685Addr, MODE2, data);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Inverts the state of the PWM output pins where:
	
	Mode is the require state. Valid values are:
		false	The PWM output pins are not inverted.
		true	The PWM output pins are inverted. */
uint8_t Invert(uint8_t Mode)
{
	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE2, &data);
	if( status != TWIM_SUCCESS)
		return FALSE;

	data = (data & MODE2_INVRT_MASK) | (Mode << MODE2_INVRT_BIT);
	status = TwiMWriteByteInReg(_PCA9685Addr, MODE2, data);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Sets whether or not the device will respond the I2C sub address 1 where:

	Mode sets the required state. Valid values are:
		false		The device will not respond to I2C sub address 1.
		true		The device will respond to I2C sub address 1. */
uint8_t Enable_Sub1(uint8_t Mode)
{
	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE1, &data);
	if( status != TWIM_SUCCESS)
		return FALSE;

	data = (data & MODE1_SUB1_MASK) | (Mode << MODE1_SUB1_BIT);
	status = TwiMWriteByteInReg(_PCA9685Addr, MODE1, data);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Sets whether or not the device will respond the I2C sub address 2 where:

	Mode sets the required state. Valid values are:
		false		The device will not respond to I2C sub address 2.
		true		The device will respond to I2C sub address 2. */
uint8_t Enable_Sub2(uint8_t Mode)
{
	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE1, &data);
	if( status != TWIM_SUCCESS)
		return FALSE;

	data = (data & MODE1_SUB2_MASK) | (Mode << MODE1_SUB2_BIT);
	status = TwiMWriteByteInReg(_PCA9685Addr, MODE1, data);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Sets whether or not the device will respond the I2C sub address 3 where:

	Mode sets the required state. Valid values are:
		false		The device will not respond to I2C sub address 3.
		true		The device will respond to I2C sub address 3. */
uint8_t Enable_Sub3(uint8_t Mode)
{
	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE1, &data);
	if( status != TWIM_SUCCESS)
		return FALSE;

	data = (data & MODE1_SUB3_MASK) | (Mode << MODE1_SUB3_BIT);
	status = TwiMWriteByteInReg(_PCA9685Addr, MODE1, data);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Sets whether or not the device will respond the I2C ALLCALL address where:

	Mode sets the required state. Valid values are:
		false		The device will not respond to I2C ALLCALL address.
		true		The device will respond to I2C ALLCALL address. */
uint8_t Enable_AllCall(uint8_t Mode)
{
	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE1, &data);
	if( status != TWIM_SUCCESS)
		return FALSE;

	data = (data & MODE1_ALLCALL_MASK) | (Mode << MODE1_ALLCALL_BIT);
	status = TwiMWriteByteInReg(_PCA9685Addr, MODE1, data);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Sets the I2C address for one of the 3 sub addresses where:
	
	SubAddress is one of the 3 sub addresses to set the I2C address for. Valid values are:
		SUBADR1		Selects sub address 1.
		SUBADR2		Selects sub address 2.
		SUBADR3		Selects sub address 3.
		
	Address is and 8 bit byte representing the I2C address to set the selected sub address too. Only the 7 LSBs representing the I2C-bus sub address are valid. */
uint8_t SetSubAddress(uint8_t SubAddress, uint8_t Address)
{
	uint8_t status = TwiMWriteByteInReg(_PCA9685Addr, SubAddress, ((0x7F & Address) << 1));
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}

/* Sets the I2C address for the ALLCALL address where:
		
	Address is and 8 bit byte representing the I2C address to set the ALLCALL  address too. Only the 7 LSBs representing the I2C-bus ALLCALL address are valid. */
uint8_t SetAllCallAddress(uint8_t Address)
{
	uint8_t status = TwiMWriteByteInReg(_PCA9685Addr, ALLCALLADR, ((0x7F & Address) << 1));
	if( status != TWIM_SUCCESS)
		return FALSE;
		
	return TRUE;
}

/* Sets external clock mode allowing the device to be driven by an external clock source. When in external clock mode, it can only be cleared by recycling the power */
uint8_t ExtClk(void)
{
	PCA9685_Sleep(TRUE);

	uint8_t data = 0;
	uint8_t status = TwiMReadByte(_PCA9685Addr, MODE1, &data);
	if( status != TWIM_SUCCESS)
		return FALSE;

	data = data | (1 << MODE1_EXTCLK_BIT) | (1 << MODE1_SLEEP_BIT);
	status = TwiMWriteByteInReg(_PCA9685Addr, MODE1, data);
	if( status != TWIM_SUCCESS)
		return FALSE;
	
	return TRUE;
}
