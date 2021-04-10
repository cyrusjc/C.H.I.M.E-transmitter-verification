#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <SPI.h>

// DEFINE YOUR PINS FOR YOUR MICROCONTROLLER DOWN BELOW
#define SPI_CS 52
#define SPI_CK 76
#define SPI_MOSI 75

// Please consult datasheet for reg values:
// https://www.maximintegrated.com/en/products/comms/wireless-rf/MAX41460.html#online-ds

#define CFG1 0b10010001     // XOCLKOUT_DELAY[7:6], XOCLKDIV[5:4], FSKSHAPE[2], SYNC[1], MODMODE[0]: 0x1 (FSK)
#define CFG2 0b10000001     // CLKOUT_DELAY [7:6]
#define CFG3 0b00000000     // RESERVED [7:0]
#define CFG4 0b00000010     // PWDN_MODE[1:0]// SHUTDOWN MODE
#define CFG5 0b00000001     // TSTEP[5:0] GFSK SHAPING // IMPORTANT

#define SHDN 0b00000100     // PA_BOOST[0]
#define PA1 0b10000000     // PAPWR[2:0] 0x0 -0x7 (MAX)
#define PA2 0b00000000      // CAPACITANCE CONTROL PACAP [4:0] 0 - 5425 fF
#define PLL1 0b01100000     // CPLIN[7:6], FRACMODE[5} LODIV[2:1], LOMODE[0]
#define PLL2 0b00000000     // CPVAL[1:0]

#define CFG6 0b0000000     // SPI_TXEN1[1], FOURWIRE1[0]
#define PLL3 0x13         //FREQ[23:16]
#define PLL4 0xB0         //FREQ[15:8] 
#define PLL5 0x00         //FREQ[7:0]
#define PLL6 0b01111111     // DElTAF[6:0]

#define PLL7 0b00001000     // DElTAF_SHAPE[6:0]
#define CFG7 0b00000011     // SPI_TXEN2[1], FOURWIRE[0]
#define CFG8 0b00000000
#define CFG9 0b00000000


#define CHIRP_DELAY 500 // Delay before switching frequency - 500ms
#define WAIT_TIME 1     // 1us
#define START_DELAY 10  // 10us FOR < 10 MHZ HOPS ... 20us FOR >20MHZ HOPS
#define DELAY 1
#define DATA_LENGTH 8   // MAX41460 REGISTER SIZES 6

// ARDUINO HARDWARE SPI SETTINGS
#define SPI_SPEED 20000000  // 20MHz
#define DATA_ORDER MSBFIRST 
#define SPI_MODE SPI_MODE0  

//function prototypes to implement depending on your mcu:
void setPinHigh(int pin);
void setPinLow(int pin);

//function prototypes 
int spi_exchange(int data);
void spiWriteReg(const unsigned char regAddr, const unsigned char regData, bool cont);
void setFrequency(int freq);
void startTX();
void setChirp();
void spi1WriteRegByte(const unsigned char regData, bool cont)

void setup() {
    delay(DELAY);
    Serial.begin(9600);
    Serial.print("Starting. \n");


    SPI.begin(SPI_CS);
    SPI.setClockDivider(SPI_CS,255);
    SPI.setDataMode(SPI_CS,SPI_MODE0);
    SPI.setBitOrder(SPI_CS,MSBFIRST);
// ======================================================
// FIRST WE WANT TO BURST WRITE 17 REGS INITIAL START UP DEFINED
// BY DATA SHEET.
// ======================================================   
    SPI.transfer(SPI_CS, 0, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x90, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x81, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x3, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x00, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x00, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x04, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x80, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x80, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x60, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x00, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x00, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0xC4, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0xDE, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x98, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x28, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x04, SPI_CONTINUE);
    SPI.transfer(SPI_CS, 0x01);
// ============================================
//  CONFIGURING REGS AS DEFInEd ABOVE
// ============================================
    SPI.transfer(SPI_CS, 0, SPI_CONTINUE);
    SPI.transfer(SPI_CS, CFG1, SPI_CONTINUE);
    SPI.transfer(SPI_CS, CFG2, SPI_CONTINUE);
    SPI.transfer(SPI_CS, CFG3, SPI_CONTINUE);
    SPI.transfer(SPI_CS, CFG4, SPI_CONTINUE);
    SPI.transfer(SPI_CS, CFG5, SPI_CONTINUE);
    SPI.transfer(SPI_CS, SHDN, SPI_CONTINUE);
    SPI.transfer(SPI_CS, PA1, SPI_CONTINUE);
    SPI.transfer(SPI_CS, PA2, SPI_CONTINUE);
    SPI.transfer(SPI_CS, PLL1, SPI_CONTINUE);
    SPI.transfer(SPI_CS, PLL2, SPI_CONTINUE);
    SPI.transfer(SPI_CS, CFG6, SPI_CONTINUE);
    SPI.transfer(SPI_CS, PLL3, SPI_CONTINUE);
    SPI.transfer(SPI_CS, PLL4, SPI_CONTINUE);
    SPI.transfer(SPI_CS, PLL5, SPI_CONTINUE);
    SPI.transfer(SPI_CS, PLL6, SPI_CONTINUE);
    SPI.transfer(SPI_CS, PLL7, SPI_CONTINUE);
    SPI.transfer(SPI_CS, CFG7);
    
   SPI.endTransaction();
   SPI.end();
   SPI.end(SPI_CS);

   // Configuring pin to outputs
   pinMode(75, OUTPUT);
   pinMode(76, OUTPUT);
   pinMode(SPI_CS, OUTPUT);
   setPinLow(75);
   setPinHigh(SPI_CS,HIGH);
   setPinLow(76);
   
   setFrequency(500);
//   setFrequency(301);
//   delay(2000);
   
//
}

// sending 1010 on repeat
void loop() {
  setPinHigh(SPI_MOSI);
  delayMicroseconds(500);
  setPinLow(SPI_MOSI);
  delayMicroseconds(500);
}

int spi_exchange(int data){
  int response = SPI.transfer(SPI_CS, data, SPI_CONTINUE);
  Serial.println("Master send command:" + String(data) + " Slave reply:" + String(response));
  delay(DELAY);
  return response;
}

//---------------------------------------------------------------
//   FUNCTION FOR BIT-BANGING SPI... WRITES TWO BYTE THEN SETS CS 
//   Param: regAddr - address of desired write
//          regData - data to write to regAddr
//          cont    - 1 sets cs low 0 sets cs high   
//---------------------------------------------------------------
void spiWriteReg(const unsigned char regAddr, const unsigned char regData, bool cont)
{
  unsigned char SPICount;                                       // Counter used to clock out the data
  unsigned char SPIData;                                        // Define a data structure for the SPI data

  //digitalWrite(SPI_CS, HIGH);                                   // Make sure we start with active-low CS high
  digitalWrite(SPI_CK, LOW);                                    // and CK low

  SPIData = regAddr;                                            // Preload the data to be sent with Address
  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(WAIT_TIME);// Set active-low CS low to start the SPI cycle
  // Although SPIData could be implemented as an "int",
  // resulting in one
  // loop, the routines run faster when two loops
  // are implemented with
  // SPIData implemented as two "char"s.

  for (SPICount = 0; SPICount < 8; SPICount++)                  // Prepare to clock out the Address byte
  {
    if ((SPIData & 0x80) >> 7)                                        // Check for a 1
      setPinHigh(SPI_MOSI);                             // and set the MOSI line appropriately
    else
      setPinLow(SPI_MOSI);
    delayMicroseconds(WAIT_TIME);
    setPinHigh(SPI_CK);
    delayMicroseconds(WAIT_TIME);// Toggle the clock line
    setPinLow(SPI_CK);
    //DEBUGGING
   // Serial.print((SPIData & 0x80) >> 7);
    SPIData <<= 1;                                              // Rotate to get the next bit
  }                                                             // and loop back to send the next bit

  // Repeat for the Data byte
  SPIData = regData;                                            // Preload the data to be sent with Data
  for (SPICount = 0; SPICount < 8; SPICount++)
  {
    if ((SPIData & 0x80) >> 7)
      digitalWrite(SPI_MOSI, HIGH);
    else
      digitalWrite(SPI_MOSI, LOW);
    delayMicroseconds(WAIT_TIME);
    digitalWrite(SPI_CK, HIGH);
    delayMicroseconds(WAIT_TIME);
    digitalWrite(SPI_CK, LOW);

    SPIData <<= 1;
  }
  if(!cont){
    digitalWrite(SPI_CS, HIGH);
  }
}
//---------------------------------------------------------------
//   FUNCTION FOR BIT-BANGING SPI... WRITES TWO BYTE THEN SETS CS 
//   Param: regData - byte to write with SPI
//          cont    - 1 sets cs low 0 sets cs high   
//---------------------------------------------------------------
void spi1WriteRegByte(const unsigned char regData, bool cont)
{
  unsigned char SPICount;                                       // Counter used to clock out the data
  unsigned char SPIData;                                        // Define a data structure for the SPI data

  setPinLow(SPI_CK);                                    // and CK low

  SPIData = regData;                                            // Preload the data to be sent with Address
  setPinLow(SPI_CS);
  delayMicroseconds(WAIT_TIME);
  
  // Set active-low CS low to start the SPI cycle
  // Although SPIData could be implemented as an "int",
  // resulting in one
  // loop, the routines run faster when two loops
  // are implemented with
  // SPIData implemented as two "char"s.

  for (SPICount = 0; SPICount < 8; SPICount++)                  // Prepare to clock out the Address byte
  {
    if ((SPIData & 0x80) >> 7)                                        // Check for a 1
      setPinHigh(SPI_MOSI);                             // and set the MOSI line appropriately
    else
      setPinLow(SPI_MOSI);
    delayMicroseconds(WAIT_TIME);
    setPinHigh(SPI_CK);
    delayMicroseconds(WAIT_TIME);// Toggle the clock line
    setPinLow(SPI_CK);
    //DEBUGGING
   // Serial.print((SPIData & 0x80) >> 7);
    SPIData <<= 1;                                              // Rotate to get the next bit
  }                                                             // and loop back to send the next bit
  
  if(!cont){
    digitalWrite(SPI_CS, HIGH);
  }
}

// ---------------------------------------------------------
// implement your pin_out HIGH function here
//-----------------------------------------------------------
void setPinHigh(int pin){
  digitalWrite(pin,HIGH);
  return;
}

// ---------------------------------------------------------
// implement your pin_out LOW function here
//-----------------------------------------------------------
void setPinLow(int pin){
  digitalWrite(pin,LOW);
}

// ---------------------------------------------------------
// This function takes frequency desired and writes to PLL3,PLL4 PLL5,
// to write desired frequency
// Param: freq - desired frequency
// Return: void
//-----------------------------------------------------------
void setFrequency(int freq){  
    
  unsigned int freqBitDec = (freq << 12);
  Serial.println(freqBitDec,HEX);
  unsigned char buff1 = (freqBitDec & 0x0000FF);
  unsigned char buff2 = (freqBitDec & 0x00FF00) >> 8 ;
  unsigned char buff3 = (freqBitDec & 0xFF0000 ) >> 16;

  Serial.print(buff3,HEX);
  Serial.print(buff2,HEX);
  Serial.print(buff1,HEX);

  setPinHigh(SPI_CS);
  spiWriteReg(0x0A,0x00, 1); 
  spiWriteReg(buff3,buff2, 1);
  spiWriteReg(buff1,PLL6, 1);
  spiWriteReg(PLL7,0x3, 1);
  delayMicroseconds(START_DELAY);
}

//======================
//==SETS TXEN2 AS HIGH==
//======================
void startTX(){
  spiWriteReg(0x10,0x3, 1); 
  delayMicroseconds(20);
}

// ---------------------------------------------------------
// This function calls setfrequency() multiple times in a loop to mimic chirp signal
// in a loop to mimic chirp signal
// Param: n/a
// Return: void
//-----------------------------------------------------------
void setChirp(){
  setPinLow(SPI_MOSI);
  int i = 400;
  for(i=400; i<405; i++){
    setFrequency(i);
    startTX();
    setPinLow(SPI_MOSI);
    delay(CHIRP_DELAY);
  }
}