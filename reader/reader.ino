#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>

// Pin definitions for RFID
#define SS_PIN 10
#define RST_PIN 9
#define LED_RING_PIN 5  // LED ring data input pin

// Number of LEDs in the ring
#define NUM_LEDS 12

// Create MFRC522 instance
MFRC522 rfid(SS_PIN, RST_PIN);

// Create LED ring instance
Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUM_LEDS, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// Define the known tag UIDs
byte knownTag1[4] = {0xCA, 0x6E, 0xEF, 0x3F}; // First tag - green color
byte knownTag2[4] = {0x76, 0x43, 0x42, 0x29}; // Second tag - red color

void setup() {
    Serial.begin(9600);      // Initialize serial communications
    SPI.begin();             // Init SPI bus
    rfid.PCD_Init();         // Init MFRC522
    
    // Initialize LED ring
    ring.begin();
    ring.show();  // Initialize all pixels to 'off'
    ring.setBrightness(50);  // Set brightness (0-255)
    
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
        Serial.println("Green tag detected!");
        setRingColor(0, 255, 0);  // Set ring to green
        delay(2000);              // Display for 2 seconds
        setRingColor(0, 0, 0);    // Turn off
    }
    // Check if it's our second tag
    else if (compareUID(rfid.uid.uidByte, knownTag2)) {
        Serial.println("Red tag detected!");
        setRingColor(255, 0, 0);  // Set ring to red
        delay(2000);              // Display for 2 seconds
        setRingColor(0, 0, 0);    // Turn off
    }
    else {
        Serial.println("Unknown tag");
        // Rainbow effect for unknown tag
        for (int i = 0; i < 3; i++) {
            rainbowCycle(10);
        }
        setRingColor(0, 0, 0);    // Turn off
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