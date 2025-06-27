/**
 * SPI Loopback Test
 * 
 * This program tests if the SPI bus is working by creating a loopback
 * between MOSI and MISO pins. This will verify if basic SPI functionality
 * is working correctly.
 * 
 * Wiring for this test:
 * 1. Connect Arduino pin 11 (MOSI) to pin 12 (MISO) with a jumper wire
 * 2. Keep pin 13 (SCK) disconnected from anything
 *
 * Don't forget to remove the jumper wire after testing!
 */

#include <Arduino.h>
#include <SPI.h>

void setup() {
  // Initialize serial communications
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  
  Serial.println("SPI Loopback Test");
  Serial.println("Make sure you've connected pin 11 (MOSI) to pin 12 (MISO) with a jumper wire!");
  
  // Initialize SPI
  SPI.begin();
  
  // Set SPI parameters
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  
  delay(100);
  
  // Perform the loopback test
  Serial.println("\nStarting SPI loopback test...");
  
  // Test a few values
  byte testValues[] = {0x55, 0xAA, 0xF0, 0x0F, 0x00, 0xFF, 0x12, 0x87};
  bool success = true;
  
  for (byte i = 0; i < sizeof(testValues); i++) {
    byte testValue = testValues[i];
    byte response = SPI.transfer(testValue);
    
    Serial.print("Sent: 0x");
    Serial.print(testValue, HEX);
    Serial.print(", Received: 0x");
    Serial.print(response, HEX);
    
    if (response == testValue) {
      Serial.println(" - MATCH");
    } else {
      Serial.println(" - ERROR");
      success = false;
    }
    
    delay(100);
  }
  
  // Display test result
  Serial.println();
  if (success) {
    Serial.println("SPI LOOPBACK TEST PASSED!");
    Serial.println("Your SPI bus is working correctly.");
  } else {
    Serial.println("SPI LOOPBACK TEST FAILED!");
    Serial.println("Your SPI connection may have issues.");
    Serial.println("Check your wiring and ensure MOSI (pin 11) is connected to MISO (pin 12).");
  }
  
  Serial.println("\nRemember to remove the jumper wire between MOSI and MISO after testing!");
}

void loop() {
  // Nothing to do here
  delay(1000);
}