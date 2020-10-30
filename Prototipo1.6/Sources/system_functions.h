/*
 * system_functions.h
 *
 *  Created on: Oct 21, 2020
 *      Author: jair2
 */

#ifndef SYSTEM_FUNCTIONS_H_
#define SYSTEM_FUNCTIONS_H_

void pulse_generator (void);
void delay (unsigned short tiempo);
void megadelay(int n);
void UART0_send4(unsigned long rpm);
int I2C0_burstRead(unsigned char slaveAddr, unsigned char memAddr, int byteCount, unsigned char* data, int* cnt);
int I2C0_singlebytewrite(unsigned char slaveAddr, unsigned char memAddr,unsigned char byte);
void IMU_data_processed (void);
void ESP_8266_init (void);
unsigned char ESP_8266_send_command (char *p,unsigned char ID,unsigned short timeout_ms);

#endif /* SYSTEM_FUNCTIONS_H_ */
