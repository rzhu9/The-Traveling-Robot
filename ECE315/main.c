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

//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#include "TM4C123.h"
#include "boardUtil.h"
#include "ece315_lab3.h"
#include "encoders.h"
#include "lcd.h"
#include "graphics.h"

#define FORWARD 0x4657
#define REVERSE 0x5256
#define RIGHT 0x5254
#define LEFT 0x4c46
#define STOP 0x5354

#define RIGHT_COUNT_RESET 10
#define RIGHT_COUNT_FACTOR 200.0
#define LEFT_COUNT_FACTOR 1.0
#define MAX_RIGHT_SAMPLE_COUNT 300
#define MAX_LEFT_SAMPLE_COUNT 12

//*****************************************************************************
// Foward Declarations
//*****************************************************************************

void handleWirelessInput(void);

//*****************************************************************************
// Global Variables
//*****************************************************************************

volatile bool tenMsTime = false;
volatile uint32_t nRightInterrupts = 0;
volatile uint32_t nLeftInterrupts = 0;
volatile uint32_t pulseCountDown = 0;
volatile bool sonarInterrupt = false;
volatile uint32_t sonarDist = 0;

// We'll use these to keep track of the number of inches between the sensors
// and an obstacle.
volatile uint16_t leftSonar, middleSonar, rightSonar;

//*****************************************************************************
//*****************************************************************************
void initializeBoard(void)
{
  DisableInterrupts();
  serialDebugInit();
	rfInit();
	sonarInit();
	motorInit();
	encodersInit();
	ece315_lcdInit();
	SysTick_Config(2500);
  EnableInterrupts();
}


//*****************************************************************************
//*****************************************************************************

int 
main(void)
{
	uint32_t rightHighCount = 0;
  uint32_t rightSampleCount = 1;
	uint32_t leftSum = 0;
	uint32_t leftSample;
	uint32_t leftSampleCount = 1;
	unsigned int i = 0;
  unsigned int nAlerts = 0;
	bool turned = false;
	
	// This string will be used for reporting only.
	char outputString[64];
	
	// This will hold the current character being processed from the left input.
	char* digits;
	
	// Wireless variable declarations
	char msg[80];
	//leftInput[3] = 0;
	
  initializeBoard();
	
  uartTxPoll(UART0_BASE, "\n\r");
  uartTxPoll(UART0_BASE,"**************************************\n\r");
  uartTxPoll(UART0_BASE,"* ECE315 Default Project\n\r");
  uartTxPoll(UART0_BASE,"**************************************\n\r");
	
	GPIOF->DATA |= PF3;
	
	// Set up the motors as stopped initially.
	drv8833_stop();
	ece315_lcdClear();
  
  // Infinite Loop
  while(1)
  {
		
		if (turned && pulseCountDown == 0) {
			drv8833_stop();
		}
		
		if (sonarInterrupt) {
			sonarInterrupt = false;
				
			digits = (char*)&sonarDist;
			
			// Check that all of the digits are valid ASCII digits.
			if (digits[0] >= '0' &&
				  digits[0] <= '9' &&
					digits[1] >= '0' &&
				  digits[1] <= '9' &&
			    digits[2] >= '0' &&
				  digits[2] <= '9') {
						
				//sprintf(msg, "Sonar: %c%c%c\n\r", digits[0], digits[1], digits[2]);
				//uartTxPoll(UART0_BASE, msg);
				// If we have valid digits, now we can update the value of the left
				// based on what we read in.
				leftSample = 0;
				leftSample += 100 * (digits[0] - '0');
				leftSample += 10 * (digits[1] - '0');
				leftSample += digits[2] - '0';
						
				if (leftSample > 3 * (leftSonar + 1)) {
						leftSample = 3 * (leftSonar + 1);
				}
				
				leftSum += leftSample;
				leftSampleCount++;
				
				if (leftSampleCount > MAX_LEFT_SAMPLE_COUNT) {
						leftSum /= 2;
						leftSampleCount /= 2;
				}
				leftSonar = leftSum * LEFT_COUNT_FACTOR / leftSampleCount;
			}
			sonarDist = 0;
		}
			
		// On 10ms intervals, we want to read data in from the middle sensor's ADC.
		if (tenMsTime) {
			tenMsTime = false;
			
			// Keep track of how often the right sensor is found to be high.
			if (GPIOE->DATA & (1 << 2)) {
				rightHighCount++;
			}
			rightSampleCount++;
			
			if (rightSampleCount > MAX_RIGHT_SAMPLE_COUNT) {
				rightHighCount /= 2;
				rightSampleCount /= 2;
			}
			
			middleSonar = getADCValue(ADC0_BASE, 0) >> 3;
			
			if (middleSonar < 12 && !turned) {
				drv8833_stop();
			}
			
			nAlerts++;
		}
		
		// Every second, we want make a decision to turn or not.
		if (nAlerts == 100) {
			
			// The value in the right sonar is based on how often it was sampled as
			// high, along with how long it has been since we reset, so we need to
			// divide. We also derived an empirical RIGHT_COUNT_RATIO that returns
			// the desired value in inches.
			rightSonar = (unsigned int)(((float)rightHighCount * RIGHT_COUNT_FACTOR) / (float)rightSampleCount);
			
			if (turned) {
					turned = false;
			} else {
				
				/*
				if (leftSonar < 12 && middleSonar > 18) {
					drv8833_rightForward(100);
					drv8833_leftForward(100);
				} else if (leftSonar > 18) {
					drv8833_left45();
					turned = true;
				} else {
					drv8833_right45();
				}
				*/
				
				if (middleSonar > leftSonar && middleSonar > rightSonar) {
					drv8833_rightForward(100);
					drv8833_leftForward(100);
				} else if (leftSonar < rightSonar) {
					turned = true;
					drv8833_right45();
				} else {
					turned = true;
					drv8833_left45();
				}
				
			}
		
			nAlerts = 0;
			
			sprintf(msg, "DIST: %.01f", (float)leftSonar);//digits[1], digits[2]);
			
			ece315_lcdWriteString(0, msg);
				
			// This is purely testing output.
			snprintf(&outputString[0], 64, "Front %d\n\rRight %d\n\rLeft %d\n\r", 
				middleSonar, 
				rightSonar,
				leftSonar);
			uartTxPoll(UART0_BASE, &outputString[0]);
			
		} // end of 100 alerts
		
  } // end of infinite loop
	
}
