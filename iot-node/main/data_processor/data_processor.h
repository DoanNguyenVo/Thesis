#ifndef _DATA_PROCESSOR_H_
#define _DATA_PROCESSOR_H_

#include <stdint.h>

void f_getRawPPGFrameFromCircularBuffer(uint32_t *u4_rawFrameIrPPG, uint32_t *u4_rawFrameRedPPG);
void f_normalizeZScore(uint32_t *u4_rawFrame, float *f_normalizedFrame);

#endif // _DATA_PROCESSOR_H_