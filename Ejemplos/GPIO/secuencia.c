// OOOX
// OOXO
// OXOO
// X000

unsigned long secuencia[]={0x0000000E, 0x0000000D,0x0000000B, 0x00000007};
unsigned char i=0;

void delay (void)
{
unsigned long cont;

for (cont=0;cont<=20000000; cont++);
}

void main (void)
{
      SIM_SCGC5=0x00000400; //       0000 0100 0000 0000
	  
	  PORTB_PCR0=0x00000100;  //	GPIO 0000 0 -001 0000 0000
	  PORTB_PCR1=0x00000100;
	  PORTB_PCR2=0x00000100;
	  PORTB_PCR3=0x00000100;
	  
	  GPIOB_PDDR=15;			// 0x0000000F
	  
	  while (1)
	  {
			GPIOB_PDOR=secuencia[i%4++];
			delay();
			//i++;
			//if (i>3) i=0;
			
			/*
			GPIOB_PDOR=0x0000000E;
			delay();
			GPIOB_PDOR=0x0000000D;
			delay();
			GPIOB_PDOR=0x0000000B;
			delay();
			GPIOB_PDOR=0x00000007;
			delay();
			*/
	  }
}