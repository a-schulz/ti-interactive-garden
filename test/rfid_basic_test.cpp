#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Test one reader at a time - change to test different readers
#define TEST_READER 4  // Options: 1, 2, 3, 4, 5

// Pin definitions - uses the standard configuration from BoardConfig.h
// Reader 1
#define SS_PIN1 10
#define RST_PIN1 9
// Reader 2
#define SS_PIN2 7
#define RST_PIN2 8
// Reader 3
#define SS_PIN3 4
#define RST_PIN3 3
// Reader 4
#define SS_PIN4 A0
#define RST_PIN4 A1
// Reader 5
#define SS_PIN5 A2
#define RST_PIN5 A3

// Create the RFID reader instance based on the selected reader
#if TEST_READER == 1
  MFRC522 mfrc522(SS_PIN1, RST_PIN1);
  #define SS_PIN_USED SS_PIN1
  #define RST_PIN_USED RST_PIN1
#elif TEST_READER == 2
  MFRC522 mfrc522(SS_PIN2, RST_PIN2);
  #define SS_PIN_USED SS_PIN2
  #define RST_PIN_USED RST_PIN2
#elif TEST_READER == 3
  MFRC522 mfrc522(SS_PIN3, RST_PIN3);
  #define SS_PIN_USED SS_PIN3
  #define RST_PIN_USED RST_PIN3
#elif TEST_READER == 4
  MFRC522 mfrc522(SS_PIN4, RST_PIN4);
  #define SS_PIN_USED SS_PIN4
  #define RST_PIN_USED RST_PIN4
#elif TEST_READER == 5
  MFRC522 mfrc522(SS_PIN5, RST_PIN5);
  #define SS_PIN_USED SS_PIN5
  #define RST_PIN_USED RST_PIN5
#else
  #error "Invalid TEST_READER value, must be 1-5"
#endif

// For testing digital pin output
void testPin(int pin) {
  pinMode(pin, OUTPUT);
  
  // Blink the pin a few times
  for (int i = 0; i < 3; i++) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
  }
  
  // Return the pin to INPUT state to not interfere with device
  pinMode(pin, INPUT);
}

void printDetailedInfo(MFRC522 &rfid, const char* message) {
  Serial.println(message);
  
  byte v = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.print("Firmware Version: 0x");
  Serial.println(v, HEX);
  
  if (v == 0x00 || v == 0xFF) {
    Serial.println("WARNING: Version 0x00 or 0xFF often indicates connection problems!");
  }
  
  Serial.print("Command Register: 0x");
  Serial.println(rfid.PCD_ReadRegister(rfid.CommandReg), HEX);
  
  Serial.print("Status1 Register: 0x");
  Serial.println(rfid.PCD_ReadRegister(rfid.Status1Reg), HEX);
  
  Serial.print("Status2 Register: 0x");
  Serial.println(rfid.PCD_ReadRegister(rfid.Status2Reg), HEX);
}

void setup() {
  // Initialize serial communications
  Serial.begin(9600);
  while (!Serial && millis() < 3000); // Wait for serial but timeout
  
  Serial.println("RFID Basic Diagnostic Test");
  Serial.print("Testing Reader #");
  Serial.println(TEST_READER);
  
  Serial.print("Using SS Pin: ");
  Serial.print(SS_PIN_USED);
  if (SS_PIN_USED >= A0) {
    Serial.print(" (Analog pin A");
    Serial.print(SS_PIN_USED - A0);
    Serial.println(")");
  } else {
    Serial.println(" (Digital)");
  }
  
  Serial.print("Using RST Pin: ");
  Serial.print(RST_PIN_USED);
  if (RST_PIN_USED >= A0) {
    Serial.print(" (Analog pin A");
    Serial.print(RST_PIN_USED - A0);
    Serial.println(")");
  } else {
    Serial.println(" (Digital)");
  }
  
  // Explicitly configure analog pins as digital outputs first
  if (SS_PIN_USED >= A0) {
    pinMode(SS_PIN_USED, OUTPUT);
    digitalWrite(SS_PIN_USED, HIGH); // Start with SS high (inactive)
  }
  if (RST_PIN_USED >= A0) {
    pinMode(RST_PIN_USED, OUTPUT);
    digitalWrite(RST_PIN_USED, HIGH); // Start with RST high
  }
  
  // Test if pins are working electrically
  Serial.println("Testing pins electrically (watch for LED blinking if connected)...");
  Serial.println("Testing SS pin...");
  testPin(SS_PIN_USED);
  Serial.println("Testing RST pin...");
  testPin(RST_PIN_USED);
  
  // Initialize SPI bus with more careful setup
  Serial.println("Initializing SPI bus...");
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  
  // Try a much slower SPI speed for more reliability
  Serial.println("Setting SPI to slower speed...");
  SPI.setClockDivider(SPI_CLOCK_DIV16); // Even slower than before
  
  // Reset the reader and init it
  Serial.println("Resetting and initializing reader...");
  
  // Manual reset sequence
  Serial.println("Performing manual reset sequence...");
  digitalWrite(RST_PIN_USED, HIGH);
  delay(50);
  digitalWrite(RST_PIN_USED, LOW);
  delay(100);
  digitalWrite(RST_PIN_USED, HIGH);
  delay(50);
  
  mfrc522.PCD_Init();
  delay(100); // Give it more time to stabilize
  
  // Get reader details
  printDetailedInfo(mfrc522, "Initial reader state:");
  
  // Try a few approaches to wake up the reader
  Serial.println("Attempting additional initialization steps...");
  
  // Perform a full power-cycle style reset
  Serial.println("Trying hardware-style reset sequence...");
  digitalWrite(RST_PIN_USED, LOW);
  delay(100);
  digitalWrite(RST_PIN_USED, HIGH);
  delay(100);
  mfrc522.PCD_Init();
  delay(100);
  
  // Set the antenna gain to maximum
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  
  // Get reader details again after additional setup
  printDetailedInfo(mfrc522, "After additional initialization:");
  
  Serial.println("\nTest complete.");
  Serial.println("If version shows 0x91 or 0x92, connection is good.");
  Serial.println("If version shows 0x00 or 0xFF, check wiring issues.");
  Serial.println("If version shows 0xEE, there may be a problem with the SPI communication.");
  Serial.println("\nNow entering card detection mode...");
}

void loop() {
  // Check if there's a card present
  if (mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("Card detected!");
    
    // Try to read serial
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("Card UID: ");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();
      
      // Halt PICC and stop crypto
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  }
  
  // Every 3 seconds, print the version to monitor if the reader stays connected
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 3000) {
    byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
    Serial.print("Current version: 0x");
    Serial.println(v, HEX);
    lastCheck = millis();
    
    // Try a reset if version is invalid (optional, uncomment if needed)
    // if (v == 0x00 || v == 0xFF) {
    //   Serial.println("Attempting reset...");
    //   mfrc522.PCD_Reset();
    //   delay(50);
    //   mfrc522.PCD_Init();
    // }
  }
  
  delay(50);  // Small delay
}