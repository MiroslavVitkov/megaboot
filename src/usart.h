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
	UBRR0H = UBRRH_VALUE;                    // Set baud rate.
	UBRR0L = UBRRL_VALUE;
#if USE_2X                                       // We are operating in asychronous mode: we need this consideration.
        UCSR0A |= (1 << U2X0);
#else
        UCSR0A &= ~(1 << U2X0);
#endif
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);          // Enable receiver and transmitter
	UCSR0C = (3<<UCSZ00);                    // Set frame format: 8 data bits, 1 stop bit, no parity aka 8N1
}

inline void usart_transmit( unsigned char data )
{
	while ( !( UCSR0A & (1<<UDRE0)) );       // Wait for empty transmit buffer
	UDR0 = data;                              // Put data into buffer, sends the data
}

inline char usart_receive(void)
{
	while ( !(UCSR0A & (1<<RXC0)) );         // Wait for data to be received.
        char c = UDR0;                           // Get and return received data from buffer.
        return c;
}

#endif //#ifndef _USART_H_
