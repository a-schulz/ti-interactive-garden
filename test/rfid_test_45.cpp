/**
 * RFID Reader 4 and 5 Specific Test
 * 
 * This program only tests readers 4 and 5 to isolate their functionality.
 */

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Pin definitions for RFID readers 4 and 5
#define SS_PIN4 A0    // Fourth reader SS/SDA
#define RST_PIN4 A1   // Fourth reader RST
#define SS_PIN5 A2    // Fifth reader SS/SDA 
#define RST_PIN5 A3   // Fifth reader RST

// Create MFRC522 instances for readers 4 and 5 only
MFRC522 rfid4(SS_PIN4, RST_PIN4);
MFRC522 rfid5(SS_PIN5, RST_PIN5);

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000); // Wait for serial connection
  
  Serial.println("Initializing SPI...");
  SPI.begin();
  
  // Initialize RFID readers with clear debug info
  Serial.println("Initializing Reader 4...");
  rfid4.PCD_Init();
  
  // Test reader 4
  Serial.print("Reader 4 Version: 0x");
  byte version = rfid4.PCD_ReadRegister(rfid4.VersionReg);
  Serial.println(version, HEX);
  
  Serial.println("Initializing Reader 5...");
  rfid5.PCD_Init();
  
  // Test reader 5
  Serial.print("Reader 5 Version: 0x");
  version = rfid5.PCD_ReadRegister(rfid5.VersionReg);
  Serial.println(version, HEX);
  
  Serial.println("RFID Reader 4 and 5 Test Program");
  Serial.println("Hold a card near reader 4 or 5 to see its ID");
  Serial.println("Expected version is 0x92 or 0x91 - if you see 0x00, there's a connection problem");
}

void printTagId(MFRC522 &rfid, int readerNumber) {
  // Reset the reader - this helps with detection
  rfid.PCD_Reset();
  rfid.PCD_Init();
  
  // Check if tag is present and read
  if (rfid.PICC_IsNewCardPresent()) {
    Serial.print("Card detected on reader #");
    Serial.println(readerNumber);
    
    if (rfid.PICC_ReadCardSerial()) {
      Serial.print("Reader #");
      Serial.print(readerNumber);
      Serial.print(" detected tag with ID: ");
      
      // Print UID in hex format
      for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();
      
      // Halt PICC and stop encryption
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }
  }
}

void loop() {
  // Check readers 4 and 5
  printTagId(rfid4, 4);
  delay(100); // Allow some time before checking the next reader
  printTagId(rfid5, 5);
  delay(100);
}