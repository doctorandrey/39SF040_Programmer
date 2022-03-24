/*
 * Based on 39SF040 / 39SF020A Programmer by Bob Szymanski
 * Published 9/24/2015.
 * Pinout should be used according to the pins as defined.
 * SD card pinout is as provided by Arduino, on pins 51-53.
 * 
 * This version is built using the Arduino Mega 2650.
 */
 
#include <SPI.h>
#include <SD.h>
#include "SerialCommands.h"
#include "utils.h"

#define SECTOR_SIZE   4096    /* Must be 4096 bytes for 39SF040 */
#define SST_ID        0xBF    /* SST Manufacturer's ID code   */
#define SST_39SF040   0xB7    /* SST 39SF040 device code      */

#define _WE A8 
#define _OE A9 
#define _CE A10 

//#define ADDR1 0x5555
//#define DATA1 0xAA

//#define ADDR2 0x2AAA
//#define DATA2 0x55

//#define ADDR3 0x5555
//#define DATA3 0xA0

#define D0 22 
#define D1 23 
#define D2 24 
#define D3 25 
#define D4 26 
#define D5 27
#define D6 28 
#define D7 29

#define ADR0 30
#define ADR1 31 
#define ADR2 32 
#define ADR3 33 
#define ADR4 34 
#define ADR5 35
#define ADR6 36 
#define ADR7 37
#define ADR8 38 
#define ADR9 39 
#define ADR10 40 
#define ADR11 41 
#define ADR12 42 
#define ADR13 43
#define ADR14 44 
#define ADR15 45
#define ADR16 46 
#define ADR17 47
#define ADR18 A11

#define SD_SS_PIN 53

#define LED_GREEN 48
#define LED_RED 49

bool SD_OK = true;

void cmd_unrecognized(SerialCommands* sender, const char* cmd);
//void cmd_run_script(SerialCommands* sender);
void cmd_erase_sector(SerialCommands*);
void cmd_erase_chip(SerialCommands* sender);
void cmd_eeprom_to_file(SerialCommands* sender);
void cmd_file_to_eeprom(SerialCommands* sender);
void cmd_chip_id(SerialCommands* sender);
void cmd_list_dir(SerialCommands* sender);
void cmd_dump(SerialCommands* sender);
void cmd_help(SerialCommands* sender);

char serial_command_buffer_[64];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\n", " ");

//SerialCommand cmd_run_script_("RUN", cmd_run_script);
SerialCommand cmd_erase_sector_("ES", cmd_erase_sector);
SerialCommand cmd_erase_chip_("EC", cmd_erase_chip);
SerialCommand cmd_eeprom_to_file_("ETF", cmd_eeprom_to_file);
SerialCommand cmd_file_to_eeprom_("FTE", cmd_file_to_eeprom);
SerialCommand cmd_chip_id_("ID", cmd_chip_id);
SerialCommand cmd_list_dir_("LS", cmd_list_dir);
SerialCommand cmd_dump_("DUMP", cmd_dump);
SerialCommand cmd_help_("HELP", cmd_help);

void setup()
{
  Serial.begin(57600);
  delay(2);
  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  //serial_commands_.AddCommand(&cmd_run_script_);
  serial_commands_.AddCommand(&cmd_erase_sector_);
  serial_commands_.AddCommand(&cmd_erase_chip_);
  serial_commands_.AddCommand(&cmd_eeprom_to_file_);
  serial_commands_.AddCommand(&cmd_file_to_eeprom_);
  serial_commands_.AddCommand(&cmd_chip_id_);
  serial_commands_.AddCommand(&cmd_list_dir_);
  serial_commands_.AddCommand(&cmd_dump_);
  serial_commands_.AddCommand(&cmd_help_);
  
  Serial.println(F("*** SST39SF010A/020A/040 Arduino Mega Programmer ***"));

  if (!SD.begin(SD_SS_PIN))
  {
    SD_OK = false;
    Serial.println("Error initializing SD Card!");
  }
  
  pinMode(_WE, OUTPUT);
  digitalWrite(_WE, HIGH);
  
  pinMode(_OE, OUTPUT);
  digitalWrite(_OE, HIGH);
  
  pinMode(_CE, OUTPUT);
  digitalWrite(_CE, LOW);
  
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  digitalWrite(D0, LOW);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);

  pinMode(ADR0, OUTPUT);
  pinMode(ADR1, OUTPUT);
  pinMode(ADR2, OUTPUT);
  pinMode(ADR3, OUTPUT);
  pinMode(ADR4, OUTPUT);
  pinMode(ADR5, OUTPUT);
  pinMode(ADR6, OUTPUT);
  pinMode(ADR7, OUTPUT);
  pinMode(ADR8, OUTPUT);
  pinMode(ADR9, OUTPUT);
  pinMode(ADR10, OUTPUT);
  pinMode(ADR11, OUTPUT);
  pinMode(ADR12, OUTPUT);
  pinMode(ADR13, OUTPUT);
  pinMode(ADR14, OUTPUT);
  pinMode(ADR15, OUTPUT);
  pinMode(ADR16, OUTPUT);
  pinMode(ADR17, OUTPUT);
  pinMode(ADR18, OUTPUT);
  
  delay(100);
  
  digitalWrite(ADR0, LOW);
  digitalWrite(ADR1, LOW);
  digitalWrite(ADR2, LOW);
  digitalWrite(ADR3, LOW);
  digitalWrite(ADR4, LOW);
  digitalWrite(ADR5, LOW);
  digitalWrite(ADR6, LOW);
  digitalWrite(ADR7, LOW);
  digitalWrite(ADR8, LOW);
  digitalWrite(ADR9, LOW);
  digitalWrite(ADR10, LOW);
  digitalWrite(ADR11, LOW);
  digitalWrite(ADR12, LOW);
  digitalWrite(ADR13, LOW);
  digitalWrite(ADR14, LOW);
  digitalWrite(ADR15, LOW);
  digitalWrite(ADR16, LOW);
  digitalWrite(ADR17, LOW);
  digitalWrite(ADR18, LOW);
  
  pinMode(SD_SS_PIN, OUTPUT); //Needed for SD to work on the Mega.
  delay(100);
}

void loop()
{  
  serial_commands_.ReadSerial(); 
}

/* ====================================================================================== */

void cmd_help(SerialCommands* sender)
{
  sender->GetSerial()->println(F("HELP \t\t\t- Shows this help"));
  sender->GetSerial()->println(F("ID \t\t\t- Shows Chip ID"));
  sender->GetSerial()->println(F("LS \t\t\t- Lists files on SD-card"));
  sender->GetSerial()->println(F("DUMP arg1 arg2 \t\t- Dump to Serial EEPROM data; arg1 - start address, arg2 - data length"));
  sender->GetSerial()->println(F("ES arg1 \t\t- Erase Sector; param1 - sector's start address"));
  sender->GetSerial()->println(F("EC \t\t\t- Erase Chip"));
  sender->GetSerial()->println(F("ETF arg1 arg2 arg3\t- Writes EEPROM data to File; arg1 - filename, arg2 - start address, arg3 - data length"));
  sender->GetSerial()->println(F("FTE arg1 arg2 \t\t- Program ROM File data to EEPROM; arg1 - filename, arg2 - start destination address"));
  sender->GetSerial()->println(F("Usage: FTE FIRMWARE.ROM 0x10000 - Writes file FIRMWARE.ROM to EEPROM starting from 0x10000"));
  sender->GetSerial()->println(F("       ETF FIRMWARE.ROM 0x10000 0x8000 - Writes EEPROM data starting from 0x10000 and length 0x8000 to file FIRMWARE.ROM"));
  sender->GetSerial()->println(F(""));
}

void cmd_list_dir(SerialCommands* sender)
{
  if (!SD_OK){
    sender->GetSerial()->println("ERROR: SD Not Ready!");
    return;
  }
  sender->GetSerial()->println("SD card listing");
  sender->GetSerial()->println("------------------------------------------");
  File root = SD.open("/");
  printDirectory(root, 0);
  sender->GetSerial()->println("------------------------------------------");
  sender->GetSerial()->println("");
}

void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
  sender->GetSerial()->print("Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");
}

void cmd_run_script(SerialCommands* sender)
{
  char* file_str = sender->Next();
  if (file_str == NULL)
  {
    sender->GetSerial()->println("ERROR: No file name given!");
    return;
  }
  if (!SD_OK){
    sender->GetSerial()->println("ERROR: SD Not Ready!");
    return;
  }
  File configFile = SD.open(file_str);
  if (!configFile)
  {
    sender->GetSerial()->println("Error reading script file!");
  }
  else
  {
    String confString = "";
    char letter = configFile.read();
    while (letter > 0 && letter != EOF && letter != '\n')
    {
      confString += letter; 
      letter = configFile.read();
    }
      configFile.close();
      long dataAddr = getValue(confString, ':', 0).toInt();
      String romFileName = getValue(confString, ':', 1);
      String chkFileName = getValue(romFileName, '.', 0) + ".chk";
      long addressCount = fileToEeprom(romFileName, dataAddr);
      eepromToFile(chkFileName, 0, addressCount);
      compareFiles(romFileName, chkFileName);
    }
}

void cmd_erase_sector(SerialCommands* sender)
{ 
  char* addr_str = sender->Next();
  if (addr_str == NULL)
  {
    sender->GetSerial()->println("Error! Address is not set!");
    return;
  }
  unsigned long addr = strToLong(addr_str);

  if (addr % SECTOR_SIZE != 0)
  {
    sender->GetSerial()->print("Error! Sector's begining address not multiple to ");
    sender->GetSerial()->println(SECTOR_SIZE);
  }
  else
  {
    sender->GetSerial()->print("Erasing sector: 0x");
    sender->GetSerial()->println(addr, HEX);
    sectorErase(addr);
    setReadMode(0);
    sender->GetSerial()->println("Done!");
  }
}

void cmd_erase_chip(SerialCommands* sender)
{
  sender->GetSerial()->print("Erasing chip...");
  chipErase();
  delay(500);
  setReadMode(0);
  sender->GetSerial()->println("Done!");
}

void cmd_chip_id(SerialCommands* sender)
{
  signalsState("ID_BEG");
  sender->GetSerial()->print("EEPROM Chip ID: 0x");
  sender->GetSerial()->println(getChipID(), HEX);
  signalsState("ID_END");
}

void cmd_dump(SerialCommands* sender)
{
  char* start_addr_str = sender->Next(); 
  char* data_len_str = sender->Next();
  if ((start_addr_str != NULL) && (data_len_str != NULL))
  {
    unsigned long start_addr = strToLong(start_addr_str);
    unsigned long data_len = strToLong(data_len_str);
    if (data_len > 0)
    {
      sender->GetSerial()->println("EEPROM Memory dump:");
      eepromToSerial(start_addr, data_len); 
    }
    else
    {
      sender->GetSerial()->println("Error! Wrong data length!");
    }
  }
  else
  {
    sender->GetSerial()->println("Error in start address or data length!");
  }
}

void cmd_eeprom_to_file(SerialCommands* sender)
{
  if (!SD_OK){
    sender->GetSerial()->println("ERROR: SD Not Ready!");
    return;
  }
  char* file_str = sender->Next();
  if (file_str == NULL)
  {
    sender->GetSerial()->println("Error! No File name given!");
    return;
  }

  char* start_addr_str = sender->Next(); 
  char* data_len_str = sender->Next();
  if ((start_addr_str != NULL) && (data_len_str != NULL))
  {
    unsigned long start_addr = strToLong(start_addr_str);
    unsigned long data_len = strToLong(data_len_str);
    if (data_len > 0)
    {
      sender->GetSerial()->print("Reading EEPROM to file: ");
      sender->GetSerial()->println(file_str);
      eepromToFile(file_str, start_addr, data_len);
      sender->GetSerial()->println("Done!");
    }
    else
    {
      sender->GetSerial()->println("Error! Wrong data length!");
      sender->GetSerial()->print("start_addr: ");
      sender->GetSerial()->println(start_addr);
      sender->GetSerial()->print("data_len: ");
      sender->GetSerial()->println(data_len);
    }
  }
}

void cmd_file_to_eeprom(SerialCommands* sender)
{
  if (!SD_OK){
    sender->GetSerial()->println("ERROR: SD Not Ready!");
    return;
  }
  char* file_str = sender->Next();
  if (file_str == NULL)
  {
    sender->GetSerial()->println("Error! No File name given!");
    return;
  }
  char* addr_str = sender->Next();
  if (addr_str == NULL)
  {
    sender->GetSerial()->println("Error! No Address given!");
    return;
  }
  
  unsigned long addr = strToLong(addr_str);
  sender->GetSerial()->print("Writing to EEPROM from file: ");
  sender->GetSerial()->println(file_str);
  unsigned long bytesWrote = fileToEeprom(file_str, addr);
  sender->GetSerial()->print("Wrote ");
  sender->GetSerial()->print(bytesWrote);
  sender->GetSerial()->println(" bytes");
  
}

unsigned long fileToEeprom(String fileName, unsigned long address)
{ 
  int datum = 0;
  unsigned long addressCount = 0;
  File sourceFile = SD.open(fileName);
  delay(500);
  if (!sourceFile)
  {
    Serial.println("Error reading ROM file!");
  }
  Serial.print("Writing to EEPROM address: 0x");
  Serial.println(address, HEX);
  while (true)
  {
    datum = sourceFile.read();
    if (datum < 0) { break; } 
    programByte(address, datum);
    addressCount++;
    address++;
    delayMicroseconds(30);
    if (address % 1024 == 0) {
      Serial.print("|");
    }  
  }
  Serial.println("");
  sourceFile.close();
  return addressCount;
}

void eepromToSerial(unsigned long offset, unsigned long dataLen)
{
  setReadMode(0);
  int datum;
  String datum_str;
  String offset_str = "";
  unsigned long count;
  unsigned long end_addr = offset + dataLen;
  int str_len = String(end_addr, HEX).length();
  int len_diff, i;
  Serial.print("Start address: 0x");
  Serial.println(offset, HEX);
  Serial.print("Data length: 0x");
  Serial.println(dataLen, HEX);
  for (count = 0; count < dataLen; count++)
  {
    offset_str = "";
    datum = 0;
    writeAddress(offset);
    delayMicroseconds(5);
    datum = DigitalReadByte();
    if (datum < 16){
      datum_str = "0" + String(datum, HEX);  
    } else {
      datum_str = String(datum, HEX);
    }
    if ((offset % 16 == 0) || (count == 0)){
      Serial.println("");
      len_diff = str_len - String(offset, HEX).length();
      if (len_diff > 0){
        for (i = 0; i < len_diff; i++){
          offset_str  += "0";
        } 
      }
      offset_str += String(offset, HEX);
      Serial.print("0x" + offset_str);
      Serial.print("| ");  
    } 
    Serial.print(datum_str);
    Serial.print(" ");
    delayMicroseconds(5);
    offset++;
  }
  setWriteMode();
  Serial.println("\n"); 
}

void eepromToFile(String fileName, unsigned long offset, unsigned long dataLen)
{
  setReadMode(0);
  int datum;
  unsigned long address;
  if (SD.exists(fileName)) {
    SD.remove(fileName);  
  }
  
  File copyFile = SD.open(fileName, O_CREAT | O_WRITE);
  if (!copyFile)
  {
    Serial.println("Error opening data file!");
  }
  Serial.print("Start address: 0x");
  Serial.println(offset, HEX);
  Serial.print("Data length: 0x");
  Serial.println(dataLen, HEX);
  Serial.println("Writing copy to file...");
  for (address = 0; address < dataLen; address++)
  {
    datum = 0x00;
    writeAddress(offset);
    delayMicroseconds(5);
    datum = DigitalReadByte();
    copyFile.write(datum);
    delayMicroseconds(5);
    offset++;
    if (address % 1024 == 0){
      Serial.print("|");  
    }
  }
  Serial.println("");
  copyFile.flush();
  copyFile.close();
  setWriteMode(); 
}

bool compareFiles(String srcFileName, String copyFileName)
{
  long errors = 0;
  long address = 0;
  int rawData = 0;
  int datum = 0;
  File copyFile = SD.open(copyFileName); 
  File sourceFile = SD.open(srcFileName);
  if (!sourceFile || !copyFile)
  {
    Serial.println("Error reading original / verification file!");
    return false; 
  } 
  Serial.println("Verifying data...");
  while(true){
    rawData = sourceFile.read();
    if (rawData < 0) { break; }
    datum = copyFile.read();
    if (address % 1024 == 0){
      Serial.print("|");  
    }
    if (datum != rawData){
      errors++;
      Serial.print(address, HEX);
      Serial.print(": ");
      Serial.print(rawData, HEX);
      Serial.print(" <> ");
      Serial.println(datum, HEX);
    }
    address++;
  }

  sourceFile.close();
  copyFile.close();
  
  Serial.println("");
  if (errors > 0){
    Serial.println("Verification FAILED!");
    return false;
  }else{
    Serial.println("Verification OK!");
    return true;
  }
}

void setReadMode(unsigned long addr)
{
  writeAddress(addr);
  
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(D7, INPUT);
  delay(1);
  
  digitalWrite(_CE, LOW);
  digitalWrite(_OE, LOW);
  digitalWrite(_WE, HIGH);
  delay(1);
}

void setWriteMode()
{
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  delay(1);
}

void writeAddress(long value)
{
  digitalWrite(ADR0, (value >> 0) & 0x01);
  digitalWrite(ADR1, (value >> 1) & 0x01);
  digitalWrite(ADR2, (value >> 2) & 0x01);
  digitalWrite(ADR3, (value >> 3) & 0x01);
  digitalWrite(ADR4, (value >> 4) & 0x01);
  digitalWrite(ADR5, (value >> 5) & 0x01);
  digitalWrite(ADR6, (value >> 6) & 0x01);
  digitalWrite(ADR7, (value >> 7) & 0x01);
  digitalWrite(ADR8, (value >> 8) & 0x01);
  digitalWrite(ADR9, (value >> 9) & 0x01);
  digitalWrite(ADR10, (value >> 10) & 0x01);
  digitalWrite(ADR11, (value >> 11) & 0x01);
  digitalWrite(ADR12, (value >> 12) & 0x01);
  digitalWrite(ADR13, (value >> 13) & 0x01);
  digitalWrite(ADR14, (value >> 14) & 0x01);
  digitalWrite(ADR15, (value >> 15) & 0x01);
  digitalWrite(ADR16, (value >> 16) & 0x01);
  digitalWrite(ADR17, (value >> 17) & 0x01);
  digitalWrite(ADR18, (value >> 18) & 0x01);
}

unsigned int DigitalReadByte()
{
  unsigned int toReturn = 0x00;
  if (digitalRead(D0) == HIGH) {bitSet(toReturn, 0); } else { bitClear(toReturn, 0); }   
  if (digitalRead(D1) == HIGH) {bitSet(toReturn, 1); } else { bitClear(toReturn, 1); } 
  if (digitalRead(D2) == HIGH) {bitSet(toReturn, 2); } else { bitClear(toReturn, 2); } 
  if (digitalRead(D3) == HIGH) {bitSet(toReturn, 3); } else { bitClear(toReturn, 3); } 
  if (digitalRead(D4) == HIGH) {bitSet(toReturn, 4); } else { bitClear(toReturn, 4); } 
  if (digitalRead(D5) == HIGH) {bitSet(toReturn, 5); } else { bitClear(toReturn, 5); } 
  if (digitalRead(D6) == HIGH) {bitSet(toReturn, 6); } else { bitClear(toReturn, 6); } 
  if (digitalRead(D7) == HIGH) {bitSet(toReturn, 7); } else { bitClear(toReturn, 7); } 
  return toReturn;
}

void writeData(unsigned int value)
{
  digitalWrite(D0, (value >> 0) & 0x01);
  digitalWrite(D1, (value >> 1) & 0x01);
  digitalWrite(D2, (value >> 2) & 0x01);
  digitalWrite(D3, (value >> 3) & 0x01);
  digitalWrite(D4, (value >> 4) & 0x01);
  digitalWrite(D5, (value >> 5) & 0x01);
  digitalWrite(D6, (value >> 6) & 0x01);
  digitalWrite(D7, (value >> 7) & 0x01);
  delayMicroseconds(5); 
}

void programByte(long address, unsigned int data)
{
  digitalWrite(_OE, HIGH);
  digitalWrite(_WE, HIGH);
  digitalWrite(_CE, LOW);
  
  long ADDR_1 = 0x5555;
  long ADDR_2 = 0x2AAA;
  long ADDR_3 = 0x5555;
  
  //perform the 3 byte program code:
  writeAddress(ADDR_1);
  writeData(0xAA);
  
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_2);
  writeData(0x55);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_3);
  writeData(0xA0); 
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
 
  writeAddress(address);
  writeData(data);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
}

byte getChipID()
{
  long ADDR_1 = 0x5555;
  long ADDR_2 = 0x2AAA;
  long ADDR_3 = 0x5555;
  
  digitalWrite(_CE, LOW);
  digitalWrite(_OE, HIGH);
  digitalWrite(_WE, HIGH);
  
  //perform the 3 byte entry program code:
  writeAddress(ADDR_1);
  writeData(0xAA);
  
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_2);
  writeData(0x55);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_3);
  writeData(0x90); 
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  delayMicroseconds(1);

  setReadMode(1);
  byte id = DigitalReadByte();
  setWriteMode();
  
  //perform the 3 byte exit program code:
  writeAddress(ADDR_1);
  writeData(0xAA);
  
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_2);
  writeData(0x55);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_3);
  writeData(0xF0); 
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
 
  return id;
}


/* Chip erase follows the following pattern:
 * Load 6 specific bytes at specific addresses
 * Delay shortly for erase to complete.
 */ 
void chipErase()
{
  writeAddress(0);
  writeData(0);
  
  digitalWrite(_CE, LOW);
  digitalWrite(_OE, HIGH);
  digitalWrite(_WE, HIGH);
  
  long ADDR_1 = 0x5555;
  long ADDR_2 = 0x2AAA;
  long ADDR_3 = 0x5555;
  long ADDR_4 = 0x5555;
  long ADDR_5 = 0x2AAA;
  long ADDR_6 = 0x5555;
  
  byte DATA_1 = 0xAA;
  byte DATA_2 = 0x55;
  byte DATA_3 = 0x80;
  byte DATA_4 = 0xAA;
  byte DATA_5 = 0x55;
  byte DATA_6 = 0x10;
  
  //perform the 6 byte program code:
  writeAddress(ADDR_1);
  writeData(DATA_1);
  
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_2);
  writeData(DATA_2);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_3);
  writeData(DATA_3);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_4);
  writeData(DATA_4);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_5);
  writeData(DATA_5);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_6);
  writeData(DATA_6);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  delay(1000);
}

void sectorErase(long address)
{
  digitalWrite(_CE, LOW);
  digitalWrite(_OE, HIGH);
  digitalWrite(_WE, HIGH);
  
  long ADDR_1 = 0x5555;
  long ADDR_2 = 0x2AAA;
  long ADDR_3 = 0x5555;
  long ADDR_4 = 0x5555;
  long ADDR_5 = 0x2AAA;
  long ADDR_6 = address;
  
  byte DATA_1 = 0xAA;
  byte DATA_2 = 0x55;
  byte DATA_3 = 0x80;
  byte DATA_4 = 0xAA;
  byte DATA_5 = 0x55;
  byte DATA_6 = 0x30;
  
  //perform the 6 byte program code:
  writeAddress(ADDR_1);
  writeData(DATA_1);
  
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_2);
  writeData(DATA_2);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_3);
  writeData(DATA_3);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_4);
  writeData(DATA_4);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_5);
  writeData(DATA_5);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  
  writeAddress(ADDR_6);
  writeData(DATA_6);
  digitalWrite(_WE, LOW);
  digitalWrite(_WE, HIGH);
  delay(100);
}

void signalsState(String str){
  String ce = digitalRead(_CE) ? "HIGH" : "LOW";
  String we = digitalRead(_WE) ? "HIGH" : "LOW";
  String oe = digitalRead(_OE) ? "HIGH" : "LOW";
  Serial.println(str + " ------------------------------------------");
  Serial.print("\tCE:" + ce + "\t\t");
  Serial.print("WE:" + we + "\t\t");
  Serial.println("OE:" + oe);
  Serial.println(str + " ------------------------------------------");
  Serial.println("");
}
