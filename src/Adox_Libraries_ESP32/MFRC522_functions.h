#include <Arduino.h>

#include <MFRC522.h>

#define RST_PIN 33
#define SS_PIN 26
MFRC522 reader(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

String respuestaPost2 = "";
String remoteIP = "";
String remotePort = "";
//--------------------------------
bool flag_new_rfid;
long check_reader = 0;
String rfid_serial = "";

void MFRC522_connect();
void SetDatosEnvio_RFID(String, String);

void MFRC522_begin()
{
  SPI.begin(48,46,45 );

  reader.PCD_Init();
  // Just wait some seconds...
  delay(4);
  // Prepare the security key for the read and write functions.
  // Normally it is 0xFFFFFFFFFFFF
  // Note: 6 comes from MF_KEY_SIZE in MFRC522.h
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF; // keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }
}

void MFRC522_loop()
{

  if (!check_reader)
  {
    check_reader = 5000;
    // Serial.println(reader.PCD_PerformSelfTest());
  }

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!reader.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards. This returns false if read is not successful; and if that happens, we stop the code
  if (!reader.PICC_ReadCardSerial())
  {
    return;
  }

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = reader.PICC_GetType(reader.uid.sak);
  Serial.println(reader.PICC_GetTypeName(piccType));

  // At this point, the serial can be read. We transform from byte to hex

  rfid_serial = "";
  for (int x = 0; x < reader.uid.size; x++)
  {
    // If it is less than 10, we add zero
    if (reader.uid.uidByte[x] < 0x10)
    {
      rfid_serial += "0";
    }
    // Transform the byte to hex
    rfid_serial += String(reader.uid.uidByte[x], HEX);
    // Add a hypen
    if (x + 1 != reader.uid.size)
    {
      rfid_serial += "-";
    }
  }
  // Transform to uppercase
  rfid_serial.toUpperCase();

  Serial.println("Read serial is: <" + rfid_serial + ">");
  // flag_new_rfid = true;

  // DEBUGLN("UDP: " + remoteIP + ":" + remotePort)
  // Udp.beginPacket(remoteIP.c_str(), remotePort.toInt());
  // Udp.write(rfid_serial.c_str());
  // Udp.endPacket();

  // MFRC522_connect();
  // connect();

  flag_new_rfid = true;
  //---

  // LEDS();

  // Halt PICC
  reader.PICC_HaltA();
  // Stop encryption on PCD
  reader.PCD_StopCrypto1();

  //////////////////////////----------------
  // flg_guardar_dato = 1;
  // String aux = "{\"chipId\":\"" + SensorID + "\",\"rfid_key\":\"" + last_rfid_serial + "\",\"code\":" + last_http_code + "}";
  Serial.print("\n-----------------");
  // DEBUG("Envio a MQTT RFID!!!")
  // mqttClient.publish(publish_name.c_str(), (aux.c_str()));
  // flg_guardar_dato = 1;
}

/*
void MFRC522_connect()
{
  DEBUG("Envio...")

  int tiempo_1 = millis();
  // Serial.print("SensorID:\n");
  // Serial.print(SensorID + "\n");

  // Serial.print("RFID serial: ");
  // Serial.println(rfid_serial);

  //*****************************************Envio al sistema nuevo por metodo POST****************************************
  respuestaPost = "";
  String url_post = "http://143.198.180.26:3005/rfid_connect/rfidWrite";
  String data_post = "{\"chipId\":\"" + SensorID + "\",\"values\":[\"" + rfid_serial + "\"]}";

  // Serial.println("URL: " + url_post);
  // Serial.println("DATA: " + data_post);

  // Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  // http_post.begin(client_post, url_post); // HTTP
  // http_post.addHeader("Content-Type", "application/json");
  //
  //// Serial.print("[HTTP] POST...\n");
  //// start connection and send HTTP header and body
  // int httpCode_post = http_post.POST(data_post);
  //
  //// httpCode will be negative on error
  // if (httpCode_post > 0)
  //{
  //   // HTTP header has been send and Server response header has been handled
  //   //    Serial.printf("[HTTP] POST... code: %d\n", httpCode_post);
  //
  //  // file found at server
  //  if (httpCode_post == HTTP_CODE_OK)
  //  {
  //    envio_ok = true;
  //    // const String &payload = http_post.getString();
  //    //  Serial.println("received payload:\n<<");
  //    //  Serial.println(payload);
  //    //  Serial.println(">>");
  //  }
  //  respuestaPost = String(httpCode_post) + "<br>" + http_post.getString();
  //}
  // else
  //{
  //  // Serial.printf("[HTTP] POST... failed, error: %s\n", http_post.errorToString(httpCode_post).c_str());
  //}
  //
  // http_post.end();

  //*****************************************Envio al sistema nuevo por metodo POST2****************************************
  respuestaPost2 = "";
  url_post = "http://64.225.60.238:3002/write";

  // Serial.println("URL: " + url_post);
  // Serial.println("DATA: " + data_post);

  // Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  http_post2.begin(client_post2, url_post); // HTTP
  http_post2.addHeader("Content-Type", "application/json");

  // Serial.print("[HTTP] POST...\n");
  // start connection and send HTTP header and body
  int httpCode_post = http_post2.POST(data_post);

  DEBUGLN("Respone 1: " + String(httpCode_post) + ", time: " + String(millis() - tiempo_1))
  tiempo_1 = millis();

  // httpCode will be negative on error
  if (httpCode_post > 0)
  {
    // HTTP header has been send and Server response header has been handled
    //    Serial.printf("[HTTP] POST... code: %d\n", httpCode_post);

    // file found at server
    if (httpCode_post == HTTP_CODE_OK)
    {
      envio_ok = true;
      // const String &payload = http_post2.getString();
      //  Serial.println("received payload:\n<<");
      //  Serial.println(payload);
      //  Serial.println(">>");
    }
    respuestaPost2 = String(httpCode_post) + "<br>" + http_post2.getString();
  }
  else
  {
    // Serial.printf("[HTTP] POST... failed, error: %s\n", http_post.errorToString(httpCode_post).c_str());
  }

  http_post2.end();

  //*****************************************Envio al local por metodo POST ****************************************

  respuestaPostLocal = "";

  if (remoteIP != "" && remotePort != "")
  {
    url_post = "http://" + remoteIP + ":" + remotePort + "/write";

    // DEBUGLN("URL: " + url_post)
    //     Serial.println("URL: " + url_post);
    //     Serial.println("DATA: " + data_post);

    //    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http_postLocal.begin(client_postLocal, url_post); // HTTP
    http_postLocal.addHeader("Content-Type", "application/json");

    //    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    httpCode_post = http_postLocal.POST(data_post);
    DEBUGLN("Respone 2: " + String(httpCode_post) + ", time: " + String(millis() - tiempo_1))
    // httpCode will be negative on error
    if (httpCode_post > 0)
    {
      // HTTP header has been send and Server response header has been handled
      //      Serial.printf("[HTTP] POST... code: %d\n", httpCode_post);

      // file found at server
      if (httpCode_post == HTTP_CODE_OK)
      {
        envio_ok = true;
        // const String& payload = http_postLocal.getString();
        //  Serial.println("received payload:\n<<");
        //  Serial.println(payload);
        //  Serial.println(">>");
      }
      //    respuestaPostLocal = "";//****************************************************
      respuestaPostLocal = String(httpCode_post) + "<br>" + http_postLocal.getString();
    }
    else
    {
      //        Serial.printf("[HTTP] POST... failed, error: %s\n", http_post2.errorToString(httpCode_post).c_str());
    }

    http_postLocal.end();
  }
}
*/
/*
void config_envio_RFID()
{
  if (WebServer.args() > 0) // Arguments were received
  {
    if (WebServer.hasArg("Guardar"))
      SetDatosEnvio_RFID(WebServer.arg(0), WebServer.arg(1));
  }
  else
  {
    webpage = "";
    webpage += F("<FORM action='/sensed'>"); // Must match the calling argument e.g. '/chart' calls '/chart' after selection but with arguments!
    webpage += F("<h1>Configuracion de envio al servidor:</h1>");
    webpage += F("<p>IP_servidor_local: ");
    webpage += F("<input type='text' name='remoteIP' value='") + remoteIP + F("'><br></p>");
    webpage += F("<p>Puerto_UDP_servidor: ");
    webpage += F("<input type='text' name='remotePort' value='") + remotePort + F("'><br></p>");
    webpage += F("<input type='submit' name='Guardar' value='Guardar'><br><br>");
    webpage += F("</FORM>");

    webpage += F("<a href='/'>[Back]</a><br><br>");
    webpage += F("</FORM></body></html>");

    WebServer.send(200, "text/html", HTML_HEADER + webpage + HTML_END);
  }
}
*/
/*
void UltimoDatoEnviado_RFID()
{

  webpage = "";
  webpage += F("<h2>");
  webpage += F("El último dato enviado fue:\n") + rfid_serial;
  webpage += F("</h2>");
  webpage += F("<p>");
  webpage += F("<br><br>Respuesta Servidor Post:\n<br>") + respuestaPost;
  webpage += F("<br><br>Respuesta Servidor Post (/write):\n<br>") + respuestaPost2;
  webpage += F("<br><br>Respuesta Servidor Local Post:\n<br>") + respuestaPostLocal;
  webpage += F("</p>");

  webpage += F("<a href='/'>[Back]</a><br><br>");

  webpage += F("</body></html>");

  WebServer.send(200, "text/html", HTML_HEADER + webpage + HTML_END);
}

void SetDatosEnvio_RFID(const String dato1, const String dato2)
{
  grabar(100, dato1);
  remoteIP = leer(100);
  grabar(150, dato2);
  remotePort = leer(150);

  webpage = "";
  webpage += F("<h1>Datos de envío guardado</h1>");
  webpage += F("<a href='/confEnvio'>[Back]</a><br><br>");
  webpage += F("</body></html>");

  WebServer.send(200, "text/html", HTML_HEADER + webpage + HTML_END);

  DEBUGLN("Nueva IP: " + remoteIP)
  DEBUGLN("Nuevo port: " + remotePort)
}
*/
/*
void connect()
{
  DEBUG("Envio...")
  int tiempo_1 = millis();

  // Serial.print("SensorID:\n");
  // Serial.print(SensorID + "\n");
  // Serial.print("RFID serial: ");
  // Serial.println(rfid_serial);
  /*****************************************Envio al sistema nuevo por metodo POST***************************************
  respuestaPost = "";
  String url_post = "http://143.198.180.26:3005/rfid_connect/rfidWrite";
  String data_post = "{\"chipId\":\"" + SensorID + "\",\"values\":[\"" + rfid_serial + "\"]}";
  // String data_post = "{\"chipId\":\"f00021\",\"values\":[\"" + rfid_serial + "\"]}";
  //   Serial.println("URL: " + url_post);
  //   Serial.println("DATA: " + data_post);
  //   Serial.print("[HTTP] begin...\n");
  //   configure traged server and url
  //   http_post.begin(client_post, url_post); // HTTP
  //   http_post.addHeader("Content-Type", "application/json");
  //// Serial.print("[HTTP] POST...\n");
  //// start connection and send HTTP header and body
  // int httpCode_post = http_post.POST(data_post);
  //// httpCode will be negative on error
  // if (httpCode_post > 0)
  //{
  //   // HTTP header has been send and Server response header has been handled
  ///    Serial.printf("[HTTP] POST... code: %d\n", httpCode_post);
  //  // file found at server
  //  if (httpCode_post == HTTP_CODE_OK)
  //  {
  //    envio_ok = true;
  //    //const String &payload = http_post.getString();
  //    // Serial.println("received payload:\n<<");
  //    // Serial.println(payload);
  //    // Serial.println(">>");
  //  }
  //  respuestaPost = String(httpCode_post) + "<br>" + http_post.getString();
  //}
  // else
  //{
  //  // Serial.printf("[HTTP] POST... failed, error: %s\n", http_post.errorToString(httpCode_post).c_str());
  //}
  // http_post.end();
  respuestaPost2 = "";
  url_post = "http://64.225.60.238:3002/write";
  // Serial.println("URL: " + url_post);
  // Serial.println("DATA: " + data_post);
  // Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  http_post2.begin(client_post2, url_post); // HTTP
  http_post2.addHeader("Content-Type", "application/json");
  // Serial.print("[HTTP] POST...\n");
  // start connection and send HTTP header and body
  int httpCode_post = http_post2.POST(data_post);
  // httpCode will be negative on error
  if (httpCode_post > 0)
  {
    // HTTP header has been send and Server response header has been handled
    //    Serial.printf("[HTTP] POST... code: %d\n", httpCode_post);
    // file found at server
    if (httpCode_post == HTTP_CODE_OK)
    {
      envio_ok = true;
      // const String &payload = http_post2.getString();
      //  Serial.println("received payload:\n<<");
      //  Serial.println(payload);
      //  Serial.println(">>");
    }
    respuestaPost2 = String(httpCode_post) + "<br>" + http_post2.getString();
  }
  else
  {
    // Serial.printf("[HTTP] POST... failed, error: %s\n", http_post.errorToString(httpCode_post).c_str());
  }
  http_post2.end();
  /*****************************************Envio al local por metodo POST ****************************************
  respuestaPostLocal = "";
  if (remoteIP != "" && remotePort != "")
  {
    DEBUGLN("!!!!!!!!!!!!!!! ENTRE !!!!!!!!!!!!!!!!!")
    url_post = "http://" + remoteIP + ":" + remotePort + "/write";
    //    Serial.println("URL: " + url_post);
    //    Serial.println("DATA: " + data_post);
    //    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http_postLocal.begin(client_postLocal, url_post); // HTTP
    http_postLocal.addHeader("Content-Type", "application/json");
    //    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    httpCode_post = http_postLocal.POST(data_post);
    // httpCode will be negative on error
    if (httpCode_post > 0)
    {
      // HTTP header has been send and Server response header has been handled
      //      Serial.printf("[HTTP] POST... code: %d\n", httpCode_post);

      // file found at server
      if (httpCode_post == HTTP_CODE_OK)
      {

        envio_ok = true;
        // const String& payload = http_postLocal.getString();
        //  Serial.println("received payload:\n<<");
        //  Serial.println(payload);
        //  Serial.println(">>");
      }
      //    respuestaPostLocal = "";//****************************************************
      respuestaPostLocal = String(httpCode_post) + "<br>" + http_postLocal.getString();
    }
    else
    {
      //        Serial.printf("[HTTP] POST... failed, error: %s\n", http_post2.errorToString(httpCode_post).c_str());
    }
    http_postLocal.end();
  }
  last_http_code = httpCode_post;
  DEBUGLN("Respone: " + String(httpCode_post) + ", time: " + String(millis() - tiempo_1))
}
*/
/*
void rfid_index()
{

  String page = R"(
    </body>
    <style>
              input[type=text], select {
              width: 90%;
              padding: 12px 20px;
              margin: 8px 0;
              display: inline-block;
              border: 1px solid #ccc;
              border-radius: 4px;
              box-sizing: border-box;
            }

            input[type=password], select {
              width: 90%;
              padding: 12px 20px;
              margin: 8px 0;
              display: inline-block;
              border: 1px solid #ccc;
              border-radius: 4px;
              box-sizing: border-box;
            }

            button {
              width: 90%;
              background-color: #333;
              color: white;
              padding: 14px 20px;
              margin: 8px 0;
              border: none;
              border-radius: 4px;
              cursor: pointer;
            }

            button:hover {
              background-color: #EF280F;
            }

            input[type=submit] {
              width: 90%;
              background-color: #333;
              color: white;
              padding: 14px 20px;
              margin: 8px 0;
              border: none;
              border-radius: 4px;
              cursor: pointer;
              }

              input[type=submit]:hover {
                background-color: #EF280F;
              }

              div {
                width: 80%;
                border-radius: 10px;
                background-color: #f2f2f2;
                padding: 30px;
              }
    </style>
    <div>
           <form action="/guardar_rfid" method='get'>
                <label for="key">ID DEL USUARIO:</label><br>
                <input class='input1' name='key' type='text' placeholder="Numero de serie del usuario"><br><br>

                <label for="name">NOMBRE:</label><br>
                <input class='input1' name='name' type='text' placeholder="Nombre del usuario"><br><br>

                <label for="last_name">APELLIDO:</label><br>
                <input class='input1' name='last_name' type='text' placeholder="Apellido del usuario"><br><br>

                <label for="dni">DNI:</label><br>
                <input class='input1' name='dni' type='text' placeholder="Numero de documento"><br><br>

                <input type="submit" value="Guardar usuario">
            </form>
              <br>
    )";

  WebServer.send(200, "text/html", HTML_HEADER + page + "</div>" + HTML_END);
}
*/
//////////////////////