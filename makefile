PROJNAME=avr-bootloader
UC=atmega8
LDFLAGS = -lm -lc -Wall -mmcu=$(UC)
HEXFORMAT = ihex
CFLAGS = -fpack-struct -Wall -Os -mcall-prologues -mmcu=$(UC) -Winline -finline-functions -Wstrict-prototypes --std=c99 -Winline -Wno-main -Wfatal-errors

all:
	#compile
	avr-gcc $(CFLAGS) bootloader.c -c -o build/bootloader.o
	avr-gcc $(CFLAGS) usart.c -c -o build/usart.o

	#link
	avr-gcc $(LDFLAGS) build/bootloader.o build/usart.o -o build/$(PROJNAME).out
	avr-objcopy -j .text -j .data -O $(HEXFORMAT) build/$(PROJNAME).out build/$(PROJNAME).hex

.PHONY: upload fuses clean
upload:
	sudo avrdude -p $(UC) -c usbasp -e -U flash:w:build/$(PROJNAME).hex

fuses:
	sudo avrdude -p $(UC) -c usbasp -U lfuse:w:0xE4:m -U hfuse:w:0xD9:m
# Default for the atmega8 is lfuse:e1, hfuse:d9
# Low fuse for 8MHz clock: E4
# High fuse with 1024 words, start at app start: D9
# High fuse with 1024 words, start at bootloader start: D8

clean:
	rm -f build/*
