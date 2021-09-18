
#include "ask_hal.h"
#include <Arduino.h>

ask_t   ask433;
//##################################################################################
#define RX433_PIN           2
//##################################################################################
inline void ask_write_pin_433(bool data)
{
    
}
//##################################################################################
inline bool ask_read_pin_433(void)
{
    return digitalRead(RX433_PIN);
}
//##################################################################################
inline uint32_t ask_micros_433(void)
{
    return micros();
}
//##################################################################################
void extirq433(void)
{
    ask_pinchange_callback(&ask433);
}
//##################################################################################
void ask_init_rx433(void)
{
    pinMode(RX433_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(RX433_PIN), extirq433, CHANGE);
}
//##################################################################################
void ask_init_tx433(void)
{
    
}
//##################################################################################
inline void ask_delay_ms_433(uint32_t delay_ms)
{
    delay(delay_ms);
}
//##################################################################################
inline void  ask_delay_us_433(uint32_t delay_us)
{
    delayMicroseconds(delay_us);
}

