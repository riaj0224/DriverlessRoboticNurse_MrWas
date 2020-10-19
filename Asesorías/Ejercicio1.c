/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

/* Description
 * =====================================================================================================
 * Use the KL25Z microcontroller to measure the output of a potentiometer with the help of the ADC module.
 * Depending on the reading obtained, set a range and turn on an RGB LED (digital output).
 * Through the UART, show on the terminal (Tx) the readings obtained from the potentiometer.
 */

#include "derivative.h" /* include peripheral declarations */


/************************************************************************************************************
 * Name: 		PORT_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initiliaze ports (ADC, GPIO, UART)
************************************************************************************************************/
void PORT_init (void)
{
	/*!
	*        Pin Definitions
	* ===============================
	*   Pin Number  |    Function
	* --------------|----------------
	* PTB18			|GPIO [RED_LED]
	* PTB19			|GPIO [GREEN_LED]
	* PTD1			|GPIO [BLUE_LED]
	* PTB0			|ADC  [CH8_POT]
	* PTA1			|UART [Rx]
	* PTA2			|UART [Tx]
	*/
	
	/* Enable Clocks (SIM-12) */
	SIM_SCGC5 |= 1<<9									/* Enable clock for PORT A */
			  |  1<<10									/* Enable clock for PORT B */
			  |  1<<12;									/* Enable clock for PORT D */
	
	/* Configure GPIOs (PORT-11) */
	PORTB_PCR18 = 1<<8;									/* Port B18: MUX = GPIO */
	PORTB_PCR19 = 1<<8;									/* Port B19: MUX = GPIO */
	PORTD_PCR1 = 1<<8;									/* Port D1: MUX = GPIO */
	
	/* GPIOs as outputs (GPIO-41) */
	GPIOB_PDDR |= 1<<18;								/* Port B18: Data direction = output */
	GPIOB_PDDR |= 1<<19;								/* Port B19: Data direction = output */
	GPIOD_PDDR |= 1<<1;									/* Port D1: Data direction = output */
	
	/* Turn-off LEDs */
	GPIOB_PSOR |= 1<<18;								
	GPIOB_PSOR |= 1<<19;									
	GPIOD_PSOR |= 1<<1;		
	
	/* Configure ADC (PORT-11) */
	PORTB_PCR0 = 0<<8;									/* PORT B0: MUX = ADC CH_8 (default) */
	
	/* Configure UART (PORT-11) */
	PORTA_PCR1 = 2<<8;									/* PORT A1: MUX = UART_Rx */
	PORTA_PCR2 = 2<<8;									/* PORT A2: MUX = UART_Tx */
}


/************************************************************************************************************
 * Name: 		ADC0_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initiliaze ADC module
************************************************************************************************************/
void ADC0_init (void)
{
	/* Enable Clocks (SIM-12) */
	SIM_SCGC6 |= 1<<27;									/* Enable clock for ADC0 */	
	
	/* Configure ADC module (ADC-28) */
	ADC0_SC1A &= ~(1<<6)								/* AIEN = 0: Disable interrupts */
			  |  31;									/* ADCH = 31 = 0b11111 = 0x1F: Disable module */
	
	ADC0_CFG1 |= 1<<2; 									/* MODE = 1: 12-bit conversion */
	
	ADC0_SC2 = 0x00000000; 								/* ADTRG = 0: Software trigger */
														/* REFSEL = 0: Voltage reference = VREFH, VREFL */
	
	ADC0_SC3 = 0x00000000; 								/* CAL = 0: Do not start calibration */
}


/************************************************************************************************************
 * Name: 		UART0_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initiliaze UART module
************************************************************************************************************/
void UART0_init (void)
{
	/* Enable Clocks (SIM-12) */
	SIM_SOPT2 |= 1<<26;									/* Select UART Clock Source = MCGFLLCLK (48 MHz)*/
	
	SIM_SCGC4 |= 1<<10;									/* Enable clock for UART0 */	
	
	/* Configure UART module (UART-39) */
	UART0_C4 |= 15;										/* OSR = 15 (default) */
	UART0_BDL = 56;										/* SBR = 312: Baud rate divisor */
	UART0_BDH = 1;
														
	/* Baud = CLK/(OSR + 1)*SBR */
	/* 9600 ~ 48 MHz/(15 + 1)*312 */ 
	
	UART0_C1 = 0; 										/* 8 data bits, No parity, 1 bit Stop (default) */
	
	UART0_C2 |= 1<<3;									/* TE = 1: Enable transmitter */ 
														/* RE = 0: Disable receiver */
														/* TIE, RIE = 0: Disable interrupts */
}


/************************************************************************************************************
 * Name: 		UART0_send
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Send integer
************************************************************************************************************/
void UART0_send (int data)
{
	int indx = 0;										/* Array index */
	unsigned char string[4] = {0};						/* Array to store data */
	
	/* ITOA Function */
	string[3] = ((data % 10) + '0');
	data /= 10;
	string[2] = ((data % 10) + '0');
	data /= 10;
	string[1] = ((data % 10) + '0');
	string[0] = ((data / 10) + '0');
	
	do {
		/* Wait until the transmission buffer is empty */
		while (((UART0_S1 & (1<<7)) >> 7) == 0);
		
		UART0_D = string[indx++];
		
	} while(string[indx] != '\0');
	
	/* Wait until the transmission buffer is empty */
	while (((UART0_S1 & (1<<7)) >> 7) == 0);
	UART0_D = '\r';										/* Return */
	
	/* Wait until the transmission buffer is empty */
	while (((UART0_S1 & (1<<7)) >> 7) == 0);
	UART0_D = '\n';										/* New line */
	
}


int main(void)
{
	MCG_C1 |= 0<<6;										/* CLKS = 0: MCGFLLCLK Source is PLL */
	MCG_C1 |= 1<<2;										/* IREFS = 1: Slow internal reference clock*/
	MCG_C4 |= 1<<5;										/* DRST_DRS = 1: Mid Range */
	MCG_C4 |= 1<<7;										/* DMX32 = 1: DCO reference 48 MHz */
	
	int adc_result_raw = 0;								/* ADC result from the data register */
	int adc_result_mv =  0;								/* ADC result in mV */
	int counter = 0;
	
	PORT_init ();										/* Initialize ports */
	UART0_init ();										/* Initiliaze UART module at 9600 bps */
	ADC0_init ();										/* Initialize ADC module */
	
	
	for(;;) 
	{
		ADC0_SC1A &= ~(31<<0);							/* Clear prior ADCH values */
		ADC0_SC1A |= 8;									/* Start Conversion. Select CH8 (PDB0) */
		
		/* Wait until conversion complete flag COCO */
		while (((ADC0_SC1A & (1<<7)) >> 7) == 0) {}
		
		adc_result_raw = ADC0_RA;						/* Get the value from the ADC module */
		adc_result_mv = (5000 * adc_result_raw)/0xFFF;	/* Convert result to mV for a 0-5 V range */
		
		/* Compare */
		
		if (adc_result_mv > 3750)						/* If result > 3.75 V */
		{
			GPIOB_PCOR |= 1<<18;						/* Turn-on Red LED */				
			GPIOB_PSOR |= 1<<19;						/* Turn-off Green LED */				
			GPIOD_PSOR |= 1<<1;							/* Turn-off Blue LED */
		}
		
		else if (adc_result_mv > 2500)					/* If result > 2.5 V */
		{
			GPIOB_PSOR |= 1<<18;						/* Turn-off Red LED */								
			GPIOB_PCOR |= 1<<19;						/* Turn-on Green LED */							
			GPIOD_PSOR |= 1<<1;							/* Turn-off Blue LED */
		}
		
		else if (adc_result_mv > 1250)					/* If result > 1.25 V */
		{
			GPIOB_PSOR |= 1<<18;						/* Turn-off Red LED */							
			GPIOB_PSOR |= 1<<19;						/* Turn-off Green LED */						
			GPIOD_PCOR |= 1<<1;							/* Turn-on Blue LED */
		}
		
		else
		{
			/* Turn-off all LEDs */
			GPIOB_PSOR |= 1<<18;									
			GPIOB_PSOR |= 1<<19;									
			GPIOD_PSOR |= 1<<1;
		}
		
		if (counter == 5000)
		{
			UART0_send (adc_result_mv);					/* Print ADC value every 5000 readings */
			counter = 0;
		}
	   	counter++;
	}
	
	return 0;
}
