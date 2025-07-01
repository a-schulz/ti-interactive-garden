import { Plant } from './Plant';
import { ENVIRONMENT, RELATIONSHIP } from './constants';

/**
 * PlantDatabase manages all plant types and their relationships
 */
class PlantDatabase {
  constructor() {
    this.plants = {};
    this.initializePlants();
  }

  // Initialize the plant database with predefined plant types
  initializePlants() {
    // Create plant instances
    // Tomato
    const tomato = new Plant(
      'tomato',
      'Tomato',
      '/images/tomato.png', // Will add images to public/images folder
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.WET,
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.WET | ENVIRONMENT.MOIST
    );
    
    // Carrot
    const carrot = new Plant(
      'carrot',
      'Carrot',
      '/images/carrot.png',
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.MOIST,
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.MOIST | ENVIRONMENT.DRY
    );
    
    // Potato
    const potato = new Plant(
      'potato',
      'Potato',
      '/images/potato.png',
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.DRY,
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.DRY | ENVIRONMENT.MOIST
    );
    
    // Onion
    const onion = new Plant(
      'onion',
      'Onion',
      '/images/onion.png',
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.DRY,
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.DRY | ENVIRONMENT.MOIST
    );
    
    // Lettuce
    const lettuce = new Plant(
      'lettuce',
      'Lettuce',
      '/images/lettuce.png',
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.WET,
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.WET | ENVIRONMENT.MOIST
    );
    
    // Cucumber
    const cucumber = new Plant(
      'cucumber',
      'Cucumber',
      '/images/cucumber.png',
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.WET,
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.WET | ENVIRONMENT.MOIST
    );
    
    // Pea
    const pea = new Plant(
      'pea',
      'Pea',
      '/images/pea.png',
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.MOIST,
      ENVIRONMENT.PARTIALLY_SHADED | ENVIRONMENT.MOIST | ENVIRONMENT.WET
    );

    // Define relationships between plants (based on the hardware implementation)
    // Tomato relationships
    tomato.addRelationship('carrot', RELATIONSHIP.HATES);
    tomato.addRelationship('onion', RELATIONSHIP.LIKES);
    tomato.addRelationship('potato', RELATIONSHIP.HATES);
    tomato.addRelationship('lettuce', RELATIONSHIP.LIKES);
    tomato.addRelationship('cucumber', RELATIONSHIP.HATES);
    tomato.addRelationship('pea', RELATIONSHIP.HATES);
    
    // Carrot relationships
    carrot.addRelationship('tomato', RELATIONSHIP.HATES);
    carrot.addRelationship('onion', RELATIONSHIP.LIKES);
    carrot.addRelationship('potato', RELATIONSHIP.HATES);
    carrot.addRelationship('lettuce', RELATIONSHIP.LIKES);
    carrot.addRelationship('cucumber', RELATIONSHIP.LIKES);
    carrot.addRelationship('pea', RELATIONSHIP.LIKES);
    
    // Potato relationships
    potato.addRelationship('tomato', RELATIONSHIP.HATES);
    potato.addRelationship('onion', RELATIONSHIP.HATES);
    potato.addRelationship('carrot', RELATIONSHIP.HATES);
    potato.addRelationship('lettuce', RELATIONSHIP.LIKES);
    potato.addRelationship('cucumber', RELATIONSHIP.HATES);
    potato.addRelationship('pea', RELATIONSHIP.LIKES);
    
    // Onion relationships
    onion.addRelationship('tomato', RELATIONSHIP.LIKES);
    onion.addRelationship('carrot', RELATIONSHIP.LIKES);
    onion.addRelationship('potato', RELATIONSHIP.HATES);
    onion.addRelationship('lettuce', RELATIONSHIP.LIKES);
    onion.addRelationship('cucumber', RELATIONSHIP.HATES);
    onion.addRelationship('pea', RELATIONSHIP.HATES);
    
    // Lettuce relationships
    lettuce.addRelationship('tomato', RELATIONSHIP.LIKES);
    lettuce.addRelationship('carrot', RELATIONSHIP.LIKES);
    lettuce.addRelationship('potato', RELATIONSHIP.LIKES);
    lettuce.addRelationship('onion', RELATIONSHIP.LIKES);
    lettuce.addRelationship('cucumber', RELATIONSHIP.LIKES);
    lettuce.addRelationship('pea', RELATIONSHIP.LIKES);
    
    // Cucumber relationships
    cucumber.addRelationship('tomato', RELATIONSHIP.HATES);
    cucumber.addRelationship('carrot', RELATIONSHIP.LIKES);
    cucumber.addRelationship('potato', RELATIONSHIP.HATES);
    cucumber.addRelationship('onion', RELATIONSHIP.HATES);
    cucumber.addRelationship('lettuce', RELATIONSHIP.LIKES);
    cucumber.addRelationship('pea', RELATIONSHIP.LIKES);
    
    // Pea relationships
    pea.addRelationship('tomato', RELATIONSHIP.HATES);
    pea.addRelationship('carrot', RELATIONSHIP.LIKES);
    pea.addRelationship('potato', RELATIONSHIP.LIKES);
    pea.addRelationship('onion', RELATIONSHIP.HATES);
    pea.addRelationship('lettuce', RELATIONSHIP.LIKES);
    pea.addRelationship('cucumber', RELATIONSHIP.LIKES);
    
    // Add plants to the database
    this.plants = {
      tomato,
      carrot,
      potato,
      onion,
      lettuce,
      cucumber,
      pea
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

// Create and export a singleton instance
export const plantDatabase = new PlantDatabase();