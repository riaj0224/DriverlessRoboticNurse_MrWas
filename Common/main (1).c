
#include "derivative.h"  //include peripheral declarations 

unsigned long periodo;
unsigned short duty_cycle;
unsigned char string[3] = {0};
unsigned long fsA_count = 0;
unsigned long fsB_count = 0;
unsigned long rpm_promedio = 0;


void UART0_send(unsigned long rpm){
	
		int indx = 0;
		unsigned char string[4] = {0};
		
		//ITOA
		string[3] = (rpm%10) + '0';
		rpm /= 10;
		string[2] = (rpm%10) + '0';
		rpm /= 10;
		string[1] = (rpm%10) + '0';
		string[0] = (rpm/10) + '0';
		
		do{
			while((UART0_S1 & (1<<7)) == 0);         //Wait until the transmission buffer is empty
			UART0_D = string[indx++];
		}while(string[indx] != '\0');                //Waiting for null character
		
		while((UART0_S1 & (1<<7)) == 0);
		UART0_D = '\r';                                //Return
		while((UART0_S1 & (1<<7)) == 0);             
		UART0_D = '\n';                                //New line

}


void FTM1_IRQHandler(){
	
	if (((TPM1_C0SC & (1<<7)) >> 7) == 1){
		TPM1_C0SC|=(1<<7);		//apagar bandera
		fsA_count++;
	}
	
	if (((TPM1_C1SC & (1<<7)) >> 7) == 1){
		TPM1_C1SC|=(1<<7);		//apagar bandera
		fsB_count++;
	}
		
}


void LPTimer_IRQHandler()
{
	
	unsigned long rpm = 0;
	unsigned long rpmA = 0;
	unsigned long rpmB = 0;
	
	LPTMR0_CSR|=(1<<7);		//apaga bandera y mantiene configuracion
	
	rpmA = (60*fsA_count*1000*10)/(11*468*20*2);
	rpmB = (60*fsB_count*1000*10)/(11*468*20*2);
	
	rpm = (rpmA+rpmB)/2;
	
	rpm_promedio = (rpm_promedio+rpm)/2;
	
	fsA_count = 0;
	fsB_count = 0;
}


void LPTMR0_init(void){
	SIM_SCGC5|=1;
	
	LPTMR0_CMR=20-1;		// Conversion ADC cada 10 ms
	LPTMR0_PSR=5;			//LPO, bypass preescaler
	LPTMR0_CSR=(1<<6)+1;	//Hab intr y Enable timer
	
	NVIC_ISER|=(1<<28);		//hab intr LPT desde NVIC
}


void IC_TPM1_PA12_init (void)
{	
	SIM_SCGC5|=(1<<9)+(1<<13);		//PORTA & PORTE
	PORTA_PCR12=(3<<8);		//TPM1_C0
	PORTE_PCR21=(3<<8);     //TPM1_C1
	
	SIM_SCGC6|=(1<<25);		//TPM1
	TPM1_SC=(1<<3);			//reloj interno (4 MHz)
	TPM1_C0SC=(3<<2)+(1<<6); //input capture rising edge, hab intr channel
	TPM1_C1SC=(3<<2)+(1<<6); //input capture rising edge, hab intr channel
	NVIC_ISER|=(1<<18);
} 



void TPM2_CH0_init (void)
{
	SIM_SCGC5|=(1<<13);		//PORTE
	PORTE_PCR22=(3<<8);		//TMP2_CH0
	
	SIM_SCGC6|=(1<<26);		//TPM2
	TPM2_SC=(1<<3)+3;			//preescaler 8:1, reloj interno (4 MHz)
	TPM2_C0SC=(9<<2); 			//EPWM set on match
	
	TPM2_C0V=((periodo*(100-duty_cycle))/100);	//tiempo pin en cero
	TPM2_MOD=periodo;	
}



void UART0_init (void){
	SIM_SCGC5 |= 1<<9;          //enable clk port A
	PORTA_PCR1 = 2<<8;			 //PORT A1: MUX = UART_Rx 
	PORTA_PCR2 = 2<<8;			 //PORT A2: MUX = UART_Tx 
	
	SIM_SCGC4 |= 1<<10;          //enable clk UART0	
		
	//Configure UART module (UART-39) 
	UART0_C4 |= 15;										// OSR = 15 (default) 
	UART0_BDL = 26;										 //SBR = 26: Baud rate divisor 
	UART0_BDH = 0;
															
	 //Baud = CLK/((OSR + 1)*SBR) 
	 //9600 ~ 4 MHz/((15 + 1)*26)  
		
	UART0_C1 = 0; 										// 8 data bits, No parity, 1 bit Stop (default) 
		
	UART0_C2 |= (1<<3);									 //RE = 0: Disable receiver  
				         									 //RIE = 0: Disable receiver interrupt  
															// TE = 1: Enable transmitter 
															 //TIE = 1: Enable transmission interrupt 
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
	int counter = 0;
	
	clk_init();
	IC_TPM1_PA12_init();
	LPTMR0_init();
	periodo=50000;			//periodo de 100 ms, reloj de 4 MHz con preescaler (cuenta 4 us) 
	
	duty_cycle=100;			//en porcentaje
	
	TPM2_CH0_init();
	UART0_init();
	
	
	while(1){
		if ((counter % 2000) == 0){
				UART0_send(rpm_promedio);
			}
			counter++;
	}
	
	return 0;
} 


