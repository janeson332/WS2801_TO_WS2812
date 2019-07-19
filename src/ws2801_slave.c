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

#include <string.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_exti.h"

#include "ws2801_slave.h"
#include "stm32f10x_systick.h"

static void (*colorCompleteCb)(uint32_t ledNum, tWS2801_RGB color) = 0;
static void (*frameCompleteCb)(void) = 0;
static uint32_t lednum = 0;
static uint8_t cnt = 0;
static uint8_t frameComplete = 0;
static uint32_t receivedLedNum = 0;


void WS2801_Slave_Init(void){

	// initialize spi
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2ENR_AFIOEN, ENABLE);
	GPIO_InitTypeDef spiGPIO; GPIO_StructInit(&spiGPIO);
	spiGPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	spiGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	spiGPIO.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5 | GPIO_Pin_4;  // pin7... mosi, pin5...sck, pin4.. nss
	GPIO_Init(GPIOA,&spiGPIO);

	//spi init
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_SPI1EN,ENABLE);
	SPI_InitTypeDef spiInit; SPI_StructInit(&spiInit);
	spiInit.SPI_DataSize = SPI_DataSize_8b;
	spiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
	spiInit.SPI_NSS = SPI_NSS_Hard;
	spiInit.SPI_Mode = SPI_Mode_Slave;
	spiInit.SPI_CPHA = SPI_CPHA_1Edge;
	spiInit.SPI_CPOL = SPI_CPOL_Low;
	spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_Init(SPI1,&spiInit);
	SPI1->CR1 &= ~SPI_CR1_CRCEN;
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE);

	NVIC_InitTypeDef spiInt;
	spiInt.NVIC_IRQChannel = SPI1_IRQn;
	spiInt.NVIC_IRQChannelCmd = ENABLE;
	spiInt.NVIC_IRQChannelPreemptionPriority = 0;
	spiInt.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&spiInt);

	//enable external interrupt on rising flag of nss (for frame complete detection)
	EXTI_InitTypeDef nssInt;
	nssInt.EXTI_Line = EXTI_Line4;
	nssInt.EXTI_LineCmd = ENABLE;
	nssInt.EXTI_Mode = EXTI_Mode_Interrupt;
	nssInt.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&nssInt);
	AFIO->EXTICR[2] |= AFIO_EXTICR2_EXTI4_PA;

	NVIC_EnableIRQ(EXTI4_IRQn);

	SPI_Cmd(SPI1,ENABLE);
}

void WS2801_Slave_SetColorReceivedCallback(void (*cb)(uint32_t ledNum, tWS2801_RGB color)){
	colorCompleteCb = cb;
}
void WS2801_Slave_SetFrameCompleteCallback(void(*cb)(void)){
	frameCompleteCb = cb;
}

uint8_t WS2801_Slave_FrameComplete(void){
	NVIC_DisableIRQ(EXTI4_IRQn);
	uint8_t tmp = frameComplete;
	NVIC_EnableIRQ(EXTI4_IRQn);
	if(tmp==1){
		frameComplete=0;
	}
	return tmp;
}

uint32_t WS2801_Slave_GetLastReceivedLedNumber(void){
	return receivedLedNum;
}

static void Spi_Handler(uint8_t ch){
	static uint32_t last = 0;
	uint32_t now = Systick_GetMillis();
	static tWS2801_RGB color;

	if(now-last>10){
		lednum = 0;
		cnt = 0;
		memset(&color,0,sizeof(tWS2801_RGB));
	}

	if(cnt==0){
		color.g = ch;
		cnt++;
	}
	else if(cnt==1){
		color.r = ch;
		cnt++;
	}
	else if(cnt==2){
		color.b = ch;
		cnt = 0;
		if(colorCompleteCb != 0){
			colorCompleteCb(lednum,color);
		}
		memset(&color,0,sizeof(tWS2801_RGB));
		lednum++;
	}

	last = now;
}

void SPI1_IRQHandler(void){
	if(SPI_I2S_GetITStatus(SPI1,SPI_I2S_IT_RXNE)){
		uint8_t recv = SPI1->DR;
		Spi_Handler(recv);
		SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
	}
}

void EXTI4_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line4)){
		EXTI_ClearITPendingBit(EXTI_Line4);
		frameComplete = 1;
		cnt = 0;
		receivedLedNum = lednum;
		lednum = 0;

		if(frameCompleteCb != 0){
			frameCompleteCb();
		}
	}
}
