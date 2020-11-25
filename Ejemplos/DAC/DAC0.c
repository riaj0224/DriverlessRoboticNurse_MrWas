void DAC0_Init(void)
{
    // DAC0 clock enabled
    SIM_SCGC6 |= (1<<31);

    // Clear status register
    DAC0_SR = 0;

    DAC0_C1 = 0; /* disable the use of buffer */

    // The DAC system is enabled
    DAC0_C0 = (1<<7) | (1<<5);
    
}

bool DAC0_Output(uint16_t value)
{
    uint8_t msb = 0;
    uint8_t lsb = 0;

    if (value < 4095)
    {
        msb = (uint8_t)(value >> 8);
        lsb = (uint8_t)(value);

        DAC0_DAT0L = lsb;
        DAC0_DAT0H = msb;

        return true;
    }
    else
        return false;
}