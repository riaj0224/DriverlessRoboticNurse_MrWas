// OOOX
// OOXO
// OXOO
// XOOO

unsigned long secuencia[]={0x00003800,0x00003400,0x00002C00,0x00001C00};
unsigned char i=0;

void delay (void)
{
   unsigned long cont;
   
   for (cont=0; cont<=2000000;cont++);
}

void main (void)
{
	SIM_SCGC5=0x00000800;   	//0x0000    0000 1000 0000 0000
	
	PORTC_PCR10=0x00000100;   	//0x0000    0000 0-001- 0000 0000
	PORTC_PCR11=0x00000100;
	PORTC_PCR12=0x00000100;
	PORTC_PCR13=0x00000100;
	
	PORTC_PCR9=0x00000100;		//swith para seleccionar dirección de la secuencia
	
	GPIOC_PDDR=0x00003C00;   	//0011 1100 0000 0000
	
	while (1)
	{
		if ((GPIOC_PDIR & 0x00000200)== 0x00000200) GPIOC_PDOR=secuencia[i%4++];
		else GPIOC_PDOR=secuencia[i%4--];
		delay();
		
	//	  xyzt abcd pqrs
	//and 0010 0000 0000	
	//    00b0 0000 0000

	// Líneas eliminadas al integrarlas en la el cálculo del índice del arreglo
		//i++;
		//if (i>3) i=0;

	/* Forma de implementar sin usar el arreglo
	GPIOC_PDOR=0x00003800;      //   0011 1000
	delay();
	GPIOC_PDOR=0x00003400;		//   0011 0100
	delay();
	GPIOC_PDOR=0x00002C00;		//   0010 1100
	delay();
	GPIOC_PDOR=0x00001C00;		//	 0001 1100
	delay();
	*/
	}
}