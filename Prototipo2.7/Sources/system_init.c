/*
 * system_init.c
 *
 *  Created on: Oct 19, 2020
 *      Author: jair2
 */


/* Description
 * =====================================================================================================
 * Use the KL25Z micro-controller to set all the functionalities and PORTs for the Mr. Was project. This
 * includes setting the configurations, clocks, timers, communication protocols, etc. This document only
 * contains the initialising functions. 
 */



/*
	                                    Pin Definitions                                               
	====================================================================================
    Pin Number  |    Function    |         Assigned to          |     Functions by     |
	------------|----------------|------------------------------|----------------------|
	PTA1		|UART0_RX        |Terminal management           |Polling               |
	PTA2		|UART0_TX        |Terminal management           |Polling               |
	PTB0		|ADC0_SE8        |Reading temperature           |Polling               |
	PTB2		|I2C0_SCL        |Reading the IMU               |Interruption          |
	PTB3		|I2C0_SDA        |Reading the IMU               |Interruption          |
	PTC1		|TPM0_CH0        |Reading frequency motor1      |Interruption          |
	PTC2		|TPM0_CH1        |Reading frequency motor2      |Interruption          |
	PTC3		|TPM0_CH2        |Reading return pulse us sensor|Polling               |
	PTE0		|UART1_TX        |WiFi data shipping            |Interruption          |
	PTE1		|UART1_RX        |WiFi data receipt             |Interruption          |
	PTE2        |GPIO(output)    |Sending input pulse to sensor |Polling               |
	PTE20		|TPM1_CH0        |Sending PWM signal to motor1  |Polling               |
	PTE22		|TPM2_CH0        |Sending PWM signal to motor2  |polling               |
*/



// Libraries needed for the file 
//**************************************************************************************

#include "derivative.h"
#include "system_init.h"

//**************************************************************************************



// Declaration of global variables
//**************************************************************************************

unsigned short periodo = 2000;								//Since the system core is of 4MHz it will provide a frequency of 500Hz to the motors
unsigned short duty_cycleA = 80;							//The duty cycle of motor1
unsigned short duty_cycleB = 80;							//The duty cycle of motor2 

//**************************************************************************************



// Functions used to initialise the system
//**************************************************************************************

/*
==========================================================================================================
 * Name: 		PORT_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises ports as ADC, GPIO, UART, etc...
==========================================================================================================
*/

void PORT_init(void)
{
	// Enable PORTs from the clocks (SIM-12)
	SIM_SCGC5 |= (1<<9)									//Enable PORTA
			  |  (1<<10)								//Enable PORTB
			  |  (1<<11)								//Enable PORTC
			  |  (1<<12)								//Enable PORTD
			  |  (1<<13);								//Enable PORTE
	
	
	// Configure ports as UART (PORT-11)
	// UART0
	PORTA_PCR1 = (2<<8);								//Select 2nd alternative: UART0_RX
	PORTA_PCR2 = (2<<8);								//Select 2nd alternative: UART0_TX
	
	//UART1
	PORTE_PCR0 = (3<<8);                				//PORTE 0 as TX
	PORTE_PCR1 = (3<<8);                				//PORTE 1 as RX
	
	// Configure ports as I2C (PORT-11)
	// I2C0
	PORTB_PCR2 = (2<<8);								//Select 2nd alternative: I2C0_SCL
	PORTB_PCR3 = (2<<8);								//Select 2nd alternative: I2C0_SDA
	
	// Configure ports as ADC (PORT-11)
	// ADC0
	PORTB_PCR0 = (0<<8);								//Select default: ADC0_SE8 
	
	// Configure ports as TPM (PORT-11)
	// TPM0
	PORTC_PCR1 = (4<<8);								//Select 4th alternative: TPM0_CH0
	PORTC_PCR2 = (4<<8);								//Select 4th alternative: TPM0_CH1
	PORTC_PCR3 = (4<<8);								//Select 4th alternative: TPM0_CH2
	
	// TPM1
	PORTE_PCR20 = (3<<8);								//Select 3rd alternative: TPM1_CH0
	
	// TPM2
	PORTE_PCR22 = (3<<8);								//Select 3rd alternative: TPM2_CH0
	
	
	// Configure ports as GPIO (PORT-11)
	// GPIO
	PORTE_PCR2  = (1<<8);								//Select 1st alternative: GPIO
	// Output
	GPIOE_PDDR |= (1<<2);								//Set as output
	// Turn off output
	GPIOE_PCOR=(1<<2);									//Turn off output
}



/*
==========================================================================================================
 * Name: 		CLK_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises: MCGOUTCLK (core clock, platform clock and system clock)
 * 				MCGIRCLK (peripherals clock; UART, TPM)
==========================================================================================================
*/

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



/*
==========================================================================================================
 * Name: 		UART0_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises UART0 as both transmitter and receiver
==========================================================================================================
*/

void UART0_init(void)
{
	// Enable UART0 from the clocks: (SIM-12)
	SIM_SCGC4 |= (1<<10);								//Enable UART0
	
	// Configure Baud rate to 9600: (UART-39)			Equation: Baud = CLK/((OSR + 1)*SBR) =>9600 = 4MHz/((15+1)*SBR)
	UART0_C4 |= 15;										//Configure OSR as 15
	UART0_BDH=0;										//Configure SBR as 26*
	UART0_BDL=26;										//Configure SBR as 26*
	
	// Configure UART as a 8-bit data without parity: (UART-39)
	UART0_C1 = 0;										//8-bit data without any parity
	
	// Select function mode: receiver, transmitter, by polling or interruptions: (UART-39)
	UART0_C2 |= (3<<2);									//Enable both transmitter and receiver by polling
	
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER |= (1<<12);                       		//Intr NVIC UART0
}



/*
==========================================================================================================
 * Name: 		UART1_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises UART1 as both transmitter and receiver
==========================================================================================================
*/

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
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER |= (1<<13);                       		//Intr NVIC UART1
}



/*
==========================================================================================================
 * Name: 		I2C_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises I2C
==========================================================================================================
*/

void I2C0_init (void)
{
	// Enable I2C from the clocks (SIM-12)
	SIM_SCGC4 |= (1<<6);								//Enable I2C0
	
	// Enable access to clicks
	I2C0_F = 35;										//Set I2C baud rate to 15,625 and SCL to 256Hz  //OJO registro muy importante
	I2C0_C1 = (1<<7);									//Enable I2C module (allows access to clocks);
}



/*
==========================================================================================================
 * Name: 		ADC_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises ADC with a 12-bit resolution
==========================================================================================================
*/

void ADC0_init (void)
{
	// Enable ADC from the clocks (SIM-12)
	SIM_SCGC6=(1<<27);									//Enable ADC0
	
	// Configure ADC module (ADC-28)
	ADC0_SC1A &= ~(1<<6)								//Disable interruptions
			  |  31;									//Disable module by software
	
	// Configure bit resolution
	ADC0_CFG1 |= 1<<2;									//Bit configuration set to 12-Bit
	
	// Functioning set by software and default voltage compare
	ADC0_SC2 = 0x00000000;								//ADC module configuration
}



/*
==========================================================================================================
 * Name: 		TPM0_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises TPM0 
==========================================================================================================
*/

void TPM0_init (void)
{
	// Enable TPM from the clocks (SIM-12)
	SIM_SCGC6|=(1<<24);									//Enable TPM0
	
	// Configure TPM module (TPM-31)
	TPM0_SC|=(1<<3);									//Set preescaler to 1:1 and select LPTMR as time SRC (4MHz)
	
	// Configure mode (output capture) (TPM-31)	
	TPM0_C0SC=(1<<6)+(2<<2);							//Select mode by input capture and falling edge, enable interruption
	TPM0_C1SC=(1<<6)+(2<<2);							//Select mode by input capture and falling edge, enable interruption
	TPM0_C2SC=(1<<2); 									//Select mode by Input capture rising edge
	
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER|=(1<<17);									//Enable from both NVIC and software
}



/*
==========================================================================================================
 * Name: 		TPM1_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises TPM1: Motor 1
==========================================================================================================
*/

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
	TPM1_C0V=((periodo*(100-duty_cycleA))/100);			//Time in zero 0
	
	// Set MAX value for the counter to restart process
	TPM1_MOD=periodo;									//Period of cycle
			
}



/*
==========================================================================================================
 * Name: 		TPM2_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises TPM2: Mootor2 
==========================================================================================================
*/

void TPM2_init (void)
{
	// Enable TPM from the clocks (SIM-12)
	SIM_SCGC6|=(1<<26);									//Enable TPM2
	
	// Configure TPM module (TPM-31)
	TPM2_SC=(1<<3);										//Set preescaler to 1:1 and select LPTMR as time SRC (4MHz)
	
	// Configure mode (output capture) (TPM-31)	
	TPM2_C0SC=(9<<2);									//Select mode by edge aligned PWM
	
	// Set values for the PWM operation mode
	TPM2_C0V=((periodo*(100-duty_cycleB))/100);			//Time in zero 0
	
	// Set MAX value for the counter to restart process
	TPM2_MOD=periodo;									//Period of cycle
		
}



/*
==========================================================================================================
 * Name: 		PIT_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises TPM1 for the motors' encoders
==========================================================================================================
*/

void PIT_init (void)
{
	// Enable PIT from the clocks (SIM-12)
	SIM_SCGC6|=(1<<23);									//Enable PIT
	
	// Enable PIT timer 
	PIT_MCR&=~(1<<1);									//By default it is in 1, so it must be turned off
	
	// Configure number of pulses for interruption
	PIT_LDVAL0= 400000;									//As the core clock is of 4MHz, 400k pulses equals 100 ms
	PIT_LDVAL1 = 400;									//As the core clock is of 4Mhz, 40 pulses equals 10 us
	
	// Configure module
	PIT_TCTRL0= (1<<1)+(1<<0); 							//Enable interruption and enable timer '0'
	PIT_TCTRL1&=~(1<<0);								//Turn off timer for the us sensor's input pulse 
	
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER=(1<<22);									//Enable interruption from NVIC
}

//**************************************************************************************
