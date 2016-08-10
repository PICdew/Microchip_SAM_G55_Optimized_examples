/*
 * G55_basic_interrupt_example.c
 *
 * Created: 6/1/2016 9:55:54 AM
 * Author : jerome.semette
 */ 


#include "sam.h"

void Configure_GPIO(void);
volatile uint8_t SW0_Flag ;

void Configure_GPIO(void)
{
	/* Enable PIOA Clock */
	PMC->PMC_PCER0 = (0x1 << PIOA_IRQn);
	
	/* Configure PA16 for Driving LED0  */
	PIOA->PIO_OER = PIO_OER_P6;
	PIOA->PIO_PUDR = PIO_PUDR_P6;
		
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
	
	/* Enable PIOA IRQ at core level (NVIC) */
	NVIC_EnableIRQ(PIOA_IRQn);	
}

void PIOA_Handler(void)
{
	/* Clear PIOA interrupt flag */
	PIOA->PIO_ISR;
	/* Toggle LED0 */
	if(PIOA->PIO_ODSR & PIO_ODSR_P6){
		PIOA->PIO_CODR = PIO_CODR_P6;
	} else {
		PIOA->PIO_SODR = PIO_SODR_P6;
	}
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{

	/* Initialize GPIO of the system */	 
 	Configure_GPIO();
	/* Enter main program loop */	
    while (1);
	
}
