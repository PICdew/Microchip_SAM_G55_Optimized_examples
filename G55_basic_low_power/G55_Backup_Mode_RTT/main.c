/*
 * G55_Backup_Mode_RTC.c
 *
 * Created: 6/7/2016 3:35:59 PM
 * Author : jerome.semette
 */ 

#include "sam.h"

void Wait_mode(void);
void Configure_GPIO(void);
void RTT_init(void);
volatile uint8_t RTT_Flag = 0 ;

void Configure_GPIO(void)
{
	/* Enable PIOA Clock */
	PMC->PMC_PCER0 = (0x1 << PIOA_IRQn);
	/* Configure PA16 for Driving LED0  */
	PIOA->PIO_OER = PIO_OER_P6;
	PIOA->PIO_PUDR = PIO_PUDR_P6;
	/* Indicator Led ON */ 
	PIOA->PIO_CODR = PIO_ODR_P6;
}

void RTT_Handler(void)
{
	/*Clear RTT interrupt Flags*/
	RTT->RTT_SR;
	/* Set apps flag*/
	RTT_Flag = 1;
	RTT_init();
}


void RTT_init(void)
{
	/* Disable RTT */
	RTT->RTT_MR |= RTT_MR_RTTDIS;
	/* Set RTT prescaler to 0x8000 (1Hz count) */
	RTT->RTT_MR |= RTT_MR_RTPRES(0x8000);
	/* Set Alarm when counter reach 5 (5 seconds) */
	RTT->RTT_AR = RTT_AR_ALMV(0x5);
	/* Enable Alarm interrupt at peripheral level */
	RTT->RTT_MR |= RTT_MR_ALMIEN;
	/* Enable RTT interrupt at core level */
	NVIC_EnableIRQ(RTT_IRQn);
	/* Reset RTT counter value */
	RTT->RTT_MR |= (RTT_MR_RTTRST);
	/* Enable RTT */
	RTT->RTT_MR &= ~(RTT_MR_RTTDIS);
}

 void Backup_mode(void)
 {	
	 /* Enable Wake-up on RTT Alarm */
	 SUPC->SUPC_WUMR = SUPC_WUMR_RTTEN;
	 SUPC->SUPC_CR = (SUPC_CR_KEY_PASSWD|SUPC_CR_VROFF);
	 while(1);
 }

int main(void)
{
	/* Initialize clock of the system */
	SystemInit();
	/* Configure LED0 pin*/
	Configure_GPIO();
	/* Initialize RTC to generate interrupt every 5 seconds */
	RTT_init();
	/* Enter main program loop */	
    while (1) 
	{
	  RTT_Flag = 0;
	  /* Indicator Led ON */ 
	  PIOA->PIO_CODR = PIO_CODR_P6;
	  // Wait until SW0 button is pressed
	  while(!RTT_Flag);
	  /* Indicator Led OFF */ 
	  PIOA->PIO_SODR = PIO_SODR_P6;
	  // Enter Sleep mode		
      Backup_mode();
    }
}
