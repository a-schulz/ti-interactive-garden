#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>

// Pin definitions for RFID readers
#define SS_PIN1 10    // First reader SS/SDA
#define RST_PIN1 9    // First reader RST
#define SS_PIN2 7     // Second reader SS/SDA
#define RST_PIN2 8    // Second reader RST
#define LED_RING_PIN 5  // LED ring data input pin

// Number of LEDs in the ring
#define NUM_LEDS 12

// Create MFRC522 instances
MFRC522 rfid1(SS_PIN1, RST_PIN1);
MFRC522 rfid2(SS_PIN2, RST_PIN2);

// Create LED ring instance
Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUM_LEDS, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// Define the known tag UIDs
byte knownTag1[4] = {0xCA, 0x6E, 0xEF, 0x3F}; // First tag - green color
byte knownTag2[4] = {0x76, 0x43, 0x42, 0x29}; // Second tag - red color

// Variables to track current tag state
bool tagDetected = false;
unsigned long lastDetectionTime = 0;
const unsigned long TAG_TIMEOUT = 500; // ms to wait before considering tag gone

void setup() {
    Serial.begin(9600);      // Initialize serial communications
    SPI.begin();             // Init SPI bus
    
    // Initialize both RFID readers
    rfid1.PCD_Init();
    rfid2.PCD_Init();
    
    // Initialize LED ring
    ring.begin();
    ring.show();  // Initialize all pixels to 'off'
    ring.setBrightness(50);  // Set brightness (0-255)
    
    Serial.println("Place your RFID tag near either reader...");
}

void loop() {
    bool tag1Present = checkReader(rfid1, "Reader 1");
    bool tag2Present = checkReader(rfid2, "Reader 2");
    
    // If no tags detected for a while, turn off the ring
    if (!tag1Present && !tag2Present) {
        Serial.println("No tag detected");
        if (tagDetected && (millis() - lastDetectionTime > TAG_TIMEOUT)) {

            tagDetected = false;
            setRingColor(0, 0, 0); // Turn off
        }
    }
}

// Function to check a specific reader
bool checkReader(MFRC522 &rfid, String readerName) {
    // Look for new cards on this reader
    if (!rfid.PICC_IsNewCardPresent()) {
        return false;
    }
    
    // Select one of the cards
    if (!rfid.PICC_ReadCardSerial()) {
        return false;
    }
    
    // Tag detected, update tracking
    tagDetected = true;
    lastDetectionTime = millis();
    
    // Display UID in Serial Monitor
    Serial.print(readerName + " - RFID Tag UID:");
    for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    // Check if it's our first tag
    if (compareUID(rfid.uid.uidByte, knownTag1)) {
        Serial.println("Green tag detected!");
        setRingColor(0, 255, 0);  // Set ring to green
    }
    // Check if it's our second tag
    else if (compareUID(rfid.uid.uidByte, knownTag2)) {
        Serial.println("Red tag detected!");
        setRingColor(255, 0, 0);  // Set ring to red
    }
    else {
        Serial.println("Unknown tag");
        // Rainbow effect for unknown tag
        for (int i = 0; i < 3; i++) {
            rainbowCycle(10);
        }
        setRingColor(0, 0, 255);  // Blue for unknown
    }
    
    rfid.PICC_HaltA(); // Halt PICC_HaltA
    return true;
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

// Set all pixels in the ring to the same color
void setRingColor(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < NUM_LEDS; i++) {
        ring.setPixelColor(i, ring.Color(r, g, b));
    }
    ring.show();
}

// Rainbow cycle effect for unknown tags
void rainbowCycle(uint8_t wait) {
    for (int j = 0; j < 256; j++) {
        for (int i = 0; i < NUM_LEDS; i++) {
            ring.setPixelColor(i, wheel(((i * 256 / NUM_LEDS) + j) & 255));
        }
        ring.show();
        delay(wait);
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheel(byte wheelPos) {
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85) {
        return ring.Color(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if (wheelPos < 170) {
        wheelPos -= 85;
        return ring.Color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return ring.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}