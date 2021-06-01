# All mcu and platform Remote Receiver/Sender (EV1527,PT2262,PT2264,...)

 *	WebSite:    https://www.github.com/NimaLTD
 *	Instagram:  https://www.instagram.com/github.NimaLTD
 *	LinkedIn:   https://www.linkedin.com/in/NimaLTD
 *	Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw

This is the ASK RF remote decoder for ALL platform Library  

## Platform Ported:
[x] STM32 HAL
[x] ESP32 Arduino

How to use this Library:
* You have to port your platrform's function to library.
* fn_init_rx: init rx pin as input and enable pin change interrupt.
* fn_init_tx: init tx pin as putput.
* fn_pinchange_callback: pin change callback.
* fn_micros: return startup system timer in microseconds.
* fn_write_pin: write tx pin to 0 and 1.
* fn_read_pin: read rx pin.
* fn_delay_ms: delay in miliseconds.
* fn_delay_us: delay in microseconds.

