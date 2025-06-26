#include "BoardController.h"
#include <SPI.h>

void BoardController::begin() {
    // Initialize SPI
    SPI.begin();
    
    // Initialize RFID readers with appropriate pins
    readers[0].PCD_Init(SS_PIN1, RST_PIN1);
    readers[1].PCD_Init(SS_PIN2, RST_PIN2);
    readers[2].PCD_Init(SS_PIN3, RST_PIN3);
    readers[3].PCD_Init(SS_PIN4, RST_PIN4);
    readers[4].PCD_Init(SS_PIN5, RST_PIN5);
    readers[5].PCD_Init(SS_PIN6, RST_PIN6);
    
    // Initialize FastLED for our two LED chains
    FastLED.addLeds<WS2812B, LED_RING_CHAIN_PIN1, GRB>(leds, 0, LEDS_PER_CHAIN);
    FastLED.addLeds<WS2812B, LED_RING_CHAIN_PIN2, GRB>(leds, LEDS_PER_CHAIN, LEDS_PER_CHAIN);
    FastLED.setBrightness(30);  // Set global brightness
    
    // Clear all LEDs
    FastLED.clear();
    FastLED.show();
    
    // Initialize reader states
    for (uint8_t i = 0; i < NUM_READERS; i++) {
        readerStates[i].tagPresent = false;
        readerStates[i].lastReadTime = 0;
        readerStates[i].currentPlant = UNKNOWN;
    }
    
    // Initialize the grid
    initializeGrid();
    
    // Initialize plant database
    PlantDatabase::initialize();
    
    Serial.println(F("Board controller initialized"));
}

void BoardController::update() {
    unsigned long currentMillis = millis();
    static unsigned long lastReadAttempts[NUM_READERS] = {0};
    
    // Check each reader at appropriate intervals
    for (uint8_t i = 0; i < NUM_READERS; i++) {
        // Skip if this reader hasn't been placed on the grid
        if (readerPositions[i] == nullptr) continue;
        
        // Time to check this reader?
        if (currentMillis - lastReadAttempts[i] >= READ_INTERVAL) {
            lastReadAttempts[i] = currentMillis;
            
            if (checkReader(i)) {
                readerStates[i].lastReadTime = currentMillis;
                
                // If this is a new tag detection
                if (!readerStates[i].tagPresent) {
                    readerStates[i].tagPresent = true;
                    Serial.print(F("Reader "));
                    Serial.print(i + 1);
                    Serial.println(F(" - Tag detected"));
                    
                    // Evaluate plant interactions whenever a new plant is placed
                    evaluatePlantInteractions(i);
                }
            }
        }
        
        // Check for tag timeouts
        if (readerStates[i].tagPresent && 
            (currentMillis - readerStates[i].lastReadTime > TAG_TIMEOUT)) {
            readerStates[i].tagPresent = false;
            readerStates[i].currentPlant = UNKNOWN;
            
            Serial.print(F("Reader "));
            Serial.print(i + 1);
            Serial.println(F(" - Tag removed"));
            
            // Turn off the ring
            clearRing(i + 1);
        }
    }
}

bool BoardController::placeReader(uint8_t readerNum, uint8_t row, uint8_t col, uint8_t attributes) {
    if (readerNum < 1 || readerNum > NUM_READERS || 
        row >= MATRIX_ROWS || col >= MATRIX_COLS) {
        Serial.print(F("Invalid reader position: Reader "));
        Serial.print(readerNum);
        Serial.print(F(" at ("));
        Serial.print(row);
        Serial.print(F(","));
        Serial.print(col);
        Serial.println(F(")"));
        return false;
    }
    
    // Store reader position and attributes
    grid[row][col].readerIndex = readerNum - 1;  // 0-based index internally
    grid[row][col].attributes = attributes;
    readerPositions[readerNum - 1] = &grid[row][col];
    
    Serial.print(F("Reader "));
    Serial.print(readerNum);
    Serial.print(F(" placed at ("));
    Serial.print(row);
    Serial.print(F(","));
    Serial.print(col);
    Serial.print(F(") with attributes: "));
    Serial.println(attributes, BIN);
    
    return true;
}

bool BoardController::areNeighbors(uint8_t row1, uint8_t col1, uint8_t row2, uint8_t col2) {
    // Check if positions are within bounds
    if (row1 >= MATRIX_ROWS || col1 >= MATRIX_COLS || 
        row2 >= MATRIX_ROWS || col2 >= MATRIX_COLS) {
        return false;
    }
    
    // Check if positions are adjacent (including diagonals)
    int rowDiff = abs((int)row1 - (int)row2);
    int colDiff = abs((int)col1 - (int)col2);
    
    // Adjacent horizontally, vertically, or diagonally
    return (rowDiff <= 1 && colDiff <= 1) && !(rowDiff == 0 && colDiff == 0);
}

uint8_t BoardController::getReaderAt(uint8_t row, uint8_t col) {
    if (row < MATRIX_ROWS && col < MATRIX_COLS && grid[row][col].readerIndex >= 0) {
        return grid[row][col].readerIndex + 1; // Return 1-based reader number
    }
    return 0; // No reader at this position
}

uint8_t BoardController::getAttributesAt(uint8_t row, uint8_t col) {
    if (row < MATRIX_ROWS && col < MATRIX_COLS) {
        return grid[row][col].attributes;
    }
    return NONE;
}

GridPosition* BoardController::getReaderPosition(uint8_t readerIndex) {
    if (readerIndex < NUM_READERS) {
        return readerPositions[readerIndex];
    }
    return nullptr;
}

void BoardController::showLikesEffect(uint8_t readerNum) {
    // Pulsing green effect for positive feedback
    for (int i = 0; i < 3; i++) { // Pulse 3 times
        pulseEffect(readerNum, 0, 255, 0);
        delay(100);
    }
}

void BoardController::showDislikesEffect(uint8_t readerNum) {
    // Pulsing red effect for negative feedback
    for (int i = 0; i < 3; i++) { // Pulse 3 times
        pulseEffect(readerNum, 255, 0, 0);
        delay(100);
    }
}

void BoardController::showNeutralEffect(uint8_t readerNum) {
    // Pulsing blue effect for neutral feedback
    pulseEffect(readerNum, 0, 0, 255);
}

void BoardController::clearRing(uint8_t readerNum) {
    setRingColor(readerNum, 0, 0, 0);
}

void BoardController::pulseEffect(uint8_t readerNum, uint8_t r, uint8_t g, uint8_t b) {
    // Save current color
    uint8_t savedR = r;
    uint8_t savedG = g;
    uint8_t savedB = b;
    
    // Pulse down and up
    for (int i = 10; i >= 2; i--) {
        setRingColorWithBrightness(readerNum, savedR, savedG, savedB, i * 10);
        delay(20);
    }
    for (int i = 2; i <= 10; i++) {
        setRingColorWithBrightness(readerNum, savedR, savedG, savedB, i * 10);
        delay(20);
    }
    
    // Restore full brightness
    setRingColor(readerNum, savedR, savedG, savedB);
}

void BoardController::growthEffect(uint8_t readerNum, uint8_t targetReaderNum) {
    // Green growing effect between two plants
    for (int i = 0; i < 5; i++) {
        // Start dim and grow brighter
        setRingColorWithBrightness(readerNum, 0, 200, 0, 20 * (i+1));
        setRingColorWithBrightness(targetReaderNum, 0, 200, 0, 20 * (i+1));
        delay(100);
    }
}

void BoardController::rainbowEffect(uint8_t readerNum, uint8_t duration) {
    uint16_t startLED = getRingStartLED(readerNum);
    if (startLED == 0xFFFF) return; // Invalid ring
    
    for (int j = 0; j < duration; j++) {
        for (int i = 0; i < NUM_LEDS_PER_RING; i++) {
            // Create rainbow pattern
            leds[startLED + i] = CHSV((i * 256 / NUM_LEDS_PER_RING) + j, 255, 255);
        }
        FastLED.show();
        delay(15);
    }
}

void BoardController::initializeGrid() {
    // Initialize the grid with default values
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            grid[row][col].row = row;
            grid[row][col].col = col;
            grid[row][col].attributes = NONE;
            grid[row][col].readerIndex = -1; // No reader at this position
        }
    }
    
    // Initialize reader positions array to nullptr
    for (uint8_t i = 0; i < NUM_READERS; i++) {
        readerPositions[i] = nullptr;
    }
}

bool BoardController::checkReader(uint8_t readerNum) {
    if (readerNum >= NUM_READERS) return false;
    
    // Properly initialize SPI for this reader
    SPI.begin();
    
    // Reset the reader with proper pin initialization
    readers[readerNum].PCD_Init();
    
    // Give the reader a moment to stabilize
    delay(10);
    
    // Look for new cards
    if (!readers[readerNum].PICC_IsNewCardPresent()) {
        return false;
    }
    
    // Select one of the cards
    if (!readers[readerNum].PICC_ReadCardSerial()) {
        return false;
    }
    
    // Get reader position
    GridPosition* pos = readerPositions[readerNum];
    if (!pos) return false;
    
    // If this is a new tag or a different tag
    bool isNewOrChangedTag = !readerStates[readerNum].tagPresent;
    
    // Check if it's a different tag from the previously detected one
    if (readerStates[readerNum].tagPresent) {
        isNewOrChangedTag = false;
        for (byte i = 0; i < readers[readerNum].uid.size; i++) {
            if (readers[readerNum].uid.uidByte[i] != readerStates[readerNum].tagUID[i]) {
                isNewOrChangedTag = true;
                break;
            }
        }
    }
    
    if (isNewOrChangedTag) {
        // Store UID of this tag
        memcpy(readerStates[readerNum].tagUID, readers[readerNum].uid.uidByte, 4);
        
        // DEBUG: Print the full tag UID in hex format for comparison
        Serial.print(F("Reader "));
        Serial.print(readerNum + 1);
        Serial.print(F(" - Tag UID (HEX): "));
        for (byte i = 0; i < readers[readerNum].uid.size && i < 4; i++) {
            if (readers[readerNum].uid.uidByte[i] < 0x10) Serial.print('0');
            Serial.print(readers[readerNum].uid.uidByte[i], HEX);
            if (i < 3) Serial.print(':');
        }
        Serial.println();
        
        // DEBUG: Show registered tag values for comparison
        Serial.println(F("Known tag values:"));
        Serial.println(F(" - Tomato: 04:53:45:3B"));
        Serial.println(F(" - Potato: 04:DA:41:3B"));
        
        // Identify the plant
        PlantID plantId = PlantDatabase::identifyPlantByTag(readers[readerNum].uid.uidByte);
        readerStates[readerNum].currentPlant = plantId;
        
        // Debug output
        Serial.print(F("Reader "));
        Serial.print(readerNum + 1);
        Serial.print(F(" - Identified as: "));
        Serial.println(PlantDatabase::getPlantInfo(plantId)->name);
        
        // Set the ring color based on the plant
        const Plant* plant = PlantDatabase::getPlantInfo(plantId);
        setRingColor(readerNum + 1, plant->color[0], plant->color[1], plant->color[2]);
    }
    
    // Properly end the communication with this card
    readers[readerNum].PICC_HaltA(); // Halt PICC
    readers[readerNum].PCD_StopCrypto1(); // Stop encryption
    
    return true;
}

void BoardController::evaluatePlantInteractions(uint8_t readerNum) {
    PlantID currentPlant = readerStates[readerNum].currentPlant;
    if (currentPlant == UNKNOWN) return;
    
    GridPosition* pos = readerPositions[readerNum];
    if (!pos) return;
    
    // 1. Check if plant likes the environment
    const Plant* plant = PlantDatabase::getPlantInfo(currentPlant);
    bool plantHappy = PlantDatabase::plantThrives(currentPlant, pos->attributes);
    bool plantOkay = PlantDatabase::plantTolerates(currentPlant, pos->attributes);
    
    if (plantHappy) {
        Serial.print(F("Plant '"));
        Serial.print(plant->name);
        Serial.print(F("' is happy with the environment at reader "));
        Serial.println(readerNum + 1);
        showLikesEffect(readerNum + 1);
    } else if (plantOkay) {
        Serial.print(F("Plant '"));
        Serial.print(plant->name);
        Serial.print(F("' tolerates the environment at reader "));
        Serial.println(readerNum + 1);
        showNeutralEffect(readerNum + 1);
    } else {
        Serial.print(F("Plant '"));
        Serial.print(plant->name);
        Serial.print(F("' is unhappy with the environment at reader "));
        Serial.println(readerNum + 1);
        showDislikesEffect(readerNum + 1);
    }
    
    // 2. Check neighboring plants for interactions
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            // Skip if this is not a neighboring position or has no reader
            if (!areNeighbors(pos->row, pos->col, row, col) || 
                grid[row][col].readerIndex < 0) {
                continue;
            }
            
            uint8_t neighborReaderNum = grid[row][col].readerIndex;
            
            // Skip if neighbor has no tag present
            if (!readerStates[neighborReaderNum].tagPresent) continue;
            
            PlantID neighborPlant = readerStates[neighborReaderNum].currentPlant;
            
            // Evaluate relationship
            PlantRelationship relationship = PlantDatabase::getRelationship(currentPlant, neighborPlant);
            
            if (relationship == LIKES) {
                // Positive interaction
                Serial.print(F("Plant '"));
                Serial.print(plant->name);
                Serial.print(F("' likes being next to '"));
                Serial.print(PlantDatabase::getPlantInfo(neighborPlant)->name);
                Serial.println(F("'"));
                
                growthEffect(readerNum + 1, neighborReaderNum + 1);
            }
            else if (relationship == HATES) {
                // Negative interaction
                Serial.print(F("Plant '"));
                Serial.print(plant->name);
                Serial.print(F("' dislikes being next to '"));
                Serial.print(PlantDatabase::getPlantInfo(neighborPlant)->name);
                Serial.println(F("'"));
                
                showDislikesEffect(readerNum + 1);
            }
        }
    }
}

void BoardController::setRingColor(uint8_t readerNum, uint8_t r, uint8_t g, uint8_t b) {
    uint16_t startLED = getRingStartLED(readerNum);
    
    if (startLED != 0xFFFF) { // Valid ring
        for (int i = 0; i < NUM_LEDS_PER_RING; i++) {
            leds[startLED + i] = CRGB(r, g, b);
        }
        FastLED.show();
    }
}

void BoardController::setRingColorWithBrightness(uint8_t readerNum, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
    uint8_t adjustedR = (r * brightness) / 100;
    uint8_t adjustedG = (g * brightness) / 100;
    uint8_t adjustedB = (b * brightness) / 100;
    setRingColor(readerNum, adjustedR, adjustedG, adjustedB);
}

uint16_t BoardController::getRingStartLED(uint8_t readerNum) {
    if (readerNum < 1 || readerNum > NUM_READERS) return 0xFFFF; // Invalid
    
    // Calculate which LED chain and position based on reader number
    if (readerNum <= 4) {
        // First chain (rings 1-4)
        return (readerNum - 1) * NUM_LEDS_PER_RING;
    } else {
        // Second chain (rings 5-8)
        return LEDS_PER_CHAIN + ((readerNum - 5) * NUM_LEDS_PER_RING);
    }
}