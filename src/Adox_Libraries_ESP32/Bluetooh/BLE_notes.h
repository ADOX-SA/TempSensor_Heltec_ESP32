
#include "Adox_Libraries_ESP32/Bluetooh/Bluetooth_functions.h"

void setup()
{
    BluetoothBegin(); // Iniciar bluetooth.
}

void loop()
{

    BluetoothLoop(); // Recibir mensajes BLE y comprobar conexión.

    //---- Estado de la conexión BLE:
    //---- ble_status = true -> Conectado
    //---- ble_status = false -> Desconectado
    if (ble_status)
    {
        Serial.print("BLE conectado");
    }
    else
    {
        Serial.print("BLE desconectado");
    }
    

    //---- Para enviar datos:
    String data = "Test_msg";
    BluetoothSend(data); //Envío "data" vía BLE.


    //---- Recepción de nuevos mensajes.
    // ble_flag_new_data = true; -> Nuevo mensaje recibido.
    // ble_mode: indica el modo de trabajo de la placa (client o servidor)
    if (ble_flag_new_data)
    {
        Serial.println("Se recibió un mensaje nuevo");
        Serial.print(ble_mode + ble_msg);
    }
}