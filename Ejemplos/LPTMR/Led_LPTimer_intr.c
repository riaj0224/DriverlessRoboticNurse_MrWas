void ISR_LPTMR0 (void)
{
LPTMR0_CSR=LPTMR0_CSR|(1<<7);	//LPTMR0_CSR|=(1<<7);
GPIOD_PDOR=GPIOD_PDOR^2;		//GPIOD_PDOR^=2;
								//  			000000  00x0
								//xor           000000  0010
								//              000000  00x'0
}


void main (void)
{
SIM_SCGC5=(1<<12)+ (1<<0);		//CLK interfaz PORTD, LPTMR0

PORTD_PCR1=(1<<8);				//GPIO
GPIOD_PDDR=(1<<1);				//Output

LPTMR0_CMR=500-1;				//500 ms
LPTMR0_PSR=5;					//Bypass preescaler (1<<2), LPO b01
LPTMR0_CSR=(1<<6)+1;			//Hab Intr local, Hab timer

NVIC_ISER=(1<<28);				//Hab NVIC LPTMR0

while (1);
	
}