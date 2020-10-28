/*
 * system_IRQ.c
 *
 *  Created on: Oct 21, 2020
 *      Author: jair2
 */

#include "derivative.h"
#include "system_IRQ.h"
#include "system_functions.h"

unsigned short periodoA;
unsigned short periodoB;
unsigned short tviejoA;
unsigned short tviejoB;
unsigned short frecuenciaA;	
unsigned short frecuenciaB;
unsigned long rpmA;
unsigned long rpmB;


void FTM0_IRQHandler(void)
{
	
	if ((1<<7) == (TPM0_C0SC & (1<<7) ))
	{
		TPM0_C0SC|=(1<<7);
		
		periodoA=11*468*(TPM0_C0V-tviejoA);
		frecuenciaA=40000000/periodoA;
		rpmA=60*frecuenciaA;
		tviejoA=TPM0_C0V;
	}
	
	if ((1<<7) == (TPM0_C1SC & (1<<7) ) )
	{
		TPM0_C1SC|=(1<<7);
		periodoB=11*468*(TPM0_C0V-tviejoB);
		frecuenciaB=40000000/periodoB;
		rpmB=60*frecuenciaB;
		tviejoB=TPM0_C0V;
		
		
	}
}


void PIT_IRQHandler()
{
	unsigned short t1;
	unsigned short t2;
	unsigned short tiempo_us;
	unsigned short distancia;
	unsigned char paro;
	
	PIT_TFLG0|=(1<<0);
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
	
}
