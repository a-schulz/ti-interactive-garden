#include <FastLED.h>

// Number of LEDs in the rings
#define NUM_LEDS_PER_RING 12
#define NUM_RINGS 8
#define TOTAL_LEDS (NUM_LEDS_PER_RING * NUM_RINGS)

// Data pin for LED control
#define LED_PIN 5  // Update with your actual data pin

// LED array
CRGB leds[TOTAL_LEDS];

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    Serial.println("LED Test Starting");
    
    // Initialize LED strip
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, TOTAL_LEDS);
    FastLED.setBrightness(10);  // Set to 20% brightness
    
    // Clear all LEDs
    FastLED.clear();
    FastLED.show();
    delay(1000);
}

void loop() {
    // Test 1: Light up each ring one at a time
    Serial.println("Test 1: Lighting up each ring");
    for (int ring = 0; ring < NUM_RINGS; ring++) {
        // Clear all LEDs
        FastLED.clear();
        
        // Light up current ring
        for (int led = 0; led < NUM_LEDS_PER_RING; led++) {
            leds[ring * NUM_LEDS_PER_RING + led] = CRGB::Red;
        }
        
        FastLED.show();
        Serial.print("Ring ");
        Serial.print(ring);
        Serial.println(" should be lit");
        delay(1000);
    }
    
    // Test 2: Chase pattern around each ring to verify wiring direction
    Serial.println("Test 2: Testing LED sequence in each ring");
    for (int ring = 0; ring < NUM_RINGS; ring++) {
        FastLED.clear();
        
        for (int led = 0; led < NUM_LEDS_PER_RING; led++) {
            FastLED.clear();
            leds[ring * NUM_LEDS_PER_RING + led] = CRGB::Green;
            FastLED.show();
            delay(200);
        }
    }
    
    // Test 3: Rainbow pattern across all LEDs
    Serial.println("Test 3: Rainbow pattern across all LEDs");
    for (int i = 0; i < 255; i++) {
        for (int led = 0; led < TOTAL_LEDS; led++) {
            leds[led] = CHSV(i, 255, 255);
        }
        FastLED.show();
        delay(10);
    }
    
    // Test 4: Concentric rings animation
    Serial.println("Test 4: Concentric rings animation");
    for (int i = 0; i < 3; i++) {
        for (int ring = 0; ring < NUM_RINGS; ring++) {
            FastLED.clear();
            for (int led = 0; led < NUM_LEDS_PER_RING; led++) {
                leds[ring * NUM_LEDS_PER_RING + led] = CRGB::Blue;
            }
            FastLED.show();
            delay(300);
        }
    }
    
    // Wait before repeating all tests
    FastLED.clear();
    FastLED.show();
    delay(2000);
}