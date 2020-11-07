#include "derivative.h" /* include peripheral declarations */

char buffer[500] = "";
char to_parse[100] = "";
unsigned char found_char = 0;
unsigned char buff_ind = 0;
unsigned char messageComplete = 0;

void clk_init(){
	MCG_C1 |=  0<<6;						// CLKS = 0: MCGOUTCLK Source is FLL (default) 
	MCG_C1 |= (1<<2) + (1<<1);				// IREFS = 1: Slow internal reference clock (default) for FLL & MCGIRCLK active
	MCG_C2 |= 1;					        //Mux IRCLK : FIRC (4 MHz) pag 116
	MCG_SC = 0;					            //Preescaler FIRC 1:1 pag 116
	MCG_C4 |= (1<<5) + (1<<7);				//DRTS_DRS = 1: Mid range & DCO range 48MHz
}

void PORT_init(){
	SIM_SCGC5 |= (1<<9) + (1<<10) + (1<<13);      //clk PORTA & PORTE & PORTB
	
	//UART0 TX & RX
	PORTA_PCR1 = (2<<8);                //PORTA 1 as RX
	PORTA_PCR2 = (2<<8);                //PORTA 2 as TX
	
	//UART1 TX & RX
	PORTE_PCR0 = (3<<8);                //PORTE 0 as TX
	PORTE_PCR1 = (3<<8);                //PORTE 1 as RX
	
	//LED AZUL
	PORTE_PCR4 = (1<<8);                //PORTE 5 as GPIO
	GPIOE_PDDR |= (1<<4);               //E5 as output
	GPIOE_PCOR |= (1<<4);               //Turns off led
	
	//LED ROJO
	PORTB_PCR18 = (1<<8);               //PORTB 18 as GPIO
	GPIOB_PDDR |= (1<<18);              //B18 as output
	GPIOB_PSOR |= (1<<18);              //Turns off led
}

void LPTMR_init(){
	SIM_SCGC5 |= 1;
	
	LPTMR0_PSR |= (1<<2) + 1;                     //bypass prescaler & LPO
	LPTMR0_CSR |= (1<<6);                         //intr enabled
	NVIC_ISER |= (1<<28);                         //intr enabled NVIC
}

void UART0_init(){
	SIM_SOPT2 |= (1<<26);                       //clk source for UART0 - MCGFLLCLK (48MHz)
	SIM_SCGC4 |= (1<<10);                       //clk UART0
	
	//Configure UART0
	UART0_C4 = 15;
	UART0_BDH = 0;
	UART0_BDL = 26;                            //baud rate = 115200
			
	//Baud rate = CLK/(OSR+1)*SBR
	
	UART0_C1 = 0;                               //8 data bits, no parity, 1 bit stop
	UART0_C2 |= (3<<2);                          //TE = 1, transmitter enabled
											    //RE = 1, receiver enabled
											    //TIE = 0, RIE = 0, 	interrupts disabled
	NVIC_ISER |= (1<<12);                       //Intr NVIC UART0
}

void UART1_init(){
	SIM_SCGC4 |= (1<<11);                       //clk UART1
	
	UART1_BDH = 0;                              //baud rate = 115200
	UART1_BDL = 13;                            
	
	//Baud rate = CLK/(OSR+1)*SBR
	
	UART1_C1 = 0;                               //8 data bits, no parity, 1 bit stop
	UART1_C2 |= (3<<2);                         //TE = 1, transmitter enabled
												//RE = 1, receiver enabled
												//TIE = 0, RIE = 0, 	interrupts disabled
	NVIC_ISER |= (1<<13);                       //Intr NVIC UART1
}

//======================LPTMR FUNCTIONS=====================================================

void LPTimer_start(int time){
	LPTMR0_CMR = time - 1;                        //Sets timer count to time in ms
	LPTMR0_CSR |= 1;                               //Starts timer
}

void LPTimer_IRQHandler(){
	GPIOE_PTOR |= (1<<4);                         //Toggles led
	LPTMR0_CSR |= (1<<7);                         //Erasing flag w1c
	LPTMR0_CSR &= ~(1<<0);                        //Stops timer
	LPTimer_start(500);
}

//======================LPTMR FUNCTIONS=====================================================

//======================UART0 FUNCTIONS=====================================================

void UART0_write(char string[]){
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\n';
	unsigned char i = 0;
	do{
		while((UART0_S1 & (1<<7)) == 0);
		UART0_D = string[i++];
	}while(string[i] != '\0');
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\r';
	while((UART0_S1 & (1<<7)) == 0);
	UART0_D = '\n';
}

void UART0_writeChar(unsigned char c){
	while ((UART0_S1 & (1<<7)) == 0);
	UART0_D = c;
}

/*void UART0_IRQHandler(){
	if (UART0_S1 & (1<<5)){
		char temp = UART0_D;
		if (temp != 13){
			dato_read[ir_u0] = temp;
			ir_u0++;
		}else{
			dato_read[ir_u0] = '\0';
			ir_u0 = 0;
			messageComplete = 1;
		}
	}
}*/

//========================UART0 FUNCITONS================================================

//========================UART1 FUNCTIONS================================================

void UART1_write(char string[]){
	UART0_write(string);
	unsigned char i = 0;
	do{
		while((UART1_S1 & (1<<7)) == 0);
		UART1_D = string[i++];
	}while(string[i] != '\0');
	while((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\r';
	while((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\n';
}

void UART1_writeChar(unsigned char c){
	while ((UART1_S1 & (1<<7)) == 0);
	UART1_D = c;
	while ((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\r';
	while ((UART1_S1 & (1<<7)) == 0);
	UART1_D = '\n';
}

void UART1_read(char string[]){
	unsigned char i = 0;
	while(string[i] != '\0'){
		to_parse[i] = string[i];
		i++;
	}
	to_parse[i] = '\0';
	messageComplete = 0;
	buff_ind = 0;
	UART1_C2 |= (1<<5);                      //intr enabled
}

void UART1_IRQHandler(){
	if (UART1_S1 & (1<<5)){
		char temp = UART1_D;                     //Erasing flag by reading
		UART0_writeChar(temp);
		buffer[buff_ind] = temp;                 //Saving read message into buffer
		buff_ind++;
		if (temp == to_parse[found_char]){       //Coincidence found
			found_char++;
			if (to_parse[found_char] == '\0'){   //Whole string found
				buffer[buff_ind] = '\0';
				messageComplete = 1;
				UART1_C2 &= ~(1<<5);             //intr disabled
			}
		}
		else{
			found_char = 0;                      //Looking up again
		}
	}
}
//=========================UART1 FUNCTIONS=================================================

//=========================WIFI FUNCTIONS==================================================

void WiFi_setup(){
	UART1_write("AT+RST");                                    //Resetear el modulo
	UART1_read("ready");                                      //Esperar a que el modulo este listo
	while(!(messageComplete));
	UART1_write("AT");                                        //Confirmar que haya comunicacion
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CWMODE=1");                               //Configurar como cliente
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CWLAP");                                  //Mostar redes disponibles
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CWJAP=\"INFINITUM8234\",\"Rki4KckWRi\""); //Conectarse a red
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CIFSR");                                  //Mostrar direccion IP
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CIPMUX=1");                               //Configurar para multiples conexiones
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write("AT+CIPSERVER=1,80");                         //Configurar puerto
	UART1_read("OK");
	while(!(messageComplete));
}

void WiFi_send(char id,char message[]){
	unsigned char len = 0;
	while(message[len] != '\0') len++;
	char open[] = "AT+CIPSEND=i,le";
	char close[] = "AT+CIPCLOSE=i";
	
	open[11] = id;
	open[13] = (len/10) + '0';
	open[14] = (len%10) + '0';
	close[12] = id;
	
	UART1_write(open);
	UART1_read("OK");
	while(!(messageComplete));
	UART1_write(message);
	UART1_write(close);
	UART1_read("OK");
	while(!(messageComplete));
}

void WiFi_execute_instructions(){
	char instruction[100] = "";
	char pre_instruction[] = "GET /";
	char pre_id[] = "+IPD,";
	unsigned char instruction_obtained = 0;
	unsigned char id;
	unsigned char pre_id_found = 0;
	unsigned char pre_inst_found = 0;
	unsigned char inst_ind = 0;
	unsigned char i = 0;
	
	UART1_read(" HTTP/");
	while(!(messageComplete));
	
	//FINDING INSTRUCTION
	while (!(instruction_obtained)){
		if (pre_id_found){
			if (pre_inst_found){
				instruction[inst_ind] = buffer[i];
				inst_ind++;
				if (buffer[i] == '\0'){
					instruction_obtained = 1;
					instruction[inst_ind-7] = '\0';
				}
			}else{
				if (buffer[i] == pre_instruction[inst_ind]){           //Coincidence found
					inst_ind++;
					if (pre_instruction[inst_ind] == '\0'){            //Found GET /
						pre_inst_found = 1;
						inst_ind = 0;
					}
				}else inst_ind = 0;
			}
		}else{
			if (buffer[i] == pre_id[inst_ind]){                        //Coincidence found
				inst_ind++;
				if (pre_id[inst_ind] == '\0'){                         //Fount +IPD,
					pre_id_found = 1;
					id = buffer[++i];
					inst_ind = 0;
				}
			}else inst_ind = 0;
		}
		i++;
	}
	
	switch (instruction[0]){
		case 'r':
		case 'R':
			GPIOB_PTOR |= (1<<18);                      //Toggle red led
			WiFi_send(id, "<h1> LED ROJO CAMBIO DE ESTADO! <h1/>");
			break;
		case 'b':
		case 'B':
			GPIOE_PTOR |= (1<<4);                       //Toggle blue led
			WiFi_send(id, "<h1> LED AZUL CAMBIO DE ESTADO! <h1/>");
			break;
		default:
			break;
	}
}

//=========================WiFi FUNCTIONS==================================================

int main(void) {
	clk_init();
	PORT_init();
	//LPTMR_init();
	UART0_init();
	UART1_init();
	WiFi_setup();
	
	while(1){
		WiFi_execute_instructions();
	}
	return 0;
}
