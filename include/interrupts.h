#ifndef __ECE353_INTERRUPTS_H__
#define __ECE353_INTERRUPTS_H__

#include <stdint.h>
#include <stdbool.h>
#include "TM4C123.h"

#include "uart.h"
#include "pc_buffer.h"

void UART0_Handler(void);
void GPIOC_Handler(void);
void GPIOF_Handler(void);

#endif
