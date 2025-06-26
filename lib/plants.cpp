#include "PlantDatabase.h"

// Initialize the plant database with plant information
Plant PlantDatabase::plants[NUM_PLANTS] = {
    // UNKNOWN
    {"Unknown", NONE, NONE, {0, 0, 255}},  // Blue for unknown
    
    // TOMATO
    {"Tomato", SUNNY | MOIST, PARTIALLY_SHADED | MOIST, {255, 50, 0}},  // Red-orange
    
    // POTATO
    {"Potato", PARTIALLY_SHADED | MOIST, DRY | MOIST, {150, 75, 0}},  // Brown
    
    // CARROT
    {"Carrot", SUNNY | MOIST, SUNNY | DRY | PARTIALLY_SHADED, {255, 120, 0}},  // Orange
    
    // BASIL
    {"Basil", SUNNY | MOIST, PARTIALLY_SHADED | MOIST, {0, 200, 0}},  // Green
    
    // PEPPER
    {"Pepper", SUNNY | MOIST, SUNNY | DRY, {255, 0, 0}},  // Red
    
    // LETTUCE
    {"Lettuce", PARTIALLY_SHADED | MOIST, SUNNY | MOIST | WET, {0, 255, 0}},  // Light green
    
    // ONION
    {"Onion", SUNNY | DRY, SUNNY | MOIST | PARTIALLY_SHADED, {255, 255, 0}},  // Yellow
    
    // CUCUMBER
    {"Cucumber", SUNNY | MOIST, SUNNY | WET | PARTIALLY_SHADED, {0, 255, 50}}  // Green-blue
};

// Plant relationship matrix (-1: hates, 0: neutral, 1: likes)
int8_t PlantDatabase::plantRelationships[NUM_PLANTS][NUM_PLANTS] = {
    // Unknown plant has neutral relationship with everything
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    
    // TOMATO relationships
    {0, 0, -1, 1, 1, 0, 0, 1, 0},  // Tomato hates potato, likes carrot and basil
    
    // POTATO relationships
    {0, -1, 0, 0, -1, 0, 0, 0, 0},  // Potato hates tomato and basil
    
    // CARROT relationships
    {0, 1, 0, 0, 0, 0, 1, 1, 0},    // Carrot likes tomato, lettuce, onion
    
    // BASIL relationships
    {0, 1, -1, 0, 0, 1, 0, 0, 0},   // Basil likes tomato, pepper
    
    // PEPPER relationships
    {0, 0, 0, 0, 1, 0, 0, 0, 0},    // Pepper likes basil
    
    // LETTUCE relationships
    {0, 0, 0, 1, 0, 0, 0, 1, 0},    // Lettuce likes carrot, onion
    
    // ONION relationships
    {0, 1, 0, 1, 0, 0, 1, 0, -1},   // Onion likes tomato, carrot, lettuce; hates cucumber
    
    // CUCUMBER relationships
    {0, 0, 0, 0, 0, 0, 0, -1, 0}    // Cucumber hates onion
};

// Start with some pre-registered RFID tags - up to 20 tags
TagInfo PlantDatabase::registeredTags[20] = {
    // Format: {{tag UID bytes}, associated plant}
    {{0x04, 0x53, 0x45, 0x3B}, TOMATO},   // Tomate
    {{0x04, 0x5B, 0x2B, 0x3B}, CUCUMBER}, // Gurke
    {{0x04, 0xDA, 0x41, 0x3B}, POTATO},   // Kartoffel
    {{0x04, 0xFF, 0x33, 0x3B}, CARROT},   // Karotte
    // Add more pre-registered tags as needed
};

uint8_t PlantDatabase::tagCount = 4;  // Number of pre-registered tags

void PlantDatabase::initialize() {
    // Any additional initialization can go here
}

PlantID PlantDatabase::identifyPlantByTag(byte* tagUid) {
    for (uint8_t i = 0; i < tagCount; i++) {
        if (compareUID(tagUid, registeredTags[i].uid)) {
            return registeredTags[i].plantId;
        }
    }
    return UNKNOWN;
}

const Plant* PlantDatabase::getPlantInfo(PlantID plantId) {
    if (plantId < NUM_PLANTS) {
        return &plants[plantId];
    }
    return &plants[UNKNOWN];  // Return unknown plant as fallback
}

PlantRelationship PlantDatabase::getRelationship(PlantID plant1, PlantID plant2) {
    if (plant1 < NUM_PLANTS && plant2 < NUM_PLANTS) {
        return static_cast<PlantRelationship>(plantRelationships[plant1][plant2]);
    }
    return NEUTRAL;  // Default to neutral if invalid plants
}

bool PlantDatabase::plantThrives(PlantID plantId, uint8_t environment) {
    if (plantId < NUM_PLANTS) {
        // All preferred conditions must be met
        return (plants[plantId].preferredEnvironment & environment) == plants[plantId].preferredEnvironment;
    }
    return false;
}

bool PlantDatabase::plantTolerates(PlantID plantId, uint8_t environment) {
    if (plantId < NUM_PLANTS) {
        // At least some tolerated conditions must be met
        return (plants[plantId].toleratedEnvironment & environment) != 0;
    }
    return false;
}

bool PlantDatabase::registerTag(byte* tagUid, PlantID plantId) {
    // Check if we have space for a new tag
    if (tagCount >= sizeof(registeredTags) / sizeof(registeredTags[0])) {
        return false;  // No more space
    }
    
    // Check if tag is already registered
    for (uint8_t i = 0; i < tagCount; i++) {
        if (compareUID(tagUid, registeredTags[i].uid)) {
            // Update existing tag
            registeredTags[i].plantId = plantId;
            return true;
        }
    }
    
    // Add new tag
    memcpy(registeredTags[tagCount].uid, tagUid, 4);
    registeredTags[tagCount].plantId = plantId;
    tagCount++;
    
    return true;
}

bool PlantDatabase::compareUID(byte* uid1, byte* uid2) {
    for (byte i = 0; i < 4; i++) {
        if (uid1[i] != uid2[i]) {
            return false;
        }
    }
    return true;
}