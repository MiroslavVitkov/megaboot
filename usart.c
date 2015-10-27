#include "usart.h"

#include <avr/io.h>

// TODO: extract the configuration values to the header file.

/*
//calculate baudrate register
#define BAUDREG            ((unsigned int)((F_CPU * 10) / (16UL * BAUDRATE) - 5) / 10)

//check baudrate register error
//mocro below maybe not same in different C compiler
#define FreqTemp           (16UL * BAUDRATE * (((F_CPU * 10) / (16 * BAUDRATE) + 5)/ 10))
#if ((FreqTemp * 50) > (51 * F_CPU)) || ((FreqTemp * 50) < (49 * F_CPU))
#error "BaudRate error > 2% ! Please check BaudRate and F_CPU value."
#endif
*/

#define UBRR_FOR_BAUD_RATE_38K4_CPU_CLOCK_8M 12

void usart_init(void)
{
	UBRRH = (unsigned char)(UBRR_FOR_BAUD_RATE_38K4_CPU_CLOCK_8M>>8);	//Set baud rate
	UBRRL = (unsigned char)UBRR_FOR_BAUD_RATE_38K4_CPU_CLOCK_8M;
	UCSRB = (1<<RXEN)|(1<<TXEN);						//Enable receiver and transmitter
	UCSRC = (1<<URSEL) | (3<<UCSZ0);                                        //Set frame format: 8 data bits, 1 stop bit, no parity aka 8N1
}

void usart_transmit( unsigned char data )
{
	while ( !( UCSRA & (1<<UDRE)) );		//Wait for empty transmit buffer
	UDR = data;					//Put data into buffer, sends the data
}

void usart_transmit_block(unsigned char *const data, unsigned char bytes )
{
	for(int i = bytes; i > 0; --i)
        {
		usart_transmit(*data++);
	}
}

char usart_receive(void)
{
	while ( !(UCSRA & (1<<RXC)) );	// Wait for data to be received.

/*	if(UCSR0A & (1 << FE0))
		return ERROR_USART_FRAME_ERROR;
	if(UCSR0A & (1 << DOR0))
		return ERROR_USART_DATA_OVERRUN_ERROR;
	if(UCSR0A & (1 << UPE0))
		return ERROR_USART_PARITY_ERROR;
*/
        char c = UDR;			// Get and return received data from buffer.
        return c;
}

void usart_flush( void ){
	unsigned char dummy;
	while ( UCSRA & (1<<RXC) ) dummy = UDR;
}



