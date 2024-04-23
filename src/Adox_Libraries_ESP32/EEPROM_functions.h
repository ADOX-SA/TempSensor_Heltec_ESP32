#include "EEPROM.h"

int dir_ssid = 0;
int dir_pass = 50;
int dir_vbat_max = 100;
int dir_vbat_min = 150;

//----------------Función para inicializar EEPROM-------------------
void ESP32_eeprom_begin() {}

//----------------Función para grabar en la EEPROM-------------------
void eeprom_write(int addr, String a)
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