#include "interrupts.h"

extern volatile bool tenMsTime;
extern volatile uint32_t nRightInterrupts;
extern volatile uint32_t nLeftInterrupts;
extern volatile uint32_t pulseCountDown;
extern volatile bool sonarInterrupt;
extern volatile uint32_t sonarDist;

//*****************************************************************************
// Handler for SysTick interrupts
//*****************************************************************************
void SysTick_Handler() {
	uint32_t val;
	// We'll want to keep track of the number of microseconds since the last
	// 10 millisecond interval so we can signal another appropriately
	static unsigned int nMicroSec = 0;
	
	// This will be called every 50 microseconds, so increment by that.
	nMicroSec += 50;
	
	// Every 10,000 microseconds (10 milliseconds) set the tenMsTime signal to
	// true of main to catch, and reset the timing.
	if (nMicroSec == 10000) {
		tenMsTime = true;
		nMicroSec = 0;
	}
	val = SysTick->VAL;
}


//*****************************************************************************
// Handler for interrupts on the right side encoder.
//
// This is unused in the current implementation since GPIOF provides pulses.
//*****************************************************************************
void GPIOC_Handler() {
	nRightInterrupts++;
	((GPIOA_Type*)GPIOC_BASE)->ICR = 0xFF;
}

//*****************************************************************************
// Handler for interrupts on the left side encoder. We used this one for the pulses measurement and countdown.
//*****************************************************************************
void GPIOF_Handler() {
	nLeftInterrupts++;
	if (pulseCountDown > 0) {
		pulseCountDown--;
	}
	((GPIOA_Type*)GPIOF_BASE)->ICR = 0xFF;
}

void UART7_Handler() {
	static uint8_t sonarSlot = 5;
	UART0_Type* myUart = (UART0_Type*)UART7_BASE;
	uint8_t data;
	myUart->ICR |= (1 << 4);
	data = myUart->DR;
	
	if (!sonarInterrupt && sonarSlot == 5 && data == 'R') {
		sonarSlot--;
	} else if (sonarSlot == 4) {
		sonarDist |= (((uint32_t)data) << 0);
		sonarSlot--;
	} else if (sonarSlot == 3) {
		sonarDist |= (((uint32_t)data) << 8);
		sonarSlot--;
	} else if (sonarSlot == 2) {
		sonarDist |= (((uint32_t)data) << 16);
		sonarSlot--;
	}  else if (data == '\r' && sonarSlot == 1) {
		sonarInterrupt = true;
		sonarSlot = 5;
	} 
}
