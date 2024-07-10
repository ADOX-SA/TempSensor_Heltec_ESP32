#include <Arduino.h>
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "Bluetooth_data.h"

/**
 * BLE global variables.
 */
bool ble_flag_new_data = false;
bool ble_flag_send_msg = false;
//---> Client
bool ble_client_connection = false;
bool ble_client_first_connection = false;
bool ble_client_status = false;

String ble_msg = "";

#ifdef SERVER_CODE
//------ Codigo del servidor:

String ble_mode = "server";

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0)
        {
            Serial.print("[BLE] received value: ");
            for (int i = 0; i < rxValue.length(); i++)
                Serial.print(rxValue[i]);
            Serial.println();
        }
    }
};
//------------------------------------------------------
void BluetoothBegin()
{ // Create the BLE Device
    BLEDevice::init(BLE_SERVER_NAME);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);

    pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);

    pRxCharacteristic->setCallbacks(new MyCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");
}
//------------------------------------------------------
void BluetoothLoop()
{
    /*
    if (deviceConnected)
    {
        pTxCharacteristic->setValue(&txValue, 1);
        pTxCharacteristic->notify();
        txValue++;
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    }
    */

    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("[BLE]: device disconnected");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
    /*
        static int timer1 = millis();
        int timer2 = millis();
        if ((timer2 - timer1) > 5000)
        {
            timer1 = timer2;

            String mensaje = "Lo estas leyendo?";
            // Convertir el mensaje a un arreglo de bytes (uint8_t)
            uint8_t msg[mensaje.length() + 1]; // +1 para el carácter nulo
            mensaje.getBytes(msg, mensaje.length() + 1);

            // Configurar el valor de la característica TX con el mensaje
            pTxCharacteristic->setValue(msg, mensaje.length() + 1);

            // Enviar la notificación a los dispositivos conectados
            pTxCharacteristic->notify();
        }
        */
}

void BluetoothSend(String mensaje)
{
    if (mensaje != "")
    {

        // Convertir el mensaje a un arreglo de bytes (uint8_t)
        uint8_t msg[mensaje.length() + 1]; // +1 para el carácter nulo
        mensaje.getBytes(msg, mensaje.length() + 1);

        // Configurar el valor de la característica TX con el mensaje
        pTxCharacteristic->setValue(msg, mensaje.length() + 1);

        // Enviar la notificación a los dispositivos conectados
        pTxCharacteristic->notify();
    }
}

#else
//-------------------------------------------
//------------- CLIENT CODE -----------------
//-------------------------------------------
//---------Codigo del cliente:

String ble_mode = "client";

static BLEUUID ServiceUUID(SERVICE_UUID);
static BLEUUID RXCharacteristicUUID(CHARACTERISTIC_UUID_TX);

// Address of the peripheral device. Address will be found during scanning...
static BLEAddress *pServerAddress;
BLEClient *pClient; // Definir el cliente BLE globalmente

// Characteristicd that we want to read
static BLERemoteCharacteristic *RXCharacteristic;

// Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

static boolean client_doConnect = false;

String client_msg_str;
boolean client_msg_received = false;

//****************************************************************************************** */

// When the BLE Server sends a new temperature reading with the notify property
static void RXNotifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic,
                             uint8_t *pData, size_t length, bool isNotify)
{
    // store temperature value
    client_msg_received = true;

    // Convertir a String
    String dataStr;
    for (size_t i = 0; i < length; i++)
    {
        dataStr += (char)pData[i];
    }
    client_msg_str = dataStr;

    // Usar dataStr como necesites
    Serial.print("Datos recibidos: ");
    Serial.println(dataStr);
    //-----------------------------
    ble_msg = dataStr;
    ble_flag_new_data = true;
}

// Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress)
{
    pClient = BLEDevice::createClient();

    // Connect to the remote BLE Server.
    if (!pClient->connect(pAddress))
    {
        return false;
    }
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService *pRemoteService = pClient->getService(ServiceUUID);
    if (pRemoteService == nullptr)
    {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(ServiceUUID.toString().c_str());
        return false;
    }

    // Obtain a reference to the characteristics in the service of the remote BLE server.
    RXCharacteristic = pRemoteService->getCharacteristic(RXCharacteristicUUID);

    if (RXCharacteristic == nullptr)
    {
        Serial.print("Failed to find our characteristic UUID");
        return false;
    }
    Serial.println(" - Found our characteristics");

    // Assign callback functions for the Characteristics
    RXCharacteristic->registerForNotify(RXNotifyCallback);
    return true;
}

// Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        if (advertisedDevice.getName() == BLE_SERVER_NAME)
        {                                                                   // Check if the name of the advertiser matches
            advertisedDevice.getScan()->stop();                             // Scan can be stopped, we found what we are looking for
            pServerAddress = new BLEAddress(advertisedDevice.getAddress()); // Address of advertiser is the one we need
            client_doConnect = true;                                        // Set indicator, stating that we are ready to connect
            Serial.println("Device found. Connecting!");
        }
        else
        {
            Serial.println("Device not found!");
        }
    }
};

void BluetoothBegin()
{
    // Init BLE device
    BLEDevice::init("");

    // Retrieve a Scanner and set the callback we want to use to be informed when we
    // have detected a new device.  Specify that we want active scanning and start the
    // scan to run for 30 seconds.
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(15);
}

bool BLECheckConnection()
{
    //Serial.println("[BLE Client] checkconnection");
    if (pClient != nullptr)
    {
        bool status = pClient->isConnected();
        // Serial.println("[BLE Client] status: " + String(status));
        if (status)
        {
            Serial.println("[BLE Client] connected! ");
            return true;
        }
    }
    return false;
}

void BluetoothLoop()
{
    if (client_doConnect)
    {
        if (connectToServer(*pServerAddress))
        {
            Serial.println("We are now connected to the BLE Server.");
            ble_client_first_connection = true;
        }
        else
        {
            Serial.println("Failed to connect to the server; retrying.");
        }
        client_doConnect = false;
    }


    // Si hay datos nuevos disponibles, procesarlos
    if (client_msg_received)
    {
        client_msg_received = false;
        Serial.print("\n[RECEPTOR]: ");
        Serial.println(client_msg_str);
    }
}

void BluetoothSend(String msg)
{
    /**
     *
     * DEFINIR CARACTERISTICA PARA ENVIAR MENSAJES!
     *
     */
    /* code */
    String mensaje = "Lo estas leyendo?";
    // Convertir el mensaje a un arreglo de bytes (uint8_t)
    // uint8_t msg[mensaje.length() + 1]; // +1 para el carácter nulo
    // mensaje.getBytes(msg, mensaje.length() + 1);

    // Configurar el valor de la característica TX con el mensaje
    // pTxCharacteristic->setValue(msg, mensaje.length() + 1);

    // Enviar la notificación a los dispositivos conectados
    // pTxCharacteristic->notify();
}
#endif