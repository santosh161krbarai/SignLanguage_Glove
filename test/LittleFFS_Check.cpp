#include <LittleFS.h>

void setup() {
  Serial.begin(115200);
  Serial.println("\nMounting LittleFS...");

  if (!LittleFS.begin(true)) {
    Serial.println("❌ Mount failed even after format!");
    return;
  }

  Serial.println("✅ LittleFS mounted successfully!");

  File root = LittleFS.open("/");
  if (!root) {
    Serial.println("❌ Failed to open root directory!");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    Serial.printf("File: %s (%d bytes)\n", file.name(), file.size());
    file = root.openNextFile();
  }
}
void loop() {
  // Nothing to do here
}