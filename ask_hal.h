#ifndef _ASK_HAL_H_
#define _ASK_HAL_H_

#ifdef __cplusplus
 extern "C" {
#endif
#include <stdint.h>
#include "ask.h"

extern      ask_t ask433;
extern      ask_t ask315;

void        ask_write_pin_433(bool data);
bool        ask_read_pin_433(void);
uint32_t    ask_micros_433(void);
void        ask_init_rx433(void);
void        ask_init_tx433(void);
void        ask_delay_ms_433(uint32_t delay_ms);
void        ask_delay_us_433(uint32_t delay_us);

void        ask_write_pin_315(bool data);
bool        ask_read_pin_315(void);
uint32_t    ask_micros_315(void);
void        ask_init_rx315(void);
void        ask_init_tx315(void);
void        ask_delay_ms_315(uint32_t delay_ms);
void        ask_delay_us_315(uint32_t delay_us);

#ifdef __cplusplus
}
#endif
#endif
