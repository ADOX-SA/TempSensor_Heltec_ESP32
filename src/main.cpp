#include <Arduino.h>
String firmVer = "1.1";
String oled_modo = "Receptor";

#include "EEPROM.h"
#include <WiFi.h>

int pin_led = 35;

// --- Redondear una float o una double
float redondear(float valor, int decimales)
{
  double _potencia = pow(10, decimales);
  return (roundf(valor * _potencia) / _potencia);
};

#include "oled_esp32.h"
#include "UDP_functions.h"
#include "Sensor_MLX90614.h"
#include "I2C_scanner.h"

void setup()
{
  Serial.begin(115200);
  oled_esp32_begin();
  oled_init();
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
  // MLX90614_object_temp = 21.1;

  I2C_scanner();
  

  MLX90614_begin();

  pinMode(pin_led, OUTPUT);
}

void loop()
{

  static int x = 1;
  if (x)
  {

    x = 0;
    Serial.print("\nLoop!");
  }

  //////////////////////////////////////////////
  static int time1 = millis();
  int time2 = millis();
  if ((time2 - time1) > 5000)
  {

    for (int i = 0; i < 7; i++)
    {
      digitalWrite(pin_led, 1);
      delay(25);

      digitalWrite(pin_led, 0);
      delay(25);
    }

    //---
    display.clearDisplay();
    display.display();
    //-
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    //-
    String aux = "Measuring";
    int tam = aux.length();
    display.setCursor(10, 5); // Start at top-left corner
    display.setTextSize(2);
    for (int i = 0; i < tam; i++)
    {
      display.print(aux[i]);
      display.display();
      delay(35);
    }
    //-
    aux = String(MLX90614_object_temp, 1) + " ";
    tam = aux.length();
    display.setCursor(25, 30); // Start at top-left corner
    display.setTextSize(2);
    for (int i = 0; i < tam; i++)
    {
      display.print(aux[i]);
      display.display();
      delay(10);
    }
    display.cp437(true);
    display.write(248);
    display.display();
    delay(70);
    display.print("C");
    display.display();
    //----
    time1 = time2;
    /*MLX90614_object_temp += 0.1;
    if (MLX90614_object_temp > 23.9)
    {
      MLX90614_object_temp = 21.1;
    }
    */
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
    // Serial.print("Envio: " + Paquete);
    MLX90614_read();
  }
}