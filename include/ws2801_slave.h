/**
  ******************************************************************************
  * @file    ws2801_slave.h
  * @author  janeson332
  * @version V1.0
  * @date    18.07.2019
  * @brief   Simple Slave for the WS2801 led spi protocol for the STM32F10x
  *
  * Used Peripherals:  SPI1, EXTI Line 4
  * Input Pin: PA7 ... MOSI
  *            PA5 ... SCK
  *            PA4 ... NSS (slave select, chip select)
  ******************************************************************************
*/

#ifndef WS2801_SLAVE_H_INCLUDED
#define WS2801_SLAVE_H_INCLUDED


typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}tWS2801_RGB;

/**
 * @brief initializes the peripherals
 */
void    WS2801_Slave_Init(void);

/**
 * @brief set a callback function, which gets called, when a color gets received
 * @param cb: function pointer
 */
void    WS2801_Slave_SetColorReceivedCallback(void (*cb)(uint32_t ledNum, tWS2801_RGB color));

/**
 * @brief set a callback function, which gets called, when a the whole frame has been received
 * @param cb: function pointer
 */
void    WS2801_Slave_SetFrameCompleteCallback(void(*cb)(void));

/**
 * @brief returns 1 if whole frame has been received
 */
uint8_t WS2801_Slave_FrameComplete(void);








#endif
