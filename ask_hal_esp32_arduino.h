#ifndef _ASK_HAL_H_
#define _ASK_HAL_H_

#ifdef __cplusplus
 extern "C" {
#endif
#include <stdint.h>
#include <arduino.h>
#include "ask.h"

extern      ask_t ask433;
extern      ask_t ask315;

void        write_pin_433(bool data);
bool        read_pin_433(void);
uint32_t    micros10_433(void);
void        ask_pinchange433(void);
void        extirq433(void);
void        ask_init_rx433(void);
void        ask_init_tx433(void);
void        ask_delay_433(uint32_t delay_ms);

void        write_pin_315(bool data);
bool        read_pin_315(void);
uint32_t    micros10_315(void);
void        ask_pinchange315(void);
void        extirq315(void);
void        ask_init_rx315(void);
void        ask_init_tx315(void);
void        ask_delay_315(uint32_t delay_ms);

#ifdef __cplusplus
}
#endif
#endif
