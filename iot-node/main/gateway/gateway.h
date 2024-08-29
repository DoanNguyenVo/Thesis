#ifndef TRANSMIT_DATA_H_
#define TRANSMIT_DATA_H_

#include "env.h"

#include <stdint.h>

const uint32_t ID_DEVICE_LEN = 32;
const uint32_t DATA_LEN = 100;
const uint32_t PACKET_LEN = 820;

typedef struct
{
    char deviceId[ID_DEVICE_LEN];
    uint32_t index;
    uint32_t irData[DATA_LEN];
    uint32_t redData[DATA_LEN];
} packet_t;

void f_setupGateway(void);
void f_transmitPacketPPG(uint32_t u4_index, uint32_t *pu2_irData, uint32_t *pu2_redData);

#endif /* TRANSMIT_DATA_H_ */