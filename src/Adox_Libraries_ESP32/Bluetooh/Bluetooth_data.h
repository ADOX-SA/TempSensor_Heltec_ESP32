#include <Arduino.h>
//-------------------- Variables usadas por cliente y servidor:
#define BLE_SERVER_NAME "MatiasRey"
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

/**
 * 
 * Si se comenta la siguiente linea funciona como cliente.
 * Si NO se comenta la siguiente linea funciona como server.
 * 
 */
//#define SERVER_CODE