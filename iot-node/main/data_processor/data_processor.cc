#include "main_function.h"
#include "data_processor.h"

void f_getRawPPGFrameFromCircularBuffer(uint32_t *u4_rawFrameIrPPG, uint32_t *u4_rawFrameRedPPG)
{
    uint32_t u4_length;

    do
    {
        f_getCircularBufferLength(&irCircularBuffer, &u4_length);
        vTaskDelay(10);
    } while (u4_length < FRAME_SIZE);

    if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
    {
        for (uint32_t i = 0; i < FRAME_SIZE; i++)
        {
            f_getCircularBuffer(&irCircularBuffer, &u4_rawFrameIrPPG[i]);
            f_getCircularBuffer(&redCircularBuffer, &u4_rawFrameRedPPG[i]);
        }
        xSemaphoreGive(xSemaphore);
    }
}

void f_normalizeZScore(uint32_t *u4_rawFrame, float *f_normalizedFrame)
{
    float f_mean = 0;
    float f_std = 0;
    float f_sum = 0;
    float f_sumSquare = 0;

    for (uint32_t i = 0; i < FRAME_SIZE; i++)
    {
        f_sum += u4_rawFrame[i];
        f_sumSquare += (float)u4_rawFrame[i] * u4_rawFrame[i];
    }

    f_mean = f_sum / FRAME_SIZE;
    f_std = sqrt((f_sumSquare - (f_sum * f_sum) / FRAME_SIZE) / (FRAME_SIZE - 1));

    for (uint32_t i = 0; i < FRAME_SIZE; i++)
    {
        if (f_std != 0)
        {
            f_normalizedFrame[i] = (u4_rawFrame[i] - f_mean) / f_std;
        }
        else
        {
            f_normalizedFrame[i] = 0;
        }
    }
}