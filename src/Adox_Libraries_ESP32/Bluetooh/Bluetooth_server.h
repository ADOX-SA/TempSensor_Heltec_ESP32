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

//--------------------------------------------------------
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

            // Procesar el mensaje recibido
            String receivedMessage = "";
            for (int i = 0; i < rxValue.length(); i++)
            {
                receivedMessage += rxValue[i];
            }

            ble_msg = receivedMessage;
            Serial.print("[BLE] received value: " + ble_msg);
            ble_flag_new_data = true;
        }
    }
};

void BluetoothBegin()
{
    // Create the BLE Device
    BLEDevice::init(BLE_SERVER_NAME);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic for TX (notify)
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);

    pTxCharacteristic->addDescriptor(new BLE2902());

    // Create a BLE Characteristic for RX (write)
    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);

    pRxCharacteristic->setCallbacks(new MyCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting for a client connection to notify...");
}

void BluetoothLoop()
{
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
