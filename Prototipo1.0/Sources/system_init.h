/*
 * system_init.h
 *
 *  Created on: Oct 19, 2020
 *      Author: jair2
 */

#ifndef SYSTEM_INIT_H_
#define SYSTEM_INIT_H_

void PORT_init(void);
void CLK_init (void);
void UART_init(void);
void I2C_init (void);
void ADC_init(void);

#endif /* SYSTEM_INIT_H_ */
