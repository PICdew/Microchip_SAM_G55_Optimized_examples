/*
 * G55_TCM_Example.c
 *
 * Created: 2/16/2016 12:49:05 PM
 * Author : jerome.semette
 */ 


#include "sam.h"

#define DATA_TCM __attribute__ ((section (".tcm")))

DATA_TCM uint8_t  test_wr_buffer1[8];
DATA_TCM uint16_t test_wr_buffer2[4];
DATA_TCM uint32_t test_wr_buffer3[2];
uint8_t  test_rd_buffer1[8];
uint16_t test_rd_buffer2[4];
uint32_t test_rd_buffer3[2];


void CMCC_init(void)
{
	uint32_t *pDest;
	
	// Configure Data Cache/ TCM RAM power switch
	SUPC->SUPC_MR |=  SUPC_MR_CDPSWITCH_ON; // Cache blocks enabled
	//Ensure that Cache is disabled
	if(CMCC->CMCC_SR & CMCC_SR_CSTS);
		CMCC->CMCC_CTRL &= ~(CMCC_CTRL_CEN);
	//Configure Cache for Instruction only with size of 2KB
	CMCC->CMCC_CFG = (CMCC_CFG_PRGCSIZE(1) | CMCC_CFG_DCDIS);
	//Enable Cache controller
	CMCC->CMCC_CTRL |= CMCC_CTRL_CEN;
	/*Clear TCM area */
	for (pDest = (uint32_t *)0x1FC00800 ; pDest < (uint32_t *)0x1FC04000; pDest++) {
		*pDest = 0xFFFFFFFF;
	}
}

int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	// Initialize CMCC (2KB ICACHE and 14KB DTCM)
	CMCC_init();
	
	//Test 1 : multiple 8bit write access  
	test_wr_buffer1[0]=0xCA;
	test_wr_buffer1[1]=0xFE;
	test_wr_buffer1[2]=0xDE;
	test_wr_buffer1[3]=0xCA;
	test_wr_buffer1[4]=0xCA;
	test_wr_buffer1[5]=0xFE;
	test_wr_buffer1[6]=0xDE;
	test_wr_buffer1[7]=0xCA;	
	
	//Test 2 : multiple 16bit write access  
	test_wr_buffer2[0]=0xCAFE;
	test_wr_buffer2[1]=0xDECA;
	test_wr_buffer2[2]=0xCAFE;
	test_wr_buffer2[3]=0xDECA;

	//Test 3 : multiple 32bit write access  
	test_wr_buffer3[0]=0xCAFEDECA;
	test_wr_buffer3[1]=0xCAFEDECA;
	
	//Test 4 : multiple 8bit read access  
	test_rd_buffer1[0]=test_wr_buffer1[0];
	test_rd_buffer1[1]=test_wr_buffer1[1];
	test_rd_buffer1[2]=test_wr_buffer1[2];
	test_rd_buffer1[3]=test_wr_buffer1[3];
	test_rd_buffer1[4]=test_wr_buffer1[4];
	test_rd_buffer1[5]=test_wr_buffer1[5];
	test_rd_buffer1[6]=test_wr_buffer1[6];
	test_rd_buffer1[7]=test_wr_buffer1[7];	
	
	//Test 5 : multiple 16bit read access  
	test_rd_buffer2[0]=test_wr_buffer2[0];
	test_rd_buffer2[1]=test_wr_buffer2[1];
	test_rd_buffer2[2]=test_wr_buffer2[2];
	test_rd_buffer2[3]=test_wr_buffer2[3];
	
	//Test 6 : multiple 32bit read access  
	test_rd_buffer3[0]=test_wr_buffer3[0];
	test_rd_buffer3[1]=test_wr_buffer3[1];

/* Replace with your application code */
    while (1);
}
