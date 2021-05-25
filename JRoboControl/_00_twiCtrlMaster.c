/*
 * _twiCtrl.c
 *
 * Created: 12.02.2021 21:38:10
 *  Author: jayst
 */ 

#include "_00_twiCtrlMaster.h"


uint8_t InitTwiM(uint32_t CPU_FREQ, uint32_t TWI_Bitrate, uint8_t TwiPreScaler)
{
	// Set TWI Master bitrate
	// F_SCL = (F_CPU) / (16 + 2(TWBR)*(PrescalerValue))
	// TWRB = ((F_CPU/F_SCL)-16)/(2*PreScaler)
	// expl: TWBR	= ((16e6/4e5)-16)/2*1 --> 400 kbit
	//				= ((40)-16)/2) = 12
	uint8_t twps = 0;
	
	// Set PreScaler Bits
	switch(TwiPreScaler)
	{
		case 1:		// PreScaler = 1
		 	TWSR &= ~(TWPS0<<1);
			TWSR &= ~(TWPS1<<1);
			twps = 1;
			break;
		case 4:		// PreScaler = 4
			TWSR |= (TWPS0<<1);
			TWSR &= ~(TWPS1<<1);
			twps = 4;
			break;
		case 16:	// PreScaler = 16
			TWSR &= ~(TWPS0<<1);
			TWSR |= (TWPS1<<1);
			twps = 16;
			break;
		case 64:	// PreScaler = 64
			TWSR |= (TWPS0<<1);
			TWSR |= (TWPS1<<1);
			twps = 64;
			break;
	}

	if(twps == 0)
		return FALSE;
	
	// Init TWI Master
	else
	{
		// TWRB = ((F_CPU/F_SCL)-16)/(2*PreScaler)
		TWBR = (uint8_t)((CPU_FREQ/TWI_Bitrate)-16)/(2*twps);
		if (TWBR < 11)
			return FALSE;
		else
			return TRUE;
	}
}

/*******************************************************
 Public Function: TwiMStart
 Purpose: Start the TWI Master Interface

 Input Parameter:
 	- uint8_t	Device address
 	- uint8_t	Type of required Operation:
				TWIM_READ: Read data from the slave
				TWIM_WRITE: Write data to the slave

 Return Value: uint8_t
  	- TRUE:		OK, TWI Master accessible
 	- FALSE:	Error in starting TWI Master

*******************************************************/
uint8_t TwiMStart (uint8_t Address, uint8_t TwiMode)
{
	uint8_t	twiMStatus = 0;
/////	Send START condition	/////
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
/////	Wait until transmission completed	/////
	while (!(TWCR & (1<<TWINT)));
/////	Check value of TWI Status Register. Mask prescaler bits	/////
	twiMStatus = TWSR & 0xF8;
	if ((twiMStatus != TWI_START) && (twiMStatus != TWI_REP_START)) 
		return TWIM_SEND_START_ERROR;

/////	Send device address	/////
	TWDR = (Address<<1) + TwiMode;
	TWCR = (1<<TWINT)|(1<<TWEN);
/////	Wait until transmission completed and ACK/NACK has been received	/////
	while (!(TWCR & (1<<TWINT)));

/////	Check value of TWI Status Register. Mask prescaler bits.	//////
	twiMStatus = TWSR & 0xF8;
	if ((twiMStatus != TWI_MTX_ADR_ACK) && (twiMStatus != TWI_MRX_ADR_ACK)) 
	{
		TwiMStop();
		return TWIM_SEND_MODE_ERROR;
	}

	return TWIM_SUCCESS;
}

/*******************************************************
 Public Function: TimMStop
 Purpose: Stop the TWI Master
 Input Parameter: None
 Return Value: None
*******************************************************/
void TwiMStop (void)
{
//////	Send stop condition	/////
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
/////	Wait until stop condition is executed and bus released	/////
	while (TWCR & (1<<TWINT));
}

/*******************************************************
 Public Function: TwiMWriteByte
 Purpose: Write a byte to the slave

 Input Parameter:
	- uint8_t	Address of the Slave
 	- uint8_t	Byte to be sent

 Return Value: uint8_t
 - TWIM_SUCCESS				(0x01)	- OK, data sent
 - TWIM_SEND_START_ERROR	(0x02)	- Error while sending "Start"
 - TWIM_SEND_MODE_ERROR		(0x03)	- Error while sending mode to slave
 - TWIM_SEND_DATA_ERROR		(0x04)	- Error while sending data
 *******************************************************/
uint8_t	TwiMWriteByte (uint8_t SlaveAdress, uint8_t Byte)
{
/////	Send Start and set Mode		/////
	uint8_t twiMStatus = TwiMStart(SlaveAdress, TWIM_WRITE);
	if( (twiMStatus == TWIM_SEND_START_ERROR) || (twiMStatus == TWIM_SEND_MODE_ERROR) )
	{
		TwiMStop();
		return twiMStatus;
	}

/////	Send data to the previously addressed device	/////
	TWDR = Byte;
	TWCR = (1<<TWINT)|(1<<TWEN);
/////	Wait until transmission completed	/////
	while (!(TWCR & (1<<TWINT)));

/////	Check value of TWI Status Register. Mask prescaler bits	//////
	twiMStatus = TWSR & 0xF8;
	if (twiMStatus != TWI_MTX_DATA_ACK) 
	{
		TwiMStop();
		return TWIM_SEND_DATA_ERROR;
	}

/////	Send Stop	//////
	TwiMStop();
	return TWIM_SUCCESS;
}

/*******************************************************
 Public Function: TwiMWriteByteInReg
 Purpose: Write a byte to the slave

 Input Parameter:
	- uint8_t	Address of the Slave
	- uint8_t	Register of the Slave	
 	- uint8_t	Byte to be sent

 Return Value: uint8_t
 - TWIM_SUCCESS				(0x01)	- OK, data sent
 - TWIM_SEND_START_ERROR	(0x02)	- Error while sending "Start"
 - TWIM_SEND_MODE_ERROR		(0x03)	- Error while sending mode to slave
 - TWIM_SEND_DATA_ERROR		(0x04)	- Error while sending data
 *******************************************************/
uint8_t	TwiMWriteByteInReg (uint8_t SlaveAdress, uint8_t Reg, uint8_t Byte)
{
	uint8_t data[2] = {Reg, Byte};
	uint8_t status = TwiMWriteData(SlaveAdress, data, 2);
	
	return status;
}

/*******************************************************
 Public Function: TwiMWriteData
 Purpose: Write date (a array of bytes) to the slave

 Input Parameter:
	- uint8_t  Adress of the Slave
 	- uint8_t*	Pointer to be the Buffer array
	- uint8_t  length of the data (no. of bytes)

 Return Value: uint8_t
	- TWIM_SUCCESS			(0x01)	- OK, data sent
	- TWIM_SEND_START_ERROR	(0x02)	- Error while sending "Start"
	- TWIM_SEND_MODE_ERROR	(0x03)	- Error while sending mode to slave
	- TWIM_SEND_DATA_ERROR	(0x04)	- Error while sending data
*******************************************************/
uint8_t	TwiMWriteData(uint8_t SlaveAdress, uint8_t* Data, uint8_t Length)
{
/*   // Sende Start
    TWCR=(1<<TWINT) | (1<<TWEN) | (1<<TWSTA);
    while (!(TWCR & (1<<TWINT)));
    uint8_t status=TWSR & 0xf8;

    if (status != 0x08 && status != 0x10) 
		return TWIM_SEND_START_ERROR;
        
    // Send Adresse (write mode)
	// Shift adress a Bit to left, space r/w flag
	SlaveAdress = SlaveAdress << 1;
    TWDR=SlaveAdress;
    TWCR=(1<<TWINT) | (1<<TWEN);

    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xf8) != 0x18) 
		return TWIM_SEND_MODE_ERROR;
*/        

/////	Send Start and set Mode		/////
	uint8_t twiMStatus = TwiMStart(SlaveAdress, TWIM_WRITE);
	if( (twiMStatus == TWIM_SEND_START_ERROR) || (twiMStatus == TWIM_SEND_MODE_ERROR) )
	{
		TwiMStop();
		return twiMStatus;
	}
		
/////	Send Data	/////
    while (Length>0) 
	{
	    TWDR=*((uint8_t*)Data);
	    TWCR=(1<<TWINT) | (1<<TWEN);
	    
		while (!(TWCR & (1<<TWINT)));
	    if ((TWSR & 0xf8) != 0x28) 
		{
			TwiMStop();
			return TWIM_SEND_DATA_ERROR;
		}
	    
		Data++;
	    Length--;
    }

/////	Send Stop	/////
	TwiMStop();
	return TWIM_SUCCESS;
}

/*******************************************************
 Public Function: TwiReadByte
 Purpose: Write a byte to the slave

 Input Parameter:
	- uint8_t	Address of the Slave
	- uint8_t	Register to be read
 	- uint8_t	Byte to be sent

 Return Value: uint8_t
 - TWIM_SUCCESS				(0x01)	- OK, data sent
 - TWIM_SEND_START_ERROR	(0x02)	- Error while sending "Start"
 - TWIM_SEND_MODE_ERROR		(0x03)	- Error while sending mode to slave
 - TWIM_SEND_DATA_ERROR		(0x04)	- Error while sending data
 - TWIM_READ_DATA_ERROR		(0x05)	- Error while reading data
 *******************************************************/
uint8_t	TwiMReadByte(uint8_t SlaveAdress, uint8_t SlaveReg, uint8_t* Byte)
{
/////	Send Start and set Write-Mode		/////
	uint8_t twiMStatus = TwiMStart(SlaveAdress, TWIM_WRITE);
	if( (twiMStatus == TWIM_SEND_START_ERROR) || (twiMStatus == TWIM_SEND_MODE_ERROR) )
	{
		TwiMStop();
		return twiMStatus;
	}
/////	Set Register Address	/////
	TWDR = SlaveReg;
	TWCR = (1<<TWINT)|(1<<TWEN);
/////	Wait until transmission completed	/////
	while (!(TWCR & (1<<TWINT)));
/////	Check value of TWI Status Register. Mask prescaler bits	//////
	twiMStatus = TWSR & 0xF8;
	if (twiMStatus != TWI_MTX_DATA_ACK)
	{
		TwiMStop();
		return TWIM_SEND_DATA_ERROR;
	}

/////	Send Start and set Read-Mode		/////
	twiMStatus = TwiMStart(SlaveAdress, TWIM_READ);
	if( (twiMStatus == TWIM_SEND_START_ERROR) || (twiMStatus == TWIM_SEND_MODE_ERROR) )
	{	
		TwiMStop();
		return twiMStatus;
	}
/////	Read Byte (sent ReadNack due to read only one byte)	/////
	*Byte = TwiMReadNack();
//	TWCR=(1<<TWINT) | (1<<TWEN);
//	while (!(TWCR & (1<<TWINT)));
/////	Check value of TWI Status Register. Mask prescaler bits	//////
	twiMStatus = TWSR & 0xF8;
	if (twiMStatus!=TWI_MRX_DATA_ACK && twiMStatus != TWI_MRX_DATA_NACK) 
	{
		*Byte = 0;
		TwiMStop();
		return TWIM_READ_DATA_ERROR;
	}
	
/////	Send Stop	/////
	TwiMStop();
	return TWIM_SUCCESS;
}

/*******************************************************
 Public Function: TwiReadData
 Purpose: Write a byte to the slave

 Input Parameter:
	- uint8_t	Address of the Slave
	- uint8_t	Register to be read
 	- uint8_t	Receive buffer
	- uint8_t	No of bytes to read
	- uint8_t	No of received bytes

 Return Value: uint8_t
 - TWIM_SUCCESS				(0x01)	- OK, data sent
 - TWIM_SEND_START_ERROR	(0x02)	- Error while sending "Start"
 - TWIM_SEND_MODE_ERROR		(0x03)	- Error while sending mode to slave
 - TWIM_SEND_DATA_ERROR		(0x04)	- Error while sending data
 - TWIM_READ_DATA_ERROR		(0x05)	- Error while reading data
 *******************************************************/
uint8_t	TwiMReadDats(uint8_t SlaveAdress, uint8_t SlaveReg, uint8_t* Data, uint8_t Length, uint8_t* RcvCount)
{
	*RcvCount = 0;
/////	Send Start and set Write-Mode		/////
	uint8_t twiMStatus = TwiMStart(SlaveAdress, TWIM_WRITE);
	if( (twiMStatus == TWIM_SEND_START_ERROR) || (twiMStatus == TWIM_SEND_MODE_ERROR) )
	{
		TwiMStop();
		return twiMStatus;
	}
/////	Set Register Address	/////
	TWDR = SlaveReg;
	TWCR = (1<<TWINT)|(1<<TWEN);
/////	Wait until transmission completed	/////
	while (!(TWCR & (1<<TWINT)));
/////	Check value of TWI Status Register. Mask prescaler bits	//////
	twiMStatus = TWSR & 0xF8;
	if (twiMStatus != TWI_MTX_DATA_ACK)
	{
		TwiMStop();
		return TWIM_SEND_DATA_ERROR;
	}

/////	Send Start and set Read-Mode		/////
	twiMStatus = TwiMStart(SlaveAdress, TWIM_READ);
	if( (twiMStatus == TWIM_SEND_START_ERROR) || (twiMStatus == TWIM_SEND_MODE_ERROR) )
	{
		TwiMStop();
		return twiMStatus;
	}
/////	Read Data (sent ReadNack for the last bytw	/////
	while (Length > 0) 
	{
		if (Length==1)
			*Data = TwiMReadNack();
		else 
			*Data = TwiMReadAck();
/////	Check value of TWI Status Register. Mask prescaler bits	//////
		twiMStatus = TWSR & 0xF8;
		if (twiMStatus!=TWI_MRX_DATA_ACK && twiMStatus != TWI_MRX_DATA_NACK)
		{
			*Data = 0;
			TwiMStop();
			return TWIM_READ_DATA_ERROR;
		}
			
		Length++;
		Length--;
		RcvCount++;
	}


/////	Send Stop	/////
	TwiMStop();
	return TWIM_SUCCESS;
}

/*******************************************************
 Public Function: TwiMReadAck
 Purpose: Read a byte from the slave and request next byte

 Input Parameter: None

 Return Value: uint8_t
  	- uint8_t	Read byte
*******************************************************/
uint8_t	TwiMReadAck (void)
{
//////	Send read condition	/////
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
/////	Wait until transmission completed	/////
	while (!(TWCR & (1<<TWINT)));

	return TWDR;
}

/*******************************************************
 Public Function: TwiMReadNAck
 Purpose: Read the last byte from the slave

 Input Parameter: None

 Return Value: uint8_t
  	- uint8_t	Read byte

*******************************************************/
uint8_t	TwiMReadNack (void)
{
//////	Send last read condition	/////
	TWCR = (1<<TWINT)|(1<<TWEN);
/////	Wait until transmission completed	/////
	while(!(TWCR & (1<<TWINT)));
		
	return TWDR;
}


/* TWI - Register	////////////////////////////////////////////////////////
// TWI Bit Rate Register	////////////////////////////////////////////////
TWBR	; // Bite rate byte
TWBR7	;	// Bit 7
TWBR6	;	// Bit 6
TWBR5	;	// Bit 5
TWBR4	;	// Bit 4
TWBR3	;	// Bit 3
TWBR2	;	// Bit 2
TWBR1	;	// Bit 1
TWBR0	;	// Bit 0
// bitRate = F_CPU / ( 16 + 2*(TWBR) + 4^(TWPS) ) -> TWPS - Prescaler s. TWSR

// TWI Control Register	////////////////////////////////////////////////////
TWCR ; // Control byte
TWINT ; // Bit 7 - interrupt flag - is set when ready, twi unit is stopped, to reset set true
TWEA ; // Bit 6 - enable acknowledge bit - set to send ACK-bit after data or slave adresse recieve, have to be set to be a slave
TWSTA ; // Bit 5 - START condition bit - set a start condition wehen bus is free
TWSTO ; // Bit 4 - STOP condition bit - set a stop condition
TWWC ; // Bit 3 - write collision flag - is set, when write access on data register an twi is busy
TWEN ; // Bit 2 - ?
// - ; // Bit 1
TWIE ; // Bit 0 - interrupt enable - if it´s set, it starts an interrupt when TWINT is set

// TWI Status Register	////////////////////////////////////////////////////
TWSR	; // Status byte
TWS7	; // Bit 7	- bit7 - bit 3 -> state bits - look at manuall
TWS6	; // Bit 6
TWS5	; // Bit 5
TWS4	; // Bit 4
TWS3	; // Bit 3
// -	; // Bit 2
TWPS1	; // Bit 1 - Prescaler bit1
TWPS0	; // Bit 0 - Prescaler bit0

// TWI Data Register	////////////////////////////////////////////////////
TWDR ; // Data byte
TWD7	; // Bit 7
TWD6	; // Bit 6
TWD5	; // Bit 5
TWD4	; // Bit 4
TWD3	; // Bit 3
TWD2	; // Bit 2
TWD1	; // Bit 1
TWD0	; // Bit 0

// TWI Adress Register	////////////////////////////////////////////////////
TWAR	; // Adress byte
TWA6	; // Bit 7 - bit7 - bit1 -> 7bit slave adress of ATmega
TWA5	; // Bit 6
TWA4	; // Bit 5
TWA3	; // Bit 4
TWA2	; // Bit 3
TWA1	; // Bit 2
TWA0	; // Bit 1
TWGCE	; // Bit 0 - general call recognition enable - ATmega reaction at general call enable


-------------------------------------------------------
Program Example
-------------------------------------------------------
#include "General.h"
#include "TWI_Slave.h"

int main (void)
{
	uint8_t		i=0;
	uint8_t		j=0;
	uint8_t		byte[8];
	uint8_t		TWIS_ResonseType;

	// Clear any interrupt
	cli ();

	// Wait 0.5 second for POR
	Delay_ms (500);

	// Initiate the TWI slave interface
	TWIS_Init (15, 100000);

	// Endless loop
	while (1)
	{

		// Is something to do for the TWI slave interface ?
		if (TWIS_ResonseRequired (&TWIS_ResonseType))
		{
			switch (TWIS_ResonseType)
			{
				// TWI requests to read a byte from the master.
				// It is implicitely assumed, that the master
				// sends 8 bytes.

				case TWIS_ReadBytes:
				for (i=0;i<7;i++)
				{
					byte[0] = TWIS_ReadAck ();
				}
				byte[7] = TWIS_ReadNack ();
				TWIS_Stop ();
				break;

				// TWI requests to write a byte to the master.
				// It is implicitely assumed, that the master
				// is prepared to receive 8 bytes.

				case TWIS_WriteBytes:
				for (i=0;i<8;i++)
				{
					TWIS_Write (j++);
				}
				TWIS_Stop ();
				break;
			}
		}
		// Do something else ....... e.g.:
		i++;
	}
	return 0;
}
*/
