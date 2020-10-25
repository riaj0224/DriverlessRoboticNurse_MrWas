
#include "derivative.h" /* include peripheral declarations */


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

void UART0_pins(void)
{
	SIM_SCGC5|=(1<<9);		//PORTA
	PORTA_PCR1=(2<<8);		//MUX para UART0
	PORTA_PCR2=(2<<8);		//MUX para UART0
}


void UART0_init (void)
{
    UART0_pins();
    SIM_SCGC4|=(1<<10);		//UART0
	
	UART0_BDH=0;
	UART0_BDL=26;			//Clk_UART=4 MHz, baud rate = 9600 bps
	
	UART0_C2=12;			//TEN=REN=1    

	SIM_SCGC5|=1;
	LPTMR0_PSR=5;
	
	LPTMR0_CMR=50;
	LPTMR0_CSR=1;
	
	do{
		do{} while (((UART0_S1 & (1<<5))==0) && ((LPTMR0_CSR & (1<<7))==0));
		(void) UART0_D;
	}while ((LPTMR0_CSR & (1<<7))==0);
	LPTMR0_CSR=(1<<7);
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
	
UART0_init();
	
estado=ESP_8266_send_command("AT+RST\r\n",0,10000);						// resetear módulo 							2000
estado=ESP_8266_send_command("AT+CWMODE=1\r\n",0,10000); 					// configurar como cliente 					1000
estado=ESP_8266_send_command("AT+CWJAP=\"SSID\",\"PASSWORD\"\r\n",0,10000); //SSID y contraseña para unirse a red 		8000 
estado=ESP_8266_send_command("AT+CIFSR\r\n",3,10000);    					// obtener dirección IP 1000				1000
estado=ESP_8266_send_command("AT+CIPMUX=1\r\n",0,10000); 					// configurar para multiples conexiones		1000
estado=ESP_8266_send_command("AT+CIPSERVER=1,80\r\n",0,10000);        		// servidor en el puerto 80					1000
}

int main(void)
{
	clk_init();
	ESP_8266_init();
	
	for(;;);
	
	return 0;
}
