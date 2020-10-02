unsigned char i=0;
unsigned char ADC_string[]={"#.### V"};   

void UART0_IRQHandler()
{
	UART0_D=ADC_string[i++];				//se borra la bandera al escribir un nuevo dato
    if (ADC_string[i]==0) UART0_C2=12;		//desh intr TDRE  
    	
}

void ADCO_IRQHandler (void)
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

UART0_C2= 12 + (1<<7);			//TEN=REN=1, hab intr TDRE (THRE)

}

void LPTIMER0_IRQHandler (void)
{
	LPTMR0_CSR|=(1<<7);
	ADC0_SC1A=(1<<6)+8;
}

int main(void)
{
	SIM_SCGC5=(1<<10)+(1<<9)+1;		//PORTA, PORTB, LPTimer
	PORTA_PCR1=(2<<8);		//MUX para UART0
	PORTA_PCR2=(2<<8);		//MUX para UART0

	SIM_SCGC4=(1<<10);		//UART0
	
	UART0_BDH=0;
	UART0_BDL=26;			//Clk_UART=4 MHz, baud rate = 9600 bps
	
	NVIC_ISER|=(1<<12);
	
	LPTMR0_CMR=10-1;		// Conversion ADC cada 10 ms
	LPTMR0_PSR=5;			//LPO, bypass preescaler
	LPTMR0_CSR=(1<<6)+1;	//Hab intr y Enable timer

	NVIC_ISER|=(1<<28);		//hab intr LPT desde NVIC

	SIM_SCGC6=(1<<27);		//ADC0
	PORTB_PCR0=(0<<8);		//ADC  -OJO no es necesario al ser la configuracion por default

	NVIC_ISER|=(1<<15);		// Enable Intr ADC0 en NVIC
	
	while (1);
	
	return 0;
}



