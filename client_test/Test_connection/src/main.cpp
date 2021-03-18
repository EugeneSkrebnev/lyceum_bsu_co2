#include <Arduino.h>
#include <Wire.h>

#include "SoftwareSerial.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"


//#include <ESP8266WiFiMulti.h>
#include  <ESP8266WiFi.h>

extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
#include "c_types.h"
}

//ESP8266WiFiMulti wifiMulti;
#define I2C_ADDRESS 0x3C

SSD1306AsciiWire oled;
SoftwareSerial co2Serial(D5, D6); // RX, TX

char ssid[] = "BSULyceum";
char username[] = "Skrebnev";
char identity[] = "Skrebnev";
char password[] = "";
uint8_t target_esp_sensor_mac[6] = {0x24, 0x0a, 0xc4, 0x9a, 0x58, 0x28};




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


void setupWifiEnterprise() {

  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.setDebugOutput(true);
  Serial.printf("SDK version: %s\n", system_get_sdk_version());
  Serial.printf("Free Heap: %4d\n", ESP.getFreeHeap());
  
  // Setting ESP into STATION mode only (no AP mode or dual mode)
  wifi_set_opmode(STATION_MODE);

  struct station_config wifi_config;

  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, ssid);
  strcpy((char*)wifi_config.password, password);

  wifi_station_set_config(&wifi_config);
  wifi_set_macaddr(STATION_IF, target_esp_sensor_mac);
  

  wifi_station_set_wpa2_enterprise_auth(1);

  // Clean up to be sure no old data is still inside
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();
  wifi_station_clear_enterprise_identity();
  wifi_station_clear_enterprise_username();
  wifi_station_clear_enterprise_password();
  wifi_station_clear_enterprise_new_password();
  
  wifi_station_set_enterprise_identity((uint8*)identity, strlen(identity));
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen((char*)password));

  
  wifi_station_connect();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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
  setupWifiEnterprise();
}


// the loop function runs over and over again forever
void loop() {
  oled.clear();                       
  oled.println(readCO2());
  oled.println(readTempInC());
  delay(12000);  
}