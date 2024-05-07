#include <Arduino.h>

String firmVer = "1.0";

/* Añade tu SSID & Clave para acceder a tu Wifi */
char ssid[50];
char pass[50];

const String SensorID = String((uint16_t)((ESP.getEfuseMac()) >> 32), HEX);

// uint64_t chipId=ESP.getEfuseMac();
// Serial.printf("ESP32ChipID=%04X",(uint16_t)(chipId>>32));//print High 2bytes

int pin_led = 35;

//------------> EEPROM:
int dir_ssid = 0;
int dir_pass = 50;
int dir_vbat_max = 100;
int dir_vbat_min = 150;