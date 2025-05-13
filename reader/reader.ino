#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>

// Pin definitions for RFID readers
#define SS_PIN1 10    // First reader SS/SDA
#define RST_PIN1 9    // First reader RST
#define SS_PIN2 7     // Second reader SS/SDA
#define RST_PIN2 8    // Second reader RST
#define LED_RING_PIN1 5  // First LED ring data input pin
#define LED_RING_PIN2 6  // Second LED ring data input pin

// Number of LEDs in the rings
#define NUM_LEDS 12

// Create MFRC522 instances
MFRC522 rfid1(SS_PIN1, RST_PIN1);
MFRC522 rfid2(SS_PIN2, RST_PIN2);

// Create LED ring instances
Adafruit_NeoPixel ring1 = Adafruit_NeoPixel(NUM_LEDS, LED_RING_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring2 = Adafruit_NeoPixel(NUM_LEDS, LED_RING_PIN2, NEO_GRB + NEO_KHZ800);

// Define the known tag UIDs
byte knownTag1[4] = {0xCA, 0x6E, 0xEF, 0x3F}; // First tag - green color
byte knownTag2[4] = {0x76, 0x43, 0x42, 0x29}; // Second tag - red color

// Variables to track current tag state
bool tag1Detected = false;
bool tag2Detected = false;
unsigned long lastDetectionTime1 = 0;
unsigned long lastDetectionTime2 = 0;
const unsigned long TAG_TIMEOUT = 500; // ms to wait before considering tag gone

void setup() {
    Serial.begin(9600);      // Initialize serial communications
    SPI.begin();             // Init SPI bus
    
    // Initialize both RFID readers
    rfid1.PCD_Init();
    rfid2.PCD_Init();
    
    // Initialize LED rings
    ring1.begin();
    ring1.show();  // Initialize all pixels to 'off'
    ring1.setBrightness(50);  // Set brightness (0-255)
    
    ring2.begin();
    ring2.show();  // Initialize all pixels to 'off'
    ring2.setBrightness(50);  // Set brightness (0-255)
    
    Serial.println("Place your RFID tag near either reader...");
}

void loop() {
    // Check readers and update corresponding LED rings
    bool reader1Active = checkReader(rfid1, "Reader 1", 1);
    bool reader2Active = checkReader(rfid2, "Reader 2", 2);
    
    // Check timeout for reader 1
    if (!reader1Active) {
        if (tag1Detected && (millis() - lastDetectionTime1 > TAG_TIMEOUT)) {
            Serial.println("Reader 1 - No tag detected");
            tag1Detected = false;
            setRingColor(ring1, 0, 0, 0); // Turn off
        }
    }
    
    // Check timeout for reader 2
    if (!reader2Active) {
        if (tag2Detected && (millis() - lastDetectionTime2 > TAG_TIMEOUT)) {
            Serial.println("Reader 2 - No tag detected");
            tag2Detected = false;
            setRingColor(ring2, 0, 0, 0); // Turn off
        }
    }
}

// Function to check a specific reader
bool checkReader(MFRC522 &rfid, String readerName, int readerNum) {
    // Look for new cards on this reader
    if (!rfid.PICC_IsNewCardPresent()) {
        return false;
    }
    
    // Select one of the cards
    if (!rfid.PICC_ReadCardSerial()) {
        return false;
    }
    
    // Tag detected, update tracking based on which reader
    if (readerNum == 1) {
        tag1Detected = true;
        lastDetectionTime1 = millis();
    } else {
        tag2Detected = true;
        lastDetectionTime2 = millis();
    }
    
    // Display UID in Serial Monitor
    Serial.print(readerName + " - RFID Tag UID:");
    for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    // Select which ring to use based on the reader number
    Adafruit_NeoPixel& currentRing = (readerNum == 1) ? ring1 : ring2;
    
    // Check if it's our first tag
    if (compareUID(rfid.uid.uidByte, knownTag1)) {
        Serial.println(readerName + " - Green tag detected!");
        setRingColor(currentRing, 0, 255, 0);  // Set ring to green
    }
    // Check if it's our second tag
    else if (compareUID(rfid.uid.uidByte, knownTag2)) {
        Serial.println(readerName + " - Red tag detected!");
        setRingColor(currentRing, 255, 0, 0);  // Set ring to red
    }
    else {
        Serial.println(readerName + " - Unknown tag");
        // Rainbow effect for unknown tag
        for (int i = 0; i < 3; i++) {
            rainbowCycle(currentRing, 10);
        }
        setRingColor(currentRing, 0, 0, 255);  // Blue for unknown
    }
    
    rfid.PICC_HaltA(); // Halt PICC
    rfid.PCD_StopCrypto1(); // Stop encryption on PCD
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

// Set all pixels in the specified ring to the same color
void setRingColor(Adafruit_NeoPixel &ring, uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < NUM_LEDS; i++) {
        ring.setPixelColor(i, ring.Color(r, g, b));
    }
    ring.show();
}

// Rainbow cycle effect for unknown tags on the specified ring
void rainbowCycle(Adafruit_NeoPixel &ring, uint8_t wait) {
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
        return ring1.Color(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if (wheelPos < 170) {
        wheelPos -= 85;
        return ring1.Color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return ring1.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}