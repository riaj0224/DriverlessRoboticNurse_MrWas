void PIT_IRQHandler()
{

   if (PIT_TFLG0 == (1<<0))
   {
          PIT_TFLG0=(1<<0);
		  GPIOB_PDOR^=(1<<19);    //Toggle led verde
   }
   
   if (PIT_TFLG1 == (1<<0))
   {
          PIT_TFLG1=(1<<0);
		  //lo que se deba de hacer
   }
   

	
}

void clk_init (void)
{
// FIRC = 4 MHz. BusClk = 4 MHz
// UART0: FIRC. UART1: BusClk. UART2: BusClk. TPM: FIRC. IIC: BusClk
MCG_C1|=(1<<6) + (1<<1);	//MCGOUTCLK : IRCLK. CG: Clock gate, MCGIRCLK enable pag 116
MCG_C2|=1;					//Mux IRCLK : FIRC (4 MHz) pag 116
MCG_SC=0;					//Preescaler FIRC 1:1 pag 116

SIM_CLKDIV1=0;				//OUTDIV4=OUTDIV1= 1:1 pag 116. Busclk 4 MHz
SIM_SOPT2|=15<<24;			//Seleccion MCGIRCLK tanto para UART0 como para TPM

}


void main (void)
{
	clk_init();
	SIM_SCGC5=(1<<10);			//Clk PORTB
	PORTB_PCR19=(1<<8);			//GPIO led verde
	GPIOB_PDDR=(1<<19);			// Output
	
	SIM_SCGC6=(1<<23);			//PIT
	PIT_MCR&=~(1<<1);     		//Enable PITs
	
	PIT_LDVAL0=4000000;			//500 ms. Cuenta 4 MHz (250 ns). Tiner 32 bits, 2^n (unidades) =4, decenas: 10 (mil), 20 (millones), 30 (mil millones)
	PIT_TCTRL0= (1<<1)+(1<<0); // TEN=1, Hab Intr
	
	NVIC_ISER=(1<<22);
	
	while (1);
	
//     Reg      abcd efgh .... xyzt pqrs
// And          1111 1111 .... 0111 1111  negado  0000 0000 ..... 1000 0000   (1<<7)
/// Resultado   abcd efgh .....0yzt pqrs
	
//     Reg      abcd efgh .... xyzt pqrs   no adecuado para apagar un bit
// XOR          0000 0000 .....x000 0000
/// Resultado   abcd efgh .....0yzt pqrs

//    ab  a xor b
//    00     0
//    01     1
//    10     1
//    11     0
	
}