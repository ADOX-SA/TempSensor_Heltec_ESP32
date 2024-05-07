#ifndef Eeprom_library
#define Eeprom_library

#include "EEPROM.h"

void eeprom_write(int, String);
String eeprom_read(int addr);

//----------------Función para inicializar EEPROM-------------------
void ESP32_eeprom_begin()
{
  EEPROM.begin(512);
  // Serial.print("EEPROM_begin: "+eeprom_read(dir_ssid)+", "+eeprom_read(dir_pass));
}

//----------------Función para grabar en la EEPROM-------------------
void eeprom_write(int addr, String a)
{
  if (addr != -1)
  {
    int tamano = a.length();
    char inchar[50];
    a.toCharArray(inchar, tamano + 1);
    for (int i = 0; i < tamano; i++)
    {
      EEPROM.write(addr + i, inchar[i]);
    }
    for (int i = tamano; i < 50; i++)
    {
      EEPROM.write(addr + i, 255);
    }
    EEPROM.commit();
  }
  else
  {
    Serial.print("\n[EEPROM]: wrong address");
  }
}

//-----------------Función para leer la EEPROM------------------------
String eeprom_read(int addr)
{
  byte lectura;
  String strlectura;
  for (int i = addr; i < addr + 50; i++)
  {
    lectura = EEPROM.read(i);
    if (lectura != 255)
    {
      strlectura += (char)lectura;
    }
  }
  return strlectura;
}

#endif