#include <SPI.h>
//#include <avr/
#define SCK_PIN   13  // D13 = pin19 = PortB.5
#define MISO_PIN  12  // D12 = pin18 = PortB.4
#define MOSI_PIN  11  // D11 = pin17 = PortB.3

// see https://www.tutorialspoint.com/arduino/arduino_serial_peripheral_interface.htm
volatile byte index;
char buff[50];


void setup() {
  // Set MISO output, all others input
  pinMode(SCK_PIN, INPUT);
  pinMode(MOSI_PIN, INPUT);
  pinMode(MISO_PIN, OUTPUT); 
  //pinMode(SS_PIN, INPUT);

  Serial.begin(9600);
  //SPI.begin(); // controller
  //SPI.beginTransaction(SPISettings(115200, LSBFIRST, SPI_MODE0)); // controller
  SPCR |= _BV(SPE); // enable peripheral mode
  SPI.attachInterrupt();
}

ISR (SPI_STC_vect) // SPI 
{
  // read incoming byte
  byte c = SPDR;
  if (index < sizeof(buff)) {
    buff[index++] = c;
  }
  // write response
  SPDR = 'b';
}


void loop() {
  //SPI.transfer('a'); // controller mode
  Serial.println(buff);
  index=0;
}
