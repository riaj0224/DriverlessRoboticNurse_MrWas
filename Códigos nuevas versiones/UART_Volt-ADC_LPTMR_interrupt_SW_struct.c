/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "derivative.h" /* include peripheral declarations */

unsigned char i=0;
unsigned char ADC_string[]={"#.### V\n\r"};   

void UART0_IRQHandler()
{
		UART0_D=ADC_string[i++];				//se borra la bandera al escribir un nuevo dato
	    if (ADC_string[i]==0)
	    {
	    	UART0_C2=12;		//desh intr TDRE
	    }	
}

void ADC0_IRQHandler()
{unsigned char valor_ADC;
	 unsigned short mV;

	valor_ADC=ADC0_RA;						//0<=x<=255    0<=V<=3.3V      "0.000 V" ... "3.300 V"
	mV=3300*valor_ADC/255;                  // 0<=mV<=3300

	//funcion itoa
	ADC_string[4]=(mV%10)+48;   			//'0'0x30 (48), '1' 0x31 .....
	mV/=10;									//mV=mV/10
	ADC_string[3]=(mV%10)+0x30;
	mV/=10;
	ADC_string[2]=(mV%10)+'0';
	ADC_string[0]=(mV/10)+'0';
    i=0;
	UART0_C2= 12 + (1<<7);			//TEN=REN=1, hab intr TDRE (THRE)
}

void LPTimer_IRQHandler()
	{
		LPTMR0_CSR|=(1<<7);		// Apaga bandera
		ADC0_SC1A=(1<<6)+8;		//Inicio de conversion ADC canal 8
	}

void UART0_init (void)
{
		SIM_SCGC5|=(1<<9);		//PORTA	
		PORTA_PCR1=(2<<8);		//MUX para UART0
		PORTA_PCR2=(2<<8);		//MUX para UART0
		
		SIM_SCGC4|=(1<<10);		//UART0

		UART0_BDH=0;
		UART0_BDL=26;			//Clk_UART=4 MHz, baud rate = 9600 bps

		NVIC_ISER|=(1<<12);		//Intr NVIC UART
}
	
void LPTMR_init (void)
{
		SIM_SCGC5|=1;			//LPTMR
		
		LPTMR0_PSR=5;			//LPO (1 KHz), bypass preescaler
		LPTMR0_CMR=50-1;		// Conversion ADC cada 10 ms
		LPTMR0_CSR=(1<<6)+1;	//Hab intr y Enable timer		

		NVIC_ISER|=(1<<28);		//hab intr LPTMR desde NVIC
}

void ADC0_init (void)
{
		SIM_SCGC5|=(1<<10);		//PORTB

		//PORTB_PCR0=(0<<8);		//ADC  -OJO no es necesario al ser la configuracion por default
	
		SIM_SCGC6|=(1<<27);		//ADC0
		NVIC_ISER|=(1<<15);		// Enable Intr ADC0 en NVIC
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
		UART0_init ();
		ADC0_init();
		LPTMR_init ();
		ADC0_init();
		
		while (1);
		
		return 0;
	}
