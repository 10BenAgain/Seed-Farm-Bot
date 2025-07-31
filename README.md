# Pokemon FRLG Seed Farm
All the following information is primarily for my own sanity so I don't forget the logic behind the process. The original FRLG seeds were not farmed by me and this process was initially created (and improved) by [imablisy](https://github.com/notblisy). See [Credits](##credits) section for all the details. 

## Links I Visit Often
1. Firered decomp functions
    - Init seed [func](https://github.com/pret/pokefirered/blob/4f5fe2a27941770cb1d7c33fcc1fd4c9495838af/src/main.c#L239)
    - Random [func](https://github.com/pret/pokefirered/blob/master/include/random.h#L23)
    - ..
2. Link to my result [spreadsheet](https://docs.google.com/spreadsheets/d/1ZNchTvoCpHFVPBscEJZG3JaaqR41D8VVnbXb23fzc44/edit?gid=0#gid=0)
3. Link to original FR seed [spreadsheet](https://docs.google.com/spreadsheets/d/1Mf3F4kTvNEYyDGWKVmMSiar3Fwh1PLzWVXUvGx9YxfA/edit)
4. Modify Nintendo DS to allow it to run with only 1 screen attached: [link](https://gbatemp.net/threads/turn-on-a-nds-lite-without-lower-or-upper-screen-or-convert-it-to-a-gba-ds.484432/)
5. Atmega328p technical reference [manual](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)
6. AVR libc [docs](https://avrdudes.github.io/avr-libc/)

## Arduino
This project requires an Arduino with an `atmega328p`chip. The source code specifically targets this chip. Other Arduino/Atmel chips might work depending on their design but you would have to read the documentation on the specific chip and edit the script register values. (See section above for reference manual link)

## Hardware
Setup for this botted seed farm requires a lot of hardware modification to the Nintendo DS. I wrote a more detailed guide on how to do this here: [Link](https://github.com/10BenAgain/nds-automation/blob/main/docs/hardware.md)

Since writing this, I have made a lot of improvements. Noteably, instead of using individial transistors and resistors and creating my own transistor array, I now use transistor array IC's. This is the one I chose - [ULN2003A](https://www.ti.com/product/ULN2003A). In addition, its a huge pain to have *both* of the console screens attached and I realized its easier to just have the console shell open and the top screen entirely removed. 

To communicate with my Arduino, I use a Raspberry PI. The PI is where I edit the Makefile and flash the board each time I need a new sequence ran on the console. The added benefit of the PI is that is has low power consumption and can run continously and headless. 

## Special save files
Thanks to [imablisy](https://github.com/notblisy), the `.sav` files stored in this repository are able to automatically save the initial seed to a special place in the game's unused space within the save file. This makes collecting seeds a lot easier than before. <ins>ALL credit for these files go to blisy. I did not make these files. </ins>

## Scripts
All the scripts in this repo are here for my own sanity. The process of extracting `.savs` and getting the seed data from them can be very tedious. 

### Setup.sh
Run this on a new environment. Assumes `Go` is already installed and in `$PATH`. This just builds the binaries and puts them in a directory for extractors to uses. 

### Extract.sh
Must be in the root of this repository and assumes that `Go` and `Python` are installed on the system and the SSH Client is already setup with an SSH key. Takes `1` argument: the path to the `.sav` file to extract the seeds from. This will `ssh` into the rpi client, pull the latest Arduino board settings from the Makefile, pipe that into `name.py` then extracts the seed data into `results.txt`. This just makes it easier to copy and paste the data into a the spread sheet.

### Sanity
Run this to check if the seed spreadsheet is currently aligned properly. Sends an `http` request to Google sheets and pulls the data down for checking. This is literally for my own sanity. If the sanity checks ever reach 0, this project is complete... 

## Why use Go?
I like it.

## Credits
This would not be possible without the following: 


1. [imablisy](https://github.com/notblisy) for starting this project and improving it. Making so many things possible and continuing to do awesome work for Gen 3 games. I cannot thank you enough for all you do. 
2. [Dean Camera's](https://github.com/abcminiuser) writeups for AVR things made this possible. Thank you, Dean
3. All the people in Blisy's retail RNG discord server especially Shaonova who always answers my questions. 

