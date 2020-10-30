#include <Wire.h>
#include <ESP8266WiFi.h>

#define MPU6050_ADDR         0x68
#define MPU6050_SMPLRT_DIV   0x19
#define MPU6050_CONFIG       0x1a
#define MPU6050_GYRO_CONFIG  0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_PWR_MGMT_1   0x6b

#define MPU6050_DATA         0x3b


#define GYRO_LSB_2_DEGSEC  65.5     // [bit/(°/s)]
#define ACC_LSB_2_G        16384.0  // [bit/gravity]
#define RAD_2_DEG          57.29578 // [°/rad]
#define GYRO_OFFSET_NB_MES 3000     //
#define TEMP_LSB_2_DEGREE  340.0    // [bit/celsius]
#define TEMP_LSB_OFFSET    12412.0  //
#define DEFAULT_GYRO_COEFF 0.98


double       AngX,AngY,GyrX,GyrY,Temp;
const double R2G = 180.0/3.141592654;
const double ASF = 16384.0;
const double GSF = 131.0;

float gyroXoffset, gyroYoffset, gyroZoffset;
float temp, accX, accY, accZ, gyroX, gyroY, gyroZ;
float angleAccX, angleAccY;
float angleX, angleY, angleZ;
long preInterval;
float accCoef, gyroCoef;



int16_t Acc_rawX, Acc_rawY, Acc_rawZ, Mpu_rawT, Gyr_rawX, Gyr_rawY, Gyr_rawZ;

void setup() {
  Serial.begin(115200);
  Wire.begin(D1,D2);
  writeMPU(MPU6050_SMPLRT_DIV,  0x00);
  writeMPU(MPU6050_CONFIG,      0x00);
  writeMPU(MPU6050_GYRO_CONFIG, 0x08);
  writeMPU(MPU6050_ACCEL_CONFIG,0x00);
  writeMPU(MPU6050_PWR_MGMT_1,  0x01);

  accCoef = 1.0-DEFAULT_GYRO_COEFF;
  gyroCoef = DEFAULT_GYRO_COEFF;

  calcGyroOffsets();
}

void writeMPU(byte reg, byte data){
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void loop() {
  readMPU();

  Serial.print("X: ");
  Serial.print(angleX);
  Serial.print("  Y: ");
  Serial.print(angleY);
  Serial.print("  Z: ");
  Serial.println(angleZ);

  
}


void calcGyroOffsets(){
  float xyz[3] = {0,0,0};
  int16_t b;
  
  for(int i = 0; i < GYRO_OFFSET_NB_MES; i++){
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom((int)MPU6050_ADDR, 6);

  for(int j=0;j<3;j++){
    b  = Wire.read() << 8;
    b |= Wire.read();
    xyz[j] += ((float)b) / GYRO_LSB_2_DEGSEC;
  }
  }
  gyroXoffset = xyz[0] / GYRO_OFFSET_NB_MES;
  gyroYoffset = xyz[1] / GYRO_OFFSET_NB_MES;
  gyroZoffset = xyz[2] / GYRO_OFFSET_NB_MES;
}


void readMPU(){
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(MPU6050_DATA); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR,14); 

  int16_t rawData[7]; // [ax,ay,az,temp,gx,gy,gz]

  for(int i=0;i<7;i++){
    rawData[i]  = Wire.read() << 8;
    rawData[i] |= Wire.read();
  }

  accX = ((float)rawData[0]) / ACC_LSB_2_G;
  accY = ((float)rawData[1]) / ACC_LSB_2_G;
  accZ = ((float)rawData[2]) / ACC_LSB_2_G;
  temp = (rawData[3] + TEMP_LSB_OFFSET) / TEMP_LSB_2_DEGREE;
  gyroX = ((float)rawData[4]) / GYRO_LSB_2_DEGSEC - gyroXoffset;
  gyroY = ((float)rawData[5]) / GYRO_LSB_2_DEGSEC - gyroYoffset;
  gyroZ = ((float)rawData[6]) / GYRO_LSB_2_DEGSEC - gyroZoffset;
  
  float sgZ = (accZ>=0)-(accZ<0);
  angleAccX = atan2(accY, sgZ*sqrt(accZ*accZ + accX*accX)) * RAD_2_DEG;
  angleAccY = - atan2(accX, sqrt(accZ*accZ + accY*accY)) * RAD_2_DEG;

  unsigned long Tnew = millis();
  float dt = (Tnew - preInterval) * 1e-3;
  preInterval = Tnew;

  angleX = (gyroCoef * (angleX + gyroX*dt)) + (accCoef * angleAccX);
  angleY = (gyroCoef * (angleY + gyroY*dt)) + (accCoef * angleAccY);
  angleZ += gyroZ*dt;
  
}
