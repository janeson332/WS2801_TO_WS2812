/**
  ******************************************************************************
  * @file    ws2812.c
  * @author  janeson332
  * @version V1.0
  * @date    18.07.2019
  * @brief   Simple WS2812 LED library for the STM32F10x
  *
  * This lib uses a double buffering method with cyclic reload of the
  * DMA to keep the RAM usage at a minimum level
  * Used Peripherals:  DMA1, TIM4 with output capture compare (PWM)
  *
  ******************************************************************************
*/

#include <stm32f10x.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <string.h>
#include "ws2812.h"


#define MAX_LED_NUM        (1000)  /**< maximum number of leds */
#define BYTE_PER_LED       (24)

#define WS2812_T1H         (57)
#define WS2812_T0H         (34)
#define RESET_PULSE_T      (60)    // in microsec
#define RESET_PULSE_CNTS   ((((RESET_PULSE_T*1.0)/(1.25))/BYTE_PER_LED)  )


typedef tWS2812_RGB tRGB_Buffer[MAX_LED_NUM];

static tRGB_Buffer rgbBuffer[2];
static uint8_t     currentRGBIdx = 0;
static uint8_t     nextRGBIdx    = 1;
static uint8_t     dmaBuffer[2*BYTE_PER_LED];

static uint32_t currentLEDIdx = 0;
static uint32_t lednumToTransmit= 0;

static uint32_t resetPulseIdx = 0;
static uint8_t sendResetPulse = 0;
static uint8_t const cResetPulseValue = 0;

static uint8_t transferComplete = 1;
static void (*transferCompleteCb)(void) = 0;

static void Init_DMA(void);
static void Init_TIM(void);
static void Start_DMA(void);
static void Setup_DMA_Buffer(uint8_t bufferPos);


void WS2812_Init(void){
	//enable clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	//set all values to "off"
	memset(rgbBuffer,0,sizeof(rgbBuffer));

	Init_TIM();
	Init_DMA();

	TIM_DMACmd(TIM4, TIM_DMA_CC1, ENABLE);
}


void WS2812_Refresh(uint32_t numLeds){
	if(numLeds > MAX_LED_NUM){
		lednumToTransmit = MAX_LED_NUM;
	}
	else{
		lednumToTransmit = numLeds;
	}

	while(transferComplete == 0);

	uint8_t tmp = currentRGBIdx;
	currentRGBIdx = nextRGBIdx;
	nextRGBIdx = tmp;

	currentLEDIdx = 0;
	sendResetPulse = 0;
	resetPulseIdx = 0;
	Setup_DMA_Buffer(0);
	Setup_DMA_Buffer(1);
	Start_DMA();

	memcpy(rgbBuffer[nextRGBIdx],rgbBuffer[currentRGBIdx],sizeof(tRGB_Buffer));
}

void WS2812_SetLed(uint32_t lednum, tWS2812_RGB const * color){
	if(lednum<MAX_LED_NUM){
		rgbBuffer[nextRGBIdx][lednum] = *color;
	}
}

void WS2812_SetAllLeds(uint32_t numLeds, tWS2812_RGB const * color){
	if(numLeds>MAX_LED_NUM){
		numLeds = MAX_LED_NUM;
	}
	for(uint32_t i = 0; i<numLeds;i++){
		WS2812_SetLed(i,color);
	}
}

tWS2812_RGB WS2812_GetLed(uint32_t lednum){
	tWS2812_RGB color;
	memset(&color,0,sizeof(tWS2812_RGB));

	if(lednum < MAX_LED_NUM){
		color = rgbBuffer[nextRGBIdx][lednum];
	}

	return color;
}

uint8_t WS2812_TransferComplete(void){
	NVIC_DisableIRQ(DMA1_Channel1_IRQn);
	uint8_t tmp = transferComplete;
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	if(tmp == 1){
		transferComplete = 0;
	}
	return tmp;
}

void WS2812_SetTransferCompleteCallback(void (*cb)(void)){
	transferCompleteCb = cb;
}


void DMA1_Channel1_IRQHandler(void){

	if(DMA_GetITStatus(DMA1_IT_HT1)){
		DMA_ClearITPendingBit(DMA1_IT_HT1);

		// initialize first half of dma buffer
		Setup_DMA_Buffer(0);

	}
	else if(DMA_GetITStatus(DMA1_IT_TC1)){
		DMA_ClearITPendingBit(DMA1_IT_TC1);

		if(resetPulseIdx < RESET_PULSE_CNTS){
			// initialize second half of dma buffer
			Setup_DMA_Buffer(1);
		}
		else{
			transferComplete = 1;
			DMA_Cmd(DMA1_Channel1,DISABLE);

			if(transferCompleteCb != 0){
				transferCompleteCb();
			}
		}
	}
}


static void Init_DMA(void){

	DMA_InitTypeDef dmaInit;
	dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&TIM4->CCR1;
	dmaInit.DMA_MemoryBaseAddr = (uint32_t)(dmaBuffer);
	dmaInit.DMA_DIR = DMA_DIR_PeripheralDST;
	dmaInit.DMA_BufferSize = sizeof(dmaBuffer);
	dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmaInit.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_HalfWord;
	dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dmaInit.DMA_Mode = DMA_Mode_Circular;
	dmaInit.DMA_Priority = DMA_Priority_High;
	dmaInit.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &dmaInit);

	// Initialize dma interrupt
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Channel1,DMA_IT_HT,ENABLE);
	NVIC_InitTypeDef dmaNVIC;
	dmaNVIC.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	dmaNVIC.NVIC_IRQChannelCmd = ENABLE;
	dmaNVIC.NVIC_IRQChannelPreemptionPriority = 0;
	dmaNVIC.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&dmaNVIC);
}

static void Init_TIM(void){
	//initialize PB6 as pwm pin

	GPIO_InitTypeDef pwmGPIO;
	pwmGPIO.GPIO_Pin = GPIO_Pin_6;
	pwmGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	pwmGPIO.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &pwmGPIO);

	TIM_TimeBaseInitTypeDef timInit;
	timInit.TIM_Prescaler = 0;
	timInit.TIM_Period = 89;
	timInit.TIM_ClockDivision = TIM_CKD_DIV1;
	timInit.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &timInit);

	TIM_OCInitTypeDef pwmInit;
	TIM_OCStructInit(&pwmInit);
	TIM_OC1PreloadConfig(TIM4, 8);
	pwmInit.TIM_OCMode = TIM_OCMode_PWM1;
	pwmInit.TIM_OutputState = TIM_OutputState_Enable;

	TIM_OC1Init(TIM4, &pwmInit);
}

static void Start_DMA(void){
	TIM_Cmd(TIM4,DISABLE);
	DMA_Cmd(DMA1_Channel1,DISABLE);
	TIM4->CNT = 0;
	DMA_SetCurrDataCounter(DMA1_Channel1,sizeof(dmaBuffer));
	TIM_Cmd(TIM4,ENABLE);
	DMA_Cmd(DMA1_Channel1,ENABLE);
}


static void Setup_DMA_Buffer(uint8_t bufferPos){
	uint8_t *dmaBufferPos = dmaBuffer;

	if(bufferPos == 1){ // second half dma buffer
		dmaBufferPos = dmaBuffer + sizeof(dmaBuffer)/2;
	}

	if(currentLEDIdx<lednumToTransmit){
		tWS2812_RGB color = rgbBuffer[currentRGBIdx][currentLEDIdx];

		for(uint8_t i = 0x80; i!=0; i>>=1){
			(*dmaBufferPos) = (color.g & i) ? WS2812_T1H : WS2812_T0H;
			dmaBufferPos++;
		}
		for(uint8_t i = 0x80; i!=0; i>>=1){
			(*dmaBufferPos) = (color.r & i) ? WS2812_T1H : WS2812_T0H;
			dmaBufferPos++;
		}
		for(uint8_t i = 0x80; i!=0; i>>=1){
			(*dmaBufferPos) = (color.b & i) ? WS2812_T1H : WS2812_T0H;
			dmaBufferPos++;
		}

		currentLEDIdx++;
	}
	else{ // initialize reset pulse
		memset(dmaBufferPos,cResetPulseValue,BYTE_PER_LED);
		++resetPulseIdx;
	}
}
