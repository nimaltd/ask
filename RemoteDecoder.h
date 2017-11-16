#ifndef _REMOTEDECODER_H
#define _REMOTEDECODER_H

#include <stdint.h>
#include <stdbool.h>
#include "remotedecoderConfig.h"
#include "tim.h"

//##########################################################################################
typedef enum
{
  RemoteDecoder_Protocol_Error,
  RemoteDecoder_Protocol_EV1527,
  RemoteDecoder_Protocol_PT226X,  
  
}RemoteDecoder_Protocol_t;
//##########################################################################################
typedef enum
{
  RemoteDecoder_BitValue_Error = 'E', 
  RemoteDecoder_BitValue_Zero = '0',
  RemoteDecoder_BitValue_One = '1',
  RemoteDecoder_BitValue_Float = 'F',
  
}RemoteDecoder_BitValue_t;
//##########################################################################################
typedef struct
{
  uint16_t                  Buffer[_REMOTE_DECODER_BUFFER_SIZE];
  uint16_t                  BufferProcess[_REMOTE_DECODER_BUFFER_SIZE];
  RemoteDecoder_BitValue_t  Code[20];
  uint8_t                   Channel;
  RemoteDecoder_Protocol_t  Protocol;
  
  uint32_t                  StartFrameTime;
  uint16_t                  PinChangeLastTime;  
  uint16_t                  SyncTime;
  uint16_t                  PinChangeCounter;
  uint16_t                  BitTime;
  
  uint8_t                   StartFrame;                   
  uint8_t                   EndFrame;
  
}RemoteDecoder_t;
//##########################################################################################
#if (_REMOTE_DECODER_ENABLE_315==1)
extern RemoteDecoder_t  RemoteDecoder_315;
void  RemoteDecoder_CallBack315(void);
void  RemoteDecoder_UserGetData315(char *Code,uint8_t Channel,RemoteDecoder_Protocol_t Protocol);
#endif
//##########################################################################################
#if (_REMOTE_DECODER_ENABLE_433==1)
extern RemoteDecoder_t  RemoteDecoder_433;
void  RemoteDecoder_CallBack433(void);
void  RemoteDecoder_UserGetData433(char *Code,uint8_t Channel,RemoteDecoder_Protocol_t Protocol);
#endif
//##########################################################################################
void  RemoteDecoder_Init(void);
void  RemoteDecoder_Process(void);
//##########################################################################################
#endif
