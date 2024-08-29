#include "main_function.h"

SemaphoreHandle_t xSemaphore = NULL; // Define semaphore

CircularBuffer_t irCircularBuffer;
CircularBuffer_t redCircularBuffer;

void setup(void)
{
    Serial.begin(115200);

    f_setupMax30102();

    f_setupInference();

    f_setupGateway();

    CircularBufferStatus_t status = f_initCircularBuffer(&irCircularBuffer);
    if (status != CIRCULAR_BUFFER_OK)
    {
        Serial.println("Failed to initialize IR circular buffer");
    }

    status = f_initCircularBuffer(&redCircularBuffer);
    if (status != CIRCULAR_BUFFER_OK)
    {
        Serial.println("Failed to initialize RED circular buffer");
    }
}