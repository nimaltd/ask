
#include "RemoteDecoder.h"


void	RemoteDecoder_User_Detect(uint8_t *DecodeInByte,uint8_t DecodeLenInBit,uint8_t *RawBitTimeArray,uint8_t RawBitTimeArrayLen)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);	
}

