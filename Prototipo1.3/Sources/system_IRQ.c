/*
 * system_IRQ.c
 *
 *  Created on: Oct 21, 2020
 *      Author: jair2
 */

#include "derivative.h"
#include "system_IRQ.h"
#include "system_functions.h"

unsigned short countA = 0;	
unsigned short countB = 0;
unsigned long rpmA;
unsigned long rpmB;

unsigned short t1;
unsigned short t2;
unsigned short tiempo_us;
unsigned short distancia;
unsigned char paro;
int rv;

void FTM0_IRQHandler(void)
{
	if ((1<<7) == (TPM0_C0SC & (1<<7) ))
	{
		TPM0_C0SC|=(1<<7);
		
		countA++;

	}
	
	if ((1<<7) == (TPM0_C1SC & (1<<7) ) )
	{
		TPM0_C1SC|=(1<<7);
		
		countB++;
		
	}
}


void PIT_IRQHandler()
{

	PIT_TFLG0|=(1<<0);
	
	rv = I2C0_burstRead();
	
	/*
	rpmA = ((60*countA*100*26)/(11*468*17));
	countA = 0;
			
	rpmB = ((60*countB*100*26)/(11*468*17));
	countB = 0;
	
	
	pulse_generator ();
	do{}while ((TPM0_C2SC & (1<<7))==0);  //esperando flanco de subida
	TPM0_C2SC|=(1<<7);
	TPM0_C2SC&=~(1<<2);
	TPM0_C2SC=(1<<3);		//flanco de bajada
	t1=TPM0_C2V;
	do{}while ((TPM0_C2SC & (1<<7))==0);  //esperando flanco de bajada
	TPM0_C2SC|=(1<<7);
	t2=TPM0_C2V;
	TPM0_C2SC&=~(1<<3);
	TPM0_C2SC=(1<<2);
	tiempo_us= ((t2-t1)*250)/1000;
	distancia=tiempo_us/58;
	TPM0_CNT = 1;
	
	if (distancia<50)
	{
		paro =1;
	}
	else
	{
		paro = 0;
	}
	*/
	
	
	
	
}
