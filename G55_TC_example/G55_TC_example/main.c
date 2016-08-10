/*
 * G55_TC_example.c
 *
 * Created: 1/7/2016 1:17:37 PM
 * Author : jerome.semette
 */ 


#include "sam.h"

void TC0_channel0_waveform_init( void );
void TC0_channel1_capture_init( void );
volatile uint32_t Signal_freq;

void TC1_Handler( void ){
	uint32_t status;
	/*Get TC0 Channel 1 Status */
	status = TC0->TC_CHANNEL[1].TC_SR;
	/* if capture on TIOA detected */
	if(status & TC_SR_LDRAS)
	{
		Signal_freq = 8000000 / (TC0->TC_CHANNEL[1].TC_RA) ;
	}
}


void TC0_channel0_waveform_init( void ){
	/* Disable Disable PA0 control at controller level */
	PIOA->PIO_PDR = PIO_PDR_P0;
	/* Set PA0 mux to peripheral B (TIOA0) */
	PIOA->PIO_ABCDSR[0] |= PIO_ABCDSR_P0;
	PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P0);
	/* Disable pull-up and pull-down on PA0(TIOA0) */
	PIOA->PIO_PPDDR |= PIO_PPDDR_P0;
	PIOA->PIO_PUDR |= PIO_PUDR_P0;
	
	/*Enable TC0 peripheral clock if not already done*/
	if (!(PMC->PMC_PCSR0 & (1<<TC0_IRQn)))
	PMC->PMC_PCER0 = (1<<TC0_IRQn);
	/*Disable TC0 channel 0 Clock*/
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	/* Set TC0 Channel 0 in WAVEFORM mode*/
	TC0->TC_CHANNEL[0].TC_CMR |= TC_CMR_WAVE;
	/* Set MCK/8 as TC0 Channel 0 clock (TC0C0 clock = 2MHz)*/
	TC0->TC_CHANNEL[0].TC_CMR |= TC_CMR_TCCLKS_TIMER_CLOCK2;
	/* TC0 Channel 0 counter Reset on RC */
	TC0->TC_CHANNEL[0].TC_CMR |= TC_CMR_WAVSEL_UP_RC;
	/* Set TIOA0 toggle on TC0 Channel0 RC Compare effect*/
	TC0->TC_CHANNEL[0].TC_CMR |= TC_CMR_ACPC_TOGGLE;
	/* Set TIOA0 frequency to 1kHz */
	TC0->TC_CHANNEL[0].TC_RC = 3000;
	/* Enable TC0 Channel 0 Clock*/
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN ;
	/* Software trigg TC0 channel0*/
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG ;
}

void TC0_channel1_capture_init( void ){
	/* Disable Disable PA23(TIOA1) control at controller level */
	PIOA->PIO_PDR = PIO_PDR_P23;
	/* Set PA23 mux to peripheral B (TIOA1) */
	PIOA->PIO_ABCDSR[0] |= PIO_ABCDSR_P23;
	PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P23);
	/* Disable pull-up and pull-down on PA23(TIOA1) */
	PIOA->PIO_PPDDR |= PIO_PPDDR_P23;
	PIOA->PIO_PUDR |= PIO_PUDR_P23;
	
	/*Enable TC0 Channel 1 peripheral clock if not already done*/
	if (!(PMC->PMC_PCSR0 & (1<<TC1_IRQn)))
	PMC->PMC_PCER0 = (1<<TC1_IRQn);
	/*Disable TC0 channel 1 Clock*/
	TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	/* Set TC0 Channel 1 in CAPTURE mode*/
	TC0->TC_CHANNEL[1].TC_CMR &= ~(TC_CMR_WAVE);
	/* Set MCK/2 as TC0 Channel 1 clock (TC0C1 clock = 4MHz)*/
	TC0->TC_CHANNEL[1].TC_CMR |= TC_CMR_TCCLKS_TIMER_CLOCK1;
	/* Set TIOA Rising edge as Counter RESET trigger input*/
	TC0->TC_CHANNEL[1].TC_CMR |= TC_CMR_ABETRG;
	TC0->TC_CHANNEL[1].TC_CMR &= ~(TC_CMR_ETRGEDG_Msk);
	TC0->TC_CHANNEL[1].TC_CMR |= TC_CMR_ETRGEDG_RISING;
	/* Set TIOA falling edge as capture event */
	TC0->TC_CHANNEL[1].TC_CMR |= TC_CMR_LDRA_RISING;
	/* Enable interrupt on RA capture */
	TC0->TC_CHANNEL[1].TC_IER |= TC_IER_LDRAS;
	NVIC_EnableIRQ(TC1_IRQn);
	/* Enable TC0 Channel 1 Clock*/
	TC0->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN ;
}




int main(void)
{
	/* Initialize SAM G55 to run at 8MHz*/
	SystemInit();

	TC0_channel0_waveform_init();
	TC0_channel1_capture_init();

	/* Replace with your application code */
	while (1);
}
