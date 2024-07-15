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

//-------------------------------------------
//------------- CLIENT CODE -----------------
//-------------------------------------------
//---------Codigo del cliente:

String ble_mode = "client";

static BLEUUID ServiceUUID(SERVICE_UUID);
static BLEUUID RXCharacteristicUUID(CHARACTERISTIC_UUID_TX);
static BLEUUID TXCharacteristicUUID(CHARACTERISTIC_UUID_RX); // Añadir el UUID de la característica de escritura

// Address of the peripheral device. Address will be found during scanning...
static BLEAddress *pServerAddress;
BLEClient *pClient; // Definir el cliente BLE globalmente

// Characteristics that we want to read and write
static BLERemoteCharacteristic *RXCharacteristic;
static BLERemoteCharacteristic *TXCharacteristic; // Añadir la característica de escritura

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
        Serial.print("Failed to find our characteristic UUID for RX");
        return false;
    }
    Serial.println(" - Found our RX characteristic");

    // Assign callback functions for the Characteristics
    RXCharacteristic->registerForNotify(RXNotifyCallback);

    TXCharacteristic = pRemoteService->getCharacteristic(TXCharacteristicUUID);

    if (TXCharacteristic == nullptr)
    {
        Serial.print("Failed to find our characteristic UUID for TX");
        return false;
    }
    Serial.println(" - Found our TX characteristic");

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
    if (pClient != nullptr)
    {
        bool status = pClient->isConnected();
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
    if (TXCharacteristic == nullptr)
    {
        Serial.println("TX Characteristic not found.");
        return;
    }

    // Convertir el mensaje a un arreglo de bytes
    uint8_t msgData[msg.length() + 1];
    msg.getBytes(msgData, msg.length() + 1);

    // Escribir el valor en la característica
    TXCharacteristic->writeValue(msgData, msg.length());

    Serial.print("Mensaje enviado: ");
    Serial.println(msg);
}
