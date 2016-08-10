/*
 * G55_basic_low_power.c
 *
 * Created: 5/31/2016 6:04:35 PM
 * Author : jerome.semette
 */ 


#include "sam.h"
#define RAMFUNC __attribute__ ((long_call, section (".ramsection")))

void Sleep_mode(void);
void Configure_GPIO(void);

volatile uint8_t SW0_Flag = 0 ;

void Configure_GPIO(void)
{
	/* Enable PIOA Clock */
	PMC->PMC_PCER0 = (0x1 << PIOA_IRQn);
	/* Configure PA16 for Driving LED0  */
	PIOA->PIO_OER = PIO_OER_P6;
	PIOA->PIO_PUDR = PIO_PUDR_P6;
	/* Indicator Led ON */ 
	PIOA->PIO_CODR = PIO_ODR_P6;
	/*enable pull-up on PA2 (SW0)*/
	PIOA->PIO_PUER = PIO_PUER_P2;
	/* Enable falling edge detection on PA2 (SW0) */
	PIOA->PIO_ESR = PIO_ESR_P2;
	PIOA->PIO_FELLSR = PIO_FELLSR_P2;
	PIOA->PIO_AIMER = PIO_AIMER_P2;
	/* Enable glitch Filter on PA2 (SW0) */
	PIOA->PIO_IFER = PIO_IFER_P2;
	/* Enable interrupt on PA02 (SW0) */
	PIOA->PIO_IER = PIO_IER_P2;
	/* Enable PIOA interrupt at core level (NVIC) */
	NVIC_EnableIRQ(PIOA_IRQn);	
}

void PIOA_Handler(void)
{
	/* Clear PIOA interrupt flag */
	PIOA->PIO_ISR;
	/* Set Push button detection flag */
	SW0_Flag = 1;
}

 void Sleep_mode(void)
 {		        
	PMC->PMC_FSMR &= PMC_FSMR_LPM;
	/* Clear Sleepdeep bit */ 
	SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
	__WFI();
	/*** MCU IN SLEEP MODE until interrupt detected (SW0 pressed) ***/
}


/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
 {

	/* Initialize clock of the system */
	SystemInit();
	/* Initialize GPIO of the system */	 
 	Configure_GPIO();
	/* Enter main program loop */	
    while (1) 
	{
	  SW0_Flag = 0;
	  /* Indicator Led ON */ 
	  PIOA->PIO_CODR = PIO_CODR_P6;
	  // Wait until SW0 button is pressed
	  while(!SW0_Flag);
	  /* Indicator Led OFF */ 
	  PIOA->PIO_SODR = PIO_SODR_P6;
	  // Enter Sleep mode		
      Sleep_mode();
    }
}