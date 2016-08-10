/*
 * G55_Backup_Mode.c
 *
 * Created: 6/6/2016 2:05:15 PM
 * Author : jerome.semette
 */ 


#include "sam.h"

void Wait_mode(void);
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
	PIOA->PIO_IFSCER = PIO_IFSCER_P2;
	PIOA->PIO_SCDR = PIO_SCDR_DIV((32768 /(2 * (10))) - 1);
	/* Clear any pending interrupt on PIOA */
	PIOA->PIO_ISR;
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

 void Backup_mode(void)
 {	
	 /* Enable Wake-up on pin PA2 */
	 SUPC->SUPC_WUIR = (SUPC_WUIR_WKUPEN2_ENABLE|SUPC_WUIR_WKUPT2_LOW);
	 SUPC->SUPC_WUMR = SUPC_WUMR_WKUPDBC(3);
	 SUPC->SUPC_CR = (SUPC_CR_KEY_PASSWD|SUPC_CR_VROFF);
	 while(1);
 }

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
      Backup_mode();
    }
}
