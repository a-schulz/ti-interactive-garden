#include <Arduino.h>
#include "BoardConfig.h"
#include "BoardController.h"
#include "PlantDatabase.h"

// Create global board controller instance
BoardController garden;

// Forward declaration of helper function
void processSerialCommand();
void runDiagnosticTest();

void setup() {
    Serial.begin(9600);
    while (!Serial && millis() < 3000); // Wait for serial but timeout after 3 seconds
    
    Serial.println(F("Interactive Garden - Test Mode"));
    
    // Initialize the board controller
    // Note: BoardController::begin() will call PlantDatabase::initialize()
    garden.begin();
    
    // For testing, only place the first 3 readers with strategic environments
    // Row 0
    garden.placeReader(1, 0, 0, SUNNY | DRY);         // Reader 1: Sunny and Dry (Potato tolerates it, Tomato dislikes)
    garden.placeReader(2, 0, 2, SUNNY | MOIST);       // Reader 2: Sunny and Moist (Tomato thrives, Potato tolerates)
    garden.placeReader(3, 0, 4, SUNNY | WET);         // Reader 3: Sunny and Wet (Neither prefers this)
    
    Serial.println(F("Test Garden initialized with 3 readers!"));
    Serial.println(F("Reader 1: Sunny and Dry (Potato tolerates it, Tomato dislikes)"));
    Serial.println(F("Reader 2: Sunny and Moist (Tomato thrives, Potato tolerates)"));
    Serial.println(F("Reader 3: Sunny and Wet (Neither prefers this)"));
    
    // Quick startup test of all LEDs
    for (int i = 1; i <= 3; i++) {
        garden.rainbowEffect(i, 50); // Brief rainbow effect on each ring
    }
    
    Serial.println(F("Type 'test' for a diagnostic test or 'help' for commands"));
}

void loop() {
    // Update the garden board (checks readers, handles LED effects)
    garden.update();
    
    // Process any serial commands (for testing/diagnostics)
    if (Serial.available()) {
        processSerialCommand();
    }
    
    // Small delay to prevent CPU hogging
    delay(10);
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
    
    Serial.println(F("\n=== Test Complete ==="));
    Serial.println(F("Place your tomato and potato tags on different readers and observe:"));
    Serial.println(F(" - LED colors should indicate plant type"));
    Serial.println(F(" - Reader 2 (SUNNY|MOIST) is ideal for Tomato"));
    Serial.println(F(" - If Tomato and Potato are on adjacent readers, they should show dislike effect"));
}

void processSerialCommand() {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "test") {
        // Run diagnostic test
        runDiagnosticTest();
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
        Serial.println(F("register [tag_id_hex] [plant_id] - Register a new RFID tag"));
        Serial.println(F("  Plant IDs: 1=Tomato, 2=Potato, 3=Carrot, etc."));
        Serial.println(F("help - Display this help message"));
    }
    else {
        Serial.println(F("Unknown command. Type 'help' for available commands."));
    }
}
