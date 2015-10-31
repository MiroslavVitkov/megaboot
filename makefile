PROJNAME  = megaboot
UC        = atmega8
BOOTLOAD  = 0x1800                                                         # byte address, start of bootlaoder
LDFLAGS   = -lm -lc -Wall -mmcu=$(UC) -nostartfiles
LDFLAGS   += -Wl,-Map,build/$(PROJNAME).map
LDFLAGS   += -Wl,--section-start=.text=$(BOOTLOAD)
HEXFORMAT = ihex
CFLAGS    = -fpack-struct -Os -mcall-prologues -mmcu=$(UC)
CFLAGS    += -finline-functions --std=c11
CFLAGS    += -Wall -Winline -Wstrict-prototypes -Wno-main -Wfatal-errors
CFLAGS    += -DBOOTLOAD=$(BOOTLOAD)

all:
	#compile
	avr-gcc $(CFLAGS) bootloader.c -c -o build/bootloader.o

	#link
	avr-gcc $(LDFLAGS) build/bootloader.o -o build/$(PROJNAME).out
	avr-objcopy -j .text -j .data -O $(HEXFORMAT) build/$(PROJNAME).out build/$(PROJNAME).hex

.PHONY: upload fuses clean
upload:
	sudo avrdude -p $(UC) -c usbasp -e -U flash:w:build/$(PROJNAME).hex

fuses:
	sudo avrdude -p $(UC) -c usbasp -U lfuse:w:0xE4:m -U hfuse:w:0xD8:m
# Default for the atmega8 is lfuse:e1, hfuse:d9
# Low fuse for 8MHz clock: E4
# High fuse with 1024 words bootloader, start at app start: D9
# High fuse with 1024 words bootloader, start at bootloader start: D8
# High fuse with <2 * 128> bytes bootloader, start at app start: DF

disasm:
	avr-gcc $(CFLAGS) bootloader.c -S -o build/bootloader.S && nano build/bootloader.S

clean:
	@mv build/empty.txt .
	rm -f build/*
	@mv empty.txt build/
