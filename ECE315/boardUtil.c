// Copyright (c) 2014, Joe Krachey
// All rights reserved.
//
// Redistribution and use in binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "TM4C123GH6PM.h"
#include "boardUtil.h"
#include "../include/sysctrl.h"

void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}

//*****************************************************************************
// Configure peripherals needed for the motor encoders unit.
//*****************************************************************************
void encodersInit(void) {
	GPIOA_Type* myGPIO;
	
	gpio_enable_port(GPIOF_BASE);
	gpio_enable_port(GPIOC_BASE);
	
	gpio_config_enable_input(GPIOF_BASE, PF0 | PF1);
	gpio_config_enable_input(GPIOC_BASE, PC5 | PC6);
	
	// Pins 0, and 1 need to be digital enables for direct reading.
	gpio_config_digital_enable(GPIOF_BASE, PF0 | PF1);
	gpio_config_digital_enable(GPIOC_BASE, PC5 | PC6);
	
	// Enable interrupts
	myGPIO = (GPIOA_Type*)GPIOF_BASE;
	myGPIO->IM &= ~(PF0 | PF1);
	myGPIO->IBE |= (PF0 | PF1);
	NVIC_EnableIRQ(GPIOF_IRQn);
	myGPIO->IM |= (PF0 | PF1);
	
	// This configuration is unused, since port F is used for encoder measurements.
	myGPIO = (GPIOA_Type*)GPIOC_BASE;
	myGPIO->IM &= ~(PC5 | PC6);
	myGPIO->IBE |= (PC5 | PC6);
	NVIC_EnableIRQ(GPIOC_IRQn);
	myGPIO->IM |= (PC5 | PC6);
}

//*****************************************************************************
// Configure peripherals needed for the sonar unit.
//*****************************************************************************
void sonarUARTInit(void)
{
	
    UART0_Type* myUart = (UART0_Type*)UART7_BASE;
    
    // Turn on the UART Clock
    SYSCTL->RCGCUART |= SYSCTL_RCGCUART_R7;
    
    // Wait until the UART is ready
    while( (SYSCTL->PRUART & SYSCTL_PRUART_R7) == 0)
    {
      // busy wait
    }
    
    // Set the baud rate
    myUart->IBRD = 325;
    myUart->FBRD = 33;
    
    // Disable UART
    myUart->CTL &= ~UART_CTL_UARTEN;
    
    // Configure the Line Control for 8N1, FIFOs
    myUart->LCRH =   UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    // Configure the FIFO Interrupt Levels
    myUart->IFLS = UART_IFLS_RX7_8 | UART_IFLS_TX1_8;
        
    // Enable Tx, Rx, and the UART
    myUart->CTL =  UART_CTL_RXE |  UART_CTL_TXE |  UART_CTL_UARTEN;
		
		myUart->IM &= ~(1 << 4);
		NVIC_EnableIRQ(UART7_IRQn);
		myUart->IM |= (1 << 4);

}

//*****************************************************************************
// Configure peripherals needed for the motor unit.
//*****************************************************************************
void motorInit(void) {
	  drv8833_gpioInit();
}

//*****************************************************************************
// Configure peripherals needed for the sonar unit.
//*****************************************************************************
void sonarInit(void) {
	 
	 // Initialize ADC 0 to convert one sonar input.
	 initializeADC(ADC0_BASE);
	
	 // We'll be grabbing input from GPIOE, so enable it and set it up for input.
	 gpio_enable_port(GPIOE_BASE);
	 gpio_config_enable_input(GPIOE_BASE, PE2 | PE3);
	
	 // Pins 0, 1 and 2 need to be digital enables for direct reading.
	 gpio_config_digital_enable(GPIOE_BASE, PE0 | PE1 | PE2);
	
	 // Pin 3 will be our analog input for converting.
	 gpio_config_analog_enable(GPIOE_BASE, PE3);
	
	 // Here, we set pins E0 and E1 for UART 7 RX and TX respectively.
	 gpio_config_alternate_function(GPIOE_BASE, PE0 | PE1);
	 gpio_config_port_control(GPIOE_BASE, GPIO_PCTL_PE0_U7RX | GPIO_PCTL_PE1_U7TX);
	
	 // Run the necessary UART configuration to begin communication.
	 sonarUARTInit();
}

//*****************************************************************************
// Configure PA0 and PA1 to be UART pins
//*****************************************************************************
void uart0_config_gpio(void)
{
   gpio_enable_port(GPIOA_BASE);
   gpio_config_digital_enable( GPIOA_BASE, PA0 | PA1);
   gpio_config_alternate_function( GPIOA_BASE, PA0 | PA1);
   gpio_config_port_control( GPIOA_BASE, GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX);

}


//*****************************************************************************
//*****************************************************************************
void serialDebugInit(void)
{
  // Configure GPIO Pins
  uart0_config_gpio();

  
  // Initialize UART0 for 8N1, interrupts enabled.
  uart_init_115K(
    UART0_BASE, 
    SYSCTL_RCGCUART_R0, 
    SYSCTL_PRUART_R0
  );
}
