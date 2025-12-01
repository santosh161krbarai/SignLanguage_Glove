#include <Wire.h>
#include <Adafruit_PN532.h>

// Pins (not really used, just for constructor)
#define PN532_IRQ   4
#define PN532_RESET 5

// Create PN532 instance
Adafruit_PN532 nfc( PN532_IRQ, PN532_RESET,&Wire);


void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL

  Serial.println("Starting PN532...");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN532! Check wiring + I2C DIP switches.");
    while (1);
  }

  Serial.print("Found PN532! Firmware version: ");
  Serial.print((versiondata >> 24) & 0xFF, HEX);
  Serial.print(".");
  Serial.println((versiondata >> 16) & 0xFF, HEX);

  nfc.SAMConfig(); // configure board to read cards

  Serial.println("Waiting for an NFC tag...");
}

void loop() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.print("Tag detected! UID: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }
    Serial.println("\n--------------------");
    delay(1000);
  }
}
