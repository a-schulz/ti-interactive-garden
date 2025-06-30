#ifndef PLANT_DATABASE_H
#define PLANT_DATABASE_H

#include <Arduino.h>
#include "BoardConfig.h"

// Plant ID enum for easy reference
enum PlantID {
    UNKNOWN = 0,
    CARROT,
    TOMATO,
    ONION,
    POTATO,
    EGGPLANT,
    LETTUCE,
    PEA,
    CUCUMBER,
    // Add more plants as needed
    NUM_PLANTS  // This will be the count of plants
};

// Structure to hold plant information
struct Plant {
    const char* name;               // Plant name
    uint8_t preferredEnvironment;   // Bit flags for preferred conditions
    uint8_t toleratedEnvironment;   // Conditions plant can tolerate
    uint8_t color[3];               // RGB color for this plant's LED display
};

// Structure to store registered RFID tag info
struct TagInfo {
    byte uid[4];       // UID of the tag (assuming 4-byte UIDs)
    PlantID plantId;   // Associated plant
};

class PlantDatabase {
public:
    // Initialize the database
    static void initialize();
    
    // Lookup plant by tag UID
    static PlantID identifyPlantByTag(byte* tagUid);
    
    // Get plant information
    static const Plant* getPlantInfo(PlantID plantId);
    
    // Check relationships between plants
    static PlantRelationship getRelationship(PlantID plant1, PlantID plant2);
    
    // Check if plant thrives in given environment
    static bool plantThrives(PlantID plantId, uint8_t environment);
    
    // Check if plant tolerates given environment
    static bool plantTolerates(PlantID plantId, uint8_t environment);
    
    // Register a new tag-plant association
    static bool registerTag(byte* tagUid, PlantID plantId);

private:
    static Plant plants[NUM_PLANTS];
    static TagInfo registeredTags[];
    static int8_t plantRelationships[NUM_PLANTS][NUM_PLANTS];
    static uint8_t tagCount;
    
    // Helper function to compare UIDs
    static bool compareUID(byte* uid1, byte* uid2);
};

#endif // PLANT_DATABASE_H