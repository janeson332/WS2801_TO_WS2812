/**
  ******************************************************************************
  * @file    stm32f10x_uart1.h
  * @author  janeson332
  * @version V1.0
  * @date    03.12.2017
  * @brief   functions to operate with the usart1 as uart
  ******************************************************************************
  */

#ifndef STM32F10X_UART1_H_INCLUDED
#define STM32F10X_UART1_H_INCLUDED

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdint.h>

/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define UART1_BAUD_115200   0x271
#define UART1_BAUD_9600     46875
#define UART_BAUD_1000000   0x48

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief initializes the uart1
  */
void UART1_init(uint32_t baud);

/**
  * @brief sends 1 char
  * @param ch: char to send
  */
void UART1_SendChar(uint8_t ch);

/**
  * @brief sends a string (without \0)
  * @param str: string to send
  */
void UART1_SendString(char const *str);

/**
  * @brief sets a receiver function
  * @param ParserFunc: function pointer to call after interrupt
  */
void UART1_SetReceiveParser(void (*ParserFunc)(uint8_t ch));

#endif

