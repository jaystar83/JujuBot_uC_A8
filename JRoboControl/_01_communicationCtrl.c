/*
 * _CommunicationCtrl.c
 *
 * Created: 20.03.2021 15:54:27
 *  Author: jayst
 */ 

#include "_01_communicationCtrl.h"
#include "_00_serial.h"

// CONSTANTS	/////////////////////////////////////////////////////////////////////

// VARS	/////////////////////////////////////////////////////////////////////////////

uint8_t serialReadBuffer [SERIAL_MESSAGE_LENGTH] = {0};
uint8_t serialSendBuffer [SERIAL_MESSAGE_LENGTH] = {0};


uint8_t stepperCommand(uint16_t );
void uart_writeServoData(uint8_t ServoNo);
void uart_writeMessageData(uint8_t* Message, uint8_t Length);

uint8_t CRC_verification(uint8_t* Message);
uint8_t CRC_calc(uint8_t* Message);

watchdogCtrl wdCtrl;

uint8_t CommunicationCtrlInit()
{
	if(watchdogInit(SERIAL_COM_WD_TICKS, &wdCtrl))
	{

		// Watchdog deaktiviert
		wdCtrl.WD_Allert = 0;
		*wdCtrl.WD_Allert = 0;

		uart_init(F_CPU, BAUD, sizeof(serialReadBuffer), SERIAL_MESSAGE_LENGTH, wdCtrl.WD_Allert );
		
		return TRUE;
	}
	else
		return FALSE;

}


uint8_t CommunicationCtrl()
{
	*wdCtrl.WD_Activation = TRUE;

	if( uart_getData(serialReadBuffer) )
	{
		if( !(CRC_verification(serialReadBuffer)) )
		{
			serialSendBuffer[0] = 255;	// ID - Error message
			serialSendBuffer[1] = 1;	// Byte 0 - Error ID - LOW
			serialSendBuffer[2] = 0;	// Byte 1 - Error ID - HIGH
			serialSendBuffer[3] = serialReadBuffer[0];	// Byte 2 - Message ID
			serialSendBuffer[4] = 0;	// Byte 3 - Empty
			serialSendBuffer[5] = 0;	// Byte 4 - Empty
			serialSendBuffer[6] = 0;	// Byte 5 - Empty
			serialSendBuffer[7] = 0;	// Byte 6 - Empty
			serialSendBuffer[8] = 0;	// Byte 7 - Empty
			serialSendBuffer[9] = 0;	// Byte 8 - Empty
			serialSendBuffer[10] = 0;	// Byte 9 - Empty
			serialSendBuffer[11] = CRC_calc(serialSendBuffer);
			
			uart_putd( serialSendBuffer, SERIAL_MESSAGE_LENGTH);
			
			return FALSE;
		}

		#if( DEV_SERIAL_OUTPUT == TRUE )		
			uart_writeMessageData(serialReadBuffer, SERIAL_MESSAGE_LENGTH);
	
			uart_puts("---ServoData current values---\r\n");
			uart_writeServoData(0);
			uart_writeServoData(1);
			uart_puts("---ServoData new values---\r\n");
		#endif
	
		// Check Message ID
		switch(serialReadBuffer[0])	// Message ID
		{
			case 1:
				ServoData_SetTargetPosition(0, (uint16_t)(serialReadBuffer[1]) + ((uint16_t)(serialReadBuffer[2])<<8) );
				ServoData_SetTargetSpeed(0, serialReadBuffer[3]);
				ServoData_SetAcceleration(0, serialReadBuffer[4]);
				ServoData_SetDeceleratiom(0, serialReadBuffer[5]);

				#if( DEV_SERIAL_OUTPUT == TRUE )
					uart_writeServoData(0);
				#endif
				
				ServoData_SetTargetPosition(1, (uint16_t)(serialReadBuffer[6]) + ((uint16_t)(serialReadBuffer[7])<<8) );
				ServoData_SetTargetSpeed(1, serialReadBuffer[8]);
				ServoData_SetAcceleration(1, serialReadBuffer[9]);
				ServoData_SetDeceleratiom(1, serialReadBuffer[10]);
						
				#if( DEV_SERIAL_OUTPUT == TRUE )
					uart_writeServoData(1);
				#endif
			
				break;
			
			case 2:
				break;
			
			// Message ID unknown
			default:
				return 255;
		}
	
	}
	else if(wdCtrl.WD_Allert)
	{
		serialSendBuffer[0] = 255;	// ID - Error message
		serialSendBuffer[1] = 2;	// Byte 0 - Error ID - LOW
		serialSendBuffer[2] = 0;	// Byte 1 - Error ID - HIGH
		serialSendBuffer[3] = serialReadBuffer[0];	// Byte 2 - Message ID
		serialSendBuffer[4] = 0;	// Byte 3 - Empty
		serialSendBuffer[5] = 0;	// Byte 4 - Empty
		serialSendBuffer[6] = 0;	// Byte 5 - Empty
		serialSendBuffer[7] = 0;	// Byte 6 - Empty
		serialSendBuffer[8] = 0;	// Byte 7 - Empty
		serialSendBuffer[9] = 0;	// Byte 8 - Empty
		serialSendBuffer[10] = 0;	// Byte 9 - Empty
		serialSendBuffer[11] = CRC_calc(serialSendBuffer);
			
		uart_putd( serialSendBuffer, SERIAL_MESSAGE_LENGTH);
		
		*wdCtrl.WD_Activation = FALSE;
		*wdCtrl.WD_Reset= TRUE;

		return FALSE;		
	}
	*wdCtrl.WD_Activation = FALSE;
	*wdCtrl.WD_Reset= TRUE;


	return TRUE;
}

void uart_writeServoData(uint8_t ServoNo)
{
	uart_puts("Servo: ");
	uart_putc('0'+(char)ServoData_GetServoNo(ServoNo));
	uart_putc('\r');uart_putc('\n');
	
	uart_puts("tarPos: ");
	uart_putc('0'+ (ServoData_GetTargetPosition(ServoNo)/100));
	uart_putc('0'+ (ServoData_GetTargetPosition(ServoNo)%100)/10);
	uart_putc('0'+ (ServoData_GetTargetPosition(ServoNo))%10);	
	uart_putc('\r');uart_putc('\n');
	
	uart_puts("tarSpe: ");
	uart_putc('0'+ (ServoData_GetTargetSpeed(ServoNo))/100);
	uart_putc('0'+ (ServoData_GetTargetSpeed(ServoNo)%100)/10);
	uart_putc('0'+ (ServoData_GetTargetSpeed(ServoNo))%10);
	uart_putc('\r');uart_putc('\n');
	
	uart_puts("curPos: ");
	uart_putc('0'+ (ServoData_GetCurrentPosition(ServoNo))/100);
	uart_putc('0'+ (ServoData_GetCurrentPosition(ServoNo)%100)/10);
	uart_putc('0'+ (ServoData_GetCurrentPosition(ServoNo))%10);
	uart_putc('\r');uart_putc('\n');
	
	uart_puts("curSpe: ");
	uart_putc('0'+ (ServoData_GetCurrentSpeed(ServoNo))/100);
	uart_putc('0'+ (ServoData_GetCurrentSpeed(ServoNo)%100)/10);
	uart_putc('0'+ (ServoData_GetCurrentSpeed(ServoNo))%10);
	uart_putc('\r');uart_putc('\n');
	
	return;
}

void uart_writeMessageData(uint8_t* Message, uint8_t Length)
{
	uart_puts("Message: ");
	for(uint8_t i=0; i<Length; i++)
	{
		uart_putc('0'+ (*Message)/100);
		uart_putc('0'+ (*Message%100)/10);
		uart_putc('0'+ (*Message)%10);
		uart_putc(' ');
		Message++;
	}
	uart_putc('\r');uart_putc('\n');
	
	return;
}

uint8_t CRC_verification(uint8_t* Message)
{
	uint8_t crc = 0;
	
	Message++;	// Payload starts from Byte 1 (Byte 0 = Message ID)
	for(uint8_t i=0; i<SERIAL_MESSAGE_LENGTH-2; i++)
		crc ^= 	 *Message++;
		
	#if( DEV_SERIAL_OUTPUT == TRUE )
		uart_puts("CRC = ");
		uart_putc('0'+ (crc)/100);
		uart_putc('0'+ (crc%100)/10);
		uart_putc('0'+ (crc)%10);
		uart_putc('\r');uart_putc('\n');
	
		if(crc == *Message)
			uart_puts("CRC OK\r\n");
		else
			uart_puts("CRC NOK\r\n");
	#endif
		
			
	return (crc == *Message);
}

uint8_t CRC_calc(uint8_t* Message)
{
	uint8_t crc = 0;
	Message++;	// Payload starts from Byte 1 (Byte 0 = Message ID)
	for(uint8_t i=0; i<SERIAL_MESSAGE_LENGTH-2; i++)
		crc ^= 	 *Message++;
		
	return crc;
}
