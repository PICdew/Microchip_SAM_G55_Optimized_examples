/*
 * G55_RTC_example.c
 *
 * Created: 6/6/2016 3:40:02 PM
 * Author : jerome.semette
 */ 


/**
 * \file
 *
 * \brief This example shows how to configure SAM G53 RTC and generate interrupt on specific RTC event 
 *
 */

#include <sam.h>

void RTC_Handler(void)
{
	uint32_t status;
	status = RTC->RTC_SR;
	
	if (status & RTC_SR_SEC)
	{
		// Clear SEC flag 
		RTC->RTC_SCCR |= RTC_SCCR_SECCLR;
		
		// Toggle led
		if ( PIOA->PIO_ODSR & PIO_ODSR_P6)
			// LED ON
			PIOA->PIO_CODR = PIO_CODR_P6;
		else
			// LED OFF
			PIOA->PIO_SODR = PIO_SODR_P6;
	}
	
	if(status & RTC_SR_TIMEV)
	{
		// Clear IMEV flag 
		RTC->RTC_SCCR |= RTC_SCCR_TIMCLR;
		__NOP();
	}
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

void RTC_init(void)
{
	/* Disable RTC write protection */
	PMC->PMC_WPMR = (PMC_WPMR_WPKEY_PASSWD);
	// Enable RTC peripheral clock
	PMC->PMC_PCER0 |= ID_RTC;
	// Check stop possible ongoing RTC configuration request
	if(RTC->RTC_CR & (RTC_CR_UPDCAL|RTC_CR_UPDTIM))
	RTC->RTC_CR &= ~(RTC_CR_UPDCAL|RTC_CR_UPDTIM);
	// Configure/Update RTC (see flowchart from G55 data sheet)
	// 1 - request Configuration
	RTC->RTC_CR |= (RTC_CR_UPDCAL|RTC_CR_UPDTIM);
	// 2 - Wait for ack
	while (!(RTC->RTC_SR & RTC_SR_ACKUPD));
	// 3 - Clear ACK flag
	RTC->RTC_SCCR |= RTC_SCCR_ACKCLR;
	// 4 - Set TIME value
	RTC->RTC_TIMR = (( 0x11 << RTC_TIMR_HOUR_Pos) | ( 0x30 << RTC_TIMR_MIN_Pos) | ( 0x00 << RTC_TIMR_SEC_Pos));
	// 5 - Set CALENDAR value
	RTC->RTC_CALR = (( 0x85 << RTC_CALR_YEAR_Pos) | ( 0x19 << RTC_CALR_CENT_Pos) | ( 0x09 << RTC_CALR_MONTH_Pos) | ( 0x5 << RTC_CALR_DATE_Pos) | ( 0x1 << RTC_CALR_DAY_Pos));
	// Stop Configuration
	RTC->RTC_CR &= ~(RTC_CR_UPDCAL|RTC_CR_UPDTIM);
	// Enable Interrupt at NVIC level
	NVIC_EnableIRQ(RTC_IRQn);
	// Clear SEC flag
	RTC->RTC_SCCR = 0xFF;
	// Enable Interrupt at peripheral level
	RTC->RTC_IER = RTC_IER_SECEN | RTC_IER_TIMEN ;
}


int main (void)
{
		// Initialize Main clock at 24MHz
		SystemInit();
		// Configure LED
		Configure_GPIO();
		// Initialize RTC () 
		RTC_init();
		// Infinite Loop
		while(1);
}