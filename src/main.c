/**
  ******************************************************************************
  * @file    main.c
  * @author  janeson332
  * @version V1.0
  * @date    18.07.2019
  * @brief   Default main function.
  ******************************************************************************
*/

#include <string.h>
#include "stm32f10x.h"
#include "ws2812.h"
#include "stm32f10x_uart1.h"
#include "stm32f10x_systick.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "ws2801_slave.h"

// Callback function for setting the leds
void setLed(uint32_t lednum, tWS2801_RGB color){
	tWS2812_RGB rgb;
	memcpy(&rgb,&color,sizeof(color));
	WS2812_SetLed(lednum,&rgb);
}

// Callback function for refreshing the leds
void refresh(void){
	WS2812_Refresh(200);
}

int main(void){

	Systick_Init();

	WS2812_Init();

	WS2801_Slave_Init();
	WS2801_Slave_SetColorReceivedCallback(setLed);
    WS2801_Slave_SetFrameCompleteCallback(refresh);

	while(1){

	}
}
