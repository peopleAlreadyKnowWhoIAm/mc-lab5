# Makefile made by me to compile targets for arduino based on atmega328p chip

DEVICE = atmega328p # From https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
TARGETS := master slave1

USB_UART := /dev/ttyUSB0

#SOURCES += src/main.c
SOURCES += $(wildcard src/*.c)

INC := inc

FREQUENCY = 16000000

#Toolchain

TOOLCHAIN := /usr/bin

CC := $(TOOLCHAIN)/avr-gcc
AS := $(TOOLCHAIN)/as
LD := $(TOOLCHAIN)/ld
OC := $(TOOLCHAIN)/avr-objcopy
OD := $(TOOLCHAIN)/objdump
OS := $(TOOLCHAIN)/avr-size

# Assembler flags
ASFLAGS := -Wall
ASFLAGS += -ggdb
# Linker flags
LFLAGS := -ggdb
# Compiler flags
CFLAGS := -c
CFLAGS += -Os
CFLAGS += -I $(INC)
CFLAGS += -D F_CPU=$(FREQUENCY)
# CFLAGS += --param=min-pagesize=0

#Device dependent
DFLAGS = -mmcu=$(DEVICE)


TARGETS := master slave1 slave2

.PHONY: all
.PHONY: clean flash

all: master_m.hex slave1_u.hex slave2_u.hex


%_m.o: %.c
	$(CC) $(CFLAGS) $(ASFLAGS) -mmcu=atmega2560 $< -o $@

%_m.elf: $(filter-out $(patsubst %, src/%_m.o,$(TARGETS) ),$(patsubst %.c, %_m.o, $(SOURCES))) src/%_m.o
	$(CC) $(ASFLAGS) $(LFLAGS) -mmcu=atmega2560 $^ -o $@

%_u.o: %.c
	$(CC) $(CFLAGS) $(ASFLAGS) -mmcu=atmega328p $< -o $@

%_u.elf: $(filter-out $(patsubst %, src/%_u.o,$(TARGETS) ),$(patsubst %.c, %_u.o, $(SOURCES))) src/%_u.o
	$(CC) $(ASFLAGS) $(LFLAGS) -mmcu=atmega328p $^ -o $@

%.hex: %.elf
	$(OC) -S -O ihex $< $@
	$(OS) $<

clean:
	rm -f src/*.o *.hex *.elf

flash: $(TARGET).hex
	avrdude -c arduino -P $(USB_UART) -p $(DEVICE) -U flash:w:$(TARGET).hex

