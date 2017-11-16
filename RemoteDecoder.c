
#include "RemoteDecoder.h"
#include "RemoteDecoderConfig.h"
#include <string.h>

//##########################################################################################
#if (_REMOTE_DECODER_ENABLE_315==1)
RemoteDecoder_t  RemoteDecoder_315;
#endif
#if (_REMOTE_DECODER_ENABLE_433==1)
RemoteDecoder_t  RemoteDecoder_433;
#endif

//##########################################################################################
void  RemoteDecoder_Init(void)
{
  _REMOTE_DECODER_TIMER.Instance->ARR = 0xFFFF;
  HAL_TIM_Base_Start(&_REMOTE_DECODER_TIMER);
  
}
//##########################################################################################
#if (_REMOTE_DECODER_ENABLE_315==1)
//##########################################################################################
void  RemoteDecoder_CallBack315(void)
{
  if( (RemoteDecoder_315.StartFrame==0) && (_REMOTE_DECODER_TIMER.Instance->CNT - RemoteDecoder_315.PinChangeLastTime)>_REMOTE_DECODER_START_FRAME_US)
  {
    RemoteDecoder_315.StartFrameTime = HAL_GetTick();
    RemoteDecoder_315.StartFrame=1;
    RemoteDecoder_315.EndFrame=0;
    RemoteDecoder_315.PinChangeCounter=0;
    memset(RemoteDecoder_315.Buffer,0,sizeof(RemoteDecoder_315.Buffer));    
  }
  else
  {   
    RemoteDecoder_315.Buffer[RemoteDecoder_315.PinChangeCounter]=_REMOTE_DECODER_TIMER.Instance->CNT - RemoteDecoder_315.PinChangeLastTime;
    if((RemoteDecoder_315.Buffer[RemoteDecoder_315.PinChangeCounter]>_REMOTE_DECODER_START_FRAME_US) && (RemoteDecoder_315.EndFrame==0)  && (RemoteDecoder_315.PinChangeCounter==49))
    {
      memcpy(RemoteDecoder_315.BufferProcess,RemoteDecoder_315.Buffer,_REMOTE_DECODER_BUFFER_SIZE*2);
      RemoteDecoder_315.EndFrame=1;
      RemoteDecoder_315.SyncTime =  RemoteDecoder_315.Buffer[RemoteDecoder_315.PinChangeCounter];
    }
    if(RemoteDecoder_315.PinChangeCounter<_REMOTE_DECODER_BUFFER_SIZE-1)
      RemoteDecoder_315.PinChangeCounter++;
  }
  
  RemoteDecoder_315.PinChangeLastTime = _REMOTE_DECODER_TIMER.Instance->CNT;
}
//##########################################################################################
void  RemoteDecoder_BufferToCode315(void)
{
  RemoteDecoder_Protocol_t  Protocol;
  uint8_t   PinChangeCounter = 0;
  uint32_t  BitTime=0;
  uint16_t  Bit10,Bit25,Bit75;
  uint16_t  Bit5,Bit47,Bit20,Bit30;
  while( RemoteDecoder_315.BufferProcess[PinChangeCounter] != 0)
  {
    PinChangeCounter++;
    if(PinChangeCounter == _REMOTE_DECODER_BUFFER_SIZE)
      break;          
  }
  if(PinChangeCounter<50)
    return;
  //++++++++++++      try to find EV1527  
  Protocol = RemoteDecoder_Protocol_EV1527;
  memset(RemoteDecoder_315.Code,0,sizeof(RemoteDecoder_433.Code));
  if(PinChangeCounter == 50)
  {
     for(uint8_t i=0 ; i<PinChangeCounter-2 ; i+=2)
    BitTime += RemoteDecoder_315.BufferProcess[i]+RemoteDecoder_315.BufferProcess[i+1];    
    RemoteDecoder_315.BitTime = BitTime/((PinChangeCounter-2)/2);
    Bit10 = (RemoteDecoder_315.BitTime/100)*10;
    Bit25 = (RemoteDecoder_315.BitTime/100)*25;
    Bit75 = (RemoteDecoder_315.BitTime/100)*75;  
    
    for(uint8_t i=0 ; i<40 ; i+=2)
    {
      if((RemoteDecoder_315.BufferProcess[i]>(Bit25-Bit10)) && (RemoteDecoder_315.BufferProcess[i]<(Bit25+Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]>(Bit75-Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]<(Bit75+Bit10)))     
        RemoteDecoder_315.Code[i/2] = RemoteDecoder_BitValue_Zero;        
      else if((RemoteDecoder_315.BufferProcess[i]>(Bit75-Bit10)) && (RemoteDecoder_315.BufferProcess[i]<(Bit75+Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]>(Bit25-Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]<(Bit25+Bit10)))     
        RemoteDecoder_315.Code[i/2] = RemoteDecoder_BitValue_One; 
      else
      {
        RemoteDecoder_315.Code[i/2] = RemoteDecoder_BitValue_Error;
        Protocol = RemoteDecoder_Protocol_Error;        
      }
    }
    if(Protocol == RemoteDecoder_Protocol_Error)
      goto TryToFindPT226X;
    uint8_t BitSel=0;
    RemoteDecoder_315.Channel=0;
    for(uint8_t i=40 ; i<48 ; i+=2)
    {
      if((RemoteDecoder_315.BufferProcess[i]>(Bit25-Bit10)) && (RemoteDecoder_315.BufferProcess[i]<(Bit25+Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]>(Bit75-Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]<(Bit75+Bit10)))     
        RemoteDecoder_315.Channel &=  ~(1<<BitSel);        
      else if((RemoteDecoder_315.BufferProcess[i]>(Bit75-Bit10)) && (RemoteDecoder_315.BufferProcess[i]<(Bit75+Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]>(Bit25-Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]<(Bit25+Bit10)))     
        RemoteDecoder_315.Channel |= (1<<BitSel);        
      BitSel++;
    }
  }
  
  #if (_REMOTE_DECODER_DEBUG==1)
  printf("\r\nDETECT EV1527 @315 MHZ,PULSE:%d,SYNC:%d us,BIT TIME:%d us\n",PinChangeCounter,RemoteDecoder_315.SyncTime,RemoteDecoder_315.BitTime);
  printf("CODE : ");
  for(uint8_t i=0;i<20 ; i++)
    printf("%c",RemoteDecoder_315.Code[i]);
  printf("\nCHANNEL : %d\r\n",RemoteDecoder_315.Channel);
  #endif
  goto END;
  //------------      try to find EV1527 
  
  //++++++++++++      try to find PT226X 
  TryToFindPT226X:
  Protocol = RemoteDecoder_Protocol_PT226X;
  memset(RemoteDecoder_315.Code,0,sizeof(RemoteDecoder_315.Code));
  if(PinChangeCounter == 50)
  {
    BitTime=0;
    for(uint8_t i=0 ; i<PinChangeCounter-2 ; i+=4)
      BitTime += RemoteDecoder_315.BufferProcess[i]+RemoteDecoder_315.BufferProcess[i+1]+RemoteDecoder_315.BufferProcess[i+2]+RemoteDecoder_315.BufferProcess[i+3];    
    RemoteDecoder_315.BitTime = BitTime/((PinChangeCounter-2)/4);
    
    Bit5 = (RemoteDecoder_315.BitTime/100)*5;
    Bit10 = (RemoteDecoder_315.BitTime/100)*10;
    Bit47 = (RemoteDecoder_315.BitTime/100)*47;
    Bit20 = (RemoteDecoder_315.BitTime/100)*20;  
    Bit30 = (RemoteDecoder_315.BitTime/100)*30;  
    
    for(uint8_t i=0 ; i<32 ; i+=4)
    {
      if((RemoteDecoder_315.BufferProcess[i]>(Bit20-Bit10)) && (RemoteDecoder_315.BufferProcess[i]<(Bit20+Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]>(Bit30-Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]<(Bit30+Bit10)) \
      && (RemoteDecoder_315.BufferProcess[i+2]>(Bit20-Bit10)) && (RemoteDecoder_315.BufferProcess[i+2]<(Bit20+Bit10)) && (RemoteDecoder_315.BufferProcess[i+3]>(Bit30-Bit10)) && (RemoteDecoder_315.BufferProcess[i+3]<(Bit30+Bit10)))
      {
          RemoteDecoder_315.Code[i/4] = RemoteDecoder_BitValue_Zero;        
      }
      else if((RemoteDecoder_433.BufferProcess[i]>(Bit47-Bit5)) && (RemoteDecoder_315.BufferProcess[i]<(Bit47+Bit5)) && (RemoteDecoder_315.BufferProcess[i+1]>(Bit5-Bit5/2)) && (RemoteDecoder_315.BufferProcess[i+1]<(Bit5+Bit5/2))\
      && (RemoteDecoder_315.BufferProcess[i+2]>(Bit47-Bit5)) && (RemoteDecoder_315.BufferProcess[i+2]<(Bit47+Bit5)) && (RemoteDecoder_315.BufferProcess[i+3]>(Bit5-Bit5/2)) && (RemoteDecoder_315.BufferProcess[i+3]<(Bit5+Bit5/2)))
      {
        RemoteDecoder_315.Code[i/4] = RemoteDecoder_BitValue_One;        
      }
      else if((RemoteDecoder_315.BufferProcess[i]>(Bit20-Bit10)) && (RemoteDecoder_315.BufferProcess[i]<(Bit20+Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]>(Bit30-Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]<(Bit30+Bit10))\
      && (RemoteDecoder_315.BufferProcess[i+2]>(Bit47-Bit5)) && (RemoteDecoder_315.BufferProcess[i+2]<(Bit47+Bit5)) && (RemoteDecoder_315.BufferProcess[i+3]>(Bit5-Bit5/2)) && (RemoteDecoder_315.BufferProcess[i+3]<(Bit5+Bit5/2)))
      {
          RemoteDecoder_315.Code[i/4] = RemoteDecoder_BitValue_Float;        
      }
      else
      {
        RemoteDecoder_315.Code[i/4] = RemoteDecoder_BitValue_Error; 
        Protocol = RemoteDecoder_Protocol_Error;
      }
    }
    if(Protocol == RemoteDecoder_Protocol_Error)
      goto END;
    uint8_t BitSel=0;
    RemoteDecoder_315.Channel=0;
    for(uint8_t i=32 ; i<48 ; i+=4)
    {
      if((RemoteDecoder_315.BufferProcess[i]>(Bit20-Bit10)) && (RemoteDecoder_315.BufferProcess[i]<(Bit20+Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]>(Bit30-Bit10)) && (RemoteDecoder_315.BufferProcess[i+1]<(Bit30+Bit10)))     
      {
        if((RemoteDecoder_315.BufferProcess[i+2]>(Bit20-Bit10)) && (RemoteDecoder_315.BufferProcess[i+2]<(Bit20+Bit10)) && (RemoteDecoder_315.BufferProcess[i+3]>(Bit30-Bit10)) && (RemoteDecoder_315.BufferProcess[i+3]<(Bit30+Bit10)))
        {
           RemoteDecoder_315.Channel &=  ~(1<<BitSel);        
        }
      }
      else if((RemoteDecoder_315.BufferProcess[i]>(Bit47-Bit5)) && (RemoteDecoder_315.BufferProcess[i]<(Bit47+Bit5)) && (RemoteDecoder_315.BufferProcess[i+1]>(Bit5-Bit5/2)) && (RemoteDecoder_315.BufferProcess[i+1]<(Bit5+Bit5/2)))     
      {
        if((RemoteDecoder_315.BufferProcess[i+2]>(Bit47-Bit5)) && (RemoteDecoder_315.BufferProcess[i+2]<(Bit47+Bit5)) && (RemoteDecoder_315.BufferProcess[i+3]>(Bit5-Bit5/2)) && (RemoteDecoder_315.BufferProcess[i+3]<(Bit5+Bit5/2)))
        {
          RemoteDecoder_315.Channel |= (1<<BitSel);          
        }
      }
      else
      {
        Protocol = RemoteDecoder_Protocol_Error;
      } 
      BitSel++;
    }
  }
  
  #if (_REMOTE_DECODER_DEBUG==1)
  printf("\r\nDETECT PT2264 @315 MHZ,PULSE:%d,SYNC:%d us,BIT TIME:%d us\n",PinChangeCounter,RemoteDecoder_315.SyncTime,RemoteDecoder_315.BitTime);
  printf("CODE : ");
  for(uint8_t i=0;i<8 ; i++)
    printf("%c",RemoteDecoder_315.Code[i]);
  printf("\nCHANNEL : %d\r\n",RemoteDecoder_315.Channel);
  #endif
  goto END; 
  //------------      try to find PT226X  
  END:
  RemoteDecoder_315.Protocol =  Protocol;
  if(RemoteDecoder_315.Protocol!=RemoteDecoder_Protocol_Error)
  {
    RemoteDecoder_UserGetData315((char*)RemoteDecoder_315.Code,RemoteDecoder_315.Channel,RemoteDecoder_315.Protocol);
  }
  return;
}
//##########################################################################################
void  RemoteDecoder_ProcessBuffer315(void)
{
  if((RemoteDecoder_315.StartFrame==1) && ( RemoteDecoder_315.EndFrame==0) && (HAL_GetTick()-RemoteDecoder_315.StartFrameTime > _REMOTE_DECODER_FRAME_TIMEOUT_MS))
  {
    RemoteDecoder_315.StartFrame=0;
    RemoteDecoder_315.PinChangeCounter=0;
  }
  if(RemoteDecoder_315.EndFrame==1)
  {   
    RemoteDecoder_BufferToCode315();
    RemoteDecoder_315.EndFrame=0;
  }  
}
//##########################################################################################
#endif
//##########################################################################################
//##########################################################################################
//##########################################################################################
#if (_REMOTE_DECODER_ENABLE_433==1)
//##########################################################################################
void  RemoteDecoder_CallBack433(void)
{
  if( (RemoteDecoder_433.StartFrame==0) && (_REMOTE_DECODER_TIMER.Instance->CNT - RemoteDecoder_433.PinChangeLastTime)>_REMOTE_DECODER_START_FRAME_US)
  {
    RemoteDecoder_433.StartFrameTime = HAL_GetTick();
    RemoteDecoder_433.StartFrame=1;
    RemoteDecoder_433.EndFrame=0;
    RemoteDecoder_433.PinChangeCounter=0;
    memset(RemoteDecoder_433.Buffer,0,sizeof(RemoteDecoder_433.Buffer));    
  }
  else
  {   
    RemoteDecoder_433.Buffer[RemoteDecoder_433.PinChangeCounter]=_REMOTE_DECODER_TIMER.Instance->CNT - RemoteDecoder_433.PinChangeLastTime;
    if((RemoteDecoder_433.Buffer[RemoteDecoder_433.PinChangeCounter]>_REMOTE_DECODER_START_FRAME_US) && (RemoteDecoder_433.EndFrame==0)  && (RemoteDecoder_433.PinChangeCounter==49))
    {
      memcpy(RemoteDecoder_433.BufferProcess,RemoteDecoder_433.Buffer,_REMOTE_DECODER_BUFFER_SIZE*2);
      RemoteDecoder_433.EndFrame=1;
      RemoteDecoder_433.SyncTime =  RemoteDecoder_433.Buffer[RemoteDecoder_433.PinChangeCounter];
    }
    if(RemoteDecoder_433.PinChangeCounter<_REMOTE_DECODER_BUFFER_SIZE-1)
      RemoteDecoder_433.PinChangeCounter++;
  }
  
  RemoteDecoder_433.PinChangeLastTime = _REMOTE_DECODER_TIMER.Instance->CNT;
}
//##########################################################################################
void  RemoteDecoder_BufferToCode433(void)
{
  RemoteDecoder_Protocol_t  Protocol;
  uint8_t   PinChangeCounter = 0;
  uint32_t  BitTime=0;
  uint16_t  Bit10,Bit25,Bit75;
  uint16_t  Bit5,Bit47,Bit20,Bit30;
  while( RemoteDecoder_433.BufferProcess[PinChangeCounter] != 0)
  {
    PinChangeCounter++;
    if(PinChangeCounter == _REMOTE_DECODER_BUFFER_SIZE)
      break;          
  }
  if(PinChangeCounter<50)
    return;
  //++++++++++++      try to find EV1527  
  Protocol = RemoteDecoder_Protocol_EV1527;
  memset(RemoteDecoder_433.Code,0,sizeof(RemoteDecoder_433.Code));
  if(PinChangeCounter == 50)
  {
     for(uint8_t i=0 ; i<PinChangeCounter-2 ; i+=2)
    BitTime += RemoteDecoder_433.BufferProcess[i]+RemoteDecoder_433.BufferProcess[i+1];    
    RemoteDecoder_433.BitTime = BitTime/((PinChangeCounter-2)/2);
    Bit10 = (RemoteDecoder_433.BitTime/100)*10;
    Bit25 = (RemoteDecoder_433.BitTime/100)*25;
    Bit75 = (RemoteDecoder_433.BitTime/100)*75;  
    
    for(uint8_t i=0 ; i<40 ; i+=2)
    {
      if((RemoteDecoder_433.BufferProcess[i]>(Bit25-Bit10)) && (RemoteDecoder_433.BufferProcess[i]<(Bit25+Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]>(Bit75-Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]<(Bit75+Bit10)))     
        RemoteDecoder_433.Code[i/2] = RemoteDecoder_BitValue_Zero;        
      else if((RemoteDecoder_433.BufferProcess[i]>(Bit75-Bit10)) && (RemoteDecoder_433.BufferProcess[i]<(Bit75+Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]>(Bit25-Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]<(Bit25+Bit10)))     
        RemoteDecoder_433.Code[i/2] = RemoteDecoder_BitValue_One; 
      else
      {
        RemoteDecoder_433.Code[i/2] = RemoteDecoder_BitValue_Error;
        Protocol = RemoteDecoder_Protocol_Error;        
      }
    }
    if(Protocol == RemoteDecoder_Protocol_Error)
      goto TryToFindPT226X;
    uint8_t BitSel=0;
    RemoteDecoder_433.Channel=0;
    for(uint8_t i=40 ; i<48 ; i+=2)
    {
      if((RemoteDecoder_433.BufferProcess[i]>(Bit25-Bit10)) && (RemoteDecoder_433.BufferProcess[i]<(Bit25+Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]>(Bit75-Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]<(Bit75+Bit10)))     
        RemoteDecoder_433.Channel &=  ~(1<<BitSel);        
      else if((RemoteDecoder_433.BufferProcess[i]>(Bit75-Bit10)) && (RemoteDecoder_433.BufferProcess[i]<(Bit75+Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]>(Bit25-Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]<(Bit25+Bit10)))     
        RemoteDecoder_433.Channel |= (1<<BitSel);        
      BitSel++;
    }
  }
  
  #if (_REMOTE_DECODER_DEBUG==1)
  printf("\r\nDETECT EV1527 @433 MHZ,PULSE:%d,SYNC:%d us,BIT TIME:%d us\n",PinChangeCounter,RemoteDecoder_433.SyncTime,RemoteDecoder_433.BitTime);
  printf("CODE : ");
  for(uint8_t i=0;i<20 ; i++)
    printf("%c",RemoteDecoder_433.Code[i]);
  printf("\nCHANNEL : %d\r\n",RemoteDecoder_433.Channel);
  #endif
  goto END;
  //------------      try to find EV1527 
  
  //++++++++++++      try to find PT226X 
  TryToFindPT226X:
  Protocol = RemoteDecoder_Protocol_PT226X;
  memset(RemoteDecoder_433.Code,0,sizeof(RemoteDecoder_433.Code));
  if(PinChangeCounter == 50)
  {
    BitTime=0;
    for(uint8_t i=0 ; i<PinChangeCounter-2 ; i+=4)
      BitTime += RemoteDecoder_433.BufferProcess[i]+RemoteDecoder_433.BufferProcess[i+1]+RemoteDecoder_433.BufferProcess[i+2]+RemoteDecoder_433.BufferProcess[i+3];    
    RemoteDecoder_433.BitTime = BitTime/((PinChangeCounter-2)/4);
    
    Bit5 = (RemoteDecoder_433.BitTime/100)*5;
    Bit10 = (RemoteDecoder_433.BitTime/100)*10;
    Bit47 = (RemoteDecoder_433.BitTime/100)*47;
    Bit20 = (RemoteDecoder_433.BitTime/100)*20;  
    Bit30 = (RemoteDecoder_433.BitTime/100)*30;  
    
    for(uint8_t i=0 ; i<32 ; i+=4)
    {
      if((RemoteDecoder_433.BufferProcess[i]>(Bit20-Bit10)) && (RemoteDecoder_433.BufferProcess[i]<(Bit20+Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]>(Bit30-Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]<(Bit30+Bit10)) \
      && (RemoteDecoder_433.BufferProcess[i+2]>(Bit20-Bit10)) && (RemoteDecoder_433.BufferProcess[i+2]<(Bit20+Bit10)) && (RemoteDecoder_433.BufferProcess[i+3]>(Bit30-Bit10)) && (RemoteDecoder_433.BufferProcess[i+3]<(Bit30+Bit10)))
      {
          RemoteDecoder_433.Code[i/4] = RemoteDecoder_BitValue_Zero;        
      }
      else if((RemoteDecoder_433.BufferProcess[i]>(Bit47-Bit5)) && (RemoteDecoder_433.BufferProcess[i]<(Bit47+Bit5)) && (RemoteDecoder_433.BufferProcess[i+1]>(Bit5-Bit5/2)) && (RemoteDecoder_433.BufferProcess[i+1]<(Bit5+Bit5/2))\
      && (RemoteDecoder_433.BufferProcess[i+2]>(Bit47-Bit5)) && (RemoteDecoder_433.BufferProcess[i+2]<(Bit47+Bit5)) && (RemoteDecoder_433.BufferProcess[i+3]>(Bit5-Bit5/2)) && (RemoteDecoder_433.BufferProcess[i+3]<(Bit5+Bit5/2)))
      {
        RemoteDecoder_433.Code[i/4] = RemoteDecoder_BitValue_One;        
      }
      else if((RemoteDecoder_433.BufferProcess[i]>(Bit20-Bit10)) && (RemoteDecoder_433.BufferProcess[i]<(Bit20+Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]>(Bit30-Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]<(Bit30+Bit10))\
      && (RemoteDecoder_433.BufferProcess[i+2]>(Bit47-Bit5)) && (RemoteDecoder_433.BufferProcess[i+2]<(Bit47+Bit5)) && (RemoteDecoder_433.BufferProcess[i+3]>(Bit5-Bit5/2)) && (RemoteDecoder_433.BufferProcess[i+3]<(Bit5+Bit5/2)))
      {
          RemoteDecoder_433.Code[i/4] = RemoteDecoder_BitValue_Float;        
      }
      else
      {
        RemoteDecoder_433.Code[i/4] = RemoteDecoder_BitValue_Error; 
        Protocol = RemoteDecoder_Protocol_Error;
      }
    }
    if(Protocol == RemoteDecoder_Protocol_Error)
      goto END;
    uint8_t BitSel=0;
    RemoteDecoder_433.Channel=0;
    for(uint8_t i=32 ; i<48 ; i+=4)
    {
      if((RemoteDecoder_433.BufferProcess[i]>(Bit20-Bit10)) && (RemoteDecoder_433.BufferProcess[i]<(Bit20+Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]>(Bit30-Bit10)) && (RemoteDecoder_433.BufferProcess[i+1]<(Bit30+Bit10)))     
      {
        if((RemoteDecoder_433.BufferProcess[i+2]>(Bit20-Bit10)) && (RemoteDecoder_433.BufferProcess[i+2]<(Bit20+Bit10)) && (RemoteDecoder_433.BufferProcess[i+3]>(Bit30-Bit10)) && (RemoteDecoder_433.BufferProcess[i+3]<(Bit30+Bit10)))
        {
           RemoteDecoder_433.Channel &=  ~(1<<BitSel);        
        }
      }
      else if((RemoteDecoder_433.BufferProcess[i]>(Bit47-Bit5)) && (RemoteDecoder_433.BufferProcess[i]<(Bit47+Bit5)) && (RemoteDecoder_433.BufferProcess[i+1]>(Bit5-Bit5/2)) && (RemoteDecoder_433.BufferProcess[i+1]<(Bit5+Bit5/2)))     
      {
        if((RemoteDecoder_433.BufferProcess[i+2]>(Bit47-Bit5)) && (RemoteDecoder_433.BufferProcess[i+2]<(Bit47+Bit5)) && (RemoteDecoder_433.BufferProcess[i+3]>(Bit5-Bit5/2)) && (RemoteDecoder_433.BufferProcess[i+3]<(Bit5+Bit5/2)))
        {
          RemoteDecoder_433.Channel |= (1<<BitSel);          
        }
      }
      else
      {
        Protocol = RemoteDecoder_Protocol_Error;
      } 
      BitSel++;
    }
  }
  
  #if (_REMOTE_DECODER_DEBUG==1)
  printf("\r\nDETECT PT2264 @433 MHZ,PULSE:%d,SYNC:%d us,BIT TIME:%d us\n",PinChangeCounter,RemoteDecoder_433.SyncTime,RemoteDecoder_433.BitTime);
  printf("CODE : ");
  for(uint8_t i=0;i<8 ; i++)
    printf("%c",RemoteDecoder_433.Code[i]);
  printf("\nCHANNEL : %d\r\n",RemoteDecoder_433.Channel);
  #endif
  goto END; 
  //------------      try to find PT226X  
  END:
  RemoteDecoder_433.Protocol =  Protocol;
  if(RemoteDecoder_433.Protocol!=RemoteDecoder_Protocol_Error)
  {
    RemoteDecoder_UserGetData433((char*)RemoteDecoder_433.Code,RemoteDecoder_433.Channel,RemoteDecoder_433.Protocol);
  }
  return;
}
//##########################################################################################
void  RemoteDecoder_ProcessBuffer433(void)
{
  if((RemoteDecoder_433.StartFrame==1) && ( RemoteDecoder_433.EndFrame==0) && (HAL_GetTick()-RemoteDecoder_433.StartFrameTime > _REMOTE_DECODER_FRAME_TIMEOUT_MS))
  {
    RemoteDecoder_433.StartFrame=0;
    RemoteDecoder_433.PinChangeCounter=0;
  }
  if(RemoteDecoder_433.EndFrame==1)
  {   
    RemoteDecoder_BufferToCode433();
    RemoteDecoder_433.EndFrame=0;
  }  
}
//##########################################################################################
#endif
//##########################################################################################
//##########################################################################################
//##########################################################################################
void  RemoteDecoder_Process(void)
{
  #if (_REMOTE_DECODER_ENABLE_315==1)
  RemoteDecoder_ProcessBuffer315();
  #endif
  #if (_REMOTE_DECODER_ENABLE_433==1)
  RemoteDecoder_ProcessBuffer433();  
  #endif
}
//##########################################################################################
