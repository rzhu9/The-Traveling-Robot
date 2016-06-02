#ifndef __DRV8833_H__
#define __DRV8833_H__

#include "TM4C123GH6PM.h"
#include "boardUtil.h"
#include "pwm.h"

extern volatile uint32_t pulseCountDown;

void  drv8833_gpioInit(void);
void  drv8833_stop(void);
void  drv8833_leftForward(uint8_t dutyCycle);
void  drv8833_leftReverse(uint8_t dutyCycle);
void  drv8833_rightForward(uint8_t dutyCycle);
void  drv8833_rightReverse(uint8_t dutyCycle);
void  drv8833_turnLeft(uint8_t dutyCycle);
void  drv8833_turnRight(uint8_t dutyCycle);
void drv8833_right45(void);
void drv8833_left45(void);
  
#endif
