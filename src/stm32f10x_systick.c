/**
  ******************************************************************************
  * @file    stm32f10x_systick.h
  * @author  janeson332
  * @version V1.0
  * @date    03.12.2017
  * @brief   Initializes the systick-timer with 1ms intervall and provides millis
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t ticks = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief Increment Ticks
  */
static void IncTick(void){
    ++ticks;
}

/**
  * @brief initialize systick-timer with 1 ms
  */
void Systick_Init(void){
    SysTick_Config(SystemCoreClock / 1000);
}

/**
  * @brief Systick handler
  */
void SysTick_Handler(void){
    IncTick();
}

/**
  * @brief returns the amount of millis since programm started
  * @return millies since start (overflow can occur)
  */
uint32_t Systick_GetMillis(void){
    uint32_t retVal;
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
    retVal = ticks;
    SysTick->CTRL |= SysTick_CTRL_ENABLE;
    
    return retVal;
}
