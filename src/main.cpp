#include <Arduino.h>
#include "BoardConfig.h"
#include "BoardController.h"
#include "PlantDatabase.h"

// Create global board controller instance
BoardController garden;

// Forward declaration of helper function
void processSerialCommand();

void setup() {
    Serial.begin(9600);
    while (!Serial && millis() < 3000); // Wait for serial but timeout after 3 seconds
    
    Serial.println(F("Interactive Garden - Starting"));
    
    // Initialize the board controller
    // Note: BoardController::begin() will call PlantDatabase::initialize()
    garden.begin();
    
    // Place readers in the grid with environment attributes
    // Row 0
    garden.placeReader(1, 0, 0, SUNNY | DRY);
    garden.placeReader(2, 0, 2, SUNNY | MOIST);
    garden.placeReader(3, 0, 4, SUNNY | WET);
    
    // Row 2
    garden.placeReader(4, 2, 1, PARTIALLY_SHADED | MOIST);
    
    // Row 4
    garden.placeReader(5, 4, 2, SHADY | MOIST);
    garden.placeReader(6, 4, 5, SHADY | DRY);
    
    Serial.println(F("Garden initialized! Place your plants on the readers"));
    
    // Quick startup test of all LEDs
    for (int i = 1; i <= NUM_READERS; i++) {
        garden.rainbowEffect(i, 50); // Brief rainbow effect on each ring
    }
}

void loop() {
    // Update the garden board (checks readers, handles LED effects)
    garden.update();
    
    // Process any serial commands (optional for debugging/control)
    if (Serial.available()) {
        processSerialCommand();
    }
    
    // Small delay to prevent CPU hogging
    delay(10);
}

void processSerialCommand() {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command.startsWith("test")) {
        // Test all LED rings
        Serial.println(F("Testing all LED rings..."));
        for (int i = 1; i <= NUM_READERS; i++) {
            garden.rainbowEffect(i, 100);
        }
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
        Serial.println(F("test - Run a test pattern on all LED rings"));
        Serial.println(F("register [tag_id_hex] [plant_id] - Register a new RFID tag"));
        Serial.println(F("help - Display this help message"));
    }
    else {
        Serial.println(F("Unknown command. Type 'help' for available commands."));
    }
}
