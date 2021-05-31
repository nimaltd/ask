# All mcu and platform Remote decoder (EV1527,PT2262,PT2264 and ...)

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

Arduino example for both frequency:
```
  ask433.fn_init_rx = ask_init_rx433;
  ask433.fn_init_tx = ask_init_tx433;
  ask433.fn_pinchange_callback = ask_pinchange433;
  ask433.fn_micros10 = micros10_433;
  ask433.fn_read_pin = read_pin_433;
  ask433.fn_write_pin = write_pin_433;
  ask433.fn_delay = ask_delay_433;  
  ask_init(&ask433);

  ask315.fn_init_rx = ask_init_rx315;
  ask315.fn_init_tx = ask_init_tx315;
  ask315.fn_pinchange_callback = ask_pinchange315;
  ask315.fn_micros10 = micros10_315;
  ask315.fn_read_pin = read_pin_315;
  ask315.fn_write_pin = write_pin_315;
  ask315.fn_delay = ask_delay_315;      
  ask_init(&ask315);
 
  
  Sender loop:
  
  void loop()
  {
      uint8_t data[3];
      data[0] = 0x3b;
      data[1] = 0x33;
      data[2] = 0x51;
      delay(2000);   
      ask_send_bytes(&ask433, data, 3, 138, 3); // 138 means 1380 us for each bit   --____ or ----_
  }


  Receiver loop:
   
   void loop()
   {
      delay(1);
      if (ask_available(&ask433))
      {
          uint8_t data[10];
          Serial.println("433 -------------------");
          Serial.printf("Len: %d\r\n", ask_read_bytes(&ask433, data));
          Serial.printf("%02X%02X%02X\r\n", data[0], data[1], data[2]);
          ask_reset_available(&ask433);
          delay(100);
          while (ask_available(&ask433))
          {
              ask_reset_available(&ask433);
              delay(100);
          }
      }
      
      if (ask_available(&ask315))
      {
          uint8_t data[10];
          Serial.println("315 -------------------");
          Serial.printf("Len: %d\r\n", ask_read_bytes(&ask315, data));
          Serial.printf("%02X%02X%02X\r\n", data[0], data[1], data[2]);
          ask_reset_available(&ask315);
          delay(100);
          while (ask_available(&ask315))
          {
              ask_reset_available(&ask315);
              delay(100);
          }
      }
      
      
```
