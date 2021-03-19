#include <Arduino.h>
#include <Wire.h>

#include "SoftwareSerial.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "Сredentials.h"

#include  <ESP8266WiFi.h>

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>


extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
#include "c_types.h"
}

//ESP8266WiFiMulti wifiMulti;
#define I2C_ADDRESS 0x3C

SSD1306AsciiWire oled; //D1 D2
SoftwareSerial co2Serial(D5, D6); // RX, TX
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);


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

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
    // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
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
  oled.println("Welcome, this is a");
  oled.println("Temp and CO2 sensor");
  oled.println("Connecting to wifi...");
  oled.println("Connecting to iot...");

  oled.setFont(CalLite24);
  setupWifiEnterprise();
}

void submitToDB(String measurement, String device, int value) {
  Point data_point(measurement);
  data_point.addTag("device", device);
  data_point.addField(measurement, value);
  if (!client.writePoint(data_point)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  Serial.print("Writing: ");
  Serial.println(data_point.toLineProtocol());
}

// the loop function runs over and over again forever
void loop() {
  oled.clear();    
  
  int wifi = WiFi.RSSI();
  submitToDB("WIFI_RSSI", DEVICE, wifi);
  
  int co2 = readCO2();
  int temp = readTempInC();
  
  if (co2 != -1) {
    submitToDB("CO2", DEVICE, co2);
  }

  if (temp != -1) {
    submitToDB("TEMP", DEVICE, temp);
  }

  if ((temp != -1) && (co2 != -1)) {
    String co2_str = "CO2 : " + String(co2);
    oled.println(co2_str);
    String temp_str = "   T : " + String(temp) + " C";
    oled.println(temp_str);
  }

  Serial.println("Wait 10s");
  delay(10000);
}