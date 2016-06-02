#include "lcd.h"
#include "../include/spi.h"
#include "fonts.h"

#define NUM_PAGES   8
#define NUM_COLS    102

void dogs102_assert_cmd_mode(void)
{
	((GPIOA_Type*)GPIOD_BASE)->DATA &= ~PD7;
}

void dogs102_deassert_cmd_mode(void)
{
	((GPIOA_Type*)GPIOD_BASE)->DATA |= PD7;
}

//*****************************************************************************
// Initializes the pins needed to communicate with the LCD and issues the 
// initialization sequence found in the LCD datasheet via the SPI interface
//*****************************************************************************
void ece315_lcdInit(void)
{
  uint8_t tx_data;
  uint8_t rx_data;
	
	gpio_enable_port(LCD_GPIO_BASE);
	
  // Configure SPI CLK
  gpio_config_digital_enable(LCD_GPIO_BASE, LCD_CLK_PIN);
  gpio_config_alternate_function(LCD_GPIO_BASE, LCD_CLK_PIN);
  gpio_config_port_control(LCD_GPIO_BASE, LCD_CLK_PIN_PCTL);
    
  // Configure SPI CS
  gpio_config_digital_enable(LCD_GPIO_BASE, LCD_CS_PIN);
  gpio_config_alternate_function(LCD_GPIO_BASE, LCD_CS_PIN);
  gpio_config_port_control(LCD_GPIO_BASE, LCD_CS_PIN_PCTL);

  // Configure SPI MOSI
  gpio_config_digital_enable(LCD_GPIO_BASE, LCD_MOSI_PIN);
  gpio_config_alternate_function(LCD_GPIO_BASE, LCD_MOSI_PIN);
  gpio_config_port_control(LCD_GPIO_BASE, LCD_MOSI_PIN_PCTL);
  
  // Configure CD
  gpio_config_digital_enable(GPIO_LCD_CD_BASE,LCD_CD_PIN);
  gpio_config_enable_output(GPIO_LCD_CD_BASE,LCD_CD_PIN);
  
  // Configure RST_N
  gpio_config_digital_enable(GPIO_LCD_RST_N_BASE, LCD_RST_N_PIN);
  gpio_config_enable_output(GPIO_LCD_RST_N_BASE, LCD_RST_N_PIN);
  
  initialize_spi(LCD_SPI_BASE, 3);
  
  
  // Bring the LCD out of reset
  ((GPIOA_Type *)GPIOD_BASE)->DATA |= PD6;
	
  // Use spiTx() from the ece315 driver library to issue the sequence of 
  // commands in the LCD data sheet to initialize the LCD.  
	
  //Enter Command Mode
  dogs102_assert_cmd_mode();
  
	//Set Scroll Line
  tx_data = 0x40;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
  //Set SEG Directions
  tx_data = 0xA1;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
  //Set COM direction
  tx_data = 0xC0;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
  //Set All Pixel on
  tx_data = 0xA4;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
  //Set Inverse Display
  tx_data = 0xA6;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
  //et LCD Bias Ratio
  tx_data = 0xA2;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
  //Set Power Control
  tx_data = 0x2F;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
  //Set VLCD Resistor Ratio
  tx_data = 0x27;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
  //Set Electronic Volume
  tx_data = 0x81;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
	
  tx_data = 0x10;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
  //Set Adv Program Control
  tx_data = 0xFA;
  spiTx(SSI3_BASE, &tx_data, 1, &rx_data);
	
  tx_data = 0x90;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);
  
 //Set Display Enable
  tx_data = 0xAF;
  spiTx(SSI3_BASE,&tx_data, 1, &rx_data);

  //Exit Command Mode
  dogs102_deassert_cmd_mode();
  
}

//****************************************************************************
// Sets the currently active page
//*****************************************************************************
  void ece315_lcdSetPage(uint8_t   page)
  {
  uint8_t data, rec;
    
	//Enter Command Mode
  dogs102_assert_cmd_mode();
	
  // Set the active page using spiTx
	data = 0xB0 | (page & 0x0F);
	spiTx(SSI3_BASE, &data, 1, &rec);
    
  //Exit Command Mode
	dogs102_deassert_cmd_mode();
  }
  
//*****************************************************************************
// Sets the currently active column
//*****************************************************************************
void ece315_lcdSetColumn(uint8_t   column)
{
	uint8_t data, rec;

  //Enter Command Mode
	dogs102_assert_cmd_mode();
  
  // Set the active column LSB using spiTx
	data = column & 0x0F;
	spiTx(SSI3_BASE, &data, 1, &rec);
                  
  // Set the active column MSB using spiTx
	data = 0x10 | (column >> 4);
	spiTx(SSI3_BASE, &data, 1, &rec);
  
  //Exit Command Mode
	dogs102_deassert_cmd_mode();
}
  
//*****************************************************************************
// Writes 8-bits of data to the current column of the LCD
//*****************************************************************************
  void ece315_lcdWriteData(uint8_t   data)
  {
    uint8_t rec;

		// Send the data using spiTx
		spiTx(SSI3_BASE, &data, 1, &rec);
  }
  
//*****************************************************************************
// Erases the LCD screen.
//*****************************************************************************
 void ece315_lcdClear(void)
 {
   uint8_t page, col;
	
	for( page = 0; page < NUM_PAGES; page++) {
		
    ece315_lcdSetPage( page );
		
		for(col = 0; col < NUM_COLS; col++) {
			
			ece315_lcdSetColumn( col );
      ece315_lcdWriteData( 0x00 );
			
		}
  }
 }

//*****************************************************************************
// Each character is 10 columns wide.  The colStart is the column number where
// the first column will be printed.
//
// font.c contains a lookup table for printing out characters to the LCD screen.
// You should note that each character is 16 pixels high and 10 pixels wide.
// For each character, you will have to write to two different pages to print
// out a single character.  
//*****************************************************************************
void ece315_lcdWriteChar( uint8_t page, char c, uint8_t colStart)
 {
	 const uint8_t* data = &courierNew_10ptBitmaps[(c - 32) * 20];
	 uint8_t curCol;
   ece315_lcdSetPage(page);
	 
	 for (curCol = colStart; curCol < colStart + 10; curCol++) {
		 ece315_lcdSetColumn(curCol);
		 ece315_lcdWriteData(*data);
		 data++;
	 }
	 
	 
   ece315_lcdSetPage(page + 1);
	 
	 for (curCol = colStart; curCol < colStart + 10; curCol++) {
		 ece315_lcdSetColumn(curCol);
		 ece315_lcdWriteData(*data);
		 data++;
	 }
 }
 
//*****************************************************************************
// Write a string of characters out to the LCD screen.  Only the first 
// 10 characters will be printed.  The function will also terminate when
// a null character is encountered.
//*****************************************************************************
void ece315_lcdWriteString( uint8_t line, char *string)
{
	uint8_t count = 0;
  while (*string && count < 10) {
		ece315_lcdWriteChar(line, *string, 10 * count);
		count++;
		string++;
	}
}  

