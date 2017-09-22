/*
 * SAM4S_PLL_120MHz_MCK_example.c
 *
 * Created: 12/1/2014 11:11:24 AM
 *  Author: jerome.semette
 */ 

#include "sam.h"

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */


int main(void)
{
	/* Disable watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
	/* Switch to external 32KHz Oscilator*/
	SUPC->SUPC_CR = (SUPC_CR_KEY_PASSWD | SUPC_CR_XTALSEL);
	while(!(SUPC->SUPC_SR & SUPC_SR_OSCSEL));
	/* Configure PLLA to generate 120MHz (MUL=10 ; DIV=1) */
	PMC->CKGR_PLLAR = (CKGR_PLLAR_MULA(3750) | CKGR_PLLAR_PLLACOUNT(0x3fU)| CKGR_PLLAR_PLLAEN(1)) ;
	/*Wait until PLLA is Locked */
	while (!(PMC->PMC_SR & PMC_SR_LOCKA));
	/* Switch to PLLA */
	//PMC->PMC_MCKR =  (PMC_MCKR_PRES_CLK_1 | PMC_MCKR_CSS_PLLA_CLK) ;
	/* Wait until MCK is ready */
	while (!(PMC->PMC_SR & PMC_SR_MCKRDY));
    /* Configure PA17 in Peripheral B mode for PCK0 output */
	PMC->PMC_PCER0 |= (1 << PIOA_IRQn);
	PIOA->PIO_PDR = PIO_PDR_P17;
	PIOA->PIO_ABCDSR[0] |= PIO_ABCDSR_P17;
	PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P17);
	/* Configure PCK1 to output MCK */
	PMC->PMC_PCK[1] = (PMC_PCK_CSS_PLLA_CLK | PMC_PCK_PRES(120));
	/* Enable PCK output */
	PMC->PMC_SCER = PMC_SCER_PCK1;
	while(1);
}
