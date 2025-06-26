#include <vector>
#include <string>
#include <unordered_map>

// Define plant IDs as constants for clarity
enum PlantID {
    TOMATO = 0,
    POTATO = 1,
    CARROT = 2,
    BASIL = 3,
    // Add more plants as needed
    NUM_PLANTS
};

// Plant properties
struct Plant {
    const char* name;
    std::vector<uint32_t> tags;
    uint8_t attributes;  // Combine moisture + sunlight preferences in one byte
};

// Plant data
Plant plantCatalog[NUM_PLANTS] = {
    {"Tomato", {0xA1B2C3D4, 0xE5F6A7B8}, MOIST | SUNNY},
    {"Potato", {0x12345678, 0x87654321}, MOIST | PARTIALLY_SHADED},
    {"Carrot", {0xAABBCCDD}, DRY | MOIST | SUNNY | PARTIALLY_SHADED},
    {"Basil", {0x11223344}, MOIST | SUNNY},
};

// Relationship matrix (1: likes, -1: hates, 0: neutral)
// [row plant] relationship with [column plant]
int8_t plantRelationships[NUM_PLANTS][NUM_PLANTS] = {
    //TOMATO  POTATO  CARROT  BASIL
    {   0,     -1,      1,      1   },  // TOMATO's relationships
    {  -1,      0,      0,      0   },  // POTATO's relationships
    {   1,      0,      0,      0   },  // CARROT's relationships
    {   1,      0,      0,      0   },  // BASIL's relationships
};

// Helper functions for checking relationships
bool plantsLikeEachOther(PlantID plant1, PlantID plant2) {
    return plantRelationships[plant1][plant2] == 1;
}

bool plantsHateEachOther(PlantID plant1, PlantID plant2) {
    return plantRelationships[plant1][plant2] == -1;
}

// Find plant by tag ID
PlantID findPlantByTag(uint32_t tagID) {
    for (int i = 0; i < NUM_PLANTS; i++) {
        for (uint32_t tag : plantCatalog[i].tags) {
            if (tag == tagID)
                return static_cast<PlantID>(i);
        }
    }
    return NUM_PLANTS; // Not found
}