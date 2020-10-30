#include "derivative.h" /* include peripheral declarations */
#define SLAVE_ADDR 0x68     /* 001 1101. */ //0110 1000
#define ERR_NONE 0
#define ERR_NO_ACK 0x01
#define ERR_ARB_LOST 0x02
#define ERR_BUS_BUSY 0x03
int I2C0_singlebytewrite(unsigned char slaveAddr, unsigned char memAddr,unsigned char byte)
{
	/* send start */
		I2C0_C1 |= (1<<4);           // Tx on 
	    I2C0_C1 |= (1<<5);           // become master, start

	    /* send slave address and write flag */
	    I2C0_D = slaveAddr << 1;	// slaveADDR-0-  0 indica write
	    while(!(I2C0_S & 0x02));   // wait for transfer complete
	    I2C0_S |= 0x02;            // clear IF    
	    
	    if (I2C0_S & 0x01)         /* got NACK from slave */
	    return ERR_NO_ACK;
	    
	/* send memory address */
		I2C0_D = memAddr;
		while(!(I2C0_S & 0x02));   /* wait for transfer complete */
		I2C0_S |= 0x02;            /* clear IF */
		
		if (I2C0_S & 0x01)         /* got NACK from slave */
		return ERR_NO_ACK;
		
		/* send data */
		I2C0_D = byte;
		while(!(I2C0_S & 0x02));   /* wait for transfer complete */
		I2C0_S |= 0x02;            /* clear IF */
		
		if (I2C0_S & 0x01)         /* got NACK from slave */
		return ERR_NO_ACK;
		
		I2C0_C1 &= ~0x20;      /* send stop  */
		
		return ERR_NONE;
}
int I2C0_burstRead(unsigned char slaveAddr, unsigned char memAddr, int byteCount, unsigned char* data, int* cnt)
{
unsigned char dummy;

	/* send start */
	I2C0_C1 |= (1<<4);           // Tx on 
    I2C0_C1 |= (1<<5);           // become master, start

    /* send slave address and write flag */
    I2C0_D = slaveAddr << 1;	// slaveADDR-0-  0 indica write
    while(!(I2C0_S & 0x02));   // wait for transfer complete
    I2C0_S |= 0x02;            // clear IF    

    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* send memory address */
    I2C0_D = memAddr;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;
    
    /* restart */
    I2C0_C1 |= 0x04;           /* send Restart */

    /* send slave address and read flag */
    I2C0_D = (slaveAddr << 1) | 1;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;

    /* change bus direction to read */
    I2C0_C1 &= ~0x18;          /* Tx off, prepare to give ACK */
    if (byteCount == 1)
        I2C0_C1 |= 0x08;       /* prepare to give NACK */
    dummy = I2C0_D;            /* dummy read to initiate bus read */

    /* read data */
    while (byteCount > 0) {
        if (byteCount == 1)
            I2C0_C1 |= 0x08;       /* prepare to give NACK for last byte */
        while(!(I2C0_S & 0x02));   /* wait for transfer complete */
        I2C0_S |= 0x02;            /* clear IF */
        if (byteCount == 1) {
            I2C0_C1 &= ~0x20;      /* stop the bus before reading last byte */
        }
        *data++ = I2C0_D;          /* read received data */
        byteCount--;
        (*cnt)++;
    }

    return ERR_NONE;
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

int main(void)
{	int WV;
	int rv;
	unsigned char datos[2];
    int count;
	
	clk_init();
	SIM_SCGC5=(1<<11);    //PORTE y PORTA
	SIM_SCGC4=(1<<6);
	PORTC_PCR8=(2<<8);				//IIC SCL
	PORTC_PCR9=(2<<8);				//IIC SDA
	
    I2C0_F = (1<<6) + 21;  //0x1C;             /* set clock to 97.09KHz @13.981MHz bus clock */ //OJO registro muy importante
    I2C0_C1 = 0x80;               /* enable I2C0 */
    
    WV=I2C0_singlebytewrite (SLAVE_ADDR, 0x6B,0x0);
    do{
    	
    	rv = I2C0_burstRead(SLAVE_ADDR, 0x3C, 1, datos, &count);
    }
      while (1);
    
	return 0;
}
