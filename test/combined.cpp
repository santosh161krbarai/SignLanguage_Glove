#include <Wire.h>
#include <Arduino.h>

const int MPU_ADDR = 0x68;
const int FILTER_SIZE = 10;  // For MPU smoothing

// === Buffers for MPU ===
float accXBuf[FILTER_SIZE], accYBuf[FILTER_SIZE], accZBuf[FILTER_SIZE];
int bufIndex = 0;

// === Flex sensor pins ===
const int FLEX_INDEX_PIN = 14;    
const int FLEX_MIDDLE_PIN = 13;

const int FLEX_SAMPLES = 10;      // Flex moving average
int flexIndexBuf[FLEX_SAMPLES], flexMiddleBuf[FLEX_SAMPLES];
int flexIdx = 0;
int totalIndex = 0, totalMiddle = 0;
int baselineIndex = 0, baselineMiddle = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // === Flex Calibration ===
  long sumIndex = 0, sumMiddle = 0;
  for (int i = 0; i < 100; i++) {
    sumIndex += analogRead(FLEX_INDEX_PIN);
    sumMiddle += analogRead(FLEX_MIDDLE_PIN);
    delay(10);
  }
  baselineIndex = sumIndex / 100;
  baselineMiddle = sumMiddle / 100;

  // Initialize buffers
  for (int i = 0; i < FLEX_SAMPLES; i++) {
    flexIndexBuf[i] = baselineIndex;
    flexMiddleBuf[i] = baselineMiddle;
    totalIndex += baselineIndex;
    totalMiddle += baselineMiddle;
  }
  for (int i = 0; i < FILTER_SIZE; i++) {
    accXBuf[i] = accYBuf[i] = accZBuf[i] = 0;
  }

  Serial.println("Calibration done. Format: indexFlex,middleFlex,accX,accY,accZ");
}

void loop() {
  // === Read MPU6050 accelerometer (using your smoothing code) ===
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  int16_t AcX = Wire.read() << 8 | Wire.read();
  int16_t AcY = Wire.read() << 8 | Wire.read();
  int16_t AcZ = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // Skip temp
  Wire.read(); Wire.read(); Wire.read(); Wire.read(); Wire.read(); Wire.read(); // Skip gyro

  float AccX = AcX / 16384.0;
  float AccY = AcY / 16384.0;
  float AccZ = AcZ / 16384.0;

  accXBuf[bufIndex] = AccX;
  accYBuf[bufIndex] = AccY;
  accZBuf[bufIndex] = AccZ;
  bufIndex = (bufIndex + 1) % FILTER_SIZE;

  float avgX = 0, avgY = 0, avgZ = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    avgX += accXBuf[i];
    avgY += accYBuf[i];
    avgZ += accZBuf[i];
  }
  avgX /= FILTER_SIZE;
  avgY /= FILTER_SIZE;
  avgZ /= FILTER_SIZE;

  // === Flex index moving average ===
  totalIndex -= flexIndexBuf[flexIdx];
  flexIndexBuf[flexIdx] = analogRead(FLEX_INDEX_PIN);
  totalIndex += flexIndexBuf[flexIdx];

  totalMiddle -= flexMiddleBuf[flexIdx];
  flexMiddleBuf[flexIdx] = analogRead(FLEX_MIDDLE_PIN);
  totalMiddle += flexMiddleBuf[flexIdx];

  flexIdx = (flexIdx + 1) % FLEX_SAMPLES;

  float avgIndex = totalIndex / (float)FLEX_SAMPLES;
  float avgMiddle = totalMiddle / (float)FLEX_SAMPLES;

  float angleIndex = map(avgIndex, baselineIndex, 4095, 0, 1000);
  float angleMiddle = map(avgMiddle, baselineMiddle, 4095, 0, 1000);
  angleIndex = angleIndex < 0 ? 0 : angleIndex;
  angleMiddle = angleMiddle < 0 ? 0 : angleMiddle;

  // === Send comma-separated output ===
  Serial.print(angleIndex, 2); Serial.print(",");
  Serial.print(angleMiddle, 2); Serial.print(",");
  Serial.print(avgX, 2); Serial.print(",");
  Serial.print(avgY, 2); Serial.print(",");
  Serial.println(avgZ, 2);

  delay(50);
}
