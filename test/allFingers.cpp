#include <Wire.h>
#include <Arduino.h>

// ===========================
// FLEX SENSOR CONFIGURATION
// ===========================
// Assign your sensor pins here
// (Use ADC1 pins for stability)

const int PIN_INDEX_UP      = 36; 
const int PIN_INDEX_LOW     = 39;

const int PIN_MIDDLE_UP     = 13;
const int PIN_MIDDLE_LOW    = 14;

const int PIN_RING_UP       = 32;
const int PIN_RING_LOW      = 33;

const int PIN_THUMB         = 34;   // One sensor only
const int PIN_PINKY         = 35;   // One sensor only

// ===========================
// MOVING AVERAGE SETTINGS
// ===========================
const int FLEX_SAMPLES = 10;

// Structure for each flex sensor
struct FlexSensor {
  int pin;
  int buffer[FLEX_SAMPLES];
  int index = 0;
  int total = 0;
  int baseline = 0;
};

// Create objects for each sensor
FlexSensor indexUp, indexLow;
FlexSensor middleUp, middleLow;
FlexSensor ringUp, ringLow;
FlexSensor thumbFlex, pinkyFlex;


// ===========================
// FLEX SENSOR FUNCTIONS
// ===========================

// --- Initialize one sensor (calibration + buffer fill) ---
void initFlex(FlexSensor &fs, int pin) {
  fs.pin = pin;

  long sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  fs.baseline = sum / 100;

  fs.total = 0;
  for (int i = 0; i < FLEX_SAMPLES; i++) {
    fs.buffer[i] = fs.baseline;
    fs.total += fs.baseline;
  }
}

// --- Read and smooth one sensor ---
float readFlex(FlexSensor &fs) {
  fs.total -= fs.buffer[fs.index];
  fs.buffer[fs.index] = analogRead(fs.pin);
  fs.total += fs.buffer[fs.index];

  fs.index = (fs.index + 1) % FLEX_SAMPLES;

  float avg = fs.total / (float)FLEX_SAMPLES;

  // Convert to bend angle (0–1000 scale)
  float angle = map(avg, fs.baseline, 4095, 0, 1000);
  if (angle < 0) angle = 0;

  return angle;
}


// ===========================
// SETUP
// ===========================
void setup() {
  Serial.begin(115200);

  // Initialize all flex sensors
  initFlex(indexUp,    PIN_INDEX_UP);
  initFlex(indexLow,   PIN_INDEX_LOW);

  initFlex(middleUp,   PIN_MIDDLE_UP);
  initFlex(middleLow,  PIN_MIDDLE_LOW);

  initFlex(ringUp,     PIN_RING_UP);
  initFlex(ringLow,    PIN_RING_LOW);

  initFlex(thumbFlex,  PIN_THUMB);
  initFlex(pinkyFlex,  PIN_PINKY);

  Serial.println("Calibration complete.");
  Serial.println("Output: idxUp,idxLow,midUp,midLow,ringUp,ringLow,thumb,pinky");
}


// ===========================
// MAIN LOOP
// ===========================
void loop() {

  float idxUp   = readFlex(indexUp);
  float idxLow  = readFlex(indexLow);

  float midUp   = readFlex(middleUp);
  float midLow  = readFlex(middleLow);

  float RingUp  = readFlex(ringUp);
  float RingLow = readFlex(ringLow);

  float thumb   = readFlex(thumbFlex);
  float pinky   = readFlex(pinkyFlex);

  // Output CSV
  Serial.print(idxUp, 2);  Serial.print(",");
  Serial.print(idxLow, 2); Serial.print(",");

  Serial.print(midUp, 2);  Serial.print(",");
  Serial.print(midLow, 2); Serial.print(",");

  Serial.print(RingUp, 2); Serial.print(",");
  Serial.print(RingLow, 2); Serial.print(",");

  Serial.print(thumb, 2);  Serial.print(",");
  Serial.println(pinky, 2);

  delay(40);
}
