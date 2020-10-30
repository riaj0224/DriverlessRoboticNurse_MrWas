/*
 * system_functions.c
 *
 *  Created on: Oct 21, 2020
 *      Author: jair2
 */

#include "derivative.h"
#include "system_functions.h"

#define SLAVE_ADDR 0x68
#define ERR_NONE 0
#define ERR_NO_ACK 0x01
#define ERR_ARB_LOST 0x02
#define ERR_BUS_BUSY 0x03
#define ERR_NO_ACK2 0x04
#define ERR_NO_ACK3 0x05
#define ERR_NO_ACK4 0x06
#define ERR_NO_ACK5 0x07

unsigned char datos[14];
int count;

int WV;
int rv;
unsigned long ACCX ;
unsigned long ACCY ;
unsigned long ACCZ ;
unsigned long TEMP ;
unsigned long GYRX ;
unsigned long GYRY ;
unsigned long GYRZ ;
unsigned long* GYROSCOPE[6];
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

void delay (unsigned short tiempo)
{
	LPTMR0_CMR=tiempo-1;

	LPTMR0_PSR= (1<<2)+1;   	//Bypass al preescaler y LPO como base de tiempo
	LPTMR0_CSR=1;      		//Habilita LPTimer (inicia conteo)

	do{} while (LPTMR0_CSR & (1<<7)==0);   //polling (escrutinio)

//opción 1
	LPTMR0_CSR=LPTMR0_CSR | (1<<7);		//apagar bandera
	LPTMR0_CSR=0;		 //detener el timer
//opción 2
/*
	LPTMR0_CSR=(1<<7);	//w1c bit 7, bit 0 : 0 Desh timer
*/

}

void megadelay(int n)
{
    while (n>0)
    {
    	delay(300000);
    	n--;
    }
    
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


int I2C0_singlebytewrite(unsigned char slaveAddr, unsigned char memAddr, unsigned char data) 
{
	unsigned char read;

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
    
    //megadelay(21);

    /* send slave address and write flag */
    I2C0_D = slaveAddr << 1;	// slaveADDR-0-  0 indica write
    while(!(I2C0_S & 0x02));   // wait for transfer complete
    //while ((I2C0_S & 0x02)==0);	
     
    I2C0_S |= 0x02;            // clear IF
    I2C0_D = memAddr;
    /*
    if (I2C0_S & 0x10) {       // arbitration lost
        I2C0_S |= 0x10;        // clear IF 
        return ERR_ARB_LOST;
    }
    */
    
    //megadelay(10);
    
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;
    
    /* send memory address */
    I2C0_D = memAddr;

    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    I2C0_D = data;
    
    //megadelay(21);
        
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK2;
    
      /* send data */
    I2C0_D = data;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    
    //megadelay(15);
    
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK3;

    /* stop */
    I2C0_C1 &= ~0x30;			//~0x30 = 0xCF 1100 1111  Apaga bit 4 y bit 5
    return ERR_NONE;
    
}



int I2C0_burstRead(unsigned char slaveAddr, unsigned char memAddr, int byteCount, unsigned char* data, int* cnt)
{
unsigned char dummy;

	//megadelay(35);

	/* send start */
	I2C0_C1 |= (1<<4);           // Tx on 
    I2C0_C1 |= (1<<5);           // become master, start
        
    /* send slave address and write flag */
    I2C0_D = slaveAddr << 1;	// slaveADDR-0-  0 indica write
    while(!(I2C0_S & 0x02));   // wait for transfer complete
    I2C0_S |= 0x02;            // clear IF
    I2C0_D = memAddr;  
    
    //megadelay(100);
    
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK;
           
      /* send memory address */
    I2C0_D = memAddr;
    
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    
    //megadelay(21);
   
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK2;
    
    /* restart */
    I2C0_C1 |= 0x04;           /* send Restart */

    /* send slave address and read flag */
    I2C0_D = (slaveAddr << 1) + 1;
    while(!(I2C0_S & 0x02));   /* wait for transfer complete */
    I2C0_S |= 0x02;            /* clear IF */
    
    //megadelay(21);
    
    if (I2C0_S & 0x01)         /* got NACK from slave */
        return ERR_NO_ACK3;

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



void IMU_data_processed (void)
{
	WV=I2C0_singlebytewrite (SLAVE_ADDR, 0x6B,0x0);
	
	WV=I2C0_singlebytewrite (SLAVE_ADDR, 0x6C,0x0);
	
	WV=I2C0_singlebytewrite (SLAVE_ADDR, 0x1B,0x18);

	WV=I2C0_singlebytewrite (SLAVE_ADDR, 0x1C,0x18);
	
		
	rv = I2C0_burstRead(SLAVE_ADDR, 0x3B, 14, datos, &count);
	
	ACCX = (datos[0]<<8) | datos[1];
	ACCY = (datos[2]<<8) | datos[3];
	ACCZ = (datos[4]<<8) | datos[5];

	GYRX = (datos[8]<<8) | datos[9];
	GYRY = (datos[10]<<8) | datos[11];
	GYRZ = (datos[11]<<8) | datos[13];
	
	if (ACCX>>15) (ACCX ^= (ACCX - 1)); 
	if (ACCY>>15) (ACCY ^= (ACCY - 1)); 
	if (ACCZ>>15) (ACCZ ^= (ACCZ - 1)); 

	if (GYRX>>15) (GYRX ^= (GYRX - 1));
	if (GYRY>>15) (GYRY ^= (GYRY - 1));
	if (GYRZ>>15) (GYRZ ^= (GYRZ - 1));
	
	//https://os.mbed.com/users/213468891/code/MPU_6050_Hello_World//file/1221112820f7/main.cpp/
	
	GYROSCOPE[0]=ACCX;
	GYROSCOPE[1]=ACCY;
	GYROSCOPE[2]=ACCZ;
	GYROSCOPE[3]=GYRX;
	GYROSCOPE[4]=GYRY;
	GYROSCOPE[5]=GYRZ;
	
}

unsigned char ESP_8266_send_command (char *p,unsigned char ID,unsigned short timeout_ms)
{
	unsigned char arreglo_respuestas[][15]={"OK\n","mensaje2\n","mensaje3\n"};
	unsigned char host_string[]={"Host: "};
	unsigned char IP[20];
	
	unsigned char caracter=0;
	unsigned char dato_temp;
	unsigned char cadena_encontrada=0;
	unsigned char time_out=0;
	unsigned char cont_ip=0;
	unsigned char cont_host=0;
	unsigned char host_string_encontrado=0;
	unsigned char mac_recibida;
	//******************************************mandar comando
	
	do{
		do{}while ((UART0_S1 & (1<<7))==0);
		UART0_D=*p++;
	}while (*p!=0);
	
	//***************************************** recibir respuesta
	if (ID==3)							//obtener direccion IP CIFSR
	{
		mac_recibida=0;
		LPTMR0_CMR=timeout_ms-1;
		//LPTMR0_CSR=1;
		do{
		if (host_string_encontrado==0)
		{
		//buscar "Host:"
		if (host_string[cont_host]==UART0_D)
			{cont_host++;
			 if (cont_host==7) host_string_encontrado=1;
			}
		else cont_host=0;
		}
		else
		{
			//guardar direccion mac
			if ( UART0_D!=13) IP[cont_ip++]=UART0_D;
			else {
			IP[cont_ip]=0;	
			mac_recibida=1;
			}
		}
		}while ((mac_recibida==0) && ((LPTMR0_CSR & (1<<7))==0));
	}
		
	else
	{
	// leer respuesta a comando con respuesta unica
	LPTMR0_CMR=timeout_ms-1;
	//LPTMR0_CSR=1;
	do{
		do{} while ((UART0_S1 & (1<<5))==0); //&& (LPTMR0_CSR & (1<<7))==0);
		
		if ((UART0_S1 & (1<<5))!=0)
		{
			dato_temp=UART0_D;
		
			if (dato_temp==arreglo_respuestas[ID][caracter]) caracter++;
				else 
				{if (dato_temp==arreglo_respuestas[ID][0]) caracter=1;			// "a5OpKrOOKrOOK\n45
				else caracter=0;
				}
			if ((dato_temp==13) && (arreglo_respuestas[ID][caracter]==0)) cadena_encontrada=1;
		} else time_out=1;
	}while ((time_out==0) && (cadena_encontrada==0));
	}
   return cadena_encontrada;
}

void ESP_8266_init (void)
{
	unsigned char estado;
	
estado=ESP_8266_send_command("AT+RST\r\n",0,10000);						// resetear módulo 							2000
estado=ESP_8266_send_command("AT+CWMODE=1\r\n",0,10000); 					// configurar como cliente 					1000
estado=ESP_8266_send_command("AT+CWJAP=\"SSID\",\"PASSWORD\"\r\n",0,10000); //SSID y contraseña para unirse a red 		8000 
estado=ESP_8266_send_command("AT+CIFSR\r\n",3,10000);    					// obtener dirección IP 1000				1000
estado=ESP_8266_send_command("AT+CIPMUX=1\r\n",0,10000); 					// configurar para multiples conexiones		1000estado=ESP_8266_send_command("AT+CIPSERVER=1,80\r\n",0,10000);        		// servidor en el puerto 80					1000
}
