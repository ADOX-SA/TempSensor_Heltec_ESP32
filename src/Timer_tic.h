#include "Arduino.h"
#include <Ticker.h>

Ticker timer_1ms;

long MLX90614_tic = 0;
long send_UDP_tic = 0;
long oled_efect_1_tic = 0;
long battery_tic = 0;

void Timer_1ms()
{
    if (MLX90614_tic > 0)
        MLX90614_tic--;
    if (send_UDP_tic > 0)
        send_UDP_tic--;
    if (oled_efect_1_tic > 0)
        oled_efect_1_tic--;
    if (battery_tic > 0)
        battery_tic--;
}

void Timer_begin()
{
    timer_1ms.attach_ms(1, Timer_1ms);
}
