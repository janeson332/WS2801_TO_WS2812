/**
  ******************************************************************************
  * @file    stm32f10x_uart1.c
  * @author  janeson332
  * @version V1.0
  * @date    03.12.2017
  * @brief   functions to operate with the usart1 as uart
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_uart1.h"
#include "Ringbuffer.h"
#include <string.h>
#include <assert.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TX_BUFFER_SIZE 256
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t tx_buffer[TX_BUFFER_SIZE] = {0};
static tCircularBuffer txBufferStruct;

/* Private function prototypes -----------------------------------------------*/
static void DummyFunc(uint8_t ch){return;}
static void (*RCParserFunc)(uint8_t ch) = &DummyFunc;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief interrupt handler for the usart1
  */
void USART1_IRQHandler(void){
    uint16_t txChar = 0;
    //transmit
    if((USART1->SR & USART_SR_TXE) && !Ringbuffer_IsEmpty(&txBufferStruct)){
        if (Ringbuffer_Pop(&txBufferStruct,&txChar)){
            USART1->DR = txChar;
        }
    }
    
    if(Ringbuffer_IsEmpty(&txBufferStruct)){
        USART1->CR1 &= ~USART_CR1_TXEIE;
    }

    //receive
    if((USART1->SR & USART_SR_RXNE)){
        char ch = USART1->DR;
        (*RCParserFunc)(ch);
    }
}

/**
  * @brief initializes the uart1
  */
void UART1_init(uint32_t baud){
    Ringbuffer_Init(&txBufferStruct,tx_buffer,TX_BUFFER_SIZE,sizeof(uint8_t));
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    
    //set PA9 (tx) as push-pull and PA10 as floating input
    GPIOA->CRH &= ~GPIO_CRH_MODE9;
    GPIOA->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF9;
    GPIOA->CRH |= GPIO_CRH_CNF9_1;
    
    GPIOA->CRH &= ~GPIO_CRH_MODE10;
    GPIOA->CRH &= ~GPIO_CRH_CNF10;
    GPIOA->CRH |= GPIO_CRH_CNF10_0;
    
    //initialize usart
    
    USART1->BRR = baud;
    
    USART1->CR1 &= ~USART_CR1_M; // word lenght 8
    USART1->CR1 &= ~USART_CR1_PCE; //parity disabled
    USART1->CR2 &= ~USART_CR2_STOP; // enable 1 Stopp -bit
    
    //USART1->CR1 |= USART_CR1_TXEIE; // enable TDR empty interrupt
    USART1->CR1 |= USART_CR1_RXNEIE; // enable RDR data available interrupt
    
    USART1->CR1 |= USART_CR1_TE; //transmitter enable
    USART1->CR1 |= USART_CR1_RE; //receiver enable
    
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn,0);
    
    USART1->CR1 |= USART_CR1_UE; // enables usart1
}

/**
  * @brief sends 1 char
  * @param ch: char to send
  */
void UART1_SendChar(uint8_t ch){
    USART1->CR1 |= USART_CR1_TXEIE;
    
    if((USART1->SR & USART_SR_TXE) == USART_SR_TXE && Ringbuffer_IsEmpty(&txBufferStruct)){
        USART1->DR = (uint16_t)(ch);
    }
    else{
        __disable_irq();
        Ringbuffer_Push(&txBufferStruct,&ch);
        __enable_irq();
    }
}

/**
  * @brief sends a string (without \0)
  * @param str: string to send
  */
void UART1_SendString(char const *str){
    assert(str != 0);
    uint32_t strLength = strlen(str);
    
    for(uint32_t i = 0; i<strLength;++i){
        UART1_SendChar(str[i]);
    }
}

/**
  * @brief sets a receiver function
  * @param ParserFunc: function pointer to call after interrupt
  */
void UART1_SetReceiveParser(void (*ParserFunc)(uint8_t ch)){
    if(ParserFunc == 0){
        return;
    }
    RCParserFunc=ParserFunc;
}

