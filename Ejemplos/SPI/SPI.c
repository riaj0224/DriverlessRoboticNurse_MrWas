#include "../Library-FRDM-KL25Z/spi.h"

void spi_Init()
{
	SIM_SCGC5 |= (1<<11);           //Turn on clock to C module
	PORTC_PCR4 = (2<<8);            //Set PTC4 to mux 2 [SPI0_PCS0]
	PORTC_PCR5 = (2<<8);    		//Set PTC5 to mux 2 [SPI0_SCK]
	PORTC_PCR6 = (2<<8);    		//Set PTC6 to mux 2 [SPI0_MOSI]
	PORTC_PCR7 = (2<<8);    		//Set PTC7 to mux 2 [SPIO_MISO]
	
	//Enable SPI0 clock
	SIM_SCGC4 |= SIM_SCGC4_SPI0_MASK;

	// Configure SPI Register C2
    SPI0_C2 = (1<<4);   //Master SS pin acts as slave select output

	// Set baud rate prescale divisor to 6 & set baud rate divisor to 4 for baud rate of 1 Mhz
	SPI0_BR = (5<<4)|1;    //  Mhz

	// Enable SPI
    SPI0_C1 |= (1<<6);
}

void spi_Send(uint8_t data)
{
	//While buffer is not empty do nothing
	while(!((1<<5) & SPI0_S))
	{
		__asm("nop");
	}
	SPI0_D= data;
}

uint8_t spi_Read()
{
	// Wait for receive flag to set
	while(!(SPI0_S & (1<<7)))
	{
		__asm("nop");
	}
	return SPI0_D;
}

