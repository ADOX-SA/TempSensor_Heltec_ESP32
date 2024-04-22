#include "Arduino.h"

/**
 * ADC1_CH0 is used to read the lithium battery voltage, the voltage of the lithium battery is:
 * VADC_IN1 = 100 / (100+390) * VBAT
 * Se encuentra en GPIO 1
 * 12 1 I/O GPIO1, ADC1_CH03
, TOUCH1, Read VBAT Voltage

 **/

#define PIN_ADC_IN 1
#define PIN_ADC_CTRL 37

float battery_value = -1;
int adc1_value = -1;

void battery_config()
{
    pinMode(PIN_ADC_IN, INPUT);
    pinMode(PIN_ADC_CTRL, OUTPUT);
    digitalWrite(PIN_ADC_CTRL, LOW);
}

void battery_read()
{
    static int x = 0;
    //x == 0 ? x = 1 : x = 0;

    if (x)
    {
        digitalWrite(PIN_ADC_CTRL, LOW);
        adc1_value = analogRead(PIN_ADC_IN);
        battery_value = (100 / (100 + 390)) * adc1_value;
        Serial.print("\nBatery voltage 1: " + String(battery_value) + " _ ADC: " + String(adc1_value));
        x = 0;
    }
    else
    {
        x = 1;
        digitalWrite(PIN_ADC_CTRL, HIGH);
        adc1_value = analogRead(PIN_ADC_IN);
        battery_value = (100 / (100 + 390)) * adc1_value;
        Serial.print("\nBatery voltage 2: " + String(battery_value) + " _ ADC: " + String(adc1_value));
    }
}