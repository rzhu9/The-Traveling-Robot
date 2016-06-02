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

#include "../include/adc.h"
#include "../include/sysctrl.h"

/******************************************************************************
 * Initializes ADC to use Sample Sequencer #3, triggered by software, no IRQs
 *****************************************************************************/
bool initializeADC(  uint32_t adc_base )
{
  ADC0_Type  *myADC;
  
  if( adc_base == 0)
  {
    return false;
  }
  
  switch (adc_base) 
  {
    case ADC0_BASE :
    {
      SYSCTL->RCGCADC   |= SYSCTL_RCGCADC_R0;
      while ((SYSCTL->PRADC & SYSCTL_PRADC_R0) == 0);
      break;
    }
    case ADC1_BASE :
    {
      SYSCTL->RCGCADC   |= SYSCTL_RCGCADC_R1;
      while ((SYSCTL->PRADC & SYSCTL_PRADC_R0) == 0);
      break;
    }
    
    default:
      return false;
  }
  
  myADC = (ADC0_Type *)adc_base;
  
  // disable the sample sequencer by writing a 0 to the corresponding ASENn bit in the ADCACTSS register 
  myADC->ACTSS &= ~ADC_ACTSS_ASEN3;

  // Sequencer 3 is the lowest priority
  myADC->SSPRI = ADC_SSPRI_SS3_4TH | ADC_SSPRI_SS2_3RD | ADC_SSPRI_SS1_2ND | ADC_SSPRI_SS0_1ST;

  myADC->EMUX &= ~ADC_EMUX_EM3_ALWAYS;

  myADC->SSMUX3 &=  ~ADC_SSMUX3_MUX0_M;

  myADC->SSCTL3 = ADC_SSCTL3_IE0 | ADC_SSCTL3_END0;

  // Clear Averaging Bits
  myADC->SAC &= ~ADC_SAC_AVG_M  ;
  
  // Average 64 samples
  myADC->SAC |= ADC_SAC_AVG_64X;
  
  return true;
}

/******************************************************************************
 * Reads SSMUX3 for the given ADC.  Busy waits until completion
 *****************************************************************************/
uint32_t getADCValue( uint32_t adc_base, uint8_t channel)
{
  ADC0_Type  *myADC;
  uint32_t result;
  
  if( adc_base == 0)
  {
    return false;
  }
  
  myADC = (ADC0_Type *)adc_base;
  
  myADC->SSMUX3 = channel;          // Set the Channel
  
  myADC->ACTSS |= ADC_ACTSS_ASEN3;  // Enable SS3
  
  myADC->PSSI =   ADC_PSSI_SS3;     // Start SS3
  
  while( (myADC->RIS & ADC_RIS_INR3)  == 0)
  {
    // wait
  }
  
  result = myADC->SSFIFO3 & 0xFFF;    // Read 12-bit data
  
  myADC->ISC  = ADC_ISC_IN3;          // Ack the conversion
  
  return result;
}

