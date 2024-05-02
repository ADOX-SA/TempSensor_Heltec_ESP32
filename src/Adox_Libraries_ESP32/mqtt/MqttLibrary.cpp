#include "Arduino.h"
/**
 * 
 * Arriba del setup:
 * #include "mqtt/MqttLibrary.h"
 * mqtt_wifi mq;
 * 
 * En el setup:
 * mq.begin();
 * 
 * En el loop:
 * mq.loop();
 */

#include "MqttLibrary.h"

void mqtt_wifi::callback(char *topic, byte *payload, int length)
{
    Serial.print("\nMensaje recibido [");
    char payload_string[length + 1];

    memcpy(payload_string, payload, length);
    payload_string[length] = '\0';
    // resultI = atoi(payload_string);

    String resultS = "";

    for (int i = 0; i < length; i++)
    {
        resultS = resultS + (char)payload[i];
    }
    Serial.print(resultS);
    Serial.print("]");

    send(resultS);
    //  mqtt_commands(resultS);
}

mqtt_wifi::mqtt_wifi() : mqttClient(esp8266Client)
{
    server_ip.fromString(mqtt_server_ip);
    mqttClient.setServer(server_ip, port);
    // mqttClient.setCallback(callback); // Genera error debido a que callback no es una funcion estatica.
    mqttClient.setCallback([&](char *topic, byte *payload, unsigned int length)
                           { callback(topic, payload, length); });

    client_name = mqtt_group + String("_") + String(((ESP.getEfuseMac()) >> 32), HEX);
    topic_name = mqtt_group + String("/") + String(((ESP.getEfuseMac()) >> 32), HEX);
}

void mqtt_wifi::begin(String _user, String _pass)
{
    user = _user;
    pass = _pass;

    reconnect();

    Serial.println("MQTT: ");
    Serial.println("client: " + client_name);
    Serial.println("Suscribe: " + topic_name);
    Serial.println("Publish: " + topic_name + "/data");
    Serial.println("Status: " + topic_name + "/status");
}

void mqtt_wifi::send(String _data)
{
    bool res = mqttClient.publish((topic_name + "/data").c_str(), _data.c_str());
    // DEBUG("result: " + String(res))
}

boolean mqtt_wifi::reconnect()
{
    if (mqttClient.connect(client_name.c_str(), user.c_str(), pass.c_str(), (topic_name + "/status").c_str(), 0, 1, (client_name + " desconectado").c_str()))
    {
        Serial.println("mqtt connected OK");
        mqttClient.publish((topic_name + "/status").c_str(), (client_name + " conectado").c_str(), 1);
        mqttClient.subscribe((topic_name).c_str());
    }
    else
    {
        Serial.println(mqttClient.state());
    }
    return mqttClient.connected();
}

void mqtt_wifi::loop()
{
    mqttClient.loop();
}