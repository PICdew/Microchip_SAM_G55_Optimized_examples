/*
 * G55_RTT_example.c
 *
 * Created: 6/7/2016 2:36:38 PM
 * Author : jerome.semette
 */ 


#include "sam.h"

void RTT_init(void);

RTT_Handler()
{
	/*Clear RTT Flags*/
	RTT->RTT_SR;
	/* toggle LED0 */
	if ( PIOA->PIO_ODSR & PIO_ODSR_P6)
		PIOA->PIO_CODR = PIO_CODR_P6; // LED ON
	else
		PIOA->PIO_SODR = PIO_SODR_P6; // LED OFF
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

void Configure_GPIO(void)
{
	/*Enable PIOA Clock */
	PMC->PMC_PCER0 = (0x1 << PIOA_IRQn);
	/* Configure PA6 for Driving LED0  */
	PIOA->PIO_OER = PIO_OER_P6;
	PIOA->PIO_PUDR = PIO_PUDR_P6;
	/* Indicator Led ON */
	PIOA->PIO_CODR = PIO_ODR_P6;
}



int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	/* Configure LED0 pin*/
	Configure_GPIO();
	/* Initialize RTC to generate interrupt every 5 seconds */
	RTT_init();
    while (1);
}
