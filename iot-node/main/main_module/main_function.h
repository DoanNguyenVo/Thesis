#ifndef _MAIN_FUNCTION_H_
#define _MAIN_FUNCTION_H_

#include "env.h"
#include "arduino.h"
#include "tasks.h"
#include "read_max30102.h"
#include "circular_buffer.h"
#include "data_processor.h"
#include "inference.h"
#include "gateway.h"

#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <stdint.h>

extern const char *TAG;

extern SemaphoreHandle_t xSemaphore; // Declare semaphore as extern

extern CircularBuffer_t irCircularBuffer;
extern CircularBuffer_t redCircularBuffer;

const uint32_t FRAME_SIZE = 100;

void setup(void);

#endif // _MAIN_FUNCTION_H_
