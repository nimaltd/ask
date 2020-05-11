
#ifndef _ASK_H
#define _ASK_H

/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    2.3.0
  
  
  Reversion History:
  
  (2.3.0)
  Change file and functions name.
  Add check remote and channel pressed.
  
  (2.2.0)
  Detect new or hold key pressed.
  
  (2.1.0)
  Add "_REMOTE_DECODER_MIN_DATA_BYTE" to config.
  Add "RemoteDecoder_read() function.
  Improve noise cancelling.
  
  (2.0.0)
  Rewrite again.
  Improve performance.

*/

#ifdef __cplusplus
 extern "C" {
#endif

#include "askConfig.h"

#include <stdbool.h>
#include "gpio.h"   
#include "main.h"

#define   _ASK_EDGE    (_ASK_MAX_DATA_BYTE*16+2) 

//#####################################################################################################
typedef struct
{
  GPIO_TypeDef  *gpio;
  uint16_t      pin;
  uint32_t      lastPinChangeTimeMs;
  uint16_t      lastCNT;
  uint8_t       newFrame;
  uint8_t       endFrame;
  uint8_t       index;    
  
  uint16_t      dataRawStart;       
  uint8_t       dataRaw[_ASK_EDGE];
  uint16_t      dataRawEnd;
  
  uint8_t       data[_ASK_MAX_DATA_BYTE];
  uint8_t       dataLast[_ASK_MAX_DATA_BYTE];
  uint8_t       dataLen;
  uint8_t       dataAvailable;
  uint32_t      dataTime;
  
}ask_t;

//#####################################################################################################
void    ask_init(ask_t *rf, GPIO_TypeDef  *gpio, uint16_t  pin);
void    ask_callBackPinChange(ask_t *rf);
void    ask_loop(ask_t *rf);
bool    ask_available(ask_t *rf);
bool    ask_read(ask_t *rf, uint8_t *code, uint8_t *codeLenInByte, uint8_t *syncTime_us);
/*
        return -1 if faild
        return >= 0 , pressed channel . mask 4 last bit
*/
int8_t  ask_checkChannel(uint8_t *newCode, uint8_t *refrence, uint8_t len);
//#####################################################################################################
#ifdef __cplusplus
}
#endif

#endif
