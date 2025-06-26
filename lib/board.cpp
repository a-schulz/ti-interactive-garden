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


#define LED_RING_CHAIN_PIN1 5  // LED data input pin
#define LED_RING_CHAIN_PIN2 6  // LED data input pin

// Number of LEDs in the rings
#define NUM_LEDS_PER_RING 12
#define NUM_RINGS 4
#define TOTAL_LEDS (NUM_LEDS_PER_RING * NUM_RINGS)

// Matrix dimensions
#define MATRIX_ROWS 6
#define MATRIX_COLS 6
#define NUM_READERS 6

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