#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

// If undefined, error checking on the XMODEM protocol is disabled.
// Also, the flash writing routine is not protected form overwriting the bootloader section.
// Furthermore, flash memory verification after write is not performed.
#define ERROR_CHECKING

// CPU nominal frequency, Hz.
#define F_CPU 8000000

// USART baud rate.
#define BAUD 38400

// This macro is passed from the makefile.
// It contains the byte address of the start of the bootloader section.
// This depends upon device and the BOOTSZ[1, 0] bits.
// YOU MUST SET THIS MANUALLY IN THE MAKEFILE.
// #define BOOTLOAD <byte_address>

enum
{
    ERROR_NONE                             =  0,
    ERROR_PROTOCOL_FIRST_CHARACTER         = -1,
    ERROR_PROTOCOL_PACKET_NUMBER_INVERSION = -2,
    ERROR_PROTOCOL_PACKET_NUMBER_ORDER     = -3,
    ERROR_PROTOCOL_CRC                     = -4,
};
typedef int8_t error_t;

#endif	//#ifndef _CONFIG_H_
