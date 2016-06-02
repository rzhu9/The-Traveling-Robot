#include "drv8833.h"

#define RIGHT_DUTY			91
#define LEFT_DUTY				47
#define PULSE_LOAD      23000
#define RIGHT_45_VAL    2300000
#define LEFT_45_VAL     2300000
#define PWM_LOAD_VAL    10000
#define PWM_CHANNEL_PWM   (PWM_GEN_ACTCMPAD_LOW | PWM_GEN_ACTLOAD_HIGH | PWM_GEN_ACTZERO_NOTHING)
#define PWM_CHANNEL_LOW   (PWM_GEN_ACTCMPBD_LOW | PWM_GEN_ACTLOAD_LOW | PWM_GEN_ACTZERO_NOTHING)
#define PWM_CHANNEL_HIGH  (PWM_GEN_ACTCMPBD_HIGH | PWM_GEN_ACTLOAD_HIGH | PWM_GEN_ACTZERO_NOTHING)

//*****************************************************************************
// Initializes the 6 pins needed to control the DRV8833
//*****************************************************************************
void  drv8833_gpioInit(void)
{
  // We'll be grabbing input from GPIOE, so enable it and set it up for input.
	gpio_enable_port(GPIOB_BASE);
	gpio_enable_port(GPIOE_BASE);
	gpio_enable_port(GPIOF_BASE);
	 
	gpio_config_digital_enable(GPIOB_BASE, PB4 | PB5);
	gpio_config_digital_enable(GPIOE_BASE, PE4 | PE5);
	gpio_config_digital_enable(GPIOF_BASE, PF2 | PF3);
	
	// Here, we set pins E1 and E2 for UART 7 RX and TX respectively.
	gpio_config_alternate_function(GPIOB_BASE, PB4 | PB5);
	gpio_config_alternate_function(GPIOE_BASE, PE4 | PE5);
	
	gpio_config_enable_input(GPIOF_BASE, PF2);
	gpio_config_enable_output(GPIOF_BASE, PF3);
	
	gpio_config_port_control(GPIOB_BASE, 0x00440000);//(GPIO_PCTL_PB4_M | 0x4) | (GPIO_PCTL_PB5_M | 0x4));
	gpio_config_port_control(GPIOE_BASE, 0x00550000);//(GPIO_PCTL_PE4_M | 0x5) | (GPIO_PCTL_PE5_M | 0x5));
}

void drv8833_stop() {
	pwmConfig(
		PWM0_BASE,
		1, 
		PWM_LOAD_VAL, 
		(uint32_t)(0),
		(uint32_t)(0),
		PWM_CHANNEL_LOW,
		PWM_CHANNEL_LOW
	);
	pwmConfig(
		PWM1_BASE,
		1, 
		PWM_LOAD_VAL, 
		(uint32_t)(0),
		(uint32_t)(0),
		PWM_CHANNEL_LOW,
		PWM_CHANNEL_LOW
	);
}

//*****************************************************************************
//*****************************************************************************
void  drv8833_leftForward(uint8_t dutyCycle)
{
  pwmConfig(
		PWM0_BASE,
		1, 
		PWM_LOAD_VAL, 
		(uint32_t)(PWM_LOAD_VAL * ((double)(100 - dutyCycle)) / 100.0),
		(uint32_t)(PWM_LOAD_VAL * ((double)(100 - dutyCycle)) / 100.0),
		PWM_CHANNEL_PWM,
		PWM_CHANNEL_LOW
	);
}

//*****************************************************************************
//*****************************************************************************
void  drv8833_leftReverse(uint8_t dutyCycle)
{
	pwmConfig(
		PWM0_BASE,
		1, 
		PWM_LOAD_VAL, 
		(uint32_t)(PWM_LOAD_VAL * ((double)(100 - dutyCycle)) / 100.0),
		(uint32_t)(PWM_LOAD_VAL * ((double)dutyCycle) / 100.0), 
		PWM_CHANNEL_LOW,
		PWM_CHANNEL_PWM
	);
}


//*****************************************************************************
//*****************************************************************************
void  drv8833_rightForward(uint8_t dutyCycle)
{
	pwmConfig(
		PWM1_BASE,
		1, 
		PWM_LOAD_VAL, 
		(uint32_t)(PWM_LOAD_VAL * ((double)(100 - dutyCycle)) / 100.0),
		(uint32_t)(PWM_LOAD_VAL * ((double)(100 - dutyCycle)) / 100.0),
		PWM_CHANNEL_PWM,
		PWM_CHANNEL_LOW
	);
}

//*****************************************************************************
//*****************************************************************************
void  drv8833_rightReverse(uint8_t dutyCycle)
{
	 pwmConfig(
		PWM1_BASE,
		1, 
		PWM_LOAD_VAL, 
		(uint32_t)(PWM_LOAD_VAL * ((double)(100 - dutyCycle)) / 100.0),
		(uint32_t)(PWM_LOAD_VAL * ((double)dutyCycle) / 100.0), 
		PWM_CHANNEL_LOW,
		PWM_CHANNEL_PWM
	);
}

//*****************************************************************************
//*****************************************************************************
void  drv8833_turnLeft(uint8_t dutyCycle)
{
	 drv8833_leftReverse(dutyCycle);
	 drv8833_rightForward(dutyCycle);
}

//*****************************************************************************
//*****************************************************************************
void  drv8833_turnRight(uint8_t dutyCycle)
{
	 drv8833_rightReverse(dutyCycle);
	 drv8833_leftForward(dutyCycle);
}

void drv8833_right45() {
	pulseCountDown = PULSE_LOAD;
	volatile uint32_t i;
	
	drv8833_rightReverse(RIGHT_DUTY);
	drv8833_leftForward(LEFT_DUTY);
	//for (i = 0; i < RIGHT_45_VAL; i++)
	//	; // Do nothing
	drv8833_stop();
}

void drv8833_left45() {
	volatile uint32_t i;
	pulseCountDown = PULSE_LOAD;
	drv8833_leftReverse(LEFT_DUTY);
	drv8833_rightForward(RIGHT_DUTY);
	//for (i = 0; i < LEFT_45_VAL; i++)
	//	; // Do nothing
	drv8833_stop();
}
