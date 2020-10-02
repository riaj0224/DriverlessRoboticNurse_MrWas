int main(void)
{
	unsigned char i;
	
	SIM_SCGC5=(1<<9);		//PORTA
	PORTA_PCR1=(2<<8);		//MUX para UART0
	PORTA_PCR2=(2<<8);		//MUX para UART0
	
	SIM_SCGC4=(1<<10);		//UART0
	
	UART0_BDH=0;
	UART0_BDL=26;			//Clk_UART=4 MHz, baud rate = 9600 bps
	
	UART0_C2=12;			//TEN=REN=1
	
	for (i='A'; i<='Z';i++)
	{
	do {} while ((UART0_S1 & (1<<7))==0);   //polling TDRE	   0000000...x000 0000, si x=0 resultado and 0. si x=1 resultado (1<<7)
	UART0_D=i;
	}
	
	while (1);
	
	return 0;
}

