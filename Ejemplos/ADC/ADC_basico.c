 void ADC0_IRQHandler()
{
	valor_ADC=ADC0_RA;	// Borra COCO y tenemos resultado en una varible
}


int main(void)
{
SIM_SCGC5=(1<<10);		//PORTB

PORTB_PCR0=(0<<8);		//ADC  -OJO no es necesario al ser la configuracion por default

SIM_SCGC6=(1<<27);		//ADC0

ADC0_SC1A=(1<<6)+8;		// 01000 selector de canal en MUX de ADC, asociado a PTB0

NVIC_ISER=(1<<15);		// Enable Intr ADC0 en NVIC

while (1);
	
	
	return 0;
}
