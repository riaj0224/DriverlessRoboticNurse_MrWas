
#include "derivative.h" /* include peripheral declarations */

unsigned char canal[]={(1<<6)+8,(1<<6)+0,(1<<6)+14};   //PTB0, PTE20, PTC0
unsigned char ADC[3][4];
unsigned char canal_i=0;
unsigned char muestra_j=0;

void ADC0_IRQHandler()
{
	ADC[canal_i++][muestra_j]=ADC0_RA;	// Borra COCO y tenemos resultado en una varible
	if (3<=canal_i)
		{canal_i=0;
		 //muestra_j++;
		 if (4<=++muestra_j) muestra_j=0;
		}
}

void LPTimer_IRQHandler()
{
	LPTMR0_CSR|=(1<<7);		//apaga bandera y mantiene configuracion
	ADC0_SC1A=canal[canal_i];		// 01000 selector de canal en MUX de ADC, asociado a PTB0
}

int main(void)
{

SIM_SCGC5=(1<<10)+ (1<<13)+ (1<<11)+1;		//PORTB, PORTE, PORTC, LPTimer

PORTB_PCR0=(0<<8);		//ADC  -OJO no es necesario al ser la configuracion por default
PORTE_PCR20=(0<<8);		//ADC  -OJO no es necesario al ser la configuracion por default
PORTC_PCR0=(0<<8);		//ADC  -OJO no es necesario al ser la configuracion por default

LPTMR0_CMR=10-1;		// Conversion ADC cada 10 ms
LPTMR0_PSR=5;			//LPO, bypass preescaler
LPTMR0_CSR=(1<<6)+1;	//Hab intr y Enable timer

NVIC_ISER|=(1<<28);		//hab intr LPT desde NVIC

SIM_SCGC6=(1<<27);		//ADC0

NVIC_ISER|=(1<<15);		// Enable Intr ADC0 en NVIC

while (1);
	
return 0;
}
