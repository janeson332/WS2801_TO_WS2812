/**
  ******************************************************************************
  * @file    Ringbuffer.c 
  * @author  janeson332
  * @version V1.0
  * @date    05.01.2018
  * @brief   Ringbuffer (circular Buffer) Functions with int32_t
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "Ringbuffer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief initializes the ringbuffer
  * @param rb: Pointer on RingbufferTypeDef (holds rb infos)
  * @param buffer: pointer on the buffer to operate
  * @param capacity: capacity of the given buffer
  * @returns None
  */
void Ringbuffer_Init(tCircularBuffer* rb, void* buffer, uint32_t capacity, uint32_t size){
    memset(rb,0,sizeof(tCircularBuffer));
    memset(buffer,0,size*capacity);
    rb->buffer = buffer;
    rb->capacity = capacity;
    rb->sz = size;
    rb->buffer_end = (void*)((uint32_t)buffer + (size*capacity));
    rb->head = buffer;
    rb->tail = buffer;
}

/** 
  * @brief clears the ringbuffer
  * @param rb: ringbuffer-structure to clear
  * @returns None
  */
void Ringbuffer_Clear(tCircularBuffer* rb){
    memset(rb->buffer,0,sizeof(int32_t)*rb->capacity);
    rb->count = 0;
    rb->head = rb->buffer;
    rb->tail = rb->buffer;
}

/**
  * @brief pushes the given value on the ringbuffer (if rb is full first val gets overridden)
  * @param rb: ringbuffer to operate
  * @param value: value to push
  */
void Ringbuffer_Push(tCircularBuffer* rb, void const* value){
    if(rb->count == rb->capacity){
        // handle error, do nothing
        return;
    }
    memcpy(rb->head, value, rb->sz);
    rb->head = (char*)rb->head + rb->sz;
    if(rb->head == rb->buffer_end)
        rb->head = rb->buffer;
    rb->count++;
}

/**
  * @brief pops the first value of the ringbuffer (element gets removed)
  * @param rb: ringbuffer to operate
  * @param pVal: pointer on value to store the popped val
  * @returns true if something got popped, else false
  */
uint32_t Ringbuffer_Pop(tCircularBuffer* rb, void *pVal){
    if(rb->count == 0){
        // handle error
        return 0;
    }
    memcpy(pVal, rb->tail, rb->sz);
    rb->tail = (char*)rb->tail + rb->sz;
    if(rb->tail == rb->buffer_end)
        rb->tail = rb->buffer;
    rb->count--;
    return 1;
}

/**
  * @brief returns true if buffer is empty, else false
  * @param rb: ringbuffer to operate
  * @returns true if empty else false
  */
uint32_t Ringbuffer_IsEmpty(tCircularBuffer const * const rb){
    
    return rb->count == 0 ? 1 : 0;
}
