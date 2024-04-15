#include <Arduino.h>
String firmVer = "1.1";
String oled_modo = "Receptor";

#include "EEPROM.h"
#include <WiFi.h>

// --- Redondear una float o una double
float redondear(float valor, int decimales)
{
  double _potencia = pow(10, decimales);
  return (roundf(valor * _potencia) / _potencia);
};

#include "oled_esp32.h"
#include "UDP_functions.h"
#include "Sensor_MLX90614.h"

void setup()
{
  Serial.begin(115200);
  oled_esp32_begin();

  // initialize SX1262 with default settings
  Serial.print("Inicianco... ");

  // Connect WiFi:
  WiFi.mode(WIFI_STA);
  WiFi.begin("temperatured", "tempred2311");
  int c = 0;
  while ((WiFi.status() != WL_CONNECTED) && c < 50)
  {
    c++;
    delay(100);
    Serial.print('.');
  }
}

void loop()
{

  //////////////////////////////////////////////
  static int time1 = millis();
  int time2 = millis();
  if ((time2 - time1) > 2000)
  {
    time1 = time2;
  }
  //////////////////////////////////////////////

  static int timer_http_3 = millis();
  int timer_http_4 = millis();

  if ((timer_http_4 - timer_http_3) > 3000)
  {
    String Paquete = "temperature=" + String(MLX90614_object_temp) + "&battery_state=A Bateria";
    Send_UDP(IP_remote, PORT_remote, Paquete);
    // flag_measure = 1;
    timer_http_3 = timer_http_4;
      Serial.print("Envio: "+Paquete);
  }
}