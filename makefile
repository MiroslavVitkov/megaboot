PROJNAME        = megaboot
UC              = atmega168
BOOTLOAD        = 0x3E00 # byte address, start of bootlaoder
HEXFORMAT       = ihex

LDFLAGS         = -lm -lc -Wall -mmcu=$(UC)
LDFLAGS_LOADER  = -nostartfiles
LDFLAGS_LOADER += -Wl,-Map,build/bootloader.map
LDFLAGS_LOADER += -Wl,--section-start=.text=$(BOOTLOAD)
LDFLAGS_APP     = -Wl,-Map,build/test.map
LDFLAGS_APP    += -Wl,--section-start=.text=0
CFLAGS          = -fpack-struct -Os -mcall-prologues -mmcu=$(UC)
CFLAGS         += -finline-functions --std=c11
CFLAGS         += -Wall -Winline -Wstrict-prototypes -Wno-main -Wfatal-errors -Wpedantic
CFLAGS         += -DBOOTLOAD=$(BOOTLOAD)

all:
	# Compile.
	avr-gcc $(CFLAGS) src/bootloader.c -c -o build/bootloader.o
	avr-gcc $(CFLAGS) src/test.c -c -o build/test.o

	# Link.
	avr-gcc $(LDFLAGS) $(LDFLAGS_LOADER) build/bootloader.o -o build/bootloader.elf
	avr-gcc $(LDFLAGS) $(LDFLAGS_APP) build/test.o -o build/test.elf
	avr-objcopy -j .text -j .data -O $(HEXFORMAT) build/bootloader.elf build/bootloader.hex
	avr-objcopy -j .text -j .data -O $(HEXFORMAT) build/test.elf build/test.hex

	# Combine.
	srec_cat build/test.hex -I build/bootloader.hex -I -o build/$(PROJNAME).hex -I

	# Report.
	# If bootloader .text size exceeds 512 bytes, it will no longer fit in the NRWW section!
	avr-size -B build/bootloader.elf build/test.elf

upload:
	sudo avrdude -p $(UC) -c usbasp -e -U flash:w:build/$(PROJNAME).hex

fuses:
	sudo avrdude -p $(UC) -c usbasp -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0x05:m
	# Default for the atmega168 is lfuse:62, hfuse:df, efuse:01
	# Low fuse for 8MHz clock: 0xE2
	# Extended fuse with 512 bytes bootloader, start at application start: 0x05

disasm:
	avr-gcc $(CFLAGS) bootloader.c -S -o build/bootloader.S && nano build/bootloader.S

clean:
	@mv build/empty.txt .
	rm -f build/*
	@mv empty.txt build/
