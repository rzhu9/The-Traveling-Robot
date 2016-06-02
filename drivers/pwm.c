#include "pwm.h"



//*****************************************************************************
//*****************************************************************************
uint8_t pwmConfig(
  uint32_t base,
  uint8_t pwm_generator, 
  uint32_t load, 
  uint32_t cmpa, 
  uint32_t cmpb,
  uint32_t gena,
  uint32_t genb
)
{
	
	uint32_t rcgc_mask = 0, pr_mask = 0;
	PWM0_Type* pwm = (PWM0_Type*)base;
	
	switch(base) {
		
		case PWM0_BASE:
			rcgc_mask = SYSCTL_RCGCPWM_R0;
      pr_mask = SYSCTL_PRPWM_R0; 
			break;
		
		case PWM1_BASE:
			rcgc_mask = SYSCTL_RCGCPWM_R1;
      pr_mask = SYSCTL_PRPWM_R1; 
			break;
		
		default:
			return 1;
	}
	
   // Turn the clock on using the rcgc_mask
   SYSCTL->RCGCPWM |= rcgc_mask;
	
   // Wait for the PRPWM to indicate the PWM is ready
   while( (SYSCTL->PRPWM & pr_mask) == 0)
		 ; // Do nothing
	 
	 switch(pwm_generator) {
		 
		 case 0:
			 pwm->_0_CTL = 0;
			 pwm->_0_LOAD = load;
		   pwm->_0_CMPA = cmpa;
		   pwm->_0_CMPB = cmpb;
		   pwm->_0_GENA = gena;
		   pwm->_0_GENB = genb;
			 pwm->_0_CTL = 1;
			 pwm->ENABLE = 3 << 0;
		 break;
		 
		 case 1:
			 pwm->_1_CTL = 0;
			 pwm->_1_LOAD = load;
		   pwm->_1_CMPA = cmpa;
		   pwm->_1_CMPB = cmpb;
		   pwm->_1_GENA = gena;
		   pwm->_1_GENB = genb;
			 pwm->_1_CTL = 1;
			 pwm->ENABLE = 3 << 2;
		 break;
		 
		 case 2:
			 pwm->_2_CTL = 0;
			 pwm->_2_LOAD = load;
		   pwm->_2_CMPA = cmpa;
		   pwm->_2_CMPB = cmpb;
		   pwm->_2_GENA = gena;
		   pwm->_2_GENB = genb;
			 pwm->_2_CTL = 1;
			 pwm->ENABLE = 3 << 4;
		 break;
		 
		 case 3:
			 pwm->_3_CTL = 0;
			 pwm->_3_LOAD = load;
		   pwm->_3_CMPA = cmpa;
		   pwm->_3_CMPB = cmpb;
		   pwm->_3_GENA = gena;
		   pwm->_3_GENB = genb;
			 pwm->_3_CTL = 1;
			 pwm->ENABLE = 3 << 6;
		 break;
		 
		 default:
			 return 1;
	 }
	
  return 0;
}
