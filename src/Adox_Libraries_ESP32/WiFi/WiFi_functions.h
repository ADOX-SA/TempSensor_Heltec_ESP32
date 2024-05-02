/*
 * Servidor web con ESP32
 * Librerias necesarias para conectarnos a un entorno Wifi y poder configurar
 * un servidor WEB
 */
#include <Arduino.h>
#include <WiFi.h>

#include "WiFi_update.h"
//#include <WebServer.h>
#include "HTML_code.h"


String mensaje = "";
void leer(int);
WebServer server(80);
HTTPUpdateServer updater;

//---> Functions definitions.
void ESP32_Index();
void ESP32_File_Download();
void ESP32_loop();
void ESP32_Wificonf();
void ESP32_guardar_conf();
void ESP32_escanear();
void ESP32_Config_Date();

void ESP32_setup_wifi()
{
    // Conexión WIFI
    // leer(0).toCharArray(ssid, 50);
    // leer(50).toCharArray(pass, 50);
    Serial.print("\nSSID: " + String(ssid) + ", pass: " + String(pass));
    WiFi.mode(WIFI_STA); // para que no inicie el SoftAP en el modo normal
    //  WiFi.mode(WIFI_AP_STA); /*1*/ //ESP8266 works in both AP mode and station mode
    WiFi.begin(ssid, pass);
    int contconexion = 0;
    while (WiFi.status() != WL_CONNECTED and contconexion < 50)
    { // Cuenta hasta 50 si no se puede conectar lo cancela
        ++contconexion;
        delay(100);
    }
    if (contconexion < 50)
    {
        Serial.println("");
        Serial.println("WiFi conectado");
        Serial.print("IP que asignada en la red: ");
        Serial.println(WiFi.localIP());
        contconexion = 0;
    }
    else
    {
        Serial.println("");
        Serial.println("Error de conexion");
        contconexion = 0;
    }

    
    
}

void ESP32_modoconf()
{
    updater.setup(&server,"/update");

    server.on("/", ESP32_Index); // Index
    server.on("/download", ESP32_File_Download);

    /* Configuracion de WiFi */
    server.on("/wifi", ESP32_Wificonf);             // Configuración WiFi
    server.on("/guardar_conf", ESP32_guardar_conf); // Graba en la eeprom la configuracion
    server.on("/escanear", ESP32_escanear);         // Escanean las redes wifi disponibles

    /* Configuracion de la fecha */
    server.on("/date", ESP32_Config_Date);
    /*
                                                     
                                                     WebServer.on("/ultimodato", UltimoDatoEnviado); // Podemos ver el ultimo dato enviado

                                                     WebServer.on("/envio", DatoEnviado); // Enviar un dato

                                                     WebServer.on("/realtimedata", websocketdata); // Enviar un dato

                                                     WebServer.on("/borrar_memoria", HTTP_GET, [&]()
                                                                  {
                                                     if (SPIFFS.format()) WebServer.send(200, "text/plain", "Memoria borrada OK");
                                                     else WebServer.send(200, "text/plain", "Error al borrar Memoria"); });


                                                     
                                                     WebServer.on("/sensed", Config_Sensed);
                                                     WebServer.on("/config_mqtt", Config_mqtt);

                                                     WebServer.on("/save_topic", save_mqtt_topic);*/

    // WebServer.begin();
    server.begin();
}

void ESP32_Index()
{

    Serial.print("Index");

    String webpage = "<br>";
    webpage += ("<h2>Chip ID: ") + SensorID + ("</h2>");
    webpage += ("<h2>Version: ") + firmVer + ("</h2>");
    webpage += ("<h3> Queda mucho por agregar </h3>");
    webpage += ("<h3> Pero ya tenemos el /update :) </h3>");
    // pagina += F("<h3>Cambios: ") + version_changes + F("</h3>");

    server.send(200, "text/html", HTML_HEADER + webpage + HTML_END);
}

void ESP32_loop()
{
    server.handleClient();
}

void ESP32_File_Download()
{
}

void ESP32_Wificonf()
{

    String webpage = "<p>SSID: " + String(ssid) + "</p>";
    webpage += "<p>IP asignada por la red: " + WiFi.localIP().toString() + "</p>";
    webpage += "</form>"
               "<form action='guardar_conf' method='get'>"
               "SSID:<br><br>"
               "<input class='input1' name='ssid' type='text'><br>"
               "PASSWORD:<br><br>"
               "<input class='input1' name='pass' type='password'><br><br>"
               "<input class='boton' type='submit' value='GUARDAR'/><br><br>"
               "</form>"
               "<a href='escanear'><button class='boton'>ESCANEAR</button></a><br>"
               "<br><a href='/'>[Back]</a><br>";

    // WebServer.send(200, "text/html", pagina + mensaje + paginafin);
    server.send(200, "text/html", HTML_HEADER + webpage + mensaje + HTML_END);
}

void ESP32_guardar_conf()
{
    Serial.println(server.arg("ssid")); // Recibimos los valores que envia por GET el formulario web
    eeprom_write(dir_ssid, server.arg("ssid"));
    Serial.println(server.arg("pass"));
    eeprom_write(dir_pass, server.arg("pass"));

    mensaje = "Configuracion Guardada...";
    ESP32_Wificonf();
    mensaje = "";
    // flag_guardar = 1;
    /*Poner en el loop:
    if (flag_guardar)
      {
        leer(0).toCharArray(ssid, 50);
        leer(50).toCharArray(pass, 50);
        WiFi.mode(WIFI_AP_STA); // para que no inicie el SoftAP en el modo normal
        WiFi.begin(ssid, pass);
        Serial.println("------------------------------------------------");
        Wificonf();
        flag_guardar = 0;
        fin_AP_tic = 300000; // 5 minutos con AP
      }*/
}

void ESP32_escanear()
{
    int n = WiFi.scanNetworks(); // devuelve el número de redes encontradas
    Serial.println("escaneo terminado");
    if (n == 0)
    { // si no encuentra ninguna red
        Serial.println("no se encontraron redes");
        mensaje = "no se encontraron redes";
    }
    else
    {
        Serial.print(n);
        Serial.println(" redes encontradas");
        mensaje = "";
        for (int i = 0; i < n; ++i)
        {
            // agrega al STRING "mensaje" la información de las redes encontradas
            mensaje = (mensaje) + "<p>" + String(i + 1) + ": " + WiFi.SSID(i) + " </p>\r\n";
            //      mensaje = (mensaje) + "<p>" + String(i + 1) + ": " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") Ch: " + WiFi.channel(i) + " Enc: " + WiFi.encryptionType(i) + " </p>\r\n";
            // WiFi.encryptionType 5:WEP 2:WPA/PSK 4:WPA2/PSK 7:open network 8:WPA/WPA2/PSK
            delay(20);
        }
        Serial.println(mensaje);
        ESP32_Wificonf();
        mensaje = "";
    }
}

void ESP32_Config_Date()
{ // This gets called twice, the first pass selects the input, the second pass then processes the command line arguments
  if (server.args() > 0)
  { // Arguments were received
    //if (server.hasArg("currentDateTime"))
      //Set_Date_Time(server.arg(0));
  }
  else
  {
    // SendHTML_Header();
    String webpage = "";
    webpage += F("<h2>Ajustar Fecha y Hora</h2>");

    //DateTime now = rtc.now();
    //webpage += F("<p>Fecha y Hora actual: ") + now.timestamp(DateTime::TIMESTAMP_DATE) + F("&nbsp;&nbsp;&nbsp;&nbsp;") + now.timestamp(DateTime::TIMESTAMP_TIME) + F("</p>");
    webpage += F("<p>Fecha y Hora actual: arreglar esto</p>");
    webpage += F("<p>Seleccione nueva Fecha y Hora: </p>");
    webpage += F("<FORM action='/date'>"); // Must match the calling argument e.g. '/chart' calls '/chart' after selection but with arguments!
    webpage += F("<input type='datetime-local' id='currentDateTime' name='currentDateTime'>");
    webpage += F("<input type='submit'><br>");
    webpage += F("<a href='/'>[Back]</a><br><br>");

    webpage += F("</FORM></body></html>");

    // SendHTML_Content();
    // SendHTML_Stop();

    server.send(200, "text/html", HTML_HEADER + webpage + HTML_END);
  }
}