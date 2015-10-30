#ifndef _USART_H_
#define _USART_H_

#define NEWLINE "\r\n"
enum
{
    ASCII_SOH  = 0x01,
    ASCII_EOT  = 0x04,
    ASCII_Y    = 0x59,
    ASCII_N    = 0x4e,
    ASCII_ACK  = 0x06,
    ASCII_NACK = 0x15,
};


void usart_init(void);
void usart_transmit(unsigned char data);
char usart_receive(void);  // Blocking!

#endif //#ifndef _USART_H_
