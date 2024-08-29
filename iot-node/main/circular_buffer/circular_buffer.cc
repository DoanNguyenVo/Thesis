#include "circular_buffer.h"
#include <stdlib.h>

CircularBufferStatus_t f_initCircularBuffer(CircularBuffer_t *p_circularBuffer)
{
    if (p_circularBuffer == NULL)
    {
        return CIRCULAR_BUFFER_ERROR;
    }
    p_circularBuffer->pu4_buffer = (uint32_t *)malloc(BUFFER_SIZE * sizeof(uint32_t));
    if (p_circularBuffer->pu4_buffer == NULL)
    {
        return CIRCULAR_BUFFER_ERROR;
    }
    p_circularBuffer->u4_start = 0;
    p_circularBuffer->u4_end = 0;
    return CIRCULAR_BUFFER_OK;
}

CircularBufferStatus_t f_putCircularBuffer(CircularBuffer_t *p_circularBuffer, uint32_t u4_data)
{
    if (p_circularBuffer == NULL || p_circularBuffer->pu4_buffer == NULL)
    {
        return CIRCULAR_BUFFER_ERROR;
    }
    uint32_t next_end = (p_circularBuffer->u4_end + 1) % BUFFER_SIZE;
    if (next_end == p_circularBuffer->u4_start)
    {
        return CIRCULAR_BUFFER_FULL;
    }
    p_circularBuffer->pu4_buffer[p_circularBuffer->u4_end] = u4_data;
    p_circularBuffer->u4_end = next_end;
    return CIRCULAR_BUFFER_OK;
}

CircularBufferStatus_t f_getCircularBuffer(CircularBuffer_t *p_circularBuffer, uint32_t *pu4_data)
{
    if (p_circularBuffer == NULL || pu4_data == NULL || p_circularBuffer->pu4_buffer == NULL)
    {
        return CIRCULAR_BUFFER_ERROR;
    }
    if (p_circularBuffer->u4_start == p_circularBuffer->u4_end)
    {
        return CIRCULAR_BUFFER_EMPTY;
    }
    *pu4_data = p_circularBuffer->pu4_buffer[p_circularBuffer->u4_start];
    p_circularBuffer->u4_start = (p_circularBuffer->u4_start + 1) % BUFFER_SIZE;
    return CIRCULAR_BUFFER_OK;
}

CircularBufferStatus_t f_getCircularBufferLength(CircularBuffer_t *p_circularBuffer, uint32_t *pu4_length)
{
    if (p_circularBuffer == NULL || pu4_length == NULL || p_circularBuffer->pu4_buffer == NULL)
    {
        return CIRCULAR_BUFFER_ERROR;
    }
    if (p_circularBuffer->u4_end >= p_circularBuffer->u4_start)
    {
        *pu4_length = p_circularBuffer->u4_end - p_circularBuffer->u4_start;
    }
    else
    {
        *pu4_length = BUFFER_SIZE - p_circularBuffer->u4_start + p_circularBuffer->u4_end;
    }
    return CIRCULAR_BUFFER_OK;
}