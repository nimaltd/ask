#ifndef _ASK_H_
#define _ASK_H_


/*
 *	Author:     Nima Askari
 *	WebSite:    https://www.github.com/NimaLTD
 *	Instagram:  https://www.instagram.com/github.NimaLTD
 *	LinkedIn:   https://www.linkedin.com/in/NimaLTD
 *	Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
 */

/*
 * Version:	3.0.2
 *
 * History:
 * 
 * (3.0.2): Fixed some bugs.
 * 
 * (3.0.1): Fixed some bugs. Changed some functions. Add ask_wait().
 * 
 * (3.0.0): Easy to port all mcu and platform. (STM32, ESP8266, ESP32, AVR, ARDUINO , ...) 
 *          Can use 2 frequency at the same time. 
 * 
 * 
 * */

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "ask_config.h"

typedef struct 
{
    uint16_t    buffer[_ASK_MAX_BYTE_LEN_ * 16 + 2];
    uint8_t     buffer_byte[_ASK_MAX_BYTE_LEN_];
    uint16_t    buffer_time;
    uint16_t    buffer_time_low[2];
    uint16_t    buffer_time_high[2];
    uint8_t     data_byte[_ASK_MAX_BYTE_LEN_];
    uint8_t     data_bit;
    uint32_t    time;
    uint16_t    index;
    bool        detect_begin;
    bool        detect_end;
    bool        detect_busy;
    bool        enable_rx;
    bool        lock;

    void        (*fn_init_rx)(void);    
    void        (*fn_init_tx)(void);    
    uint32_t    (*fn_micros)(void);
    void        (*fn_write_pin)(bool);
    bool        (*fn_read_pin)(void);
    void        (*fn_delay_ms)(uint32_t);
    void        (*fn_delay_us)(uint32_t);

}ask_t;

bool            ask_init(ask_t *ask);   //  init ask structure
void            ask_pinchange_callback(ask_t *ask); //  external pin change callback
bool            ask_available(ask_t *ask);  //  return true if data is available
void            ask_wait(ask_t *ask);   //  Wait for the key release and reset available
void            ask_reset_available(ask_t *ask);    //  reset data and ready for next data
uint8_t         ask_read_bytes(ask_t *ask, uint8_t *data);  //  read data
uint16_t        ask_read_time_of_bit(ask_t *ask);   //  return readed data bit time in microseconds
void            ask_send_bytes(ask_t *ask, uint8_t *data, uint8_t len, uint32_t bit_time_micros, uint8_t try_to_send); // send data
int16_t         ask_checkChannelLast4Bit(uint8_t *newCode, uint8_t *refrence, uint8_t len); //  compare and return channel, return -1 if failed  
int16_t         ask_checkChannelLast8Bit(uint8_t *newCode, uint8_t *refrence, uint8_t len); //  compare and return channel, return -1 if failed  

#ifdef __cplusplus
}
#endif
#endif
