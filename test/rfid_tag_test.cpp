/**
 * RFID Tag Testing Program
 * 
 * This program tests all connected RFID readers and provides detailed debug information
 * about detected tags. Use this to identify new tags before adding them to the main program.
 */

 #include <Arduino.h>
 #include <SPI.h>
 #include <MFRC522.h>
 
 // Pin definitions for RFID readers
 #define SS_PIN1 10    // First reader SS/SDA
 #define RST_PIN1 9    // First reader RST
 #define SS_PIN2 7     // Second reader SS/SDA
 #define RST_PIN2 8    // Second reader RST
 #define SS_PIN3 4     // Third reader SS/SDA
 #define RST_PIN3 3    // Third reader RST
 
 // Create MFRC522 instances
 MFRC522 rfid1(SS_PIN1, RST_PIN1);
 MFRC522 rfid2(SS_PIN2, RST_PIN2);
 MFRC522 rfid3(SS_PIN3, RST_PIN3);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  
  // Initialize RFID readers
  rfid1.PCD_Init();
  rfid2.PCD_Init();
  rfid3.PCD_Init();
  
  Serial.println("RFID Tag Test Program");
  Serial.println("Hold a card near any reader to see its ID");
}

void printTagId(MFRC522 &rfid, int readerNumber) {
  // Check if tag is present and read
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
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

void loop() {
  // Check all readers
  printTagId(rfid1, 1);
  printTagId(rfid2, 2);
  printTagId(rfid3, 3);
  
  delay(100); // Small delay to prevent too frequent polling
}