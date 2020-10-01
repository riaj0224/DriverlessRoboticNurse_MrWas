unsigned long secuencia[]={0x0000000E, 0x0000000D,0x0000000B, 0x00000007};
unsigned char i=0;							// 0<=x<=255 (2^8-1) 8: numero de bits
unsigned char sentido=0;					// signed char -128<=x<=127  -2^(8-1)<=x<=2^(8-1)-1
											
void ISR_LPTMR0 (void)
{
LPTMR0_CSR=LPTMR0_CSR|(1<<7);	//LPTMR0_CSR|=(1<<7);
if (sentido==0) GPIOB_PDOR=secuencia[i%4++];
else GPIOB_PDOR=secuencia[i%4--];
}

void ISR_PORTA (void)
{
   PORTA_PCR1|=(1<<24);					//w1c bandera de interrupcion
   sentido^=1;
}

void main (void)
{
SIM_SCGC5=(1<<10)+(1<<9)+(1<<0);		//CLK interfaz PORTB, PORTA, LPTMR0

PORTB_PCR0=(1<<8);  					//	GPIO 0000 0 -001 0000 0000
PORTB_PCR1=(1<<8);
PORTB_PCR2=(1<<8);
PORTB_PCR3=(1<<8);

PORTA_PCR1=(1<<8)+(10<<16);				//GPIO selector de direccion, interrupt on falling edge (hab intr local)
	  
GPIOB_PDDR=15;							// 0x0000000F

LPTMR0_CMR=500-1;						//500 ms
LPTMR0_PSR=5;							//Bypass preescaler (1<<2), LPO b01
LPTMR0_CSR=(1<<6)+1;					//Hab Intr local, Hab timer

NVIC_ISER=(1<<28) + (1<<30);			//Hab NVIC LPTMR0 y PORTA IRQ

while (1);
}