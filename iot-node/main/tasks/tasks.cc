#include "main_function.h"
#include "tasks.h"

uint8_t u1_bufferLength = 0;

void f_taskPutPPGToCircularBuffer(void *pvParameters)
{
    uint32_t u4_irBuffer;
    uint32_t u4_redBuffer;

    Serial.println("f_taskPutPPGToCircularBuffer started");

    if (xSemaphore == NULL)
    {
        Serial.println("Semaphore is NULL in f_taskPutPPGToCircularBuffer");
        vTaskDelete(NULL);
    }

    while (true)
    {
        f_readMax30102(&u4_irBuffer, &u4_redBuffer);

        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
        {
            if (f_putCircularBuffer(&irCircularBuffer, u4_irBuffer) != CIRCULAR_BUFFER_OK)
            {
                Serial.println("IR Circular buffer full, data lost");
            }
            if (f_putCircularBuffer(&redCircularBuffer, u4_redBuffer) != CIRCULAR_BUFFER_OK)
            {
                Serial.println("Red Circular buffer full, data lost");
            }
        }
        xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(1);
}

void f_taskInference(void *pvParameters)
{
    uint32_t u4_rawFrameIrPPG[FRAME_SIZE];
    uint32_t u4_rawFrameRedPPG[FRAME_SIZE];

    float f4_nomalizedFrameIrPPG[FRAME_SIZE];
    float f4_nomalizedFrameRedPPG[FRAME_SIZE];

    uint32_t u4_index = 0;
    float f4_inferenceResult = 0;

    Serial.println("f_taskInference started");

    while (true)
    {
        f_getRawPPGFrameFromCircularBuffer(u4_rawFrameIrPPG, u4_rawFrameRedPPG);

        f_normalizeZScore(u4_rawFrameIrPPG, f4_nomalizedFrameIrPPG);

        // Inference
        memcpy(input->data.f, f4_nomalizedFrameIrPPG, FRAME_SIZE * sizeof(float));
        interpreter->Invoke();
        f4_inferenceResult = output->data.f[0];

        Serial.print("\nInference result: ");
        Serial.println(f4_inferenceResult);

        if (f4_inferenceResult > 0.8)
        {
            Serial.println("\nPPG detected");
            f_transmitPacketPPG(u4_index, u4_rawFrameIrPPG, u4_rawFrameRedPPG);
        }
        else
        {
            Serial.println("\nNo PPG detected");
        }

        u4_index++;

        vTaskDelay(1);
    }
}