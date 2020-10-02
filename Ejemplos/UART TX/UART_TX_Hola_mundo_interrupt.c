unsigned char i=0;
unsigned char mensaje[]={"Hola mundo"};   

void UART0_IRQHandler()
{
	UART0_D=mensaje[i++];
    if (mensaje[i]==0) UART0_C2=12;		//desh intr TDRE  
    	
}

int main(void)
{
	SIM_SCGC5=(1<<9);		//PORTA
	PORTA_PCR1=(2<<8);		//MUX para UART0
	PORTA_PCR2=(2<<8);		//MUX para UART0
	
	SIM_SCGC4=(1<<10);		//UART0
	
	UART0_BDH=0;
	UART0_BDL=26;			//Clk_UART=4 MHz, baud rate = 9600 bps
	
	UART0_C2= 12 + (1<<7);			//TEN=REN=1, hab intr TDRE (THRE)
	NVIC_ISER=(1<<12);
	
	while (1);
	
	return 0;
}
