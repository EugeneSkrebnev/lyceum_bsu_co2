#include <Arduino.h>
#include <Wire.h>

#include "SoftwareSerial.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"


#define I2C_ADDRESS 0x3C

SSD1306AsciiWire oled;
SoftwareSerial co2Serial(D5, D6); // RX, TX

void setup() {
  Serial.begin(9600); //for bebug
  co2Serial.begin(9600);
  // OLED
  Wire.begin();         
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.set400kHz();  
  oled.setFont(Verdana12_bold);    
  oled.clear();  
  oled.println("Screen works okay");
}


int readCO2() { 
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  char antwort[9];
  co2Serial.write(cmd, 9);
  co2Serial.readBytes(antwort, 9);
  if (antwort[0] != 0xFF) return -1;
  if (antwort[1] != 0x86) return -1;
  int antwortHigh = (int) antwort[2]; // CO2 High Byte
  int antwortLow = (int) antwort[3];  // CO2 Low Byte
  int ppm = (256 * antwortHigh) + antwortLow;
  return ppm;                         
}


int readTempInC() { 
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  char antwort[9];
  co2Serial.write(cmd, 9);
  co2Serial.readBytes(antwort, 9);
  if (antwort[0] != 0xFF) return -1;
  if (antwort[1] != 0x86) return -1;  
  int temp = (int) antwort[4];  
  return temp - 40;
}

// the loop function runs over and over again forever
void loop() {
  oled.clear();                       
  oled.println(readCO2());
  oled.println(readTempInC());
  delay(12000);  
}