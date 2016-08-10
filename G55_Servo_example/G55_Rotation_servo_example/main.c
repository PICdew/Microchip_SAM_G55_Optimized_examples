/*
 * G55_TC_example.c
 *
 * Created: 30/5/2016 
 * Author : jerome.semette
 */ 


#include "sam.h"

void Servo_init( void );
uint8_t Set_servo_value(uint8_t serv_value);

void Servo_init( void ){
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
	/* Set MCK/8 as TC0 Channel 0 clock (TC0C0 clock = 3MHz)*/
	TC0->TC_CHANNEL[0].TC_CMR |= TC_CMR_TCCLKS_TIMER_CLOCK2;
	/* TC0 Channel 0 counter Reset on RC */
	TC0->TC_CHANNEL[0].TC_CMR |= TC_CMR_WAVSEL_UP_RC;
	/* Set TIOA0 toggle on TC0 Channel0 RC Compare effect*/
	TC0->TC_CHANNEL[0].TC_CMR |= TC_CMR_ACPA_CLEAR;
	TC0->TC_CHANNEL[0].TC_CMR |= TC_CMR_ACPC_SET;
	/* Set Servo signal frequency to 50Hz */
	TC0->TC_CHANNEL[0].TC_RC = 60000;
	/* Set Pulse to 1500us (HSR1425CR rotation servo stopped) */
	TC0->TC_CHANNEL[0].TC_RA = 3960;
	/* Enable TC0 Channel 0 Clock*/
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN ;
	/* Software trigg TC0 channel0*/
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG ;
}

uint8_t Set_servo_value(uint8_t serv_value)
{
	/* HSR1425CR rotation servo : Pulse vs Shaft RPM 
	1320us -48.4 RPM (max)
	1390us -44.1 RPM
	1410us -40.2 RPM
	1450us -23.4 RPM
	1470us -11.4 RPM
	1480us  -5.5 RPM
	1500us     0 RPM
	1520us  +5.5 RPM
	1530us +11.4 RPM
	1550us +23.4 RPM
	1590us +40.2 RPM
	1610us +44.1 RPM
	1680us +48.4 RPM (max) */
	
	if((serv_value < 0)||(serv_value > 100))
	return -1;
	/*Convert value in percent in TC pulses (range )*/ 
	TC0->TC_CHANNEL[0].TC_RA = (serv_value * 11) + 4000 ;
	return 0;
}

int main(void)
{
	volatile uint32_t i,y;
	/* Initialize SAM G55 to run at 24MHz*/
	SystemInit();
	/* Initialize Servo */
	Servo_init();

	/* Test HSR1425CR rotation servo*/
	while (1){
		for(i=1;i<100;i++)
		{
			Set_servo_value(i);
			for(y=0;y<100000;y++);
		}
		for(i=100;i>0;i--)
		{
			Set_servo_value(i);
			for(y=0;y<1000000;y++);
		}				
	}
}
