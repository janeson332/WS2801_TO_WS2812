/**
  ******************************************************************************
  * @file    ws2812.h
  * @author  janeson332
  * @version V1.0
  * @date    18.07.2019
  * @brief   Simple WS2812 LED library for the STM32F10x
  *
  * This lib uses a double buffering method with cyclic reload of the
  * DMA to keep the RAM usage at a minimum level
  * Used Peripherals:  DMA1, TIM4 with output capture compare (PWM)
  * Output Pin: PB6
  ******************************************************************************
*/

#ifndef WS2812_H_INCLUDED
#define WS2812_H_INCLUDED


typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}tWS2812_RGB;

/**
 * @brief initializes the peripherals and the lib
 */
void WS2812_Init	(void);

/**
 * @brief Sets the led on lednum (first one starts with 0) with the given color
 * @param lednum: led to set
 * @param color:  color to set
 */
void WS2812_SetLed(uint32_t lednum, tWS2812_RGB const * color);

/**
 * @brief Sets the first n leds with the given color
 * @param numLeds: how much leds should get set
 * @param color:  color to set
 */
void WS2812_SetAllLeds(uint32_t numLeds, tWS2812_RGB const * color);

/**
 * @brief Starts transmitting the led data to the peripheral (this function can block if, old transmission hasn't completed)
 * @param numLeds: how much leds should get refreshed
 */
void WS2812_Refresh(uint32_t numLeds);

/**
 * @brief Returns the color of the requested led
 * @param lednum: led number
 */
tWS2812_RGB WS2812_GetLed(uint32_t lednum);

/**
 * @brief Returns 1 if Transfer has been completed
 */
uint8_t WS2812_TransferComplete(void);

/**
 * @brief Sets a callback function which gets called, when the transfer has been completed (get called in an ISR)
 */
void WS2812_SetTransferCompleteCallback(void (*cb)(void));


#endif
