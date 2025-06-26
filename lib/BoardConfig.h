#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <Arduino.h>

// Pin definitions for RFID readers
#define SS_PIN1 10    // First reader SS/SDA
#define RST_PIN1 9    // First reader RST
#define SS_PIN2 7     // Second reader SS/SDA
#define RST_PIN2 8    // Second reader RST
#define SS_PIN3 4     // Third reader SS/SDA
#define RST_PIN3 3    // Third reader RST
#define SS_PIN4 6     // Fourth reader SS/SDA
#define RST_PIN4 5    // Fourth reader RST
#define SS_PIN5 A0    // Fifth reader SS/SDA
#define RST_PIN5 A1   // Fifth reader RST
#define SS_PIN6 A2    // Sixth reader SS/SDA
#define RST_PIN6 A3   // Sixth reader RST

// LED data pins
#define LED_RING_CHAIN_PIN1 5  // LED chain 1 data pin (rings 1-4)
#define LED_RING_CHAIN_PIN2 6  // LED chain 2 data pin (rings 5-8)

// LED configuration
#define NUM_LEDS_PER_RING 12
#define NUM_RINGS 8
#define LEDS_PER_CHAIN (NUM_LEDS_PER_RING * 4)
#define TOTAL_LEDS (NUM_LEDS_PER_RING * NUM_RINGS)

// Garden grid configuration
#define MATRIX_ROWS 6
#define MATRIX_COLS 6
#define NUM_READERS 6

// Environment attributes as bit flags
enum EnvironmentAttribute {
    NONE = 0,
    DRY = 1,
    MOIST = 2,
    WET = 4,
    SUNNY = 8,
    PARTIALLY_SHADED = 16,
    SHADY = 32
};

// Relationship between plants
enum PlantRelationship {
    HATES = -1,
    NEUTRAL = 0,
    LIKES = 1
};

// Grid position structure
struct GridPosition {
    uint8_t row;
    uint8_t col;
    uint8_t attributes;  // Environmental attributes at this position
    int8_t readerIndex;  // -1 means no reader at this position
};

#endif // BOARD_CONFIG_H