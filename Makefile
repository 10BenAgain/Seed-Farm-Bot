CC    := avr-gcc
COPY  := avr-objcopy

PROG  := main
PORT  := COM4
DUDE  := avrdude

CLOCK := 16000000UL
CHIP  := atmega328p

OPT   := -Os
FLAGS := -Wall -Wextra -g

all: $(PROG).hex

$(PROG).hex: $(PROG).c
	$(CC) $(FLAGS) $(OPT) -DF_CPU=$(CLOCK) -mmcu=$(CHIP) -o $(PROG).elf $(PROG).c
	$(COPY) -O ihex -R .eeprom $(PROG).elf $(PROG).hex

flash: $(PROG).hex
	$(DUDE) -c arduino -p $(CHIP) -P $(PORT) -b 115200 -U flash:w:$(PROG).hex

clean:
	rm -f *.o *.elf *.hex

