#include <Arduino.h>
String ble_mode = "Receptor";
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//-------------------- Variables usadas por cliente y servidor:
#define BLE_SERVER_NAME "MatiasRey"
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

//-------------------------------------------
//------------- SERVER CODE -----------------
//-------------------------------------------
//#define SERVER_CODE
#ifdef SERVER_CODE
//------ Codigo del servidor:

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

    static int timer1 = millis();
    int timer2 = millis();
    if ((timer2 - timer1) > 5000)
    {
        timer1 = timer2;
        /* code */
        String mensaje = "Lo estas leyendo?";
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

static BLEUUID ServiceUUID(SERVICE_UUID);
static BLEUUID RXCharacteristicUUID(CHARACTERISTIC_UUID_TX);

// Address of the peripheral device. Address will be found during scanning...
static BLEAddress *pServerAddress;

// Characteristicd that we want to read
static BLERemoteCharacteristic *RXCharacteristic;

// Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

static boolean doConnect = false;
static boolean connected = false;

String RX_str;
boolean RX_bool = false;

//****************************************************************************************** */

// When the BLE Server sends a new temperature reading with the notify property
static void RXNotifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic,
                             uint8_t *pData, size_t length, bool isNotify)
{
    // store temperature value
    RX_bool = true;

     // Convertir a String
    String dataStr;
    for (size_t i = 0; i < length; i++) {
        dataStr += (char)pData[i];
    }
    RX_str = dataStr;

    // Usar dataStr como necesites
    Serial.print("Datos recibidos: ");
    Serial.println(dataStr);
}

// Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress)
{
    BLEClient *pClient = BLEDevice::createClient();

    // Connect to the remove BLE Server.
    pClient->connect(pAddress);
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService *pRemoteService = pClient->getService(ServiceUUID);
    if (pRemoteService == nullptr)
    {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(ServiceUUID.toString().c_str());
        return (false);
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
            doConnect = true;                                               // Set indicator, stating that we are ready to connect
            Serial.println("Device found. Connecting!");
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
    pBLEScan->start(30);
}

void BluetoothLoop()
{
    
    if (doConnect == true)
    {
        if (connectToServer(*pServerAddress))
        {
            Serial.println("We are now connected to the BLE Server.");
            // Activate the Notify property of each Characteristic
            //RXCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t *)notificationOn, 2, true);
            connected = true;
        }
        else
        {
            Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
        }
        doConnect = false;
    }
    // if new temperature readings are available, print in the OLED
    if (RX_bool)
    {
        RX_bool = false;

        Serial.print("\n[RECEPTOR]: ");
        Serial.println(RX_str);
    }
}
#endif