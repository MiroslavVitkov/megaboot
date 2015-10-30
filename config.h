#ifndef _CONFIG_H_
#define _CONFIG_H_

// Disable assert macros for smaller code size and faster execution.
#define NDEBUG

// CPU nominal frequency, Hz.
#define F_CPU 8000000

// USART baud rate.
#define BAUD 38400

// Last page available to the application section. Next page is part of the bootloader section.
#define APPLICATION_SECTION_END_PAGES 24

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
