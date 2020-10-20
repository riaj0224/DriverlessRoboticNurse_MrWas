#include "derivative.h" /* include peripheral declarations */

unsigned short periodo;
unsigned short duty_cycle;

void TPM1_CH0_init (void)
{
	SIM_SCGC5|=(1<<9);		//PORTA
	PORTA_PCR12=(3<<8);		//TMP1_CH0
	
	SIM_SCGC6|=(1<<25);		//TPM1
	TPM1_SC=(1<<3)+3;			//preescaler 8:1, reloj interno (4 MHz)
	TPM1_C0SC=(9<<2); 			//EPWM set on match
	
	TPM1_C0V=((periodo*(100-duty_cycle))/100);	//tiempo pin en cero
	TPM1_MOD=periodo;	
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
	periodo=50000;			//periodo de 100 ms, reloj de 4 MHz con preescaler (cuenta 4 us) 
	duty_cycle=30;			//en porcentaje
	
	TPM1_CH0_init();
	
	while (1);
	return 0;
}