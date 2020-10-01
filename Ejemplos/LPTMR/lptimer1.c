void delay (unsigned short tiempo)
{
	LPTMR0_CMR=tiempo-1;

	LPTMR0_PSR= (1<<2)+1;   	//Bypass al preescaler y LPO como base de tiempo
	LPTMR0_CSR=1;      		//Habilita LPTimer (inicia conteo)

	do{} while (LPTMR0_CSR & (1<<7)==0);   //polling (escrutinio)

//opción 1
	LPTMR0_CSR=LPTMR0_CSR | (1<<7);		//apagar bandera
	LPTMR0_CSR=0;		 //detener el timer
//opción 2
/*
	LPTMR0_CSR=(1<<7);	//w1c bit 7, bit 0 : 0 Desh timer
*/

}

void main (void)
{

SIM_SCGC5=(1<<12)+1;	//Clk PORTD, LPTRM

PORTD_PCR1=(1<<8);    //GPIO
GPIOD_PDDR=(1<<1);    //Output


while (1)
{
    GPIOD_PDOR=2;	//0010 apagar led
	delay(500);
	GPIOD_PDOR=0;   //0000 prender led
	delay(500);
}
}