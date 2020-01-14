# Stm32 Remote decoder (EV1527,PT2262,PT2264 and ...)

* http://www.github.com/NimaLTD   
* https://www.instagram.com/github.nimaltd/   
* https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw   

This is the ASK RF remote decoder for STM32 HAL Library  

How to use this Library:
* Select "General peripheral Initalizion as a pair of '.c/.h' file per peripheral" on project settings.   
* Enable a Timer with 10 us tick. (EX: Set Prescaler to "480-1" for 48MHZ Timer).  
* Enable a GPIO as Interrupt pin on rising and falling edge.
* Include Header and source into your project.   
* Config your "RemoteDecoderConf.h".
* Create a struct(EX: RemoteDecoder_t rf).
* Add RemoteDecoder_callBack(&rf) on external interrupt routin.
* RemoteDecoder_init(&rf,GPIO,PIN) on your app.
* Put RemoteDecoder_loop(&rf) function in your loop.
* Read data after RemoteDecoder_available returned true.
* Read data by RemoteDecoder_read(&rf,code,&len,&time);

  Example:

```
#include "remotedecoder.h"
#include <stdio.h>
.
.
RemoteDecoder_t	rf;
uint8_t	RemoteData[3];
uint8_t RemoteDataLen;
.
.
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_11)
		RemoteDecoder_callBack(&rf);  
}
.
.
int main()
{
	.
	.
	.
	RemoteDecoder_init(&rf,GPIOA,GPIO_PIN_11);
	.
	.
	.
	while(1)
	{
		.
		.
		.
		RemoteDecoder_loop(&rf);
		if(RemoteDecoder_available(&rf) == true)
		{
			if(RemoteDecoder_read(&rf, RemoteData, &RemoteDataLen,NULL))
				printf("(NEW KEY) CODE %d , 0x%X , 0x%X , 0x%X \r\n",RemoteDataLen,RemoteData[0],RemoteData[1],RemoteData[2]);
			else
				printf("(HOLD KEY) CODE %d , 0x%X , 0x%X , 0x%X \r\n",RemoteDataLen,RemoteData[0],RemoteData[1],RemoteData[2]);
		}
	}
}
```
