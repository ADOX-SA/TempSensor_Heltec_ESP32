#include <Arduino.h>
#include <LittleFS.h>
#define SPIFFS LittleFS
#include <Wire.h>

// #include <SPIFFS.h>
// #include "FS.h"

void ESP32_spiffs_begin()
{

    bool spiffs_state = SPIFFS.begin(true);
    if (!spiffs_state)
    {
        Serial.println("[Storage] Couldn't mount file system.");
    }
    else
    {


        SPIFFS.format() ? Serial.print("Format") : Serial.print("NO Format");

        File file = SPIFFS.open("/");

        String filename = "";
        //Dir dir = SPIFFS.openDir("/");
        long usedbytes = 0;
        Serial.println("\nSPIFFS");
        int x=0;
        while (file.openNextFile()) 
        {
            x++;
            usedbytes += file.size();
            Serial.println(" - " +String(x)+" - " + String(file.name()) + " ," + String(file.size()) + "bytes");
        }

        float spiffs_percentage = ((float)(SPIFFS.usedBytes()) / (SPIFFS.totalBytes())) * 100;
        Serial.println("total: "+String(usedbytes)+", %: " + String(spiffs_percentage));
    }
    /*
        String filename = "";
        String filename_aux = "";
        Dir dir = SPIFFS.openDir("/");
        DateTime now = rtc.now();

        while (dir.next())
        {
          filename = dir.fileName();
          filename_aux = filename.substring(4, 14) + "T00:00:00";
          DateTime fecha_archivo(filename_aux.c_str());
    */
}

bool ESP32_spiffs_search_file(String filename) // retorna "true" si existe, "false" si no existe
{
    bool result = SPIFFS.exists(filename + ".csv");
    Serial.print("\nBusco: " + filename + ",result: " + result);
    return result;
}

void ESP32_spiffs_write()
{
    String name = "/prueba_";
    static int x = 0;

    // Incrementa el número hasta encontrar un nombre de archivo disponible
    while (ESP32_spiffs_search_file(name + String(x)))
    {
        Serial.print("\nAca x: " + String(x));
        x++;
    }

    // Construye el nombre completo del archivo
    String filename = name + String(x) + ".csv";

    // Abre el archivo en modo de escritura
    File new_file = SPIFFS.open(filename, "w");

    // Verifica si la apertura del archivo fue exitosa
    if (!new_file)
    {
        Serial.println("Error al abrir el archivo para escritura");
        return;
    }

    // Escribe en el archivo
    new_file.print("Hola matias: ");
    new_file.print(x);
    new_file.close();

    Serial.println("Archivo creado: " + filename);
}
