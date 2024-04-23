#include <Arduino.h>
 #include <LittleFS.h>
 #define SPIFFS LittleFS

//#include <SPIFFS.h>
//#include "FS.h"

void ESP32_spiffs_begin()
{

    bool spiffs_state = SPIFFS.begin();
    if (!spiffs_state)
    {
        Serial.println("[Storage] Couldn't mount file system.");
    }
    else
    {

        File root = SPIFFS.open("/");

        String filename = "";
        // Dir dir = SPIFFS.openDir("/");
        long usedbytes = 0;
        Serial.println("\nSPIFFS");
        Serial.print("1: " + String(root.name()) + " ," + String(root.size()));
        while (root.openNextFile())
        {
            usedbytes += root.size();
            Serial.print(" - " + String(root.name()) + " ," + String(root.size()) + "bytes");
        }
        // FSInfo fsInfo;
        // SPIFFS.info(fsInfo);

        float spiffs_percentage = ((float)(SPIFFS.usedBytes()) / (SPIFFS.totalBytes())) * 100;
        Serial.println("%: " + String(spiffs_percentage));
    }
}

void ESP32_spiffs_write()
{
    File new_file = SPIFFS.open("/prueba.txt");
    new_file.print("Hola matias");
    new_file.close();
}