# --------- COMPILER & AVR Programs -----------------------#
CC    	:= avr-gcc
COPY  	:= avr-objcopy
DUDE  	:= avrdude

#---------- ARDUINO settings -----------------------------#
PROG  	:= main
PORT  	:= COM1
CLOCK 	:= 16000000UL
CHIP  	:= atmega328p

#---------- FLAGS & PARAMS -------------------------------#
OPT   	:= -Os -mcall-prologues
FLAGS 	:= -Wall -Wextra -g
AVR		:= -DF_CPU=$(CLOCK) -mmcu=$(CHIP)

#----------- BOT VALUES ----------------------------------#
CDEFS := -DDEFAULT -DSEED_BUTTON_A -DSTART=6969 -DSEEDS_TO_STORE=2000

all: $(PROG).hex

$(PROG).hex: $(PROG).c
	$(CC) $(FLAGS) $(OPT) $(AVR) $(CDEFS) -o $(PROG).elf $(PROG).c
	$(COPY) -O ihex -R .eeprom $(PROG).elf $(PROG).hex

flash: $(PROG).hex
	$(DUDE) -c arduino -p $(CHIP) -P $(PORT) -b 115200 -U flash:w:$(PROG).hex

clean:
	rm -f *.o *.elf *.hex
.PHONY: clean