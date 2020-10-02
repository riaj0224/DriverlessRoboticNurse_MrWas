
void sensor_us_init (void)
{
	SIM_SCGC5|=(1<<9);		//PORTA
	PORTA_PCR12=(3<<8);		//TPM1_C0
	PORTA_PCR13=(1<<8);		//GPIO
	GPIOA_PDDR|=(1<<13);	//PA_13 output
	
	SIM_SCGC6|=(1<<25);		//TPM1
	TPM1_SC=(1<<3);			// Reloj interno (4 MHz)
	TPM1_C0SC=(1<<2); //input capture rising edge,
	TPM1_C1SC=(4<<2); //software
}

void pulso_salida_sensor_ultrasonico (void)
{
	GPIOA_PDOR=(1<<13);			//pin en 1
	TPM1_C1V=TPM1_CNT+400;		// 400 = 40 pulsos de 25 ns para generar un 1 us
	do{}while ((TPM1_C1SC & (1<<7))==0);
	TPM1_C1SC|=(1<<7);
	GPIOA_PDOR=0;
}

int main(void)
{
	unsigned short t1;
	unsigned short t2;
	unsigned short tiempo_us;
	unsigned short distancia;
	
	sensor_us_init();
	pulso_salida_sensor_ultrasonico();
	do{}while ((TPM1_C0SC & (1<<7))==0);  //esperando flanco de subida
	TPM1_C0SC|=(1<<7);
	t1=TPM1_C0V;
	TPM1_C0SC=(2<<2);		//flanco de bajada
	do{}while ((TPM1_C0SC & (1<<7))==0);  //esperando flanco de subida
	TPM1_C0SC|=(1<<7);
	t2=TPM1_C0V;
	tiempo_us= ((t2-t1)*25)/1000;
	distancia=tiempo_us/58;
	
	while(1);
	
	return 0;
}
