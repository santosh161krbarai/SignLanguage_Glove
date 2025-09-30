#include <Arduino.h>
void setup() {
  Serial.begin(115200);
}
void loop() {
  int rawValue = analogRead(4);
  int y =  map(rawValue,3400,4095,0,180);
  Serial.print("value ");
  Serial.print(rawValue);
    Serial.print("degree ");
  Serial.println(y);
}