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
void UART0_init(void);
void I2C0_init (void);
void ADC0_init(void);
void TPM0_init (void);
void TPM1_init (void);
void TPM2_init (void);
void PIT_init (void);

#endif /* SYSTEM_INIT_H_ */
