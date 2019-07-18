/**
  ******************************************************************************
  * @file    Ringbuffer.c 
  * @author  janeson332
  * @version V1.0
  * @date    05.01.2018
  * @brief   Ringbuffer (circular Buffer) Functions with int32_t
  ******************************************************************************
  */

#ifndef RINGBUFFER_H_INCLUDED
#define RINGBUFFER_H_INCLUDED

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported typedef ------------------------------------------------------------*/
typedef struct{
    void *buffer;     /**< pointer on buffer to operate */
    void *buffer_end; /**< position of the last element of the rb */
    uint32_t capacity;  /**< capacity of the ringbuffer */
    uint32_t count;     /**< actual stored number of elements */
    uint32_t sz;        /**< size of each item in the buffer */
    void *head;       /**< position of the first element of the rb */
    void *tail;       /**< position of the last element of the rb */
}tCircularBuffer; /**< ringbuffer structure with all the infos */
    
/* Exported define ------------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------*/
/* Exported variables ---------------------------------------------------------*/
/* Exported function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

/**
  * @brief initializes the ringbuffer
  * @param rb: Pointer on RingbufferTypeDef (holds rb infos)
  * @param buffer: pointer on the buffer to operate
  * @param capacity: capacity of the given buffer
  * @returns None
  */
void Ringbuffer_Init(tCircularBuffer* rb, void* buffer, uint32_t capacity, uint32_t size);

/** 
  * @brief clears the ringbuffer
  * @param rb: ringbuffer-structure to clear
  * @returns None
  */
void Ringbuffer_Clear(tCircularBuffer* rb);

/**
  * @brief pushes the given value on the ringbuffer (if rb is full first val gets overridden)
  * @param rb: ringbuffer to operate
  * @param value: value to push
  */
void Ringbuffer_Push(tCircularBuffer* rb, void const* value);

/**
  * @brief pops the first value of the ringbuffer (element gets removed)
  * @param rb: ringbuffer to operate
  * @param pVal: pointer on value to store the popped val
  * @returns true if something got popped, else false
  */
uint32_t Ringbuffer_Pop(tCircularBuffer* rb, void *pVal);

/**
  * @brief returns true if buffer is empty, else false
  * @param rb: ringbuffer to operate
  * @returns true if empty else false
  */
uint32_t Ringbuffer_IsEmpty(tCircularBuffer const * const rb);





#endif
