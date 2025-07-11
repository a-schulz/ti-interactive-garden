#ifndef BOARD_CONTROLLER_H
#define BOARD_CONTROLLER_H

#include <Arduino.h>
// Replace MFRC522 with RFID1 library
#include "RFID1/rfid1.h"
#include <FastLED.h>
#include "BoardConfig.h"
#include "PlantDatabase.h"

// Game modes
enum GameMode {
    ENVIRONMENT_MODE = 0,
    NEIGHBORS_MODE = 1,
    COMBINED_MODE = 2
};

// Effect status for each reader
struct EffectState {
    bool environmentHappy;
    bool neighborRelationshipGood;
    unsigned long lastEffectTime;
};

// Holds the current state of a reader/position
struct ReaderState {
    bool tagPresent;
    unsigned long lastReadTime;
    PlantID currentPlant;
    byte tagUID[4];
};

class BoardController {
public:
    // Initialize the board hardware
    void begin();
    
    // Update method to be called in the main loop
    void update();
    
    // Place a reader at the specified grid position with environmental attributes
    bool placeReader(uint8_t readerNum, uint8_t row, uint8_t col, uint8_t attributes);
    
    // Check if two positions are neighbors (including diagonal)
    bool areNeighbors(uint8_t row1, uint8_t col1, uint8_t row2, uint8_t col2);
    
    // Get reader number at position, returns 0 if no reader
    uint8_t getReaderAt(uint8_t row, uint8_t col);
    
    // Get environment attributes at position
    uint8_t getAttributesAt(uint8_t row, uint8_t col);
    
    // Get the position information for a specific reader
    GridPosition* getReaderPosition(uint8_t readerIndex);
    
    // Change game mode
    void changeGameMode();
    
    // Get current game mode
    GameMode getCurrentGameMode() { return currentGameMode; }
    
    // Display current game mode on LEDs
    void displayGameMode();
    
    // Visual effects for feedback
    void showLikesEffect(uint8_t readerNum);
    void showDislikesEffect(uint8_t readerNum);
    void showNeutralEffect(uint8_t readerNum);
    void clearRing(uint8_t readerNum);
    
    // Specialized effects
    void pulseEffect(uint8_t readerNum, uint8_t r, uint8_t g, uint8_t b);
    void growthEffect(uint8_t readerNum, uint8_t targetReaderNum);
    void rainbowEffect(uint8_t readerNum, uint8_t duration);
    
    // Direct LED control for testing
    void setRingColor(uint8_t readerNum, uint8_t r, uint8_t g, uint8_t b);
    
    // RFID Reader optimization
    void optimizeRFIDReaders();
    void setRFIDMaxGain(uint8_t readerNum);

private:
    // Changed from MFRC522 to RFID1
    RFID1 readers[NUM_READERS];
    CRGB leds[TOTAL_LEDS];
    GridPosition grid[MATRIX_ROWS][MATRIX_COLS];
    ReaderState readerStates[NUM_READERS];
    EffectState effectStates[NUM_READERS];
    GridPosition* readerPositions[NUM_READERS];
    
    GameMode currentGameMode;
    
    const unsigned long READ_INTERVAL = 100;      // Time between read attempts (ms)
    const unsigned long TAG_TIMEOUT = 500;        // Time until tag is considered removed (ms)
    const unsigned long EFFECT_INTERVAL = 2000;   // Time between effect cycles (ms)
    
    // Initialize the grid matrix
    void initializeGrid();
    
    // Reader handling
    bool checkReader(uint8_t readerNum);
    void evaluatePlantInteractions(uint8_t readerNum);
    
    // Continuous effect handling
    void updateContinuousEffects();
    void applyContinuousEffect(uint8_t readerNum);
    
    // LED control
    void setRingColorWithBrightness(uint8_t readerNum, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
    uint16_t getRingStartLED(uint8_t readerNum);
};

#endif // BOARD_CONTROLLER_H