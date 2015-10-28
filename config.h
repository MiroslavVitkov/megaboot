#ifndef _CONFIG_H_
#define _CONFIG_H_

//cpu
#define F_CPU 8000000

typedef enum
{
    ERROR_NONE                             =  0,
    ERROR_PROTOCOL_FIRST_CHARACTER         = -1,
    ERROR_PROTOCOL_PACKET_NUMBER_INVERSION = -2,
    ERROR_PROTOCOL_PACKET_NUMBER_ORDER     = -3,
    ERROR_PROTOCOL_CRC                     = -4,
}error_t;

#endif	//#ifndef _CONFIG_H_


/*
//If you will remove IVT(interrupt vect table), define macro noIVT to 1
//and add "-nostartfiles" in linker option
//remove interrupt vect table
#if noIVT
void initstack(void) __attribute__ ((section(".init9")));
void initstack(void) 
{
  //set stack
  asm volatile ( ".set __stack, %0" :: "i" (RAMEND) ); 
  //jump to main function
  asm volatile ( "rjmp main");
}
#endif*/
