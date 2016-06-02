#include "uart.h"

/****************************************************************************
 * Verify that the uart base address is valid
 ****************************************************************************/
bool verifyUartBase(uint32_t base)
{
   switch(base)
   {
     case UART0_BASE:
     case UART1_BASE:
     case UART2_BASE:
     case UART3_BASE:
     case UART4_BASE:
     case UART5_BASE:
     case UART6_BASE:
     case UART7_BASE:
     {
       return true;
     }
     default:
     {
       return false;
     }
   }
}


/****************************************************************************
 * This routine transmits a character string out the UART / COM port.
 * Only the lower 8 bits of the 'data' variable are transmitted.
 ****************************************************************************/
void uartTxPoll(uint32_t base, char *data)
{
  UART0_Type *myUart =((UART0_Type *) base);

  if( verifyUartBase(base) == false)
  {
    return;
  }

  if ( data != 0)
  {
    while(*data != '\0')
    {
      while( ((myUart->FR)&(UART_FR_TXFF)) != 0 );
      myUart->DR = *data;
      data++;
    }
  }
  return;
}

/****************************************************************************
 * This routine returns a character received from the UART/COM port.
 * If blocking is enabled, this routine should not return until data
 * is available. If blocking is disabled and no data is available,
 * this function should return 0.
 ****************************************************************************/
char uartRxPoll(uint32_t base, bool block)
{
  UART0_Type *myUart =((UART0_Type *) base);

  if( verifyUartBase(base) == false)
  {
    return 0;
  }

  if( (block == false) && !(myUart->FR & UART_FR_RXFE))
  {
       return myUart->DR;
  }
  else if((block == false) && (myUart->FR & UART_FR_RXFE))
  {
    return 0;
  }

  while(myUart->FR & UART_FR_RXFE && block)
  {
    // Wait
  }

   return myUart->DR;
}

//************************************************************************
// Configure UART0 to be 115200, 8N1.  Data will be sent/recieved using
// polling (Do Not enable interrupts)
//************************************************************************
bool uart_init_115K(
  uint32_t base_addr, 
  uint32_t rcgc_mask, 
  uint32_t pr_mask
)
{
    UART0_Type *myUart;
    if( verifyUartBase(base_addr) == false)
    {
      return false;
    }
    
    myUart = (UART0_Type *)base_addr;
    
    // Turn on the UART Clock
    SYSCTL->RCGCUART |= rcgc_mask;
    
    // Wait until the UART is ready
    while( (SYSCTL->PRUART & pr_mask) == 0)
    {
      // busy wait
    }
    
    // Set the baud rate
    myUart->IBRD = 27;
    myUart->FBRD = 9;
    
    // Disable UART
    myUart->CTL &= ~UART_CTL_UARTEN;
    
    // Configure the Line Control for 8N1, FIFOs
    myUart->LCRH =   UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    // Configure the FIFO Interrupt Levels
    myUart->IFLS = UART_IFLS_RX7_8 | UART_IFLS_TX1_8;
    
        
    // Enable Tx, Rx, and the UART
    myUart->CTL =  UART_CTL_RXE |  UART_CTL_TXE |  UART_CTL_UARTEN;
    
    return true;

}
