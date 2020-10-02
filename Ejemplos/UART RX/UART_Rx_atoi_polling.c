
/* atoi
 * tres numeros (ASCII) maximo    
 * caso 1)  '5' <enter>
 * caso 2) '1''8' <enter>         1*10+8
 * caso 3) '2''5''1' <enter>      (2*10+5)*10+1
 * 
 * 
 * <enter> : ASCII 13
 */

int main(void)
{
unsigned char dato_UART;
unsigned char dato_in;

	SIM_SCGC5= (1<<9);			//clk PortA
	PORTA_PCR1= (2<<8);			//seleccionando UART0
	PORTA_PCR2= (2<<8);			//seleccionando UART0
	
	SIM_SCGC4=(1<<10);			//clk UART0
	
	UART0_BDL=26;				//clk uart 4 MHZ, baud rate a 9600 bps
	UART0_C2= (1<<2);   		//RE
	
	dato_in=0;
	do{
		do{} while ((UART0_S1 & (1<<5))==0);
		dato_UART=UART0_D;
	    if (dato_UART!=13) dato_in=dato_in*10 + (dato_UART-'0');
	}while (dato_UART!=13);

    while (1);
		
	return 0;
}
