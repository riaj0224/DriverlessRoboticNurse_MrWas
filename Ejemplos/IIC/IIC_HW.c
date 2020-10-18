#define SLAVE_ADDR 0x1D     /* 001 1101. */
#define ERR_NONE 0
#define ERR_NO_ACK 0x01
#define ERR_ARB_LOST 0x02
#define ERR_BUS_BUSY 0x03


int I2C0_byteWrite(unsigned char slaveAddr, unsigned char memAddr, unsigned char data) {

/*Solo para asegurar bus en estado idle	
	int retry = 1000;
    
    while (I2C0_S & (1<<5)) 		{    // wait until bus is available
        if (--retry <= 0)
            return ERR_BUS_BUSY;
        delayUs(100);
    }
*/
	
    /* send start */
    I2C0_C1 |= (1<<4);           // Tx on 
    I2C0_C1 |= (1<<5);           // become master, start

    /* send slave address and write flag */
    I2C0_D = slaveAddr << 1;	// slaveADDR-0-  0 indica write
    while(!(I2C0_S & 0x02));   // wait for transfer complete
    //while ((I2C0_S & 0x02)==0);	
    
    
    I2C0_S |= 0x02;            // clear IF 
    /*
    if (I2C0_S & 0x10) {       // arbitration lost
        I2C0_S |= 0x10;        // clear IF 
        return ERR_ARB_LOST;
    }
    */
    
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* send memory address */
    I2C0_D = memAddr;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* send data */
    I2C0_D = data;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* stop */
    I2C0_C1 &= ~0x30;			//~0x30 = 0xCF 1100 1111  Apaga bit 4 y bit 5

    return ERR_NONE;
}

/* delay n microseconds
 * The CPU core clock is set to MCGFLLCLK at 41.94 MHz in SystemInit().
 */
void delayUs(int n)
{
    int i; int j;
    for(i = 0 ; i < n; i++) {
        for(j = 0; j < 7; j++) ;
    }
}


int main(void)
{
	unsigned char IIC_error_value;
	
	SIM_SCGC5=(1<<13)+(1<<9);    //PORTE y PORTA
	PORTA_PCR14=(1<<8);			//faltar[ia definir el tipo de interrupci[on
	PORTA_PCR15=(1<<8);			//faltar[ia definir el tipo de interrupci[on
	
	SIM_SCGC4=(1<<6);
	PORTE_PCR24=(5<<8);				//IIC
	PORTE_PCR25=(5<<8);				//IIC
	
    //I2C0_F = 0x1C;             /* set clock to 97.09KHz @13.981MHz bus clock */ //OJO registro muy importante
    I2C0_C1 = 0x80;               /* enable I2C0 */

    IIC_error_value = I2C0_byteWrite(SLAVE_ADDR, 0, 0x25);
    
    while (1);
    
	return 0;
}
