#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance

void setup() {
    Serial.begin(9600); // Initialize serial communications
    SPI.begin();        // Init SPI bus
    rfid.PCD_Init();    // Init MFRC522
    Serial.println("Place your RFID tag near the reader...");
}

void loop() {
    // Look for a new RFID card
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        Serial.print("RFID Tag UID:");
        for (byte i = 0; i < rfid.uid.size; i++) {
            Serial.print(rfid.uid.uidByte[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        rfid.PICC_HaltA(); // Halt PICC
    }
}