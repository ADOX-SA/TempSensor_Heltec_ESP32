#include <Arduino.h>
/**
 * 
 * Si se comenta la siguiente linea funciona como cliente.
 * Si NO se comenta la siguiente linea funciona como server.
 * 
 */
//#define SERVER_CODE

#ifdef SERVER_CODE

//------ Codigo del servidor:
#include "Bluetooth_server.h"

#else

//------ Codigo del cliente:
#include "Bluetooth_client.h"

#endif