import { RELATIONSHIP } from './constants';

/**
 * Plant class representing a plant type in the interactive garden
 */
export class Plant {
  constructor(id, name, imagePath, preferredEnvironment, toleratedEnvironment) {
    this.id = id;
    this.name = name;
    this.imagePath = imagePath;
    this.preferredEnvironment = preferredEnvironment;
    this.toleratedEnvironment = toleratedEnvironment;
    this.relationships = {}; // Will hold relationships with other plants
    this.color = this.getPlantColor(id); // For visual representation
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

  // Determine a color for the plant based on its ID for visual consistency
  getPlantColor(id) {
    const colors = {
      'tomato': '#e53935',      // Red
      'carrot': '#fb8c00',      // Orange
      'potato': '#8d6e63',      // Brown
      'lettuce': '#7cb342',     // Green
      'basil': '#43a047',       // Dark green
      'corn': '#fdd835',        // Yellow
      'cucumber': '#66bb6a',    // Light green
      'pea': '#c5e1a5',         // Very light green
      'onion': '#e0e0e0',       // Light grey
    };

    return colors[id] || '#9c27b0'; // Default purple if ID not found
  }
}