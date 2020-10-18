/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */

//SCL PORTA_1
//SDA PORTA_2

#define SCL_0 GPIOA_PCOR=(1<<1); 
#define SCL_1 GPIOA_PSOR=(1<<1);
#define SDA_0 GPIOA_PCOR=(1<<2);
#define SDA_1 GPIOA_PSOR=(1<<2);

void vIIC_init (void)
{
	SIM_SCGC5|=(1<<9);				//Clk interfaz PORTA
	PORTA_PCR1=(1<<8);				//GPIO
	PORTA_PCR2=(1<<8);				//GPIO
	GPIOA_PDDR|=(3<<1);				//Output

//SDA=SCL=0
}

void vIIC_start (void)
{
    SDA_1;	
	SCL_1;
	//esperar t7
	asm ("nop");
	asm ("nop");
	asm ("nop");
	SDA_0;
	//esperar t6
	asm ("nop");
	asm ("nop");
	asm ("nop");
	SCL_0;
}

void vIIC_stop (void)
{
	SCL_1;
	//esperar tiempo
	asm ("nop");
	asm ("nop");
	asm ("nop");
	SDA_1;
	//esperar tiempo
	asm ("nop");
	asm ("nop");
	asm ("nop");
	SCL_0;
	SDA_0;
}

void vIIC_send_byte (unsigned char dato)		//se envia primero el bit mas significativo
{
unsigned char cont=8;
do								// dato : xyzt abcd
								//    and 1000 0000
{
	if ((dato & (1<<7))==0) SDA_0
	else SDA_1;
	SCL_1;
	//esperar t2
	SCL_0;
	dato=dato<<1;				//       yzta bcd0
	//esperar t3
}while (cont--);
SDA_0;

}

unsigned char vIIC_rec_byte (void)
{
unsigned char cont=8;
unsigned char temp;

GPIOA_PDDR&=~(1<<2);		//Configurar el pin 2 como entrada y el resto de los piner pmantienen su configuracion
do
{
	SCL_1;
	temp<<=1;									//      yzta bcd0
	if ((GPIOA_PDIR & (1<<2))) temp|=1;			//temp 	yzta bcd1
	//else temp&=0xFE;							//   or 0000 0001
												//      xyzt abc b7
	//temp<<=1;									//      yzta bc b7 0
	//cuidar tiempo en 1
	SCL_0;										//  b7b6b5b4 b3b2b1b0
												//  b6b5b4b3 b2b1b00
	// cuidar el tiempo en 0
} while (--cont);

GPIOA_PDDR|=(1<<2);			//salida
return temp;
}

unsigned char vIIC_ACK_input (void)
{
unsigned char temp;
	GPIOA_PDDR&=~(1<<2);		//Configurar el pin 2 como entrada y el resto de los piner pmantienen su configuracion
	
	SCL_1;
	if ((GPIOA_PDIR & (1<<2))==0) temp=0;
	else temp=1;
	SCL_0;
	
	GPIOA_PDDR|=(1<<2);			//salida
	return temp;
}

void vIIC_ACK_output (unsigned char ack_type)
{
	if (ack_type) SDA_1
	else SDA_0;
	
	SCL_1;
	//esperar tiempo en 1
	SCL_0;
	SDA_0;
}

unsigned char vIIC_byte_write (unsigned short direccion, unsigned char dato)
{
unsigned char error=0;
	//direccion    xxxA12 A11A10A9A8 A7A6A5A4 A3A2A1A0
	vIIC_start();
	vIIC_send_byte (0xA0);		//1010 A2A1A0 0
	if (vIIC_ACK_input()==0)
	{
		vIIC_send_byte (direccion>>8);			//0000 0000 xxxA12 A11A10A9A8 y uso solo los 8 bits menos sig
		if (vIIC_ACK_input()==0)
		{
			vIIC_send_byte (direccion);			//toma los bits menos sig A7....A0
			if (vIIC_ACK_input()==0)
			{
				vIIC_send_byte (dato);
				if (vIIC_ACK_input()==0)
				{
					vIIC_stop();
				}
				else error=4;
			}
			else error=3;
		}
		else error=2;
	}
	else error=1;
return error;
}

unsigned short vIIC_random_read (unsigned short direccion)
{
unsigned char error=0;
unsigned char temp;
		//direccion    xxxA12 A11A10A9A8 A7A6A5A4 A3A2A1A0
		vIIC_start();
		vIIC_send_byte (0xA0);		//1010 A2A1A0 0
		if (vIIC_ACK_input()==0)
		{
			vIIC_send_byte (direccion>>8);			//0000 0000 xxxA12 A11A10A9A8 y uso solo los 8 bits menos sig
			if (vIIC_ACK_input()==0)
			{
				vIIC_send_byte (direccion);			//toma los bits menos sig A7....A0
				if (vIIC_ACK_input()==0)
				{
					vIIC_start();
					vIIC_send_byte(0xA1);
					if (vIIC_ACK_input()==0)
					{
						temp=vIIC_rec_byte();
						vIIC_ACK_output(1);
						vIIC_stop();
					}
					else error=4;
				}
				else error=3;
			}
			else error=2;
		}
		else error=1;
	return (error<<8)+temp;
}


int main(void)
{
unsigned char dato_leido;
unsigned short temp;

	vIIC_init();
	if (vIIC_byte_write (0x200,0x25)!=0 )
			{
		
			}
	temp=vIIC_random_read(0x200);
    if ((temp & (0xFF00))==0) dato_leido=(unsigned char) temp; 
    else 
    {
    	
    }
	return 0;
}