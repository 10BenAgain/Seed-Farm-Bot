#include <stdio.h>
#include <stdlib.h>

const char* file = "Makefile";

int main(int argc, char* argv[])
{
    if (argc < 6) {
        printf("Missing command line arguments..\n");
    }

    char* port = argv[1];
    char* start = argv[2];
    char* seeds = argv[3];
    char* button = argv[4];
    char* interval = argv[5];

    FILE* fp = fopen(file, "w");
    if (fp == NULL)
    {
        printf("error opening file %s", file);
        return -1;
    }

    fprintf(
            fp, "# --------- COMPILER & AVR Programs -----------------------#\n"
                "CC    \t:= avr-gcc\n"
                "COPY  \t:= avr-objcopy\n"
                "DUDE  \t:= avrdude\n\n"
            );

    fprintf(
            fp, "#---------- ARDUINO settings -----------------------------#\n"
                "PROG  \t:= main\n"
                "PORT  \t:= %s\n"
                "CLOCK \t:= 16000000UL\n"
                "CHIP  \t:= atmega328p\n\n",
                port
            );

    fprintf(
            fp, "#---------- FLAGS & PARAMS -------------------------------#\n"
                "OPT   \t:= -Os -mcall-prologues\n"
                "FLAGS \t:= -Wall -Wextra -g\n"
                "AVR\t\t:= -DF_CPU=$(CLOCK) -mmcu=$(CHIP)\n\n"
            );

    fprintf(
            fp, "#----------- BOT VALUES ----------------------------------#\n"
                "CDEFS := -D%s -DSEED_BUTTON_%s -DSTART=%s -DSEEDS_TO_STORE=%s\n\n",
                interval, button, start, seeds
            );

    fprintf(
            fp, "all: $(PROG).hex\n"
                "\n"
                "$(PROG).hex: $(PROG).c\n"
                "\t$(CC) $(FLAGS) $(OPT) $(AVR) $(CDEFS) -o $(PROG).elf $(PROG).c\n"
                "\t$(COPY) -O ihex -R .eeprom $(PROG).elf $(PROG).hex\n"
                "\n"
                "flash: $(PROG).hex\n"
                "\t$(DUDE) -c arduino -p $(CHIP) -P $(PORT) -b 115200 -U flash:w:$(PROG).hex\n"
                "\n"
                "clean:\n"
                "\trm -f *.o *.elf *.hex\n"
                ".PHONY: clean"
            );

    fclose(fp);

    system("make flash");

    return 0;
}