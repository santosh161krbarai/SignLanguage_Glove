#include <Arduino.h>

float filteredValue = 0.0;
float fastAlpha = 0.7;         // Fast rise
float slowAlpha = 0.1;         // Smooth fall
int hysteresis = 30;
unsigned long decayDelay = 80;
unsigned long lastStrongValueTime = 0;

const float rawMin = 3510.0;
const float rawMax = 4095.0;
const float degMin = 0.0;
const float degMax = 180.0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int rawValue = analogRead(4);

  bool goingUp = rawValue > filteredValue + hysteresis;
  bool goingDown = rawValue < filteredValue - hysteresis;
  unsigned long now = millis();

  float alpha = slowAlpha;

  if (goingUp) {
    alpha = fastAlpha;
    lastStrongValueTime = now;
  } else if (goingDown) {
    if (now - lastStrongValueTime > decayDelay) {
      alpha = slowAlpha;
    } else {
      alpha = 0.0;
    }
  } else {
    alpha = 0.0;
  }

  filteredValue = alpha * rawValue + (1 - alpha) * filteredValue;

  // Map filteredValue to degrees (0-90)
  float degree = (filteredValue - rawMin) * (degMax - degMin) / (rawMax - rawMin) + degMin;

  // Clamp degree between 0 and 90
  if (degree < degMin) degree = degMin;
  if (degree > degMax) degree = degMax;

  // Print degrees with 2 decimals
    Serial.print("value: ");
  Serial.print(filteredValue);
  Serial.print("  Degree: ");
  Serial.println(degree, 2);

  delay(1);
}
