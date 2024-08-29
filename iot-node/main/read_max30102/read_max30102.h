#ifndef _READ_MAX30102_H_
#define _READ_MAX30102_H_

#include <stdint.h>
#include "MAX30105.h"

extern MAX30105 particleSensor;

void f_setupMax30102(void);
void f_readMax30102(uint32_t *pu4_irBuffer, uint32_t *pu4_redBuffer);

#endif /* READ_MAX30102_H_ */