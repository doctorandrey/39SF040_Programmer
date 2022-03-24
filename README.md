#  39SF010A/020A/040 Programmer
##### SST39SF010A/020A/040 Arduino Mega Programmer
Based on the code of 39SF040 / 39SF020A Programmer by Bob Szymanski Published 9/24/2015.

Pinout should be used according to the pins as defined in the source file. SD card pinout is as provided by Arduino, on pins 51-53.
 
Based on Arduino Mega 2650. Serial terminal speed 57600.

Quick serial commands manual:

**HELP**                - Shows this help

**ID**                  - Shows Chip ID

**LS**                  - Lists files on SD-card

**DUMP arg1 arg2**      - Dump to Serial EEPROM data; arg1 - start address, arg2 - data length

**ES arg1**             - Erase Sector; param1 - sector's start address

**EC**                  - Erase Chip

**ETF arg1 arg2 arg3**	- Writes EEPROM data to File; arg1 - filename, arg2 - start address, arg3 - data length

**FTE arg1 arg2**        - Program ROM File data to EEPROM; arg1 - filename, arg2 - start destination address

Example Usage: 

FTE FIRMWARE.ROM 0x10000          - Writes file FIRMWARE.ROM to EEPROM starting from 0x10000

ETF FIRMWARE.ROM 0x10000 0x8000   - Writes EEPROM data starting from 0x10000 and length 0x8000 to file FIRMWARE.ROM

*(ETF stands for Eeprom To File, FTE - File To Eeprom, etc.)*

![Example workflow](https://github.com/doctorandrey/39SF040_Programmer/blob/6d708868ea0c198c6b744fc6fe6072bf5909737e/EEPROM_Prg.png)
