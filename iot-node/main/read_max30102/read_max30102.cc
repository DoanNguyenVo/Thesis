#include "read_max30102.h"

MAX30105 particleSensor;             // Create the MAX30105 object
const uint8_t u1_bufferLength = 100; // buffer length

void f_setupMax30102(void)
{
    // Initialize sensor
    if (particleSensor.begin() == false)
    {
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
        while (1)
            ;
    }

    // Configure sensor
    byte powerLevel = 0x3F; // Options: 0x02, 0x1F, 0x7F, 0xFF
    byte sampleAverage = 1; // Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 2;       // Options: 1, 2
    int sampleRate = 100;   // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411;   // Options: 69, 118, 215, 411
    int adcRange = 4096;    // Options: 2048, 4096, 8192, 16384

    // Set up the MAX30105
    particleSensor.setup(powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void f_readMax30102(uint32_t *pu4_irBuffer, uint32_t *pu4_redBuffer)
{
    *pu4_irBuffer = particleSensor.getIR();
    *pu4_redBuffer = particleSensor.getRed();
}