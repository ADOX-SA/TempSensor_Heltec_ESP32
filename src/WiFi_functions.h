/*
 * Servidor web con ESP32
 * Librerias necesarias para conectarnos a un entorno Wifi y poder configurar
 * un servidor WEB
 */
#include <WiFi.h>
#include <WebServer.h>

/* Añade tu SSID & Clave para acceder a tu Wifi */
const char *ssid = "Awifi";    // Tu SSID
const char *pass = "Adox2369"; // Tu Clave
void leer(int);
WebServer server(80);
void Index();
void ESP32_loop();

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

    // WebServer.on("/", Index); // Index
    server.on("/", Index); // Index

    /*WebServer.on("/wifi", Wificonf); // Configuración WiFi

    WebServer.on("/guardar_conf", guardar_conf); // Graba en la eeprom la configuracion

    WebServer.on("/escanear", escanear); // Escanean las redes wifi disponibles

    WebServer.on("/ultimodato", UltimoDatoEnviado); // Podemos ver el ultimo dato enviado

    WebServer.on("/envio", DatoEnviado); // Enviar un dato

    WebServer.on("/realtimedata", websocketdata); // Enviar un dato

    WebServer.on("/borrar_memoria", HTTP_GET, [&]()
                 {
    if (SPIFFS.format()) WebServer.send(200, "text/plain", "Memoria borrada OK");
    else WebServer.send(200, "text/plain", "Error al borrar Memoria"); });

    WebServer.on("/download", File_Download);
    WebServer.on("/date", Config_Date);
    WebServer.on("/sensed", Config_Sensed);
    WebServer.on("/config_mqtt", Config_mqtt);

    WebServer.on("/save_topic", save_mqtt_topic);*/

    // WebServer.begin();
    server.begin();
}

void Index()
{

    Serial.print("Index");
    String webpage = F("<!DOCTYPE html><html>");
    webpage += F("<head>");
    webpage += F("<title>File Server</title>"); // NOTE: 1em = 16px
    webpage += F("<meta name='viewport' content='user-scalable=yes,initial-scale=1.0,width=device-width'>");
    webpage += F("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>");
    webpage += F("<style>");
    webpage += F("</style></head><body>");

    webpage = "<br>";
    webpage += ("<h2>Chip ID: ") + SensorID + ("</h2>");
    webpage += ("<h2>Version: ") + firmVer + ("</h2>");
    webpage += ("<h3> Queda mucho por agregar </h3>");
    // pagina += F("<h3>Cambios: ") + version_changes + F("</h3>");

    webpage += R"( </body></html> )";

    server.send(200, "text/html", webpage);
}

void ESP32_loop()
{
    server.handleClient();
}

void ESP32_spiffs(){}
