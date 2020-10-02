
int main(void)
{
unsigned char dato_UART;

	SIM_SCGC5= (1<<9);			//clk PortA
	PORTA_PCR1= (2<<8);			//seleccionando UART0
	PORTA_PCR2= (2<<8);			//seleccionando UART0
	
	SIM_SCGC4=(1<<10);			//clk UART0
	
	UART0_BDL=26;				//clk uart 4 MHZ, baud rate a 9600 bps
	UART0_C2= (1<<2);   		//RE
	
		while (1);
	{
		do{} while ((UART0_S1 & (1<<5))==0);
		dato_UART=UART0_D;
	
		if (dato_UART=='b')
		{
			//apaga led rojo
			//apaga led verde
			//prende led azul
		}
		
		if (dato_UART=='r')
		{
			//prende led rojo
			//apaga led verde
			//apaga led azul
		}

		if (dato_UART=='g')
		{
			//apaga led rojo
			//prende led verde
			//apaga led azul
		}
	}
		
	return 0;
}
