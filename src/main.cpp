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

// Matrix dimensions
#define MATRIX_ROWS 6
#define MATRIX_COLS 6
#define MAX_READERS 3 // Number of readers

// Reader attributes enum - can be extended as needed
enum ReaderAttribute {
    NONE = 0,
    DRY = 1,
    MOIST = 2,
    WET = 4,
    SUNNY = 8,
    PARTIALLY_SHADED = 16,
    SHADY = 32,
};

// Structure to represent a reader's position and attributes in the grid
struct ReaderPosition {
    uint8_t row;         // Row in the matrix (0-5)
    uint8_t col;         // Column in the matrix (0-5)
    uint8_t attributes;  // Bitmap of attributes (using ReaderAttribute enum)
    int readerIndex;     // Which reader (1-3 currently) or -1 if no reader at this position
};

// Define the matrix grid - initialize all positions without readers
ReaderPosition readerMatrix[MATRIX_ROWS][MATRIX_COLS];

// Array to directly access reader positions by reader number
ReaderPosition* readerPositions[MAX_READERS];

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

// Reader position management functions
void initializeReaderMatrix();
void placeReader(int readerNum, uint8_t row, uint8_t col, uint8_t attributes);
bool areNeighbors(int reader1, int reader2);
float getDistance(int reader1, int reader2);
ReaderPosition* getReaderPosition(int readerNum);
bool hasAttribute(int readerNum, ReaderAttribute attr);
void getNeighbors(int readerNum, int* neighborList, int* count);

// Check for interactions between neighboring readers
void checkReaderInteractions();

// Create a pulse effect on the specified ring
void pulseEffect(int ringNumber, uint8_t r, uint8_t g, uint8_t b);

// Set ring color with specific brightness
void setRingColorBrightness(int ringNumber, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);


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
    
    // Initialize reader matrix
    initializeReaderMatrix();
    
    // Place readers in the grid with attribute
    
    // Reader 1 at position (1,1) with DRY attribute
    placeReader(1, 0, 0, DRY);
    
    // Reader 2 at position (1,4) with MOIST attribute
    placeReader(2, 0, 2, MOIST);
    
    // Reader 3 at position (4,2) with DRY attribute
    placeReader(3, 0, 4, DRY);
    
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
    
    // Check for interactions between active readers
    checkReaderInteractions();
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

// Initialize the reader matrix with default values
void initializeReaderMatrix() {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            readerMatrix[row][col] = {static_cast<uint8_t>(row), static_cast<uint8_t>(col), NONE, -1};
        }
    }
    
    // Initialize reader positions array to NULL
    for (int i = 0; i < MAX_READERS; i++) {
        readerPositions[i] = nullptr;
    }
}

// Place a reader in the matrix at the specified location with given attributes
void placeReader(int readerNum, uint8_t row, uint8_t col, uint8_t attributes) {
    if (readerNum < 1 || readerNum > MAX_READERS || row >= MATRIX_ROWS || col >= MATRIX_COLS) {
        Serial.print(F("Invalid reader position: Reader "));
        Serial.print(readerNum);
        Serial.print(F(" at ("));
        Serial.print(row);
        Serial.print(F(","));
        Serial.print(col);
        Serial.println(F(")"));
        return;
    }
    
    readerMatrix[row][col].readerIndex = readerNum - 1;  // Store 0-based index
    readerMatrix[row][col].attributes = attributes;
    readerPositions[readerNum - 1] = &readerMatrix[row][col];
    
    Serial.print(F("Reader "));
    Serial.print(readerNum);
    Serial.print(F(" placed at ("));
    Serial.print(row);
    Serial.print(F(","));
    Serial.print(col);
    Serial.print(F(") with attributes: "));
    Serial.println(attributes, BIN);  // Print attributes in binary format
}

// Check if two readers are neighbors in the matrix
bool areNeighbors(int reader1, int reader2) {
    ReaderPosition* pos1 = getReaderPosition(reader1);
    ReaderPosition* pos2 = getReaderPosition(reader2);
    
    if (!pos1 || !pos2) return false;
    
    // Check if they are in adjacent cells (horizontal, vertical, or diagonal)
    int rowDiff = abs((int)pos1->row - (int)pos2->row);
    int colDiff = abs((int)pos1->col - (int)pos2->col);
    
    // Adjacent horizontally or vertically (not diagonally)
    // return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
    
    // If you want to include diagonal neighbors, use:
    return rowDiff <= 1 && colDiff <= 1 && !(rowDiff == 0 && colDiff == 0);
}


// Get the position structure for a given reader number
ReaderPosition* getReaderPosition(int readerNum) {
    if (readerNum < 1 || readerNum > MAX_READERS) return nullptr;
    return readerPositions[readerNum - 1];
}

// Check if a reader has a specific attribute
bool hasAttribute(int readerNum, ReaderAttribute attr) {
    ReaderPosition* pos = getReaderPosition(readerNum);
    if (!pos) return false;
    return (pos->attributes & attr) != 0;
}

// Get the neighbor readers of a given reader
void getNeighbors(int readerNum, int* neighborList, int* count) {
    *count = 0;  // Initialize count
    
    ReaderPosition* pos = getReaderPosition(readerNum);
    if (!pos) return;
    
    // Check all 4 adjacent positions (up, right, down, left)
    const int dr[4] = {-1, 0, 1, 0};  // row offsets
    const int dc[4] = {0, 1, 0, -1};  // column offsets
    
    for (int i = 0; i < 4; i++) {
        int newRow = pos->row + dr[i];
        int newCol = pos->col + dc[i];
        
        // Check if position is within bounds
        if (newRow >= 0 && newRow < MATRIX_ROWS && newCol >= 0 && newCol < MATRIX_COLS) {
            // Check if there's a reader at this position
            int neighborIndex = readerMatrix[newRow][newCol].readerIndex;
            if (neighborIndex >= 0) {
                neighborList[*count] = neighborIndex + 1;  // Convert back to 1-based reader number
                (*count)++;
            }
        }
    }
}

// Check for interactions between neighboring readers
void checkReaderInteractions() {
    // Array to store neighbor reader numbers
    int neighbors[MAX_READERS];
    int neighborCount = 0;
    
    // Check for reader 1 interactions
    if (tag1Active) {
        getNeighbors(1, neighbors, &neighborCount);
        
        // Process each neighbor
        for (int i = 0; i < neighborCount; i++) {
            int neighborReader = neighbors[i];
            
            // Check if neighbor has active tag
            bool neighborActive = false;
            byte neighborRed = 0, neighborGreen = 0, neighborBlue = 0;
            
            if (neighborReader == 2 && tag2Active) {
                neighborActive = true;
                neighborRed = lastTag2Red;
                neighborGreen = lastTag2Green;
                neighborBlue = lastTag2Blue;
            } else if (neighborReader == 3 && tag3Active) {
                neighborActive = true;
                neighborRed = lastTag3Red;
                neighborGreen = lastTag3Green;
                neighborBlue = lastTag3Blue;
            }
            
            // If neighbor has active tag, create an interaction effect
            if (neighborActive) {
                // Mix colors for interaction effect between the two readers
                byte mixedRed = (lastTag1Red + neighborRed) / 2;
                byte mixedGreen = (lastTag1Green + neighborGreen) / 2;
                byte mixedBlue = (lastTag1Blue + neighborBlue) / 2;
                
                // Create a visual effect on both rings to show interaction
                pulseEffect(1, mixedRed, mixedGreen, mixedBlue);
                pulseEffect(neighborReader, mixedRed, mixedGreen, mixedBlue);
                
            }
        }
    }
    
    // Check for reader 2 interactions (only need to check with reader 3, as reader 1 was already checked)
    if (tag2Active) {
        getNeighbors(2, neighbors, &neighborCount);
        
        // Look for reader 3 in the neighbors
        for (int i = 0; i < neighborCount; i++) {
            if (neighbors[i] == 3 && tag3Active) {
                // Mix colors
                byte mixedRed = (lastTag2Red + lastTag3Red) / 2;
                byte mixedGreen = (lastTag2Green + lastTag3Green) / 2;
                byte mixedBlue = (lastTag2Blue + lastTag3Blue) / 2;
                
                // Create visual effect
                pulseEffect(2, mixedRed, mixedGreen, mixedBlue);
                pulseEffect(3, mixedRed, mixedGreen, mixedBlue);
                
                break;
            }
        }
    }
}

// Create a pulse effect on the specified ring
void pulseEffect(int ringNumber, uint8_t r, uint8_t g, uint8_t b) {
    // Save current color
    uint8_t savedR = r;
    uint8_t savedG = g;
    uint8_t savedB = b;
    
    // Pulse down and up
    for (int i = 10; i >= 2; i--) {
        setRingColorBrightness(ringNumber, savedR, savedG, savedB, i * 10);
        delay(20);
    }
    for (int i = 2; i <= 10; i++) {
        setRingColorBrightness(ringNumber, savedR, savedG, savedB, i * 10);
        delay(20);
    }
    
    // Restore full brightness
    setRingColorBrightness(ringNumber, savedR, savedG, savedB, 100);
}

// Set ring color with specific brightness
void setRingColorBrightness(int ringNumber, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
    uint8_t adjustedR = (r * brightness) / 100;
    uint8_t adjustedG = (g * brightness) / 100;
    uint8_t adjustedB = (b * brightness) / 100;
    setRingColor(ringNumber, adjustedR, adjustedG, adjustedB);
}

void growthEffect(int reader1, int reader2) {
    // Green growing effect
    for (int i = 0; i < 5; i++) {
        // Start dim
        setRingColorBrightness(reader1, 0, 100, 0, 20 * i);
        setRingColorBrightness(reader2, 0, 100, 0, 20 * i);
        delay(100);
    }
}
