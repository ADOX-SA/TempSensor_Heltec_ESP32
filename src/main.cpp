/*****************************************************************************
 *  \brief     File: main.cpp.                                                *
 *  \details   This is the main program                                       *
 *  \author    Matias Herrera                                                 *
 *  \version   1.0                                                            *
 *  \date      2024                                                           *
 ******************************************************************************/

#include <Arduino.h>
#include "global_variables.h"

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
#include "Adox_Libraries_ESP32/Bluetooh/Bluetooth_functions.h"
#include "Adox_Libraries_ESP32/gpio_functions.h"
#include "Adox_Libraries_ESP32/MFRC522_functions.h"

//-----> MQTT
#include "Adox_Libraries_ESP32/mqtt/MqttLibrary.h"
mqtt_wifi mq;

void setup()
{
  //---->
  Serial.begin(115200);
  Serial.println("Iniciando... ");
  Serial.println("CHIP ID: " + String((uint16_t)((ESP.getEfuseMac()) >> 32), HEX));

  //----> Pantalla OLED
  oled_esp32_begin();
  oled_init(ble_mode);

  //----> Memoria EEPROM
  ESP32_eeprom_begin();
  eeprom_read(dir_ssid).toCharArray(ssid, 50);
  eeprom_read(dir_pass).toCharArray(pass, 50);
  datos_set = eeprom_read(dir_datos_set);
  // Serial.print("\nDatos_set: " + datos_set);

  //----> WiFi
  ESP32_setup_wifi();
  ESP32_modoconf();

  //----> Escanear dispositivos I2C conectados
  // I2C_scanner();

  //----> MQTT
  mq.begin();
  mq.set_wifi(ssid, dir_ssid, pass, dir_pass);
  mq.set_datos_set_time(&datos_set, dir_datos_set);
  mq.set_topic_dir(dir_mqtt_topic);

  //----> Sensor de temperatura optico.
  MLX90614_begin();

  //----> Lector RFID
  MFRC522_begin();

  //----> Timer
  Timer_begin();

  //----> Battery
  battery_config();

  //----> LED pin configuration.
  pinMode(pin_led, OUTPUT);

  //----> GPIO
  GpioPinsBegin();

  Serial.println("[BLE]: init in setup");
  //----> Bluetooth
  BluetoothBegin(); // Iniciar bluetooth.
  Serial.println("[BLE]: finish in setup");
}

void loop()
{
  /**************** LOOPING FUNCTIONS ****************/
  BluetoothLoop();    // Bluetooth loop
  mq.loop();          // Peticiones de mqtt
  Serial_read_wifi(); // Para grabar ssid y pass por puerto serie.
  ESP32_loop();       // Recibe peticiones.
  CheckButtons();     // Lectura de botones
  MFRC522_loop();     // Lectura de tarjetas RFID.
  /***************************************************/



  //--------------------------------------------------
  //------> Recepcion de RFID
  //--------------------------------------------------

  if (flag_new_rfid)
  {
    flag_new_rfid = false;
    ble_flag_send_msg = true;
    //---------------------
    display.clearDisplay();
    display.display();
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print("RFID ID:");
    display.setTextSize(1);
    int pos_med = 0;
    pos_med = (((SCREEN_WIDTH - rfid_serial.length() * 6)) / 2);
    display.setCursor(pos_med, 35);
    display.println(rfid_serial);
    display.display();
    //---------------------
    oled_efect_1_tic = 2500; // reset
  }


  //--------------------------------------------------
  //------> Enviar mensaje por BLE
  //--------------------------------------------------

  if (ble_flag_send_msg)
  {
    Serial.print("\n[BLE]: send");
    ble_flag_send_msg = false;
    BluetoothSend(rfid_serial);

    display.clearDisplay();
    display.display();
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(8, 6); // Start at top-left corner
    display.print("BLE " + ble_mode + " send:");
    display.setCursor(8, 21); // Start at top-left corner
    display.print(rfid_serial);
    display.display();

    oled_efect_1_tic = 2500; // reset
  }


  //--------------------------------------------------
  //------> Recepción de mensaje BLE
  //--------------------------------------------------

  if (ble_flag_new_data)
  {
    ble_flag_new_data = false;
    //---------------------
    display.clearDisplay();
    display.display();
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(8, 6); // Start at top-left corner
    display.print("BLE " + ble_mode + " msg:");
    display.setCursor(8, 21); // Start at top-left corner
    display.print(ble_msg);
    display.display();
    //---------------------
    oled_efect_1_tic = 2500; // reset
  }


  //--------------------------------------------------
  //------> Lectura de sensor
  //--------------------------------------------------
  if (!MLX90614_tic)
  {
    // MLX90614_read();
    MLX90614_tic = 500;
  }


  //--------------------------------------------------
  //------> Lectura de bateria
  //--------------------------------------------------
  if (!battery_tic)
  {
    battery_tic = 50000;
    battery_read();
  }


  //--------------------------------------------------
  //------> Pantalla OLED
  //--------------------------------------------------

  if (!oled_efect_1_tic)
  {
    static int index = 1;
    oled_efect_1_tic = 5000;
    switch (index)
    {
    case 1:
      oled_effect_1();
      break;
    case 2:
      oled_wifi();
      break;
    case 3:
      oled_battery();
      break;
    case 4:
      display.clearDisplay();
      display.display();
      display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
      display.setTextSize(1);
      display.setCursor(8, 15); // Start at top-left corner
      display.print("Bluetooth");
      display.setCursor(8, 30); // Start at top-left corner

      if (!ble_client_connection)
      {
        display.print("disconnected");
      }
      else
      {
        display.print("connected");
      }
      display.display();
      //---------------------
      break;

    default:
      index = 0;
      break;
    }
    index++;
  }


  //--------------------------------------------------
  //------> Sensor de temperatura optico
  //--------------------------------------------------

  if (last_MLX90614_object_temp != MLX90614_object_temp)
  {
    last_MLX90614_object_temp = MLX90614_object_temp;
  }


  //--------------------------------------------------
  //------> Envio de datos por UDP
  //--------------------------------------------------

  if (!send_UDP_tic)
  {
    send_UDP_tic = 3000;
    String Paquete = "temperature=" + String(MLX90614_object_temp) + "&battery_state=A Bateria";
    Send_UDP(IP_remote, PORT_remote, Paquete);
    MLX90614_read();
  }
}