/*
 * G53_UART_basic_example.c
 *
 * Created: 04/11/2014 05:48:15
 *  Author: Jerome Semette
 */ 

#include "sam.h"

void Console_init() 
{
	/*Enable USART7 Clock */
	PMC->PMC_PCR = (PMC_PCR_PID(7)|PMC_PCR_CMD|PMC_PCR_DIV_PERIPH_DIV_MCK|PMC_PCR_EN);
	
	/* Configure FLEXCOM7 MUX */
	FLEXCOM7->FLEXCOM_MR = FLEXCOM_MR_OPMODE_USART;
	
	/* Set PA27 and PA28 MUX to peripheral B (TXD7 RXD7)*/
	PIOA->PIO_PDR = (PIO_PDR_P28 | PIO_PDR_P27);
	PIOA->PIO_ABCDSR[0] |= (PIO_ABCDSR_P27|PIO_ABCDSR_P28);
	PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P27|PIO_ABCDSR_P28);

	/* Reset and disable receiver & transmitter */
	USART7->US_CR = (US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS);
	USART7->US_MR = US_MR_USCLKS_MCK | US_MR_CHRL_8_BIT | US_MR_PAR_NO ;
	
	/* Set baud rate to 115200 */
	USART7->US_BRGR = (SystemCoreClock / (115200*16) ) ;
	
	/* Enable receiver and transmitter */
	USART7->US_CR = US_CR_RXEN | US_CR_TXEN;
}

char USART_getc() {
	while(!(USART7->US_CSR & US_CSR_RXRDY));
	return  (char) FLEXCOM7->FLEXCOM_RHR;
}


void USART_putc(char Char) {
	while(!(USART7->US_CSR & US_CSR_TXRDY));
	FLEXCOM7->FLEXCOM_THR = (uint32_t) Char;
}


/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	
	uint8_t test_rx_char;
	
    /* Initialize the SAM system */
    SystemInit();
	SystemCoreClockUpdate();
	Console_init();
	
    while (1) 
    {
		// echo test
	    test_rx_char = USART_getc();
		USART_putc(test_rx_char);
	}
}
