/*
 * PIT.c
 *
 *  Created on: Oct 2, 2020
 *      Author: nxf33313
 */
#include "derivative.h" /* include peripheral declarations */
#include "pit.h"

unsigned char ADC_value;

void PIT_init (void)
{
	SIM_SCGC6|=(1<<23);				//Clock PITs
	PIT_MCR&=~(1<<30);				//apaga bit 30, MDIS

//prender bit 10
//          bi31 bit 30         Bit11 bit 10 bit 9 ....
//     or   0    0	            0       1      0
//	reslt   b31 bit 30         bit 11   1    bit 9
	
// apagar bit 10 	
//          bi31 bit 30         Bit11 bit 10 bit 9 ....
//   and     1    1    11        1      0     1    11
//	reslt   b31 bit 30         bit 11   0    bit 9
}

void ADC_IRQHandler (void)
{
	ADC_value=ADC0_RA;
}
