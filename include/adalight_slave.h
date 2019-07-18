/**
  ******************************************************************************
  * @file    adalight.h
  * @author  janeson332
  * @version V1.0
  * @date    18.07.2019
  * @brief   Simple Slave for the Adalight UART protocol
  *
  * Used Peripherals:  UART1
  * Output Pin: PA9  ... UART_TX
  * Input Pin:  PA10 ... UART_RX
  ******************************************************************************
*/

#ifndef ADALIGHT_SLAVE_H_INCLUDED
#define ADALIGHT_SLAVE_H_INCLUDED

#include <stdint.h>

typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}tAdalight_RGB;

/**
 * @brief initializes the peripherals
 */
void    Adalight_Slave_Init(void);

/**
 * @brief set a callback function, which gets called, when a color gets received
 * @param cb: function pointer
 */
void    Adalight_Slave_SetColorReceivedCallback(void (*cb)(uint32_t ledNum, tAdalight_RGB color));

/**
 * @brief set a callback function, which gets called, when a the whole frame has been received
 * @param cb: function pointer
 */
void    Adalight_Slave_SetFrameCompleteCallback(void(*cb)(void));

/**
 * @brief returns 1 if whole frame has been received
 */
uint8_t Adalight_Slave_FrameComplete(void);



#endif
