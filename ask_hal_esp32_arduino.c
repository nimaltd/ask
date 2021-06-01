
#include "ask_hal.h"

ask_t   ask433;
//##################################################################################
#define RX433_PIN           27
#define TX433_PIN           25
//##################################################################################
inline void ask_write_pin_433(bool data)
{
    digitalWrite(TX433_PIN, data);
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
void IRAM_ATTR extirq433(void)
{
    ask_pinchange_callback(&ask433);
}
//##################################################################################
void ask_init_rx433(void)
{
    pinMode(RX433_PIN, INPUT);
    attachInterrupt(RX433_PIN, extirq433, CHANGE);
}
//##################################################################################
void ask_init_tx433(void)
{
    pinMode(TX433_PIN, OUTPUT);
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


ask_t   ask315;
//##################################################################################
#define RX315_PIN           14
#define TX315_PIN           26
//##################################################################################
inline void ask_write_pin_315(bool data)
{
    digitalWrite(TX315_PIN, data);
}
//##################################################################################
inline bool ask_read_pin_315(void)
{
    return digitalRead(RX315_PIN);
}
//##################################################################################
inline uint32_t ask_micros_315(void)
{
    return micros();
}
//##################################################################################
void IRAM_ATTR extirq315(void)
{
    ask_pinchange_callback(&ask315);
}
//##################################################################################
void ask_init_rx315(void)
{
    pinMode(RX315_PIN, INPUT);
    attachInterrupt(RX315_PIN, extirq315, CHANGE);
}
//##################################################################################
void ask_init_tx315(void)
{
    pinMode(TX315_PIN, OUTPUT);
}
//##################################################################################
inline void ask_delay_ms_315(uint32_t delay_ms)
{
    delay(delay_ms);
}
//##################################################################################
inline void  ask_delay_us_315(uint32_t delay_us)
{
    delayMicroseconds(delay_us);
}
