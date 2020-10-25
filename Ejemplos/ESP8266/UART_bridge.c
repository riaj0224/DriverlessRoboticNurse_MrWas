
#include "derivative.h" /* include peripheral declarations */

void vUART_init(void)
{
//UART init	
	SIM_SCGC4=(3<<10); //Hab clk UART0 y UART1
	UART0_BDH=0;
	UART0_BDL=26;
	UART1_BDH=0;
	UART1_BDL=26;
	//UART0_C1=0;
	UART0_C2=12; // bit 3: Hab Tx, bit 2: Hab Rx
	UART1_C2=12;
	
//Pin terminal init
	SIM_SCGC5=(1<<13)+(1<<9);		//PORTC y PORTA
	PORTA_PCR1=(2<<8);		//MUX para UART0
	PORTA_PCR2=(2<<8);		//MUX para UART0
	
	PORTE_PCR0=(3<<8);		//MUX para UART1
	PORTE_PCR1=(3<<8);		//MUX para UART1
	}

void clk_init (void)
{
// FIRC = 4 MHz. BusClk = 4 MHz
// UART0: FIRC. UART1: BusClk. UART2: BusClk. TPM: FIRC. IIC: BusClk

MCG_C1|=(1<<6) + (1<<1);	//MCGOUTCLK : IRCLK. CG: Clock gate, MCGIRCLK enable pag 116
MCG_C2|=1;					//Mux IRCLK : FIRC (4 MHz) pag 116
MCG_SC=0;					//Preescaler FIRC 1:1 pag 116

SIM_CLKDIV1=0;				//OUTDIV4=OUTDIV1= 1:1 pag 116. Busclk 4 MHz
SIM_SOPT2|=15<<24;			//Seleccion MCGIRCLK tanto para UART0 como para TPM

}

int main(void)
{
	clk_init();
	vUART_init();
	
	while (1)
	{
		if (UART0_S1&0x20)
		{
			do{}while (!(UART1_S1&0x80));
			UART1_D=UART0_D;
		}
		if (UART1_S1&0x20)
		{
			do{}while (!(UART0_S1&0x80));
			UART0_D=UART1_D;
		}
		
			}//while (1)
	return 0;
} //main
