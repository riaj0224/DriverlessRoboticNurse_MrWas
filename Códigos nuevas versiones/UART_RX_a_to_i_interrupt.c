#include "derivative.h" /* include peripheral declarations */

unsigned char UART_data_in_flag;
unsigned char dato=0;

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

void UART0_IRQHandler()
{unsigned char temp;
	if ((UART0_S1 & (1<<5))==(1<<5))
	{
		temp=UART0_D;
		if (temp!=13) dato=dato*10+(temp-'0');
		else UART_data_in_flag=1;
	}
}

int main(void)
{
	unsigned char temp;
	clk_init();
	SIM_SCGC5=(1<<9);		//PORTA
	PORTA_PCR1=(2<<8);		//MUX para UART0
	PORTA_PCR2=(2<<8);		//MUX para UART0
	
	SIM_SCGC4=(1<<10);		//UART0
	
	UART0_BDH=0;
	UART0_BDL=26;			//Clk_UART=4 MHz, baud rate = 9600 bps

	
	//UART0_C2=12;
	UART0_C2=(1<<5)+12;
	
	UART0_S1|=0x1F;
	do{
		UART0_S1|=0x1F;
	if ((UART0_S1 & (1<<5))!=0) temp=UART0_D;
	}while ((UART0_S1)!=0xC0);
	
	NVIC_ISER=(1<<12);
	
	
	while (1)
	{
	/*
		dato=0;
	do{
		do{} while ((UART0_S1 & (1<<5))==0);	//polling a data ready
		temp=UART0_D;
		if (temp!=13) dato=dato*10+(temp-'0');
	}while (temp!=13);*/
	}
		
	return 0;
}
