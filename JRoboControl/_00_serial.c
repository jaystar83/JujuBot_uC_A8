/*
 * _serial.c
 *
 * Created: 15.03.2019 14:45:01
 *  Author: jayst
 */ 

#include "_00_serial.h"

unsigned char readBufferLength = 50;
unsigned char dataLength = 50;
unsigned char *WdAllert;


// Initialisiet UART gegebener oszillatorfrequenz udn Baudrate
void uart_init(long Oszi, long Baud, unsigned char SizeOfReadBuffer, unsigned char DataLength, unsigned char *WdAllert)
{
	// Reset UART	/////////////////////////////////////
	//	UCSR0A - Register
	UCSR0A |= (0<<RXC0);	// Bit7 R	- Recive Complete	1 -> Byte wurde empfangen und kann ausgelesen werden
	UCSR0A |= (0<<TXC0);	// Bit6 R/W	- Transmit Complete	1 -> Byte wurde volsständig gesendet
	UCSR0A |= (0<<UDRE0);	// Bit5	R	- Date Reg. Empty	1 -> Sendebuffer ist leer, 0 -> Daten im UDR
	UCSR0A |= (0<<FE0);		// Bit4 R	- Error Frame		1 -> Byte im Empfangspuf. hat einen Frame Error
	UCSR0A |= (0<<DOR0);	// Bit3 R	- Date OverRun		1 -> ReciveBuffer ist voll, bleibt bis ausgelesen
	UCSR0A |= (0<<UPE0);	// Bit2 R	- UART Parity Error 1 -> Byte im ReciveBudffer hat Parity Fehler
	UCSR0A |= (0<<U2X0);	// Bit1 R/W	- Double Transm. speed 1-> setzt den Baud Raten Devisor von 16 auf 8
	UCSR0A |= (0<<MPCM0);	// Bit0 R/W - Multi Processor Communication Mode - Erklärung siehe Hilfe
	// UCSR0A = 0x00;

	UCSR0B |= (0<<RXCIE0);	// Bit7 R/W	- RX Compl. Interrupt Enable	1 -> Enables Recive Interrupt
	UCSR0B |= (0<<TXCIE0);	// Bit6 R/W	- TX Compl. Interrupt Enable	1 -> Enables Transmit Interrupt
	UCSR0B |= (0<<UDRIE0);	// Bit5 R/W	- UART Data Reg. Empty Interupt 1 -> Enables Interrupt if UDRE = 1
	UCSR0B |= (0<<RXEN0);	// Bit4 R/W	- Reciver Enable				1 -> Enables UART Data Reciving
	UCSR0B |= (0<<TXEN0);	// Bit3 R/W	- Transmitter Enable			1 -> Enables UART Data Transmission
	UCSR0B |= (0<<UCSZ02);	// Bit2 R/W	- Character Size Bit2			1 -> Zusammen mit UCSZ01,00 in UCSR0B - Setzen dir Anzahl DatenBits für Rx/Tx, Reserved bei "Master SPI Mode"
	UCSR0B |= (0<<RXB80);	// Bit1 R	- Neunte DatenBit bei 9-Bit-Telegrammen, Muss vor UDR gelesen werden
	UCSR0B |= (0<<TXB80);	// Bit0 R/W	- Neunte DatenBit bei 9-Bit-Telegrammen, Muss vor UDR geschrieben werden

	UCSR0C |= (0<<UMSEL01);	// Bit7 R/W	- UART ModeSelect Bit 1			00 - Asynchr.UART,	01 - Synchr.UART // Bit 1 muss immer geschreiben werden wenn
	UCSR0C |= (0<<UMSEL00);	// Bit6 R/W	- UART ModeSelect Bit 0			10 - Reserved,		11 - Master SPI	 // UCSR0C beschrieben wird (UCSR0C und UBRR0H haben die gleichre Adresse, oberstes Bit entscheidet welches beschreiben wird)
	UCSR0C |= (0<<UPM01);	// Bit5 R/W	- UART Parity Mode 1			00 - Disable, 01 - Reserved, 10 - Enable EvenParity,
	UCSR0C |= (0<<UPM00);	// Bit4 R/W	- UART Parity Mode 2			11 - Enable OddParity !!! Reserved in SPI Mode
	UCSR0C |= (0<<USBS0);	// Bit3 R/W	- UART StopBit Select			0 - 1 StopBit, 1 - 2 StopBits
	// Serial Mode
	UCSR0C |= (0<<UCSZ01);	// Bit2 R/W	- Character Size Bit1 ->	000 - 5Bit, 001 - 6Bit, 010 - 7Bit, 011 - 8Bit, 100 - Reserved
	UCSR0C |= (0<<UCSZ00);	// Bit1 R/W	- Character Size Bit0 ->	101 - Reserved, 110 - Reserved, 111 - 9Bit 1
	// Master SPI Mode
	//	UCSR0C |= (0<<UDORD0);	// Bit2 R/W	- SPI Data Order	1 -> LSB des Datenworts wird zuerst gesendet 0 -> MSB zuerst
	//	UCSR0C |= (0<<UCPHA0);	// Bit1 R/W	- UART Clock Phase	1 -> Datenübernahme bei steigender Flanke 0 -> bei fallender  des SynchronosClock (XCK0 -> siehe hilfe)
	UCSR0C |= (0<<UCPOL0);	// Bit0 R/W	- Clock Polarity	- für synchronen Modus -> Details siehe Hilfe

	UBRR0L = 0x00;	// 4bit von UBRR0H + 8bit von UBBR0L ergeben das Teilungsverhältnis für die Baudrate
	UBRR0H = 0x0;	// TV = (CpuTakt / (Baudrate * 16)) - 1; Bzw. TV = (CpuTakt / (Baudrate * 8)) - 1; wenn Doppeltes TV (UCSR0A |= (0<<U2X0);)
	
	
	if( (SizeOfReadBuffer == 0) || (SizeOfReadBuffer > 50) )
	readBufferLength = 50;
	else
	readBufferLength = SizeOfReadBuffer;
	
	if( (DataLength == 0) || (DataLength > 20) )
		dataLength = 20;
	else
		dataLength = DataLength;
	
	// Berechnungs des Teilungsverhältnises
	if ((Oszi / 16L / Baud -1) > 30)	// Teilungsverhältnis > 30 -> groß genug
	{
		UBRR0L = (unsigned char)(Oszi / 16L / Baud -1);
		UBRR0H = (unsigned char)(Oszi / 16L / Baud -1) >> 8;
	}
	else								// Sonst Baudraten-Divisor auf 8 setzen (Doppelte Baudrate)
	{
		UBRR0L = (unsigned char)(Oszi / 8L / Baud -1);
		UBRR0H = (unsigned char)(Oszi / 8L / Baud -1) >> 8;
		UCSR0A |= (1<<U2X0);	// Bit1 R/W	- Double Transm. speed 1-> setzt den Baud Raten Devisor von 16 auf 8
	}
	
	UCSR0B |= (1 << TXEN0);		// Transmit Enable
	UCSR0B |= (1 << RXEN0);		// Recive Enable
	
	UCSR0C |= (1 << UMSEL01);	// ModeSelect 01 - Reserved -> muss gesetzt werden, weil UCSR0C beschrieben wird
	UCSR0C |= (1 << UCSZ01);	// Character Size 011
	UCSR0C |= (1 << UCSZ00);	// -> 8Bit
}

// Sendet 1 Byte an UART
void uart_putc(unsigned char data)
{
	while(!(UCSR0A &(1 << UDRE0)));
		UDR0 = (char)data;
}

// Sendet String an UART
void uart_puts(const char *str)	//"const char * Str" nötig -> sonst: Warning - deprecated conversion from string constant to 'char*' [-Wwrite-strings]
{
	unsigned char c;
	while ((c = *str++) != '\0')
	uart_putc(c);
}

// Sendet Daten UART
void uart_putd(const unsigned char *d, char length)
{
	for(unsigned char i=0; i<length; i++)
		uart_putc(*d++);
}

// Empfaengt 1 Byte vom UART
unsigned char uart_getc(void)
{
	if(WdAllert == 0)	// ****	Watchdog inactive	****
	{
		while ( !(UCSR0A & (1 << RXC0)) );	// RXC0A Bit 7 in UCSR0A -> zeigt an, dass ein Zeichen empfangen wurde und in UDR0 bereitsteht
			return UDR0;
	}
	else	// ****	Watchdog active	****
	{
		while ( (!(UCSR0A & (1 << RXC0))) | *WdAllert );	// RXC0A Bit 7 in UCSR0A -> zeigt an, dass ein Zeichen empfangen wurde und in UDR0 bereitsteht
			return 0;
	}
	
}

// Empfängt Strings vom UART
void uart_gets(char *str)
{
	if(UCSR0A & (1 << RXC0))	// Byte wurde empfangen -> Daten am Emfangspuffer
	{
		if(errorHandlerFlags & (1<<0)) // Wenn Fehlerflag gesetzt - lösche restliche ankommende Zeichen
		{	uart_getc();
			*str = '\0';
		}
		//	Wenn kein Fehler anliegt -> Lese UART aus bis '\r' gesendet wird oder wenn Länge größer 30 (31 - Abschluss) Zeichen
		else
		{	int i = 0;
			while( (*str = uart_getc()) != '\r' ) // So lange kein CR als Abschlusszeichen eingelesen wurde
			{	str++;
				i++;
				if(i>=readBufferLength)	// Pufferüberlauf !!!
				{	str -= readBufferLength;
					*str = '\0';
					uart_puts("Eingabe zu lang!\r\n");		// -> PROBLEM: Restlichen Gesendeten Werte werden im nächsten Druchlauf ausgelesen -> evtl. bis es zu Fehler kommt durch asynchronität durch Programmablauf
					errorHandlerFlags |= 1<<0;				// set errorHandlerFlag 0000_0001
					return;
				}
			}
		}
	}
	else
	{	if(errorHandlerFlags & (1<<0))
		errorHandlerFlags ^= 1<<0;	// reset errorHandlerFlag 0000_0001
		*str = '\0';
	}
}

// Empfängt Daten vom UART
unsigned char uart_getData(uint8_t *DataBuffer)
{
	if(UCSR0A & (1 << RXC0))	// Byte wurde empfangen -> Daten am Emfangspuffer
	{
		if(errorHandlerFlags & (1<<0)) // Wenn Fehlerflag gesetzt - lösche restliche ankommende Zeichen
		{	
			uart_getc();
			*DataBuffer = 0;
			return 0;
		}
		//	Wenn kein Fehler anliegt -> Lese UART aus bis dataLength erreicht wurde
		else
		{	
			for(unsigned char i=0; i < dataLength; i++)
				*DataBuffer++ = uart_getc();
		}
	}
	else
	{	
		if(errorHandlerFlags & (1<<0))
			errorHandlerFlags ^= 1<<0;	// reset errorHandlerFlag 0000_0001

		*DataBuffer = 0;
		return 0;
	}

	return 1;
}
