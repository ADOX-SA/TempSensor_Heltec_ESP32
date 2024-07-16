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

  Serial.println("\n-----------------");
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

  Serial.println("\nRead serial is: <" + rfid_serial + ">");
    Serial.println("-----------------");
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

  // DEBUG("Envio a MQTT RFID!!!")
  // mqttClient.publish(publish_name.c_str(), (aux.c_str()));
  // flg_guardar_dato = 1;
}