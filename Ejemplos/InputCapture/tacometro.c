unsigned short t_viejo=0;

void FTM1_IRQHandler()
{
unsigned short periodo;
unsigned long frecuencia;
unsigned long rpm;
	
TPM1_C0SC|=(1<<7);		//apaga bandera w1c

periodo=TPM1_C0V-t_viejo;	//delta T
frecuencia=4000000/periodo;
rpm=60*frecuencia;
	
}

void IC_TPM1_PA12_init (void)
{
	SIM_SCGC5|=(1<<9);		//PORTA
	PORTA_PCR12=(3<<8);		//TPM1_C0
	
	SIM_SCGC6|=(1<<25);		//TPM1
	TPM1_SC=(1<<3);			// Reloj interno (4 MHz)
	TPM1_C0SC=(2<<2)+(1<<6); //input capture fallingedge, hab intr
	NVIC_ISER|=(1<<18);
}

int main(void)
{
	IC_TPM1_PA12_init();
	
	while(1);
	
	return 0;
}
