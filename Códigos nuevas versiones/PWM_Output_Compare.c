unsigned short periodo;
unsigned char duty_cycle;
unsigned char espejo_pin;

void FTM1_IRQHandler()
{
	TPM1_C0SC|=(1<<7);		//Apagar bandera
	
	if (espejo_pin==0)
	{
		espejo_pin=1;
		TPM1_C0V+=periodo*(duty_cycle)/100;		//tiempo en 1
	}
	else
	{
		espejo_pin=0;
		TPM1_C0V+=periodo*(100-duty_cycle)/100;		//tiempo en 0
	}
}

void TPM1_CH0_init (void)
{
	SIM_SCGC5|=(1<<9);		//PORTA
	PORTA_PCR12=(3<<8);		//MUX: TPM1_CH0
	
	SIM_SCGC6|=(1<<25);		//TPM1
	TPM1_SC=(1<<3);			//reloj interno (4 MHz, 250 ns), preescaler 1:1
	TPM1_C0SC=(5<<2)+(1<<6);   //Output compare toggle, hab intr modulo
	NVIC_ISER|=(1<<18);		//NVIC intr TPM1

	espejo_pin=0;
	TPM1_C0V=periodo*(100-duty_cycle)/100;		//tiempo en 0
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
	periodo=4000;				// periodo= 1 ms
	duty_cycle=30;

	TPM1_CH0_init();
	
	while (1);
	return 0;
}