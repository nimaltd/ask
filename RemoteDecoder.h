
#ifndef _REMOTE_DECODER_H
#define _REMOTE_DECODER_H


/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    2.0.0
  
  
  Reversion History:

  (2.0.0)
  Rewrite again.
  Improve performance.

*/
#ifdef __cplusplus
 extern "C" {
#endif

#include "RemoteDecoderConf.h"

#include <stdbool.h>
#include "gpio.h"   
#include "main.h"


#define   _REMOTE_DECODER_EDGE    (_REMOTE_DECODER_MAX_DATA_BYTE*16+2) 

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
  uint8_t       dataRaw[_REMOTE_DECODER_EDGE];
  uint16_t      dataRawEnd;
  
  uint8_t       data[_REMOTE_DECODER_MAX_DATA_BYTE];
  uint8_t       dataLen;
  uint8_t       dataAvailable;
  
}RemoteDecoder_t;

//#####################################################################################################
void    RemoteDecoder_init(RemoteDecoder_t *rf, GPIO_TypeDef  *gpio, uint16_t  pin);
void    RemoteDecoder_callBack(RemoteDecoder_t *rf);
void    RemoteDecoder_loop(RemoteDecoder_t *rf);
bool    RemoteDecoder_available(RemoteDecoder_t *rf, uint8_t *code, uint8_t *codeLenInByte, uint8_t *syncTime_us);
//#####################################################################################################
#ifdef __cplusplus
}
#endif

#endif
