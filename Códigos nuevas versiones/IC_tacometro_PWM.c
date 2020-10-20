//*******************************PWM y tacometro********************************************************

unsigned short periodo;
unsigned short duty_cycle;

unsigned short tviejo=0;
unsigned short frecuencia;
unsigned long rpm;

void FTM0_IRQHandler()
{
unsigned short periodo;

	TPM0_C0SC|=(1<<7);		//apagar bandera
	periodo=TPM0_C0V-tviejo;
	frecuencia=4000000/periodo;
	rpm=60000000/periodo;
	tviejo=TPM0_C0V;
}

void TPM0_CH0_init (void)
{
SIM_SCGC5|=(1<<11);			//PORTC
PORTC_PCR1=(4<<8);			//TPM0_CH0

SIM_SCGC6|=(1<<24);			//TPMO
TPM0_C0SC=(1<<6)+(2<<2);	//Hab Intr, Input capture falling edge
TPM0_SC|=(1<<3);
NVIC_ISER|=(1<<17);			//Intr NVIC TPM0
}

void TPM1_CH0_init (void)
{
	SIM_SCGC5|=(1<<9);		//PORTA
	PORTA_PCR12=(3<<8);		//TMP1_CH0
	
	SIM_SCGC6|=(1<<25);		//TPM1
	TPM1_SC=(1<<3)+2;			//preescaler 4:1, reloj interno (4 MHz) 
	TPM1_C0SC=(9<<2); 			//EPWM set on match
	//NVIC_ISER=(1<<18);		//NVIC intr TPM1
	
	TPM1_C0V=((periodo*(100-duty_cycle))/100);	//tiempo pin en cero
	TPM1_MOD=periodo;	
//	espejo_pin=0;
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
	periodo=1000;			//periodo de 1 ms, reloj de 4 MHz con preescaler de 4(cuenta 1 us) 
	duty_cycle=30;			//en porcentaje
	
	TPM1_CH0_init();
	TPM0_CH0_init();	
	
	while (1);
	return 0;
}