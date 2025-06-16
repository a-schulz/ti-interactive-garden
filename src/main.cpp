#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>

// Pin definitions for RFID readers
#define SS_PIN1 10    // First reader SS/SDA
#define RST_PIN1 9    // First reader RST
#define SS_PIN2 7     // Second reader SS/SDA
#define RST_PIN2 8    // Second reader RST
#define SS_PIN3 4     // Third reader SS/SDA
#define RST_PIN3 3    // Third reader RST
#define LED_RING_PIN 5  // LED data input pin

// Number of LEDs in the rings
#define NUM_LEDS_PER_RING 12
#define NUM_RINGS 8
#define TOTAL_LEDS (NUM_LEDS_PER_RING * NUM_RINGS)

// Create MFRC522 instances
MFRC522 rfid1(SS_PIN1, RST_PIN1);
MFRC522 rfid2(SS_PIN2, RST_PIN2);
MFRC522 rfid3(SS_PIN3, RST_PIN3);

// Create single LED strip instance for all rings
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, LED_RING_PIN, NEO_GRB + NEO_KHZ800);


// Define the known tag UIDs
byte knownTag1[4] = {0xCA, 0x6E, 0xEF, 0x3F}; // First tag - green color
byte knownTag2[4] = {0x76, 0x43, 0x42, 0x29}; // Second tag - red color

// Variables to track current tag state
bool tag1Active = false;
bool tag2Active = false;
bool tag3Active = false;
unsigned long lastSuccessfulRead1 = 0;
unsigned long lastSuccessfulRead2 = 0;
unsigned long lastSuccessfulRead3 = 0;
const unsigned long READ_INTERVAL = 100;   // Try to read cards every 100ms
const unsigned long TAG_MISSING_THRESHOLD = 500; // Consider tag gone after 500ms of no reads

// Store the last detected tag colors
byte lastTag1Red = 0, lastTag1Green = 0, lastTag1Blue = 0;
byte lastTag2Red = 0, lastTag2Green = 0, lastTag2Blue = 0;
byte lastTag3Red = 0, lastTag3Green = 0, lastTag3Blue = 0;

// Function declarations
bool checkAndUpdateReader(MFRC522 &rfid, String readerName, int readerNum);
bool compareUID(byte* uid1, byte* uid2);
void setRingColor(int ringNumber, uint8_t r, uint8_t g, uint8_t b);
void rainbowCycle(int ringNumber, uint8_t wait);
uint32_t wheel(byte wheelPos);

void setup() {
    Serial.begin(9600);      // Initialize serial communications
    SPI.begin();             // Init SPI bus
    
    // Initialize RFID readers
    rfid1.PCD_Init();
    rfid2.PCD_Init();
    rfid3.PCD_Init();
    
    // Initialize LED
    strip.begin();
    strip.show();  // Initialize all pixels to 'off'
    strip.setBrightness(10);  // Set brightness (0-255)
    
    Serial.println("Place your RFID tag near any reader...");
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Check readers at regular intervals - prevents too frequent reads
    static unsigned long lastReadAttempt1 = 0;
    static unsigned long lastReadAttempt2 = 0;
    static unsigned long lastReadAttempt3 = 0;
    
    // Try reader 1
    if (currentMillis - lastReadAttempt1 >= READ_INTERVAL) {
        lastReadAttempt1 = currentMillis;
        
        if (checkAndUpdateReader(rfid1, "Reader 1", 1)) {
            lastSuccessfulRead1 = currentMillis;
            if (!tag1Active) {
                tag1Active = true;
                Serial.println("Reader 1 - Tag detected");
            }
        }
    }
    
    // Try reader 2
    if (currentMillis - lastReadAttempt2 >= READ_INTERVAL) {
        lastReadAttempt2 = currentMillis;
        
        if (checkAndUpdateReader(rfid2, "Reader 2", 2)) {
            lastSuccessfulRead2 = currentMillis;
            if (!tag2Active) {
                tag2Active = true;
                Serial.println("Reader 2 - Tag detected");
            }
        }
    }
    
    // Try reader 3
    if (currentMillis - lastReadAttempt3 >= READ_INTERVAL) {
        lastReadAttempt3 = currentMillis;
        
        if (checkAndUpdateReader(rfid3, "Reader 3", 3)) {
            lastSuccessfulRead3 = currentMillis;
            if (!tag3Active) {
                tag3Active = true;
                Serial.println("Reader 3 - Tag detected");
            }
        }
    }
    
    // Check if tags are still present
    if (tag1Active && (currentMillis - lastSuccessfulRead1 > TAG_MISSING_THRESHOLD)) {
        tag1Active = false;
        Serial.println("Reader 1 - Tag removed");
        setRingColor(1, 0, 0, 0); // Turn off
    }
    
    if (tag2Active && (currentMillis - lastSuccessfulRead2 > TAG_MISSING_THRESHOLD)) {
        tag2Active = false;
        Serial.println("Reader 2 - Tag removed");
        setRingColor(2, 0, 0, 0); // Turn off
    }
    
    if (tag3Active && (currentMillis - lastSuccessfulRead3 > TAG_MISSING_THRESHOLD)) {
        tag3Active = false;
        Serial.println("Reader 3 - Tag removed");
        setRingColor(3, 0, 0, 0); // Turn off
    }
}

// Function to check the reader and update the corresponding ring if a tag is detected
bool checkAndUpdateReader(MFRC522 &rfid, String readerName, int readerNum) {
    // Reset the RFID module
    rfid.PCD_Init();
    
    // Look for cards
    if (!rfid.PICC_IsNewCardPresent()) {
        return false;
    }
    
    // Select one of the cards
    if (!rfid.PICC_ReadCardSerial()) {
        return false;
    }
    
    // Check if this is a new tag detection
    bool isNewDetection = false;
    if (readerNum == 1) {
        isNewDetection = !tag1Active;
    } else if (readerNum == 2) {
        isNewDetection = !tag2Active;
    } else if (readerNum == 3) {
        isNewDetection = !tag3Active;
    }
    
    // Display UID in Serial Monitor if it's a new detection
    if (isNewDetection) {
        Serial.print(readerName + " - RFID Tag UID:");
        for (byte i = 0; i < rfid.uid.size; i++) {
            Serial.print(rfid.uid.uidByte[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        
        // Select which ring to use based on the reader number
        int currentRing = readerNum;

        // Print uuid of the current tag
        Serial.print("Current tag UID: ");
        for (byte i = 0; i < rfid.uid.size; i++) {
            Serial.print(rfid.uid.uidByte[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        
        // Check if it's our first tag
        if (compareUID(rfid.uid.uidByte, knownTag1)) {
            Serial.println(readerName + " - Green tag detected!");
            setRingColor(currentRing, 0, 255, 0);  // Set ring to green
            
            // Store the color
            if (readerNum == 1) {
                lastTag1Red = 0; lastTag1Green = 255; lastTag1Blue = 0;
            } else if (readerNum == 2) {
                lastTag2Red = 0; lastTag2Green = 255; lastTag2Blue = 0;
            } else {
                lastTag3Red = 0; lastTag3Green = 255; lastTag3Blue = 0;
            }
        }
        // Check if it's our second tag
        else if (compareUID(rfid.uid.uidByte, knownTag2)) {
            Serial.println(readerName + " - Red tag detected!");
            setRingColor(currentRing, 255, 0, 0);  // Set ring to red
            
            // Store the color
            if (readerNum == 1) {
                lastTag1Red = 255; lastTag1Green = 0; lastTag1Blue = 0;
            } else if (readerNum == 2) {
                lastTag2Red = 255; lastTag2Green = 0; lastTag2Blue = 0;
            } else {
                lastTag3Red = 255; lastTag3Green = 0; lastTag3Blue = 0;
            }
        }
        else {
            Serial.println(readerName + " - Unknown tag");
            // Rainbow effect for unknown tag
            for (int i = 0; i < 3; i++) {
                rainbowCycle(currentRing, 10);
            }
            setRingColor(currentRing, 0, 0, 255);  // Blue for unknown
            
            // Store the color
            if (readerNum == 1) {
                lastTag1Red = 0; lastTag1Green = 0; lastTag1Blue = 255;
            } else if (readerNum == 2) {
                lastTag2Red = 0; lastTag2Green = 0; lastTag2Blue = 255;
            } else {
                lastTag3Red = 0; lastTag3Green = 0; lastTag3Blue = 255;
            }
        }
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
void setRingColor(int ringNumber, uint8_t r, uint8_t g, uint8_t b) {
    int startLED = (ringNumber - 1) * NUM_LEDS_PER_RING;
    
    for (int i = 0; i < NUM_LEDS_PER_RING; i++) {
        strip.setPixelColor(startLED + i, strip.Color(r, g, b));
    }
    strip.show();
}

// Rainbow cycle effect for the specified ring
void rainbowCycle(int ringNumber, uint8_t wait) {
    int startLED = (ringNumber - 1) * NUM_LEDS_PER_RING;
    for (int j = 0; j < 256; j++) {
        for (int i = 0; i < NUM_LEDS_PER_RING; i++) {
            strip.setPixelColor(startLED + i, wheel(((i * 256 / NUM_LEDS_PER_RING) + j) & 255));
        }
        strip.show();
        delay(wait);
    }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheel(byte wheelPos) {
    wheelPos = 255 - wheelPos;
    if (wheelPos < 85) {
        return strip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
    }
    if (wheelPos < 170) {
        wheelPos -= 85;
        return strip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
    wheelPos -= 170;
    return strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}