/*
 * system_init.c
 *
 *  Created on: Oct 19, 2020
 *      Author: jair2
 */
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
	*                         Pin Definitions                                         
	* ===============================================================
	*   Pin Number  |    Function    |         Assigned to          |
	* --------------|----------------|------------------------------|
	* PTA1			|UART0_RX        |                              |
	* PTA2			|UART0_TX        |                              |
	* PTB0			|ADC0_SE8        |                              |
	* PTB1			|ADC0_SE9        |                              |
	* PTB2			|ADC0_SE12       |                              |
	* PTB3			|ADC0_SE13       |                              |
	* PTC12			|GPIO (input)    |                              |
	* PTC13			|GPIO (input)    |                              |
	* PTC16			|GPIO (input)    |                              |
	* PTC17			|GPIO (input)    |                              |
	* PTE0			|UART1_TX        |                              |
	* PTE1			|UART1_RX        |                              |
	* PTE2			|GPIO (output)   |                              |
	* PTE3			|GPIO (output)   |                              |
	* PTE4			|GPIO (output)   |                              |
	* PTE5			|GPIO (output)   |                              |
	* PTE24			|I2C0_SCL        |                              |
	* PTE25			|I2C0_SDA        |                              |
	*/


/*
==========================================================================================================
 * Name: 		PORT_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initialises ports (ADC, GPIO, UART,)
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
	// UART1
	PORTE_PCR0 = (3<<8);								//Select 3rd alternative: UART1_TX
	PORTE_PCR1 = (3<<8);								//Select 3rd alternative: UART1_RX
	
	// Configure ports as I2C (PORT-11)
	// I2C0
	PORTE_PCR25 = (5<<8);								//Select 5th alternative: I2C0_SCL
	PORTE_PCR24 = (5<<8);								//Select 5th alternative: I2C0_SDA
	
	// Configure ports as ADC (PORT-11)
	//ADC0
	PORTB_PCR0 = (0<<8);								//Select default: ADC0_SE8 
	PORTB_PCR1 = (0<<8);								//Select default: ADC0_SE9 
	PORTB_PCR2 = (0<<8);								//Select default: ADC0_SE12 
	PORTB_PCR3 = (0<<8);								//Select default: ADC0_SE13
	
	// Configure ports as GPIO (PORT-11)
	PORTC_PCR12 = (1<<8);								//Select 1st alternative: GPIO
	PORTC_PCR13 = (1<<8);								//Select 1st alternative: GPIO
	PORTC_PCR16 = (1<<8);								//Select 1st alternative: GPIO
	PORTC_PCR17 = (1<<8);								//Select 1st alternative: GPIO
	PORTE_PCR2  = (1<<8);								//Select 1st alternative: GPIO
	PORTE_PCR3  = (1<<8);								//Select 1st alternative: GPIO
	PORTE_PCR4  = (1<<8);								//Select 1st alternative: GPIO
	PORTE_PCR5  = (1<<8);								//Select 1st alternative: GPIO
	// Output
	GPIOE_PDDR |= (1<<2)								//Set as output
			   |  (1<<3)								//Set as output
			   |  (1<<4)								//Set as output
			   |  (1<<5);								//Set as output	
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
MCG_SC=0;												//Prescaler of 1:1

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
	SIM_SCGC4 |= (1<<10)								//Enable UART0
			  |  (1<<11);								//Enable UART1
	
	// Configure Baud rate to 9600: (UART-39)			Equation: Baud = CLK/((OSR + 1)*SBR)
																//9600 = 4MHz/((15+1)*SBR)
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
