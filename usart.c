#include "usart.h"
#include "config.h"

#include <avr/io.h>
#include <util/setbaud.h>    // F_CPU and BAUD defined in config.h

void usart_init(void)
{
	UBRRH = UBRRH_VALUE;                            //Set baud rate.
	UBRRL = UBRRL_VALUE;
#if USE_2X                                              // We are operating in asychronous mode: we need this consideration.
        UCSRA |= (1 << U2X);
#else
        UCSRA &= ~(1 << U2X);
#endif
	UCSRB = (1<<RXEN)|(1<<TXEN);			// Enable receiver and transmitter
	UCSRC = (1<<URSEL) | (3<<UCSZ0);                // Set frame format: 8 data bits, 1 stop bit, no parity aka 8N1
}

void usart_transmit( unsigned char data )
{
	while ( !( UCSRA & (1<<UDRE)) );		// Wait for empty transmit buffer
	UDR = data;					// Put data into buffer, sends the data
}

void usart_transmit_block(const unsigned char *data, unsigned char bytes )
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
	unsigned char dummy; (void)dummy;
	while ( UCSRA & (1<<RXC) ) dummy = UDR;
}



