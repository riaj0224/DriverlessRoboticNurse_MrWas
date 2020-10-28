#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "math.h"


unsigned int dato=2;
unsigned int dato1=2;
unsigned int dato2=2;
unsigned int dato3=2;
extern double pos_y;
extern double pos_x;
double pos_z;
unsigned int vel=3.1416;





unsigned int arreglo[];
unsigned short i=0;


unsigned char error = 0;
unsigned char tx = 0;

int8_t aceleracionx[2];
int16_t aceleracion_x;

int8_t aceleraciony[2];
int16_t aceleracion_y;

int8_t aceleracionz[2];
int16_t aceleracion_z;

int8_t gyrox[2];
int16_t gyro_x;

int8_t gyroy[2];
int16_t gyro_y;

int8_t gyroz[2];
int16_t gyro_z;

double pitch_y_acc;
double roll_x_acc;
double yaw_z;

double* datos[4]={&roll_x_acc,&pitch_y_acc,&yaw_z,&dato};



unsigned long bandera=0;
unsigned long tiempo_prev;
float dt;
double pitch_y;
double roll_x;


//*********************

void SIRC_div(void)
{

    // Slow IRC Control Status Register
    SCG->SIRCCSR &= ~ (1 << 24);
    // [24] LK = 0 Control Status Register can be written.

    SCG->SIRCCSR &= ~ (1 << 0);
    // [0] SIRCEN = 0 Disable Slow IRC

    // Slow IRC Divide Register
    SCG->SIRCDIV |= 0x0101;
    // [10-8] SIRCDIV2 0b001 Divide by 1 (8MHz)
    // [2-0]  SIRCDIV1 0b001 Divide by 1 (8MHz)

    SCG->SIRCCSR |= (1 << 0);
    // [0] SIRCEN = 1 Enable Slow IRC

    while((SCG->SIRCCSR & (1 << 24)) == 0); // wait until clock is valid
    // [24] SIRCVLD = 1 Slow IRC is enabled and output clock is valid
}


/*******************************************************************************
Function Name : LPI2C0_init
Notes         : BAUD RATE: 400 kbps
                I2C module frequency 8Mhz (SIRCDIV2_CLK)
                PRESCALER:0x00; FILTSCL/SDA:0x0/0x0; SETHOLD:0x4; CLKLO:0x0B; CLKHI:0x05; DATAVD:0x02
                See Table 50-10 Example timing configuration in S32K1xx Reference manual rev.9
 *******************************************************************************/
void LPI2C0_init(void)
{
    // Peripheral Clock Controller
    PCC-> PCCn[PCC_PORTA_INDEX] = 1<<30;

    // PTA2 ALT3 LPI2C0_SDA gris
    PORTA->PCR[2] |= 3<<8;

    // PTA3 ALT3 LPI2C0_SCL rosita
    PORTA->PCR[3] |= 3<<8;

    PCC->PCCn[PCC_LPI2C0_INDEX] |= 2<<24;   // SIRCDIV2_CLK (8 MHz)
    PCC->PCCn[PCC_LPI2C0_INDEX] |= 1<<30;

    LPI2C0->MCCR0 = 0x0204050B;
    // [24] DATAVD  0x02
    // [16] SETHOLD 0x04
    // [8]  CLKHI   0x05
    // [0]  CLKLO   0x0B

    // Master Interrupt Enable Register (MIER)
    LPI2C0->MIER = (1<<10);
    // [10] NDIE = 1  (NACK detect interrupt enabled)

    // Master Control Register
    LPI2C0->MCR = 0x301;
    // [9] RRF = 1   (Receive FIFO is reset)
    // [8] RTF = 1   (Transmit FIFO is reset)
    // [0] MEN = 1   (Master logic is enabled)

    // LPI2C_Master_IRQHandler
    S32_NVIC->ICPR[0] = (1 << (24 % 32));
    S32_NVIC->ISER[0] = (1 << (24 % 32));
    S32_NVIC->IP[24]  = 0x00;                 // Priority level 0
}

/*******************************************************************************
Function Name : I2C_bus_busy
Returns       : uint8_t
Notes         : I2C Bus is idle/busy
 *******************************************************************************/
static unsigned char bus_busy(void)
{
     error = 0;                 // CLEAR ALL ERRORS

     unsigned int timeout_b = 0;
     while ((LPI2C0->MSR & (1 << 25)) && (timeout_b < 1000))  ++timeout_b;

     if(timeout_b >= 1000) return (error |= (1 << 1));

     /*
      * For debugging purposes
      */ //PTD-> PCOR |= (1 << 0); // BLUE LED ON

     return 0;
}

/*******************************************************************************
Function Name : I2C_start_ACK
Parameters    : uint8_t address
Notes         : Generate (repeated) START and transmit address in DATA[7:0]
 *******************************************************************************/
void generate_start_ACK(uint8_t address)
{
    uint32_t command    = (address << 0);
    command             |= (0b100 << 8);
    LPI2C0->MTDR = command;
}

/*******************************************************************************
Function Name : I2C_write_byte
Parameters    : uint8_t data
Notes         : Transmit DATA[7:0]
 *******************************************************************************/
static void transmit_data(uint8_t data)
{
    LPI2C0->MTDR = data;
}


/*******************************************************************************
Function Name : I2C_stop
Returns       : uint8_t
Notes         : Generate STOP condition
 *******************************************************************************/
static uint8_t generate_stop(void)
{
    uint32_t timeout_s      = 0;
    uint8_t stop_sent_err   = 0;

    LPI2C0->MTDR = 0x0200; //command

    while((!(LPI2C0->MSR & (1 << 9))) && (!stop_sent_err))
    {
        if(timeout_s > 3000)
        {
            error |= (1 << 3);
            stop_sent_err = 1;
        }
        timeout_s++;
    }

    if(LPI2C0->MSR & (1 << 9))
    {
        LPI2C0->MSR |= (1 << 9); // Clear Stop flag
    }

     if(error) return error;
     else return 0;
}

/*******************************************************************************
Function Name : I2C_read
Parameters    : uint8_t *p_buffer, uint8_t n_bytes
Modifies      : uint8_t *p_buffer
Returns       : uint8_t
Notes         : Receive (DATA[7:0] + 1) bytes
 *******************************************************************************/
static void receive_data(int8_t *p_buffer, uint8_t n_bytes)
{
    uint8_t  n;
    uint16_t time;
    uint16_t timeout_r = (2000 * n_bytes);
    uint16_t command;

    command =    0x0100;
    command |=  (n_bytes - 1);
    LPI2C0->MTDR = command;

    while (((LPI2C0->MFSR) >> 16 != n_bytes) && (time < timeout_r)) ++time;

    if(time >= timeout_r)
    {
        LPI2C0->MCR |= (1 << 9);     // reset receive FIFO
        error |= (1 << 2);
    }
    else{
        for(n = 0; n < n_bytes; ++n)
        {
            p_buffer[n] = (uint8_t)(LPI2C0->MRDR & 0x000000FF);
        }
    }
}

/*******************************************************************************
Function Name : I2C_write_reg
Parameters    : uint8_t s_w_address, uint8_t s_reg_address, uint8_t byte
Returns       : uint8_t
Notes         : Write a single byte to a slave's register
 *******************************************************************************/
uint8_t LPI2C0_write(uint8_t s_w_address, uint8_t s_reg_address, uint8_t byte)
{
    if(bus_busy()) return (error |= (1 << 1));
    generate_start_ACK(s_w_address);
    transmit_data(s_reg_address);
    transmit_data(byte);
    if(generate_stop()) return error;
    else return 0;
}

/*******************************************************************************
Function Name : I2C_read
Parameters    : uint8_t s_r_address, uint8_t s_reg_address, uint8_t *p_buffer, uint8_t n_bytes
Modifies      : uint8_t *p_buffer
Returns       : uint8_t
Notes         : Read from a slave
 *******************************************************************************/
int8_t LPI2C0_read(uint8_t s_r_address, uint8_t s_reg_address, int8_t *p_buffer, uint8_t n_bytes)
{
    if(bus_busy()) return (error |= (1 << 1));
    generate_start_ACK(s_r_address - 1);
    transmit_data(s_reg_address);
    generate_start_ACK(s_r_address);
    receive_data(p_buffer, n_bytes);
    if(generate_stop()) return error;
    else return 0;
}


/*******************************************************************************
Function Name : LPI2C0_Master_IRQHandler
 *******************************************************************************/
void LPI2C0_Master_IRQHandler(void)
{
    if(LPI2C0->MSR & (1 << 10))
    {
        error |= (1 << 4);
        // NACK/ACK detected and expecting ACK/NACK for address byte
        // When set, the master will transmit a STOP condition and will not initiate a new START
        // condition until this flag has been cleared.
        LPI2C0->MSR = 0x400;     // clear NDF
    }



}

//************************************************************
void LPIT0_Ch2_IRQHandler(void)
{
  LPIT0->MSR|=1;
  PTD->PTOR=1;
  bandera++;
             LPI2C0_write((0x68<<1),0x6B,0x00);
             LPI2C0_write((0x68<<1),0x1B,0x00);
      	     LPI2C0_write((0x68<<1),0x1C,0x00);

      	     LPI2C0_read((0x68<<1)+1,0x3B,aceleracionx,2);
      	     aceleracion_x =(aceleracionx[0]<<8)+aceleracionx[0];



      	     LPI2C0_read((0x68<<1)+1,0x3D,aceleraciony,2);
      	     aceleracion_y =(aceleraciony[0]<<8)+aceleraciony[0];


      	     LPI2C0_read((0x68<<1)+1,0x3F,aceleracionz,2);
      	     aceleracion_z =(aceleracionz[0]<<8)+aceleracionz[0];

      	     LPI2C0_read((0x68<<1)+1,0x43,gyrox,2);


      	     gyro_x =(gyrox[0]<<8)+gyrox[0];



      	     LPI2C0_read((0x68<<1)+1,0x45,gyroy,2);
      	     gyro_y =(gyroy[0]<<8)+gyroy[0];

      	     LPI2C0_read((0x68<<1)+1,0x47,gyroz,2);
      	     gyro_z =(gyroz[0]<<8)+gyroz[0];

             //Metodo de las aceleraciones por las tangentes
      	     *datos[2]=pitch_y_acc=atan(-1*(((float)aceleracion_x)/16384)/sqrt(pow(((float)aceleracion_y/16384),2)+pow(((float)aceleracion_z/16384),2)))*(57.295779); // 2/32768=1/16384 en terminos de g, 180/pirad=57.295779
             *datos[1]=roll_x_acc=atan(((float)aceleracion_y/16384)/sqrt(pow(((float)aceleracion_x/16384),2)+pow(((float)aceleracion_z/16384),2)))*(57.295779); //se van a la app

             //Metodo del giroscopio
             dt=((float)bandera*(0.03125)-tiempo_prev);
             tiempo_prev=((float)bandera*(0.03125));

             //Aplicar el filtro complementario pasa bajas para el acelerometro y pasa altas para el giroscopio
             roll_x=0.98*(roll_x+(float)((gyro_x)/131)*dt)+0.02*(roll_x_acc); //250/32768=1/131


             pitch_y=0.98*(pitch_y+(float)((gyro_y)/131)*dt)+0.02*(pitch_y_acc);

             //Integracion respecto del tiempo para calcular el YAW

             *datos[3]=yaw_z=yaw_z+(float)((gyro_z)/131)*dt; //Se va a la app




}

void LPIT_init(void){
	 PCC->PCCn[PCC_PORTD_INDEX]=(1<<30);
	    PORTD->PCR[0]=(1<<8);
	    PTD->PDDR=1;
	    PCC->PCCn[PCC_LPIT_INDEX]=(1<<30)+(2<<24);
	    LPIT0->MCR=1;
	    LPIT0->MIER|=4;
	    LPIT0->TMR[2].TVAL=250000; //X CUENTAS de 125ns CMR
	    LPIT0->TMR[2].TCTRL=1;

	    S32_NVIC->ISER[50/32]|=(1<<(50%32));

}

//�******************
void LPUART2_RxTx_IRQHandler (void)
{
                if ((LPUART2->STAT & (1<<23))==(1<<23))   //Rx empty flag
                {
                	LPUART2->DATA = roll_x_acc;

                	//LPUART2->DATA = pitch_y_acc;

                	//LPUART2->DATA = yaw_z;

                	//LPUART2->DATA=mensaje[0];

                	// if(*datos[i]==*datos[0]) i=0;

                  //  if (dato!='2'){LPUART2->CTRL&=~(1<<23);}
                }
                if ((LPUART2->STAT & (1<<21))==(1<<21))   //Rx full flag
                {
                    dato=LPUART2->DATA;

                }

}

void UART_init(void){
	PCC->PCCn[PCC_PORTA_INDEX]=1<<30;
	                PORTA->PCR[9]=2<<8;  //tx                                                                               //LPUART2 TX
	                PORTA->PCR[8]=2<<8;  //rx                                                                           //LPUART2 RX

	                //LED de respuesta
	                PCC->PCCn[PCC_PORTD_INDEX]=1<<30;
	                PORTD->PCR[16]=(1<<8);    //GPIO
	                PORTD->PCR[15]=(1<<8);    //GPIO
	                PORTD->PCR[0]=(1<<8);     //GPIO
	                //
	                PTD->PDDR=(1<<16)+(1<<15)+1;   //outputs
	                PTD->PDOR=(1<<16)+(1<<15)+1;   //estado inicial off

	                SCG->SIRCDIV=1<<8;                                                                                    //SIRCDIV2: 8 MHz/1

	                PCC->PCCn[PCC_LPUART2_INDEX]=2<<24;                     //SIRCDIV2
	                PCC->PCCn[PCC_LPUART2_INDEX]|=1<<30;
	                LPUART2->BAUD|=52;                                                                                 //BAUD_SRG=CLK_UART/(16*9600)
	                LPUART2->CTRL|=(1<<23)+(1<<21)+(1<<19)+(1<<18);   //TE=RE=1
	                S32_NVIC->ISER[35/32]=(1<<(35%32));
}

int main(void)
{
	SIRC_div();
	LPI2C0_init();
	LPIT_init();
	UART_init();

    while(1){
   // if (dato=='A') PTD->PDOR=(1<<16)+1;  //rojo
  //  if (dato=='B') PTD->PDOR=(1<<15)+1;  //prender rojo
   // if (dato=='C') PTD->PDOR=(1<<16)+(1<<15);  //prender azul
    //if (dato=='E') PTD->PDOR=(1<<16)+(1<<15)+1; //apagar leds

    if (dato=='F') tx = roll_x_acc;
    if (dato=='G') tx = pitch_y_acc;
    if (dato=='H') tx = yaw_z;
    if (dato=='I') tx = vel;





    }
                return 0;
}

