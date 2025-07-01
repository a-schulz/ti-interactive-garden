#include <Arduino.h>
#include "BoardConfig.h"
#include "BoardController.h"
#include "PlantDatabase.h"

// Create global board controller instance
BoardController garden;

// Button pin for changing game mode
#define MODE_BUTTON_PIN 2  // Use digital pin 2 for the button

// Button debounce parameters
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 300;  // Longer debounce delay for human pressing

// Forward declaration of helper function
void processSerialCommand();
void runDiagnosticTest();
void checkModeButton();

void setup() {
    Serial.begin(9600);
    while (!Serial && millis() < 3000); // Wait for serial but timeout after 3 seconds
    
    Serial.println(F("Interactive Garden - Test Mode"));
    
    // Initialize the mode button pin with internal pull-up resistor
    pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
    
    // Initialize the board controller
    // Note: BoardController::begin() will call PlantDatabase::initialize()
    garden.begin();
    
    // Optimize RFID readers for NXP Ultra NFC tags to improve reading distance
    garden.optimizeRFIDReaders();
    
    // Place readers as neighbors (adjacent positions) to properly test plant relationships
    garden.placeReader(1, 2, 0, PARTIALLY_SHADED | DRY);         // Reader 1: Sunny and Dry (Potato tolerates it, Tomato dislikes)
    garden.placeReader(2, 4, 0, PARTIALLY_SHADED | DRY);       // Reader 2: Sunny and Moist (Tomato thrives, Potato tolerates)
    garden.placeReader(3, 5, 1, PARTIALLY_SHADED | MOIST);         // Reader 3: Sunny and Wet (Neither prefers this)
    garden.placeReader(4, 3, 1, PARTIALLY_SHADED | MOIST);
    garden.placeReader(5, 1, 1, PARTIALLY_SHADED | MOIST);
    garden.placeReader(6, 2, 2, PARTIALLY_SHADED | WET);

    // Quick startup test of all LEDs
    for (int i = 1; i <= 3; i++) {
        garden.rainbowEffect(i, 50); // Brief rainbow effect on each ring
    }
    
    // Show initial game mode
    garden.displayGameMode();
    
    Serial.println(F("Type 'test' for a diagnostic test or 'help' for commands"));
}

void loop() {
    // Check if mode button is pressed
    checkModeButton();
    
    // Update the garden board (checks readers, handles LED effects)
    garden.update();
    
    // Process any serial commands (for testing/diagnostics)
    if (Serial.available()) {
        processSerialCommand();
    }
    
    // Small delay to prevent CPU hogging
    delay(10);
}

void checkModeButton() {
    // Read the button state (LOW when pressed with pull-up resistor)
    int buttonState = digitalRead(MODE_BUTTON_PIN);
    
    // If the button is pressed (LOW), debounce and change mode
    if (buttonState == LOW) {
        unsigned long currentTime = millis();
        
        // Check if enough time has passed since the last button press
        if ((currentTime - lastDebounceTime) > debounceDelay) {
            // Change the game mode
            garden.changeGameMode();
            
            // Update the debounce time
            lastDebounceTime = currentTime;
        }
    }
}

void runDiagnosticTest() {
    Serial.println(F("=== Running Diagnostic Test ==="));
    
    // 1. Test registered tags
    Serial.println(F("Registered Plant Tags:"));
    Serial.println(F(" - Tomato: UID 04:53:45:3B"));
    Serial.println(F(" - Potato: UID 04:DA:41:3B"));
    
    // 2. Test LED rings
    Serial.println(F("\nTesting LED rings..."));
    for (int i = 1; i <= 3; i++) {
        Serial.print(F("Ring "));
        Serial.print(i);
        Serial.println(F(": Testing..."));
        
        // Test colors: Red, Green, Blue, then off
        garden.setRingColor(i, 255, 0, 0);  // Red
        delay(500);
        garden.setRingColor(i, 0, 255, 0);  // Green
        delay(500);
        garden.setRingColor(i, 0, 0, 255);  // Blue
        delay(500);
        garden.clearRing(i);
    }
    
    // 3. Report environment at each position
    Serial.println(F("\nEnvironment at each reader position:"));
    for (int i = 1; i <= 3; i++) {
        int readerIndex = i - 1;
        GridPosition* pos = garden.getReaderPosition(readerIndex);
        if (pos != nullptr) {
            Serial.print(F("Reader "));
            Serial.print(i);
            Serial.print(F(" at ("));
            Serial.print(pos->row);
            Serial.print(F(","));
            Serial.print(pos->col);
            Serial.print(F("): Attributes = "));
            Serial.println(pos->attributes, BIN);
        }
    }
    
    // 4. Show current game mode
    Serial.print(F("\nCurrent Game Mode: "));
    switch (garden.getCurrentGameMode()) {
        case ENVIRONMENT_MODE:
            Serial.println(F("Environment Check"));
            break;
        case NEIGHBORS_MODE:
            Serial.println(F("Neighbors Check"));
            break;
        case COMBINED_MODE:
            Serial.println(F("Combined Environment & Neighbors Check"));
            break;
    }
    
    Serial.println(F("\n=== Test Complete ==="));
    Serial.println(F("Place your tomato and potato tags on different readers and observe:"));
    Serial.println(F(" - LED effects will show plant interactions based on current game mode"));
    Serial.println(F(" - Press the button to cycle through different game modes"));
}

void processSerialCommand() {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "test") {
        // Run diagnostic test
        runDiagnosticTest();
    }
    else if (command == "mode") {
        // Change game mode via serial command
        garden.changeGameMode();
    }
    else if (command.startsWith("register ")) {
        // Format: "register [tag_id_hex] [plant_id]"
        // Example: "register 04E5121A 1" to register tag 04E5121A as TOMATO (1)
        
        String params = command.substring(9); // Skip "register "
        int spacePos = params.indexOf(' ');
        
        if (spacePos > 0) {
            String tagIdStr = params.substring(0, spacePos);
            int plantId = params.substring(spacePos + 1).toInt();
            
            // Convert hex string to bytes
            byte tagId[4] = {0};
            for (int i = 0; i < 4; i++) {
                if (i*2+2 <= tagIdStr.length()) {
                    String byteStr = tagIdStr.substring(i*2, i*2+2);
                    tagId[i] = strtol(byteStr.c_str(), NULL, 16);
                }
            }
            
            if (PlantDatabase::registerTag(tagId, static_cast<PlantID>(plantId))) {
                Serial.println(F("Tag registered successfully"));
            } else {
                Serial.println(F("Failed to register tag"));
            }
        } else {
            Serial.println(F("Invalid format. Use: register [tag_id_hex] [plant_id]"));
        }
    }
    else if (command == "help") {
        Serial.println(F("Available commands:"));
        Serial.println(F("test - Run a diagnostic test"));
        Serial.println(F("mode - Change game mode (same as pressing the button)"));
        Serial.println(F("register [tag_id_hex] [plant_id] - Register a new RFID tag"));
        Serial.println(F("  Plant IDs: 1=Tomato, 2=Potato, 3=Carrot, etc."));
        Serial.println(F("help - Display this help message"));
    }
    else {
        Serial.println(F("Unknown command. Type 'help' for available commands."));
    }
}
