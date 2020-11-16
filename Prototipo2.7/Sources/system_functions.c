/*
 * system_functions.c
 *
 *  Created on: Oct 21, 2020
 *      Author: jair2
 */



/* Description
 * =====================================================================================================
 * Use the KL25Z micro-controller to generate all the functions needed to realise the different
 * tasks related to the project: detect distances below 50 centimeter, measuring the motors speed by the
 * encoder, reading the data from the IMU (MPU 6050) to compare it with the encoders and analyse the
 * rest of the kinematics data, and establishing connection via Wi-Fi to receive data/instructions and 
 * send information.     
 */



/*
	                                    Pin Definitions                                               
	====================================================================================
    Pin Number  |    Function    |         Assigned to          |     Functions by     |
	------------|----------------|------------------------------|----------------------|
	PTA1		|UART0_RX        |Terminal management           |Polling               |
	PTA2		|UART0_TX        |Terminal management           |Polling               |
	PTB0		|ADC0_SE8        |Reading temperature           |Polling               |
	PTB2		|I2C0_SCL        |Reading the IMU               |Interruption          |
	PTB3		|I2C0_SDA        |Reading the IMU               |Interruption          |
	PTC1		|TPM0_CH0        |Reading frequency motor1      |Interruption          |
	PTC2		|TPM0_CH1        |Reading frequency motor2      |Interruption          |
	PTC3		|TPM0_CH2        |Reading return pulse us sensor|Polling               |
	PTE0		|UART1_TX        |WiFi data shipping            |Interruption          |
	PTE1		|UART1_RX        |WiFi data receipt             |Interruption          |
	PTE2        |GPIO(output)    |Sending input pulse to sensor |Polling               |
	PTE20		|TPM1_CH0        |Sending PWM signal to motor1  |Polling               |
	PTE22		|TPM2_CH0        |Sending PWM signal to motor2  |polling               |
*/



// Libraries needed for the file 
//**************************************************************************************

#include "derivative.h"
#include "system_functions.h"
#include "math.h"

//**************************************************************************************



// Define the registers needed to configure the IMU, as well as the slave address
//**************************************************************************************

#define MPU6050_ADDR         0x68						//Slave address
#define MPU6050_SMPLRT_DIV   0x19						//Register for frequency and time
#define MPU6050_CONFIG       0x1a						//Register for external clocks
#define MPU6050_GYRO_CONFIG  0x1b						//Register for gyroscope configuration
#define MPU6050_ACCEL_CONFIG 0x1c						//Register for acceleration configuration
#define MPU6050_PWR_MGMT_1   0x6b						//Register for initialising the IMU

#define MPU6050_DATA         0x3b						//Register from starting point to read data

//**************************************************************************************



// Declaration of important constants for reading the IMU
//**************************************************************************************

#define GYRO_LSB_2_DEGSEC  65.5     					// [bit/(°/s)]
#define ACC_LSB_2_G        16384.0  					// [bit/gravity]
#define RAD_2_DEG          57.29578 					// [°/rad]
#define GYRO_OFFSET_NB_MES 3000     					//
#define TEMP_LSB_2_DEGREE  340.0    					// [bit/celsius]
#define TEMP_LSB_OFFSET    12412.0  					//
#define DEFAULT_GYRO_COEFF 0.98

const double R2G = 180.0/3.141592654;
const double ASF = 16384.0;
const double GSF = 131.0;

//**************************************************************************************



// Declaration of variables used for reading the IMU
//**************************************************************************************

double       AngX,AngY,GyrX,GyrY,Temp;

float gyroXoffset, gyroYoffset, gyroZoffset;
float temp, accX, accY, accZ, gyroX, gyroY, gyroZ;
float angleAccX, angleAccY;
float angleX, angleY, angleZ;
long preInterval;
float accCoef, gyroCoef;

int16_t Acc_rawX, Acc_rawY, Acc_rawZ, Mpu_rawT, Gyr_rawX, Gyr_rawY, Gyr_rawZ;

//**************************************************************************************



// Declaration of variables used for reading the Wi-Fi module
//**************************************************************************************

char buffer[500] = "";
char to_parse[100] = "";
unsigned char found_char = 0;
unsigned char buff_ind = 0;
unsigned char messageComplete = 0;
extern unsigned short duty_cycleA;							//The duty_cycle of motor1

//**************************************************************************************



// Functions used for the sensors
//**************************************************************************************

/*
==========================================================================================================
 * Name: 		pulse_generator
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Generate a 10us pulse for the ultra-sonic sensor
==========================================================================================================
*/

void pulse_generator (void)
{
	/*
	PIT_TCTRL1=(1<<0);									//Encender timer
	GPIOE_PSOR=(1<<2);									//prender pin US
	do{}while (PIT_TFLG1==0);							//espera 10 us
	GPIOE_PCOR=(1<<2);									//apaga pin US
	PIT_TCTRL1&=~(1<<0);								//apaga timer
	*/
	
	TPM1_CNT = 1;										//Reset counter
	GPIOE_PDOR=(1<<2);									//Turn on output
	TPM1_C1V=TPM1_CNT+40;								//400 = 40 pulsos de 25 ns para generar un 1 us
	do{}while ((TPM1_C1SC & (1<<7))==0);				//Wait
	TPM1_C1SC|=(1<<7);									//Turn down flag
	GPIOE_PDOR=0;										//Turn off output
}

//**************************************************************************************



// Functions used for reading info. via UART
//**************************************************************************************

/*
==========================================================================================================
 * Name: 		UART0_send4
 * Parameter: 	unsigned long (whichever data is needed to read)
 * Returns: 	NONE
 * Description: Print in terminal the data
==========================================================================================================
*/

void UART0_send4(unsigned long rpm){
	
		int indx = 0;
		unsigned char string[4] = {0};
		
		//ITOA
		string[3] = (rpm%10) + '0';
		rpm /= 10;
		string[2] = (rpm%10) + '0';
		rpm /= 10;
		string[1] = (rpm%10) + '0';
		string[0] = (rpm/10) + '0';
		
		do{
			while((UART0_S1 & (1<<7)) == 0);         //Wait until the transmission buffer is empty
			UART0_D = string[indx++];
		}while(string[indx] != '\0');                //Waiting for null character
		
		while((UART0_S1 & (1<<7)) == 0);
		UART0_D = '\r';                                //Return
		while((UART0_S1 & (1<<7)) == 0);             
		UART0_D = '\n';                                //New line

}

//**************************************************************************************



// Functions used for reading the IMU by I2C
//**************************************************************************************

/*
==========================================================================================================
 * Name: 		i2c_DisableAck
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Send NACK signal
==========================================================================================================
*/

void i2c_DisableAck(void)
{
	I2C0_C1 |= (1<<3);										//NACK
}



/*
==========================================================================================================
 * Name: 		i2c_EnableAck
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Send ACK signal
==========================================================================================================
*/

void i2c_EnableAck(void)
{
	I2C0_C1 &= ~(1<<3);										//ACK
}



/*
==========================================================================================================
 * Name: 		i2c_RepeatedStart
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Send restart signal
==========================================================================================================
*/

void i2c_RepeatedStart(void)
{
	I2C0_C1 |= (1<<2);										//Restart
}



/*
==========================================================================================================
 * Name: 		i2c_EnterRxMode
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Enter receiver mode
==========================================================================================================
*/

void i2c_EnterRxMode(void)
{
	I2C0_C1 &= ~(1<<4);										//Change to receiver
}



/*
==========================================================================================================
 * Name: 		i2c_Start
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Send START signal
==========================================================================================================
*/

void i2c_Start(void)
{
	I2C0_C1 |= (1<<4);										//Send
	I2C0_C1 |= (1<<5);										//Master
}



/*
==========================================================================================================
 * Name: 		i2c_Stop
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Send STOP signal
==========================================================================================================
*/

void i2c_Stop(void)
{
	I2C0_C1 &= ~(1<<5);										//Slave
	I2C0_C1 &= ~(1<<4);										//Receiver
}



/*
==========================================================================================================
 * Name: 		i2c_Wait
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Wait for answer
==========================================================================================================
*/

void i2c_Wait(void)
{
	uint32_t i = 0;											//Declare counter
	i = 1000000;											//Set value for counter

	while(((I2C0_S & (1<<1)) == 0) && i)					//Interrupt pending or counter zero
	{
		i--;												//Decrease counter
	}
	I2C0_S |= (1<<1);										//Turn down flag
}



/*
==========================================================================================================
 * Name: 		i2c_Wait
 * Parameter: 	uint8_t (whichever data of 8bits is going to be sent)
 * Returns: 	NONE
 * Description: Write on register
==========================================================================================================
*/

void i2c_WriteByte(uint8_t data)
{
	I2C0_D = (uint8_t)data;									//Write in register
}



/*
==========================================================================================================
 * Name: 		i2c_ReadByte
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Read register
==========================================================================================================
*/

uint8_t i2c_ReadByte(void)
{
	return (uint8_t)( I2C0_D);								//Read register
}



/*
==========================================================================================================
 * Name: 		delay
 * Parameter: 	amount of delay
 * Returns: 	NONE
 * Description: delay 
==========================================================================================================
*/

void delay(uint32_t t)
{
	uint32_t cnt = 0;									//Declare counter
	for(cnt=0; cnt<t; cnt++)							//Loop until set value
	{
		__asm("nop");									//Delay function
	};
}



/*
==========================================================================================================
 * Name: 		i2c_WriteRegister
 * Parameter: 	Slave address, register address and data
 * Returns: 	NONE
 * Description: Write data in a register according to protocol
==========================================================================================================
*/

void i2c_WriteRegister(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t data)
{
	
	i2c_Start();
	i2c_WriteByte((SlaveAddress << 1));
	i2c_Wait();

	i2c_WriteByte(RegisterAddress);
	i2c_Wait();

	i2c_WriteByte(data);
	i2c_Wait();

	i2c_Stop();

	delay(50);
}



/*
==========================================================================================================
 * Name: 		i2c_ReadRegister
 * Parameter: 	Slave address, and register address
 * Returns: 	int8_t
 * Description: Read and return the data from a register according to protocol
==========================================================================================================
*/

int8_t i2c_ReadRegister(uint8_t SlaveAddress,uint8_t RegisterAddress)
{
	uint8_t res = 0;

	i2c_Start();
	i2c_WriteByte((SlaveAddress << 1));
	i2c_Wait();

	i2c_WriteByte(RegisterAddress);
	i2c_Wait();

	i2c_RepeatedStart();

	i2c_WriteByte(((SlaveAddress << 1) | 1));
	i2c_Wait();

	i2c_EnterRxMode();
	i2c_DisableAck();

	res = i2c_ReadByte();
	i2c_Wait();
	i2c_Stop();
	res = i2c_ReadByte();
	delay(50);

	return res;
}



/*
==========================================================================================================
 * Name: 		i2c_ReadMultRegister
 * Parameter: 	Slave address, register address, iterations, where to storage
 * Returns: 	pointer with all the data
 * Description: Read and return the data from a register according to protocol
==========================================================================================================
*/

int8_t i2c_ReadMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress,uint8_t n_data, uint8_t *res)
{
	uint8_t i = 0;										//Counter to recover data from several registers

	i2c_Start();
	i2c_WriteByte((SlaveAddress << 1));
	i2c_Wait();

	i2c_WriteByte(RegisterAddress);
	i2c_Wait();

	i2c_RepeatedStart();

	i2c_WriteByte(((SlaveAddress << 1) | 1));
	i2c_Wait();

	i2c_EnterRxMode();
	i2c_EnableAck();

	*res = i2c_ReadByte();								//CHECK
	i2c_Wait();

	for(i=0;i<(n_data-2);i++)							//Loop to recover data from all registers
	{
		*res = i2c_ReadByte();							//Store data
		res++;											//Increase storage capacity 
		i2c_Wait();
	}

	i2c_DisableAck();

	*res = i2c_ReadByte();								//Read last data
	res++;												//Increase storage capacity						
	i2c_Wait();

	i2c_Stop();

	*res = i2c_ReadByte();								//Read NULL

	delay(50);
}



/*
==========================================================================================================
 * Name: 		i2c_WriteMultRegister
 * Parameter: 	Slave address, register address, iterations, data to write
 * Returns: 	NONE
 * Description: Write the data in several registers
==========================================================================================================
*/

void i2c_WriteMultRegister(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t n_data, uint8_t *data)
{
	uint8_t i = 0;										//Declare counter to recover n amount of data

	i2c_Start();
	i2c_WriteByte((SlaveAddress << 1));
	i2c_Wait();

	i2c_WriteByte(RegisterAddress);
	i2c_Wait();

	for(i=0;i<n_data;i++)								//Loop to write data
	{
		i2c_WriteByte(*data);							
		i2c_Wait();
		data++;											//Increase counter
	}
	i2c_Stop();

	delay(50);
}



/*
==========================================================================================================
 * Name: 		IMU_setup
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Configure the IMU to start reading
==========================================================================================================
*/

void IMU_setup (void)
{
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_SMPLRT_DIV,  0x00);
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_CONFIG,      0x00);
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_GYRO_CONFIG, 0x08);
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_ACCEL_CONFIG,0x00);
	i2c_WriteRegister(MPU6050_ADDR, MPU6050_PWR_MGMT_1,  0x01);
}



/*
==========================================================================================================
 * Name: 		IMU_data
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Recover data from IMU
==========================================================================================================
*/

void IMU_data(void)
{
	int8_t data[15];									//Declare array to store data
	int RA;												//Declare register to read
	int i = 0;											//Initialise counter
	
	while(i < 14)
	{
		RA = 59 + i;									//Update register to read
		data[i] = i2c_ReadRegister(MPU6050_ADDR,RA);	//Read register
		i++;											//Increase counter//Combine HIGH and LOW registers
	}
	
	
	Acc_rawX = (data[0]<<8) |data[1];					//Combine HIGH and LOW registers
	Acc_rawY = (data[2]<<8) |data[3];					//Combine HIGH and LOW registers
	Acc_rawZ = (data[4]<<8) |data[5]; 					//Combine HIGH and LOW registers
	Mpu_rawT = (data[6]<<8) |data[7];					//Combine HIGH and LOW registers
	Gyr_rawX = (data[8]<<8) |data[9];					//Combine HIGH and LOW registers
	Gyr_rawY = (data[10]<<8)|data[11];					//Combine HIGH and LOW registers
	Gyr_rawZ = (data[12]<<8)|data[13];					//Combine HIGH and LOW registers
	
	
	accX = ((float)Acc_rawX) / ACC_LSB_2_G;				//Cast variable and transform units
	accY = ((float)Acc_rawY) / ACC_LSB_2_G;				//Cast variable and transform units
	accZ = ((float)Acc_rawZ) / ACC_LSB_2_G;				//Cast variable and transform units
	temp = (Mpu_rawT + TEMP_LSB_OFFSET) / TEMP_LSB_2_DEGREE;//Cast variable and transform units
	gyroX = ((float)Gyr_rawX) / GYRO_LSB_2_DEGSEC;		//Cast variable and transform units
	gyroY = ((float)Gyr_rawY) / GYRO_LSB_2_DEGSEC;		//Cast variable and transform units
	gyroZ = ((float)Gyr_rawZ) / GYRO_LSB_2_DEGSEC;		//Cast variable and transform units
}

//**************************************************************************************



// Functions used for transmitting and receiving data from the Wi-Fi module
//**************************************************************************************

/*
==========================================================================================================
 * Name: 		UART0_write
 * Parameter: 	String
 * Returns: 	NONE
 * Description: Send data to UART0 (Terminal)
==========================================================================================================
*/

void UART0_write(char string[])
{
	while((UART0_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART0_D = '\n';										//NEW LINE
	
	unsigned char i = 0;								//Initialise counter
	
	do{
		
		while((UART0_S1 & (1<<7)) == 0);				//Wait till data buffer is empty
		UART0_D = string[i++];							//Send string character by character
		
	}while(string[i] != '\0');							//Wait till NULL symbol
	
	while((UART0_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART0_D = '\r';										//RETURN LINE
	
	while((UART0_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART0_D = '\n';										//NEW LINE
}



/*
==========================================================================================================
 * Name: 		UART0_writeChar
 * Parameter: 	Char
 * Returns: 	NONE
 * Description: Send data to UART0 (Terminal)
==========================================================================================================
*/

void UART0_writeChar(unsigned char c)
{
	while ((UART0_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART0_D = c;										//Send char
}



/*
==========================================================================================================
 * Name: 		UART1_write
 * Parameter: 	String
 * Returns: 	NONE
 * Description: Transmit data between KL25Z and Wi-Fi module
==========================================================================================================
*/

void UART1_write(char string[])
{
	UART0_write(string);								//Send string to terminal
	
	unsigned char i = 0;								//Initialise counter
	
	do{
		
		while((UART1_S1 & (1<<7)) == 0);				//Wait till data buffer is empty
		UART1_D = string[i++];							//Send string character by character
		
	}while(string[i] != '\0');							//Wait till the string shows the NULL symbol
	
	while((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = '\r';										//RETURN LINE
	
	while((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = '\n';										//NEW LINE
}



/*
==========================================================================================================
 * Name: 		UART1_writeChar
 * Parameter: 	Char
 * Returns: 	NONE
 * Description: Transmit data between KL25Z and Wi-Fi module
==========================================================================================================
*/

void UART1_writeChar(unsigned char c)
{
	while ((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = c;										//Write character in register
	
	while ((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = '\r';										//RETURN LINE
	
	while ((UART1_S1 & (1<<7)) == 0);					//Wait till data buffer is empty
	UART1_D = '\n';										//NEW LINE
}



/*
==========================================================================================================
 * Name: 		UART1_read
 * Parameter: 	String
 * Returns: 	NONE
 * Description: Receive data between KL25Z and Wi-Fi module
==========================================================================================================
*/

void UART1_read(char string[])
{
	unsigned char i = 0;								//Initialise counter
	
	while(string[i] != '\0')							//Work until the NULL (end) symbol
	{							
		to_parse[i] = string[i];						//Store data in a global variable
		i++;											//Increase counter
	}
	
	to_parse[i] = '\0';									//Add NULL symbol
	messageComplete = 0;                      			//Message received
	buff_ind = 0;										//Restart global variable
	
	UART1_C2 |= (1<<5);                      			//Enable interruption
}



/*
==========================================================================================================
 * Name: 		concatenate
 * Parameter: 	3 strings
 * Returns: 	NONE
 * Description: Send info via WiFi
==========================================================================================================
*/

void concatenate(char *str1, char *str2, char *shrt)
{
    int i, j, k;										//Declare counters
    
    i = 0;												//Initialise counter
    
    while (str1[i] != '\0')								//Until end is reached
    {
        i++;											//Increase counter
    }
    
    j = 0;												//Initialise second counter
    
    while (str2[j] != '\0')								//Until end is reached
    {
        str1[i] = str2[j];								//Save data
        i++;											//Increase counter
        j++;											//Increase counter
    }
    
    k=0;												//Initialise counter
    
    while (shrt[k] != '\0')								//Until end is reached
    {
        str1[i] = shrt[k];								//Save data
        i++;											//Increase counter
        k++;											//Increase counter
    }

    str1[i] = '\0';  									//Declaring the end of the string
}



/*
==========================================================================================================
 * Name: 		concatenate2
 * Parameter: 	2 strings
 * Returns: 	NONE
 * Description: Concatenate info to give format
==========================================================================================================
*/

void concatenate2(char *str1, char *str2)
{
    int i, j;											//Declare counters
    
    i = 0;												//Initialise counter
    
    while (str1[i] != '\0')								//Until end is reached
    {
        i++;											//Increase counter
    }
    
    j = 0;												//Initialise second counter
    
    while (str2[j] != '\0')								//Until end is reached
    {
        str1[i] = str2[j];								//Save data
        i++;											//Increase counter
        j++;											//Increase counter
    }

    str1[i] = '\0';  									//Declaring the end of the string
}



/*
==========================================================================================================
 * Name: 		WiFi_setup
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Configure the Wi-Fi module
==========================================================================================================
*/

void WiFi_setup(void)
{
	UART1_write("AT+RST");                              //Reset module
	UART1_read("ready");                                //Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	UART1_write("AT");                                  //Confirm communication
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	UART1_write("AT+CWMODE=1");                        	//Configure as client
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	UART1_write("AT+CWLAP");                            //Show available webs
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	UART1_write("AT+CWJAP=\"INFINITUM55FE_2.4\",\"7025383801\"");//Connect to web
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	UART1_write("AT+CIFSR");                            //Show IP
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	UART1_write("AT+CIPMUX=1");                         //Configure for multiple connections
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	UART1_write("AT+CIPSERVER=1,80");                   //Configure PORT
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
}



/*
==========================================================================================================
 * Name: 		WiFi_send
 * Parameter: 	Connection ID, message
 * Returns: 	NONE
 * Description: Send data via UDP
==========================================================================================================
*/

void WiFi_send(char id,char message[])
{                      
	unsigned char len = 0;								//Initialise counter for message length
	
	while(message[len] != '\0') len++;					//Calculate message length
	
	char open[] = "AT+CIPSEND=i,le";					//Initialise command to prepare message shipment
	char close[] = "AT+CIPCLOSE=i";						//Initialise command to close message shipment
	
	open[11] = id;										//Choose to which client send the data and add it to the open command
	open[13] = (len/10) + '0';							//Write second digit of the message's length 
	open[14] = (len%10) + '0';							//Write first digit of the message's length
	close[12] = id;										//Choose to which client send the data and add it to the close command
	
	UART1_write(open);									//Send command to prepare message shipment
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	UART1_write(message);								//Ship message
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	UART1_write(close);									//Send command to close message shipment
	UART1_read("OK");									//Read specific answer
	while(!(messageComplete));							//Message received confirmation
}



/*
==========================================================================================================
 * Name: 		WiFi_execute_instructions
 * Parameter: 	NONE
 * Returns: 	NONE
 * Description: Send data via UDP
==========================================================================================================
*/

void WiFi_execute_instructions(void)
{                         
	char instruction[100] = "";							//Initialise variable to storage instruction
	char pre_instruction[] = "GET /";					//First string to localise
	char pre_id[] = "+IPD,";							//Second string to localise
	
	unsigned char instruction_obtained = 0;				//Control variable to detect command
	unsigned char id;									//Variable to save client ID request
	unsigned char pre_id_found = 0;						//Control variable to detect second string
	unsigned char pre_inst_found = 0;					//Control variable to detect first string
	unsigned char inst_ind = 0;							//Instruction finder counter (detect length)
	
	unsigned char i = 0;								//Initialise counter	
	
	char string1[100] = "<h1>";
	char string2[100] = "<h1/>";
	char data_requested [100] = "";
	int raw_value;

		
	UART1_read(" HTTP/");								//Read specific answer
	while(!(messageComplete));							//Message received confirmation
	
	while (!(instruction_obtained))						//Until instruction is found
	{					
		if (pre_id_found)								//If first string found
		{
			if (pre_inst_found)							//If second string found
			{
				instruction[inst_ind] = buffer[i];  	//Storage the instruction     
				inst_ind++;								//Increase counter 
				
				if (buffer[i] == '\0')					//Wait till NULL symbol
				{
					instruction_obtained = 1;			//Confirm instruction received
					instruction[inst_ind-7] = '\0';		//Add NULL symbol to instruction
				}
			}
			else
			{
				if (buffer[i] == pre_instruction[inst_ind])	//Find second guide string
				{           
					inst_ind++;								//Increase counter of the desired string
					if (pre_instruction[inst_ind] == '\0')	//Wait until NULL
					{            
						pre_inst_found = 1;					//Confirm second string found
						inst_ind = 0;						//Set counter back to 0
					}
				}
				else 
					inst_ind = 0;							//If not found reset counter
			}
		}
		else
		{
			if (buffer[i] == pre_id[inst_ind])				//Find first guide string
			{                        
				inst_ind++;									//Increase counter of the desired string
				if (pre_id[inst_ind] == '\0')				//Wait until NULL
				{                         
					pre_id_found = 1;						//Confirm first string found
					id = buffer[++i];						//Get client IP
					inst_ind = 0;							//Set counter back to 0
				}
			}
			else 
				inst_ind = 0;								//If not found reset counter
		}
		i++;
	}
	
	switch (instruction[0])
	{
		case 'f':
			WiFi_send(id, "<h1> Robot moving forward <h1/>");
			break;
			
		case 'b':
			WiFi_send(id, "<h1> Robot moving reverse <h1/>");
			break;
			
		case 's':
			WiFi_send(id, "<h1> Robot stopping <h1/>");
			break;
			
		case 'l':
			WiFi_send(id, "<h1> Robot turning left <h1/>");
			break;
			
		case 'r':
			WiFi_send(id, "<h1> Robot turning right <h1/>");
			break;
			
		case 'd':
			raw_value = duty_cycleA & 0xFF ;
			data_requested [0] = (raw_value/100) + 48 ;
			data_requested [1] = ((raw_value%100)/10) + 48 ;
			data_requested [2] = ((raw_value%100)%10) + 48 ;
			data_requested [3] =  '%';
			concatenate2(string1,"The duty cycle of motor A is of ");
			concatenate2(string1,data_requested);
			concatenate2(string1,string2);
			WiFi_send(id, string1);
			break;
				
		default:
			break;
	}
}

//**************************************************************************************
