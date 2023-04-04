#include <SPI.h>  // SPI library built into Arduino IDE
#include <SD.h>   // SD-card library built into Arduino IDE

// Define ATmega1284p pinout using Arduino pin codes
#define IMF     0
#define OMF     1
#define SIMF    2
#define COMF    3
#define CS      4
#define MOSI    5
#define MISO    6
#define SCK     7
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

volatile bool step = true;    // Global step mode indicator
const int chipSelect = CS;    // Required by SPI library
volatile byte editLine[80];   // String where user types in commands
volatile byte pos = 0;        // Position in edit line currently occupied by cursor
void(* resetFunc) (void) = 0; // Software reset fuction at address 0

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
  digitalWrite(STPSLC, LOW);  // Active
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
}

void loop() {
  byte inByte; // Byte coming in from serial port
  if (Serial1.available() > 0) { // If some data has arrived, proceed
    while(Serial1.available() > 0) { // While there is data to be processed, proceed
      inByte = Serial1.read(); // Read a byte of data
      if ((inByte != KB_BS) &&
          (inByte != KB_ESC)) {
        if (pos <= 79) { // Limit line input to 80 columns (index 0 to 79), the last character being CR
          Serial1.write(inByte); // Echo the byte back
          editLine[pos] = inByte; // Add new character to edit line
          if (inByte == KB_CR) {
            Serial1.println(); // Jump to next line for readability
            pos = 0; // Reset position in edit line
            enter(); // Process edit line if ENTER is pressed
          } else pos++; // Increment edit line index
        } else if (inByte == KB_CR) {
          Serial1.write(inByte); // Still echo the CR
          editLine[pos] = inByte; // Still append the CR to mark end of edit line
          Serial1.println(); // Jump to next line for readability
          pos = 0; // Reset position in edit line
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
  String command; // First word in the edit line. '$xxxx' is a command, specifying that what follows is a mnemonic.
  String mnemonic; // Possible second word in the edit line
  String address; // Possible second word in the edit line
  String data; // Possible third word in the edit line
  String status; // Possible second word in the edit line
  command = getNextWord(true);
  if (step) { // If in step mode, check if user just pressed ENTER
    if (editLine[0] == KB_CR) {
      if (digitalRead(STPCLK) == LOW) digitalWrite(STPCLK, HIGH); // If so, perform a clock transition step
      else digitalWrite(STPCLK, LOW);
    }
  }
  if (command == F("ipeek")) {
    address = getNextWord(false);
    address.remove(0,1); // Removes the leading '$' denoting hexadecimal value
    unsigned int a = strtol(address.c_str(), NULL, 16);  // Converts to HEX number type
    byte d = ipeek(a); // Peek into instruction memory
    Serial1.print("$");
    Serial1.println(d, HEX); // Print the result
  }
  else if (command == F("ipoke")) {
    address = getNextWord(false);
    address.remove(0,1); // Removes the leading '$' denoting hexadecimal value
    unsigned int a = strtol(address.c_str(), NULL, 16); // Converts to HEX number type
    data = getNextWord(false);
    data.remove(0,1); // Removes the leading '$' denoting hexadecimal value
    byte d = byte(strtol(data.c_str(), NULL, 16)); // Converts to HEX number type
    ipoke(a, d);
  }
  else if (command == F("dpeek")) {
    address = getNextWord(false);
    address.remove(0,1); // Removes the leading '$' denoting hexadecimal value
    unsigned int a = strtol(address.c_str(), NULL, 16);  // Converts to HEX number type
    byte d = dpeek(a); // Peek into instruction memory
    Serial1.print("$");
    Serial1.println(d, HEX); // Print the result
  }
  else if (command == F("dpoke")) {
    address = getNextWord(false);
    address.remove(0,1); // Removes the leading '$' denoting hexadecimal value
    unsigned int a = strtol(address.c_str(), NULL, 16); // Converts to HEX number type
    data = getNextWord(false);
    data.remove(0,1); // Removes the leading '$' denoting hexadecimal value
    byte d = byte(strtol(data.c_str(), NULL, 16)); // Converts to HEX number type
    dpoke(a, d);
  }
  else if (command == F("reset")) {
    status = getNextWord(false);
    if (status == F("on")) digitalWrite(RST, HIGH);
    else if (status == F("off")) digitalWrite(RST, LOW);
  }
  else if (command == F("halt")) {
    status = getNextWord(false);
    if (status == F("on")) digitalWrite(HLT, HIGH);
    else if (status == F("off")) digitalWrite(HLT, LOW);
  }
  else if (command == F("step")) {
    status = getNextWord(false);
    if (status == F("on")) {
      step = true;
      digitalWrite(STPSLC, LOW);
    } else if (status == F("off")) {
      step = false;
      digitalWrite(STPSLC, HIGH);
    }
  }
  else if (command == F("simf")) {
    digitalWrite(SIMF, HIGH);
    delay(DELAY);
    digitalWrite(SIMF, LOW);
  }
  else if (command == F("comf")) {
    digitalWrite(COMF, HIGH);
    delay(DELAY);
    digitalWrite(COMF, LOW);
  }
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

String getNextWord(bool fromTheBeginning) {
  // A very simple parser that returns the next word in the edit line
  static byte initialPosition; // Start parsing from this point in the edit line
  byte i, j, k; // General-purpose indices
  String editText = String((char *)editLine); // Cast edit line to string format
  if (fromTheBeginning) initialPosition = 0; // Starting from the beginning of the edit line
  i = initialPosition; // Otherwise, continuing on from where we left off in previous call
  while (editText[i] == KB_SPACE) i++; // Ignore leading spaces or commas
  j = i + 1; // Now start indexing the next word proper
  // Find the end of the word, marked either by a space, a comma or the end of line
  while ((editText[j] != KB_SPACE) && (editText[j] != KB_COMMA) && (editText[j] != KB_CR)) j++;
  char nextWord[j - i + 1]; // Create a buffer (the +1 is to make space for null-termination)
  for (k = i; k < j; k++) nextWord[k - i] = editText[k]; // Transfer the word to the buffer
  nextWord[j - i] = 0; // Null-termination
  initialPosition = j; // Next time round, start from here, unless...
  return (nextWord); // Return the contents of the buffer
}
