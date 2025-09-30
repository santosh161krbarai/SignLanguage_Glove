#include <Wire.h>
#include <Arduino.h>

const int MPU_ADDR = 0x68;
const int FILTER_SIZE = 10;  // number of samples for smoothing

float accXBuf[FILTER_SIZE], accYBuf[FILTER_SIZE], accZBuf[FILTER_SIZE];
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

  int16_t AcX = Wire.read() << 8 | Wire.read();
  int16_t AcY = Wire.read() << 8 | Wire.read();
  int16_t AcZ = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // Skip temperature
  Wire.read(); Wire.read(); Wire.read(); Wire.read(); Wire.read(); Wire.read(); // Skip gyro

  float AccX = AcX / 16384.0;
  float AccY = AcY / 16384.0;
  float AccZ = AcZ / 16384.0;

  // Store readings in circular buffer
  accXBuf[bufIndex] = AccX;
  accYBuf[bufIndex] = AccY;
  accZBuf[bufIndex] = AccZ;

  bufIndex = (bufIndex + 1) % FILTER_SIZE;

  // Compute averages
  float avgX = 0, avgY = 0, avgZ = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    avgX += accXBuf[i];
    avgY += accYBuf[i];
    avgZ += accZBuf[i];
  }
  avgX /= FILTER_SIZE;
  avgY /= FILTER_SIZE;
  avgZ /= FILTER_SIZE;

  Serial.print(avgX, 2); Serial.print(", ");
  Serial.print(avgY, 2); Serial.print(", ");
  Serial.println(avgZ, 2);

  delay(50);
}
