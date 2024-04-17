#include "Arduino.h"

/**
 * ADC1_CH0 is used to read the lithium battery voltage, the voltage of the lithium battery is:
 * VADC_IN1 = 100 / (100+390) * VBAT
 * Se encuentra en GPIO 1
 * 12 1 I/O GPIO1, ADC1_CH03
, TOUCH1, Read VBAT Voltage

 **/

#define PIN_ADC1 1

float battery_value = -1;
int adc1_value = -1;

void battery_config()
{
    pinMode(PIN_ADC1, INPUT);
}

void battery_read()
{
    adc1_value = analogRead(PIN_ADC1);
    battery_value = (float)(0.204) * adc1_value;
    Serial.print("\nBatery voltage: " + String(battery_value) +" _ "+String(adc1_value));
}