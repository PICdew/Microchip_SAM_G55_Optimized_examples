/*
 * G55_Reset_Controller_example.c
 *
 * Created: 1/7/2016 1:14:45 PM
 * Author : jerome.semette
 */ 

#include "sam.h"

void console_init(uint32_t baudrate);
char console_getc();
void console_putc(char Char);
void console_puts(char *str);

void console_init(uint32_t baudrate) 
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
	USART7->US_BRGR = (SystemCoreClock / (baudrate*16) ) ;
	
	/* Enable receiver and transmitter */
	USART7->US_CR = US_CR_RXEN | US_CR_TXEN;	 
}

char console_getc() {
	while(!(USART7->US_CSR & US_CSR_RXRDY)){
		/* Update Watchdog to avoid unwanted reset */
		WDT->WDT_CR = (WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT);
	}
	return  (char) FLEXCOM7->FLEXCOM_RHR;
}


void console_putc(char Char) {
	while(!(USART7->US_CSR & US_CSR_TXRDY));
	FLEXCOM7->FLEXCOM_THR = (uint32_t) Char;
}

void console_puts(char  *str) {
	while(*str){
		console_putc(*str++);
	}
}

watchdog_init()
{
	/* Setup Watchdog reset and refresh window */ 
	WDT->WDT_MR = (WDT_MR_WDRSTEN | WDT_MR_WDDBGHLT | (0xFFF<< WDT_MR_WDD_Pos) | (0x3F<< WDT_MR_WDV_Pos)); 
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint32_t rst_source=0;
	uint8_t rst_choice; 
	uint8_t refresh_watchdog_disabled = 0;
	
    /* Initialize the SAM system */
    SystemInit();
	watchdog_init();
    uint8_t test_rx_char;
	SystemCoreClockUpdate();
	console_init(115200);
	console_puts("\n**** SAMG55 Reset Example ****\n");
	console_puts("\n\r Reset condition : ");
	
	/* check wake-up/reset source */
	rst_source = (RSTC->RSTC_SR & RSTC_SR_RSTTYP_Msk);
	switch (rst_source)	{
		
		/* Wake up from power off  */
		case RSTC_SR_RSTTYP_GENERAL_RST :	console_puts(" Wake up from power off \n\r ");
											break;
		
		/* wake up from Software Reset */
		case RSTC_SR_RSTTYP_SOFT_RST :		console_puts(" Wake up from Software reset \n\r ");
											break;
		
		/* wake up from Watchdog reset */
		case RSTC_SR_RSTTYP_WDT_RST :		console_puts(" Wake up from Watchdog reset \n\r ");
											break;
		
		/* wake up User Reset */
		case RSTC_SR_RSTTYP_USER_RST :		
											if (WDT->WDT_SR & WDT_SR_WDUNF)
												console_puts(" Wake up from User reset - WDT \n\r ");
											else 
												console_puts(" Wake up from User reset - NRST \n\r ");
											break;
	}
	
	
	
	/* Display example Menu */
	console_puts("h : Hard reset instructions \n\r " );
	console_puts("u : User Reset instructions \n\r " );
	console_puts("w : Perform a Watchdog Reset\n\r " );
	console_puts("s : Perform a Software Reset\n\r " );
	console_putc('>');
	
	/* Wait for reset selection*/
	rst_choice = console_getc();
	
	/* Display selected reset source*/
	console_putc(rst_choice);
	console_puts("\n\r");
	
	/* Perform action according to selected reset*/
	switch(rst_choice){
		 /* - Hardware reset selected : display instruction */ 
		 case 'h' :	console_puts("board power cycle required");
					break;
		 /* - User reset selected : display instruction */
		 case 'u' : console_puts("Wait for NRST button press");
					break;
		/* - Watchdog  reset selected : disable watchdog refresh from main */		
		 case 'w' : console_puts("Disable watchdog refresh");
					  refresh_watchdog_disabled = 1;
					break;
		/* - Software reset selected : perform a software reset */
		 case 's' : console_puts("Perform Software Reset using Reset controller");
					RSTC->RSTC_CR = (RSTC_CR_KEY_PASSWD|RSTC_CR_PROCRST|RSTC_CR_PERRST);
					break;
	}
	
	/* Infinite loop */ 
    while (1){
		if (!refresh_watchdog_disabled)
			WDT->WDT_CR = (WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT);
	}
}
