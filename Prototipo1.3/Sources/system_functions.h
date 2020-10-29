/*
 * system_functions.h
 *
 *  Created on: Oct 21, 2020
 *      Author: jair2
 */

#ifndef SYSTEM_FUNCTIONS_H_
#define SYSTEM_FUNCTIONS_H_

void pulse_generator (void);
void UART0_send4(unsigned long rpm);
int I2C0_burstRead(void);

#endif /* SYSTEM_FUNCTIONS_H_ */
