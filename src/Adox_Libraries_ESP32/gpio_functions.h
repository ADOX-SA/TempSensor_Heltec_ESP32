/*****************************************************************************
*  \brief     GPIO file.                                                     *
*  \details   This file contains the functions for operate the buttons.      *
*  \author    Matias Herrera                                                 *
*  \version   1.0                                                            *
*  \date      2024                                                           *
******************************************************************************/


#include <Arduino.h>
#define PIN_BUTTON_1 7
#define PIN_BUTTON_2 6
#define PIN_BUTTON_3 5

bool flag_button1_pressed = false;
bool flag_button2_pressed = false;
bool flag_button3_pressed = false;

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
        Serial.println("Button 1 pressed");
        flag_button1_pressed = true;
        while (digitalRead(PIN_BUTTON_1) == LOW)
        {
            delay(1);
        }
        delay(50); // Para evitar rebotes
    }
    else if (digitalRead(PIN_BUTTON_2) == LOW)
    {
        Serial.println("Button 2 pressed");
        flag_button2_pressed = true;
        while (digitalRead(PIN_BUTTON_2) == LOW)
        {
            delay(1);
        }
        delay(50); // Para evitar rebotes
    }
    else if (digitalRead(PIN_BUTTON_3) == LOW)
    {
        Serial.println("Button 3 pressed");
        flag_button3_pressed = true;
        while (digitalRead(PIN_BUTTON_3) == LOW)
        {
            delay(1);
        }
        delay(50); // Para evitar rebotes
    }
}