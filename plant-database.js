/**
 * Plant Database for the Interactive Garden
 * Defines all plants, their environmental preferences, and relationships with other plants
 */

// Environment constants
const ENVIRONMENT = {
    // Light conditions
    SUNNY: 1,
    PARTIAL_SHADE: 2,
    SHADE: 4,
    // Moisture conditions
    DRY: 8,
    MOIST: 16,
    WET: 32
};

// Relationship types
const RELATIONSHIP = {
    NEUTRAL: 0,
    LIKES: 1,
    HATES: 2
};

// Plant class definition
class Plant {
    constructor(id, name, imagePath, preferredEnvironment, toleratedEnvironment) {
        this.id = id;
        this.name = name;
        this.imagePath = imagePath; 
        this.preferredEnvironment = preferredEnvironment;
        this.toleratedEnvironment = toleratedEnvironment;
        this.relationships = {}; // Will hold relationships with other plants
    }

    // Define relationships with other plants
    addRelationship(otherPlantId, relationship) {
        this.relationships[otherPlantId] = relationship;
    }

    // Check if the plant thrives in a given environment
    thrives(environment) {
        // Plant thrives if all its preferred conditions are met
        return (this.preferredEnvironment & environment) === this.preferredEnvironment;
    }

    // Check if the plant tolerates a given environment
    tolerates(environment) {
        // Plant tolerates if any of its tolerated conditions are met
        return (this.toleratedEnvironment & environment) > 0;
    }

    // Get relationship with another plant
    getRelationship(otherPlantId) {
        return this.relationships[otherPlantId] || RELATIONSHIP.NEUTRAL;
    }
}

class PlantDatabase {
    constructor() {
        this.plants = {};
        this.initializePlants();
    }

    initializePlants() {
        // Create plant instances
        // Tomato
        const tomato = new Plant(
            'tomato',
            'Tomato',
            'https://cdn-icons-png.flaticon.com/512/1412/1412511.png',
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST,
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST | ENVIRONMENT.PARTIAL_SHADE
        );
        
        // Carrot
        const carrot = new Plant(
            'carrot',
            'Carrot',
            'https://cdn-icons-png.flaticon.com/512/2909/2909844.png',
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST,
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST | ENVIRONMENT.PARTIAL_SHADE | ENVIRONMENT.DRY
        );
        
        // Potato
        const potato = new Plant(
            'potato',
            'Potato',
            'https://cdn-icons-png.flaticon.com/512/1135/1135449.png',
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST,
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST | ENVIRONMENT.PARTIAL_SHADE
        );
        
        // Lettuce
        const lettuce = new Plant(
            'lettuce',
            'Lettuce',
            'https://cdn-icons-png.flaticon.com/512/1514/1514951.png',
            ENVIRONMENT.PARTIAL_SHADE | ENVIRONMENT.MOIST,
            ENVIRONMENT.PARTIAL_SHADE | ENVIRONMENT.MOIST | ENVIRONMENT.WET | ENVIRONMENT.SUNNY
        );
        
        // Basil
        const basil = new Plant(
            'basil',
            'Basil',
            'https://cdn-icons-png.flaticon.com/512/2713/2713487.png',
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST,
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST | ENVIRONMENT.PARTIAL_SHADE
        );
        
        // Corn
        const corn = new Plant(
            'corn',
            'Corn',
            'https://cdn-icons-png.flaticon.com/512/2909/2909926.png',
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST,
            ENVIRONMENT.SUNNY | ENVIRONMENT.MOIST | ENVIRONMENT.DRY
        );

        // Define relationships between plants
        // Tomato relationships
        tomato.addRelationship('carrot', RELATIONSHIP.LIKES);
        tomato.addRelationship('basil', RELATIONSHIP.LIKES);
        tomato.addRelationship('potato', RELATIONSHIP.HATES);
        tomato.addRelationship('corn', RELATIONSHIP.HATES);
        
        // Carrot relationships
        carrot.addRelationship('tomato', RELATIONSHIP.LIKES);
        carrot.addRelationship('lettuce', RELATIONSHIP.LIKES);
        carrot.addRelationship('potato', RELATIONSHIP.NEUTRAL);
        
        // Potato relationships
        potato.addRelationship('tomato', RELATIONSHIP.HATES);
        potato.addRelationship('corn', RELATIONSHIP.LIKES);
        potato.addRelationship('basil', RELATIONSHIP.NEUTRAL);
        
        // Lettuce relationships
        lettuce.addRelationship('carrot', RELATIONSHIP.LIKES);
        lettuce.addRelationship('basil', RELATIONSHIP.LIKES);
        lettuce.addRelationship('corn', RELATIONSHIP.NEUTRAL);
        
        // Basil relationships
        basil.addRelationship('tomato', RELATIONSHIP.LIKES);
        basil.addRelationship('lettuce', RELATIONSHIP.LIKES);
        basil.addRelationship('potato', RELATIONSHIP.NEUTRAL);
        
        // Corn relationships
        corn.addRelationship('potato', RELATIONSHIP.LIKES);
        corn.addRelationship('tomato', RELATIONSHIP.HATES);
        corn.addRelationship('basil', RELATIONSHIP.NEUTRAL);
        
        // Add plants to the database
        this.plants = {
            tomato,
            carrot,
            potato,
            lettuce,
            basil,
            corn
        };
    }

    // Get a plant by its ID
    getPlant(plantId) {
        return this.plants[plantId];
    }

    // Get all plants
    getAllPlants() {
        return Object.values(this.plants);
    }

    // Check if a plant thrives in a specific environment
    plantThrives(plantId, environment) {
        const plant = this.getPlant(plantId);
        return plant ? plant.thrives(environment) : false;
    }

    // Check if a plant tolerates a specific environment
    plantTolerates(plantId, environment) {
        const plant = this.getPlant(plantId);
        return plant ? plant.tolerates(environment) : false;
    }

    // Get the relationship between two plants
    getRelationship(plant1Id, plant2Id) {
        const plant1 = this.getPlant(plant1Id);
        return plant1 ? plant1.getRelationship(plant2Id) : RELATIONSHIP.NEUTRAL;
    }
}

// Create and expose a global instance
const plantDatabase = new PlantDatabase();