#include "main_function.h"

extern "C" void app_main(void)
{
    setup();

    xSemaphore = xSemaphoreCreateMutex();
    if (xSemaphore != NULL)
    {
        Serial.println("Semaphore creation successful. Creating tasks...");
        xTaskCreatePinnedToCore(f_taskPutPPGToCircularBuffer, "taskPutPPGToCircularBuffer", 2048, NULL, 1, NULL, 0);
        xTaskCreatePinnedToCore(f_taskInference, "taskInference", 81920, NULL, 1, NULL, 1);
    }
    else
    {
        Serial.println("Semaphore creation failed. Tasks not created.");
    }
}
