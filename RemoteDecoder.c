
#include <string.h>
#include "RemoteDecoder.h"

osThreadId RemoteDecoderTaskHandle;
RemoteDecoder_t RemoteDecoder;

void 	StartRemoteDecoderTask(void const * argument);
//##############################################################################################
uint8_t	RemoteDecoder_CalcDuty(uint8_t	TimH,uint8_t	TimL)
{	
	return (uint8_t)(((float)TimH / (float)(TimH+TimL))*100.0f);	
}
//##############################################################################################
bool	RemoteDecoder_CheckTolerance(uint8_t Data,uint8_t	Refrence)
{
	if((Data<Refrence+_REMOTE_DECODER_TOLERANCE_PERCENT) && (Data>Refrence-_REMOTE_DECODER_TOLERANCE_PERCENT))
		return true;
	else
		return false;
}
//##############################################################################################	
bool	RemoteDecoder_CheckBitLen(uint8_t	TimH,uint8_t	TimL)	// check This Bit len by first bit
{
	uint16_t FirstBitLen	= RemoteDecoder.Buff[1]+RemoteDecoder.Buff[2];
	if((TimH+TimL<=FirstBitLen+(FirstBitLen*_REMOTE_DECODER_TOLERANCE_PERCENT/100)) && (TimH+TimL>=FirstBitLen - (FirstBitLen*_REMOTE_DECODER_TOLERANCE_PERCENT/100)))
		return true;
	else
		return false;
}
//##############################################################################################
bool	RemoteDecoder_Decode(uint8_t SelectDecodedData)
{
	int8_t		BitCnt=7;
	uint8_t		ByteCnt=0;
	if(SelectDecodedData<2)
		SelectDecodedData=0;
	else
		SelectDecodedData=1;
	if(RemoteDecoder.Index<50)
		goto ERROR;
	memset(RemoteDecoder.DecodedData[SelectDecodedData],0,sizeof(RemoteDecoder.DecodedData[SelectDecodedData]));
	RemoteDecoder.DecodedDataLen[SelectDecodedData]=0;
	for(uint16_t i=0 ; i<RemoteDecoder.Index-2 ; i+=2)
	{
		if(RemoteDecoder_CheckBitLen(RemoteDecoder.Buff[i+1],RemoteDecoder.Buff[i+2])==false)
		{
			goto ERROR;
		}
		uint8_t D=RemoteDecoder_CalcDuty(RemoteDecoder.Buff[i+1],RemoteDecoder.Buff[i+2]);
		if(RemoteDecoder_CheckTolerance(D,_REMOTE_DECODER_BIT_0_DUTY)==true) // BIT 0
		{
			
		}
		else if(RemoteDecoder_CheckTolerance(D,_REMOTE_DECODER_BIT_1_DUTY)==true) // BIT 1
		{
			RemoteDecoder.DecodedData[SelectDecodedData][ByteCnt] |=  1<<BitCnt;
		}
		else	// BIT error
			goto ERROR;
		RemoteDecoder.DecodedDataLen[SelectDecodedData]++;
		BitCnt--;
		if(BitCnt==-1)
		{
			BitCnt=7;
			ByteCnt++;
		}
	}
	return true;		
	ERROR:
	memset(RemoteDecoder.DecodedData[SelectDecodedData],0,sizeof(RemoteDecoder.DecodedData[SelectDecodedData]));
	RemoteDecoder.DecodedDataLen[SelectDecodedData]=0;
	return false;				
}
//##############################################################################################
void	RemoteDetector_PinChangeCallBack(void)
{
	RemoteDecoder.LastPinChangeInSystick=HAL_GetTick();
	if(RemoteDecoder.AllowToGetNewSample==0)
		return;
	if(RemoteDecoder.Index==0)
	{
		_REMOTE_DECODER_TIM.Instance->CNT = 0;
		RemoteDecoder.LastPinChangeInTimer = 0;
		RemoteDecoder.Buff[RemoteDecoder.Index] = _REMOTE_DECODER_TIM.Instance->CNT;
	}
	else
	{
		RemoteDecoder.Buff[RemoteDecoder.Index] = _REMOTE_DECODER_TIM.Instance->CNT-RemoteDecoder.LastPinChangeInTimer;			
		RemoteDecoder.LastPinChangeInTimer = _REMOTE_DECODER_TIM.Instance->CNT;
	}
	if(RemoteDecoder.Index < _REMOTE_DECODER_BUFFER_SIZE-1)
		RemoteDecoder.Index++;		
}
//##############################################################################################
bool RemoteDecoder_Init(osPriority Priority)
{	
	osThreadDef(RemoteDecoderTask, StartRemoteDecoderTask, Priority, 0, 128);
  RemoteDecoderTaskHandle = osThreadCreate(osThread(RemoteDecoderTask), NULL);
	if(RemoteDecoderTaskHandle==NULL)
		return false;
	else
		return true;
}
//##############################################################################################
void StartRemoteDecoderTask(void const * argument)
{
	uint8_t   RemoteDetect=0;
	_REMOTE_DECODER_TIM.Instance->ARR = 0xFFFF;
	HAL_TIM_Base_Start(&_REMOTE_DECODER_TIM);
	while(1)
	{		
		if(HAL_GetTick()-RemoteDecoder.LastPinChangeInSystick > _REMOTE_DECODER_MINIMUM_STAY_IN_LOW_STATE_FOR_DETECT_SIGNAL_IN_MS)
		{			
			if(HAL_GPIO_ReadPin(_REMOTE_DETECTOR_GPIO,_REMOTE_DETECTOR_PIN)==GPIO_PIN_RESET)
			{
				if(RemoteDecoder.AllowToGetNewSample==0)
				{
					memset(RemoteDecoder.Buff,0,_REMOTE_DECODER_BUFFER_SIZE);
					RemoteDecoder.Index=0;
					RemoteDecoder.AllowToGetNewSample=1;
				}
				if(RemoteDecoder.Index>0)
				{
					RemoteDecoder.AllowToGetNewSample=0;
					//+++	Got a frame
					if(RemoteDecoder_Decode(RemoteDetect)==true)
					{
						if(RemoteDetect==2) // check 2 times for ensure
						{
							RemoteDetect=3;
							if( memcmp(RemoteDecoder.DecodedData[0],RemoteDecoder.DecodedData[1],sizeof(RemoteDecoder.DecodedData[0]))==0)
							{
								RemoteDecoder_User_Detect(RemoteDecoder.DecodedData[0],RemoteDecoder.DecodedDataLen[0],RemoteDecoder.Buff,RemoteDecoder.Index);
								osDelay(100);
							}							
						}
						if(RemoteDetect==1)				
						{
							RemoteDetect=2;	
						}
						if(RemoteDetect==0)				
						{
							RemoteDetect=1;
						}						
					}
					else						
					{
						RemoteDetect=0;				
					}
					//---	Got a frame
				}				
			}
		}					
		osDelay(1);
	}	
}
//##############################################################################################



