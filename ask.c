
#include "ask.h"
#include "tim.h"
#include <string.h>

  
//###########################################################################################################
void ask_init(ask_t *rf, GPIO_TypeDef  *gpio, uint16_t  pin)
{
  if (rf == NULL)
    return;
  memset(rf, 0, sizeof(ask_t));
  rf->gpio = gpio;
  rf->pin = pin; 
  _ASK_TIM.Instance->ARR = 0xFFFF;
  rf->lastPinChangeTimeMs = HAL_GetTick(); 
  HAL_TIM_Base_Start(&_ASK_TIM);  
}
//###########################################################################################################
void ask_callBackPinChange(ask_t *rf)
{
  if (rf == NULL)
    return;
  if ((rf->newFrame == 1) && (rf->endFrame == 0))
  {
    rf->dataRaw[rf->index] = _ASK_TIM.Instance->CNT - rf->lastCNT;
    if (rf->index < _ASK_EDGE)
      rf->index++;
    if ((HAL_GetTick() - rf->lastPinChangeTimeMs > _ASK_MINIMUM_STAY_IN_LOW_STATE_TO_DETECT_NEW_FRAME_IN_MS)\
      && (HAL_GPIO_ReadPin(rf->gpio, rf->pin) == GPIO_PIN_SET))
    {
      rf->endFrame = 1;
      rf->dataRawEnd = _ASK_TIM.Instance->CNT - rf->lastCNT;
    }
  }
  else if ((rf->newFrame == 0) && (HAL_GetTick() - rf->lastPinChangeTimeMs > _ASK_MINIMUM_STAY_IN_LOW_STATE_TO_DETECT_NEW_FRAME_IN_MS)\
    && (HAL_GPIO_ReadPin(rf->gpio, rf->pin) == GPIO_PIN_SET))
  {
    rf->newFrame = 1;
    rf->dataRawStart = _ASK_TIM.Instance->CNT - rf->lastCNT;
  }
  rf->lastCNT = _ASK_TIM.Instance->CNT;
  rf->lastPinChangeTimeMs = HAL_GetTick();
}
//###########################################################################################################
void ask_loop(ask_t *rf)
{
  if (rf == NULL)
    return;
  if ((rf->newFrame == 1) && (rf->endFrame == 0) && (HAL_GetTick() - rf->lastPinChangeTimeMs > _ASK_TIMEOUT_TO_DETECT_NEW_FRAME_IN_MS))
  {
    rf->index = 0;
    rf->newFrame = 0;
    rf->endFrame = 0;      
  }
  else if (rf->endFrame == 1)
  {
    // +++ decode
    do
    {
      if ((rf->dataRawStart < (rf->dataRawEnd - rf->dataRawEnd * _ASK_TOLERANCE_IN_PERCENT / 100))\
        || (rf->dataRawStart > (rf->dataRawEnd + rf->dataRawEnd * _ASK_TOLERANCE_IN_PERCENT / 100)))
        break;
      uint8_t signalSum = rf->dataRawStart / 8;
      uint8_t signalH = signalSum * 75 / 100;
      uint8_t signalL = signalSum * 25 / 100;
      int8_t bit = 7,byte = 0;
      memset(rf->data,0,sizeof(rf->data));
      rf->dataLen = 0;
      for (uint8_t i=0; i<rf->index ; i+=2)
      {
        if ((rf->dataRaw[i] >= (signalH - signalH * _ASK_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i] <= (signalH + signalH * _ASK_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i+1] >= (signalL - signalL * _ASK_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i+1] <= (signalL + signalL * _ASK_TOLERANCE_IN_PERCENT / 100)))
        {
          rf->data[byte] |= (1 << bit);          
        }
        else if ((rf->dataRaw[i] >= (signalL - signalL * _ASK_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i] <= (signalL + signalL * _ASK_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i+1] >= (signalH - signalH * _ASK_TOLERANCE_IN_PERCENT / 100))\
          && (rf->dataRaw[i+1] <= (signalH + signalH * _ASK_TOLERANCE_IN_PERCENT / 100)))
        {
          __NOP();
        }
        else
          break;        
        bit--;
        if (bit < 0)
        {
          byte++;
          bit = 7;
        }        
      }
      rf->dataLen = byte;
      if ((rf->dataLen >= _ASK_MIN_DATA_BYTE) && (rf->dataLen <= _ASK_MAX_DATA_BYTE)) 
        rf->dataAvailable = 1;  
    }
    while (0);
    // --- decode
    rf->index = 0;
    rf->newFrame = 0;
    rf->endFrame = 0; 
  }
}
//###########################################################################################################
bool ask_available(ask_t *rf)
{
  if (rf == NULL)
    return false;
  if (rf->dataAvailable == 1)
  {
    rf->dataAvailable = 0;
    return true;
  }
  else
  {
    if (HAL_GetTick() - rf->dataTime > _ASK_HOLD_LAST_FRAME_IN_MS)
      memset(rf->dataLast, 0, sizeof(rf->dataLast));        
    return false; 
  }
}
//###########################################################################################################
bool ask_read(ask_t *rf, uint8_t *code, uint8_t *codeLenInByte, uint8_t *syncTime_us)
{
  if (rf == NULL)
    return false;
  bool  isNew = false;
  if (code != NULL)
    memcpy(code, rf->data, rf->dataLen);
  if (codeLenInByte != NULL)
    *codeLenInByte = rf->dataLen;
  if (syncTime_us != NULL)
    *syncTime_us = rf->dataRawStart * 10;    
  if (memcmp(rf->dataLast, rf->data, rf->dataLen) != 0)
    isNew = true;
  memcpy(rf->dataLast, rf->data, rf->dataLen);
  rf->dataTime = HAL_GetTick();
  return isNew;
}
//###########################################################################################################
int16_t ask_checkChannelLast4Bit(uint8_t *newCode, uint8_t *refrence, uint8_t len)
{
  if (len < 1)
    return -1;
  uint8_t maskNew[len];
  uint8_t maskRef[len];
  memcpy(maskNew, newCode, len);
  memcpy(maskRef, refrence, len);
  maskNew[len - 1] &= 0xF0; 
  maskRef[len - 1] &= 0xF0; 
  if (memcmp(maskNew, maskRef, len) != 0)
    return -1;
  return newCode[len - 1] & 0x0F;
}
//###########################################################################################################
int16_t ask_checkChannelLast8Bit(uint8_t *newCode, uint8_t *refrence, uint8_t len)
{
  if (len < 1)
    return -1;
  if (memcmp(newCode, refrence, len - 1) != 0)
    return -1;
  return newCode[len - 1];
}
//###########################################################################################################
