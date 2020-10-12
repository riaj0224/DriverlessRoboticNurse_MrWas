/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "US_sensor.h"

extern unsigned char ADC_value;

int main(void)
{
	US_sensor_output_init();
	US_sensor_generate_pulse();
	
	return 0;
}
