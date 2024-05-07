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
    commands(resultS);
}

void mqtt_wifi::config_topic()
{
    if (topic_name == "")
    {
        topic_name = String(((ESP.getEfuseMac()) >> 32), HEX);
    }
    client_name = topic_prefix + String("_") + topic_name;
    topic = topic_prefix + String("/") + topic_name;
    Serial.print("\nZ[Topic]: " + topic);
    Serial.print("\nZ[Client]: " + client_name);
}

mqtt_wifi::mqtt_wifi() : mqttClient(esp8266Client)
{
    server_ip.fromString(mqtt_server_ip);
    mqttClient.setServer(server_ip, port);
    // mqttClient.setCallback(callback); // Genera error debido a que callback no es una funcion estatica.
    mqttClient.setCallback([&](char *topic, byte *payload, unsigned int length)
                           { callback(topic, payload, length); });


    config_topic();
}

void mqtt_wifi::begin(String _user, String _pass)
{
    user = _user;
    pass = _pass;

    reconnect();

    Serial.println("MQTT: ");
    Serial.println("client: " + client_name);
    Serial.println("Suscribe: " + topic);
    Serial.println("Publish: " + topic + "/data");
    Serial.println("Status: " + topic + "/status");
}

void mqtt_wifi::send(String _data)
{
    bool res = mqttClient.publish((topic + "/data").c_str(), _data.c_str());
    Serial.print("\nsend: " + topic + "/data");
    // DEBUG("result: " + String(res))
}

boolean mqtt_wifi::reconnect()
{
    if (mqttClient.connect(client_name.c_str(), user.c_str(), pass.c_str(), (topic + "/status").c_str(), 0, 1, (client_name + " desconectado").c_str()))
    {
        Serial.println("mqtt connected OK");
        mqttClient.publish((topic + "/status").c_str(), (client_name + " conectado").c_str(), 1);
        mqttClient.subscribe((topic).c_str());
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

bool mqtt_wifi::set_wifi(char *_ssid, char *_pass)
{
    bool ret = false;
    if ((_ssid != NULL) && (_pass != NULL))
    {
        p_ssid = _ssid;
        p_pass = _pass;
        ret = true;
    }
    return ret;
}

void mqtt_wifi::commands(String s_received)
{

    if (s_received.indexOf("<update:") != -1)
    {

        String s = s_received.substring(8, s_received.indexOf('>'));
        Serial.print("\nActualizando: " + s);

        int index_1 = s.indexOf("ver");
        int index_2 = s.indexOf(".bin");
        String aux_version = s.substring(index_1 + 3, index_2);

        Serial.println("\nindex_1: " + String(index_1) + ", index_2: " + String(index_2));
        Serial.println("aux_version:" + aux_version);

        /*
        if (aux_version != firmVer)
        {
          flag_update_firmware = 1;
          file_name = s;
        }
        else if (aux_version == firmVer)
        {
          mqttClient.publish((mqtt_prefix_topic + "/" + mqtt_topic_name + "/data").c_str(), "El dispositivo cuenta con la ultima versión!");
        }
        */
    }
    else if (s_received.indexOf("<reset>") != -1)
    {
        delay(300);
        s_received = "";
        Serial.print("\nReseteando");
        ESP.restart(); //* Usar RESTART!
    }
    else if (s_received.indexOf("<recirculate>") != -1)
    {
        // recirculate_mode = 1;
        // estado_vent = 0;
    }
    //-------------------------------------------------------------
    else if (s_received.indexOf("<datos_set_time:") != -1)
    {
        String s = s_received.substring(16, s_received.indexOf('>'));

        if (s == "2_m" || s == "3_m" || s == "5_m" || s == "10_m" || s == "15_m" || s == "20_m" || s == "30_m")
        {
            if (p_datos_set_time != NULL)
            {
                strcpy(p_datos_set_time, s.c_str());
                // grabar(dir_datos_set, datos_set);
                send(("actualizado. Intervalo: " + s).c_str());
            }
            else
            {
                send(("no actualizado [Err: unallocated pointer]" + s).c_str());
                Serial.print("\np_datos_set_time UNSET");
            }
        }
        else
        {
            send(("intervalo incorrecto. Intervalo: " + s).c_str());
        }
    }
    ///////////////////////////////////////////////////////////////
    else if (s_received.indexOf("<topic_change:") != -1)
    {
        String s = s_received.substring(14, s_received.indexOf('>'));
        // Guardo el nuevo topic:

        // grabar(dir_mqtt_topic_name, s);
        topic_name = s;

        Serial.print("\nNew topic:\"" + s + "\"");

        config_topic();

        if (WiFi.status() == WL_CONNECTED)
        {
            mqttClient.disconnect();
            reconnect();
        }
    }
    //---------------------------------------------------------------
    else if (s_received.indexOf("<data>") != -1)
    {
        // flag_send_mqtt_sim800 = 1;
    }
    else if (s_received.indexOf("<info>") != -1)
    {
        String aux = "Available commands:\n<update>\n<reset>\n<recirculate>\n<topic_change:\"name\">\n<data>\n<ssid:\"name\">\n<pass:\"pass\">\n<wifi_data>";
        /*if (WiFi.status() == WL_CONNECTED)
        {
          mqttClient.publish((mqtt_prefix_topic + "/" + mqtt_topic_name + "/data").c_str(), (aux.c_str()));
        }
        else
        {
          mqttClient_SIM800.publish((mqtt_prefix_topic + "/" + mqtt_topic_name + "/data").c_str(), (aux.c_str()));
        }*/
    }
    else if (s_received.indexOf("<ssid:") != -1)
    {
        // nuevo ssid
        int x = s_received.indexOf('>');
        String s = s_received.substring(6, x);

        strcpy(p_ssid, s.c_str()); //* Guardo en ssid lo recibido.

        // Guardo el valor recibido:
        /*grabar(0, s);
        delay(30);
        leer(0).toCharArray(ssid, 50);*/
        Serial.print("\nNew ssid:\"" + String(s) + "\"");
    }
    else if (s_received.indexOf("<pass:") != -1)
    {
        // nuevo pass
        int x = s_received.indexOf('>');
        String s = s_received.substring(6, x);

        strcpy(p_pass, s.c_str());

        Serial.print("\nNew pass:\"" + String(s) + "\"");

        // Guardo el valor recibido:
        /*grabar(50, s);
        delay(30);
        leer(50).toCharArray(pass, 50);
        // Muestro:
        Serial.print("\nNew pass:\"" + String(pass) + "\"");
        setup_wifi();
        if (WiFi.status() == WL_CONNECTED)
        {
          mqtt_mode = 1;
          flag_mqtt_connected = 0;
        }*/
    }
    else if (s_received.indexOf("<wifi_data>") != -1)
    {
        String aux = "ssid:" + String(*p_ssid) + "\npass:" + String(*p_pass);
        if (WiFi.status() == WL_CONNECTED)
        {
            // mqttClient.publish((mqtt_prefix_topic + "/" + mqtt_topic_name + "/data").c_str(), (aux.c_str()));
            mqttClient.publish((topic + "/data").c_str(), aux.c_str());
        }
        else
        {
            mqttClient.publish((topic + "/data").c_str(), aux.c_str());
            // mqttClient_SIM800.publish((mqtt_prefix_topic + "/" + mqtt_topic_name + "/data").c_str(), (aux.c_str()));
        }
    }
}
