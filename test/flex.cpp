// #include <Arduino.h>

// const int FLEX_PIN = 14;       // Analog pin where the flex sensor is connected
// const int NUM_READINGS = 10;   // Number of readings for moving average

// int readings[NUM_READINGS];    // Array to store readings
// int readIndex = 0;             // Index of the current reading
// int total = 0;                 // Total sum of readings
// int average = 0;               // Calculated average
// int intt=0;

// void setup() {
//   Serial.begin(115200);
//   for(int i=0;i<100;i++){
//   intt=analogRead(4)+intt;
// }
//   intt=intt/100;
//   delay(100);
//   Serial.println(intt + " done calibrating");

//   // Initialize readings array to 0
//   for (int i = 0; i < NUM_READINGS; i++) {
//     readings[i] = 0;
//   }
// }

// void loop() {
//   Serial.println(analogRead(FLEX_PIN));
//   // Subtract the oldest reading from total
//   total -= readings[readIndex];

//   // Read new sensor value
//   readings[readIndex] = analogRead(FLEX_PIN);

//   // Add the new reading to the total
//   total += readings[readIndex];

//   // Advance to the next index
//   readIndex = (readIndex + 1) % NUM_READINGS;

//   // Calculate the average
//   average = total / NUM_READINGS;
// float angle = map(average, intt, 4095, 0, 180); // example
// angle=angle < 0 ? 0 : angle;

//   // Print the smoothed value
//   Serial.print("index,");
//   Serial.println(angle, 2);  // 2 decimal places

//   delay(50); // Adjust delay as needed
// }
#include <Arduino.h>

const int FLEX_INDEX_PIN = 14;   
const int FLEX_MIDDLE_PIN = 13;
const int FLEX_SAMPLES = 10;      // Flex moving average
int flexIndexBuf[FLEX_SAMPLES], flexMiddleBuf[FLEX_SAMPLES];
int flexIdx = 0;
int totalIndex = 0, totalMiddle = 0;
int baselineIndex = 0, baselineMiddle = 0;

void setup() {
  Serial.begin(115200);
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
  Serial.println("Calibration done. Format: indexFlex,middleFlex");
}
void loop(){
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

  Serial.print(angleIndex, 2); Serial.print(",");
  Serial.println(angleMiddle, 2); 
  delay(50);
}