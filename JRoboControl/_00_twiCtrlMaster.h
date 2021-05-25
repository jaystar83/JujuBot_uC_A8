/*
 * _twiCtrl.h
 *
 *	V_0.0.1
 *
 *  Author: jaystar
 *
 * 28.08.2017 07:51:11:
 *		- Created
 */ 

/***********************************************************************
	GLOBAL DEFINITIONS
***********************************************************************/
#include "_99_defines.h"

/***********************************************************************
	INCLUDES
***********************************************************************/
#include <util/twi.h>

#ifndef _TWICTRLMASTER_H_
#define _TWICTRLMASTER_H_

/***********************************************************************
	DEFINITIONS
***********************************************************************/
#define TWIM_READ    1
#define TWIM_WRITE   0

/****************************************************************************
  TWI State codes
****************************************************************************/
// General TWI Master staus codes
#define TWI_START					0x08  // START has been transmitted
#define TWI_REP_START				0x10  // Repeated START has been transmitted
#define TWI_ARB_LOST				0x38  // Arbitration lost

// TWI Master Transmitter staus codes
#define TWI_MTX_ADR_ACK				0x18  // SLA+W has been tramsmitted and ACK received
#define TWI_MTX_ADR_NACK			0x20  // SLA+W has been tramsmitted and NACK received
#define TWI_MTX_DATA_ACK			0x28  // Data byte has been tramsmitted and ACK received
#define TWI_MTX_DATA_NACK			0x30  // Data byte has been tramsmitted and NACK received

// TWI Master Receiver staus codes
#define TWI_MRX_ADR_ACK				0x40  // SLA+R has been tramsmitted and ACK received
#define TWI_MRX_ADR_NACK			0x48  // SLA+R has been tramsmitted and NACK received
#define TWI_MRX_DATA_ACK			0x50  // Data byte has been received and ACK tramsmitted
#define TWI_MRX_DATA_NACK			0x58  // Data byte has been received and NACK tramsmitted

// TWI Slave Transmitter staus codes
#define TWI_STX_ADR_ACK				0xA8  // Own SLA+R has been received; ACK has been returned
#define TWI_STX_ADR_ACK_M_ARB_LOST	0xB0  // Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
#define TWI_STX_DATA_ACK			0xB8  // Data byte in TWDR has been transmitted; ACK has been received
#define TWI_STX_DATA_NACK			0xC0  // Data byte in TWDR has been transmitted; NOT ACK has been received
#define TWI_STX_DATA_ACK_LAST_BYTE	0xC8  // Last data byte in TWDR has been transmitted (TWEA = 0); ACK has been received

// TWI Slave Receiver staus codes
#define TWI_SRX_ADR_ACK				0x60  // Own SLA+W has been received ACK has been returned
#define TWI_SRX_ADR_ACK_M_ARB_LOST	0x68  // Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
#define TWI_SRX_GEN_ACK				0x70  // General call address has been received; ACK has been returned
#define TWI_SRX_GEN_ACK_M_ARB_LOST	0x78  // Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
#define TWI_SRX_ADR_DATA_ACK		0x80  // Previously addressed with own SLA+W; data has been received; ACK has been returned
#define TWI_SRX_ADR_DATA_NACK		0x88  // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
#define TWI_SRX_GEN_DATA_ACK		0x90  // Previously addressed with general call; data has been received; ACK has been returned
#define TWI_SRX_GEN_DATA_NACK		0x98  // Previously addressed with general call; data has been received; NOT ACK has been returned
#define TWI_SRX_STOP_RESTART		0xA0  // A STOP condition or repeated START condition has been received while still addressed as Slave

// TWI Miscellaneous status codes
#define TWI_NO_STATE				0xF8  // No relevant state information available; TWINT = 0
#define TWI_BUS_ERROR				0x00  // Bus error due to an illegal START or STOP condition

#define	TWIS_ReadBytes		0x60
#define	TWIS_WriteBytes		0xA8

// Return Codes
#define TWIM_SUCCESS			0x01
#define TWIM_SEND_START_ERROR	0x02
#define TWIM_SEND_MODE_ERROR	0x03
#define TWIM_SEND_DATA_ERROR	0x04
#define TWIM_READ_DATA_ERROR	0x05

uint8_t InitTwiM(uint32_t CPU_FREQ, uint32_t TWI_Bitrate, uint8_t TwiPreScaler);

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
uint8_t TwiMStart (uint8_t Address, uint8_t TwiMType);

/*******************************************************
 Public Function: TimMStop
 Purpose: Stop the TWI Master
 Input Parameter: None
 Return Value: None
*******************************************************/
void TwiMStop (void);

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
uint8_t	TwiMWriteByte (uint8_t SlaveAdress, uint8_t Byte);

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
uint8_t	TwiMWriteByteInReg (uint8_t SlaveAdress, uint8_t Reg, uint8_t Byte);

/*******************************************************
 Public Function: TwiMWriteData
 Purpose: Write date (a array of bytes) to the slave

 Input Parameter:
	- uint8_t  Address of the Slave
 	- uint8_t*	Pointer to be the Buffer array
	- uint8_t  length of the data (no. of bytes)

 Return Value: uint8_t
	- TWIM_SUCCESS			(0x01)	- OK, data sent
	- TWIM_SEND_START_ERROR	(0x02)	- Error while sending "Start"
	- TWIM_SEND_MODE_ERROR	(0x03)	- Error while sending mode to slave
	- TWIM_SEND_DATA_ERROR	(0x04)	- Error while sending data
*******************************************************/
uint8_t	TwiMWriteData(uint8_t SlaveAdress, uint8_t* Data, uint8_t Length);

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
uint8_t	TwiMReadByte (uint8_t SlaveAdress, uint8_t SlaveReg, uint8_t* Byte);

/*******************************************************
 Public Function: TwiReadData
 Purpose: Write a byte to the slave

 Input Parameter:
	- uint8_t	Address of the Slave
	- uint8_t	Register to be read
 	- uint8_t*	Receive buffer
	- uint8_t	No of bytes to read
	- uint8_t	No of received bytes

 Return Value: uint8_t
 - TWIM_SUCCESS				(0x01)	- OK, data sent
 - TWIM_SEND_START_ERROR	(0x02)	- Error while sending "Start"
 - TWIM_SEND_MODE_ERROR		(0x03)	- Error while sending mode to slave
 - TWIM_SEND_DATA_ERROR		(0x04)	- Error while sending data
 - TWIM_READ_DATA_ERROR		(0x05)	- Error while reading data
 *******************************************************/
uint8_t	TwiMReadDats(uint8_t SlaveAdress, uint8_t SlaveReg, uint8_t* Data, uint8_t Length, uint8_t* RcvCount);

/*******************************************************
 Public Function: TwiMReadAck
 Purpose: Read a byte from the slave and request next byte

 Input Parameter: None

 Return Value: uint8_t
  	- uint8_t	Read byte
*******************************************************/
uint8_t	TwiMReadAck (void);

/*******************************************************
 Public Function: TwiMReadNAck
 Purpose: Read the last byte from the slave

 Input Parameter: None

 Return Value: uint8_t
  	- uint8_t	Read byte

*******************************************************/
uint8_t	TwiMReadNack (void);

	
#endif /* _TWICTRLMASTER_H_ */