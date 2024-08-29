#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <stdint.h>
#include <cstddef>

#define BUFFER_SIZE 1024

typedef enum CircularBufferStatus
{
    CIRCULAR_BUFFER_ERROR,
    CIRCULAR_BUFFER_OK,
    CIRCULAR_BUFFER_EMPTY,
    CIRCULAR_BUFFER_FULL,
} CircularBufferStatus_t;

typedef struct CircularBuffer
{
    uint32_t *pu4_buffer;
    uint32_t u4_start;
    uint32_t u4_end;
} CircularBuffer_t;

CircularBufferStatus_t f_initCircularBuffer(CircularBuffer_t *p_circularBuffer);
CircularBufferStatus_t f_putCircularBuffer(CircularBuffer_t *p_circularBuffer, uint32_t u4_data);
CircularBufferStatus_t f_getCircularBuffer(CircularBuffer_t *p_circularBuffer, uint32_t *pu4_data);
CircularBufferStatus_t f_getCircularBufferLength(CircularBuffer_t *p_circularBuffer, uint32_t *pu4_length);

#endif // _CIRCULAR_BUFFER_H_