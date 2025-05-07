#include <SPI.h>
#include <MFRC522.h>

// Pin definitions
#define SS_PIN 10
#define RST_PIN 9
#define GREEN_LED_PIN 3  // Green LED connected to pin 3
#define RED_LED_PIN 4    // Red LED connected to pin 4

// Create MFRC522 instance
MFRC522 rfid(SS_PIN, RST_PIN);

// Define the known tag UIDs
byte knownTag1[4] = {0xCA, 0x6E, 0xEF, 0x3F}; // First tag - green LED
byte knownTag2[4] = {0x76, 0x43, 0x42, 0x29}; // Second tag - red LED

void setup() {
    Serial.begin(9600);      // Initialize serial communications
    SPI.begin();             // Init SPI bus
    rfid.PCD_Init();         // Init MFRC522
    
    // Initialize LED pins as outputs
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    
    // Ensure LEDs are off at startup
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
    
    Serial.println("Place your RFID tag near the reader...");
}

void loop() {
    // Look for new cards
    if (!rfid.PICC_IsNewCardPresent()) {
        return;
    }
    
    // Select one of the cards
    if (!rfid.PICC_ReadCardSerial()) {
        return;
    }
    
    // Display UID in Serial Monitor
    Serial.print("RFID Tag UID:");
    for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    // Check if it's our first tag
    if (compareUID(rfid.uid.uidByte, knownTag1)) {
        Serial.println("Green LED tag detected!");
        digitalWrite(GREEN_LED_PIN, HIGH); // Turn on green LED
        digitalWrite(RED_LED_PIN, LOW);    // Make sure red LED is off
        delay(2000);                      // Keep LED on for 2 seconds
        digitalWrite(GREEN_LED_PIN, LOW);  // Turn off green LED
    }
    // Check if it's our second tag
    else if (compareUID(rfid.uid.uidByte, knownTag2)) {
        Serial.println("Red LED tag detected!");
        digitalWrite(RED_LED_PIN, HIGH);   // Turn on red LED
        digitalWrite(GREEN_LED_PIN, LOW);  // Make sure green LED is off
        delay(2000);                      // Keep LED on for 2 seconds
        digitalWrite(RED_LED_PIN, LOW);    // Turn off red LED
    }
    else {
        Serial.println("Unknown tag");
        // Blink both LEDs to indicate unknown tag
        for (int i = 0; i < 3; i++) {
            digitalWrite(GREEN_LED_PIN, HIGH);
            digitalWrite(RED_LED_PIN, HIGH);
            delay(200);
            digitalWrite(GREEN_LED_PIN, LOW);
            digitalWrite(RED_LED_PIN, LOW);
            delay(200);
        }
    }
    
    rfid.PICC_HaltA(); // Halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption on PCD
}

// Helper function to compare two UIDs
bool compareUID(byte* uid1, byte* uid2) {
    for (byte i = 0; i < 4; i++) {
        if (uid1[i] != uid2[i]) {
            return false;
        }
    }
    return true;
}