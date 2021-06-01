#include <Arduino.h>
#include "ask.h"
#include "ask_hal.h"

void setup()
{
    Serial.begin(115200);
    Serial.printf("START\r\n");
    
    ask433.fn_init_rx = ask_init_rx433;
    ask433.fn_init_tx = ask_init_tx433;
    ask433.fn_micros = ask_micros_433;
    ask433.fn_read_pin = ask_read_pin_433;
    ask433.fn_write_pin = ask_write_pin_433;
    ask433.fn_delay_ms = ask_delay_ms_433;  
    ask433.fn_delay_us = ask_delay_us_433;  
    ask_init(&ask433);

    ask315.fn_init_rx = ask_init_rx315;
    ask315.fn_init_tx = ask_init_tx315;
    ask315.fn_micros = ask_micros_315;
    ask315.fn_read_pin = ask_read_pin_315;
    ask315.fn_write_pin = ask_write_pin_315;
    ask315.fn_delay_ms = ask_delay_ms_315;      
    ask315.fn_delay_us = ask_delay_us_315;  
    ask_init(&ask315);
}

void loop()
{
    delay(1);
    //  sender
    /*
    uint8_t data[5];
    data[0] = 0x3b;
    data[1] = 0x53;
    data[2] = 0x91;
    delay(2000);   
    ask_send_bytes(&ask433, data, 3, 1380, 5);
    delay(2000);   
    data[2] = 0x92;
    ask_send_bytes(&ask433, data, 3, 1380, 5);
    */

    //  receiver
    if (ask_available(&ask433))
    {
        uint8_t data[5];
        Serial.println("\r\n---- 433 MHz Receive ----");
        Serial.printf("Len: %d ---- Bit time: %d ---- ", ask_read_bytes(&ask433, data), ask_read_time_of_bit(&ask433));
        Serial.printf("Data: 0x%02X%02X%02X\r\n", data[0], data[1], data[2]);
        ask_wait(&ask433);
    }
    if (ask_available(&ask315))
    {
        uint8_t data[5];
        Serial.println("\r\n---- 315 MHz Receive ----");
        Serial.printf("Len: %d ---- Bit time: %d ---- ", ask_read_bytes(&ask315, data), ask_read_time_of_bit(&ask315));
        Serial.printf("Data: 0x%02X%02X%02X\r\n", data[0], data[1], data[2]);
        ask_wait(&ask315);
    }
}
