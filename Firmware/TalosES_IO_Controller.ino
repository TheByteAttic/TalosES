// TalosES BIOS, for the ATmega1284P-PU
// Created by Bernardo Kastrup at The Byte Attic!
// Copyright (c) 2023, all rights reserved
// Last edit: 15 November 2023

#include <SPI.h>  // SPI library built into Arduino IDE
#include <SD.h>   // SD-card library built into Arduino IDE

// Define ATmega1284P pinout using Arduino pin codes
#define IMF     0
#define OMF     1
#define SIMF    2
#define COMF    3
#define CS      4
//#define MOSI    5
//#define MISO    6
//#define SCK     7
#define Rx      10  // Serial receiver pin 16 on the ATmage1284p
#define Tx      11  // Serial transmitter pin 17 on the ATmega1284p
#define STPSLC  12  // Active LOW
#define STPCLK  13
#define HLT     14  // Active HIGH
#define RST     15  // Active HIGH
#define DRD     16  // Active LOW
#define DWR     17  // Active LOW
#define IRD     18  // Active LOW
#define IWR     19  // Active LOW
#define SLC0    20
#define SLC1    21
#define SLC2    22
#define LATCH   23  // Active HIGH

// Useful definitions about ASCII codes
#define KB_CR     13  // Carriage Return
#define KB_LF     10  // Line Feed
#define KB_ESC    27  // ESCape
#define KB_BS     8   // Back Space
#define KB_SPACE  32  // Space
#define KB_COMMA  44  // Comma

// DELAY is the time to wait for a hardware transaction
#define DELAY 1

bool step = true; // Global step mode indicator
bool run = false; // Global run mode indicator
const int chipSelect = CS; // Required by SPI library
byte editLine[80]; // String where user types in commands
String editText; // Same as above, but cast to String
byte pos = 0; // Position in edit line currently occupied by cursor
void(* resetFunc) (void) = 0; // Software reset fuction at address 0

const String opcodeTable[256] = { // Table to translate mnemonics into opcode values
  "NOP\r",
  "AH=$nn\r",
  "AL=$nn\r",
  "BH=$nn\r",
  "BL=$nn\r",
  "JAH=$nn\r",
  "JAL=$nn\r",
  "AR1++\r",
  "AR2++\r",
  "AR1++,AR2++\r",
  "AH=(AR1)\r",
  "AL=(AR1)\r",
  "BH=(AR1)\r",
  "BL=(AR1)\r",
  "JAH=(AR1)\r",
  "JAL=(AR1)\r",
  "AH=(AR2)\r",
  "AL=(AR2)\r",
  "BH=(AR2)\r",
  "BL=(AR2)\r",
  "JAH=(AR2)\r",
  "JAL=(AR2)\r",
  "AH=(AR1),AR2++\r",
  "AL=(AR1),AR2++\r",
  "BH=(AR1),AR2++\r",
  "BL=(AR1),AR2++\r",
  "JAH=(AR1),AR2++\r",
  "JAL=(AR1),AR2++\r",
  "AH=(AR2),AR1++\r",
  "AL=(AR2),AR1++\r",
  "BH=(AR2),AR1++\r",
  "BL=(AR2),AR1++\r",
  "JAH=(AR2),AR1++\r",
  "JAL=(AR2),AR1++\r",
  "(AR1)=AH\r",
  "(AR1)=AL\r",
  "(AR1)=BH\r",
  "(AR1)=BL\r",
  "(AR1)=RH\r",
  "(AR1)=RL\r",
  "(AR2)=AH\r",
  "(AR2)=AL\r",
  "(AR2)=BH\r",
  "(AR2)=BL\r",
  "(AR2)=RH\r",
  "(AR2)=RL\r",
  "(AR1)=AH,AR2++\r",
  "(AR1)=AL,AR2++\r",
  "(AR1)=BH,AR2++\r",
  "(AR1)=BL,AR2++\r",
  "(AR1)=RH,AR2++\r",
  "(AR1)=RL,AR2++\r",
  "(AR2)=AH,AR1++\r",
  "(AR2)=AL,AR1++\r",
  "(AR2)=BH,AR1++\r",
  "(AR2)=BL,AR1++\r",
  "(AR2)=RH,AR1++\r",
  "(AR2)=RL,AR1++\r",
  "A=B\r",
  "A=R\r",
  "B=A\r",
  "B=R\r",
  "AR1=A\r",
  "AR1=B\r",
  "AR1=R\r",
  "AR2=A\r",
  "AR2=B\r",
  "AR2=R\r",
  "JA=A\r",
  "JA=B\r",
  "JA=R\r",
  "AH=BH\r",
  "AH=RH\r",
  "BH=AH\r",
  "BH=RH\r",
  "JAH=AH\r",
  "JAH=BH\r",
  "JAH=RH\r",
  "AL=BL\r",
  "AL=RL\r",
  "BL=AL\r",
  "BL=RL\r",
  "JAL=AL\r",
  "JAL=BL\r",
  "JAL=RL\r",
  "AR1=A,AR2=A\r",
  "AR1=A,AR2++\r",
  "AR1=B,AR2++\r",
  "AR1=R,AR2++\r",
  "AR2=A,AR1++\r",
  "AR2=B,AR1++\r",
  "AR2=R,AR1++\r",
  "JA=A,AR1++\r",
  "JA=B,AR1++\r",
  "JA=R,AR1++\r",
  "AH=BH,AR1++\r",
  "AH=RH,AR1++\r",
  "BH=AH,AR1++\r",
  "BH=RH,AR1++\r",
  "JAH=AH,AR1++\r",
  "JAH=BH,AR1++\r",
  "JAH=RH,AR1++\r",
  "AL=BL,AR1++\r",
  "AL=RL,AR1++\r",
  "BL=AL,AR1++\r",
  "BL=RL,AR1++\r",
  "JAL=AL,AR1++\r",
  "JAL=BL,AR1++\r",
  "JAL=RL,AR1++\r",
  "JA=A,AR2++\r",
  "JA=B,AR2++\r",
  "JA=R,AR2++\r",
  "AH=BH,AR2++\r",
  "AH=RH,AR2++\r",
  "BH=AH,AR2++\r",
  "BH=RH,AR2++\r",
  "JAH=AH,AR2++\r",
  "JAH=BH,AR2++\r",
  "JAH=RH,AR2++\r",
  "AL=BL,AR2++\r",
  "AL=RL,AR2++\r",
  "BL=AL,AR2++\r",
  "BL=RL,AR2++\r",
  "JAL=AL,AR2++\r",
  "JAL=BL,AR2++\r",
  "JAL=RL,AR2++\r",
  "AH=BH,BL=AL\r",
  "AH=BH,JAL=AL\r",
  "AH=BH,JAL=BL\r",
  "BH=AH,AL=BL\r",
  "BH=AH,JAL=AL\r",
  "BH=AH,JAL=BL\r",
  "JAH=AH,AL=BL\r",
  "JAH=BH,AL=BL\r",
  "JAH=AH,BL=AL\r",
  "JAH=BH,BL=AL\r",
  "AH=BH,BL=AL,AR1++\r",
  "AH=BH,JAL=AL,AR1++\r",
  "AH=BH,JAL=BL,AR1++\r",
  "BH=AH,AL=BL,AR1++\r",
  "BH=AH,JAL=AL,AR1++\r",
  "BH=AH,JAL=BL,AR1++\r",
  "JAH=AH,AL=BL,AR1++\r",
  "JAH=BH,AL=BL,AR1++\r",
  "JAH=AH,BL=AL,AR1++\r",
  "JAH=BH,BL=AL,AR1++\r",
  "AH=BH,BL=AL,AR2++\r",
  "AH=BH,JAL=AL,AR2++\r",
  "AH=BH,JAL=BL,AR2++\r",
  "BH=AH,AL=BL,AR2++\r",
  "BH=AH,JAL=AL,AR2++\r",
  "BH=AH,JAL=BL,AR2++\r",
  "JAH=AH,AL=BL,AR2++\r",
  "JAH=BH,AL=BL,AR2++\r",
  "JAH=AH,BL=AL,AR2++\r",
  "JAH=BH,BL=AL,AR2++\r",
  "AH=RH,BH=RH\r",
  "AL=RL,BL=RL\r",
  "AH=RH,BL=RL\r",
  "BH=RH,AL=RL\r",
  "AH=RH,AL=RL\r",
  "BH=RH,BL=RL\r",
  "AH=RH,BH=RH,AR1++\r",
  "AL=RL,BL=RL,AR1++\r",
  "AH=RH,BL=RL,AR1++\r",
  "BH=RH,AL=RL,AR1++\r",
  "AH=RH,AL=RL,AR1++\r",
  "BH=RH,BL=RL,AR1++\r",
  "AH=RH,BH=RH,AR2++\r",
  "AL=RL,BL=RL,AR2++\r",
  "AH=RH,BL=RL,AR2++\r",
  "BH=RH,AL=RL,AR2++\r",
  "AH=RH,AL=RL,AR2++\r",
  "BH=RH,BL=RL,AR2++\r",
  "RH=AH-BH,RL=AL+BL\r",
  "RH=AH-BH,RL=AL-BL\r",
  "RH=AH+BH,RL=AL+BL\r",
  "R=A+B\r",
  "R=A-B\r",
  "RH=AH-BH,RL=AL+BL,AR1++\r",
  "RH=AH-BH,RL-AL-BL,AR1++\r",
  "RH=AH+BH,RL=AL+BL,AR1++\r",
  "R=A+B,AR1++\r",
  "R=A-B,AR1++\r",
  "RH=AH-BH,RL=AL+BL,AR2++\r",
  "RH=AH-BH,RL-AL-BL,AR2++\r",
  "RH=AH+BH,RL=AL+BL,AR2++\r",
  "R=A+B,AR2++\r",
  "R=A-B,AR2++\r",
  "JP\r",
  "JZH\r",
  "JZL\r",
  "JSH\r",
  "JSL\r",
  "JCH\r",
  "JCL\r",
  "JNZH\r",
  "JNZL\r",
  "JNSH\r",
  "JNSL\r",
  "JNCH\r",
  "JNCL\r",
  "JIM\r",
  "JOM\r",
  "JNIM\r",
  "JNOM\r",
  "JNZL,AH=(AR1),AR2++\r",
  "JNZL,BH=(AR1),AR2++\r",
  "JNZL,JAH=(AR1),AR2++\r",
  "JNZL,JAL=(AR1),AR2++\r",
  "JNZL,AH=(AR2),AR1++\r",
  "JNZL,BH=(AR2),AR1++\r",
  "JNZL,JAH=(AR2),AR1++\r",
  "JNZL,JAL=(AR2),AR1++\r",
  "JNZL,(AR1)=AH,AR2++\r",
  "JNZL,(AR1)=BH,AR2++\r",
  "JNZL,(AR1)=RH,AR2++\r",
  "JNZL,(AR1)=RL,AR2++\r",
  "JNZL,(AR2)=AH,AR1++\r",
  "JNZL,(AR2)=BH,AR1++\r",
  "JNZL,(AR2)=RH,AR1++\r",
  "JNZL,(AR2)=RL,AR1++\r",
  "JNSL,AH=(AR1),AR2++\r",
  "JNSL,BH=(AR1),AR2++\r",
  "JNSL,JAH=(AR1),AR2++\r",
  "JNSL,JAL=(AR1),AR2++\r",
  "JNSL,AH=(AR2),AR1++\r",
  "JNSL,BH=(AR2),AR1++\r",
  "JNSL,JAH=(AR2),AR1++\r",
  "JNSL,JAL=(AR2),AR1++\r",
  "JNSL,(AR1)=AH,AR2++\r",
  "JNSL,(AR1)=BH,AR2++\r",
  "JNSL,(AR1)=RH,AR2++\r",
  "JNSL,(AR1)=RL,AR2++\r",
  "JNSL,(AR2)=AH,AR1++\r",
  "JNSL,(AR2)=BH,AR1++\r",
  "JNSL,(AR2)=RH,AR1++\r",
  "JNSL,(AR2)=RL,AR1++\r",
  "JNCH,AL=(AR1),AR2++\r",
  "JNCH,BL=(AR1),AR2++\r",
  "JNCH,JAH=(AR1),AR2++\r",
  "JNCH,JAL=(AR1),AR2++\r",
  "JNCH,AL=(AR2),AR1++\r",
  "JNCH,BL=(AR2),AR1++\r",
  "JNCH,JAH=(AR2),AR1++\r",
  "JNCH,JAL=(AR2),AR1++\r",
  "JNCH,(AR1)=AL,AR2++\r",
  "JNCH,(AR1)=BL,AR2++\r",
  "JNCH,(AR1)=RH,AR2++\r",
  "JNCH,(AR1)=RL,AR2++\r",
  "JNCH,(AR2)=AL,AR1++\r",
  "JNCH,(AR2)=BL,AR1++\r",
  "JNCH,(AR2)=RH,AR1++\r",
  "JNCH,(AR2)=RL,AR1++\r",
  "CIMF\r",
  "SOMF\r"
};

void setup() {
  pinMode(IMF, INPUT);
  pinMode(OMF, INPUT);
  pinMode(SIMF, OUTPUT);
  pinMode(COMF, OUTPUT);
  pinMode(STPSLC, OUTPUT);
  pinMode(STPCLK, OUTPUT);
  pinMode(HLT, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(DRD, OUTPUT);
  pinMode(DWR, OUTPUT);
  pinMode(IRD, OUTPUT);
  pinMode(IWR, OUTPUT);
  pinMode(SLC0, OUTPUT);
  pinMode(SLC1, OUTPUT);
  pinMode(SLC2, OUTPUT);
  pinMode(LATCH, OUTPUT);
  // Port A is treated as a whole byte
  DDRA = B11111111;
  // Writing default values to outputs
  digitalWrite(SIMF, LOW);    // Inactive
  digitalWrite(COMF, LOW);    // Inactive
  digitalWrite(STPSLC, HIGH); // This signal is active low, so inactive by default
  digitalWrite(STPCLK, LOW);
  digitalWrite(HLT, HIGH);    // Active, CPU should be halted by default
  digitalWrite(RST, HIGH);    // Active, CPU should be in reset by default, to release the instruction memory
  digitalWrite(DRD, HIGH);    // Inactive
  digitalWrite(DWR, HIGH);    // Inactive
  digitalWrite(IRD, HIGH);    // Inactive
  digitalWrite(IWR, HIGH);    // Inactive
  digitalWrite(SLC0, HIGH);   // Select unused output 7 by default
  digitalWrite(SLC1, HIGH);
  digitalWrite(SLC2, HIGH);
  digitalWrite(LATCH, LOW);   // Inactive
  // Initialize serial comms with terminal
  while(!Serial1) ; // Wait for serial line to connect
  Serial1.begin(19200); // Start up the serial link at given baud rate
  clearLine(); // Clear the edit line
  Serial1.println(F("=================================")); // Welcome greeting
  Serial1.println(F("Welcome to TalosES"));
  Serial1.println(F("Brought to you by The Byte Attic!"));
  Serial1.println(F("Copyright (c) 2023 by B. Kastrup"));
  Serial1.println(F("Works best in 80 columns"));
  Serial1.println(F("For help, type \"help\" or \"?\""));
  Serial1.println(F("================================="));
  // Initialize uSD card, if present
  if (!SD.begin(chipSelect)) Serial1.println("uSD card not present");
}

void loop() {
  byte inByte; // Byte coming in from serial port
  if (run) { // First check the OMF if we are in run mode, to see if the running program has completed
    if(digitalRead(OMF)) {
      digitalWrite(RST, HIGH); // Clear the CPU's reset state
      digitalWrite(HLT, HIGH); // Clear the CPU's halt state
      digitalWrite(COMF, HIGH); // Clear the outgoing mail flag
      delay(DELAY);
      digitalWrite(COMF, LOW);
      run = false; // Clear the global indicator of run state
      Serial1.println(F("Program execution completed"));
    }
  }
  if (Serial1.available() > 0) { // If some data has arrived, proceed
    while(Serial1.available() > 0) { // While there is data to be processed, proceed
      inByte = Serial1.read(); // Read a byte of data
      if ((inByte != KB_BS) &&
          (inByte != KB_ESC)) {
        if (pos <= 79) { // Limit line input to 80 columns (index 0 to 79), the last character being CR
          Serial1.write(inByte); // Echo the byte back
          editLine[pos] = inByte; // Add new character to edit line
          if (inByte == KB_CR) {
            if (editLine[0] != KB_CR) { // Only echo carriage return if edit line is not empty
              Serial1.println(); // Jump to next line for readability
            }
            enter(); // Process edit line if ENTER is pressed
          } else pos++; // Increment edit line index
        } else if (inByte == KB_CR) {
          Serial1.write(inByte); // Still echo the CR
          editLine[pos] = inByte; // Still append the CR to mark end of edit line
          Serial1.println(); // Jump to next line for readability
          enter(); // Process edit line now that ENTER has been pressed
        }
      } else {
        if (inByte == KB_BS) { // If user presses backspace...
          Serial1.write(inByte); // Update cursor position
          if (pos > 0) pos--; // Update edit line index accordingly
        }
      }
    }
  }
}

void enter() {
  String command;
  String mnemonic;
  String address;
  String bottomAddr;
  String topAddr;
  String status;
  String data;
  String fileName;
  unsigned int a; // A 16-bit address, used in different places below
  byte d; // An 8-bit data value, used in different places below
  unsigned int bottom; // Bottom instruction memory address for 'list'
  unsigned int top; // Top instruction memory address for 'list' and 'save'
  byte i;
  char outputLine[15];
  editText = String((char *)editLine); // Cast edit line to string format for further processing
  command = getNextWord(true);
  if (step) { // If in step mode, check if user just pressed ENTER
    if (editLine[0] == KB_CR) {
      if (digitalRead(STPCLK) == LOW) digitalWrite(STPCLK, HIGH); // If so, perform a clock transition step
      else digitalWrite(STPCLK, LOW);
    }
  }
  if (command == F("IPEEK")) {
    address = getNextWord(false);
    address.replace("$",""); // Removes the leading '$' denoting hexadecimal value
    unsigned int a = strtol(address.c_str(), NULL, 16);  // Converts to HEX number type
    sprintf(outputLine, "I$%04x: $%02x", a, ipeek(a));
    Serial1.println(outputLine);
  }
  else if (command == F("IPOKE")) {
    address = getNextWord(false);
    address.replace("$",""); // Removes the leading '$' denoting hexadecimal value
    unsigned int a = strtol(address.c_str(), NULL, 16); // Converts to HEX number type
    data = getNextWord(false);
    data.replace("$",""); // Removes the leading '$' denoting hexadecimal value
    d = byte(strtol(data.c_str(), NULL, 16)); // Converts to HEX number type
    ipoke(a, d);
    sprintf(outputLine, "I$%04x: $%02x", a, ipeek(a));
    Serial1.println(outputLine);
  }
  else if (command == F("DPEEK")) {
    address = getNextWord(false);
    address.replace("$",""); // Removes the leading '$' denoting hexadecimal value
    unsigned int a = strtol(address.c_str(), NULL, 16);  // Converts to HEX number type
    sprintf(outputLine, "D$%04x: $%02x", a, dpeek(a));
    Serial1.println(outputLine);
  }
  else if (command == F("DPOKE")) {
    address = getNextWord(false);
    address.replace("$",""); // Removes the leading '$' denoting hexadecimal value
    unsigned int a = strtol(address.c_str(), NULL, 16); // Converts to HEX number type
    data = getNextWord(false);
    data.replace("$",""); // Removes the leading '$' denoting hexadecimal value
    d = byte(strtol(data.c_str(), NULL, 16)); // Converts to HEX number type
    dpoke(a, d);
    sprintf(outputLine, "D$%04x: $%02x", a, dpeek(a));
    Serial1.println(outputLine);
  }
  else if (command == F("RESET")) {
    status = getNextWord(false);
    if (status == F("ON")) {
      digitalWrite(RST, HIGH);
      run = false; // Also reset run status when reseting CPU
    }
    else if (status == F("OFF")) digitalWrite(RST, LOW);
  }
  else if (command == F("HALT")) {
    status = getNextWord(false);
    if (status == F("ON")) digitalWrite(HLT, HIGH);
    else if (status == F("OFF")) digitalWrite(HLT, LOW);
  }
  else if (command == F("STEP")) {
    status = getNextWord(false);
    if (status == F("ON")) {
      step = true;
      digitalWrite(STPSLC, LOW);
    } else if (status == F("OFF")) {
      step = false;
      digitalWrite(STPSLC, HIGH);
    }
  }
  else if (command == F("SIMF")) {
    digitalWrite(SIMF, HIGH);
    delay(DELAY);
    digitalWrite(SIMF, LOW);
  }
  else if (command == F("COMF")) {
    digitalWrite(COMF, HIGH);
    delay(DELAY);
    digitalWrite(COMF, LOW);
  }
  else if (command == F("STATUS")) {
    Serial1.print(F("Reset is "));
    if (digitalRead(RST)) Serial1.println("on");
    else Serial1.println(F("off"));
    Serial1.print(F("Halt is "));
    if (digitalRead(HLT)) Serial1.println("on");
    else Serial1.println(F("off"));
    Serial1.print(F("Step is "));
    if (!digitalRead(STPSLC)) Serial1.println(F("on")); // STPSLC is active low
    else Serial1.println(F("off"));
    Serial1.print(F("OMF is "));
    if(digitalRead(OMF)) Serial1.println(F("set"));
    else Serial1.println(F("clear"));
  }
  else if ((command == F("HELP")) || (command == F("?"))) {
    Serial1.println(F("--------------------------------------------------------------------------------"));
    Serial1.println(F("\"help\" or \"?\": this help screen"));
    Serial1.println(F("\"status\": displays reset, halt, step and OMF status"));
    Serial1.println(F("\"reset on\" and \"reset off\": turn CPU reset mode on and off"));
    Serial1.println(F("\"halt on\" and \"halt off\": turn CPU halt mode on and off"));
    Serial1.println(F("\"step on\" and \"step off\": turn CPU step mode on and off"));
    Serial1.println(F("\"run\": runs code in instruction memory, starting from address I$0000"));
    Serial1.println(F("\"ipeek $aaaa\": reads a byte from instruction memory at address I$aaaa"));
    Serial1.println(F("\"ipoke $aaaa $dd\": writes byte $dd to instruction memory at address I$aaaa"));
    Serial1.println(F("\"dpeek $aaaa\": reads a byte from data memory at address D$aaaa"));
    Serial1.println(F("\"dpoke $aaaa $dd\": writes byte $dd to data memory at address D$aaaa"));
    Serial1.println(F("\"i$aaaa MNEMONIC\": writes instruction with given mnemonic at address I$aaaa,"));
    Serial1.println(F("                   where the mnemonic should contain no spaces"));
    Serial1.println(F("                   (this command is not verbose, thus suitable for"));
    Serial1.println(F("                    pasting instruction lists on the terminal"));
    Serial1.println(F("\"ilist $aaaa $bbbb\": lists contents of instruction memory from"));
    Serial1.println(F("                     address I$aaaa to address I$bbbb, inclusive"));
    Serial1.println(F("\"d$aaaa $dd\": writes byte $dd to data memory at address D$aaaa"));
    Serial1.println(F("              (this command is not verbose, thus suitable for"));
    Serial1.println(F("               pasting data lists on the terminal)"));
    Serial1.println(F("\"dlist $aaaa $bbbb\": lists contents of data memory from"));
    Serial1.println(F("                     address D$aaaa to address D$bbbb, inclusive"));
    Serial1.println(F("\"dclear $aaaa $bbbb\": pokes $00 into data memory between addresses"));
    Serial1.println(F("                      D$aaaa and D$bbbb, inclusive"));
    Serial1.println(F("\"dir\": lists the files in the uSD card"));
    Serial1.println(F("\"load FILE\": loads program file with name FILE from uSD card"));
    Serial1.println(F("\"save FILE $aaaa\": saves contents of instruction memory up to address $aaaa"));
    Serial1.println(F("                   to the uSD card in a program file named FILE"));
    Serial1.println(F("\"del FILE\": deletes program file named FILE from uSD card"));
    Serial1.println(F("ATTENTION: all data and address values should be in hexadecimal"));
    Serial1.println(F("--------------------------------------------------------------------------------"));
  }
  else if (command == F("DIR")) {
    dir();
  }
  else if (command == F("DEL")) {
    fileName = getNextWord(false);
    del(fileName);
  }
  else if (command == F("SAVE")) {
    fileName = getNextWord(false);
    topAddr = getNextWord(false);
    topAddr.replace("$",""); // Removes the leading '$' denoting hexadecimal value
    save(fileName, topAddr);
  }
  else if (command == F("LOAD")) {
    fileName = getNextWord(false);
    load(fileName);
  }
  else if (command == F("ILIST")) {
    bottomAddr = getNextWord(false);
    if (bottomAddr == "\r") Serial1.println("Missing address");
    else {
      bottomAddr.replace("$",""); // Removes the leading '$' denoting hexadecimal value
      bottom = strtol(bottomAddr.c_str(), NULL, 16); // Converts address to HEX number
      topAddr = getNextWord(false);
      if (topAddr == "\r") top = bottom;
      else {
        topAddr.replace("$",""); // Removes the leading '$' denoting hexadecimal value
        top = strtol(topAddr.c_str(), NULL, 16); // Converts address to HEX number
      }
      ilist(bottom, top);
    }
  }
  else if (command == F("DLIST")) {
    bottomAddr = getNextWord(false);
    if (bottomAddr == "\r") Serial1.println("Missing address");
    else {
      bottomAddr.replace("$",""); // Removes the leading '$' denoting hexadecimal value
      bottom = strtol(bottomAddr.c_str(), NULL, 16); // Converts address to HEX number
      topAddr = getNextWord(false);
      if (topAddr == "\r") top = bottom;
      else {
        topAddr.replace("$",""); // Removes the leading '$' denoting hexadecimal value
        top = strtol(topAddr.c_str(), NULL, 16); // Converts address to HEX number
      }
      dlist(bottom, top);
    }
  }
  else if (command == F("DCLEAR")) {
    bottomAddr = getNextWord(false);
    if (bottomAddr == "\r") Serial1.println("Missing address");
    else {
      bottomAddr.replace("$",""); // Removes the leading '$' denoting hexadecimal value
      bottom = strtol(bottomAddr.c_str(), NULL, 16); // Converts address to HEX number
      topAddr = getNextWord(false);
      if (topAddr == "\r") top = bottom;
      else {
        topAddr.replace("$",""); // Removes the leading '$' denoting hexadecimal value
        top = strtol(topAddr.c_str(), NULL, 16); // Converts address to HEX number
      }
      dclear(bottom, top);
    }
  }
  else if (command == F("RUN")) { // Runs code in instruction memory
    digitalWrite(RST, LOW); // Take the system out of reset state
    digitalWrite(HLT, LOW); // Take the system out of halt state
    run = true; // Set global run indicator
  }
  else if (command.indexOf("D$") == 0) { // The user is entering a byte into data memory...
    // This is like a DPOKE, but not verbose, so enables pasting on the terminal window
    command.replace("D$",""); // Removes the leading 'D$' denoting hexadecimal value
    a = strtol(command.c_str(), NULL, 16); // Converts to HEX number type
    data = getNextWord(false);
    data.replace("$",""); // Removes the leading '$' denoting hexadecimal value
    d = byte(strtol(data.c_str(), NULL, 16)); // Converts to HEX number type
    dpoke(a, d);
  }
  else if (command.indexOf("I$") == 0) { // The user is entering assembly instructions...
    // This is like a IPOKE, but not verbose, so enables pasting on the terminal window
    command.replace("I$",""); // Removes the leading 'I$' denoting hexadecimal value
    a = strtol(command.c_str(), NULL, 16); // Converts address to HEX number
    mnemonic = getNextWord(false);
    if (mnemonic.indexOf("NOP") == 0) {
      ipoke(a, 0); // Poke instruction opcode to instruction memory
    }
    else if (mnemonic.indexOf("AH=$") == 0) { // Substring "AH=$" is present in the mnemonic
      mnemonic.replace("AH=$",""); // Leave only the immediate value
      i = strtol(mnemonic.c_str(), NULL, 16);  // Converts immediate value to HEX number
      ipoke(a, 1); // Poke instruction opcode to instruction memory
      ipoke(a+1, i); // Now poke immediate value to instruction memory
    }
    else if (mnemonic.indexOf("AL=$") == 0) { // Substring "AL=$" is present in the mnemonic
      mnemonic.replace("AL=$",""); // Leave only the immediate value
      i = strtol(mnemonic.c_str(), NULL, 16);  // Converts immediate value to HEX number
      ipoke(a, 2); // Poke instruction opcode to instruction memory
      ipoke(a+1, i); // Now poke immediate value to instruction memory
    }
    else if (mnemonic.indexOf("BH=$") == 0) { // Substring "BH=$" is present in the mnemonic
      mnemonic.replace("BH=$",""); // Leave only the immediate value
      i = strtol(mnemonic.c_str(), NULL, 16);  // Converts immediate value to HEX number
      ipoke(a, 3); // Poke instruction opcode to instruction memory
      ipoke(a+1, i); // Now poke immediate value to instruction memory
    }
    else if (mnemonic.indexOf("BL=$") == 0) { // Substring "BL=$" is present in the mnemonic
      mnemonic.replace("BL=$",""); // Leave only the immediate value
      i = strtol(mnemonic.c_str(), NULL, 16);  // Converts immediate value to HEX number
      ipoke(a, 4); // Poke instruction opcode to instruction memory
      ipoke(a+1, i); // Now poke immediate value to instruction memory
    }
    else if (mnemonic.indexOf("JAH=$") == 0) { // Substring "JAH=$" is present in the mnemonic
      mnemonic.replace("JAH=$",""); // Leave only the immediate value
      i = strtol(mnemonic.c_str(), NULL, 16);  // Converts immediate value to HEX number
      ipoke(a, 5); // Poke instruction opcode to instruction memory
      ipoke(a+1, i); // Now poke immediate value to instruction memory
    }
    else if (mnemonic.indexOf("JAL=$") == 0) { // Substring "JAL=$" is present in the mnemonic
      mnemonic.replace("JAL=$",""); // Leave only the immediate value
      i = strtol(mnemonic.c_str(), NULL, 16);  // Converts immediate value to HEX number
      ipoke(a, 6); // Poke instruction opcode to instruction memory
      ipoke(a+1, i); // Now poke immediate value to instruction memory
    }
    else {
      byte opcode = findOpcode(mnemonic + "\r");
      if (opcode == 0) Serial1.println("Mnemonic not found: " + mnemonic);
      else {
        ipoke(a, opcode); // Poke instruction opcode to instruction memory
      }
    }
  } else if (command != "\r") { // Don't print this if ENTER is being pressed for stepping
      Serial1.print("Unrecognized command: ");
      Serial1.println(command);
  }
  clearLine(); // Avoids leaving trash behind in the editLine
  pos = 0; // Reset cursor position in edit line
}

void clearLine() { // Fills the editLine with carriage returns
  for (byte i = 0; i < 80; i++) editLine[i] = KB_CR;
}

byte findOpcode (String mnemonic) {
  for (unsigned int i=7; i<256; i++) {
    if (mnemonic.equals(opcodeTable[i])) return (i);
  }
  return (0); // Returns zero if opcode not found
}

void ilist(unsigned int bottom, unsigned int top) { // Lists contents of instruction memory
  byte opcode;
  bool isImm = false; // Checks whether the value in instruction memory is an immediate (or an opcode)
  String mnemonic;
  char outputLine[15];
  if (bottom > top) Serial1.println("Top address must be higher than bottom");
  else {
    for (unsigned int a = bottom; a <= top; a++) {
      byte opcode = ipeek(a);
      sprintf(outputLine, "I$%04x: $%02x", a, opcode);
      Serial1.print(outputLine);
      if (!isImm) {
        mnemonic = opcodeTable[opcode];
        Serial1.println(" //" + mnemonic);
        if ((opcode > 0) && (opcode < 7)) isImm=true; // After a load immediate, there must come an immediate value...
      } else {
        Serial1.println(" //Immediate");
        isImm = false; // After an immediate value there must be an opcode...
      }
    }
  }
}

void dlist(unsigned int bottom, unsigned int top) { // Lists contents of data memory
  byte data;
  char outputLine[15];
  if (bottom > top) Serial1.println("Top address must be higher than bottom");
  else {
    for (unsigned int a = bottom; a <= top; a++) {
      data = dpeek(a);
      sprintf(outputLine, "D$%04x: $%02x", a, data);
      Serial1.println(outputLine);
      delay(10);
    }
  }
}

void dclear(unsigned int bottom, unsigned int top) { // Clears contents of data memory
  if (bottom > top) Serial1.println("Top address must be higher than bottom");
  else for (unsigned int a = bottom; a <= top; a++) dpoke(a, 0);
}

byte ipeek(unsigned int address) {
  PORTA = lowByte(address); // Puts LSB of address in Port A
  digitalWrite(SLC1, LOW); // Now select 5 (register LIMAL)
  pulseLatch(); // Pulse the latch signal
  PORTA = highByte(address); // Puts MSB of address in Port A
  digitalWrite(SLC0, LOW); // Now select 4 (register LIMAH)
  pulseLatch(); // Pulse the latch signal
  DDRA = B00000000; // Set port A to input
  digitalWrite(IRD, LOW); // Activate read enable of instruction memory
  delay(DELAY); // Wait for transaction
  byte data = PINA; // Read in the value from instruction memory
  digitalWrite(IRD, HIGH); // Deactive read enable of instruction memory
  digitalWrite(SLC0, HIGH); // Go back to default select value 7 (unused)
  digitalWrite(SLC1, HIGH);
  DDRA = B11111111; // Set port A back to outputs, as per default
  return data;
}

void ipoke(unsigned int address, byte data) {
  PORTA = lowByte(address); // Puts LSB of address in Port A
  digitalWrite(SLC1, LOW); // Now select 5 (register LIMAL)
  pulseLatch(); // Pulse the latch signal
  PORTA = highByte(address); // Puts MSB of address in Port A
  digitalWrite(SLC0, LOW); // Now select 4 (register LIMAH)
  pulseLatch(); // Pulse the latch signal
  PORTA = data; // Puts data in Port A
  digitalWrite(SLC0, HIGH); // Now select 3 (register LIMD)
  digitalWrite(SLC1, HIGH);
  digitalWrite(SLC2, LOW);
  pulseLatch(); // Pulse the latch signal
  digitalWrite(IWR, LOW); // Activate write enable of instruction memory
  delay(DELAY); // Wait for transaction
  digitalWrite(IWR, HIGH); // Deactive write enable of instruction memory
  digitalWrite(SLC2, HIGH); // Go back to default select value 7 (unused)
}

byte dpeek(unsigned int address) {
  PORTA = lowByte(address); // Puts LSB of address in Port A
  digitalWrite(SLC0, LOW); // Now select 2 (register LDMAL)
  digitalWrite(SLC2, LOW);
  pulseLatch(); // Pulse the latch signal
  PORTA = highByte(address); // Puts MSB of address in Port A
  digitalWrite(SLC0, HIGH); // Now select 1 (register LDMAH)
  digitalWrite(SLC1, LOW);
  pulseLatch(); // Pulse the latch signal
  DDRA = B00000000; // Set port A to input
  digitalWrite(DRD, LOW); // Activate read enable of instruction memory
  delay(DELAY); // Wait for transaction
  byte data = PINA; // Read in the value from instruction memory
  digitalWrite(DRD, HIGH); // Deactive read enable of instruction memory
  digitalWrite(SLC0, HIGH); // Go back to default select value 7 (unused)
  digitalWrite(SLC1, HIGH);
  digitalWrite(SLC2, HIGH);
  DDRA = B11111111; // Set port A back to outputs, as per default
  return data;
}

void dpoke(unsigned int address, byte data) {
  PORTA = lowByte(address); // Puts LSB of address in Port A
  digitalWrite(SLC0, LOW); // Now select 2 (register LDMAL)
  digitalWrite(SLC2, LOW);
  pulseLatch(); // Pulse the latch signal
  PORTA = highByte(address); // Puts MSB of address in Port A
  digitalWrite(SLC0, HIGH); // Now select 1 (register LDMAH)
  digitalWrite(SLC1, LOW);
  pulseLatch(); // Pulse the latch signal
  PORTA = data; // Puts data in Port A
  digitalWrite(SLC0, LOW); // Now select 0 (register LDMD)
  pulseLatch(); // Pulse the latch signal
  digitalWrite(DWR, LOW); // Activate write enable of instruction memory
  delay(DELAY); // Wait for transaction
  digitalWrite(DWR, HIGH); // Deactive write enable of instruction memory
  digitalWrite(SLC0, HIGH); // Go back to default select value 7 (unused)
  digitalWrite(SLC1, HIGH);
  digitalWrite(SLC2, HIGH);
}

void pulseLatch() {
  digitalWrite(LATCH, HIGH);
  delay(DELAY);
  digitalWrite(LATCH, LOW);
}

String getNextWord(bool fromTheBeginning) { // A very simple parser that returns the next word in the edit line
  static byte initialPosition; // Start parsing from this point in the edit line
  byte i, j, k; // General-purpose indices
  if (fromTheBeginning) initialPosition = 0; // Starting from the beginning of the edit line
  i = initialPosition; // Otherwise, continuing on from where we left off in previous call
  while (editText[i] == KB_SPACE) i++; // Ignore leading spaces
  j = i + 1; // Now start indexing the next word proper
  // Find the end of the word, marked either by a space or the end of line
  while ((editText[j] != KB_SPACE) && (editText[j] != KB_CR)) j++;
  char buffer[j - i + 1]; // Create a buffer (the +1 is to make space for null-termination)
  for (k = i; k < j; k++) buffer[k - i] = editText[k]; // Transfer the word to the buffer
  buffer[j - i] = 0; // Null-termination
  initialPosition = j; // Next time round, start from here, unless...
  String nextWord = String(buffer); // Cast to String
  nextWord.toUpperCase();
  return (nextWord); // Return capitalized the contents of the buffer
}

void dir() { // Lists the files in the root directory of uSD card, if available
  File root;            // Root directory of uSD card
  File entry;           // A file on the uSD card
  root = SD.open("/");  // Go to the root directory of uSD card
  char outputLine[15];  // Nicely formats things...
  while (true) {
    entry = root.openNextFile();  // Open next file
    if (!entry) {                 // No more files on the uSD card
      root.close();               // Close root directory
      break;                      // Get out of this otherwise infinite while() loop
    }
    sprintf(outputLine, "%12s  ", entry.name());
    Serial1.print(outputLine);
    Serial1.println(String(entry.size(), DEC));
    entry.close();                // Close file as soon as it is no longer needed
  }
  Serial1.println(" End of directory listing");
}

void del(String filename) { // Deletes a file from the uSD card
if (filename == "\r") Serial1.println("Missing file name");
	else {
    if (!SD.exists(filename)) Serial1.println("File does not exist");
    else SD.remove(filename);
    Serial1.println("File deleted");
  }
}

void save(String filename, String topAddr) { // Saves program in instruction memory to a file on uSD card
  unsigned int i; // Memory address counter
  byte data;      // Data from memory
  File dataFile;  // File to be created and written to
	if (filename == "\r") Serial1.println("Missing file name");
	else {
		if (SD.exists(filename)) Serial1.println("File already exists");
		else {
			dataFile = SD.open(filename, FILE_WRITE); // Try to create the file
			if (!dataFile) Serial1.println("Cannot create file");
			else {
        if (topAddr == "\r") Serial1.println("Missing top address");
	      else {
          unsigned int a = strtol(topAddr.c_str(), NULL, 16); // Converts top address to HEX number
				  for(i = 0; i <= a; i++) {
					  data = ipeek(i);
					  dataFile.write(data);
				  }
				  dataFile.close();
          Serial1.println("File saved");
        }
			}
		}
	}
}

void load(String filename) { // Load program (binary format) from uSD card into instruction memory
  File dataFile;
  unsigned int addr = 0; // Instruction memory address
  if (filename == "\r") Serial1.println("Missing file name");
  else {
    if (!SD.exists(filename)) Serial1.println("File does not exist"); 
    else {
      dataFile = SD.open(filename); // Open the binary file
      if (!dataFile) Serial1.println("Cannot open file");
      else {
        while (dataFile.available()) {    // While there is data to be read...
         	ipoke(addr++, dataFile.read()); // Read data from file and store it in memory
         	if (addr == 0) {                // Break if address wraps around to the start of memory
           	dataFile.close();
            Serial1.println("Program is too large");
           	break;
         	}
        }
        dataFile.close();
        Serial1.println("File loaded");
      }
    }
  }
}
