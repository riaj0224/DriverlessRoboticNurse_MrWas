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


#include "derivative.h"
#include "system_init.h"

	/*
	                                    Pin Definitions                                               
	====================================================================================
    Pin Number  |    Function    |         Assigned to          |     Functions by     |
	------------|----------------|------------------------------|----------------------|
	PTA1		|UART0_RX        |WiFi data receipt             |Polling               |
	PTA2		|UART0_TX        |WiFi data shipping            |Polling               |
	PTA12		|TPM1_CH0        |Reading speed from motor 1    |Interruption          |
	PTA13		|TPM1_CH1        |Reading speed from motor 2    |Interruption          |	
	PTB0		|ADC0_SE8        |Reading temperature           |Polling               |
	PTC1		|TPM0_CH0        |Sending PWM signal to motor1  |Interruption          |
	PTC2		|TPM0_CH1        |Sending PWM signal to motor2  |Interruption          |
	PTC3		|TPM0_CH2        |Reading return pulse sensor   |Polling               |	
	PTE2        |GPIO(output)    |Sending input pulse to sensor |Polling               |
	PTE24		|I2C0_SCL        |Reading the IMU               |Interruption          |
	PTE25		|I2C0_SDA        |Reading the IMU               |Interruption          |
	*/
	extern unsigned char duty_cycleA;					//From other function take the variable
	extern unsigned char duty_cycleB;					//From other function take the variable
	extern unsigned char espejo_pinA;					//From other function take the variable
	extern unsigned char espejo_pinB;					//From other function take the variable
	

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
	
	// Configure ports as I2C (PORT-11)
	// I2C0
	PORTE_PCR25 = (5<<8);								//Select 5th alternative: I2C0_SCL
	PORTE_PCR24 = (5<<8);								//Select 5th alternative: I2C0_SDA
	
	// Configure ports as ADC (PORT-11)
	//ADC0
	PORTB_PCR0 = (0<<8);								//Select default: ADC0_SE8 
	
	// Configure ports as TPM (PORT-11)
	//TPM0
	PORTC_PCR1 = (4<<8);								//Select 4th alternative: TPM0_CH0 
	PORTC_PCR2 = (4<<8);								//Select 4th alternative: TPM0_CH1
	PORTC_PCR3 = (4<<8);								//Select 4th alternative: TPM0_CH2
	
	// TMP1
	PORTA_PCR12 = (3<<8);								//Select 3rd alternative: TPM1_CH0
	PORTA_PCR13 = (3<<8);								//Select 3rd alternative: TPM1_CH1
	
	
	// Configure ports as GPIO (PORT-11)
	//GPIO
	PORTE_PCR2  = (1<<8);								//Select 1st alternative: GPIO
	//Output
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
 * Name: 		UART_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises UART as both transmitter and receiver
==========================================================================================================
*/

void UART_init(void)
{
	// Enable UARTs from the clocks: (SIM-12)
	SIM_SCGC4 |= (1<<10);								//Enable UART0
	
	// Configure Baud rate to 9600: (UART-39)			Equation: Baud = CLK/((OSR + 1)*SBR) =>9600 = 4MHz/((15+1)*SBR)
	UART0_C4 |= 15;										//Configure OSR as 15
	UART0_BDH=0;										//Configure SBR as 26*
	UART0_BDL=26;										//Configure SBR as 26*
	
	// Configure UART as a 8-bit data without parity: (UART-39)
	UART0_C1 = 0;										//8-bit data without any parity
	
	// Select function mode: receiver, transmitter, by polling or interruptions: (UART-39)
	UART0_C2 = 12;										//Enable both transmitter and receiver by polling	
}


/*
==========================================================================================================
 * Name: 		I2C_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises I2C
==========================================================================================================
*/

void I2C_init (void)
{
	// Enable I2C from the clocks (SIM-12)
	SIM_SCGC4 |= (1<<6);								//Enable I2C0
	
	// Enable access to clicks
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

void ADC_init (void)
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
 * Description: Initialises TPM0 for the motors' signal
==========================================================================================================
*/

void TPM0_init (void)
{
	unsigned short periodo = 4000;						//Set period for 4000 pulses (1ms)
	
	// Enable TPM from the clocks (SIM-12)
	SIM_SCGC6 |= (1<<24);								//Enable TPM0
	
	// Configure TPM module (TPM-31)
	TPM0_SC = (1<<3);									//Set preescaler to 1:1 and select LPTMR as time SRC (4MHz)
	
	// Configure mode (output capture) (TPM-31)
	TPM0_C0SC=(5<<2)+(1<<6);							//Select mode by Output Capture and toggling, plus interruption
	TPM0_C1SC=(5<<2)+(1<<6);							//Select mode by Output Capture and toggling, plus interruption
	TPM0_C2SC=(1<<2); 									//Select mode by Input capture rising edge
	
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER|=(1<<17);									//Enable from both NVIC and software
	
	// Specify the length of time in 0 (TPM-31)
	TPM0_C0V=periodo*(100-duty_cycleA)/100;				//Set period for signal in 0
	TPM0_C1V=periodo*(100-duty_cycleB)/100;				//Set period for signal in 0
	
	espejo_pinA = 0;									//Set variable to 0
	espejo_pinB = 0;									//Set variable to 0
	
	}


/*
==========================================================================================================
 * Name: 		TPM1_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises TPM1 for the motors' encoders
==========================================================================================================
*/

void TPM1_init (void)
{
	// Enable TPM from the clocks (SIM-12)
	SIM_SCGC6 |= (1<<24);								//Enable TPM0
	
	// Configure TPM module (TPM-31)
	TPM1_SC|=(1<<3);									//Set preescaler to 1:1 and select LPTMR as time SRC (4MHz)
	
	// Configure mode (output capture) (TPM-31)	
	TPM1_C0SC=(1<<6)+(2<<2);							//Select mode by input capture and falling edge, enable interruption
	TPM1_C1SC=(1<<6)+(2<<2);							//Select mode by input capture and falling edge, enable interruption
	
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER=(1<<18);									//Enable interruption from NVIC
		
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
	PIT_LDVAL1 = 40;									//As the core clock is of 4Mhz, 40 pulses equals 10 us
	
	// Configure module
	PIT_TCTRL0= (1<<1)+(1<<0); 							//Enable interruption and enable timer '0'
	PIT_TCTRL1&=~(1<<0);								//Turn off timer for the us sensor's input pulse 
	
	// Enable NVIC interruption (Chip Configuration-3)
	NVIC_ISER=(1<<22);									//Enable interruption from NVIC
}

