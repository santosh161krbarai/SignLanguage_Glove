#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define PN532_IRQ   4
#define PN532_RESET 5

Adafruit_PN532 nfc( PN532_IRQ, PN532_RESET, &Wire);

// Default Mifare key for authentication (if needed)
uint8_t keya[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

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
  Serial.println("Waiting for NFC tag...");
}

void loop() {
  uint8_t uid[7];      // Buffer to store UID
  uint8_t uidLength;   // Length of UID

  // Check if a tag is present
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.print("Tag detected! UID: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // OPTIONAL: Read a specific block (e.g., block 4)
    uint8_t blockNumber = 4;
    uint8_t readBuffer1[16];
    uint8_t readBuffer2[16];
    if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, 0, keya)) {
      if (nfc.mifareclassic_ReadDataBlock(blockNumber, readBuffer1) && nfc.mifareclassic_ReadDataBlock(5, readBuffer2)) {
        Serial.print("Block "); Serial.print(blockNumber); Serial.println(" data: ");
        for (int i = 0; i < 16; i++) {
          if (readBuffer1[i] < 32 || readBuffer1[i] > 126) Serial.print("."); // non-printable chars
          else Serial.print((char)readBuffer1[i]);
        }
        Serial.println();
        Serial.print("Block "); Serial.print(5); Serial.println(" data: ");
        for (int i = 0; i < 16; i++) {
          if (readBuffer2[i] < 32 || readBuffer2[i] > 126) Serial.print("."); // non-printable chars
          else Serial.print((char)readBuffer2[i]);
        }
        Serial.println();
      } else {
        Serial.println("Failed to read data block!");
      }
    } else {
      Serial.println("Authentication failed for block read!");
    }

    Serial.println("-----------------------------");
    delay(2000); // Wait before scanning next tag
  }
}
