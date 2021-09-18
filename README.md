# Remote Receiver/Sender (EV1527,PT2262,PT2264,...) for All MCUs and platforms

 *	WebSite:    https://www.github.com/NimaLTD
 *	Instagram:  https://www.instagram.com/github.NimaLTD
 *	LinkedIn:   https://www.linkedin.com/in/NimaLTD
 *	Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw

This is a Library for decoding ASK RF remotes.

## How to Use

Include "ask_hal.h" in your source code and implement these functions:

* fn_init_rx: init rx pin as input and enable pin change interrupt.
* fn_init_tx: init tx pin as output.
* fn_pinchange_callback: pin change callback.
* fn_micros: return startup system timer in microseconds.
* fn_write_pin: write tx pin to 0 and 1.
* fn_read_pin: read rx pin.
* fn_delay_ms: delay in miliseconds.
* fn_delay_us: delay in microseconds.

Or use one of `.c` files provided in `Port` folder if it suits your platform.

## Ported Platforms

* [x] STM32 HAL
* [x] ESP32 Arduino
* [ ] ESP8266 Arduino
* [x] AVR Arduino  
* [ ] PIC
