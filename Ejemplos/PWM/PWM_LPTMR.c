unsigned short periodo;
unsigned short duty_cycle;
unsigned char espejo_pin;

void LPTimer_IRQHandler()
{
	if (espejo_pin==1)
		{
		GPIOB_PCOR=1;									//Clear Output register bit 0 de puerto B, pin=0
		espejo_pin=0;
		LPTMR0_CMR=((periodo*(100-duty_cycle))/100)-1;	//tiempo pin en cero
		}
	else
		{
		GPIOB_PSOR=1;								//Set Output register bit 0 de puerto B, pin=1
		espejo_pin=1;
		LPTMR0_CMR=((periodo*duty_cycle)/100)-1;	//tiempo pin en uno
		}
	
	LPTMR0_CSR|=(1<<7);							//w1c para apagar la bandera
}


void GPIO_init (void)
{
	SIM_SCGC5|=(1<<10);		//PORTB
	PORTB_PCR0=(1<<8);		//GPIO
	GPIOB_PDDR=(1<<0);		//Output
	//valor de default del pin datos es 0
	espejo_pin=0;
}

void LPTMR0_init (void)
{
	SIM_SCGC5|=1;									//LPTMR0
	LPTMR0_PSR=(1<<2)+1;							//bypass preescaler, LPO (1 Khz)
	LPTMR0_CMR=((periodo*(100-duty_cycle))/100)-1;	//tiempo pin en cero
	LPTMR0_CSR=(1<<6)+ 1;							//hab local de intr, enable timer
	
	NVIC_ISER=(1<<28);								//hab intr NVIC
}

int main(void)
{

	periodo=100;			//periodo de 100 ms
	duty_cycle=30;			//en porcentaje
	
	GPIO_init();
	LPTMR0_init();
	
	while (1);
	return 0;
}
