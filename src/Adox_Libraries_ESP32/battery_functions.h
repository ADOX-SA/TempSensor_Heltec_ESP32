#include "Arduino.h"

float battery_value = -1;
int battery_percentage = -1;
#define battery_max_value 4.1
#define battery_min_value 3.2

#define VBAT_Read 1
#define ADC_Ctrl 37

void battery_config()
{
    pinMode(ADC_Ctrl, OUTPUT);
    pinMode(VBAT_Read, INPUT);
    adcAttachPin(VBAT_Read);
    analogReadResolution(12);
}

void battery_read()
{
    // ADC resolution
    const int resolution = 12;
    const int adcMax = pow(2, resolution) - 1;
    const float adcMaxVoltage = 3.3;
    // On-board voltage divider
    const int R1 = 390;
    const int R2 = 100;
    // Calibration measurements
    const float measuredVoltage = 4.21; // Medicion tester
    const float reportedVoltage = 4.16; // Medicion ADC
    // Calibration factor
    const float factor = (adcMaxVoltage / adcMax) * ((R1 + R2) / (float)R2) * (measuredVoltage / reportedVoltage);
    // const float factor = (adcMaxVoltage / adcMax) * ((R1 + R2) / (float)R2);
    digitalWrite(ADC_Ctrl, LOW);
    delay(100);
    int analogValue = analogRead(VBAT_Read);
    digitalWrite(ADC_Ctrl, HIGH);

    float floatVoltage = factor * analogValue;
    uint16_t voltage = (int)(floatVoltage * 1000.0);

    battery_value = floatVoltage;

    battery_percentage = (((battery_value - battery_min_value) * 100) / (battery_max_value - battery_min_value));
    if (battery_percentage >= 100)
    {
        battery_percentage = 100;
    }

    Serial.print("Battery: " + String(battery_value) + " V, Vin: " + String(voltage) + " mV");
    Serial.print(", %: " + String(battery_percentage));
}