/*
 * G55_Sleep_Mode_RTC.c
 *
 * Created: 7/17/2017 6:07:42 PM
 * Author : M43472
 */ 


#include "sam.h"

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
	RTC->RTC_IER = RTC_IER_TIMEN ;
}


int main(void)
{
    /* Initialize the SAM system */
    SystemInit();

    /* Replace with your application code */
    while (1) 
    {
    }
}
