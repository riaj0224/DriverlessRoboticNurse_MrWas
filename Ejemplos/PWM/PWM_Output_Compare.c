/*
unsigned short periodo;
unsigned short duty_cycle;
unsigned short espejo_pin;

void FTM1_IRQHandler()
{
	if (espejo_pin==1)
		{
		espejo_pin=0;
		TPM1_C0V+=((periodo*(100-duty_cycle))/100);	//tiempo pin en cero
		}
	else
		{
		espejo_pin=1;
		TPM1_C0V+=((periodo*duty_cycle)/100);	//tiempo pin en uno
		}
	
	TPM1_C0SC|=(1<<7);							//w1c para apagar la bandera
}

void TPM1_CH0_init (void)
{
	SIM_SCGC5|=(1<<9);		//PORTA
	PORTA_PCR12=(3<<8);		//TMP1_CH0
	
	SIM_SCGC6|=(1<<25);		//TPM1
	TPM1_SC=(1<<3)+3;			//preescaler 8:1, reloj interno (4 MHz)
	TPM1_C0SC=(5<<2)+(1<<6); //Outcompare toggle, hab intr
	NVIC_ISER=(1<<18);		//NVIC intr TPM1
	
	TPM1_C0V=((periodo*(100-duty_cycle))/100);	//tiempo pin en cero
	espejo_pin=0;
}

int main(void)
{
	periodo=50000;			//periodo de 100 ms, reloj de 4 MHz con preescaler (cuenta 4 us) 
	duty_cycle=30;			//en porcentaje
	
	TPM1_CH0_init();
	
	while (1);
	return 0;
}
*/