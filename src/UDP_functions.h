/*****************************************************************************
*  \brief     UDP library.                                                   *
*  \details   This file contains all the functions for UDP communication     *
*  \author    Matias Herrera                                                 *
*  \version   1.0                                                            *
*  \date      2024                                                           *
******************************************************************************/


#include <Arduino.h>
#include <WiFiUDP.h>
WiFiUDP UDP;

IPAddress IP_remote(192, 168, 1, 5); // Dirección IP estática del NodeMCU AP;
unsigned int PORT_remote = 8005;

/**
       * @brief Send data using UDP.
       * @param ip_f destination ip
       * @param port_f destination port
       * @param str_f data
       * @return none
       */
void Send_UDP(IPAddress ip_f, unsigned int port_f, String str_f)
{
    Serial.print("\nSe enviara: \"" + str_f + "\" a la IP: ");
    Serial.print(ip_f);
    Serial.print(" y puerto: " + String(port_f));

    UDP.beginPacket(ip_f, port_f);
    //UDP.write(str_f.c_str()); //.c_str() para convertic a const char*
    UDP.write((const uint8_t *)str_f.c_str(), str_f.length());
    UDP.endPacket();
}