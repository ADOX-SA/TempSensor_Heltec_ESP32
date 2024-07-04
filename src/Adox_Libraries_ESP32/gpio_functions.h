#include <Arduino.h>
#define PIN_BUTTON_1 7
#define PIN_BUTTON_2 6
#define PIN_BUTTON_3 5

void GpioPinsBegin()
{
    pinMode(PIN_BUTTON_1, INPUT_PULLUP);
    pinMode(PIN_BUTTON_2, INPUT_PULLUP);
    pinMode(PIN_BUTTON_3, INPUT_PULLUP);
}

void CheckButtons()
{
    if (digitalRead(PIN_BUTTON_1) == LOW)
    {
        Serial.println("Button pressed");
        ble_flag_send_msg = true;
        while (digitalRead(PIN_BUTTON_1) == LOW)
        {
            delay(1);
        }
        delay(50); //Para evitar rebotes
    }
}