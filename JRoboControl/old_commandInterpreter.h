/*
 * instructionInerpreter.c
 *
 * Created: 01.05.2016 16:40:06
 *  Author: jaystar
 */ 
#ifndef COMMANDINTERPRETER_H_
#define COMMANDINTERPRETER_H_

#if SW_TEST_ACTIVE == FALSE
	#include <avr/io.h>
#endif

#include "_serial.h"
#include "_timerConfig.h"

uint8_t parseStepperNo(int8_t*);
uint8_t parseOpMode(int8_t*);
uint8_t parseVelocity(int8_t*);
uint8_t parsStepSetpoint(int8_t*);
uint8_t parse2CharCommand(int8_t*);

// CONSTANTS	/////////////////////////////////////////////////////////////////////
const char ProgName[30]	= "uC-Prog V0.1.1_12\r\n";	

// VARS	/////////////////////////////////////////////////////////////////////////////
char ErrMessage[]		= {'C','0','1',',','0','1',',','N','O','K','\r','\n','\0'}; // Cxy,ab,NOK\r\n\0 = 13 Byte
char SuccessMessage[]	= {'C','0','1',',','O','K','\r','\n','\0'}; // Cxy,OK\r\n\0 = 9 Byte	
char *str;		// globaler Pointer, wird in "get Command" initialisiert

//void getCommand(StringIn, &timerOffsetVal, &pinNo, & velocity,NO_OF_ENGINES, opMode, &setPoint)
void getCommand(char *strIn, uint8_t *StepperNo, uint8_t *OpMode, uint8_t *Velocity, uint8_t *StepSetpoint, uint8_t *TwiWriteCommand, uint8_t *TwiReadCommand, uint8_t *TwiSlaveAddresse)
{	
	str = strIn;
	// Command 'Cxx,Wert1,...,Wertn\r'
	if(*str == 'C')
	{	
		int comm = 0;

		str++;
			
		comm = (int)((*str++)-'0') * 10;	// Zehnerstelle des Commands ermitteln
		comm += (int)((*str++)-'0');		// Einerstelle des Commands ermitteln

		// Code für entsprechendes Command///////////////////////////////////////////////////////////////////
		switch (comm)
		{
			// Command = C00 -> return ProgVersion	/////////////////////////////
			case 00:
			{
				SuccessMessage[1] = '0';
				SuccessMessage[2] = '0';
				//uart_puts ("C00,OK\r\n");
				uart_puts(SuccessMessage);
				uart_puts(ProgName);
				break;
			}
			
			// Command = C01 -> Vorgabe MotorBefehl:	C01,StepperNo,StepperComm,SpeedSetpoint, StepSetpoint'\r'	/////////////////////////////
			case 1:
			{	
				ErrMessage[1] = '0';
				ErrMessage[2] = '1';
				ErrMessage[4] = '0';
				ErrMessage[5] = '0';
				
				int8_t funcResult	= 0;
				uint8_t stepperNo	= 0;
				uint8_t opMode		= 0;
				uint8_t velo		= 0;
				uint8_t stepSetpoint= 0;
				
				str++;	// nächstes Zeichen Überspringen -> Komma

				// selected stepper (0..3)
				stepperNo = parseStepperNo(&funcResult);
				if(funcResult < 0 )
				{
					ErrMessage[5] = '1';
					uart_puts (ErrMessage);
					*StepperNo = 0;
					return;
				}
				str++;	// switch next char -> Komma
				
				// operation mode (0=Stop, 1=cont.forward, 2= cont.backward, 3=stepsForward, 4 stepsBackward
				opMode = parseOpMode(&funcResult);
				if(funcResult < 0 )
				{
					ErrMessage[5] = '2';
					uart_puts (ErrMessage);
					*OpMode = 0;
					return;
				}
				str++;	// switch next char -> Komma
				
				// setpoint of Velocity (1..10)
				velo = parseVelocity(&funcResult); 
				if(funcResult < 0 )
				{
					ErrMessage[5] = '3';
					uart_puts (ErrMessage);
					*Velocity = 0;
					return;
				}
				str++;	// switch next char -> Komma

				// set no. of steps (1..255)
				stepSetpoint = parsStepSetpoint(&funcResult);
				if(funcResult < 0 )
				{
					ErrMessage[5] = '4';
					uart_puts (ErrMessage);
					*StepSetpoint = 0;
					return;
				}
//				str++;	// switch next char -> Komma

				// Prüfen, ob Befehl i.O. (Abschlusszeichen muss CR sein)	//////////////////////////////////////
				if(*str++ == '\r')
				{	
					*StepperNo					=  stepperNo;
					*(OpMode+stepperNo)			=  opMode;
					*(Velocity+stepperNo)		=   (velo); //* 25;
//					*(Velocity+stepperNo)		=  (11 - velo) * 25;
					*(StepSetpoint+stepperNo)	=  stepSetpoint;
					SuccessMessage[1] = '0';
					SuccessMessage[2] = '1';
					uart_puts(SuccessMessage);
					uart_puts("StepperNo=");
					uart_putc('0'+ *StepperNo);
					uart_puts("\r\nOpMode=");
					uart_putc('0'+ *(OpMode+stepperNo)/10);
					uart_putc('0'+ *(OpMode+stepperNo)%10);
					uart_puts("\r\nVelocity=");
					uart_putc('0'+ *(Velocity+stepperNo)/100);
					uart_putc('0'+ (*(Velocity+stepperNo)%100)/10);
					uart_putc('0'+ *(Velocity+stepperNo)%10);
					uart_puts("\r\nStepSetpoint=");
					uart_putc('0'+ *(StepSetpoint+stepperNo)/100);
					uart_putc('0'+ (*(StepSetpoint+stepperNo)%100)/10);
					uart_putc('0'+ *(StepSetpoint+stepperNo)%10);
					uart_puts("\r\n");
					
				}
				else
				{
					ErrMessage[4] = '?';
					ErrMessage[5] = '?';
					uart_puts (ErrMessage);
				}
				
				break;
			}
			
			// Command = C02 -> manipulate Stepper Speed-> C02,StepperNo,SpeedVal'\r' ///////////
			// Bsp. C02,2,25'\r' -> Stepper 3  SpeedMax=5 -> 1/5 Of MaxSpeed...	/////////////////////
			case 2:
			{
					ErrMessage[1] = '0';
					ErrMessage[2] = '2';
					ErrMessage[4] = '0';
					ErrMessage[5] = '0';
				
					int8_t funcResult	= 0;
					uint8_t stepperNo	= 0;
					uint8_t velo		= 0;
				
					str++;	// nächstes Zeichen Überspringen -> Komma

					// selected stepper (0..3)
					stepperNo = parseStepperNo(&funcResult);
					if(funcResult < 0 )
					{
						ErrMessage[5] = '1';
						uart_puts (ErrMessage);
						*StepperNo = 0;
						return;
					}
					str++;	// switch next char -> Komma
				
					// setpoint of Velocity (1..10)
					velo = parseVelocity(&funcResult); 
					if(funcResult < 0 )
					{
						ErrMessage[5] = '2';
						uart_puts (ErrMessage);
						*Velocity = 0;
						return;
					}
	//				str++;	// switch next char -> Komma

					// Prüfen, ob Befehl i.O. (Abschlusszeichen muss CR sein)	//////////////////////////////////////
					if(*str++ == '\r')
					{	
						*StepperNo					=  stepperNo;
						*(Velocity+stepperNo)		=  (velo); //* 25;
						SuccessMessage[1] = '0';
						SuccessMessage[2] = '2';
						uart_puts(SuccessMessage);
						uart_puts("StepperNo=");
						uart_putc('0'+ *StepperNo);
						uart_puts("\r\nVelocity=");
						uart_putc('0'+ *(Velocity+stepperNo)/100);
						uart_putc('0'+ (*(Velocity+stepperNo)%100)/10);
						uart_putc('0'+ *(Velocity+stepperNo)%10);
						uart_puts("\r\n");
					
					}
					else
					{
						ErrMessage[4] = '?';
						ErrMessage[5] = '?';
						uart_puts (ErrMessage);
					}
				
					break;
				}		
		
			// Command = C03 -> Master write to TWI -> C50,WriteCommand,SlaveAdresse'\r' ///////////
			// Bsp. C50,01,15'\r' -> writeCommand = 1, SlaveAdress = 15	/////////////////////
			case 3:
			{
				;
				break;
			}
		
			// Command = C50 -> Master write to TWI -> C50,WriteCommand,SlaveAdresse'\r' ///////////
			// Bsp. C50,20,15'\r' -> writeCommand = 20, SlaveAdress = 15	/////////////////////
			case 50:
			{
				ErrMessage[1] = '5';
				ErrMessage[2] = '0';
				ErrMessage[4] = '0';
				ErrMessage[5] = '0';
				
				int8_t funcResult	= 0;
				uint8_t writeComm	= 0;
				uint8_t slaveAddr	= 0;
				
				str++;	// nächstes Zeichen Überspringen -> Komma
					// selected stepper (0..3)
					writeComm = parse2CharCommand(&funcResult);
					if(funcResult < 0 )
					{
						ErrMessage[5] = '1';
						uart_puts (ErrMessage);
						*TwiWriteCommand = 0;
						return;
					}
					str++;	// switch next char -> Komma
				
					// setpoint of Velocity (1..10)
					slaveAddr = parse2CharCommand(&funcResult); 
					if(funcResult < 0 )
					{
						ErrMessage[5] = '2';
						uart_puts (ErrMessage);
						*TwiSlaveAddresse = 0;
						return;
					}
	//				str++;	// switch next char -> Komma

					// Prüfen, ob Befehl i.O. (Abschlusszeichen muss CR sein)	//////////////////////////////////////
					if(*str++ == '\r')
					{	
						*TwiWriteCommand	=  writeComm;
						*TwiSlaveAddresse	=  slaveAddr;;
						SuccessMessage[1] = '5';
						SuccessMessage[2] = '0';
						uart_puts(SuccessMessage);
						uart_puts("WrComm=");
						uart_putc('0'+ (*TwiWriteCommand/10));
						uart_putc('0'+ *TwiWriteCommand%10);
						uart_puts("\r\nSlaveAdrr=");
						uart_putc('0'+ (*TwiSlaveAddresse/10));
						uart_putc('0'+ (*TwiSlaveAddresse%10));
						uart_puts("\r\n");
					}
					else
					{
						ErrMessage[4] = '?';
						ErrMessage[5] = '?';
						uart_puts (ErrMessage);
					}
				
					break;

					// selected stepper (0..3)
				;
				break;
			}

			// Command = C51 -> Master read from TWI -> C50,ReadCommand,SlaveAdresse'\r' ///////////
			// Bsp. C51,21,15'\r' -> writeCommand = 21, SlaveAdress = 15	/////////////////////
			// Command = C50 -> Master write to TWI -> C50,WriteCommand,SlaveAdresse'\r' ///////////
			// Bsp. C50,20,15'\r' -> writeCommand = 20, SlaveAdress = 15	/////////////////////
			case 51:
			{
				ErrMessage[1] = '5';
				ErrMessage[2] = '0';
				ErrMessage[4] = '0';
				ErrMessage[5] = '0';
			
				int8_t funcResult	= 0;
				uint8_t writeComm	= 0;
				uint8_t slaveAddr	= 0;
			
				str++;	// nächstes Zeichen Überspringen -> Komma
				// selected stepper (0..3)
				writeComm = parse2CharCommand(&funcResult);
				if(funcResult < 0 )
				{
					ErrMessage[5] = '1';
					uart_puts (ErrMessage);
					*TwiWriteCommand = 0;
					return;
				}
				str++;	// switch next char -> Komma
			
				// setpoint of Velocity (1..10)
				slaveAddr = parse2CharCommand(&funcResult);
				if(funcResult < 0 )
				{
					ErrMessage[5] = '2';
					uart_puts (ErrMessage);
					*TwiSlaveAddresse = 0;
					return;
				}
				//				str++;	// switch next char -> Komma

				// Prüfen, ob Befehl i.O. (Abschlusszeichen muss CR sein)	//////////////////////////////////////
				if(*str++ == '\r')
				{
					*TwiWriteCommand	=  writeComm;
					*TwiSlaveAddresse	=  slaveAddr;
					SuccessMessage[1] = '5';
					SuccessMessage[2] = '0';
					uart_puts(SuccessMessage);
					uart_puts("WrComm=");
					uart_putc('0'+ (*TwiWriteCommand/10));
					uart_putc('0'+ *TwiWriteCommand%10);
					uart_puts("\r\nSlaveAdrr=");
					uart_putc('0'+ (*TwiSlaveAddresse/10));
					uart_putc('0'+ (*TwiSlaveAddresse%10));
					uart_puts("\r\n");
				}
				else
				{
					ErrMessage[4] = '?';
					ErrMessage[5] = '?';
					uart_puts (ErrMessage);
				}
			
				break;

				// selected stepper (0..3)
				;
				break;
			}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
			case 99:
			{	
				SuccessMessage[1] = '9';
				SuccessMessage[2] = '9';
				uart_puts(SuccessMessage);
				//uart_puts ("C99,OK\r\n");
				break;
		
			}

			default:
			{
				ErrMessage[1] = 'x';
				ErrMessage[2] = 'x';
				ErrMessage[4] = 'y';
				ErrMessage[5] = 'y';
				uart_puts (ErrMessage);
				//		uart_puts ("Cxx,yy,NOK\r\n");
			}
		}
	}
}

// parsing comamnd for stepper no.	(0..3)	/////////////////////////////////////////////////////////
uint8_t parseStepperNo(int8_t *result )
{
	uint8_t stepperNo = 0;
	char temp;
	
	// stepper no. val bilden	////////////////////////////////////////////////////////////////////
	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 3) ) // 1er-Stelle - es mmuss eine Zahl zwischen 0 ... 3 sein
		stepperNo = (uint8_t)(temp) ;
	else
	{	*result = -1;
		return 0;}
	
	return stepperNo;
}


// parsing comamnd for opmode selection	/////////////////////////////////////////////////////////////
uint8_t parseOpMode(int8_t *result )
{
	uint8_t opMode = 0;
	char temp;
	
	
	// OpMode Val bilden	////////////////////////////////////////////////////////////////////
	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 9) ) // 10er-Stelle - es mmuss eine Zahl zwischen 0 ... 9 sein
		opMode = (uint8_t)(temp) * 10;
	else
	{	*result = -1;
		return 0;}

	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 9) ) // 1er-Stelle - es mmuss eine Zahl zwischen 0 ... 9 sein
		opMode += (uint8_t)(temp);
	else
	{	*result = -2;
		return  0;}
	
	return opMode;
}

// parsing comamnd for velocity setpoint selection (1..10)	/////////////////////////////////////////
uint8_t parseVelocity(int8_t *result )
{
	uint8_t velo = 0;
	char temp;
	
	// OpMode Val bilden	////////////////////////////////////////////////////////////////////
	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 9) ) // 10er-Stelle - es mmuss eine Zahl zwischen 0 ... 1 sein
		velo = (uint8_t)(temp) * 10;
	else
	{	*result = -1;
		return 0;}

	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 9) ) // 1er-Stelle - es mmuss eine Zahl zwischen 0 ... 9 sein
		velo += (uint8_t)(temp);
	else
	{	*result = -2;
		return  0;}
	
	return velo;
}

// parsing comamnd for steps setpoint (1..255)	////////////////////////////////////////////////
uint8_t parsStepSetpoint(int8_t *result )
{
	uint8_t steps = 0;
	char temp;
	
	// steps Val bilden	////////////////////////////////////////////////////////////////////
	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 2) ) // 100er-Stelle - es mmuss eine Zahl zwischen 0 ... 2 sein
		steps = (uint8_t)(temp) * 100;
	else
	{	*result = -1;
		return 0;}

	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 9) ) // 10er-Stelle - es mmuss eine Zahl zwischen 0 ... 9 sein
		steps += (uint8_t)(temp) * 10;
	else
	{	*result = -2;
		return 0;}

	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 9) ) // 1er-Stelle - es mmuss eine Zahl zwischen 0 ... 9 sein
		steps += (uint8_t)(temp);
	else
	{	*result = -3;
		return  0;}
	
	return steps;
}

// parsing 2 Char Command	///////////////////////////////////////////////////////////////////
uint8_t parse2CharCommand(int8_t *result )
{
	uint8_t command = 0;
	char temp;
	
	
	// OpMode Val bilden	////////////////////////////////////////////////////////////////////
	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 9) ) // 10er-Stelle - es mmuss eine Zahl zwischen 0 ... 9 sein
		command = (uint8_t)(temp) * 10;
	else
	{	
		*result = -1;
		return 0;
	}

	temp = *str++ - '0';
	if( (temp >= 0) && (temp <= 9) ) // 1er-Stelle - es mmuss eine Zahl zwischen 0 ... 9 sein
		command += (uint8_t)(temp);
	else
	{	
		*result = -2;
		return  0;
	}
	
	return command;
}



#endif /* COMMANDINTERPRETER_H_ */
