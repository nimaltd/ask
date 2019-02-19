# Stm32 Remote decoder (EV1527,PT2262,PT2264 and ...)
<br />
I hope use it and enjoy.
<br />
I use Stm32f03k6 and Keil Compiler and Stm32CubeMX wizard.
 <br />
Please Do This ...
<br />
<br />
1) Enable a Timer with 10 us tick.  
<br />
2) enable Interrupt pin on rising and falling edge.
<br />
3) Select "General peripheral Initalizion as a pair of '.c/.h' file per peripheral" on project settings.
<br />
4) Config your RemoteDecoderConfig.h
<br />
5) Add RemoteDetector_PinChangeCallBack() on external interrupt routin.
<br />
6) Call  RemoteDecoder_Init() on your app.
<br />
<br />
Example:
<br />

```
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  osDelay(100);
  RemoteDecoder_Init(osPriorityRealtime);
  for(;;)
  {	  
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}
```
<br />
Set IRQ:
<br />

```
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */
  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */
  RemoteDetector_PinChangeCallBack();
  /* USER CODE END EXTI4_15_IRQn 1 */
}
```
