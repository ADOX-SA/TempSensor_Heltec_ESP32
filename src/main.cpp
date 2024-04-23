#include <Arduino.h>
String firmVer = "1.1";

const String SensorID = String((uint16_t)((ESP.getEfuseMac()) >> 32), HEX);

// uint64_t chipId=ESP.getEfuseMac();
// Serial.printf("ESP32ChipID=%04X",(uint16_t)(chipId>>32));//print High 2bytes

#include "EEPROM.h"

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
#include "Timer_tic.h"
#include "battery_functions.h"
#include "WiFi_functions.h"

void setup()
{
  Serial.begin(115200);
  oled_esp32_begin();
  oled_init();
  // initialize SX1262 with default settings
  Serial.print("Inicianco... ");

  ESP32_setup_wifi();
  ESP32_modoconf();

  // I2C_scanner();

  MLX90614_begin();
  Timer_begin();
  battery_config();
  pinMode(pin_led, OUTPUT);
}

void loop()
{

  ESP32_loop();

  if (!MLX90614_tic)
  {
    MLX90614_read();
    MLX90614_tic = 500;
  }

  //////////////////////////////////////////////
  if (!battery_tic)
  {
    battery_tic = 4000;
    battery_read();

    display.clearDisplay();
    display.display();
    //-
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    //-
    String aux = "Bateria";
    display.setCursor(10, 5); // Start at top-left corner
    display.setTextSize(2);
    display.print(aux);
    display.setCursor(25, 30); // Start at top-left corner
    display.print(battery_value);
    display.print(" V");

    display.display();
    delay(4000);
    battery_tic = 4000;
  }

  //////////////////////////////////////////////
  // if ((last_MLX90614_ambient_temp != MLX90614_ambient_temp) || (last_MLX90614_object_temp != MLX90614_object_temp))
  if (last_MLX90614_object_temp != MLX90614_object_temp)
  {
    last_MLX90614_object_temp = MLX90614_object_temp;

    String aux = String(MLX90614_object_temp, 1) + " ";
    int tam = aux.length();
    display.setCursor(25, 30); // Start at top-left corner
    display.setTextSize(2);
    display.setTextColor(SSD1306_BLACK);
    // display.fillRect(25, 30, 50, 25, SSD1306_BLACK);
    display.cp437(true);
    display.write(219);
    display.write(219);
    display.write(219);
    display.write(219);
    display.write(219);
    display.display();

    display.setCursor(25, 30); // Start at top-left corner
    display.setTextColor(SSD1306_WHITE);
    display.print(aux);
    // display.cp437(true);
    // display.write(248);
    // display.print("C");
    display.display();
  }

  //////////////////////////////////////////////
  if (!oled_efect_1_tic)
  {
    oled_efect_1_tic = 10000;
    /*
        for (int i = 0; i < 7; i++)
        {
          digitalWrite(pin_led, 1);
          delay(25);

          digitalWrite(pin_led, 0);
          delay(25);
        }
        */

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
  }

  //////////////////////////////////////////////

  if (!send_UDP_tic)
  {
    send_UDP_tic = 3000;
    String Paquete = "temperature=" + String(MLX90614_object_temp) + "&battery_state=A Bateria";
    Send_UDP(IP_remote, PORT_remote, Paquete);
    // flag_measure = 1;
    // Serial.print("Envio: " + Paquete);
    MLX90614_read();
  }
}