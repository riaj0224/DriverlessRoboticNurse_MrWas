/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */
#include "system_init.h"
#include "system_IRQ.h"
#include "system_functions.h"

unsigned char espejo_pinA = 0;//x
unsigned char espejo_pinB = 0;//x
unsigned char duty_cycleA = 1;//x
unsigned char duty_cycleB = 1;//x
unsigned short tviejoA=0;//x
unsigned short tviejoB=0;//x
unsigned short frecuenciaA;//x
unsigned short frecuenciaB;//x
unsigned long rpmA;
unsigned long rpmB;
unsigned short periodoA;//x
unsigned short periodoB;//x
unsigned short distancia = 100;
unsigned char paro = 0;

void FTM0_IRQHandler(void);
void FTM1_IRQHandler(void);
void PIT_IRQHandler(void);

int main(void)
{
	int counter = 0;
	
	PORT_init();
	CLK_init ();
	TPM0_init ();
	TPM1_init ();
	PIT_init ();
	for(;;) {	   
	   	counter++;
	}
	
	return 0;
}
