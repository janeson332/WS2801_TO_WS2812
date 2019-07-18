/**
  ******************************************************************************
  * @file    adalight.c
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

#include "adalight_slave.h"
#include "stm32f10x_uart1.h"
#include "stm32f10x_systick.h"
#include <string.h>

static void (*colorCompleteCb)(uint32_t ledNum, tAdalight_RGB color) = 0;
static void (*frameCompleteCb)(void) = 0;

static uint8_t  frameComplete = 0;
static uint32_t ledNum = 0;
static uint32_t packetLength = 0;
static tAdalight_RGB color;

static void AdalightParser(uint8_t ch);

void    Adalight_Slave_Init(void){
	//UART1_init(0x90);   // baud 500000
	//UART1_init(0x139);  // baud 100000

	UART1_init(UART1_BAUD_115200);
	UART1_SetReceiveParser(AdalightParser);
	UART1_SendString("Ada");
}

void    Adalight_Slave_SetColorReceivedCallback(void (*cb)(uint32_t ledNum, tAdalight_RGB color)){
	colorCompleteCb = cb;
}

void    Adalight_Slave_SetFrameCompleteCallback(void(*cb)(void)){
	frameCompleteCb = cb;
}

uint8_t Adalight_Slave_FrameComplete(void){
	return frameComplete;
}



void AdalightParser(uint8_t ch){
	typedef enum{
		Header,LedG,LedR,LedB,
	}tReceiveState;
	static tReceiveState state = Header;
	static uint32_t last = 0;


	uint32_t now = Systick_GetMillis();


	if(now-last > 10){
		state = Header;
		ledNum  = 0;
		memset(&color,0,sizeof(tAdalight_RGB));
	}

	switch(state){
		case Header:{
			static uint8_t cnt = 0;

			if((cnt == 0) && (ch == 'A')){ cnt++; }
			else if((cnt == 1) && (ch == 'd')){ cnt++; }
			else if((cnt == 2) && (ch == 'a')){ cnt++; }
			else if(cnt == 3){
				packetLength = ((uint32_t)(ch)) << 8; cnt++;
			}
			else if(cnt == 4){
				packetLength |= (uint32_t)(ch); cnt++;
			}
			else if(cnt == 5){
				if(ch == (((uint8_t)(packetLength>>8)) ^ ((uint8_t)(packetLength)) ^ 0x55)){
					state = LedG;
					ledNum = 0;
				}
				else{
					cnt = 0;
					packetLength = 0;
				}
			}
			else{
				cnt = 0;
				packetLength = 0;
			}
		}break;
		case LedG:{
				color.g = ch;
				state = LedR;
		}break;
		case LedR:{
				color.r = ch;
				state = LedB;
		}break;
		case LedB:{
				color.b = ch;

				if(colorCompleteCb!=0){
					colorCompleteCb(ledNum,color);
				}

				memset(&color,0,sizeof(tAdalight_RGB));
				ledNum++;

				if(ledNum < packetLength){
					state = LedG;
				}
				else{
					state = Header;

					if(frameCompleteCb != 0){
						frameCompleteCb();
					}
				}

		}break;
		default: state = Header; break;
	}

	last = now;
}
