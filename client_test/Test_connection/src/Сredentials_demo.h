#include <Arduino.h>

char ssid[] = "";
char username[] = "";
char identity[] = "";
char password[] = "";
uint8_t target_esp_sensor_mac[6] = {0x24, 0x0a, 0xc4, 0x9a, 0x58, 0x28};
#define DEVICE "ESP8266_test_device"
#define INFLUXDB_URL "http://XX.XXX.XX.XXX:8086"
#define INFLUXDB_TOKEN "vQWnxx4md6CwHCz6hfSPSCqPgzxkMLMWyou1H7XB4E_aqU2MeLkVlMYRSQ=="
#define INFLUXDB_ORG "BSU Lyceum"
#define INFLUXDB_BUCKET "sensors"
// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"