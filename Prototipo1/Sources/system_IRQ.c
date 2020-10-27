/*
 * system_IRQ.c
 *
 *  Created on: Oct 21, 2020
 *      Author: jair2
 */

#include "derivative.h"
#include "system_IRQ.h"
#include "system_functions.h"

extern unsigned char duty_cycleA;
extern unsigned char duty_cycleB;
extern unsigned char espejo_pinA;
extern unsigned char espejo_pinB;

extern unsigned short periodoA;
extern unsigned short periodoB;
extern unsigned short tviejoA;
extern unsigned short tviejoB;
extern unsigned short frecuenciaA;	
extern unsigned short frecuenciaB;
extern unsigned long rpmA;
extern unsigned long rpmB;

void FTM0_IRQHandler(void)
{
	unsigned short periodo = 4000;
	
	if ((1<<7) == (TPM0_C0SC & (1<<7) ))
	{
		TPM0_C0SC|=(1<<7);
		
		if (espejo_pinA==0)
		{
			espejo_pinA=1;
			TPM0_C0V+=periodo*(duty_cycleA)/100;		
		}
		else
		{
			espejo_pinA=0;
			TPM0_C0V+=periodo*(100-duty_cycleA)/100;
		}
	}
	if ((1<<7) == (TPM0_C1SC & (1<<7) ) )
	{
		TPM0_C1SC|=(1<<7);
		
		if (espejo_pinB==0)
		{
			espejo_pinB=1;
			TPM0_C1V+=periodo*(duty_cycleB)/100;		
		}
		else
		{
			espejo_pinB=0;
			TPM0_C1V+=periodo*(100-duty_cycleB)/100;
		}
	}
	
}

void FTM1_IRQHandler(void)
{
	
	if ((1<<7) == (TPM1_C0SC & (1<<7) ))
	{
		TPM1_C0SC|=(1<<7);
		
		periodoA=11*468*(TPM1_C0V-tviejoA);
		frecuenciaA=40000000/periodoA;
		rpmA=60*frecuenciaA;
		tviejoA=TPM0_C0V;
	}
	
	if ((1<<7) == (TPM1_C1SC & (1<<7) ) )
	{
		TPM1_C1SC|=(1<<7);
		periodoB=11*468*(TPM1_C0V-tviejoB);
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
	extern unsigned short distancia;
	extern unsigned char paro;
	
	PIT_TFLG0=(1<<0);
	pulse_generator ();
	do{}while ((TPM0_C2SC & (1<<7))==0);  //esperando flanco de subida
	TPM0_C2SC|=(1<<7);
	t1=TPM0_C2V;
	TPM0_C2SC=(2<<2);		//flanco de bajada
	do{}while ((TPM0_C2SC & (1<<7))==0);  //esperando flanco de bajada
	TPM1_C0SC|=(1<<7);
	t2=TPM1_C0V;
	TPM0_C2SC=(1<<2);
	tiempo_us= ((t2-t1)*25)/1000;
	distancia=tiempo_us/58;
	
	if (distancia<50)
	{
		paro =1;
		duty_cycleA = 0;
	}
	else
	{
		paro = 0;
		duty_cycleB = 0;
	}

	
	
}
