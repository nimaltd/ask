# Stm32 Remote decoder (EV1527 and PT2264)
<br />
I hope use it and enjoy.
<br />
I use Stm32f103C8 and Keil Compiler and Stm32CubeMX wizard.
 <br />
Please Do This ...
<br />
<br />
1) Enable a Timer with 1 us tick.  
<br />
2) enable Interrupt pin on rising and falling edge.
<br />
3) Select "General peripheral Initalizion as a pair of '.c/.h' file per peripheral" on project settings.
<br />
4) Config your RemoteDecoderConfig.h
<br />
5) Add RemoteDecoder_CallBack315() or RemoteDecoder_CallBack433 or both on external interrupt routin.
<br />
6) Call  RemoteDecoder_Init() on your app.
<br />
7) Call RemoteDecoder_Process() in a loop.
