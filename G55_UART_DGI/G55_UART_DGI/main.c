/*
 * G55_UART_DGI.c
 *
 * Created: 5/31/2016 5:26:01 PM
 * Author : jerome.semette
 */ 

#include "sam.h"

void DGI_init(uint32_t baudrate)
{
	SystemCoreClockUpdate();
	
	/*Enable USART7 Clock */
	PMC->PMC_PCR = (PMC_PCR_PID(22)|PMC_PCR_CMD|PMC_PCR_DIV_PERIPH_DIV_MCK|PMC_PCR_EN);

	/* Configure FLEXCOM6 MUX */
	FLEXCOM6->FLEXCOM_MR = FLEXCOM_MR_OPMODE_USART;

	/* Set PB00 and PB01 MUX to peripheral B (TXD6 RXD6)*/
	PIOB->PIO_PDR = (PIO_PDR_P0 | PIO_PDR_P1);
	PIOB->PIO_ABCDSR[0] |= (PIO_ABCDSR_P0|PIO_ABCDSR_P1);
	PIOB->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P0|PIO_ABCDSR_P1);

	/* Reset and disable receiver & transmitter */
	USART6->US_CR = (US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS);
	USART6->US_MR = US_MR_USCLKS_MCK | US_MR_CHRL_8_BIT | US_MR_PAR_NO ;

	/* Set baud rate to 115200 */
	USART6->US_BRGR = (SystemCoreClock / (baudrate*16) ) ;

	/* Enable receiver and transmitter */
	USART6->US_CR = US_CR_RXEN | US_CR_TXEN;
}

char DGI_getc() {
	while(!(USART6->US_CSR & US_CSR_RXRDY));
	return  (char) FLEXCOM6->FLEXCOM_RHR;
}

void DGI_putc(char Char) {
	while(!(USART6->US_CSR & US_CSR_TXRDY));
	FLEXCOM6->FLEXCOM_THR = (uint32_t) Char;
}

void DGI_puts(char  *str) {
	while(*str){
		__NOP();
		__NOP();
		__NOP();
		DGI_putc(*str++);
	}
}


/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint8_t i;
    /* Initialize the SAM system */
    SystemInit();
	DGI_init(115200);
    while (1){
		for(i=0xFF;i>=0x0;i--){
			DGI_putc(i);
		}
	} 
}
