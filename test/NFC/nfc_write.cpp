#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define PN532_IRQ   4
#define PN532_RESET 5

Adafruit_PN532 nfc( PN532_IRQ, PN532_RESET,&Wire);

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("PN532 not found!");
    while (1);
  }

  nfc.SAMConfig();
  Serial.println("Waiting for an NFC tag to write...");
}

void loop() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.println("Tag detected!");

    // Example: write "Hello" to block 4 (Mifare Classic)
    uint8_t data1[16] = {0};
    uint8_t data2[16] = {0};
    const char* msg1 = "Hello its me nab";
    const char* msg2 = "in Shrestha bakc";
    for (int i = 0; i < 16 && msg1[i]; i++) {
      data1[i] = msg1[i];
       data2[i] = msg2[i];
    }

    // Authenticate block 4 with default key 0xFFFFFFFFFFFF
    uint8_t keya[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya)) {
      if (nfc.mifareclassic_WriteDataBlock(4, data1) && nfc.mifareclassic_WriteDataBlock(5, data2)) {
        Serial.println("Data written successfully!");
      } else {
        Serial.println("Failed to write data block!");
      }
    } else {
      Serial.println("Authentication failed!");
    }

    delay(5000); // wait 5 seconds
  }
}
