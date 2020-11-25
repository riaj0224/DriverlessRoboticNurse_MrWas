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


void i2c_DisableAck(void);
void i2c_EnableAck(void);
void i2c_RepeatedStart(void);
void i2c_EnterRxMode(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_Wait(void);
void i2c_WriteByte(uint8_t data);
uint8_t i2c_ReadByte(void);
void delay(uint32_t t);
void i2c_WriteRegister(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t data);
int8_t i2c_ReadRegister(uint8_t SlaveAddress,uint8_t RegisterAddress);
int8_t i2c_ReadMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress,uint8_t n_data, uint8_t *res);
void i2c_WriteMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t n_data, uint8_t *data);
void IMU_setup (void);
void IMU_data(void);


void UART0_write(char string[]);
void UART0_writeChar(unsigned char c);
void UART1_write(char string[]);
void UART1_writeChar(unsigned char c);
void UART1_read(char string[]);
void concatenate(char *str1, char *str2, char *shrt);
void concatenate2(char *str1, char *str2);
void WiFi_setup(void);
void WiFi_send(char id,char message[]);
void WiFi_execute_instructions(void);


#endif /* SYSTEM_FUNCTIONS_H_ */
