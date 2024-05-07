#ifndef MqttLibrary_h
#define MqttLibrary_h

#include <Arduino.h>
#include <PubSubClient.h>


#include <EEPROM.h>


/**
 * Si trabajo con ESP8266 incluir:
 * #include <ESP8266WiFi.h>
 * Si trabajo con ESP32 incluir:
 * #include <WiFi.h>
 */
#include <WiFi.h>

#include "MqttData.h"

class mqtt_wifi
{
public:
    //* Constructor
    mqtt_wifi();

    // Functions
    void config_topic();
    void begin(String = mqtt_default_user, String = mqtt_default_password);
    void send(String);
    void callback(char *, byte *, int);
    boolean reconnect();
    void loop();
    void commands(String);
    //EEPROM:
    void _eeprom_begin();
    void _eeprom_write(int, String);
    String _eeprom_read(int);

    // Variables
    String client_name;
    String topic_name;
    String topic;
    int port = 1883;

    WiFiClient esp8266Client;
    PubSubClient mqttClient;
    IPAddress server_ip;

    String user; //* User to login
    String pass; //* Pass to login

    /** Funciones para inicializacion de punteros: */
    bool set_wifi(char *, char *);
    bool set_wifi(char *,int, char *,int);
    bool set_topic_dir(int = 200); /** Default 200*/
    bool set_datos_set_time(String*,int);

    /* Punteros a variables de configuracion: */
    char *p_ssid = NULL;
    char *p_pass = NULL;
    char *p_update_ver_name = NULL;
    String *p_datos_set_time = NULL;

    /* Direcciones para grabar en EEPROM */
    int dir_ssid = -1;
    int dir_pass = -1;
    int dir_datos_set_time = -1;
    int dir_topic = -1;
    int dir_update_ver_name = -1;
};

#endif