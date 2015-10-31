#ifndef _USART_H_
#define _USART_H_

#include "usart.h"
#include "config.h"

#include <avr/io.h>
#include <util/setbaud.h>    // F_CPU and BAUD defined in config.h
#include <stdint.h>

#define NEWLINE "\r\n"
enum
{
    ASCII_SOH  = 0x01,
    ASCII_EOT  = 0x04,
    ASCII_ACK  = 0x06,
    ASCII_NACK = 0x15,
};


inline void usart_init(void)
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

inline void usart_transmit( unsigned char data )
{
	while ( !( UCSRA & (1<<UDRE)) );		// Wait for empty transmit buffer
	UDR = data;					// Put data into buffer, sends the data
}

inline char usart_receive(void)
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

#endif //#ifndef _USART_H_
