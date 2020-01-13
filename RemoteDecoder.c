
#include "RemoteDecoder.h"
#include "tim.h"
#include <string.h>

  
//###########################################################################################################
void RemoteDecoder_init(RemoteDecoder_t *rf, GPIO_TypeDef  *gpio, uint16_t  pin)
{
  if(rf == NULL)
    return;
  memset(rf,0,sizeof(RemoteDecoder_t));
  rf->gpio = gpio;
  rf->pin = pin; 
  _REMOTE_DECODER_TIM.Instance->ARR = 0xFFFF;
  rf->lastPinChangeTimeMs = HAL_GetTick(); 
  HAL_TIM_Base_Start(&_REMOTE_DECODER_TIM);  
}
//###########################################################################################################
void RemoteDecoder_callBack(RemoteDecoder_t *rf)
{
  if(rf == NULL)
    return;
  if((rf->newFrame == 1) && (rf->endFrame == 0))
  {
    rf->dataRaw[rf->index] = _REMOTE_DECODER_TIM.Instance->CNT - rf->lastCNT;
    if(rf->index < _REMOTE_DECODER_EDGE)
      rf->index++;
    if((HAL_GetTick() - rf->lastPinChangeTimeMs > _REMOTE_DECODER_MINIMUM_STAY_IN_LOW_STATE_TO_DETECT_NEW_FRAME_IN_MS)\
      && (HAL_GPIO_ReadPin(rf->gpio, rf->pin) == GPIO_PIN_SET))
    {
      rf->endFrame = 1;
      rf->dataRawEnd = _REMOTE_DECODER_TIM.Instance->CNT - rf->lastCNT;
    }
  }
  else if((rf->newFrame == 0) && (HAL_GetTick() - rf->lastPinChangeTimeMs > _REMOTE_DECODER_MINIMUM_STAY_IN_LOW_STATE_TO_DETECT_NEW_FRAME_IN_MS)\
    && (HAL_GPIO_ReadPin(rf->gpio, rf->pin) == GPIO_PIN_SET))
  {
    rf->newFrame = 1;
    rf->dataRawStart = _REMOTE_DECODER_TIM.Instance->CNT - rf->lastCNT;
  }
  rf->lastCNT = _REMOTE_DECODER_TIM.Instance->CNT;
  rf->lastPinChangeTimeMs = HAL_GetTick();
}
//###########################################################################################################
void RemoteDecoder_loop(RemoteDecoder_t *rf)
{
  if(rf == NULL)
    return;
  if((rf->newFrame == 1) && (rf->endFrame == 0) && (HAL_GetTick() - rf->lastPinChangeTimeMs > _REMOTE_DECODER_TIMEOUT_TO_DETECT_NEW_FRAME_IN_MS))
  {
    rf->index = 0;
    rf->newFrame = 0;
    rf->endFrame = 0;      
  }
  else if(rf->endFrame == 1)
  {
    // +++ decode
    do
    {
      if((rf->dataRawStart < (rf->dataRawEnd - rf->dataRawEnd * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100))\
        || (rf->dataRawStart > (rf->dataRawEnd + rf->dataRawEnd * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100)))
        break;
      uint8_t signalSum = rf->dataRawStart / 8;
      uint8_t signalH = signalSum * 75 / 100;
      uint8_t signalL = signalSum * 25 / 100;
      int8_t bit = 7,byte = 0;
      memset(rf->data,0,sizeof(rf->data));
      rf->dataLen = 0;
      for(uint8_t i=0; i<rf->index ; i+=2)
      {
        if((rf->dataRaw[i] >= (signalH - signalH * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i] <= (signalH + signalH * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i+1] >= (signalL - signalL * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i+1] <= (signalL + signalL * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100)))
        {
          rf->data[byte] |= (1 << bit);          
        }
        else if((rf->dataRaw[i] >= (signalL - signalL * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i] <= (signalL + signalL * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i+1] >= (signalH - signalH * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i+1] <= (signalH + signalH * _REMOTE_DECODER_TOLERANCE_IN_PERCENT / 100)))
        {
          __NOP();
        }
        else
          break;        
        bit--;
        if(bit < 0)
        {
          byte++;
          bit = 7;
        }        
      }
      rf->dataLen = byte;
      rf->dataAvailable = 1;  
    }while(0);
    // --- decode
    rf->index = 0;
    rf->newFrame = 0;
    rf->endFrame = 0; 
  }
}
//###########################################################################################################
bool RemoteDecoder_available(RemoteDecoder_t *rf, uint8_t *code, uint8_t *codeLenInByte, uint8_t *syncTime_us)
{
  if(rf == NULL)
    return false;
  if(rf->dataAvailable > 0)
  {
    if(code != NULL)
      memcpy(code, rf->data, rf->dataLen);
    if(codeLenInByte != NULL)
      *codeLenInByte = rf->dataLen;
    if(syncTime_us != NULL)
      *syncTime_us = rf->dataRawStart * 10;    
    rf->dataAvailable = 0;
    return true;
  }
  else
    return false; 
}
//###########################################################################################################
