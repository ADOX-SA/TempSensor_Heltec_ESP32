//----> PC MATIAS -> Falta GitHub
/**
 * Library for Heltec ESP32 LoRa V3.
 *
 *
 *
 *
 *
 */
#include <Arduino.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define MLX90614_SDA 33
#define MLX90614_SCL 34

float MLX90614_ambient_temp = -1;
float MLX90614_object_temp = -1;
float last_MLX90614_ambient_temp = -1;
float last_MLX90614_object_temp = -1;
float MLX90614_emissivity = -1;
float MLX90614_emissivity_reg = -1;

void MLX90614_begin()
{
    // mlx.begin();
    bool wireStatus = Wire1.begin(MLX90614_SDA, MLX90614_SCL);
    if (wireStatus)
    {
        Serial.println("Wire init good");
    }
    
    bool MLX90614_status = mlx.begin(0x5A,&Wire1);

    Serial.print("MLX90614: " + String(wireStatus) + " : " + String(MLX90614_status));
}

void MLX90614_read()
{

    float ambient_temp_aux = mlx.readAmbientTempC();
    float object_temp_aux = mlx.readObjectTempC();

    if ((ambient_temp_aux > -100) && (ambient_temp_aux < 200))
    {
        MLX90614_ambient_temp = ambient_temp_aux;
        MLX90614_ambient_temp = redondear(MLX90614_ambient_temp, 1);
    }

    if ((object_temp_aux > -100) && (object_temp_aux < 200))
    {
        MLX90614_object_temp = object_temp_aux;
        MLX90614_object_temp = redondear(MLX90614_object_temp, 1);
    }

    // Serial.print("\nMLX90614 sensor measuring");
    //Serial.printf("\nMLX90614: %2f, %2f", ambient_temp_aux, MLX90614_object_temp);

    // MLX90614_emissivity= mlx.readEmissivity();
    // MLX90614_emissivity_reg= mlx.readEmissivityReg();
    //Serial.print("AMB: " + String(MLX90614_ambient_temp) + ", OBJ: " + String(MLX90614_object_temp));
    // Serial.print("EMIS: "+String(MLX90614_emissivity)+", EMIS_REG: "+String(MLX90614_emissivity_reg));
}
