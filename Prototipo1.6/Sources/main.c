/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

9*¿+
e=
#include "derivative.h" 
#include "system_init.h"
#include "system_IRQ.h"
#include "system_functions.h"

extern unsigned long rpmA;									//Speed of the wheelA by encoder
extern unsigned long rpmB;									//Speed of the wheelB by encoder
extern unsigned short duty_cycleA;							//The duty_cycle of motor1
extern unsigned short duty_cycleB;							//The duty cycle of motor2
extern unsigned char paro;
extern unsigned short periodo;								//Frequency* at which the PWM of the motor operates
extern unsigned long* GYROSCOPE;


void FTM0_IRQHandler();
void PIT_IRQHandler();

int main(void)
{
	int counter = 0;
	int control;
	unsigned char estado;
	
	PORT_init();
	CLK_init ();
	//PIT_init();
	//TPM0_init ();
	//TPM1_init ();
	I2C0_init ();
	//UART0_init();
	//IMU_data_processed();
	
	
			
	for(;;) {	   
	   	counter++;

	}
	
	return 0;
}
