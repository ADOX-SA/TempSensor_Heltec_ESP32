#include <Arduino.h>
#include "global_variables.h"

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
#include "Adox_Libraries_ESP32/Bluetooh/Bluetooth_functions.h"
#include "Adox_Libraries_ESP32/gpio_functions.h"

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
  eeprom_read(dir_ssid).toCharArray(ssid, 50);
  eeprom_read(dir_pass).toCharArray(pass, 50);
  datos_set = eeprom_read(dir_datos_set);
  Serial.print("\nDatos_set: " + datos_set);

  ESP32_setup_wifi();
  ESP32_modoconf();

  // ESP32_spiffs_begin();
  // ESP32_spiffs_write();

  // I2C_scanner();

  // mqtt:
  mq.begin();
  mq.set_wifi(ssid, dir_ssid, pass, dir_pass);
  mq.set_datos_set_time(&datos_set, dir_datos_set);
  mq.set_topic_dir(dir_mqtt_topic); // Enviar direccion para no pisar con otros datos.
  // mq.set_wifi(ssid,pass);

  // MLX90614_begin();
  Timer_begin();
  battery_config();
  pinMode(pin_led, OUTPUT);

  GpioPinsBegin();
  Serial.println("Arra1sco");
  BluetoothBegin(); // Iniciar bluetooth.
  Serial.println("Arra2");
}

void loop()
{
  /**************** LOOPING FUNCTIONS ****************/
  BluetoothLoop();    // Bluetooth loop
  mq.loop();          // Peticiones de mqtt
  Serial_read_wifi(); // Para grabar ssid y pass por puerto serie.
  ESP32_loop();       // Recibe peticiones.
  CheckButtons();
  /***************************************************/

  if (!ble_reconnect_tic)
  {

    if (ble_client_first_connection) // Es la primera conexion
    {
      bool status = BLECheckConnection();
      if (!status)
      {
        // No esta conectado
        BLEDevice::getScan()->start(15, false); // Reconectamos
                                                //---------------------
        display.clearDisplay();
        display.display();
        display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(8, 15); // Start at top-left corner
        display.print("Bluetooth");
        display.setCursor(8, 30); // Start at top-left corner
        display.print("disconnected");
        display.display();
        //---------------------
      }
    }
    else if (!ble_client_first_connection) // No se conecto por primera vez
    {
      BLEDevice::getScan()->start(15, false); // Reconectamos
    }

    //---------------------
    //---------------------
    oled_efect_1_tic = 3500; // reset
    ble_reconnect_tic = 4500;
  }

  if (ble_flag_send_msg)
  {
    ble_flag_send_msg = false;
    static int x = 0;
    x++;
    String data = "Envio: ";
    data += String(x);
    BluetoothSend(data);
    //---------------------
    display.clearDisplay();
    display.display();
    display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(8, 6); // Start at top-left corner
    display.print("BLE " + ble_mode + " send:");
    display.setCursor(8, 21); // Start at top-left corner
    display.print(data);
    display.display();
    //---------------------
    oled_efect_1_tic = 2500; // reset
  }

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

  if (!MLX90614_tic)
  {
    // MLX90614_read();
    MLX90614_tic = 500;
  }

  if (!battery_tic)
  {
    Serial.println("eStOy ViVo");
    battery_tic = 50000;
    battery_read();
    // oled_battery();
  }

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

    default:
      index = 0;
      break;
    }
    index++;

    // Serial.print("\nCONEXION: " + String(ssid) + ", " + String(pass));
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