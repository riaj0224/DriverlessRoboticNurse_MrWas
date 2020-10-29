/*
 * system_functions.c
 *
 *  Created on: Oct 21, 2020
 *      Author: jair2
 */

#include "derivative.h"
#include "system_functions.h"

#define SLAVE_ADDR 0b1101000
#define ERR_NONE 0
#define ERR_NO_ACK 0x01
#define ERR_ARB_LOST 0x02
#define ERR_BUS_BUSY 0x03

unsigned char datos[14];
int count;


	
void pulse_generator (void)
{
	//
	/*
	PIT_TCTRL1=(1<<0);				//Encender timer
	GPIOE_PSOR=(1<<2);				//prender pin US
	do{}while (PIT_TFLG1==0);		//espera 10 us
	GPIOE_PCOR=(1<<2);				//apaga pin US
	PIT_TCTRL1&=~(1<<0);			//apaga timer
	*/
	
	TPM1_CNT = 1;
	GPIOE_PDOR=(1<<2);
	TPM1_C1V=TPM1_CNT+40;		// 400 = 40 pulsos de 25 ns para generar un 1 us
	do{}while ((TPM1_C1SC & (1<<7))==0);
	TPM1_C1SC|=(1<<7);
	GPIOE_PDOR=0;
}



void UART0_send4(unsigned long rpm){
	
		int indx = 0;
		unsigned char string[4] = {0};
		
		//ITOA
		string[3] = (rpm%10) + '0';
		rpm /= 10;
		string[2] = (rpm%10) + '0';
		rpm /= 10;
		string[1] = (rpm%10) + '0';
		string[0] = (rpm/10) + '0';
		
		do{
			while((UART0_S1 & (1<<7)) == 0);         //Wait until the transmission buffer is empty
			UART0_D = string[indx++];
		}while(string[indx] != '\0');                //Waiting for null character
		
		while((UART0_S1 & (1<<7)) == 0);
		UART0_D = '\r';                                //Return
		while((UART0_S1 & (1<<7)) == 0);             
		UART0_D = '\n';                                //New line

}




int I2C0_burstRead(void)
{
	unsigned char dummy;
	unsigned char slaveAddr = SLAVE_ADDR;
	unsigned char memAddr = 59;
	int byteCount = 14;
	unsigned char* data = datos;
	int* cnt = count;
	
	

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




