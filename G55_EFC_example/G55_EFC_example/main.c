/*
 * G55_EFC_example.c
 *
 * Created: 6/1/2016 1:27:46 PM
 * Author : jerome.semette
 */ 


#include "sam.h"

#define RAMFUNC __attribute__ ((long_call, section (".ramsection")))

RAMFUNC void Flash_Write_Page(uint32_t Addr , uint32_t * WrBuff);
RAMFUNC void Flash_Erase_Page(uint32_t Addr);

RAMFUNC void Flash_Write_Page(uint32_t Addr , uint32_t * WrBuff) {
	uint32_t i;	
	uint32_t * pMem;
	uint32_t PageNum;
	
	// Compute page number from addr
	PageNum = (Addr - IFLASH_ADDR) / IFLASH_PAGE_SIZE;
	
	// Wait until Flash is ready
	while(!(EFC->EEFC_FSR & EEFC_FSR_FRDY));
	// Set page address (page aligned)
	pMem =  0x000000;
	
	// Fill EEFC Latch Buffer with New Page content
	for(i=0;i<128;i++){
		*pMem++ = WrBuff[i];
	}
	
	// Send Write Page command (write Latch buffer content in User page)
	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD|EEFC_FCR_FCMD_WP|(PageNum << EEFC_FCR_FARG_Pos) );
	while(!(EFC->EEFC_FSR & EEFC_FSR_FRDY));
}


RAMFUNC void Flash_Erase_Page(uint32_t Addr) {
	uint32_t PageNum;
	// Compute page number from Addr
	PageNum = (Addr - IFLASH_ADDR) / IFLASH_PAGE_SIZE;
	// Wait until Flash is ready
	while(!(EFC->EEFC_FSR & EEFC_FSR_FRDY));
	// Perform Erase page command
	EFC->EEFC_FCR = (EEFC_FCR_FKEY_PASSWD|EEFC_FCR_FCMD_EPA|(PageNum << EEFC_FCR_FARG_Pos));
	while(!(EFC->EEFC_FSR & EEFC_FSR_FRDY));
}

void HardFault_Handler( void ){
	while(1);
}



/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint32_t i;
	uint32_t addr;
	uint32_t Write_buff[128];
	
    /* Disable Watchdog */	
	WDT->WDT_MR |= WDT_MR_WDDIS;
	
	// Fill "Write_buff" buffer with values from 0x7F to 0x00
	for(i=0;i<128;i++){
		Write_buff[i] = 0xCAFECAFE;
		//Write_buff[i] = 0xDECADECA;
	}
	
    // Important address must be page aligned.
	// example : 0x47FE00 
	addr = 0x47FE00;

	// Erase page (executed from RAM)
	Flash_Erase_Page(addr);
		
	// Write page with "Write_buff" Buffer Content (executed from RAM)
	Flash_Write_Page(addr,Write_buff);
		
    while (1) ;
}