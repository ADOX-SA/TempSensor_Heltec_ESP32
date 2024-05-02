

#ifndef MqttLibrary_h
#define MqttLibrary_h

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "MqttData.h"

class mqtt_wifi
{
public:
    //* Constructor
    mqtt_wifi();

    String client_name;
    String topic_name;
    void begin(String = mqtt_default_user, String = mqtt_default_password);
    WiFiClient esp8266Client;
    PubSubClient mqttClient;
    IPAddress server_ip;
    int port = 1883;
    void send(String);
    void callback(char *, byte *, int);
    boolean reconnect();
    String user; //* User to login
    String pass; //* Pass to login
    void loop();
};

#endif