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
	PIT_TCTRL1=(1<<0);				//Encender timer
	GPIOE_PSOR=(1<<2);				//prender pin US
	do{}while (PIT_TFLG1==0);		//espera 10 us
	GPIOE_PCOR=(1<<2);				//apaga pin US
	PIT_TCTRL1&=~(1<<0);			//apaga timer
}

