
#include "encoders.h"

// The linear factor in pulses with respect to inches.
#define BASE_PULSES_PER_INCH 80.0f

// The base number of pulses.
#define BASE_PULSES_BASE -63.0f

// The linear factor in pulses with respect to inches * duty
#define DUTY_PULSES_PER_INCH 0.0f

// The linear factor in pulses with respect to duty
#define DUTY_PULSES_BASE -0.66f

uint32_t getNumPulses(float nInches, uint16_t duty) {
	return (int)(nInches * BASE_PULSES_PER_INCH + BASE_PULSES_BASE + nInches * duty * DUTY_PULSES_PER_INCH + duty * DUTY_PULSES_BASE);
}
