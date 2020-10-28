/*
 * system_functions.c
 *
 *  Created on: Oct 21, 2020
 *      Author: jair2
 */

#include "derivative.h"
#include "system_functions.h"

void pulse_generator (void)
{
	//
	/*
	PIT_TCTRL1=(1<<0);				//Encender timer
	GPIOE_PSOR=(1<<2);				//prender pin US
	do{}while (PIT_TFLG1==0);		//espera 10 us
	GPIOE_PCOR=(1<<2);				//apaga pin US
	PIT_TCTRL1&=~(1<<0);			//apaga timer
	*/
	
	TPM1_CNT = 1;
	GPIOE_PDOR=(1<<2);
	TPM1_C1V=TPM1_CNT+40;		// 400 = 40 pulsos de 25 ns para generar un 1 us
	do{}while ((TPM1_C1SC & (1<<7))==0);
	TPM1_C1SC|=(1<<7);
	GPIOE_PDOR=0;
}

