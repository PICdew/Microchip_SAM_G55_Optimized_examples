/*
 * G55_12-bit_ADC_example.c
 *
 * Created: 6/1/2016 1:51:43 PM
 * Author : jerome.semette
 */ 

#include "sam.h"

volatile uint16_t ADC_Result; // Analog to Digital conversion result


 void Configure_TC0 (void) {
	 
	 	/* Disable Disable PA0 control at controller level */
	 	PIOA->PIO_PDR = PIO_PDR_P0;
	 	/* Set PA0 mux to peripheral B (TIOA0) */
	 	PIOA->PIO_ABCDSR[0] |= PIO_ABCDSR_P0;
	 	PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P0);
	 	/* Disable pull-up and pull-down on PA0(TIOA0) */
	 	PIOA->PIO_PPDDR |= PIO_PPDDR_P0;
	 	PIOA->PIO_PUDR |= PIO_PUDR_P0;
	 
	    /*Enable TC0 Clock */
	    PMC->PMC_PCER0 = (0x1 << TC0_IRQn) ;  // TC0 Peripheral ID = 23
		/* Configure the TC0 to count at 12MHz (24 MHz / 2) and generate TIOA signal on RA and RC compare event  */
		TC0->TC_CHANNEL[0].TC_CMR = (TC_CMR_TCCLKS_TIMER_CLOCK3  | TC_CMR_ACPC_SET | TC_CMR_WAVE | TC_CMR_ACPA_CLEAR | TC_CMR_WAVSEL_UP_RC  );
		/*configure TIO output at 12MHz/24 = 50kHz  */
		TC0->TC_CHANNEL[0].TC_RA = (uint32_t)12;
		TC0->TC_CHANNEL[0].TC_RC = (uint32_t)24;	
}

static void Configure_ADC(void)
{
	
	/* Disable pull-up on ADC channel 2 line */
	PIOA->PIO_SODR = PIO_SODR_P19;
	
	/*Enable ADC PCK */
	PMC->PMC_PCER0 = (0x1 << ADC_IRQn); 
	/* Setup ADC PCK as ADC input clock  */
	ADC->ADC_EMR |= ADC_EMR_SRCCLK_PMC_PCK;
	/* Setup ADCCLK at 12MHz (ADCCLK = PCK/(2(PRESCAL+1)) */
	ADC->ADC_MR |= ADC_MR_PRESCAL(0);  
	/* Set ADC Hardware trigger to TC0 TIOA output*/
    ADC->ADC_MR |= ADC_MR_TRGSEL_ADC_TRIG1; 
	/* Set ADC start-up to 0 periods of ADCCLK*/
	ADC->ADC_MR |= ADC_MR_STARTUP_SUT0 ;
	/* Set Track time to 1 ADCCLK(TRACKTIM + 1) x ADCCLK periods) */
	ADC->ADC_MR |= (0x0 << ADC_MR_TRACKTIM_Pos) ;
    /* Disable PDC for ADC */
    ADC->ADC_PTCR = ADC_PTCR_RXTDIS; 
	/* Perform dummy read in Last converted Data register to clear pending int. flag */
	ADC->ADC_LCDR;
	/* Enable ADC channel 2 and General Overrun interrupt */ 
    ADC->ADC_IER |= (ADC_IER_GOVRE| ADC_IER_EOC2 ) ;
	/* Enable general ADC interrupt (ID 29)*/ 
    NVIC_EnableIRQ(ADC_IRQn);
	/* Enable internal hardware Trig*/ 
    ADC->ADC_MR |= ADC_MR_TRGEN;
	/* Enable ADC channel 2 input (AD2 <=> PA19 <=> Pin 3 on EXT3 connector of the G55 Xplained Pro) */
    ADC->ADC_CHER = ADC_CHER_CH2; 
}

/*
 * Handles interrupts coming from the ADC.
 */
void ADC_Handler( void )
{
    uint32_t isr ;
	/* Read Interrupt status register*/
    isr = ADC->ADC_ISR;
    /* Check if RXBUFF interrupt appears */
    if (isr & ADC_IER_EOC2)
    {
	  //Read Result from channel 2
      ADC_Result = (ADC->ADC_CDR[2] & ADC_CDR_DATA_Msk); 
	}
	/* Check if General Overrun interrupt appears */
    if (isr & ADC_ISR_GOVRE)
    {
		/** Debug ** Put a breakpoint there to monitor General overrun */ 
		 while(1);
    } 	  
}


void main( void )
{
   // Initialize System (PLL config + Watchdog disable)
   SystemInit();
   // configure ADC 
   Configure_ADC(); 
   // configure Timer counter 0   
   Configure_TC0();
   // Start Timer counter 0
   TC0->TC_CHANNEL[0].TC_CCR = (TC_CCR_CLKEN | TC_CCR_SWTRG); 
   while(1);
}