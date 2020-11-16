/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "derivative.h" 
#include "system_init.h"
#include "system_IRQ.h"
#include "system_functions.h"

extern unsigned long rpmA;									//Speed of the wheelA by encoder
extern unsigned long rpmB;									//Speed of the wheelB by encoder
extern unsigned short duty_cycleA;							//The duty_cycle of motor1
extern unsigned short duty_cycleB;							//The duty cycle of motor2
extern unsigned char paro;


extern char buffer[500];
extern char to_parse[100];
extern unsigned char found_char;
extern unsigned char buff_ind;
extern unsigned char messageComplete;






void FTM0_IRQHandler();
void PIT_IRQHandler();
void UART1_IRQHandler();

int main(void)
{
	int counter = 0;
	
	PORT_init();
	CLK_init ();
	//PIT_init();
	//TPM0_init();
	//TPM1_init();
	//TPM2_init();
	//I2C0_init();
	//IMU_setup();
	UART0_init();
	UART1_init();
	WiFi_setup();
	
	
	for(;;) {	   
	   	counter++;
	   	WiFi_execute_instructions();

	}
	
	return 0;
}
