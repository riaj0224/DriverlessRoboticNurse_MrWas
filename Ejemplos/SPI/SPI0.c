void port_SPI0_init (void)
{
    SIM_SCGC5 |= (1 << 11);                  //Turn on clock to C module

    PORTC_PCR4 = (2 << 8);                  //Set PTC4 to mux 2 [SPI0_PCS0]
    PORTC_PCR5 = (2 << 8);                  //Set PTC5 to mux 2 [SPI0_SCK]
    PORTC_PCR6 = (2 << 8);                  //Set PTC6 to mux 2 [SPI0_MOSI]
    PORTC_PCR7 = (2 << 8);                  //Set PTC7 to mux 2 [SPIO_MISO]
}




void SPI0_init(void)
{
    /* habilita clock*/
    SIM_SCGC4 |= (1 << 22); // Habilita Clock para SPI0

    SPI0_C1 = (1<<4) | (1<<1); // Set SPI0 to Master & SS pin to auto SS & spi mode in 0,0

    SPI0_C2 = (1<<4)); //Master SS pin acts as slave select output

    SPI0_BR = (5<<4) | (1<<0); // Set baud rate prescale divisor to 6 & set baud rate divisor to 4 for baud rate of 1 Mhz

    SPI0_C1 |= (1 << 6); // Enable SPI0
}



void SPI0_Send( uint8_t data)
{
    //While buffer is not empty do nothing
    while (!((1<<5) & SPI0_S))
    {
        __asm("nop");
    }
    SPI0_D = data;
}



uint8_t SPI0_Read(void)
{
    // Wait for receive flag to set
    while (!(SPI0_S & (1<<7)))
    {
        __asm("nop");
    }
    return SPI0_D;
}