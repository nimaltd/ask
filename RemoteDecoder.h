
#ifndef _REMOTE_DECODER_H
#define _REMOTE_DECODER_H

#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "gpio.h"
#include <stdbool.h>
#include "RemoteDecoderConfig.h"
//##############################################################################################################
typedef struct
{
	uint8_t		AllowToGetNewSample;
	uint8_t		Index;
	uint8_t 	Buff[_REMOTE_DECODER_BUFFER_SIZE];
	uint8_t 	DecodedData[2][16];
	uint8_t 	DecodedDataLen[2];
	uint16_t	LastPinChangeInTimer;
	uint32_t	LastPinChangeInSystick;
	
}RemoteDecoder_t;

extern RemoteDecoder_t RemoteDecoder;
//##############################################################################################################
void	RemoteDetector_PinChangeCallBack(void);
bool	RemoteDecoder_Init(osPriority Priority);
void	RemoteDecoder_User_Detect(uint8_t *DecodeInByte,uint8_t DecodeLenInBit,uint8_t *RawBitTimeArray,uint8_t RawBitTimeArrayLen);
//##############################################################################################################

#endif
