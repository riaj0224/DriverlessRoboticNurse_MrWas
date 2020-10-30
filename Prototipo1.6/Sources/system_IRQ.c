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

extern unsigned short duty_cycleA;							//The duty_cycle of motor1
extern unsigned short duty_cycleB;							//The duty cycle of motor2


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
		duty_cycleA = 0;
		duty_cycleB = 0;
		TPM1_C0V=((periodo*(100-duty_cycleA))/100);
		TPM2_C0V=((periodo*(100-duty_cycleB))/100);
	}
	else
	{
		paro = 0;
		
	}
	
	
	
	
	
}
