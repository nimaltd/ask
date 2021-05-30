# All mcu and platform Remote decoder (EV1527,PT2262,PT2264 and ...)
[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/O5O4221XY)

 *	WebSite:    https://www.github.com/NimaLTD
 *	Instagram:  https://www.instagram.com/github.NimaLTD
 *	LinkedIn:   https://www.linkedin.com/in/NimaLTD
 *	Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw

This is the ASK RF remote decoder for ALL platform Library  

How to use this Library:
* You have to port your platrform's function to library.
* fn_init_rx: init rx pin as input and enable pin change interrupt.
* fn_init_tx: init tx pin as putput.
* fn_pinchange_callback: pin change callback.
* fn_micros10: return startup system timer in (micosecond / 10).
* fn_write_pin: write tx pin to 0 and 1.
* fn_read_pin: read rx pin.
* fn_delay: delay in miliseconds.
* 

