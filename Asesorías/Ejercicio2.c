/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

/* Description
 * =====================================================================================================
 * Use the KL25Z microcontroller to control a step-motor.
 * Configure the LPTMR0 to modify the velocity of the step-motor.
 * Through the UART, receive (Rx) the desired velocity for the step-motor.
 * Use a digital input (button) to change the spin direction of the step-motor.
 * All the program is made with interrupts.
 */

#include "derivative.h" /* include peripheral declarations */

unsigned long secuence [4] = {0x20, 0x01, 0x04, 0x08};
int indx = 0;
int direction = 0;
int data = 0;
int data_temp = 0;

/************************************************************************************************************
 * Name: 		PORT_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initiliaze ports (GPIO, UART)
************************************************************************************************************/
void PORT_init (void)
{
	/*!
	*        Pin Definitions
	* ===============================
	*   Pin Number  |    Function
	* --------------|----------------
	* PTD5			|GPIO [MOTOR]
	* PTD0			|GPIO [MOTOR]
	* PTD2			|GPIO [MOTOR]
	* PTD3			|GPIO [MOTOR]
	* PTD4			|GPIO [BUTTON]
	* PTA1			|UART [Rx]
	* PTA2			|UART [Tx]
	*/
	
	/* Enable Clocks (SIM-12) */
	SIM_SCGC5 |= 1<<9									/* Enable clock for PORT A */
			  |  1<<12;									/* Enable clock for PORT D */
	
	/* Configure GPIOs (PORT-11) */
	PORTD_PCR5 = 1<<8;									/* Port D5: MUX = GPIO */
	PORTD_PCR0 = 1<<8;									/* Port D0: MUX = GPIO */
	PORTD_PCR2 = 1<<8;									/* Port D2: MUX = GPIO */
	PORTD_PCR3 = 1<<8;									/* Port D3: MUX = GPIO */
	
	PORTD_PCR4 |= 1<<8									/* Port D4: MUX = GPIO */
			   |  9<<16;								/* Port D4: IRQC = Interrupt on raising edge  */
	
	/* GPIOs as outputs (GPIO-41) */
	GPIOD_PDDR |= 1<<5;									/* Port D5: Data direction = output */
	GPIOD_PDDR |= 1<<0;									/* Port D0: Data direction = output */
	GPIOD_PDDR |= 1<<2;									/* Port D2: Data direction = output */
	GPIOD_PDDR |= 1<<3;									/* Port D3: Data direction = output */
	
	GPIOD_PDDR &= ~(1<<4);								/* Port D4: Data direction = input (default) */
	
	/* Configure UART (PORT-11) */
	PORTA_PCR1 = 2<<8;									/* PORT A1: MUX = UART_Rx */
	PORTA_PCR2 = 2<<8;									/* PORT A2: MUX = UART_Tx */
}


/************************************************************************************************************
 * Name: 		LPTMR0_init
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Initiliaze LPTMR0 module
************************************************************************************************************/
void LPTMR0_init (void)
{
	/* Enable Clocks (SIM-12) */
	SIM_SOPT1 |= 3<<18;									/* OSC32KSEL = 3: LPO as clock source */
	
	SIM_SCGC5 |= 1;										/* Enable clock for LPTMR */
	
	/* LPTMR Configuration (LPTMR-33)*/
	LPTMR0_CSR = 0;										/* TEN = 0: Disable module */
	
	LPTMR0_PSR |= 1<<2									/* PBYP = 1: Enable bypass */
			   |  1;									/* PCS = 1: Clock Source at 1 kHz (LPO)*/
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
	
	UART0_C2 |= 1<<2									/* RE = 1: Enable receiver */ 
			 |  1<<5;									/* RIE = 1: Enable receiver interrupt */ 
														/* TE = 0: Disable transmitter */
														/* TIE = 0: Disable transmission interrupt */
}



int main(void)
{
	MCG_C1 |= 0<<6;										/* CLKS = 0: MCGFLLCLK Source is PLL */
	MCG_C1 |= 1<<2;										/* IREFS = 1: Slow internal reference clock*/
	MCG_C4 |= 1<<5;										/* DRST_DRS = 1: Mid Range */
	MCG_C4 |= 1<<7;										/* DMX32 = 1: DCO reference 48 MHz */
	
	PORT_init ();										/* Initialize ports */
	LPTMR0_init ();										/* Initialize LPTMR0 */
	UART0_init ();										/* Initiliaze UART module at 9600 bps */
	
	/* Enable Interrupt (NVIC-3) */
	NVIC_ISER |= 1<<28;									/* LPTMR0 */
	NVIC_ISER |= 1<<12;									/* UART0 */
	NVIC_ISER |= 1<<31;									/* PORTD */
	
	for(;;) 
	{
		
	}
	
	return 0;
}


void PORTD_IRQHandler (void)
{
	if ((GPIOD_PDIR & (1<<4)) == 0)						/* Check if the button was pressed */
	{
		PORTD_PCR4 |= 1<<24;							/* Clear flag */
		direction ^= 1;									/* Change motors direction */
	}
}


void LPTimer_IRQHandler (void)
{
	LPTMR0_CSR |= 1<<7;									/* Clear the timeout flag */
	
	if (direction == 0)
	{
		GPIOD_PDOR = secuence[(indx ++) % 4];			/* Move motor clockwise */
	}
	
	else
	{
		GPIOD_PDOR = secuence[(indx --) % 4];			/* Move motor counterclockwise*/
	}
}


void UART0_IRQHandler (void)
{
	data_temp = UART0_D;								/* Store data until ENTER is pressed */
	
	if (data_temp != 13)
	{
		data = (data * 10) + (data_temp - '0');
	}
	
	else 
	{
		LPTMR0_CSR = 0;									/* TEN = 0: Disable module */
		LPTMR0_CMR = (data - 1);						/* Timeout */	
		LPTMR0_CSR |= 1<<6								/* TIE = 1: Enable Interrupt */				
				   |  1;								/* TE = 1: Enable module */
		
		data = 0;										/* Restart data */
	}
}
