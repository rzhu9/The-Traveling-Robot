#ifndef _ENCODERS_H_
#define _ENCODERS_H_

#include <stdint.h>

//*****************************************************************************
// Computes the number of pulses to move a desired number of inches.
//*****************************************************************************
uint32_t getNumPulses(float nInches, uint16_t duty);

#endif // _ENCODERS_H_
