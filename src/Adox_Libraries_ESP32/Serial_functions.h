#include <Arduino.h>

void Serial_read_wifi()
{
    if (Serial.available() > 0)
    {
        delay(30);
        String data = Serial.readString();
        Serial.print("Serial: " + data);
        if ((data.indexOf("<ssid:") != -1))
        {
            int x = data.indexOf('>');
            String s = data.substring(6, x);
            Serial.print("New SSID:\"" + s + "\"");
            // Guardo el valor recibido:
            eeprom_write(dir_ssid, s);
        }
        else if (data.indexOf("<pass:") != -1)
        {
            int x = data.indexOf('>');
            String s = data.substring(6, x);
            Serial.print("New PASS:\"" + s + "\"");
            // Guardo el valor recibido:
            eeprom_write(dir_pass, s);
        }
    }
}