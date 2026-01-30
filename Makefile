# --------- COMPILER & AVR Programs -----------------------#
CC    	:= avr-gcc
COPY  	:= avr-objcopy
DUDE  	:= avrdude

#---------- ARDUINO settings -----------------------------#
PROG  	:= main
PORT  	:= COM5
CLOCK 	:= 16000000UL
CHIP  	:= atmega328p

#---------- FLAGS & PARAMS -------------------------------#
OPT   	:= -Os -mcall-prologues
FLAGS 	:= -Wall -Wextra -g
AVR		:= -DF_CPU=$(CLOCK) -mmcu=$(CHIP)

#----------- BOT VALUES ----------------------------------#
START		:= 6969			 
SEEDS 		:= 420			 
BUTTON 		:= START			 
INTERVAL 	:= HALF 		

CDEFS		:= 					\
	-D$(INTERVAL) 				\
	-DSEED_BUTTON_$(BUTTON) 	\
	-DSTART=$(START) 			\
	-DSEEDS_TO_STORE=$(SEEDS) 	\

all: $(PROG).hex

$(PROG).hex: $(PROG).c
	$(CC) $(FLAGS) $(OPT) $(AVR) $(CDEFS) -o $(PROG).elf $(PROG).c
	$(COPY) -O ihex -R .eeprom $(PROG).elf $(PROG).hex

flash: $(PROG).hex
	$(DUDE) -c arduino -p $(CHIP) -P $(PORT) -b 115200 -U flash:w:$(PROG).hex

clean:
	rm -f *.o *.elf *.hex
.PHONY: clean