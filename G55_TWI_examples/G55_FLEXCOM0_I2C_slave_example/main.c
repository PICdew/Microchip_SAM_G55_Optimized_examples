/*
 * G55_TWI0_slave_example.c
 *
 * Created: 11/18/2015 10:0:24 AM
 *  Author: jerome.semette
 */ 
/* This code Emulate an addressable I2C memory .
Test Setup with SAMG55 Xplained-PRO is the following .

		 EXT1
		 -----
		| 1 2 |
		| 3 4 |
		| 5 6 |
		| 7 8 |
		| 9 10|
		|11 12|
TWCK0 <=|13 14|=> TWD0
		|15 16|
		|17 18|
  GND <=|19 20|
		 -----

Code tested with a Totalphase-Aardvark
*/



#include "sam.h"

void I2C_init(void);
void TWI_EOSACC_callback();
void TWI_SVACC_callback();
void TWI_write_callback();
void TWI_read_callback();

volatile uint8_t databuff[1024] = {0};
volatile uint8_t first_data = 1;
volatile uint32_t intAddr = 0;

void FLEXCOM0_Handler(void){
	uint32_t status;
	status = TWI0->TWI_SR;
	/*** ERROR Interrupt Management ***/
	if (status & TWI_SR_OVRE)
		while(1);
	if (status & TWI_SR_ARBLST)
		while(1);
	/*** TWI Read Access detected ***/
	if ((status & TWI_SR_RXRDY)&&(status & TWI_SR_SVACC)) {
		TWI_read_callback();
	}
	/*** TWI Read Access detected ***/
	if (status & TWI_SR_EOSACC) {
		TWI_EOSACC_callback();
	}
	/*** TWI Read Access detected ***/
	if ((status & TWI_SR_SVACC)&&(status & TWI_SR_SVREAD)&&(status & TWI_SR_TXRDY)) {
		TWI_write_callback();	
	}
}

/* TWI call-back to be modified according to Application need (Default I2C Mem emulation) */
void TWI_EOSACC_callback(void) {
	first_data = 1;
}

void TWI_write_callback(void) {
	FLEXCOM0->FLEXCOM_THR = databuff[++intAddr];
}

void TWI_read_callback(void) {
	if (first_data){
		first_data = 0;
		intAddr = FLEXCOM0->FLEXCOM_RHR;
		FLEXCOM0->FLEXCOM_THR = databuff[intAddr];
	} else {
		databuff[intAddr++] = FLEXCOM0->FLEXCOM_RHR;
	}
}

void I2C_init (void){	
	/* TWD0 (PA10) and TWCK0 (PA9) pin init */
	PMC->PMC_PCER0 |= (1 << PIOA_IRQn);
	PIOA->PIO_PDR = (PIO_PDR_P9|PIO_PDR_P10);
	PIOA->PIO_ABCDSR[0] &= ~(PIO_ABCDSR_P9|PIO_ABCDSR_P10);
	PIOA->PIO_ABCDSR[1] &= ~(PIO_ABCDSR_P9|PIO_ABCDSR_P10);
	/* Disable internal pull-up on TWD0 (PA3) and TWCK0 (PA4) (use external one instead) */
	PIOA->PIO_PUDR |= (PIO_PUDR_P9|PIO_PUDR_P10);
	/* Enable TWI peripheral clock*/
	PMC->PMC_PCER0 |= (1 << FLEXCOM0_IRQn);
	/* Configure FLEXCOM0 mode to twi */
	FLEXCOM0->FLEXCOM_MR |= FLEXCOM_MR_OPMODE_TWI;
	/*Perform software reset*/
	TWI0->TWI_CR = TWI_CR_SWRST;
	/*Enable TWI0 standard interrupt and errors interrupts*/
	TWI0->TWI_IER = (TWI_IER_ARBLST | TWI_IER_NACK | TWI_IER_OVRE | TWI_IER_SVACC | TWI_IER_EOSACC | TWI_IER_RXRDY | TWI_IER_NACK);
	/* Dummy read in status register to clear pending interrupt */
	TWI0->TWI_SR;
	FLEXCOM0->FLEXCOM_THR ;
	NVIC_EnableIRQ(FLEXCOM0_IRQn);
	/*Configure TWI0 in slave mode*/
	TWI0->TWI_CR = TWI_CR_MSDIS|TWI_CR_SVEN;
	/*Configure TWI0 set slave address to 0x10*/
	TWI0->TWI_SMR |= TWI_SMR_SADR(0x10);
}

int main(void)
{
	int i;
	SystemInit();
    I2C_init();
	for (i;i<sizeof(databuff);i++){
		databuff[i] = i;	
	}
	/* I2C management performed in Interrupt handler*/
	while(1);
}




