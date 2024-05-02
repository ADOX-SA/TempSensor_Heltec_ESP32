#include <Arduino.h>
String firmVer = "1.0";

/* Añade tu SSID & Clave para acceder a tu Wifi */
char ssid[50];
char pass[50];

const String SensorID = String((uint16_t)((ESP.getEfuseMac()) >> 32), HEX);

// uint64_t chipId=ESP.getEfuseMac();
// Serial.printf("ESP32ChipID=%04X",(uint16_t)(chipId>>32));//print High 2bytes

int pin_led = 35;

// --- Redondear una float o una double
float redondear(float valor, int decimales)
{
  double _potencia = pow(10, decimales);
  return (roundf(valor * _potencia) / _potencia);
};

#include "Adox_Libraries_ESP32/UDP_functions.h"
#include "Adox_Libraries_ESP32/Sensor_MLX90614.h"
#include "Adox_Libraries_ESP32/I2C_scanner.h"
#include "Adox_Libraries_ESP32/Timer_tic.h"
#include "Adox_Libraries_ESP32/EEPROM_functions.h"
#include "Adox_Libraries_ESP32/battery_functions.h"
#include "Adox_Libraries_ESP32/SPIFFS_functions.h"
#include "Adox_Libraries_ESP32/WiFi/WiFi_functions.h"
#include "Adox_Libraries_ESP32/oled_esp32.h"
#include "Adox_Libraries_ESP32/Serial_functions.h"

//-----> MQTT
#include "Adox_Libraries_ESP32/mqtt/MqttLibrary.h"
mqtt_wifi mq;

void setup()
{
  Serial.begin(115200);
  oled_esp32_begin();
  oled_init();
  // initialize SX1262 with default settings
  Serial.println("Iniciando... ");

  Serial.println("CHIP ID: " + String((uint16_t)((ESP.getEfuseMac()) >> 32), HEX));

  ESP32_eeprom_begin();

  ESP32_setup_wifi();
  ESP32_modoconf();

  // ESP32_spiffs_begin();
  // ESP32_spiffs_write();

  // I2C_scanner();

  // mqtt:
  mq.begin();
  //mq.set_wifi(ssid,pass);

  MLX90614_begin();
  Timer_begin();
  battery_config();
  pinMode(pin_led, OUTPUT);
}

void loop()
{
  mq.loop();          // Peticiones de mqtt
  Serial_read_wifi(); // Para grabar ssid y pass por puerto serie.
  ESP32_loop();       // Recibe peticiones.

  if (!MLX90614_tic)
  {
    // MLX90614_read();
    MLX90614_tic = 500;
  }

  //////////////////////////////////////////////
  if (!battery_tic)
  {
    battery_tic = 4000;
    battery_read();
    oled_battery();
  }

  if (!oled_efect_1_tic)
  {
    oled_efect_1_tic = 10000;
    oled_effect_1();
  }

  //////////////////////////////////////////////
  // if ((last_MLX90614_ambient_temp != MLX90614_ambient_temp) || (last_MLX90614_object_temp != MLX90614_object_temp))
  /*
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
*/
  //////////////////////////////////////////////
  //////////////////////////////////////////////
  /*
    if (!send_UDP_tic)
    {
      send_UDP_tic = 3000;
      String Paquete = "temperature=" + String(MLX90614_object_temp) + "&battery_state=A Bateria";
      Send_UDP(IP_remote, PORT_remote, Paquete);
      // flag_measure = 1;
      // Serial.print("Envio: " + Paquete);
      MLX90614_read();
    }
    */
}