void delay (void)
{unsigned long tiempo;

for (tiempo=0; tiempo<=2000000; tiempo++);
    	
}

void main (void)
{
	//Habilitar reloj de interfaz
	SIM_SCGC5=0x00001400;
	//0b 0000 0000 0000 0000 0001 0100 0000 0000
	
	//Configurar pines GPIO
	PORTB_PCR19=0x00000100;			//0000 0000 0000 0000 0000 0 001 0000 0000
	PORTB_PCR18=0x00000100;
	PORTD_PCR1=0x00000100;
	
	//pines como salida
	GPIOB_PDDR=0x000C0000;                      //0000 0000 0000 1100 0000 0000 0000 0000
	GPIOD_PDDR=0x00000002;    //...... 0010
	
	//apagar leds verde y rojo
	GPIOB_PDOR=0x000C0000;     // 1 pin led verde, 1 pin led rojo
	
	while (1)
	{
	    //prender led azul
		GPIOD_PDOR=0;
		delay();
		//apgar led azul
		GPIOD_PDOR=2;
		delay();
	}
}