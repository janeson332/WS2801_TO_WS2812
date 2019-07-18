/**
  ******************************************************************************
  * @file    stm32f10x_systick.h
  * @author  janeson332
  * @version V1.0
  * @date    03.12.2017
  * @brief   Initializes the systick-timer with 1ms intervall and provides millis
  ******************************************************************************
  */


#ifndef STM32F10X_SYSTICK_H_INCLUDED
#define STM32F10X_SYSTICK_H_INCLUDED
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f10x.h"


/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief initialize systick-timer with 1 ms
  */
void Systick_Init(void);

/**
  * @brief returns the amount of millis since programm started
  * @return millies since start (overflow can occur)
  */
uint32_t Systick_GetMillis(void);






#endif
