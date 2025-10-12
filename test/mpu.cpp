#include <Wire.h>
#include <Arduino.h>

const int MPU_ADDR = 0x68;
const int FILTER_SIZE = 10;  // number of samples for smoothing

float accXBuf[FILTER_SIZE], accYBuf[FILTER_SIZE], accZBuf[FILTER_SIZE],temperature[FILTER_SIZE],gyroX[FILTER_SIZE],gyroY[FILTER_SIZE],gyroZ[FILTER_SIZE];
int bufIndex = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  int16_t AcX = Wire.read() << 8 | Wire.read();//accl
  int16_t AcY = Wire.read() << 8 | Wire.read();
  int16_t AcZ = Wire.read() << 8 | Wire.read();
  int16_t temp = Wire.read() << 8| Wire.read(); //  temperature
  int16_t GyX = Wire.read() << 8 | Wire.read(); //gyro
  int16_t GyY = Wire.read() << 8 | Wire.read();
  int16_t GyZ = Wire.read() << 8 | Wire.read();

  float AccX = AcX / 16384.0;
  float AccY = AcY / 16384.0;
  float AccZ = AcZ / 16384.0;
  float Temp = (temp / 340.0) + 36.53;
  float GyroX = GyX / 131.0;
  float GyroY = GyY / 131.0;
  float GyroZ = GyZ / 131.0;  

  // Store readings in circular buffer
  accXBuf[bufIndex] = AccX;
  accYBuf[bufIndex] = AccY;
  accZBuf[bufIndex] = AccZ;
  temperature[bufIndex] = Temp;
  gyroX[bufIndex] = GyroX;
  gyroY[bufIndex] = GyroY;
  gyroZ[bufIndex] = GyroZ;

  bufIndex = (bufIndex + 1) % FILTER_SIZE;

  // Compute averages
  float avgAccX = 0, avgAccY = 0, avgAccZ = 0, avgTemp = 0, avgGyroX = 0, avgGyroY = 0, avgGyroZ = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    avgAccX += accXBuf[i];
    avgAccY += accYBuf[i];
    avgAccZ += accZBuf[i];
    avgTemp += temperature[i];
    avgGyroX += gyroX[i];
    avgGyroY += gyroY[i];
    avgGyroZ += gyroZ[i];
    
  }
  avgAccX /= FILTER_SIZE;
  avgAccY /= FILTER_SIZE;
  avgAccZ /= FILTER_SIZE;
  avgTemp /= FILTER_SIZE;
  avgGyroX /= FILTER_SIZE;
  avgGyroY /= FILTER_SIZE;
  avgGyroZ /= FILTER_SIZE;


  // Serial.print(avgAccX, 2); Serial.print(", ");
  // Serial.print(avgAccY, 2); Serial.print(", ");
  // Serial.print(avgAccZ, 2); Serial.print(", ");
  // Serial.print(avgTemp, 2); Serial.print(", ");
  Serial.print((avgGyroX<0 && avgGyroX>-1)? 0 : avgGyroX,0); Serial.print(", ");
  Serial.print((avgGyroY<0 && avgGyroY>-1)? 0 : avgGyroY,0); Serial.print(", ");
  Serial.println((avgGyroZ<0 && avgGyroZ>-1)? 0 : avgGyroZ,0);
  // Serial.print(avgGyroY, 0); Serial.print(", ");
  // Serial.println(avgGyroZ, 0);
  

  delay(50);
}
