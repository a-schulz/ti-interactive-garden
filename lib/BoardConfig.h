#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <Arduino.h>

// Common SPI pins for all readers
#define COMMON_SS_PIN 10  // Shared SS pin for all readers
#define COMMON_RST_PIN 9  // Shared Reset pin for all readers

// Individual MISO pins for each reader (instead of individual SS pins)
#define MISO_PIN1 2  // MISO pin for first reader
#define MISO_PIN2 4  // MISO pin for second reader  
#define MISO_PIN3 6  // MISO pin for third reader
#define MISO_PIN4 8  // MISO pin for fourth reader
#define MISO_PIN5 A0  // MISO pin for fifth reader
#define MISO_PIN6 A2  // MISO pin for sixth reader

// LED data pins
#define LED_RING_CHAIN_PIN1 5  // LED chain 1 data pin (rings 1-4)
#define LED_RING_CHAIN_PIN2 7  // LED chain 2 data pin (rings 5-8)

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