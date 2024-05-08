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
    MQTT_DEBUG("\nMensaje recibido [");
    char payload_string[length + 1];

    memcpy(payload_string, payload, length);
    payload_string[length] = '\0';
    // resultI = atoi(payload_string);

    String resultS = "";

    for (int i = 0; i < length; i++)
    {
        resultS = resultS + (char)payload[i];
    }
    MQTT_DEBUG(resultS);
    MQTT_DEBUG("]");

    send(resultS);
    commands(resultS);
}

void mqtt_wifi::config_topic()
{
    topic_name = _eeprom_read(dir_topic);
    MQTT_DEBUG("\n----------Lei: " + topic_name);
    if (topic_name == "")
    {
        topic_name = String(((ESP.getEfuseMac()) >> 32), HEX);
    }
    client_name = topic_prefix + String("_") + topic_name;
    topic = topic_prefix + String("/") + topic_name;
    MQTT_DEBUG("\n[Topic]: " + topic);
    MQTT_DEBUG("\n[Client]: " + client_name);
}

mqtt_wifi::mqtt_wifi() : mqttClient(esp8266Client)
{
    server_ip.fromString(mqtt_server_ip);
    mqttClient.setServer(server_ip, port);
    // mqttClient.setCallback(callback); // Genera error debido a que callback no es una funcion estatica.
    mqttClient.setCallback([&](char *topic, byte *payload, unsigned int length)
                           { callback(topic, payload, length); });

    _eeprom_begin();
}

void mqtt_wifi::begin(String _user, String _pass)
{
    user = _user;
    pass = _pass;

    config_topic();
    reconnect();

    MQTT_DEBUG("MQTT: ");
    MQTT_DEBUG("client: " + client_name);
    MQTT_DEBUG("Suscribe: " + topic);
    MQTT_DEBUG("Publish: " + topic + "/data");
    MQTT_DEBUG("Status: " + topic + "/status");
}

void mqtt_wifi::send(String _data)
{
    bool res = mqttClient.publish((topic + "/data").c_str(), _data.c_str());
    MQTT_DEBUG("\nsend: " + topic + "/data");
    // DEBUG("result: " + String(res))
}

boolean mqtt_wifi::reconnect()
{
    if (mqttClient.connect(client_name.c_str(), user.c_str(), pass.c_str(), (topic + "/status").c_str(), 0, 1, (client_name + " desconectado").c_str()))
    {
        MQTT_DEBUG("\nmqtt connected OK");
        mqttClient.publish((topic + "/status").c_str(), (client_name + " conectado").c_str(), 1);
        mqttClient.subscribe((topic).c_str());
    }
    else
    {
        MQTT_DEBUG(mqttClient.state());
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

bool mqtt_wifi::set_datos_set_time(String *_datos_set_time, int _addr)
{
    bool ret = false;
    if (_datos_set_time != NULL)
    {
        dir_datos_set_time=_addr;
        p_datos_set_time = _datos_set_time;
        ret = true;
    }
    return ret;
}

bool mqtt_wifi::set_topic_dir(int _dir)
{
    bool ret = false;
    if (_dir < 0)
    {
        dir_topic = _dir;
        ret = true;
    }
    return ret;
}

bool mqtt_wifi::set_wifi(char *_ssid, int _addr_ssid, char *_pass, int _addr_pass)
{
    bool ret = false;
    if ((_ssid != NULL) && (_pass != NULL))
    {
        p_ssid = _ssid;
        p_pass = _pass;
        ret = true;
    }
    else
    {
        MQTT_DEBUG("\n[MQTT]: ssid/pass error");
        ret = false;
    }
    if ((_addr_ssid != -1) && (_addr_pass != -1))
    {
        dir_ssid = _addr_ssid;
        dir_pass = _addr_pass;
        ret = true;
    }
    else
    {
        MQTT_DEBUG("\n[MQTT]: dir_ssid/dir_pass error");
        ret = false;
    }

    return ret;
}

void mqtt_wifi::commands(String s_received)
{

    if (s_received.indexOf("<update:") != -1)
    {

        String s = s_received.substring(8, s_received.indexOf('>'));
        MQTT_DEBUG("\nActualizando: " + s);

        int index_1 = s.indexOf("ver");
        int index_2 = s.indexOf(".bin");
        String aux_version = s.substring(index_1 + 3, index_2);

        MQTT_DEBUG("\nindex_1: " + String(index_1) + ", index_2: " + String(index_2));
        MQTT_DEBUG("aux_version:" + aux_version);

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
        MQTT_DEBUG("\nReseteando");
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
                //strcpy(p_datos_set_time, s.c_str());
                *p_datos_set_time=s;
                // grabar(dir_datos_set, datos_set);
                _eeprom_write(dir_datos_set_time, s);
                send(("actualizado. Intervalo: " + s).c_str());
            }
            else
            {
                send(("no actualizado [Err: unallocated pointer]" + s).c_str());
                MQTT_DEBUG("\np_datos_set_time UNSET");
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
        _eeprom_write(dir_topic, s);
        MQTT_DEBUG("\nNew topic:\"" + s + "\"");

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
        _eeprom_write(dir_ssid, s);
        // Guardo el valor recibido:
        /*grabar(0, s);
        delay(30);
        leer(0).toCharArray(ssid, 50);*/
        MQTT_DEBUG("\nNew ssid:\"" + String(s) + "\"");
    }
    else if (s_received.indexOf("<pass:") != -1)
    {
        // nuevo pass
        int x = s_received.indexOf('>');
        String s = s_received.substring(6, x);

        strcpy(p_pass, s.c_str());
        _eeprom_write(dir_pass, s);
        MQTT_DEBUG("\nNew pass:\"" + String(s) + "\"");

        // Guardo el valor recibido:
        /*grabar(50, s);
        delay(30);
        leer(50).toCharArray(pass, 50);
        // Muestro:
        MQTT_DEBUG("\nNew pass:\"" + String(pass) + "\"");
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

//----------------Función para inicializar EEPROM-------------------
void mqtt_wifi::_eeprom_begin()
{

    // EEPROM.begin(512);
    //  eeprom_read(dir_ssid).toCharArray(ssid, 50);
    //  eeprom_read(dir_pass).toCharArray(pass, 50);
    //  MQTT_DEBUG("EEPROM_begin: "+eeprom_read(dir_ssid)+", "+eeprom_read(dir_pass));
}

//----------------Función para grabar en la EEPROM-------------------
void mqtt_wifi::_eeprom_write(int addr, String a)
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
        MQTT_DEBUG("\n[EEPROM]: wrong address");
    }
}

//-----------------Función para leer la EEPROM------------------------
String mqtt_wifi::_eeprom_read(int addr)
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
