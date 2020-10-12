/*
 * US_sensor.c
 *
 *  Created on: Oct 2, 2020
 *      Author: nxf33313
 */
#include "derivative.h" /* include peripheral declarations */
#include "pit.h"

void US_sensor_output_init (void)
{
	SIM_SCGC5|=(1<<9);				//PORTA
	PORTA_PCR1=(1<<8);				//GPIO
	GPIOA_PDDR|=(1<<1);				//Output
	
	PIT_init();
}

void US_sensor_generate_pulse (void)
{
	PIT_LDVAL0=40;
	PIT_TCTRL0=(1<<31);
	GPIOA_PSOR=(1<<1);				//prender pin US
	do{}while (PIT_TFLG0==0);		//espera 10 us
	GPIOA_PCOR=(1<<1);				//apaga pin US
	PIT_TCTRL0&=~(1<<31);			//apaga timer
}
