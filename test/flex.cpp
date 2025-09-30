#include <Arduino.h>

const int FLEX_PIN = 4;       // Analog pin where the flex sensor is connected
const int NUM_READINGS = 10;   // Number of readings for moving average

int readings[NUM_READINGS];    // Array to store readings
int readIndex = 0;             // Index of the current reading
int total = 0;                 // Total sum of readings
int average = 0;               // Calculated average
int intt=0;

void setup() {
  Serial.begin(115200);
  for(int i=0;i<100;i++){
  intt=analogRead(4)+intt;
}
  intt=intt/100;
  delay(100);
  Serial.println("done calibrating");

  // Initialize readings array to 0
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = 0;
  }
}

void loop() {
  Serial.println(analogRead(FLEX_PIN));
  // Subtract the oldest reading from total
  total -= readings[readIndex];

  // Read new sensor value
  readings[readIndex] = analogRead(FLEX_PIN);

  // Add the new reading to the total
  total += readings[readIndex];

  // Advance to the next index
  readIndex = (readIndex + 1) % NUM_READINGS;

  // Calculate the average
  average = total / NUM_READINGS;
float angle = map(average, intt, 4095, 0, 180); // example
angle=angle < 0 ? 0 : angle;

  // Print the smoothed value
  Serial.print("index,");
  Serial.println(angle, 2);  // 2 decimal places

  delay(50); // Adjust delay as needed
}

