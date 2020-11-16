/*
 * main implementation: use this 'C' sample to create your own application
 *
 */



#include "derivative.h" /* include peripheral declarations */



// Declaration of global variables
//**************************************************************************************

unsigned short periodo = 3200;								//Since the system core is of 4MHz: 3200 pulses equals 800us
unsigned short duty_cycle = 40;								//The duty cycle of motor
unsigned char obstruction = 0;
unsigned char max = 0;
unsigned char min = 0;
unsigned short countM = 0;
unsigned long rpmM = 0;
char string1[100] = "";
char string2[100] = "";
char data_requested [100] = "";
int raw_value;
char buffer[500] = "";
char to_parse[100] = "";
unsigned char found_char = 0;
unsigned char buff_ind = 0;
unsigned char messageComplete = 0;

//**************************************************************************************



void PORT_init(void)
{
	// Enable PORTs from the clocks (SIM-12)
	SIM_SCGC5 |= (1<<9)									//Enable PORTA
			  |  (1<<10)								//Enable PORTB
			  |  (1<<11)								//Enable PORTC
			  |  (1<<12)								//Enable PORTD
			  |  (1<<13);								//Enable PORTE
	
	// TPM1
	PORTE_PCR20 = (3<<8);								//Select 3rd alternative: TPM1_CH0	
	
	// Configure ports as TPM (PORT-11)
	// TPM0
	PORTC_PCR1 = (4<<8);								//Select 4th alternative: TPM0_CH0
	
	// Configure ports as GPIO (PORT-11)
	// GPIO
	PORTE_PCR2  = (1<<8);								//Select 1st alternative: GPIO OUTPUT
	PORTE_PCR3  = (1<<8);								//Select 1st alternative: GPIO OUTPUT
	
	PORTA_PCR4  = (1<<8)+(9<<16);;						//Select 1st alternative: GPIO INPUT + INTR rising edge
	PORTA_PCR5  = (1<<8)+(9<<16);						//Select 1st alternative: GPIO INPUT + INTR rising edge
	PORTA_PCR13 = (1<<8)+(9<<16);						//Select 1st alternative: GPIO INPUT + INTR rising edge
	PORTA_PCR16 = (1<<8)+(9<<16);						//Select 1st alternative: GPIO INPUT + INTR rising edge
	PORTA_PCR17 = (1<<8)+(9<<16);						//Select 1st alternative: GPIO INPUT + INTR rising edge
	// Output
	GPIOE_PDDR |= (1<<2);								//Set as output
	GPIOE_PDDR |= (1<<3);								//Set as output
	// Turn off output
	GPIOE_PCOR = (1<<2);								//Turn off output
	GPIOE_PCOR = (1<<3);								//Turn off output
}



void CLK_init (void)
{
	// Choose internal clock of 4MHz: (MCG-24)
	MCG_C1|=(1<<6) + (1<<1);								//Enable both MCGOUTCLK and MCGIRCLK
	MCG_C2|=1;												//Choose fast clock of 4MHz
	MCG_SC=0;												//Preescaler of 1:1
	
	// Clock configurations for the SIM: (SIM-12)
	SIM_CLKDIV1=0;											//Set both system dividers OUTDIV4 and OUTDIV1 to 1:1
	SIM_SOPT2|=15<<24;										//Select the clock MCGIRCLK for the UART0 and TPM

}



void TPM0_init (void)
{
	// Enable TPM from the clocks (SIM-12)
	SIM_SCGC6|=(1<<24);									//Enable TPM0
	
	// Configure TPM module (TPM-31)
	TPM0_SC|=(1<<3);									//Set preescaler to 1:1 and select LPTMR as time SRC (4MHz)
	
	// Configure mode (output capture) (TPM-31)	
	TPM0_C0SC=(1<<6)+(2<<2);							//Select mode by input capture and falling edge, enable interruption
	
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER|=(1<<17);									//Enable from both NVIC and software
}



void TPM1_init (void)
{
	// Enable TPM from the clocks (SIM-12)
	SIM_SCGC6|=(1<<25);									//Enable TPM1
	
	// Configure TPM module (TPM-31)
	TPM1_SC=(1<<3);										//Set preescaler to 1:1 and select LPTMR as time SRC (4MHz)
	
	// Configure mode (output capture) (TPM-31)	
	TPM1_C0SC=(9<<2);									//Select mode by edge aligned PWM
	TPM1_C1SC=(4<<2); 									//software
	
	// Set values for the PWM operation mode
	TPM1_C0V=((periodo*(100-duty_cycle))/100);			//Time in zero 0
	
	// Set MAX value for the counter to restart process
	TPM1_MOD=periodo;									//Period of cycle
			
}



void UART1_init(void)
{
	// Enable UART1 from the clocks: (SIM-12)
	SIM_SCGC4 |= (1<<11);                      			//Enable UART1
	
	// Configure Baud rate to 9600: (UART-39)			Equation: Baud = CLK/((OSR + 1)*SBR) =>9600 = 4MHz/((15+1)*SBR)
	//UART1_C4 = 15;										//Configure OSR as 15
	UART1_BDH=0;										//Configure SBR as 26*
	UART1_BDL=26;										//Configure SBR as 26*
	
	//Baud rate = CLK/(OSR+1)*SBR
	UART1_C1 = 0;                               		//8 data bits, no parity, 1 bit stop
	UART1_C2 |= (3<<2);                         		//TE = 1, transmitter enabled
														//RE = 1, receiver enabled
														//TIE = 0, RIE = 0, 	interrupts disabled
	
	UART1_C2 |= (1<<5);                      			//Enable interruption
	
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER |= (1<<13);                       		//Intr NVIC UART1
}



void PIT_init (void)
{
	// Enable PIT from the clocks (SIM-12)
	SIM_SCGC6|=(1<<23);									//Enable PIT
	
	// Enable PIT timer 
	PIT_MCR&=~(1<<1);									//By default it is in 1, so it must be turned off
	
	// Configure number of pulses for interruption
	PIT_LDVAL0= 4000;									//As the core clock is of 4MHz, 4k pulses equals 1 ms
	
	// Configure module
	PIT_TCTRL0= (1<<1)+(1<<0); 							//Enable interruption and enable timer '0'
	
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER=(1<<22);									//Enable interruption from NVIC
}



void UART1_write(char string[])
{
	unsigned char i = 0;								//Initialise counter
	
	do{
		
		while((UART1_S1 & (1<<7)) == 0);				//Wait till data buffer is empty
		UART1_D = string[i++];							//Send string character by character
		
	}while(string[i] != '\0');							//Wait till the string shows the NULL symbol
	
	while((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = '\r';										//RETURN LINE
	
	while((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = '\n';										//NEW LINE
}



void UART1_read(char string[])
{
	unsigned char i = 0;								//Initialise counter
	
	while(string[i] != '\0')							//Work until the NULL (end) symbol
	{							
		to_parse[i] = string[i];						//Store data in a global variable
		i++;											//Increase counter
	}
	
	to_parse[i] = '\0';									//Add NULL symbol
	messageComplete = 0;                      			//Message received
	buff_ind = 0;										//Restart global variable
	
}



void UART1_writeChar(unsigned char c)
{
	while ((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = c;										//Write character in register
	
	while ((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = '\r';										//RETURN LINE
	
	while ((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = '\n';										//NEW LINE
}



void concatenate2(char *str1, char *str2)
{
    int i, j;											//Declare counters
    
    i = 0;												//Initialise counter
    
    while (str1[i] != '\0')								//Until end is reached
    {
        i++;											//Increase counter
    }
    
    j = 0;												//Initialise second counter
    
    while (str2[j] != '\0')								//Until end is reached
    {
        str1[i] = str2[j];								//Save data
        i++;											//Increase counter
        j++;											//Increase counter
    }

    str1[i] = '\0';  									//Declaring the end of the string
}



void PORTA_IRQHandler()
{
	//Open window
	if (PORTA_PCR4 & (1<<24))							//Determine from where the interruption came
	{
		PORTA_PCR4|=(1<<24);							//Turn down flag
		
		if (!min)
		{
			GPIOE_PCOR=(1<<2);								//Turn off output
			GPIOE_PCOR=(1<<3);								//Turn off output
			
			GPIOE_PSOR=(1<<2);								//Turn on output
		}
		
		max = 0;
	}
	
	//Close window
	if (PORTA_PCR5 & (1<<24))							//Determine from where the interruption came
	{
		PORTA_PCR5|=(1<<24);							//Turn down flag
		
		if ((!obstruction) || (!max))
		{
			GPIOE_PCOR=(1<<2);							//Turn off output
			GPIOE_PCOR=(1<<3);							//Turn off output
			
			GPIOE_PSOR=(1<<3);							//Turn on output
		}
		
		min = 0;
	}
	
	//Upon obstruction, open window
	if (PORTA_PCR13 & (1<<24))							//Determine from where the interruption came
	{
		PORTA_PCR13|=(1<<24);							//Turn down flag
		
		obstruction = 1;
		
		GPIOE_PCOR=(1<<2);								//Turn off output
		GPIOE_PCOR=(1<<3);								//Turn off output
		
		GPIOE_PSOR=(1<<2);								//Turn on output
	}
	
	//Upon lowest limit, disable open
	if (PORTA_PCR16 & (1<<24))							//Determine from where the interruption came
	{
		PORTA_PCR16|=(1<<24);							//Turn down flag
		
		GPIOE_PCOR=(1<<2);								//Turn off output
		GPIOE_PCOR=(1<<3);								//Turn off output
		
		min = 1;
	}
	
	//Upon highest limit, disable close
	if (PORTA_PCR16 & (1<<24))							//Determine from where the interruption came
	{
		PORTA_PCR16|=(1<<24);							//Turn down flag
		
		GPIOE_PCOR=(1<<2);								//Turn off output
		GPIOE_PCOR=(1<<3);								//Turn off output
		
		max = 1;
	}
}



void FTM0_IRQHandler(void)
{
	TPM0_C0SC|=(1<<7);								//Turn down flag

	countM++;										//Increase counter
}



void PIT_IRQHandler()
{

	//Code for monitoring the speed
	//__________________________________________________________________
	
	PIT_TFLG0|=(1<<0);									//Turn down flag

	rpmM = ((60*countM*100*26)/(11*468*17));			//Formula for RPM
	countM = 0;											//Reset counter
	
	raw_value = rpmM & 0xFF ;
	data_requested [0] = raw_value + 1 ;
	data_requested [1] = '\0';
	
	if(!((data_requested [0] == 1) || (data_requested [0] == 53)))
	{
		UART1_write("Sistema de elevación de vidrio tiene falla");
	}
	
}



void UART1_IRQHandler()
{
	UART1_read("off"); 
	
	if (UART1_S1 & (1<<5))								//Receive data register full
	{
		char temp = UART1_D;                     		//Turn down flag and save data
		
		buffer[buff_ind] = temp;                		//Saving read message into buffer
		buff_ind++;										//Increase counter to read complete message
		
		if (temp == to_parse[found_char])				//If coincidence found
		{ 
			found_char++;								//Increase counter to compare
		
			if (to_parse[found_char] == '\0')			//Until NULL symbol found
			{   
				buffer[buff_ind] = '\0';				//Add NULL symbol 
				found_char = 0;							//Reset counter
				
				if (!min)
				{
					GPIOE_PCOR=(1<<2);					//Turn off output
					GPIOE_PCOR=(1<<3);					//Turn off output
					
					GPIOE_PSOR=(1<<2);					//Turn on output
				}
			}
		}
		else
		{
			found_char = 0;                      		//Reset counter
		}
	}
}


int main(void)
{
	
	PORT_init();
	CLK_init();
	TPM0_init();
	TPM1_init();
	UART1_init();
	PIT_init ();
	
	for(;;) 
	{	   
	   	
	}
	
	return 0;
}
