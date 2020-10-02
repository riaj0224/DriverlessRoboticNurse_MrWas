#include "derivative.h" /* include peripheral declarations */

unsigned char dato_in=0;
unsigned char dato_listo=0;

void UART0_IRQHandler()
{
	unsigned char dato_UART;
	
	dato_UART=UART0_D;
	if (dato_UART!=13) dato_in=dato_in*10 + (dato_UART-'0');
	else 
		{UART0_C2=(1<<2);
		dato_listo=1;
		}
}

int main(void)
{
	SIM_SCGC5= (1<<9);			//clk PortA
	PORTA_PCR1= (2<<8);			//seleccionando UART0
	PORTA_PCR2= (2<<8);			//seleccionando UART0
	
	SIM_SCGC4=(1<<10);			//clk UART0
	
	UART0_BDL=26;				//clk uart 4 MHZ, baud rate a 9600 bps
	UART0_C2= (1<<2) + (1<<5);   //+ (1<<7);			//RE, Hab Intr RIE
	
	NVIC_ISER=(1<<12);			//NVIC UART0
	
	while (1);
	
	return 0;
}